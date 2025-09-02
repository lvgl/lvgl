/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef LV_BUILD_FROGFS

#include "../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include "../../../stdlib/lv_string.h"

#include "esp_err.h"
#include "esp_vfs.h"

#include "frogfs_config.h"
#include "frogfs/vfs.h"
#include "frogfs/frogfs.h"


#ifndef CONFIG_FROGFS_MAX_PARTITIONS
# define CONFIG_FROGFS_MAX_PARTITIONS 1
#endif

#if defined(CONFIG_FROGFS_VFS_SUPPORT_DIR)
typedef struct {
    DIR dir;
    frogfs_dh_t *dh;
    struct dirent dirent;
} frogfs_vfs_dh_t;
#endif

typedef struct {
    frogfs_fs_t *fs;
    char base_path[ESP_VFS_PATH_MAX + 1];
    size_t fh_len;
    frogfs_fh_t *fh[];
} frogfs_vfs_t;

static frogfs_vfs_t *s_frogfs_vfs[CONFIG_FROGFS_MAX_PARTITIONS];

static esp_err_t frogfs_get_empty(int *index)
{
    int i;

    for (i = 0; i < CONFIG_FROGFS_MAX_PARTITIONS; i++) {
        if (s_frogfs_vfs[i] == NULL) {
            *index = i;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

static off_t frogfs_vfs_lseek(void *ctx, int fd, off_t offset, int mode)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    if (fd < 0 || fd >= vfs->fh_len) {
        return -1;
    }

    return frogfs_seek(vfs->fh[fd], offset, mode);
}

static ssize_t frogfs_vfs_read(void *ctx, int fd, void *data, size_t size)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    if (fd < 0 || fd >= vfs->fh_len) {
        return -1;
    }

    return frogfs_read(vfs->fh[fd], data, size);
}

static int frogfs_vfs_open(void *ctx, const char *path, int flags, int mode)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    int fd;
    for (fd = 0; fd < vfs->fh_len; fd++) {
        if (vfs->fh[fd] == NULL) {
            break;
        }
    }
    if (fd >= vfs->fh_len) {
        return -1;
    }

    if (((flags & O_ACCMODE) == O_WRONLY) | ((flags & O_ACCMODE) == O_RDWR)) {
        return -1;
    }
    if (flags & (O_APPEND | O_CREAT | O_TRUNC)) {
        return -1;
    }

    const frogfs_entry_t *entry = frogfs_get_entry(vfs->fs, path);
    if (entry == NULL) {
        return -1;
    }

    vfs->fh[fd] = frogfs_open(vfs->fs, entry, 0);
    if (vfs->fh[fd] != NULL) {
        return fd;
    }

    return -1;
}

static int frogfs_vfs_close(void *ctx, int fd)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    if (fd < 0 || fd >= vfs->fh_len) {
        return -1;
    }

    if (vfs->fh[fd] == NULL) {
        return -1;
    }

    frogfs_close(vfs->fh[fd]);
    vfs->fh[fd] = NULL;
    return 0;
}

static int frogfs_vfs_fstat(void *ctx, int fd, struct stat *st)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    if (fd < 0 || fd >= vfs->fh_len) {
        return -1;
    }

    if (vfs->fh[fd] == NULL) {
        return -1;
    }

    frogfs_fh_t *fh = vfs->fh[fd];
    lv_memset(st, 0, sizeof(*st));
    st->st_mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
            S_IXOTH | S_IFREG;

    frogfs_stat_t fst;
    frogfs_stat(vfs->fs, fh->entry, &fst);
    st->st_size = frogfs_is_raw(fh) ? fst.compressed_sz : fst.size;
    st->st_spare4[0] = FROGFS_MAGIC;
    st->st_spare4[1] = fst.compression;
    return 0;
}

#if defined(CONFIG_FROGFS_VFS_SUPPORT_DIR)
static int frogfs_vfs_stat(void *ctx, const char *path, struct stat *st)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    const frogfs_entry_t *entry = frogfs_get_entry(vfs->fs, path);
    if (entry == NULL) {
        return -1;
    }

    frogfs_stat_t fst;
    frogfs_stat(vfs->fs, entry, &fst);
    lv_memset(st, 0, sizeof(*st));
    st->st_mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    st->st_mode |= (fst.type == FROGFS_ENTRY_TYPE_FILE) ? S_IFREG : S_IFDIR;
    st->st_size = fst.size;
    st->st_spare4[0] = FROGFS_MAGIC;
    st->st_spare4[1] = fst.compression;
    return 0;
}

static DIR* frogfs_vfs_opendir(void *ctx, const char *path)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;
    frogfs_vfs_dh_t *dh = lv_malloc(sizeof(*dh));

    const frogfs_entry_t *entry = frogfs_get_entry(vfs->fs, path);
    if (entry == NULL) {
        return NULL;
    }

    dh->dh = frogfs_opendir(vfs->fs, entry);
    return (DIR *) dh;
}

static int frogfs_vfs_readdir_r(void *ctx, DIR *pdir, struct dirent *entry,
        struct dirent **out_ent);
static struct dirent *frogfs_vfs_readdir(void *ctx, DIR *pdir)
{
    frogfs_vfs_dh_t *dh = (frogfs_vfs_dh_t *) pdir;
    struct dirent *out_ent;

    int err = frogfs_vfs_readdir_r(ctx, pdir, &dh->dirent, &out_ent);
    if (err != 0) {
        return NULL;
    }

    return out_ent;
}

static int frogfs_vfs_readdir_r(void *ctx, DIR *pdir, struct dirent *ent,
        struct dirent **out_ent)
{
    frogfs_vfs_dh_t *dh = (frogfs_vfs_dh_t *) pdir;

    const frogfs_entry_t *entry = frogfs_readdir(dh->dh);
    if (entry == NULL) {
        *out_ent = NULL;
        return 0;
    }

    ent->d_ino = frogfs_telldir(dh->dh);
    const char *name = frogfs_get_name(entry);
    lv_strlcpy(ent->d_name, name, sizeof(ent->d_name));
    lv_free((void *) name);
    ent->d_type = DT_UNKNOWN;
    if (frogfs_is_dir(entry)) {
        ent->d_type = DT_DIR;
    } else if (frogfs_is_file(entry)) {
        ent->d_type = DT_REG;
    }
    *out_ent = ent;
    return 0;
}

static long frogfs_vfs_telldir(void *ctx, DIR *pdir)
{
    frogfs_vfs_dh_t *dh = (frogfs_vfs_dh_t *) pdir;

    return frogfs_telldir(dh->dh);
}

static void frogfs_vfs_seekdir(void *ctx, DIR *pdir, long offset)
{
    frogfs_vfs_dh_t *dh = (frogfs_vfs_dh_t *) pdir;

    frogfs_seekdir(dh->dh, offset);
}

static int frogfs_vfs_closedir(void *ctx, DIR *pdir)
{
    frogfs_vfs_dh_t *dh = (frogfs_vfs_dh_t *) pdir;

    frogfs_closedir(dh->dh);
    lv_free(dh);

    return 0;
}
#endif

static int frogfs_vfs_fcntl(void *ctx, int fd, int cmd, int arg)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    if (fd < 0 || fd >= vfs->fh_len) {
        return -1;
    }

    if (cmd == F_REOPEN_RAW) {
        const frogfs_entry_t *entry = vfs->fh[fd]->entry;
        frogfs_close(vfs->fh[fd]);
        vfs->fh[fd] = frogfs_open(vfs->fs, entry, FROGFS_OPEN_RAW);
        return 0;
    }

    return -1;
}

#if defined(CONFIG_FROGFS_VFS_SUPPORT_DIR)
static int frogfs_vfs_access(void *ctx, const char *path, int amode)
{
    frogfs_vfs_t *vfs = (frogfs_vfs_t *) ctx;

    const frogfs_entry_t *entry = frogfs_get_entry(vfs->fs, path);
    if (entry == NULL) {
        errno = ENOENT;
        return -1;
    }

    if (amode & (W_OK | X_OK)) {
        errno = EACCES;
        return -1;
    }

    return -1;
}
#endif

esp_err_t frogfs_vfs_register(const frogfs_vfs_conf_t *conf)
{
    LV_ASSERT_NULL(conf);
    LV_ASSERT_NULL(conf->fs);
    LV_ASSERT_NULL(conf->base_path);

    const esp_vfs_t funcs = {
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .lseek_p = &frogfs_vfs_lseek,
        .read_p = &frogfs_vfs_read,
        .open_p = &frogfs_vfs_open,
        .close_p = &frogfs_vfs_close,
        .fstat_p = &frogfs_vfs_fstat,
#if defined(CONFIG_FROGFS_VFS_SUPPORT_DIR)
        .stat_p = &frogfs_vfs_stat,
        .opendir_p = &frogfs_vfs_opendir,
        .readdir_p = &frogfs_vfs_readdir,
        .readdir_r_p = &frogfs_vfs_readdir_r,
        .telldir_p = &frogfs_vfs_telldir,
        .seekdir_p = &frogfs_vfs_seekdir,
        .closedir_p = &frogfs_vfs_closedir,
#endif
        .fcntl_p = &frogfs_vfs_fcntl,
#if defined(CONFIG_FROGFS_VFS_SUPPORT_DIR)
        .access_p = &frogfs_vfs_access,
#endif
    };

    int index;
    if (frogfs_get_empty(&index) != ESP_OK) {
        return ESP_ERR_INVALID_STATE;
    }

    frogfs_vfs_t *vfs = lv_calloc(1, sizeof(*vfs) +
            (sizeof(frogfs_fh_t *) * conf->max_files));
    if (vfs == NULL) {
        return ESP_ERR_NO_MEM;
    }

    vfs->fs = conf->fs;
    lv_strlcpy(vfs->base_path, conf->base_path, sizeof(vfs->base_path));
    vfs->fh_len = conf->max_files;

    esp_err_t err = esp_vfs_register(vfs->base_path, &funcs, vfs);
    if (err != ESP_OK) {
        lv_free(vfs);
        return err;
    }

    s_frogfs_vfs[index] = vfs;
    return ESP_OK;
}

esp_err_t frogfs_vfs_deregister(const char *base_path)
{
    LV_ASSERT_NULL(base_path);

    for (int i = 0; i < CONFIG_FROGFS_MAX_PARTITIONS; i++) {
        if (s_frogfs_vfs[i] == NULL) {
            continue;
        }

        if (lv_strcmp(s_frogfs_vfs[i]->base_path, base_path) == 0) {
            esp_err_t err = esp_vfs_unregister(base_path);
            if (err != ESP_OK) {
                return err;
            }
            lv_free(s_frogfs_vfs[i]);
            s_frogfs_vfs[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

#endif /*LV_BUILD_FROGFS*/

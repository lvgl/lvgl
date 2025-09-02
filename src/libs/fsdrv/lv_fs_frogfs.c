/**
 * @file lv_fs_frogfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_FS_FROGFS

#include "frogfs/frogfs.h"

#include "../../core/lv_global.h"

#if !LV_FS_IS_VALID_LETTER(LV_FS_FROGFS_LETTER)
    #error "Invalid drive letter"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define frogfs_fs_drv (&(LV_GLOBAL_DEFAULT()->frogfs_fs_drv))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_frogfs_init(void)
{
    extern const uint8_t frogfs_bin[];

    frogfs_config_t frogfs_config = {
        .addr = frogfs_bin,
    };

    frogfs_fs_t * fs = frogfs_init(&frogfs_config);
    if(fs == NULL) {
        LV_LOG_ERROR("Could not initialize frogfs with bin 0x%p", frogfs_bin);
        return;
    }


    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    lv_fs_drv_init(fs_drv_p);

    fs_drv_p->letter = LV_FS_FROGFS_LETTER;

    fs_drv_p->open_cb = fs_open;
    fs_drv_p->close_cb = fs_close;
    fs_drv_p->read_cb = fs_read;
    fs_drv_p->seek_cb = fs_seek;
    fs_drv_p->tell_cb = fs_tell;

    fs_drv_p->dir_close_cb = fs_dir_close;
    fs_drv_p->dir_open_cb = fs_dir_open;
    fs_drv_p->dir_read_cb = fs_dir_read;

    fs_drv_p->user_data = fs;

    lv_fs_drv_register(fs_drv_p);
}

void lv_fs_frogfs_deinit(void)
{
    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    frogfs_fs_t * fs = fs_drv_p->user_data;
    frogfs_deinit(fs);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    if(mode & LV_FS_MODE_WR) {
        LV_LOG_WARN("Cannot open files for writing with frogfs");
        return NULL;
    }

    frogfs_fs_t * fs = drv->user_data;

    const frogfs_entry_t * entry = frogfs_get_entry(fs, path);
    if(entry == NULL) {
        LV_LOG_WARN("No entry '%s' in frogfs", path);
        return NULL;
    }

    if(frogfs_is_dir(entry)) {
        LV_LOG_WARN("Cannot open directory as file with frogfs");
        return NULL;
    }

    frogfs_fh_t * fh = frogfs_open(fs, entry, 0);
    if(fh == NULL) {
        LV_LOG_WARN("Could not open '%s' even though the entry exists in frogfs", path);
        return NULL;
    }

    return fh;
}

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    frogfs_close(file_p);
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);

    ssize_t res = frogfs_read(file_p, buf, btr);
    if(res < 0) {
        LV_LOG_WARN("Error reading frogfs file");
        *br = 0;
        return LV_FS_RES_UNKNOWN;
    }

    *br = res;
    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    int w;
    switch(whence) {
        case LV_FS_SEEK_SET:
            w = SEEK_SET;
            break;
        case LV_FS_SEEK_CUR:
            w = SEEK_CUR;
            break;
        case LV_FS_SEEK_END:
            w = SEEK_END;
            break;
        default:
            return LV_FS_RES_INV_PARAM;
    }

    ssize_t res = frogfs_seek(file_p, pos, w);

    if(res < 0) {
        LV_LOG_WARN("Error `seek`ing frogfs file");
        return LV_FS_RES_UNKNOWN;
    }

    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);

    size_t res = frogfs_tell(file_p);
    if(res == (size_t) -1) {
        LV_LOG_WARN("Error `tell`ing frogfs file");
        return LV_FS_RES_UNKNOWN;
    }

    *pos_p = res;
    return LV_FS_RES_OK;
}

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    frogfs_fs_t * fs = drv->user_data;

    const frogfs_entry_t * entry = frogfs_get_entry(fs, path);
    if(entry == NULL) {
        LV_LOG_WARN("No entry '%s' in frogfs", path);
        return NULL;
    }

    if(!frogfs_is_dir(entry)) {
        LV_LOG_WARN("Cannot open non-directory as directory with frogfs");
        return NULL;
    }

    frogfs_dh_t * dh = frogfs_opendir(fs, entry);
    if(dh == NULL) {
        LV_LOG_WARN("Could not open directory '%s' even though the entry exists in frogfs", path);
        return NULL;
    }

    return dh;
}

static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    if(fn_len == 0) return LV_FS_RES_INV_PARAM;

    const frogfs_entry_t * entry = frogfs_readdir(dir_p);
    if(entry == NULL) {
        fn[0] = '\0';
        return LV_FS_RES_OK;
    }

    char * name = frogfs_get_name(entry);

    if(frogfs_is_dir(entry)) {
        lv_snprintf(fn, fn_len, "/%s", name);
    }
    else {
        lv_strlcpy(fn, name, fn_len);
    }

    lv_free(name); /* frogfs `malloc`d it */

    return LV_FS_RES_OK;
}

static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    frogfs_closedir(dir_p);
    return LV_FS_RES_OK;
}

#endif /*LV_USE_FS_FROGFS*/

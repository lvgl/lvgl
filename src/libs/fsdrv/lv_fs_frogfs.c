/**
 * @file lv_fs_frogfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_FS_FROGFS

#include "../frogfs/include/frogfs/frogfs.h"
#include "../../core/lv_global.h"
#include "../../misc/lv_ll.h"

#if !LV_FS_IS_VALID_LETTER(LV_FS_FROGFS_LETTER)
    #error "Invalid drive letter"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_ll_t blob_ll;
} fs_drv_data_t;

typedef struct {
    char * path_prefix;
    frogfs_fs_t * blob_fs;
} blob_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool get_blob_and_entry(const char * path, blob_t ** blob_dst, const frogfs_entry_t ** entry_dst);
static void destroy_blob(blob_t * blob);
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
    fs_drv_data_t * data = lv_malloc(sizeof(*data));
    LV_ASSERT_MALLOC(data);
    lv_ll_init(&data->blob_ll, sizeof(blob_t));

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

    fs_drv_p->user_data = data;

    lv_fs_drv_register(fs_drv_p);
}

void lv_fs_frogfs_deinit(void)
{
    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    fs_drv_data_t * data = fs_drv_p->user_data;

    lv_ll_clear_custom(&data->blob_ll, (void (*)(void *)) destroy_blob);

    lv_free(data);
}

lv_result_t lv_fs_frogfs_register_blob(const void * blob, const char * path_prefix)
{
    if(path_prefix[0] == '\0') {
        LV_LOG_WARN("path prefix should not be zero-length");
        return LV_RESULT_INVALID;
    }

    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    fs_drv_data_t * data = fs_drv_p->user_data;

    frogfs_config_t frogfs_config = {
        .addr = blob,
    };

    frogfs_fs_t * blob_fs = frogfs_init(&frogfs_config);
    if(blob_fs == NULL) {
        LV_LOG_WARN("Could not register frogfs blob 0x%p", blob);
        return LV_RESULT_INVALID;
    }

    blob_t * hdl = lv_ll_ins_head(&data->blob_ll);
    LV_ASSERT_MALLOC(hdl);

    hdl->path_prefix = lv_strdup(path_prefix);
    LV_ASSERT_MALLOC(hdl->path_prefix);

    hdl->blob_fs = blob_fs;

    return LV_RESULT_OK;
}

void lv_fs_frogfs_unregister_blob(const char * path_prefix)
{
    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    fs_drv_data_t * data = fs_drv_p->user_data;

    blob_t * blob_handle;
    LV_LL_READ(&data->blob_ll, blob_handle) {
        if(lv_streq(path_prefix, blob_handle->path_prefix)) {
            break;
        }
    }
    if(blob_handle == NULL) {
        LV_LOG_WARN("No frogfs blob with path prefix '%s' to unregister", path_prefix);
        return;
    }

    destroy_blob(blob_handle);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool get_blob_and_entry(const char * path, blob_t ** blob_dst, const frogfs_entry_t ** entry_dst)
{
    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    fs_drv_data_t * data = fs_drv_p->user_data;

    blob_t * blob;
    size_t path_prefix_length;
    LV_LL_READ(&data->blob_ll, blob) {
        path_prefix_length = lv_strlen(blob->path_prefix);
        if(0 == lv_strncmp(path, blob->path_prefix, path_prefix_length)
           && (blob->path_prefix[path_prefix_length - 1] == '/'
               || blob->path_prefix[path_prefix_length - 1] == '\\'
               || path[path_prefix_length] == '\0'
               || path[path_prefix_length] == '/'
               || path[path_prefix_length] == '\\')) {
            break;
        }
    }
    if(blob == NULL) {
        LV_LOG_WARN("Path '%s' does not have a prefix that matches any of the registered frogfs blobs", path);
        return false;
    }

    path += path_prefix_length;
    if(path[0] == '/' || path[0] == '\\') path++;

    const frogfs_entry_t * entry = frogfs_get_entry(blob->blob_fs, path);
    if(entry == NULL) {
        LV_LOG_WARN("No entry '%s' in frogfs blob registered under '%s'", path, blob->path_prefix);
        return false;
    }

    *blob_dst = blob;
    *entry_dst = entry;
    return true;
}

static void destroy_blob(blob_t * blob)
{
    lv_fs_drv_t * fs_drv_p = frogfs_fs_drv;
    fs_drv_data_t * data = fs_drv_p->user_data;

    lv_free(blob->path_prefix);
    frogfs_deinit(blob->blob_fs);

    lv_ll_remove(&data->blob_ll, blob);
    lv_free(blob);
}

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);

    if(mode & LV_FS_MODE_WR) {
        LV_LOG_WARN("Cannot open files for writing with frogfs");
        return NULL;
    }

    blob_t * blob;
    const frogfs_entry_t * entry;
    if(!get_blob_and_entry(path, &blob, &entry)) {
        return NULL;
    }

    if(frogfs_is_dir(entry)) {
        LV_LOG_WARN("Cannot open directory as file with frogfs");
        return NULL;
    }

    frogfs_fh_t * fh = frogfs_open(blob->blob_fs, entry, 0);
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

    ssize_t res = frogfs_seek(file_p, pos, whence);

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
    LV_UNUSED(drv);

    blob_t * blob;
    const frogfs_entry_t * entry;
    if(!get_blob_and_entry(path, &blob, &entry)) {
        return NULL;
    }

    if(!frogfs_is_dir(entry)) {
        LV_LOG_WARN("Cannot open non-directory as directory with frogfs");
        return NULL;
    }

    frogfs_dh_t * dh = frogfs_opendir(blob->blob_fs, entry);
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

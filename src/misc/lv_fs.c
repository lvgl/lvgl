/**
 * @file lv_fs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs.h"

#include <string.h>
#include "lv_assert.h"
#include "lv_ll.h"
#include "lv_gc.h"

/*********************
 *      DEFINES
 *********************/

/**
 * "free" is used as a function pointer (in lv_fs_drv_t).
 * We must make sure "free" was not defined to a platform specific
 * free function, otherwise compilation would fail.
 */
#ifdef free
    #undef free
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static const char * lv_fs_get_real_path(const char * path);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_fs_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_fsdrv_ll), sizeof(lv_fs_drv_t *));
}

bool lv_fs_is_ready(char letter)
{
    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(drv == NULL) return false; /*An unknown driver in not ready*/

    if(drv->ready_cb == NULL) return true; /*Assume the driver is always ready if no handler provided*/

    return drv->ready_cb(drv);
}

lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode)
{
    if(path == NULL) {
        LV_LOG_WARN("Can't open file: path is NULL");
        return LV_FS_RES_INV_PARAM;
    }

    char letter = path[0];
    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(drv == NULL) {
        LV_LOG_WARN("Can't open file (%s): unknown driver letter", path);
        return LV_FS_RES_NOT_EX;
    }

    if(drv->ready_cb) {
        if(drv->ready_cb(drv) == false) {
            LV_LOG_WARN("Can't open file (%s): driver not ready", path);
            return LV_FS_RES_HW_ERR;
        }
    }

    if(drv->open_cb == NULL) {
        LV_LOG_WARN("Can't open file (%s): open function not exists", path);
        return LV_FS_RES_NOT_IMP;
    }

    const char * real_path = lv_fs_get_real_path(path);
    void * file_d = drv->open_cb(drv, real_path, mode);

    if(file_d == NULL || file_d == (void *)(-1)) {
        return LV_FS_RES_UNKNOWN;
    }

    file_p->drv = drv;
    file_p->file_d = file_d;

    return LV_FS_RES_OK;
}

lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->close_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = file_p->drv->close_cb(file_p->drv, file_p->file_d);

    file_p->file_d = NULL;
    file_p->drv    = NULL;

    return res;
}

lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    if(br != NULL) *br = 0;
    if(file_p->drv == NULL) return LV_FS_RES_INV_PARAM;
    if(file_p->drv->read_cb == NULL) return LV_FS_RES_NOT_IMP;

    uint32_t br_tmp = 0;
    lv_fs_res_t res = file_p->drv->read_cb(file_p->drv, file_p->file_d, buf, btr, &br_tmp);
    if(br != NULL) *br = br_tmp;

    return res;
}

lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    if(bw != NULL) *bw = 0;

    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->write_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    uint32_t bw_tmp = 0;
    lv_fs_res_t res = file_p->drv->write_cb(file_p->drv, file_p->file_d, buf, btw, &bw_tmp);
    if(bw != NULL) *bw = bw_tmp;

    return res;
}

lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->seek_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    return file_p->drv->seek_cb(file_p->drv, file_p->file_d, pos, whence);
}

lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t * pos)
{
    *pos = 0;

    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->tell_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    return file_p->drv->tell_cb(file_p->drv, file_p->file_d, pos);
}

lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path)
{
    if(path == NULL) return LV_FS_RES_INV_PARAM;

    char letter = path[0];
    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(drv == NULL) {
        return LV_FS_RES_NOT_EX;
    }

    if(drv->ready_cb) {
        if(drv->ready_cb(drv) == false) {
            return LV_FS_RES_HW_ERR;
        }
    }

    if(drv->dir_open_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    const char * real_path = lv_fs_get_real_path(path);
    void * dir_d = drv->dir_open_cb(drv, real_path);

    if(dir_d == NULL || dir_d == (void *)(-1)) {
        return LV_FS_RES_UNKNOWN;
    }

    rddir_p->drv = drv;
    rddir_p->dir_d = dir_d;

    return LV_FS_RES_OK;
}

lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn)
{
    fn[0] = '\0';

    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_read_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    return rddir_p->drv->dir_read_cb(rddir_p->drv, rddir_p->dir_d, fn);
}

lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_close_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    lv_fs_res_t res = rddir_p->drv->dir_close_cb(rddir_p->drv, rddir_p->dir_d);

    rddir_p->dir_d = NULL;
    rddir_p->drv   = NULL;

    return res;
}

void lv_fs_drv_init(lv_fs_drv_t * drv)
{
    lv_memset_00(drv, sizeof(lv_fs_drv_t));
}

void lv_fs_drv_register(lv_fs_drv_t * drv_p)
{
    /*Save the new driver*/
    lv_fs_drv_t ** new_drv;
    new_drv = _lv_ll_ins_head(&LV_GC_ROOT(_lv_fsdrv_ll));
    LV_ASSERT_MALLOC(new_drv);
    if(new_drv == NULL) return;

    *new_drv = drv_p;
}

lv_fs_drv_t * lv_fs_get_drv(char letter)
{
    lv_fs_drv_t ** drv;

    _LV_LL_READ(&LV_GC_ROOT(_lv_fsdrv_ll), drv) {
        if((*drv)->letter == letter) {
            return *drv;
        }
    }

    return NULL;
}

char * lv_fs_get_letters(char * buf)
{
    lv_fs_drv_t ** drv;
    uint8_t i = 0;

    _LV_LL_READ(&LV_GC_ROOT(_lv_fsdrv_ll), drv) {
        buf[i] = (*drv)->letter;
        i++;
    }

    buf[i] = '\0';

    return buf;
}

const char * lv_fs_get_ext(const char * fn)
{
    size_t i;
    for(i = strlen(fn); i > 0; i--) {
        if(fn[i - 1] == '.') {
            return &fn[i];
        }
        else if(fn[i - 1] == '/' || fn[i - 1] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name.*/
}

char * lv_fs_up(char * path)
{
    size_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        path[len] = '\0';
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i;
    for(i = len; i > 0; i--) {
        if(path[i - 1] == '/' || path[i - 1] == '\\') {
            path[i - 1] = '\0';
            break;
        }
    }

    return path;
}

const char * lv_fs_get_last(const char * path)
{
    size_t len = strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i;
    for(i = len; i > 0; i--) {
        if(path[i - 1] == '/' || path[i - 1] == '\\') break;
    }

    return &path[i];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Skip the driver letter and the possible : after the letter
 * @param path path string (E.g. S:/folder/file.txt)
 * @return pointer to the beginning of the real path (E.g. /folder/file.txt)
 */
static const char * lv_fs_get_real_path(const char * path)
{
    path++; /*Ignore the driver letter*/
    if(*path == ':') path++;

    return path;
}

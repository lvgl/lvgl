/**
 * @file lv_test_fs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_test_fs.h"

#if LV_USE_TEST

#include "../../misc/lv_assert.h"
#include "../../misc/lv_fs.h"
#include "../../stdlib/lv_mem.h"

/*********************
 *      DEFINES
 *********************/

#define LV_TEST_FS_LETTER     'T'
#define LV_TEST_FS_CACHE_SIZE (512)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p);
static bool fs_ready(lv_fs_drv_t * drv);

/**********************
 *  STATIC VARIABLES
 **********************/

static bool is_ready = true;
static lv_fs_drv_t fs_drv;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

void lv_test_fs_init(void)
{
    lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_TEST_FS_LETTER;
    fs_drv.cache_size = LV_TEST_FS_CACHE_SIZE;

    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    fs_drv.ready_cb = fs_ready;

    lv_fs_drv_register(&fs_drv);
}

void lv_test_fs_set_ready(bool ready)
{
    is_ready = ready;
}

void lv_test_fs_clear_open_cb(bool is_clear)
{
    fs_drv.open_cb = is_clear ? NULL : fs_open;
}

void lv_test_fs_clear_close_cb(bool is_clear)
{
    fs_drv.close_cb = is_clear ? NULL : fs_close;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    lv_fs_file_t * file_p = lv_malloc(sizeof(lv_fs_file_t));
    LV_ASSERT_MALLOC(file_p);
    if(file_p == NULL) {
        return NULL;
    }

    lv_fs_res_t res = lv_fs_open(file_p, path, mode);
    if(res != LV_FS_RES_OK) {
        lv_free(file_p);
        return NULL;
    }

    return file_p;
}

static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    lv_fs_res_t res = lv_fs_close(file_p);
    lv_free(file_p);
    return res;
}

static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    return lv_fs_read(file_p, buf, btr, br);
}

static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    return lv_fs_write(file_p, buf, btw, bw);
}

static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    return lv_fs_seek(file_p, pos, whence);
}

static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    return lv_fs_tell(file_p, pos_p);
}

static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    lv_fs_dir_t * rddir_p = lv_malloc(sizeof(lv_fs_dir_t));
    if(rddir_p == NULL) {
        return NULL;
    }

    lv_fs_res_t res = lv_fs_dir_open(rddir_p, path);
    if(res != LV_FS_RES_OK) {
        lv_free(rddir_p);
        return NULL;
    }

    return rddir_p;
}

static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * rddir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    return lv_fs_dir_read(rddir_p, fn, fn_len);
}

static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * rddir_p)
{
    LV_UNUSED(drv);
    lv_fs_res_t res = lv_fs_dir_close(rddir_p);
    lv_free(rddir_p);
    return res;
}

static bool fs_ready(lv_fs_drv_t * drv)
{
    LV_UNUSED(drv);
    return is_ready;
}

#endif/*LV_USE_TEST*/

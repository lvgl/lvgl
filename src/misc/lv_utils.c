/**
 * @file lv_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_utils.h"
#include "lv_fs.h"
#include "lv_types.h"
#include "cache/lv_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_utils_bsearch(const void * key, const void * base, size_t n, size_t size,
                        int (*cmp)(const void * pRef, const void * pElement))
{
    const char * middle;
    int32_t c;

    for(middle = base; n != 0;) {
        middle += (n / 2) * size;
        if((c = (*cmp)(key, middle)) > 0) {
            n    = (n / 2) - ((n & 1) == 0);
            base = (middle += size);
        }
        else if(c < 0) {
            n /= 2;
            middle = base;
        }
        else {
            return (char *)middle;
        }
    }
    return NULL;
}

lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path)
{
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_WR);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("create file %s failed", path);
        return LV_RESULT_INVALID;
    }

    /*Image content modified, invalidate image cache.*/
    lv_image_cache_drop(path);

    uint32_t bw;
    res = lv_fs_write(&file, &draw_buf->header, sizeof(draw_buf->header), &bw);
    if(res != LV_FS_RES_OK || bw != sizeof(draw_buf->header)) {
        LV_LOG_ERROR("write draw_buf->header failed");
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    res = lv_fs_write(&file, draw_buf->data, draw_buf->data_size, &bw);
    if(res != LV_FS_RES_OK || bw != draw_buf->data_size) {
        LV_LOG_ERROR("write draw_buf->data failed");
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    lv_fs_close(&file);
    LV_LOG_TRACE("saved draw_buf to %s", path);
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

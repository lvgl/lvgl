/**
 * @file lv_bin_encoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_image_encoder_private.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t bin_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                    const char * path, const lv_image_encoder_args_t * args);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_bin_encoder_init(void)
{
    lv_image_encoder_t * enc = lv_image_encoder_create();
    if(enc == NULL) return;
    lv_image_encoder_set_save_cb(enc, bin_encoder_save);
    lv_image_encoder_set_name(enc, "BIN");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Bin format encoder, accepts the `.bin` extension.
 * Writes lv_image_header_t + raw pixel data (including palette for indexed formats).
 */
static lv_result_t bin_encoder_save(lv_image_encoder_t * encoder, const lv_draw_buf_t * draw_buf,
                                    const char * path, const lv_image_encoder_args_t * args)
{
    LV_UNUSED(encoder);
    LV_UNUSED(args);

    if(lv_strcmp(lv_fs_get_ext(path), "bin") != 0) return LV_RESULT_INVALID;

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_WR | LV_FS_MODE_TRUNC);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("create file %s failed, res %d", path, (int)res);
        return LV_RESULT_INVALID;
    }

    uint32_t bw;
    res = lv_fs_write(&file, &draw_buf->header, sizeof(draw_buf->header), &bw);
    if(res != LV_FS_RES_OK || bw != sizeof(draw_buf->header)) {
        LV_LOG_ERROR("write header failed, res %d, bw %d", (int)res, (int)bw);
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    res = lv_fs_write(&file, draw_buf->data, draw_buf->data_size, &bw);
    if(res != LV_FS_RES_OK || bw != draw_buf->data_size) {
        LV_LOG_ERROR("write data failed, res %d, bw %d", (int)res, (int)bw);
        lv_fs_close(&file);
        return LV_RESULT_INVALID;
    }

    res = lv_fs_close(&file);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("close file %s failed, res %d", path, (int)res);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

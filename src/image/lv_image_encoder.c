/**
 * @file lv_image_encoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_image_encoder_private.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define encoder_ll_p &(LV_GLOBAL_DEFAULT()->img_encoder_ll)

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_image_encoder_t {
    lv_image_encoder_save_cb_t save_cb;
    const char * name;
    void * user_data;
};

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

void lv_image_encoder_init(void)
{
    lv_ll_init(encoder_ll_p, sizeof(lv_image_encoder_t));
}

void lv_image_encoder_deinit(void)
{
    lv_ll_clear(encoder_ll_p);
}

lv_image_encoder_t * lv_image_encoder_create(void)
{
    lv_image_encoder_t * encoder = lv_ll_ins_head(encoder_ll_p);
    LV_ASSERT_MALLOC(encoder);
    if(encoder == NULL) return NULL;

    lv_memzero(encoder, sizeof(lv_image_encoder_t));
    return encoder;
}

void lv_image_encoder_delete(lv_image_encoder_t * encoder)
{
    if(encoder == NULL) return;

    lv_ll_remove(encoder_ll_p, encoder);
    lv_free(encoder);
}

lv_image_encoder_t * lv_image_encoder_get_next(lv_image_encoder_t * encoder)
{
    if(encoder == NULL) return lv_ll_get_head(encoder_ll_p);
    return lv_ll_get_next(encoder_ll_p, encoder);
}

void lv_image_encoder_set_save_cb(lv_image_encoder_t * encoder, lv_image_encoder_save_cb_t save_cb)
{
    LV_CHECK_ARG(encoder != NULL, return);
    encoder->save_cb = save_cb;
}

void lv_image_encoder_set_name(lv_image_encoder_t * encoder, const char * name)
{
    LV_CHECK_ARG(encoder != NULL, return);
    encoder->name = name;
}

void lv_image_encoder_set_user_data(lv_image_encoder_t * encoder, void * user_data)
{
    LV_CHECK_ARG(encoder != NULL, return);
    encoder->user_data = user_data;
}

void lv_image_encoder_args_init(lv_image_encoder_args_t * args)
{
    LV_CHECK_ARG(args != NULL, return);
    lv_memzero(args, sizeof(lv_image_encoder_args_t));
    args->quality = 85;
    args->compression_level = 6;
}

lv_result_t lv_image_encoder_save(const lv_draw_buf_t * draw_buf,
                                  const char * path,
                                  const lv_image_encoder_args_t * args)
{
    LV_CHECK_ARG(draw_buf != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(path != NULL, return LV_RESULT_INVALID);

    if(draw_buf->data == NULL || draw_buf->data_size == 0 ||
       draw_buf->header.w == 0 || draw_buf->header.h == 0 ||
       draw_buf->header.stride == 0) {
        LV_LOG_ERROR("invalid draw_buf: data %p, size %d, w %d, h %d, stride %d",
                     (void *)draw_buf->data, (int)draw_buf->data_size,
                     (int)draw_buf->header.w, (int)draw_buf->header.h,
                     (int)draw_buf->header.stride);
        return LV_RESULT_INVALID;
    }

    /*The encoders read `h` rows of `stride` bytes, make sure the data covers them*/
    const uint32_t row_size = ((uint32_t)draw_buf->header.w * lv_color_format_get_bpp(draw_buf->header.cf) + 7) >> 3;
    /*The palette of the indexed formats is stored in front of the pixels*/
    const uint64_t palette_size = (uint64_t)LV_COLOR_INDEXED_PALETTE_SIZE(draw_buf->header.cf) * sizeof(lv_color32_t);
    if(draw_buf->header.stride < row_size ||
       palette_size + (uint64_t)draw_buf->header.stride * draw_buf->header.h > draw_buf->data_size) {
        LV_LOG_ERROR("draw_buf is too small: size %d, w %d, h %d, stride %d, cf %d",
                     (int)draw_buf->data_size, (int)draw_buf->header.w, (int)draw_buf->header.h,
                     (int)draw_buf->header.stride, (int)draw_buf->header.cf);
        return LV_RESULT_INVALID;
    }

    LV_PROFILER_DECODER_BEGIN;

    lv_image_encoder_args_t default_args;
    if(args == NULL) {
        lv_image_encoder_args_init(&default_args);
        args = &default_args;
    }

    lv_image_encoder_t * enc = NULL;
    while((enc = lv_image_encoder_get_next(enc)) != NULL) {
        if(enc->save_cb == NULL) continue;
        lv_result_t res = enc->save_cb(enc, draw_buf, path, args);
        if(res == LV_RESULT_OK) {
            lv_image_cache_drop(path);
            LV_LOG_INFO("saved to %s via %s", path, enc->name ? enc->name : "unknown");
            LV_PROFILER_DECODER_END;
            return LV_RESULT_OK;
        }
    }

    LV_LOG_ERROR("no encoder found for %s", path);
    LV_PROFILER_DECODER_END;
    return LV_RESULT_INVALID;
}

void * lv_image_encoder_get_user_data(const lv_image_encoder_t * encoder)
{
    LV_CHECK_ARG(encoder != NULL, return NULL);
    return encoder->user_data;
}

lv_result_t lv_draw_buf_save_to_file(const lv_draw_buf_t * draw_buf, const char * path)
{
    LV_CHECK_ARG(draw_buf != NULL, return LV_RESULT_INVALID);
    LV_CHECK_ARG(path != NULL, return LV_RESULT_INVALID);

    return lv_image_encoder_save(draw_buf, path, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

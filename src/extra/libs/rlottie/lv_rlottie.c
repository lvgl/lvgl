/**
 * @file lv_rlottie.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_rlottie.h"
#if LV_USE_RLOTTIE

#include "rlottiedec.h"
#include <rlottie_capi.h>

/*********************
*      DEFINES
*********************/
#define MY_CLASS &lv_rlottie_class
#define LV_ARGB32   32

/**********************
*      TYPEDEFS
**********************/
/** definition in lottieanimation_capi.c */
struct Lottie_Animation_S;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_rlottie_from_file(lv_obj_t * obj, lv_coord_t width, lv_coord_t height, const char * path,
                              const size_t buf_size)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img == NULL) return LV_RES_INV;

    lv_obj_set_size(obj, width, height);
    LV_ZALLOC(img->dec_ctx, sizeof(rlottiedec_ctx_t));
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)img->dec_ctx;
    dec_ctx->max_buf_size = buf_size;
    dec_ctx->ctx.auto_allocated = 0;

    lv_img_src_t src = {};
    lv_img_src_set_file(&src, path);
    lv_img_set_src(obj, &src);
    return img->dec_ctx == NULL || dec_ctx->cache == NULL ? LV_RES_INV : LV_RES_OK;
}

lv_res_t lv_rlottie_from_raw(lv_obj_t * obj, lv_coord_t width, lv_coord_t height, const char * desc, const size_t len,
                             const size_t buf_size)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img == NULL) return LV_RES_INV;

    lv_obj_set_size(obj, width, height);
    LV_ZALLOC(img->dec_ctx, sizeof(rlottiedec_ctx_t));
    rlottiedec_ctx_t * dec_ctx = (rlottiedec_ctx_t *)img->dec_ctx;
    dec_ctx->max_buf_size = buf_size;
    dec_ctx->ctx.auto_allocated = 0;

    lv_img_src_t src = {};
    lv_img_src_set_data(&src, (const uint8_t *)desc, len);
    lv_img_set_src(obj, &src);
    return img->dec_ctx == NULL || dec_ctx->cache == NULL ? LV_RES_INV : LV_RES_OK;
}

#endif /*LV_USE_RLOTTIE*/

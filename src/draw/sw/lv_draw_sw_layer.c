/**
 * @file lv_draw_sw_layer.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../hal/lv_hal_disp.h"
#include "../../misc/lv_area.h"
#include "../../core/lv_refr.h"

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
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


struct _lv_draw_layer_ctx_t * lv_draw_sw_layer_create(struct _lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx,
                                                      lv_draw_layer_flags_t flags)
{
    lv_draw_sw_layer_ctx_t * layer_sw_ctx = (lv_draw_sw_layer_ctx_t *) layer_ctx;
    uint32_t px_size = flags & LV_DRAW_LAYER_FLAG_HAS_ALPHA ? LV_IMG_PX_SIZE_ALPHA_BYTE : sizeof(lv_color_t);
    if(flags & LV_DRAW_LAYER_FLAG_CAN_SUBDIVIDE) {
        layer_sw_ctx->buf_size_bytes = LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE;
        uint32_t full_size = lv_area_get_size(&layer_sw_ctx->base_draw.area_full) * px_size;
        if(layer_sw_ctx->buf_size_bytes > full_size) layer_sw_ctx->buf_size_bytes = full_size;
        layer_sw_ctx->base_draw.buf = lv_malloc(layer_sw_ctx->buf_size_bytes);
        if(layer_sw_ctx->base_draw.buf == NULL) {
            LV_LOG_WARN("Cannot allocate %"LV_PRIu32" bytes for layer buffer. Allocating %"LV_PRIu32" bytes instead. (Reduced performance)",
                        (uint32_t)layer_sw_ctx->buf_size_bytes, (uint32_t)LV_DRAW_SW_LAYER_SIMPLE_FALLBACK_BUF_SIZE * px_size);
            layer_sw_ctx->buf_size_bytes = LV_DRAW_SW_LAYER_SIMPLE_FALLBACK_BUF_SIZE;
            layer_sw_ctx->base_draw.buf = lv_malloc(layer_sw_ctx->buf_size_bytes);
            if(layer_sw_ctx->base_draw.buf == NULL) {
                return NULL;
            }
        }
        layer_sw_ctx->base_draw.area_act = layer_sw_ctx->base_draw.area_full;
        layer_sw_ctx->base_draw.area_act.y2 = layer_sw_ctx->base_draw.area_full.y1;
        lv_coord_t w = lv_area_get_width(&layer_sw_ctx->base_draw.area_act);
        layer_sw_ctx->base_draw.max_row_with_alpha = layer_sw_ctx->buf_size_bytes / w / LV_IMG_PX_SIZE_ALPHA_BYTE;
        layer_sw_ctx->base_draw.max_row_with_no_alpha = layer_sw_ctx->buf_size_bytes / w / sizeof(lv_color_t);
    }
    else {
        layer_sw_ctx->base_draw.area_act = layer_sw_ctx->base_draw.area_full;
        layer_sw_ctx->buf_size_bytes = lv_area_get_size(&layer_sw_ctx->base_draw.area_full) * px_size;
        layer_sw_ctx->base_draw.buf = lv_malloc(layer_sw_ctx->buf_size_bytes);
        LV_ASSERT_MALLOC(layer_sw_ctx->base_draw.buf);
        if(layer_sw_ctx->base_draw.buf == NULL) return NULL;

        lv_memzero(layer_sw_ctx->base_draw.buf, layer_sw_ctx->buf_size_bytes);

        draw_ctx->buf = layer_sw_ctx->base_draw.buf;
        draw_ctx->buf_area = &layer_sw_ctx->base_draw.area_act;
        draw_ctx->clip_area = &layer_sw_ctx->base_draw.area_act;
        draw_ctx->render_with_alpha = flags & LV_DRAW_LAYER_FLAG_HAS_ALPHA ? 1 : 0;
    }

    return layer_ctx;
}

void lv_draw_sw_layer_adjust(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                             lv_draw_layer_flags_t flags)
{

    lv_draw_sw_layer_ctx_t * layer_sw_ctx = (lv_draw_sw_layer_ctx_t *) layer_ctx;
    if(flags & LV_DRAW_LAYER_FLAG_HAS_ALPHA) {
        lv_memzero(layer_ctx->buf, layer_sw_ctx->buf_size_bytes);
        draw_ctx->render_with_alpha = 1;
    }
    else {
        draw_ctx->render_with_alpha = 0;
    }

    draw_ctx->buf = layer_ctx->buf;
    draw_ctx->buf_area = &layer_ctx->area_act;
    draw_ctx->clip_area = &layer_ctx->area_act;
}

void lv_draw_sw_layer_blend(struct _lv_draw_ctx_t * draw_ctx, struct _lv_draw_layer_ctx_t * layer_ctx,
                            const lv_draw_img_dsc_t * draw_dsc)
{
    lv_img_dsc_t img;
    img.data = draw_ctx->buf;
    img.header.always_zero = 0;
    img.header.w = lv_area_get_width(draw_ctx->buf_area);
    img.header.h = lv_area_get_height(draw_ctx->buf_area);
    img.header.cf = draw_ctx->render_with_alpha ? LV_IMG_CF_TRUE_COLOR_ALPHA : LV_IMG_CF_TRUE_COLOR;
    lv_img_cache_invalidate_src(&img);

    /*Restore the original draw_ctx*/
    draw_ctx->buf = layer_ctx->original.buf;
    draw_ctx->buf_area = layer_ctx->original.buf_area;
    draw_ctx->clip_area = layer_ctx->original.clip_area;
    draw_ctx->render_with_alpha = layer_ctx->original.render_with_alpha;

    /*Blend the layer*/
    lv_draw_img(draw_ctx, draw_dsc, &layer_ctx->area_act, &img);
    lv_draw_wait_for_finish(draw_ctx);
}

void lv_draw_sw_layer_destroy(lv_draw_ctx_t * draw_ctx, lv_draw_layer_ctx_t * layer_ctx)
{
    LV_UNUSED(draw_ctx);

    lv_free(layer_ctx->buf);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_SW*/

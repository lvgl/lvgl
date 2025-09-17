/**
 * @file lv_draw_nanovg_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_math.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_image_private.h"

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

const lv_draw_buf_t * lv_nanovg_buffer_open_image(lv_image_decoder_dsc_t * decoder_dsc, const void * src,
                                                  bool no_cache, bool premultiply)
{
    LV_ASSERT_NULL(decoder_dsc);
    LV_ASSERT_NULL(src);

    lv_image_decoder_args_t args;
    lv_memzero(&args, sizeof(lv_image_decoder_args_t));
    args.premultiply = premultiply;
    args.no_cache = no_cache;

    lv_result_t res = lv_image_decoder_open(decoder_dsc, src, &args);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return NULL;
    }

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    if(decoded == NULL || decoded->data == NULL) {
        lv_image_decoder_close(decoder_dsc);
        LV_LOG_ERROR("image data is NULL");
        return NULL;
    }

    return decoded;
}

void lv_draw_nanovg_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords, bool no_cache)
{
    LV_PROFILER_DRAW_BEGIN;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, &t->_real_area, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_image_decoder_dsc_t decoder_dsc;
    const lv_draw_buf_t * src_buf = lv_nanovg_buffer_open_image(&decoder_dsc, dsc->src, no_cache, false);

    if(!src_buf) {
        LV_PROFILER_DRAW_END;
        return;
    }

    int image_handle = lv_nanovg_push_image(u, src_buf, lv_color_to_32(dsc->recolor, dsc->opa));
    if(image_handle < 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    /* Use coords as the fallback image width and height */
    const uint32_t img_w = dsc->header.w ? dsc->header.w : lv_area_get_width(coords);
    const uint32_t img_h = dsc->header.h ? dsc->header.h : lv_area_get_height(coords);

    NVGpaint paint = nvgImagePattern(u->vg, coords->x1, coords->y1, img_w, img_h, 0, image_handle,
                                     dsc->opa / (float)LV_OPA_COVER);

    nvgBeginPath(u->vg);
    nvgRect(u->vg, coords->x1, coords->y1, img_w, img_h);
    nvgFillColor(u->vg, nvgRGBA(0, 0, 0, 0));
    nvgFillPaint(u->vg, paint);
    nvgFill(u->vg);

    LV_PROFILER_DRAW_END;
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */

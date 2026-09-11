/**
 * @file lv_draw_nanovg_blur.c
 *
 * NanoVG blur draw task handler. Translates LVGL blur tasks into
 * nvgluBlurRegion() calls — the actual shader/FBO logic lives in
 * nanovg_gl_utils.h for backend portability.
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_fbo_cache.h"
#include "../../libs/nanovg/nanovg_gl_utils.h"

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
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nanovg_blur_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    u->blur_state = nvgluCreateBlurState();
    if(u->blur_state == NULL) {
        LV_LOG_WARN("nanovg blur: failed to create blur state (FBO not supported?)");
    }
}

void lv_draw_nanovg_blur_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    if(u->blur_state) {
        nvgluDeleteBlurState(u->blur_state, u->vg);
        u->blur_state = NULL;
    }
}

void lv_draw_nanovg_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->blur_radius <= 0) return;

    if(dsc->blur_radius > 256) {
        LV_LOG_WARN("nanovg blur: radius %d exceeds backend limit (256), skipping",
                    (int)dsc->blur_radius);
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;
    lv_layer_t * layer = t->target_layer;
    NVGLUblurState * state = u->blur_state;

    if(state == NULL) {
        LV_LOG_WARN("nanovg blur: state not initialized (FBO not supported?), skipping");
        LV_PROFILER_DRAW_END;
        return;
    }

    /* Clip the blur area to clip_area and layer extent */
    lv_area_t clip;
    if(!lv_area_intersect(&clip, coords, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }
    if(!lv_area_intersect(&clip, &clip, &layer->buf_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    const int blur_w = lv_area_get_width(&clip);
    const int blur_h = lv_area_get_height(&clip);
    if(blur_w <= 0 || blur_h <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    /* Convert LVGL top-down coords to GL bottom-up coords */
    const int layer_h   = lv_area_get_height(&layer->buf_area);
    const int rel_x     = clip.x1 - layer->buf_area.x1;
    const int rel_y_top = clip.y1 - layer->buf_area.y1;
    const int gl_y      = layer_h - rel_y_top - blur_h;

    /* Determine source FBO */
    const bool is_root = (layer->user_data == NULL);
    NVGLUframebuffer * src_fb = NULL;
    if(!is_root) {
        src_fb = lv_nanovg_fbo_cache_entry_to_fb(layer->user_data);
        if(src_fb == NULL) {
            LV_PROFILER_DRAW_END;
            return;
        }
    }

    /* Flush pending NanoVG draws before raw GL operations */
    lv_nanovg_end_frame(u);

    /* Build blur params */
    NVGLUblurParams params;
    params.radius  = dsc->blur_radius;
    params.quality = (dsc->quality == LV_BLUR_QUALITY_SPEED)
                     ? NVGLU_BLUR_QUALITY_SPEED : NVGLU_BLUR_QUALITY_NORMAL;

    /* Recolor for drop shadows */
    if(dsc->base.drop_shadow_opa > 0) {
        params.recolor = nvgRGBA(dsc->base.drop_shadow_color.red,
                                 dsc->base.drop_shadow_color.green,
                                 dsc->base.drop_shadow_color.blue,
                                 255);
    }
    else {
        params.recolor = nvgRGBA(0, 0, 0, 0);
    }

    /* Call the NanoVG blur utility */
    int ret = nvgluBlurRegion(state, u->vg, src_fb, rel_x, gl_y, blur_w, blur_h, &params);
    if(ret != 0) {
        LV_LOG_WARN("nanovg blur: nvgluBlurRegion failed (ret=%d)", ret);
    }

    LV_PROFILER_DRAW_END;
}

#endif /* LV_USE_DRAW_NANOVG */

/**
 * @file lv_draw_nanovg_3d.c
 * NanoVG 3D texture rendering - uses NanoVG native 3D extension
 * No dependency on lv_opengles driver
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE

#include "../../draw/lv_draw_3d.h"
#include "../../libs/nanovg/nanovg.h"
#include "lv_nanovg_utils.h"
#include "lv_nanovg_3d.h"

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

/** Global 3D context for NanoVG 3D rendering */
static lv_nanovg_3d_ctx_t * s_3d_ctx = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_nanovg_3d_ctx_t * lv_draw_nanovg_3d_get_ctx(void)
{
    return s_3d_ctx;
}

void lv_draw_nanovg_3d_init(NVGcontext * vg)
{
    if(s_3d_ctx == NULL && vg != NULL) {
        s_3d_ctx = lv_nanovg_3d_create(vg);
    }
}

void lv_draw_nanovg_3d_deinit(void)
{
    if(s_3d_ctx != NULL) {
        lv_nanovg_3d_destroy(s_3d_ctx);
        s_3d_ctx = NULL;
    }
}

void lv_draw_nanovg_3d(lv_draw_task_t * t, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

    if(s_3d_ctx == NULL) {
        LV_LOG_WARN("3D context not initialized");
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    /* End NanoVG frame temporarily to allow direct OpenGL rendering */
    lv_nanovg_end_frame(u);

    lv_layer_t * layer = t->target_layer;

    /* Get target layer info */
    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    /* Calculate destination area relative to layer */
    lv_area_t dest_area = *coords;
    lv_area_move(&dest_area, -layer->buf_area.x1, -layer->buf_area.y1);

    /* Calculate clip area relative to layer */
    lv_area_t clip_area = t->clip_area;
    lv_area_move(&clip_area, -layer->buf_area.x1, -layer->buf_area.y1);

    /* Save OpenGL state before 3D rendering */
    lv_nanovg_3d_begin(s_3d_ctx);

    /* Render the 3D texture using NanoVG's native texture rendering
     * Note: lv_nanovg_end_frame already ended the frame, so we need to
     * work directly with NanoVG context here */
    NVGcontext * vg = u->vg;

    /* Begin a new NanoVG frame for texture rendering */
    nvgBeginFrame(vg, layer_w, layer_h, 1.0f);

    /* Render the 3D texture */
    lv_nanovg_3d_render_texture(s_3d_ctx, dsc->tex_id, &dest_area, dsc->opa,
                                layer_w, layer_h, &clip_area, dsc->h_flip, !dsc->v_flip);

    /* End NanoVG frame */
    nvgEndFrame(vg);

    /* Restore OpenGL state */
    lv_nanovg_3d_end(s_3d_ctx);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE */

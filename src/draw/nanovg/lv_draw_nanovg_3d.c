/**
 * @file lv_draw_nanovg_3d.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE

#include "../../draw/lv_draw_3d.h"
#include "../../drivers/opengles/lv_opengles_driver.h"
#include "../../drivers/opengles/lv_opengles_private.h"
#include "lv_nanovg_utils.h"
#include "lv_nanovg_fbo_cache.h"

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

void lv_draw_nanovg_3d(lv_draw_task_t * t, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;

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

    /* Reinitialize OpenGL ES driver state after NanoVG modified it */
    lv_opengles_reinit_state();

    /* Use LVGL's OpenGL ES rendering infrastructure */
    lv_opengles_viewport(0, 0, layer_w, layer_h);

    lv_opengles_render_params_t params;
    lv_opengles_render_params_init(&params);
    params.texture = dsc->tex_id;
    params.texture_area = &dest_area;
    params.opa = dsc->opa;
    params.disp_w = layer_w;
    params.disp_h = layer_h;
    params.texture_clip_area = &clip_area;
    params.h_flip = dsc->h_flip;
    params.v_flip = dsc->v_flip;
    params.rb_swap = true;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    params.matrix = &layer->matrix;
#endif
    lv_opengles_render(&params);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE */

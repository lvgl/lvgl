/**
 * @file lv_draw_nanovg_layer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_fbo_cache.h"
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

void lv_draw_nanovg_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;
    lv_layer_t * layer_to_draw = (lv_layer_t *)dsc->src;

    if(!layer_to_draw->user_data) {
        LV_PROFILER_DRAW_END;
        return;
    }

    int image_handle = lv_nanovg_fb_get_image_handle(lv_nanovg_fbo_cache_entry_to_fb(layer_to_draw->user_data));
    if(image_handle <= 0) {
        LV_LOG_WARN("Invalid image handle: %d", image_handle);
        LV_PROFILER_DRAW_END;
        return;
    }

    lv_draw_image_dsc_t new_draw_dsc = *dsc;
    new_draw_dsc.src = NULL;
    lv_draw_nanovg_image(draw_unit, layer, clip_area, &new_draw_dsc, coords, image_handle);

    lv_nanovg_end_frame(u);

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

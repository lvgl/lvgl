/**
 * @file lv_draw_m2d_layer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_M2D
#include "lv_draw_m2d_private.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_draw_m2d_layer_is_supported(const lv_draw_task_t * task, const lv_draw_image_dsc_t * draw_dsc)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    if(layer_to_draw->draw_buf == NULL) return true;

    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;

    return lv_draw_m2d_image_is_supported(task, &new_draw_dsc);
}

void lv_draw_m2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*
     * It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     * In this case just return.
     */
    if(layer_to_draw->draw_buf == NULL) return;

    /* The source should be a draw_buf, not a layer */
    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;

    lv_draw_m2d_image(t, &new_draw_dsc, coords);
}

#endif /*LV_USE_DRAW_M2D*/

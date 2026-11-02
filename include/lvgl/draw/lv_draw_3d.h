/**
 * @file lv_draw_3d.h
 *
 */

#ifndef LV_DRAW_3D_H
#define LV_DRAW_3D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw.h"

#if LV_USE_3DTEXTURE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_dsc_t base;

    /**The area to draw*/
    lv_area_t coords;
    lv_3dtexture_id_t tex_id;
    bool h_flip;
    bool v_flip;
    lv_opa_t opa;
} lv_draw_3d_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a 3D draw descriptor
 * @param dsc       pointer to a draw descriptor
 */
void lv_draw_3d_dsc_init(lv_draw_3d_dsc_t * dsc);

/**
 * Try to get a 3D draw descriptor from a draw task.
 * @param task      draw task
 * @return          the task's draw descriptor or NULL if the task is not of type LV_DRAW_DSC_TYPE_3D
 */
lv_draw_3d_dsc_t * lv_draw_task_get_3d_dsc(lv_draw_task_t * task);

/**
 * Get the 3D draw descriptor from a generic draw descriptor.
 * `dsc` must be of type LV_DRAW_DSC_TYPE_3D, use `lv_draw_dsc_get_type()` to check it first.
 * @param dsc       draw descriptor
 * @return          the 3D draw descriptor
 */
lv_draw_3d_dsc_t * lv_draw_dsc_get_3d_dsc(lv_draw_dsc_t * dsc);

/**
 * Create a 3D draw task
 * @param layer     pointer to a layer
 * @param dsc       pointer to an initialized `lv_draw_3d_dsc_t` variable
 * @param coords    the coordinates of where to draw the widget
 */
void lv_draw_3d(lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_3DTEXTURE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_3D_H*/

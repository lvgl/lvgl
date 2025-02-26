/**
 * @file lv_vglite_matrix.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_matrix.h"

#if LV_USE_DRAW_VGLITE

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

static vg_lite_matrix_t _matrix;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

vg_lite_matrix_t * vglite_get_matrix(void)
{
    return &_matrix;
}

void vglite_set_translation_matrix(const lv_area_t * dest_area)
{
    vg_lite_identity(&_matrix);
    vg_lite_translate((vg_lite_float_t)dest_area->x1, (vg_lite_float_t)dest_area->y1, &_matrix);
}

void vglite_set_transformation_matrix(const lv_area_t * dest_area, const lv_draw_image_dsc_t * dsc)
{
    vglite_set_translation_matrix(dest_area);

    bool has_scale = (dsc->scale_x != LV_SCALE_NONE || dsc->scale_y != LV_SCALE_NONE);
    bool has_rotation = (dsc->rotation != 0);

    if(has_scale || has_rotation) {
        vg_lite_translate(dsc->pivot.x, dsc->pivot.y, &_matrix);
        if(has_rotation)
            vg_lite_rotate(dsc->rotation / 10.0f, &_matrix);   /* angle is 1/10 degree */
        if(has_scale) {
            vg_lite_float_t scale_x = 1.0f * dsc->scale_x / LV_SCALE_NONE;
            vg_lite_float_t scale_y = 1.0f * dsc->scale_y / LV_SCALE_NONE;
            vg_lite_scale(scale_x, scale_y, &_matrix);
        }
        vg_lite_translate(0.0f - dsc->pivot.x, 0.0f - dsc->pivot.y, &_matrix);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/

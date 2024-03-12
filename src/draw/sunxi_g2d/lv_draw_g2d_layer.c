/**
 * @file lv_draw_g2d_layer.c
 *
 */

/**
 * Copyright 2023，2024 G2D
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D

#include "../../stdlib/lv_string.h"

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

void lv_draw_g2d_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->draw_buf == NULL)
        return;

    /*lv_image_dsc_t img_dsc;
    img_dsc.header.w = lv_area_get_width(&layer_to_draw->buf_area);
    img_dsc.header.h = lv_area_get_height(&layer_to_draw->buf_area);
    img_dsc.header.cf = layer_to_draw->color_format;
    img_dsc.header.stride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer_to_draw->buf_area),
                                                        layer_to_draw->color_format);
    img_dsc.header.always_zero = 0;
    img_dsc.data = lv_draw_buf_align(layer_to_draw->buf, layer_to_draw->color_format);*/

    lv_draw_image_dsc_t new_draw_dsc;
    lv_memcpy(&new_draw_dsc, draw_dsc, sizeof(lv_draw_image_dsc_t));
    new_draw_dsc.src = layer_to_draw->draw_buf;

    lv_draw_g2d_img(draw_unit, &new_draw_dsc, coords);
}

#endif /*LV_USE_DRAW_GD2D*/

/**
 * @file lv_draw_vglite_layer.c
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

#include "lv_draw_vglite.h"

#if LV_USE_DRAW_VGLITE

#include "../../../stdlib/lv_string.h"

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

void lv_draw_vglite_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->draw_buf.buf == NULL)
        return;

    lv_image_dsc_t img_dsc = { 0 };
    img_dsc.header.w = layer_to_draw->draw_buf.width;
    img_dsc.header.h = layer_to_draw->draw_buf.height;
    img_dsc.header.cf = layer_to_draw->draw_buf.color_format;
    img_dsc.data = lv_draw_buf_get_buf(&layer_to_draw->draw_buf);

    lv_draw_image_dsc_t new_draw_dsc;
    lv_memcpy(&new_draw_dsc, draw_dsc, sizeof(lv_draw_image_dsc_t));
    new_draw_dsc.src = &img_dsc;

    lv_draw_vglite_img(draw_unit, &new_draw_dsc, coords);
}

#endif /*LV_USE_DRAW_VGLITE*/

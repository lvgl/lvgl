/**
 * @file lv_draw_vglite.h
 *
 */

/**
 * Copyright 2023-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_DRAW_VGLITE_H
#define LV_DRAW_VGLITE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_DRAW_VGLITE
#include "../../sw/lv_draw_sw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_draw_sw_unit_t lv_draw_vglite_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_buf_vglite_init_handlers(void);

void lv_draw_vglite_init(void);

void lv_draw_vglite_deinit(void);

void lv_draw_vglite_arc(lv_draw_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_vglite_border(lv_draw_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords);

void lv_draw_vglite_fill(lv_draw_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc,
                         const lv_area_t * coords);

void lv_draw_vglite_img(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_vglite_label(lv_draw_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc,
                          const lv_area_t * coords);

void lv_draw_vglite_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_vglite_line(lv_draw_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

void lv_draw_vglite_triangle(lv_draw_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/
#endif /*LV_USE_DRAW_VGLITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_VGLITE_H*/

/**
 * @file lv_draw_eve_private.h
 *
 */

/*  Author: juanj
 *
 *  Modified by LVGL
 */

#ifndef LV_DRAW_EVE_PRIVATE_H
#define LV_DRAW_EVE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE

#include "../lv_draw_private.h"
#include "../../misc/lv_types.h"
#include "../lv_draw_triangle.h"
#include "../lv_draw_line.h"
#include "../lv_draw_label.h"
#include "../lv_draw_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
} lv_draw_eve_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_eve_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords);

void lv_draw_eve_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                       const lv_area_t * coords);

void lv_draw_eve_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_eve_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc,
                        const lv_area_t * coords);

void lv_draw_eve_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

void lv_draw_eve_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc,
                       const lv_area_t * coords);

void lv_draw_eve_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_eve_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#define DEGREES(n) ((65536UL * (n)) / 3600)
#define F16(x)     ((int32_t)((x) * 65536L))

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_PRIVATE_H*/

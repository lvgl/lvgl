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

#include "lv_draw_eve_target.h"
#include "../lv_draw_private.h"
#include "../../misc/lv_types.h"
#include "../../core/lv_global.h"
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
    uintptr_t key;
    uint32_t addr;
} lv_draw_eve_ramg_hash_table_cell_t;

typedef struct {
    uint32_t ramg_addr_end;
    uint32_t hash_table_cell_count;
    uint32_t hash_table_cells_occupied;
    lv_draw_eve_ramg_hash_table_cell_t * hash_table;
} lv_draw_eve_ramg_t;

struct _lv_draw_eve_unit_t {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    lv_display_t * disp;
    lv_draw_eve_ramg_t ramg;
    lv_draw_eve_parameters_t params;
    lv_draw_eve_operation_cb_t op_cb;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

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

#define lv_draw_eve_unit_g (LV_GLOBAL_DEFAULT()->draw_eve_unit)

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_PRIVATE_H*/

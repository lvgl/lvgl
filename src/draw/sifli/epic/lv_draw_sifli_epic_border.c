/**
 * @file lv_draw_sifli_epic_border.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sifli_epic.h"

#if LV_USE_SIFLI_EPIC
#include "../../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_border_simple(lv_draw_task_t * task, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               const lv_draw_border_dsc_t * dsc);
static void draw_fill_part(lv_draw_task_t * task, const lv_area_t * area, const lv_draw_border_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sifli_epic_border(lv_draw_task_t * task)
{
    const lv_draw_border_dsc_t * dsc = (const lv_draw_border_dsc_t *)task->draw_dsc;
    const lv_area_t * coords = &task->area;

    if(dsc->opa <= LV_OPA_MIN) return;
    if(dsc->width == 0) return;
    if(dsc->side == LV_BORDER_SIDE_NONE) return;

    if(dsc->radius != 0) {
        return;
    }

    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->side & LV_BORDER_SIDE_LEFT) ? dsc->width : -dsc->width);
    area_inner.x2 -= ((dsc->side & LV_BORDER_SIDE_RIGHT) ? dsc->width : -dsc->width);
    area_inner.y1 += ((dsc->side & LV_BORDER_SIDE_TOP) ? dsc->width : -dsc->width);
    area_inner.y2 -= ((dsc->side & LV_BORDER_SIDE_BOTTOM) ? dsc->width : -dsc->width);

    draw_border_simple(task, coords, &area_inner, dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_border_simple(lv_draw_task_t * task, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               const lv_draw_border_dsc_t * dsc)
{
    lv_area_t area;

    bool top_side = outer_area->y1 <= inner_area->y1;
    bool bottom_side = outer_area->y2 >= inner_area->y2;
    bool left_side = outer_area->x1 <= inner_area->x1;
    bool right_side = outer_area->x2 >= inner_area->x2;

    /* Top */
    area.x1 = outer_area->x1;
    area.x2 = outer_area->x2;
    area.y1 = outer_area->y1;
    area.y2 = inner_area->y1 - 1;
    if(top_side && area.y1 <= area.y2) {
        draw_fill_part(task, &area, dsc);
    }

    /* Bottom */
    area.y1 = inner_area->y2 + 1;
    area.y2 = outer_area->y2;
    if(bottom_side && area.y1 <= area.y2) {
        draw_fill_part(task, &area, dsc);
    }

    /* Left */
    area.x1 = outer_area->x1;
    area.x2 = inner_area->x1 - 1;
    area.y1 = top_side ? inner_area->y1 : outer_area->y1;
    area.y2 = bottom_side ? inner_area->y2 : outer_area->y2;
    if(left_side && area.x1 <= area.x2 && area.y1 <= area.y2) {
        draw_fill_part(task, &area, dsc);
    }

    /* Right */
    area.x1 = inner_area->x2 + 1;
    area.x2 = outer_area->x2;
    if(right_side && area.x1 <= area.x2 && area.y1 <= area.y2) {
        draw_fill_part(task, &area, dsc);
    }
}

static void draw_fill_part(lv_draw_task_t * task, const lv_area_t * area, const lv_draw_border_dsc_t * dsc)
{
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = dsc->color;
    fill_dsc.opa = dsc->opa;

    lv_draw_task_t fill_task = *task;
    fill_task.draw_dsc = &fill_dsc;
    fill_task.area = *area;
    lv_draw_sifli_epic_fill(&fill_task);
}

#endif /*LV_USE_SIFLI_EPIC*/

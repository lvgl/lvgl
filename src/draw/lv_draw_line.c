/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_private.h"
#include "../core/lv_refr.h"
#include "../misc/lv_math.h"
#include "../misc/lv_types.h"
#include "../stdlib/lv_string.h"

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

void LV_ATTRIBUTE_FAST_MEM lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc)
{
    lv_memzero(dsc, sizeof(lv_draw_line_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = lv_color_black();
    dsc->base.dsc_size = sizeof(lv_draw_line_dsc_t);
}

lv_draw_line_dsc_t * lv_draw_task_get_line_dsc(lv_draw_task_t * task)
{
    return task->type == LV_DRAW_TASK_TYPE_LINE ? (lv_draw_line_dsc_t *)task->draw_dsc : NULL;
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_line(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    LV_PROFILER_DRAW_BEGIN;

    lv_area_t a;
    lv_point_precise_t * new_points = NULL;
    if(dsc->points == NULL) {
        a.x1 = (int32_t)LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width;
        a.x2 = (int32_t)LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width;
        a.y1 = (int32_t)LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width;
        a.y2 = (int32_t)LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width;
    }
    else {
        if(dsc->point_cnt <= 1) {
            LV_LOG_INFO("Skip line drawing as point_cnt was 1");
            LV_PROFILER_DRAW_END;
            return;
        }

        a.x1 = LV_COORD_MAX;
        a.y1 = LV_COORD_MAX;
        a.x2 = LV_COORD_MIN;
        a.y2 = LV_COORD_MIN;

        const size_t array_size = dsc->point_cnt * sizeof(lv_point_precise_t);
        new_points = lv_malloc(array_size);
        LV_ASSERT_MALLOC(new_points);
        if(!new_points) {
            LV_LOG_WARN("Couldn't allocate %" LV_PRId32 "points", dsc->point_cnt);
            LV_PROFILER_DRAW_END;
            return;
        }

        int32_t i;
        for(i = 0; i < dsc->point_cnt; i++) {
            new_points[i] = dsc->points[i];
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE ||
               dsc->points[i].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }

            a.x1 = (int32_t)LV_MIN(a.x1, dsc->points[i].x) - dsc->width;
            a.x2 = (int32_t)LV_MAX(a.x2, dsc->points[i].x) + dsc->width;
            a.y1 = (int32_t)LV_MIN(a.y1, dsc->points[i].y) - dsc->width;
            a.y2 = (int32_t)LV_MAX(a.y2, dsc->points[i].y) + dsc->width;
        }

        if(a.x1 == LV_COORD_MAX) {
            lv_free(new_points);
            LV_LOG_INFO("No valid point was found. Not adding the draw task.");
            LV_PROFILER_DRAW_END;
            return;
        }
        lv_area_increase(&a, dsc->width, dsc->width);
    }

    if(dsc->base.drop_shadow_opa) {
        lv_layer_t * ds_layer = lv_draw_layer_create_drop_shadow(layer, &dsc->base, &a);
        LV_ASSERT_NULL(ds_layer);
        lv_draw_line_dsc_t ds_dsc = *dsc;
        ds_dsc.base.drop_shadow_opa = 0; /*Disable drop shadow so rendering below will render plain line*/
        lv_draw_line(ds_layer, &ds_dsc);
        lv_draw_layer_finish_drop_shadow(ds_layer, &dsc->base);
    }

    lv_draw_task_t * t = lv_draw_add_task(layer, &a, LV_DRAW_TASK_TYPE_LINE);
    lv_draw_line_dsc_t * line_draw_dsc = t->draw_dsc;
    lv_memcpy(line_draw_dsc, dsc, sizeof(*dsc));
    line_draw_dsc->points = new_points;

    lv_draw_finalize_task_creation(layer, t);
    LV_PROFILER_DRAW_END;
}


void lv_draw_line_iterate(lv_draw_task_t * t, lv_draw_line_dsc_t * dsc,
                          void (*draw_line_cb)(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc))
{
    uint32_t i;
    lv_point_precise_t * points = dsc->points;
    if(points == NULL) {
        draw_line_cb(t, dsc);
    }
    else {
        /*Create a temporary dsc where the point array is replaced by 2 points*/
        lv_draw_line_dsc_t dsc_tmp = *dsc;
        size_t point_cnt = dsc_tmp.point_cnt;
        if(point_cnt <= 1) return;

        dsc_tmp.points = NULL;
        dsc_tmp.point_cnt = 0;
        for(i = 0; i < point_cnt - 1; i++) {
            if(points[i].x == LV_DRAW_LINE_POINT_NONE ||
               points[i].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }
            if(points[i + 1].x == LV_DRAW_LINE_POINT_NONE ||
               points[i + 1].y == LV_DRAW_LINE_POINT_NONE) {
                continue;
            }

            dsc_tmp.p1 = points[i];
            dsc_tmp.p2 = points[i + 1];

            draw_line_cb(t, &dsc_tmp);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

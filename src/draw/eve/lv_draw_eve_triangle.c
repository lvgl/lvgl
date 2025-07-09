/**
 * @file lv_draw_eve_triangle.c
 *
 */

/*  Created on: 10 ene 2024
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE

#include "../../misc/lv_math.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_area_private.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "lv_eve.h"

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

void lv_draw_eve_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc)
{

    lv_area_t tri_area;
    tri_area.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    tri_area.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    tri_area.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    bool is_common;
    lv_area_t draw_area;
    is_common = lv_area_intersect(&draw_area, &tri_area, &t->clip_area);
    if(!is_common) return;

    lv_point_t p[3];
    /*If there is a vertical side use it as p[0] and p[1]*/
    if(dsc->p[0].x == dsc->p[1].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);
    }
    else if(dsc->p[0].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[1]);
    }
    else if(dsc->p[1].x == dsc->p[2].x) {
        p[0] = lv_point_from_precise(&dsc->p[1]);
        p[1] = lv_point_from_precise(&dsc->p[2]);
        p[2] = lv_point_from_precise(&dsc->p[0]);
    }
    else {
        p[0] = lv_point_from_precise(&dsc->p[0]);
        p[1] = lv_point_from_precise(&dsc->p[1]);
        p[2] = lv_point_from_precise(&dsc->p[2]);

        /*Set the smallest y as p[0]*/
        if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);
        if(p[0].y > p[2].y) lv_point_swap(&p[0], &p[2]);

        /*Set the greatest y as p[1]*/
        if(p[1].y < p[2].y) lv_point_swap(&p[1], &p[2]);
    }

    /*Be sure p[0] is on the top*/
    if(p[0].y > p[1].y) lv_point_swap(&p[0], &p[1]);

    lv_eve_save_context();

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);

    lv_eve_color(dsc->color);
    lv_eve_color_opa(dsc->opa);

    lv_eve_color_mask(0, 0, 0, 0);
    lv_eve_stencil_op(EVE_KEEP, EVE_INVERT);
    lv_eve_stencil_func(EVE_ALWAYS, 255, 255);
    lv_eve_primitive(LV_EVE_PRIMITIVE_EDGE_STRIP_A);

    lv_eve_vertex_2f(p[0].x, p[0].y);
    lv_eve_vertex_2f(p[1].x, p[1].y);
    lv_eve_vertex_2f(p[2].x, p[2].y);

    lv_eve_color_mask(1, 1, 1, 1);
    lv_eve_stencil_func(EVE_EQUAL, 255, 255) ;

    lv_eve_vertex_2f(0, 0);
    lv_eve_vertex_2f(1022, 0);

    lv_eve_restore_context();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_EVE*/



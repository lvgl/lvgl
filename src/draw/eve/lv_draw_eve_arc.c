/**
 * @file lv_draw_eve_arc.c
 *
 */

/*  Created on: 11 dic 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE

#include "../lv_draw_arc.h"
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 * STATIC PROTOTYPES
 **********************/

static void draw_eve_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static bool is_same_quadrant(int16_t start_angle, int16_t end_angle) ;
static void draw_rounded_end(lv_point_t center, int32_t radius, int32_t angle, int32_t width);
static void lv_draw_eve_mask_angle(const lv_draw_arc_dsc_t * dsc, int32_t vertex_x, int32_t vertex_y,
                                   int32_t start_angle, int32_t end_angle);
static lv_eve_primitive_t get_mask_direction(int16_t angle);
static int32_t chord_length(int16_t radius, int16_t angle_degrees);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    draw_eve_arc(t, dsc, coords);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static int32_t chord_length(int16_t radius, int16_t angle_degrees)
{
    angle_degrees %= 360;
    if(angle_degrees < 0) angle_degrees += 360;
    int32_t sin_value = lv_trigo_sin(angle_degrees / 2);
    int64_t chord_length = 2 * radius * sin_value / 32768.0;
    return (int32_t)chord_length ;
}


static lv_eve_primitive_t get_mask_direction(int16_t angle)
{
    if(angle >= 315 || angle <  45) {
        return LV_EVE_PRIMITIVE_EDGE_STRIP_R;
    }
    if(angle >=  45 && angle < 135) {
        return LV_EVE_PRIMITIVE_EDGE_STRIP_B;
    }
    if(angle >= 135 && angle < 225) {
        return LV_EVE_PRIMITIVE_EDGE_STRIP_L;
    }
    if(angle >= 225 && angle < 315) {
        return LV_EVE_PRIMITIVE_EDGE_STRIP_A;
    }
    return 0;
}


static void draw_rounded_end(lv_point_t center, int32_t radius, int32_t angle, int32_t width)
{
    int32_t rounded_y  = center.y + ((lv_trigo_sin(angle) * radius) >> LV_TRIGO_SHIFT);
    int32_t rounded_x  = center.x + ((lv_trigo_cos(angle) * radius) >> LV_TRIGO_SHIFT);
    lv_eve_draw_circle_simple(rounded_x, rounded_y, width);
}



static bool is_same_quadrant(int16_t start_angle, int16_t end_angle)
{
    if(start_angle > end_angle) {
        if((start_angle >= 0 && start_angle < 90) && (end_angle >= 0 && end_angle < 90)) {
            return true;
        }
        else if((start_angle >= 90 && start_angle < 180) && (end_angle >= 90 && end_angle < 180)) {
            return true;
        }
        else if((start_angle >= 180 && start_angle < 270) && (end_angle >= 180 && end_angle < 270)) {
            return true;
        }
        else if((start_angle >= 270 && start_angle < 360) && (end_angle >= 270 && end_angle < 360)) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}



static void lv_draw_eve_mask_angle(const lv_draw_arc_dsc_t * dsc, int32_t vertex_x, int32_t vertex_y,
                                   int32_t start_angle, int32_t end_angle)
{

    /*Constrain the input angles*/


    if(start_angle < 0)
        start_angle = 0;
    else if(start_angle > 359)
        start_angle = 359;

    if(end_angle < 0)
        end_angle = 0;
    else if(end_angle > 359)
        end_angle = 359;

    LV_ASSERT_MSG(start_angle >= 0 && start_angle <= 360, "Unexpected start angle");

    int32_t mid_angle_op;
    int32_t angle_range;
    int32_t mask_dir_start;
    int32_t mask_dir_end;
    lv_point_t start;
    lv_point_t end;
    lv_point_t angle_range_op;

    if(end_angle > start_angle) {
        angle_range = LV_ABS(end_angle - start_angle);
    }
    else {
        angle_range = 360 - start_angle + end_angle;
    }

    mid_angle_op = (angle_range / 2) + start_angle + 180;
    mid_angle_op = mid_angle_op % 360;

    mask_dir_end = LV_ABS(((360 - angle_range) / 4) + end_angle);
    mask_dir_start = LV_ABS(((360 - angle_range) / 4) + mid_angle_op);

    mask_dir_start = mask_dir_start % 360;
    mask_dir_end = mask_dir_end % 360;

    start.y = (lv_trigo_sin(start_angle) >> 5) + vertex_y;
    start.x = (lv_trigo_cos(start_angle) >> 5) + vertex_x;

    end.y = (lv_trigo_sin(end_angle) >> 5) + vertex_y;
    end.x = (lv_trigo_cos(end_angle) >> 5) + vertex_x;

    angle_range_op.y = (lv_trigo_sin(mid_angle_op) >> 5) + vertex_y;
    angle_range_op.x = (lv_trigo_cos(mid_angle_op) >> 5) + vertex_x;

    if(angle_range <= 180) {
        /* Two sides mask and 6 vertex points */

        /* Masking end angle */
        lv_eve_primitive_t edge = get_mask_direction(mask_dir_end);
        lv_eve_primitive(edge); /* Side one */
        lv_eve_vertex_2f(angle_range_op.x, angle_range_op.y);
        lv_eve_vertex_2f(vertex_x, vertex_y);
        lv_eve_vertex_2f(end.x, end.y);

        /* Masking start angle */
        edge = get_mask_direction(mask_dir_start);
        lv_eve_primitive(edge); /* Side two */
        lv_eve_vertex_2f(angle_range_op.x, angle_range_op.y);
        lv_eve_vertex_2f(vertex_x, vertex_y);
        lv_eve_vertex_2f(start.x, start.y);

    }

    else {

        if(is_same_quadrant(start_angle,
                            end_angle)) { /* "It is not an optimal implementation for the case where both angles (start and end) are in the same quadrant */
            /* todo */
            lv_point_t end_line_cntr;
            lv_point_t start_line_cntr;

            lv_point_t end_line_brd;
            lv_point_t start_line_brd;

            int16_t chord = chord_length(dsc->radius, 360 - angle_range);
            int16_t w = ((chord / 4) < 1) ? 1 : chord / 4;
            int16_t r_width = w;

            end_line_brd.y = vertex_y + ((lv_trigo_sin(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
            end_line_brd.x = vertex_x + ((lv_trigo_cos(end_angle) * dsc->radius) >> LV_TRIGO_SHIFT);

            start_line_brd.y = vertex_y + ((lv_trigo_sin(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);
            start_line_brd.x = vertex_x + ((lv_trigo_cos(start_angle) * dsc->radius) >> LV_TRIGO_SHIFT);

            lv_eve_draw_rect_simple(start_line_brd.x, start_line_brd.y, end_line_brd.x, end_line_brd.y, 0);

            start_line_brd.y = start_line_brd.y + ((lv_trigo_sin(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);
            start_line_brd.x = start_line_brd.x + ((lv_trigo_cos(start_angle - 90) * r_width) >> LV_TRIGO_SHIFT);

            end_line_brd.y = end_line_brd.y + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
            end_line_brd.x = end_line_brd.x + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);

            end_line_cntr.y = vertex_y + ((lv_trigo_sin(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);
            end_line_cntr.x = vertex_x + ((lv_trigo_cos(end_angle + 90) * r_width) >> LV_TRIGO_SHIFT);

            start_line_cntr.y = vertex_y + ((lv_trigo_sin(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);
            start_line_cntr.x = vertex_x + ((lv_trigo_cos(start_angle + 270) * r_width) >> LV_TRIGO_SHIFT);

            lv_eve_primitive(LV_EVE_PRIMITIVE_LINE_STRIP);
            lv_eve_line_width(r_width * 16);
            lv_eve_vertex_2f(start_line_cntr.x, start_line_cntr.y);
            lv_eve_vertex_2f(start_line_brd.x, start_line_brd.y);
            lv_eve_vertex_2f(end_line_brd.x, end_line_brd.y);
            lv_eve_vertex_2f(end_line_cntr.x, end_line_cntr.y);

        }
        else { /* One side mask and 3 vertex points */
            /* Masking end and start angles */
            lv_eve_primitive_t edge = get_mask_direction(mid_angle_op);
            lv_eve_primitive(edge);
            lv_eve_vertex_2f(end.x, end.y);
            lv_eve_vertex_2f(vertex_x, vertex_y);
            lv_eve_vertex_2f(start.x, start.y);
        }
    }

}



static void draw_eve_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{

    if(dsc->opa <= LV_OPA_MIN)
        return;
    if(dsc->width == 0)
        return;
    if(dsc->start_angle == dsc->end_angle)
        return;

    lv_color_t color = dsc->color;
    lv_opa_t opa = dsc->opa;
    lv_point_t center = dsc->center;
    int32_t width = dsc->width;
    uint16_t radius_out = dsc->radius;
    uint16_t radius_in = dsc->radius - dsc->width;
    int32_t start_angle = (int32_t) dsc->start_angle;
    int32_t end_angle = (int32_t) dsc->end_angle;

    if(width > radius_out)
        width = radius_out;

    while(start_angle >= 360)
        start_angle -= 360;
    while(end_angle >= 360)
        end_angle -= 360;

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);

    lv_eve_save_context();

    lv_eve_color(color);
    lv_eve_color_opa(opa);

    lv_eve_color_mask(0, 0, 0, 1);
    lv_eve_stencil_func(EVE_ALWAYS, 0, 1);
    lv_eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    lv_eve_draw_circle_simple(center.x, center.y, radius_out); /* radius_out */

    lv_eve_blend_func(EVE_ONE, EVE_ZERO);
    lv_eve_draw_circle_simple(center.x, center.y, radius_in + 2); /* radius_in */

    lv_eve_stencil_func(EVE_ALWAYS, 1, 1);
    lv_eve_stencil_op(EVE_REPLACE, EVE_REPLACE);
    lv_eve_blend_func(EVE_ZERO, EVE_ONE_MINUS_SRC_ALPHA);
    lv_eve_color_opa(0XFF);

    /* Start masking arc */

    lv_draw_eve_mask_angle(dsc, center.x, center.y, start_angle, end_angle);

    /* End masking arc */

    lv_eve_draw_circle_simple(center.x, center.y, radius_in); /* radius_in */

    lv_eve_color_mask(1, 1, 1, 1);
    lv_eve_blend_func(EVE_DST_ALPHA, EVE_ONE_MINUS_DST_ALPHA);
    lv_eve_draw_circle_simple(center.x, center.y, radius_in); /* radius_in */

    lv_eve_stencil_func(EVE_NOTEQUAL, 1, 0XFF);
    lv_eve_stencil_op(EVE_KEEP, EVE_KEEP);
    lv_eve_blend_func(EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA);

    lv_eve_color_opa(opa);
    lv_eve_draw_circle_simple(center.x, center.y, radius_out); /* radius_out */

    if(dsc->rounded) {
        lv_eve_stencil_func(EVE_EQUAL, 1, 0XFF);
        if(opa < 255) {
            lv_eve_stencil_op(EVE_ZERO, EVE_ZERO);
        }

        int32_t half_width = width / 2;
        int32_t adjusted_radius = radius_out - half_width;
        draw_rounded_end(center, adjusted_radius, end_angle, half_width);
        draw_rounded_end(center, adjusted_radius, start_angle, half_width);
    }

    lv_eve_restore_context();
}



#endif /*LV_USE_DRAW_EVE*/

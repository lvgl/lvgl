/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_triangle.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_mem.h"

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

/**
 *
 * @param points pointer to an array with 3 points
 * @param clip_area the triangle will be drawn only in this area
 * @param style style for of the triangle
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * clip_area, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_draw_polygon(points, 3, clip_area, style, opa_scale);
}

/**
 * Draw a polygon. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param clip_area polygon will be drawn only in this area
 * @param style style of the polygon
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_polygon(const lv_point_t * points, uint16_t point_cnt, const lv_area_t * clip_area, const lv_style_t * style,
                     lv_opa_t opa_scale)
{
    if(point_cnt < 3) return;
    if(points == NULL) return;

    int16_t i;
    lv_area_t poly_mask = {.x1 = LV_COORD_MAX, .y1 = LV_COORD_MAX, .x2 = LV_COORD_MIN, .y2 = LV_COORD_MIN};

    for(i = 0; i < point_cnt; i++) {
        poly_mask.x1 = LV_MATH_MIN(poly_mask.x1, points[i].x);
        poly_mask.y1 = LV_MATH_MIN(poly_mask.y1, points[i].y);
        poly_mask.x2 = LV_MATH_MAX(poly_mask.x2, points[i].x);
        poly_mask.y2 = LV_MATH_MAX(poly_mask.y2, points[i].y);
    }


    bool is_common;
    is_common = lv_area_intersect(&poly_mask, &poly_mask, clip_area);
    if(!is_common) return;

    /*Find the lowest point*/
    lv_coord_t y_min = points[0].y;
    int16_t y_min_i = 0;

    for(i = 1; i < point_cnt; i++) {
        if(points[i].y < y_min) {
            y_min = points[i].y;
            y_min_i = i;
        }
    }

    lv_draw_mask_line_param_t * mp = lv_mem_buf_get(sizeof(lv_draw_mask_line_param_t) * point_cnt);
    lv_draw_mask_line_param_t * mp_next = mp;

    int32_t i_prev_left = y_min_i;
    int32_t i_prev_right = y_min_i;
    int32_t i_next_left;
    int32_t i_next_right;
    uint32_t mask_cnt = 0;

    /* Check if the order of points is inverted or not.
     * The normal case is when the left point is on `y_min_i - 1`*/
    i_next_left = y_min_i - 1;
    if(i_next_left < 0) i_next_left = point_cnt + i_next_left;

    i_next_right = y_min_i + 1;
    if(i_next_right > point_cnt - 1) i_next_right = 0;

    bool inv = false;
    if(points[i_next_left].x > points[i_next_right].x && points[i_next_left].y < points[i_next_right].y) inv = true;

    do {
        if(!inv) {
            i_next_left = i_prev_left - 1;
            if(i_next_left < 0) i_next_left = point_cnt + i_next_left;

            i_next_right = i_prev_right + 1;
            if(i_next_right > point_cnt - 1) i_next_right = 0;
        } else {
            i_next_left = i_prev_left + 1;
            if(i_next_left > point_cnt - 1) i_next_left = 0;

            i_next_right = i_prev_right - 1;
            if(i_next_right < 0) i_next_right = point_cnt + i_next_right;
        }

        if(points[i_next_left].y >=  points[i_prev_left].y) {
            if(points[i_next_left].y != points[i_prev_left].y &&
                           points[i_next_left].x !=  points[i_prev_left].x) {
                lv_draw_mask_line_points_init(mp_next, points[i_prev_left].x, points[i_prev_left].y,
                                                         points[i_next_left].x, points[i_next_left].y,
                                                         LV_DRAW_MASK_LINE_SIDE_RIGHT);
                lv_draw_mask_add(mp_next, mp);
                mp_next++;
            }
            mask_cnt++;
            i_prev_left = i_next_left;
        }

        if(mask_cnt == point_cnt) break;

        if(points[i_next_right].y >=  points[i_prev_right].y) {
            if(points[i_next_right].y != points[i_prev_right].y &&
               points[i_next_right].x !=  points[i_prev_right].x) {

                lv_draw_mask_line_points_init(mp_next, points[i_prev_right].x, points[i_prev_right].y,
                                                         points[i_next_right].x, points[i_next_right].y,
                                                         LV_DRAW_MASK_LINE_SIDE_LEFT);
                lv_draw_mask_add(mp_next, mp);
                mp_next++;
            }
            mask_cnt++;
            i_prev_right = i_next_right;
        }

    }while( mask_cnt < point_cnt);



    lv_draw_rect(&poly_mask, &poly_mask, style);

    lv_draw_mask_remove_custom(mp);

    lv_mem_buf_release(mp);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

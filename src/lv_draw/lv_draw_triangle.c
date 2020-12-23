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
 * Draw a triangle
 * @param points pointer to an array with 3 points
 * @param clip_area the triangle will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_triangle(const lv_point_t points[], const lv_area_t * clip_area, const lv_draw_rect_dsc_t * draw_dsc)
{
    lv_draw_polygon(points, 3, clip_area, draw_dsc);
}

/**
 * Draw a polygon. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param clip_area polygon will be drawn only in this area
 * @param draw_dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_polygon(const lv_point_t points[], uint16_t point_cnt, const lv_area_t * clip_area,
                     const lv_draw_rect_dsc_t * draw_dsc)
{
    if(point_cnt < 3) return;
    if(points == NULL) return;

    /*Join adjacent points if they are on the same coordinate*/
    lv_point_t * p = _lv_mem_buf_get(point_cnt * sizeof(lv_point_t));
    if(p == NULL) return;
    uint16_t i;
    uint16_t pcnt = 0;
    p[0] = points[0];
    for(i = 0; i < point_cnt - 1; i++) {
        if(points[i].x != points[i + 1].x || points[i].y != points[i + 1].y) {
            p[pcnt] = points[i];
            pcnt++;
        }
    }
    /*The first and the last points are also adjacent */
    if(points[0].x != points[point_cnt - 1].x || points[0].y != points[point_cnt - 1].y) {
        p[pcnt] = points[point_cnt - 1];
        pcnt++;
    }

    point_cnt = pcnt;
    if(point_cnt < 3) {
        _lv_mem_buf_release(p);
        return;
    }

    lv_area_t poly_coords = {.x1 = LV_COORD_MAX, .y1 = LV_COORD_MAX, .x2 = LV_COORD_MIN, .y2 = LV_COORD_MIN};

    for(i = 0; i < point_cnt; i++) {
        poly_coords.x1 = LV_MATH_MIN(poly_coords.x1, p[i].x);
        poly_coords.y1 = LV_MATH_MIN(poly_coords.y1, p[i].y);
        poly_coords.x2 = LV_MATH_MAX(poly_coords.x2, p[i].x);
        poly_coords.y2 = LV_MATH_MAX(poly_coords.y2, p[i].y);
    }

    bool is_common;
    lv_area_t poly_mask;
    is_common = _lv_area_intersect(&poly_mask, &poly_coords, clip_area);
    if(!is_common) {
        _lv_mem_buf_release(p);
        return;
    }
    /*Find the lowest point*/
    lv_coord_t y_min = p[0].y;
    int16_t y_min_i = 0;

    for(i = 1; i < point_cnt; i++) {
        if(p[i].y < y_min) {
            y_min = p[i].y;
            y_min_i = i;
        }
    }

    lv_draw_mask_line_param_t * mp = _lv_mem_buf_get(sizeof(lv_draw_mask_line_param_t) * point_cnt);
    lv_draw_mask_line_param_t * mp_next = mp;

    int32_t i_prev_left = y_min_i;
    int32_t i_prev_right = y_min_i;
    int32_t i_next_left;
    int32_t i_next_right;
    uint32_t mask_cnt = 0;

    /*Get the index of the left and right points*/
    i_next_left = y_min_i - 1;
    if(i_next_left < 0) i_next_left = point_cnt + i_next_left;

    i_next_right = y_min_i + 1;
    if(i_next_right > point_cnt - 1) i_next_right = 0;


    /* Check if the order of points is inverted or not.
     * The normal case is when the left point is on `y_min_i - 1`
     * Explanation:
     *   if angle(p_left) < angle(p_right) -> inverted
     *   dy_left/dx_left < dy_right/dx_right
     *   dy_left * dx_right < dy_right * dx_left
     */
    lv_coord_t dxl = p[i_next_left].x - p[y_min_i].x;
    lv_coord_t dxr = p[i_next_right].x - p[y_min_i].x;
    lv_coord_t dyl = p[i_next_left].y - p[y_min_i].y;
    lv_coord_t dyr = p[i_next_right].y - p[y_min_i].y;

    bool inv = false;
    if(dyl * dxr < dyr * dxl) inv = true;

    do {
        if(!inv) {
            i_next_left = i_prev_left - 1;
            if(i_next_left < 0) i_next_left = point_cnt + i_next_left;

            i_next_right = i_prev_right + 1;
            if(i_next_right > point_cnt - 1) i_next_right = 0;
        }
        else {
            i_next_left = i_prev_left + 1;
            if(i_next_left > point_cnt - 1) i_next_left = 0;

            i_next_right = i_prev_right - 1;
            if(i_next_right < 0) i_next_right = point_cnt + i_next_right;
        }

        if(p[i_next_left].y >=  p[i_prev_left].y) {
            if(p[i_next_left].y != p[i_prev_left].y &&
               p[i_next_left].x !=  p[i_prev_left].x) {
                lv_draw_mask_line_points_init(mp_next, p[i_prev_left].x, p[i_prev_left].y,
                                              p[i_next_left].x, p[i_next_left].y,
                                              LV_DRAW_MASK_LINE_SIDE_RIGHT);
                lv_draw_mask_add(mp_next, mp);
                mp_next++;
            }
            mask_cnt++;
            i_prev_left = i_next_left;
        }

        if(mask_cnt == point_cnt) break;

        if(p[i_next_right].y >=  p[i_prev_right].y) {
            if(p[i_next_right].y != p[i_prev_right].y &&
               p[i_next_right].x !=  p[i_prev_right].x) {

                lv_draw_mask_line_points_init(mp_next, p[i_prev_right].x, p[i_prev_right].y,
                                              p[i_next_right].x, p[i_next_right].y,
                                              LV_DRAW_MASK_LINE_SIDE_LEFT);
                lv_draw_mask_add(mp_next, mp);
                mp_next++;
            }
            mask_cnt++;
            i_prev_right = i_next_right;
        }

    } while(mask_cnt < point_cnt);

    lv_draw_rect(&poly_coords, clip_area, draw_dsc);

    lv_draw_mask_remove_custom(mp);

    _lv_mem_buf_release(mp);
    _lv_mem_buf_release(p);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

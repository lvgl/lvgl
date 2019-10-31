/**
 * @file lv_draw_triangle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_triangle.h"
#include "../lv_misc/lv_math.h"

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
 * @param clip the triangle will be drawn only in this mask
 * @param style style for of the triangle
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * clip, const lv_style_t * style, lv_opa_t opa_scale)
{

    lv_area_t clip_tri;
    clip_tri.x1 = LV_MATH_MIN(LV_MATH_MIN(points[0].x, points[1].x), points[2].x);
    clip_tri.x2 = LV_MATH_MAX(LV_MATH_MAX(points[0].x, points[1].x), points[2].x);
    clip_tri.y1 = LV_MATH_MIN(LV_MATH_MIN(points[0].y, points[1].y), points[2].y);
    clip_tri.y2 = LV_MATH_MAX(LV_MATH_MAX(points[0].y, points[1].y), points[2].y);

    bool is_common;
    is_common = lv_area_intersect(&clip_tri, &clip_tri, clip);
    if(!is_common) return;

    lv_draw_mask_line_side_t side_right;
    lv_draw_mask_line_side_t side_left;
    lv_draw_mask_line_side_t side_bottom;

    /*Find the lowest point*/
    lv_coord_t y_min = points[0].y;
    int8_t y_min_i = 0;
    int8_t i;

    for(i = 1; i < 3; i++) {
        if(points[i].y < y_min) {
            y_min = points[i].y;
            y_min_i = i;
        }
    }

    int8_t i_left;
    int8_t i_right;

    switch(y_min_i) {
    case 0:
        i_left = 1;
        i_right = 2;
        break;
    case 1:
        i_left = 0;
        i_right = 2;
        break;
    case 2:
        i_left = 0;
        i_right = 1;
        break;
    default:
        /*Shouldn't happen, but GCC will complain if we don't handle it*/
        LV_LOG_WARN("lv_draw_triangle: unexpected condition for y_min_i");
        return; /*Bail here*/
    }

    if(points[i_right].x < points[i_left].x) {
        int8_t tmp;
        tmp = i_right;
        i_right = i_left;
        i_left = tmp;
    }

    if(points[y_min_i].y == points[i_right].y) side_right = LV_DRAW_MASK_LINE_SIDE_BOTTOM;
    else side_right = LV_DRAW_MASK_LINE_SIDE_LEFT;

    if(points[y_min_i].y == points[i_left].y) side_left = LV_DRAW_MASK_LINE_SIDE_BOTTOM;
    else side_left = LV_DRAW_MASK_LINE_SIDE_RIGHT;

    side_bottom = LV_DRAW_MASK_LINE_SIDE_TOP;

    lv_draw_mask_param_t p_right;
    lv_draw_mask_param_t p_left;
    lv_draw_mask_param_t p_bottom;

    lv_draw_mask_line_points_init(&p_right, points[y_min_i].x, points[y_min_i].y,
                                            points[i_right].x, points[i_right].y, side_right);

    lv_draw_mask_line_points_init(&p_left, points[y_min_i].x, points[y_min_i].y,
                                            points[i_left].x, points[i_left].y, side_left);

    lv_draw_mask_line_points_init(&p_bottom, points[i_left].x, points[i_left].y,
                                            points[i_right].x, points[i_right].y, side_bottom);


    int16_t id_right = lv_draw_mask_add(&p_right, NULL);
    int16_t id_left = lv_draw_mask_add(&p_left, NULL);
    int16_t id_bottom = lv_draw_mask_add(&p_bottom, NULL);


    lv_draw_rect(&clip_tri, &clip_tri, style, opa_scale);

    lv_draw_mask_remove_id(id_right);
    lv_draw_mask_remove_id(id_left);
    lv_draw_mask_remove_id(id_bottom);

}

/**
 * Draw a polygon from triangles. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param mask polygon will be drawn only in this mask
 * @param style style of the polygon
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_polygon(const lv_point_t * points, uint16_t point_cnt, const lv_area_t * mask, const lv_style_t * style,
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
    is_common = lv_area_intersect(&poly_mask, &poly_mask, mask);
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


    lv_draw_mask_param_t * mp = lv_draw_buf_get(sizeof(lv_draw_mask_param_t) * point_cnt);
    lv_draw_mask_param_t * mp_next = mp;

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



    lv_draw_rect(&poly_mask, &poly_mask, style, opa_scale);

    lv_draw_mask_remove_custom(mp);

    lv_draw_buf_release(mp);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

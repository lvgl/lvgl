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
static void point_swap(lv_point_t * p1, lv_point_t * p2);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if USE_LV_TRIANGLE != 0
/**
 *
 * @param points pointer to an array with 3 points
 * @param mask the triangle will be drawn only in this mask
 * @param color color of the triangle
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * mask, lv_color_t color)
{
    lv_point_t tri[3];

    memcpy(tri, points, sizeof(tri));

    /*Sort the vertices according to their y coordinate (0: y max, 1: y mid, 2:y min)*/
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);
    if(tri[2].y < tri[1].y) point_swap(&tri[2], &tri[1]);
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);

    /*Return is the triangle is degenerated*/
    if(tri[0].x == tri[1].x && tri[0].y == tri[1].y) return;
    if(tri[1].x == tri[2].x && tri[1].y == tri[2].y) return;
    if(tri[0].x == tri[2].x && tri[0].y == tri[2].y) return;

    if(tri[0].x == tri[1].x && tri[1].x == tri[2].x) return;
    if(tri[0].y == tri[1].y && tri[1].y == tri[2].y) return;

    /*Draw the triangle*/
    lv_point_t edge1;
    lv_coord_t dx1 = LV_MATH_ABS(tri[0].x - tri[1].x);
    lv_coord_t sx1 = tri[0].x < tri[1].x ? 1 : -1;
    lv_coord_t dy1 = LV_MATH_ABS(tri[0].y - tri[1].y);
    lv_coord_t sy1 = tri[0].y < tri[1].y ? 1 : -1;
    lv_coord_t err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
    lv_coord_t err_tmp1;

    lv_point_t edge2;
    lv_coord_t dx2 = LV_MATH_ABS(tri[0].x - tri[2].x);
    lv_coord_t sx2 = tri[0].x < tri[2].x ? 1 : -1;
    lv_coord_t dy2 = LV_MATH_ABS(tri[0].y - tri[2].y);
    lv_coord_t sy2 = tri[0].y < tri[2].y ? 1 : -1;
    lv_coord_t err2 = (dx1 > dy2 ? dx2 : -dy2) / 2;
    lv_coord_t err_tmp2;

    lv_coord_t y1_tmp;
    lv_coord_t y2_tmp;

    edge1.x = tri[0].x;
    edge1.y = tri[0].y;
    edge2.x = tri[0].x;
    edge2.y = tri[0].y;
    lv_area_t act_area;
    lv_area_t draw_area;

    while(1) {
        act_area.x1 = edge1.x;
        act_area.x2 = edge2.x ;
        act_area.y1 = edge1.y;
        act_area.y2 = edge2.y ;


        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2);
        draw_area.x2--; /*Do not draw most right pixel because it will be drawn by the adjacent triangle*/
        fill_fp(&draw_area, mask, color, LV_OPA_50);

        /*Calc. the next point of edge1*/
        y1_tmp = edge1.y;
        do {
            if(edge1.x == tri[1].x && edge1.y == tri[1].y) {

                dx1 = LV_MATH_ABS(tri[1].x - tri[2].x);
                sx1 = tri[1].x < tri[2].x ? 1 : -1;
                dy1 = LV_MATH_ABS(tri[1].y - tri[2].y);
                sy1 = tri[1].y < tri[2].y ? 1 : -1;
                err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
            } else if(edge1.x == tri[2].x && edge1.y == tri[2].y) return;
            err_tmp1 = err1;
            if(err_tmp1 > -dx1) {
                err1 -= dy1;
                edge1.x += sx1;
            }
            if(err_tmp1 < dy1) {
                err1 += dx1;
                edge1.y += sy1;
            }
        } while(edge1.y == y1_tmp);

        /*Calc. the next point of edge2*/
        y2_tmp = edge2.y;
        do {
            if(edge2.x == tri[2].x && edge2.y == tri[2].y) return;
            err_tmp2 = err2;
            if(err_tmp2 > -dx2) {
                err2 -= dy2;
                edge2.x += sx2;
            }
            if(err_tmp2 < dy2) {
                err2 += dx2;
                edge2.y += sy2;
            }
        } while(edge2.y == y2_tmp);
    }
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/


#if USE_LV_TRIANGLE != 0
/**
 * Swap two points
 * p1 pointer to the first point
 * p2 pointer to the second point
 */
static void point_swap(lv_point_t * p1, lv_point_t * p2)
{
    lv_point_t tmp;
    tmp.x = p1->x;
    tmp.y = p1->y;

    p1->x = p2->x;
    p1->y = p2->y;

    p2->x = tmp.x;
    p2->y = tmp.y;

}

#endif

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
void tri_draw_flat(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa);
void tri_draw_tall(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa);
static void point_swap(lv_point_t * p1, lv_point_t * p2);

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
 * @param mask the triangle will be drawn only in this mask
 * @param style style for of the triangle
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_triangle(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{

    /*Return is the triangle is degenerated*/
    if(points[0].x == points[1].x && points[0].y == points[1].y) return;
    if(points[1].x == points[2].x && points[1].y == points[2].y) return;
    if(points[0].x == points[2].x && points[0].y == points[2].y) return;

    if(points[0].x == points[1].x && points[1].x == points[2].x) return;
    if(points[0].y == points[1].y && points[1].y == points[2].y) return;

    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t)style->body.opa * opa_scale) >> 8;

    /*Is the triangle flat or tall?*/
    lv_coord_t x_min = LV_MATH_MIN(LV_MATH_MIN(points[0].x, points[1].x), points[2].x);
    lv_coord_t x_max = LV_MATH_MAX(LV_MATH_MAX(points[0].x, points[1].x), points[2].x);
    lv_coord_t y_min = LV_MATH_MIN(LV_MATH_MIN(points[0].y, points[1].y), points[2].y);
    lv_coord_t y_max = LV_MATH_MAX(LV_MATH_MAX(points[0].y, points[1].y), points[2].y);

    /* Draw the tall rectangles from vertical lines
     * and from the flat triangles from horizontal lines
     * to minimize the number of lines.
     * Some pixels are overdrawn on the common edges of the triangles
     * so use it only if the triangle has no opacity*/

    /* Draw from horizontal lines*/
    if(x_max - x_min < y_max - y_min) {
        tri_draw_tall(points, mask, style, opa);
    }
    /*Else flat so draw from vertical lines*/
    else {
        tri_draw_flat(points, mask, style, opa);
    }
}

/**
 * Draw a polygon from triangles. Only convex polygons are supported
 * @param points an array of points
 * @param point_cnt number of points
 * @param mask polygon will be drawn only in this mask
 * @param style style of the polygon
 * @param opa_scale scale down all opacities by the factor (0..255)
 */
void lv_draw_polygon(const lv_point_t * points, uint32_t point_cnt, const lv_area_t * mask, const lv_style_t * style,
                     lv_opa_t opa_scale)
{
    if(point_cnt < 3) return;
    if(points == NULL) return;

    uint32_t i;
    lv_point_t tri[3];
    tri[0].x = points[0].x;
    tri[0].y = points[0].y;
    for(i = 0; i < point_cnt - 1; i++) {
        tri[1].x = points[i].x;
        tri[1].y = points[i].y;
        tri[2].x = points[i + 1].x;
        tri[2].y = points[i + 1].y;
        lv_draw_triangle(tri, mask, style, opa_scale);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void tri_draw_flat(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa)
{
    /*Return if the points are out of the mask*/
    if(points[0].x < mask->x1 && points[1].x < mask->x1 && points[2].x < mask->x1) {
        return;
    }

    if(points[0].x > mask->x2 && points[1].x > mask->x2 && points[2].x > mask->x2) {
        return;
    }

    if(points[0].y < mask->y1 && points[1].y < mask->y1 && points[2].y < mask->y1) {
        return;
    }

    if(points[0].y > mask->y2 && points[1].y > mask->y2 && points[2].y > mask->y2) {
        return;
    }

    lv_point_t tri[3];

    memcpy(tri, points, sizeof(tri));

    /*Sort the vertices according to their y coordinate (0: y max, 1: y mid, 2:y min)*/
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);
    if(tri[2].y < tri[1].y) point_swap(&tri[2], &tri[1]);
    if(tri[1].y < tri[0].y) point_swap(&tri[1], &tri[0]);

    /*Draw the triangle*/
    lv_point_t edge1;
    lv_coord_t dx1  = LV_MATH_ABS(tri[0].x - tri[1].x);
    lv_coord_t sx1  = tri[0].x < tri[1].x ? 1 : -1;
    lv_coord_t dy1  = LV_MATH_ABS(tri[0].y - tri[1].y);
    lv_coord_t sy1  = tri[0].y < tri[1].y ? 1 : -1;
    lv_coord_t err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
    lv_coord_t err_tmp1;

    lv_point_t edge2;
    lv_coord_t dx2  = LV_MATH_ABS(tri[0].x - tri[2].x);
    lv_coord_t sx2  = tri[0].x < tri[2].x ? 1 : -1;
    lv_coord_t dy2  = LV_MATH_ABS(tri[0].y - tri[2].y);
    lv_coord_t sy2  = tri[0].y < tri[2].y ? 1 : -1;
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
        act_area.x2 = edge2.x;
        act_area.y1 = edge1.y;
        act_area.y2 = edge2.y;

        /* Get the area of a line.
         * Adjust it a little bit to perfectly match (no redrawn pixels) with the adjacent triangles*/
        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2) + 1;
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2) - 1;
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2) - 1;

        lv_draw_fill(&draw_area, mask, style->body.main_color, opa);

        /*Calc. the next point of edge1*/
        y1_tmp = edge1.y;
        do {
            if(edge1.x == tri[1].x && edge1.y == tri[1].y) {

                dx1  = LV_MATH_ABS(tri[1].x - tri[2].x);
                sx1  = tri[1].x < tri[2].x ? 1 : -1;
                dy1  = LV_MATH_ABS(tri[1].y - tri[2].y);
                sy1  = tri[1].y < tri[2].y ? 1 : -1;
                err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
            } else if(edge1.x == tri[2].x && edge1.y == tri[2].y) {
                return;
            }
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

void tri_draw_tall(const lv_point_t * points, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa)
{
    /*
     * Better to draw from vertical lines
     * |\
     * | |
     * | |
     * |  \
     * |   |
     * |___|
     */

    lv_point_t tri[3];

    memcpy(tri, points, sizeof(tri));

    /*Sort the vertices according to their x coordinate (0: x max, 1: x mid, 2:x min)*/
    if(tri[1].x < tri[0].x) point_swap(&tri[1], &tri[0]);
    if(tri[2].x < tri[1].x) point_swap(&tri[2], &tri[1]);
    if(tri[1].x < tri[0].x) point_swap(&tri[1], &tri[0]);

    /*Draw the triangle*/
    lv_point_t edge1;
    lv_coord_t dx1  = LV_MATH_ABS(tri[0].x - tri[1].x);
    lv_coord_t sx1  = tri[0].x < tri[1].x ? 1 : -1;
    lv_coord_t dy1  = LV_MATH_ABS(tri[0].y - tri[1].y);
    lv_coord_t sy1  = tri[0].y < tri[1].y ? 1 : -1;
    lv_coord_t err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
    lv_coord_t err_tmp1;

    lv_point_t edge2;
    lv_coord_t dx2  = LV_MATH_ABS(tri[0].x - tri[2].x);
    lv_coord_t sx2  = tri[0].x < tri[2].x ? 1 : -1;
    lv_coord_t dy2  = LV_MATH_ABS(tri[0].y - tri[2].y);
    lv_coord_t sy2  = tri[0].y < tri[2].y ? 1 : -1;
    lv_coord_t err2 = (dx1 > dy2 ? dx2 : -dy2) / 2;
    lv_coord_t err_tmp2;

    lv_coord_t x1_tmp;
    lv_coord_t x2_tmp;

    edge1.x = tri[0].x;
    edge1.y = tri[0].y;
    edge2.x = tri[0].x;
    edge2.y = tri[0].y;
    lv_area_t act_area;
    lv_area_t draw_area;

    while(1) {
        act_area.x1 = edge1.x;
        act_area.x2 = edge2.x;
        act_area.y1 = edge1.y;
        act_area.y2 = edge2.y;

        draw_area.x1 = LV_MATH_MIN(act_area.x1, act_area.x2);
        draw_area.x2 = LV_MATH_MAX(act_area.x1, act_area.x2);
        draw_area.y1 = LV_MATH_MIN(act_area.y1, act_area.y2);
        draw_area.y2 = LV_MATH_MAX(act_area.y1, act_area.y2) - 1;

        lv_draw_fill(&draw_area, mask, style->body.main_color, opa);

        /*Calc. the next point of edge1*/
        x1_tmp = edge1.x;
        do {
            if(edge1.y == tri[1].y && edge1.x == tri[1].x) {

                dx1  = LV_MATH_ABS(tri[1].x - tri[2].x);
                sx1  = tri[1].x < tri[2].x ? 1 : -1;
                dy1  = LV_MATH_ABS(tri[1].y - tri[2].y);
                sy1  = tri[1].y < tri[2].y ? 1 : -1;
                err1 = (dx1 > dy1 ? dx1 : -dy1) / 2;
            } else if(edge1.y == tri[2].y && edge1.x == tri[2].x) {
                return;
            }
            err_tmp1 = err1;
            if(err_tmp1 > -dx1) {
                err1 -= dy1;
                edge1.x += sx1;
            }
            if(err_tmp1 < dy1) {
                err1 += dx1;
                edge1.y += sy1;
            }
        } while(edge1.x == x1_tmp);

        /*Calc. the next point of edge2*/
        x2_tmp = edge2.x;
        do {
            if(edge2.y == tri[2].y && edge2.x == tri[2].x) {
                return;
            }

            err_tmp2 = err2;
            if(err_tmp2 > -dx2) {
                err2 -= dy2;
                edge2.x += sx2;
            }
            if(err_tmp2 < dy2) {
                err2 += dx2;
                edge2.y += sy2;
            }
        } while(edge2.x == x2_tmp);
    }
}

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

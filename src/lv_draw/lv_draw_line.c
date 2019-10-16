/**lip
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "lv_draw_mask.h"
#include "lv_draw_blend.h"
#include "../lv_core/lv_refr.h"
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
static void draw_line_skew(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale);
static void draw_line_hor(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale);
static void draw_line_ver(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale);


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
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param mask the line will be drawn only on this area
 * @param style pointer to a line's style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    if(style->line.width == 0) return;
    if(point1->x == point2->x && point1->y == point2->y) return;

    lv_area_t clip_line;
    clip_line.x1 = LV_MATH_MIN(point1->x, point2->x) - style->line.width/2;
    clip_line.x2 = LV_MATH_MAX(point1->x, point2->x) + style->line.width/2;
    clip_line.y1 = LV_MATH_MIN(point1->y, point2->y) - style->line.width/2;
    clip_line.y2 = LV_MATH_MAX(point1->y, point2->y) + style->line.width/2;

    bool is_common;
    is_common = lv_area_intersect(&clip_line, &clip_line, clip);
    if(!is_common) return;

    if(point1->y == point2->y) draw_line_hor(point1, point2, &clip_line, style, opa_scale);
    else if(point1->x == point2->x) draw_line_ver(point1, point2, &clip_line, style, opa_scale);
    else draw_line_skew(point1, point2, &clip_line, style, opa_scale);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_line_hor(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->line.opa;
    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    lv_coord_t w = style->line.width - 1;
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/


    int16_t other_mask_cnt = lv_draw_mask_get_cnt();

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(point1->x, point2->x);
    draw_area.x2 = LV_MATH_MAX(point1->x, point2->x)  - 1;
    draw_area.y1 = point1->y - w_half1;
    draw_area.y2 = point1->y + w_half0;

    /*If there is no mask then simply draw a rectangle*/
    if(other_mask_cnt == 0) {
        lv_blend_fill(clip, &draw_area,
                style->line.color, NULL, LV_DRAW_MASK_RES_FULL_COVER,opa,
                LV_BLEND_MODE_NORMAL);
    }
    /*If there other mask apply it*/
    else {
        /* Get clipped fill area which is the real draw area.
         * It is always the same or inside `fill_area` */
        bool is_common;
        is_common = lv_area_intersect(&draw_area, clip, &draw_area);
        if(!is_common) return;

        /* Now `draw_area` has absolute coordinates.
         * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
        draw_area.x1 -= vdb->area.x1;
        draw_area.y1 -= vdb->area.y1;
        draw_area.x2 -= vdb->area.x1;
        draw_area.y2 -= vdb->area.y1;

        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        lv_area_t fill_area;
        fill_area.x1 = draw_area.x1 + disp_area->x1;
        fill_area.x2 = draw_area.x2 + disp_area->x1;
        fill_area.y1 = draw_area.y1 + disp_area->y1;
        fill_area.y2 = fill_area.y1;

        lv_opa_t mask_buf[LV_HOR_RES_MAX];
        lv_coord_t h;
        lv_draw_mask_res_t mask_res;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
             memset(mask_buf, LV_OPA_COVER, draw_area_w);
             mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

             lv_blend_fill(clip, &fill_area,
                      style->line.color, mask_buf, mask_res, style->line.opa,
                      style->line.blend_mode);

             fill_area.y1++;
             fill_area.y2++;
         }
    }
}


static void draw_line_ver(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->line.opa;
    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    lv_coord_t w = style->line.width - 1;
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/


    int16_t other_mask_cnt = lv_draw_mask_get_cnt();

    lv_area_t draw_area;
    draw_area.x1 = point1->x - w_half1;
    draw_area.x2 = point1->x + w_half0;
    draw_area.y1 = LV_MATH_MIN(point1->y, point2->y);
    draw_area.y2 = LV_MATH_MAX(point1->y, point2->y) - 1;

    /*If there is no mask then simply draw a rectangle*/
    if(other_mask_cnt == 0) {

        lv_blend_fill(clip, &draw_area,
                style->line.color,  NULL, LV_DRAW_MASK_RES_FULL_COVER, opa,
                style->line.blend_mode);
    }
    /*If there other mask apply it*/
    else {
        /* Get clipped fill area which is the real draw area.
         * It is always the same or inside `fill_area` */
        bool is_common;
        is_common = lv_area_intersect(&draw_area, clip, &draw_area);
        if(!is_common) return;

        /* Now `draw_area` has absolute coordinates.
         * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
        draw_area.x1 -= vdb->area.x1;
        draw_area.y1 -= vdb->area.y1;
        draw_area.x2 -= vdb->area.x1;
        draw_area.y2 -= vdb->area.y1;

        lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

        lv_area_t fill_area;
        fill_area.x1 = draw_area.x1 + disp_area->x1;
        fill_area.x2 = draw_area.x2 + disp_area->x1;
        fill_area.y1 = draw_area.y1 + disp_area->y1;
        fill_area.y2 = fill_area.y1;

        lv_opa_t mask_buf[LV_HOR_RES_MAX];
        lv_coord_t h;
        lv_draw_mask_res_t mask_res;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
             memset(mask_buf, LV_OPA_COVER, draw_area_w);
             mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

             lv_blend_fill(clip, &fill_area,
                     style->line.color, mask_buf, mask_res, style->line.opa,
                     LV_BLEND_MODE_NORMAL);

             fill_area.y1++;
             fill_area.y2++;
         }
    }
}


static void draw_line_skew(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->line.opa;
    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;

    /*Keep the great y in p1*/
    lv_point_t p1;
    lv_point_t p2;
    if(point1->y < point2->y) {
        p1.y = point1->y;
        p2.y = point2->y;
        p1.x = point1->x;
        p2.x = point2->x;
    } else {
        p1.y = point2->y;
        p2.y = point1->y;
        p1.x = point2->x;
        p2.x = point1->x;
    }

    lv_coord_t xdiff = p2.x - p1.x;
    lv_coord_t ydiff = p2.y - p1.y;
    bool flat = LV_MATH_ABS(xdiff) > LV_MATH_ABS(ydiff) ? true : false;

    static const uint8_t wcorr[] = {
        128, 128, 128, 129, 129, 130, 130, 131,
        132, 133, 134, 135, 137, 138, 140, 141,
        143, 145, 147, 149, 151, 153, 155, 158,
        160, 162, 165, 167, 170, 173, 175, 178,
        181,
    };

    lv_coord_t w = style->line.width;
    lv_coord_t wcorr_i = 0;
    if(flat) wcorr_i = (LV_MATH_ABS(ydiff) << 5) / LV_MATH_ABS(xdiff);
    else wcorr_i = (LV_MATH_ABS(xdiff) << 5) / LV_MATH_ABS(ydiff);

    w = (w * wcorr[wcorr_i]) >> 7;
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(p1.x, p2.x) - w;
    draw_area.x2 = LV_MATH_MAX(p1.x, p2.x) + w;
    draw_area.y1 = LV_MATH_MIN(p1.y, p2.y) - w;
    draw_area.y2 = LV_MATH_MAX(p1.y, p2.y) + w;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    bool is_common = lv_area_intersect(&draw_area, &draw_area, clip);
    if(is_common == false) return;

    lv_draw_mask_param_t mask_left_param;
    lv_draw_mask_param_t mask_right_param;
    lv_draw_mask_param_t mask_top_param;
    lv_draw_mask_param_t mask_bottom_param;

    if(flat) {
        if(xdiff > 0) {
            lv_draw_mask_line_points_init(&mask_left_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0, LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1, LV_DRAW_MASK_LINE_SIDE_RIGHT);
        } else {
            lv_draw_mask_line_points_init(&mask_left_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1, LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0, LV_DRAW_MASK_LINE_SIDE_RIGHT);
        }
    } else {
        lv_draw_mask_line_points_init(&mask_left_param, p1.x + w_half1, p1.y, p2.x + w_half1, p2.y, LV_DRAW_MASK_LINE_SIDE_LEFT);
        lv_draw_mask_line_points_init(&mask_right_param, p1.x - w_half0, p1.y, p2.x - w_half0, p2.y, LV_DRAW_MASK_LINE_SIDE_RIGHT);
    }

    /*Use the normal vector for the endings*/
    lv_draw_mask_line_points_init(&mask_top_param, p1.x, p1.y, p1.x - ydiff, p1.y + xdiff, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    lv_draw_mask_line_points_init(&mask_bottom_param, p2.x, p2.y,p2.x - ydiff, p2.y + xdiff,  LV_DRAW_MASK_LINE_SIDE_TOP);

    int16_t mask_left_id = lv_draw_mask_add(&mask_left_param, NULL);
    int16_t mask_right_id = lv_draw_mask_add(&mask_right_param, NULL);
    int16_t mask_top_id = lv_draw_mask_add(&mask_top_param, NULL);
    int16_t mask_bottom_id = lv_draw_mask_add(&mask_bottom_param, NULL);

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    /*Store the coordinates of the `draw_a` relative to the VDB */
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    lv_coord_t draw_area_w = lv_area_get_width(&draw_area);

    /*Draw the background line by line*/
    lv_coord_t h;
    lv_draw_mask_res_t mask_res;
    lv_opa_t mask_buf[LV_HOR_RES_MAX];
    lv_area_t fill_area;
    fill_area.x1 = draw_area.x1 + disp_area->x1;
    fill_area.x2 = draw_area.x2 + disp_area->x1;
    fill_area.y1 = draw_area.y1 + disp_area->y1;
    fill_area.y2 = fill_area.y1;

    /*Fill the first row with 'color'*/
    for(h = draw_area.y1; h <= draw_area.y2; h++) {
        memset(mask_buf, LV_OPA_COVER, draw_area_w);
         mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

         lv_blend_fill(clip, &fill_area,
                 style->line.color, mask_buf, mask_res, opa,
                 style->line.blend_mode);

         fill_area.y1++;
         fill_area.y2++;
    }

    lv_draw_mask_remove_id(mask_left_id);
    lv_draw_mask_remove_id(mask_right_id);
    lv_draw_mask_remove_id(mask_top_id);
    lv_draw_mask_remove_id(mask_bottom_id);
}

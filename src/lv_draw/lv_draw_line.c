/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>
#include "lv_draw.h"
#include "lv_mask.h"
#include "lv_blend.h"
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

    if(point1->y == point2->y) draw_line_hor(point1, point2, clip, style, opa_scale);
    else if(point1->x == point2->x) draw_line_ver(point1, point2, clip, style, opa_scale);
    else draw_line_skew(point1, point2, clip, style, opa_scale);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_line_hor(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

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

    lv_coord_t w = style->line.width - 1;
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t draw_a;
    draw_a.x1 = LV_MATH_MIN(p1.x, p2.x);
    draw_a.x2 = LV_MATH_MAX(p1.x, p2.x);
    draw_a.y1 = LV_MATH_MIN(p1.y, p2.y) - w_half1;
    draw_a.y2 = LV_MATH_MAX(p1.y, p2.y) + w_half0;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    bool union_ok = lv_area_intersect(&draw_a, &draw_a, clip);

    /*If there are common part of `clip` and `vdb` then draw*/
    if(union_ok == false) return;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Store the coordinates of the `draw_a` relative to the VDB */
    lv_area_t draw_rel_a;
    draw_rel_a.x1 = draw_a.x1 - vdb->area.x1;
    draw_rel_a.y1 = draw_a.y1 - vdb->area.y1;
    draw_rel_a.x2 = draw_a.x2 - vdb->area.x1;
    draw_rel_a.y2 = draw_a.y2 - vdb->area.y1;

    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    uint32_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;

    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    /*Draw line by line*/
    lv_coord_t h;
    lv_mask_res_t mask_res;
    /*Fill the first row with 'color'*/
    for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
        memset(mask_buf, LV_OPA_COVER, draw_a_width);
        mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
        lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                style->line.color, LV_IMG_CF_TRUE_COLOR,
                mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
        vdb_buf_tmp += vdb_width;
    }
}


static void draw_line_ver(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

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

    lv_coord_t w = style->line.width - 1;
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t draw_a;
    draw_a.x1 = LV_MATH_MIN(p1.x, p2.x) - w_half0;
    draw_a.x2 = LV_MATH_MAX(p1.x, p2.x) + w_half1;
    draw_a.y1 = LV_MATH_MIN(p1.y, p2.y);
    draw_a.y2 = LV_MATH_MAX(p1.y, p2.y);

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    bool union_ok = lv_area_intersect(&draw_a, &draw_a, clip);

    /*If there are common part of `clip` and `vdb` then draw*/
    if(union_ok == false) return;

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Store the coordinates of the `draw_a` relative to the VDB */
    lv_area_t draw_rel_a;
    draw_rel_a.x1 = draw_a.x1 - vdb->area.x1;
    draw_rel_a.y1 = draw_a.y1 - vdb->area.y1;
    draw_rel_a.x2 = draw_a.x2 - vdb->area.x1;
    draw_rel_a.y2 = draw_a.y2 - vdb->area.y1;

    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    uint32_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;

    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    /*Draw the background line by line*/
    lv_coord_t h;
    lv_mask_res_t mask_res;
    /*Fill the first row with 'color'*/
    for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
        memset(mask_buf, LV_OPA_COVER, draw_a_width);
        mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
        lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                style->line.color, LV_IMG_CF_TRUE_COLOR,
                mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
        vdb_buf_tmp += vdb_width;
    }
}


static void draw_line_skew(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
        const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = style->body.opa;

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
    printf("w:%d\n", w);
    lv_coord_t w_half0 = w >> 1;
    lv_coord_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t draw_a;
    draw_a.x1 = LV_MATH_MIN(p1.x, p2.x) - w;
    draw_a.x2 = LV_MATH_MAX(p1.x, p2.x) + w;
    draw_a.y1 = LV_MATH_MIN(p1.y, p2.y) - w;
    draw_a.y2 = LV_MATH_MAX(p1.y, p2.y) + w;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    bool union_ok = lv_area_intersect(&draw_a, &draw_a, clip);

    /*If there are common part of `clip` and `vdb` then draw*/
    if(union_ok == false) return;

    lv_mask_param_t mask_left_param;
    lv_mask_param_t mask_right_param;
    lv_mask_param_t mask_top_param;
    lv_mask_param_t mask_bottom_param;

    if(flat) {
        if(xdiff > 0) {
            lv_mask_line_points_init(&mask_left_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0, LV_LINE_MASK_SIDE_LEFT);
            lv_mask_line_points_init(&mask_right_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1, LV_LINE_MASK_SIDE_RIGHT);
        } else {
            lv_mask_line_points_init(&mask_left_param, p1.x, p1.y + w_half0, p2.x, p2.y + w_half0, LV_LINE_MASK_SIDE_LEFT);
            lv_mask_line_points_init(&mask_right_param, p1.x, p1.y - w_half1, p2.x, p2.y - w_half1, LV_LINE_MASK_SIDE_RIGHT);
        }
    } else {
        lv_mask_line_points_init(&mask_left_param, p1.x + w_half0, p1.y, p2.x + w_half0, p2.y, LV_LINE_MASK_SIDE_LEFT);
        lv_mask_line_points_init(&mask_right_param, p1.x - w_half1, p1.y, p2.x - w_half1, p2.y, LV_LINE_MASK_SIDE_RIGHT);

    }
    /*Use the normal vector for the endings*/
    lv_mask_line_points_init(&mask_top_param, p1.x, p1.y, p1.x - ydiff, p1.y + xdiff, LV_LINE_MASK_SIDE_BOTTOM);
    lv_mask_line_points_init(&mask_bottom_param, p2.x, p2.y,p2.x - ydiff, p2.y + xdiff,  LV_LINE_MASK_SIDE_TOP);

    int16_t mask_left_id = lv_mask_add(lv_mask_line, &mask_left_param, NULL);
    int16_t mask_right_id = lv_mask_add(lv_mask_line, &mask_right_param, NULL);
    int16_t mask_top_id = lv_mask_add(lv_mask_line, &mask_top_param, NULL);
    int16_t mask_bottom_id = lv_mask_add(lv_mask_line, &mask_bottom_param, NULL);

    lv_disp_t * disp    = lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /*Store the coordinates of the `draw_a` relative to the VDB */
    lv_area_t draw_rel_a;
    draw_rel_a.x1 = draw_a.x1 - vdb->area.x1;
    draw_rel_a.y1 = draw_a.y1 - vdb->area.y1;
    draw_rel_a.x2 = draw_a.x2 - vdb->area.x1;
    draw_rel_a.y2 = draw_a.y2 - vdb->area.y1;

    uint32_t vdb_width       = lv_area_get_width(&vdb->area);
    uint32_t draw_a_width    = lv_area_get_width(&draw_rel_a);

    /*Move the vdb_buf_tmp to the first row*/
    lv_color_t * vdb_buf_tmp = vdb->buf_act;
    vdb_buf_tmp += vdb_width * draw_rel_a.y1;

    lv_opa_t mask_buf[LV_HOR_RES_MAX];

    /*Draw the background line by line*/
    lv_coord_t h;
    lv_mask_res_t mask_res;
    /*Fill the first row with 'color'*/
    for(h = draw_rel_a.y1; h <= draw_rel_a.y2; h++) {
        memset(mask_buf, LV_OPA_COVER, draw_a_width);
        mask_res = lv_mask_apply(mask_buf, vdb->area.x1 + draw_rel_a.x1, vdb->area.y1 + h, draw_a_width);
        lv_blend_color(&vdb_buf_tmp[draw_rel_a.x1], LV_IMG_CF_TRUE_COLOR, draw_a_width,
                style->line.color, LV_IMG_CF_TRUE_COLOR,
                mask_buf, mask_res, style->body.opa, LV_BLIT_MODE_NORMAL);
        vdb_buf_tmp += vdb_width;
    }

    lv_mask_remove_id(mask_left_id);
    lv_mask_remove_id(mask_right_id);
    lv_mask_remove_id(mask_top_id);
    lv_mask_remove_id(mask_bottom_id);
}

/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include <stdbool.h>
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
LV_ATTRIBUTE_FAST_MEM static void draw_line_skew(const lv_point_t * point1, const lv_point_t * point2,
                                                 const lv_area_t * clip,
                                                 const lv_draw_line_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void draw_line_hor(const lv_point_t * point1, const lv_point_t * point2,
                                                const lv_area_t * clip,
                                                const lv_draw_line_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void draw_line_ver(const lv_point_t * point1, const lv_point_t * point2,
                                                const lv_area_t * clip,
                                                const lv_draw_line_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_line_dsc_init(lv_draw_line_dsc_t * dsc)
{
    _lv_memset_00(dsc, sizeof(lv_draw_line_dsc_t));
    dsc->width = 1;
    dsc->opa = LV_OPA_COVER;
    dsc->color = LV_COLOR_BLACK;
}

/**
 * Draw a line
 * @param point1 first point of the line
 * @param point2 second point of the line
 * @param clip the line will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_line(const lv_point_t * point1, const lv_point_t * point2, const lv_area_t * clip,
                                        const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    if(point1->x == point2->x && point1->y == point2->y) return;

    lv_area_t clip_line;
    clip_line.x1 = LV_MATH_MIN(point1->x, point2->x) - dsc->width / 2;
    clip_line.x2 = LV_MATH_MAX(point1->x, point2->x) + dsc->width / 2;
    clip_line.y1 = LV_MATH_MIN(point1->y, point2->y) - dsc->width / 2;
    clip_line.y2 = LV_MATH_MAX(point1->y, point2->y) + dsc->width / 2;

    bool is_common;
    is_common = _lv_area_intersect(&clip_line, &clip_line, clip);
    if(!is_common) return;

    if(point1->y == point2->y) draw_line_hor(point1, point2, &clip_line, dsc);
    else if(point1->x == point2->x) draw_line_ver(point1, point2, &clip_line, dsc);
    else draw_line_skew(point1, point2, &clip_line, dsc);

    if(dsc->round_end || dsc->round_start) {
        lv_draw_rect_dsc_t cir_dsc;
        lv_draw_rect_dsc_init(&cir_dsc);
        cir_dsc.bg_color = dsc->color;
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        cir_dsc.bg_opa = dsc->opa;

        int32_t r = (dsc->width >> 1);
        int32_t r_corr = (dsc->width & 1) ? 0 : 1;
        lv_area_t cir_area;

        if(dsc->round_start) {
            cir_area.x1 = point1->x - r;
            cir_area.y1 = point1->y - r;
            cir_area.x2 = point1->x + r - r_corr;
            cir_area.y2 = point1->y + r - r_corr ;
            lv_draw_rect(&cir_area, clip, &cir_dsc);
        }

        if(dsc->round_end) {
            cir_area.x1 = point2->x - r;
            cir_area.y1 = point2->y - r;
            cir_area.x2 = point2->x + r - r_corr;
            cir_area.y2 = point2->y + r - r_corr ;
            lv_draw_rect(&cir_area, clip, &cir_dsc);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_line_hor(const lv_point_t * point1, const lv_point_t * point2,
                                                const lv_area_t * clip,
                                                const lv_draw_line_dsc_t * dsc)
{
    lv_opa_t opa = dsc->opa;

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    bool dashed = dsc->dash_gap && dsc->dash_width ? true : false;

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt()) simple_mode = false;
    else if(dashed) simple_mode = false;

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(point1->x, point2->x);
    draw_area.x2 = LV_MATH_MAX(point1->x, point2->x)  - 1;
    draw_area.y1 = point1->y - w_half1;
    draw_area.y2 = point1->y + w_half0;

    /*If there is no mask then simply draw a rectangle*/
    if(simple_mode) {
        _lv_blend_fill(clip, &draw_area,
                       dsc->color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa,
                       dsc->blend_mode);
    }
    /*If there other mask apply it*/
    else {
        /* Get clipped fill area which is the real draw area.
         * It is always the same or inside `fill_area` */
        bool is_common;
        is_common = _lv_area_intersect(&draw_area, clip, &draw_area);
        if(!is_common) return;

        /* Now `draw_area` has absolute coordinates.
         * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
        draw_area.x1 -= disp_area->x1;
        draw_area.y1 -= disp_area->y1;
        draw_area.x2 -= disp_area->x1;
        draw_area.y2 -= disp_area->y1;

        int32_t draw_area_w = lv_area_get_width(&draw_area);

        lv_area_t fill_area;
        fill_area.x1 = draw_area.x1 + disp_area->x1;
        fill_area.x2 = draw_area.x2 + disp_area->x1;
        fill_area.y1 = draw_area.y1 + disp_area->y1;
        fill_area.y2 = fill_area.y1;

        lv_style_int_t dash_start = 0;
        if(dashed) {
            dash_start = (vdb->area.x1 + draw_area.x1) % (dsc->dash_gap + dsc->dash_width);
        }

        lv_opa_t * mask_buf = _lv_mem_buf_get(draw_area_w);
        int32_t h;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            _lv_memset_ff(mask_buf, draw_area_w);
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            if(dashed) {
                if(mask_res != LV_DRAW_MASK_RES_TRANSP) {
                    lv_style_int_t dash_cnt = dash_start;
                    lv_coord_t i;
                    for(i = 0; i < draw_area_w; i++, dash_cnt++) {
                        if(dash_cnt <= dsc->dash_width) {
                            int16_t diff = dsc->dash_width - dash_cnt;
                            i += diff;
                            dash_cnt += diff;
                        }
                        else if(dash_cnt >= dsc->dash_gap + dsc->dash_width) {
                            dash_cnt = 0;
                        }
                        else {
                            mask_buf[i] = 0x00;
                        }
                    }

                    mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
            }

            _lv_blend_fill(clip, &fill_area,
                           dsc->color, mask_buf, mask_res, dsc->opa,
                           dsc->blend_mode);

            fill_area.y1++;
            fill_area.y2++;
        }
        _lv_mem_buf_release(mask_buf);
    }
}

LV_ATTRIBUTE_FAST_MEM static void draw_line_ver(const lv_point_t * point1, const lv_point_t * point2,
                                                const lv_area_t * clip,
                                                const lv_draw_line_dsc_t * dsc)
{
    lv_opa_t opa = dsc->opa;

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    bool dashed = dsc->dash_gap && dsc->dash_width ? true : false;

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt()) simple_mode = false;
    else if(dashed) simple_mode = false;

    lv_area_t draw_area;
    draw_area.x1 = point1->x - w_half1;
    draw_area.x2 = point1->x + w_half0;
    draw_area.y1 = LV_MATH_MIN(point1->y, point2->y);
    draw_area.y2 = LV_MATH_MAX(point1->y, point2->y) - 1;

    /*If there is no mask then simply draw a rectangle*/
    if(simple_mode) {
        _lv_blend_fill(clip, &draw_area,
                       dsc->color,  NULL, LV_DRAW_MASK_RES_FULL_COVER, opa,
                       dsc->blend_mode);
    }
    /*If there other mask apply it*/
    else {
        /* Get clipped fill area which is the real draw area.
         * It is always the same or inside `fill_area` */
        bool is_common;
        is_common = _lv_area_intersect(&draw_area, clip, &draw_area);
        if(!is_common) return;

        /* Now `draw_area` has absolute coordinates.
         * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
        draw_area.x1 -= vdb->area.x1;
        draw_area.y1 -= vdb->area.y1;
        draw_area.x2 -= vdb->area.x1;
        draw_area.y2 -= vdb->area.y1;

        int32_t draw_area_w = lv_area_get_width(&draw_area);

        lv_area_t fill_area;
        fill_area.x1 = draw_area.x1 + disp_area->x1;
        fill_area.x2 = draw_area.x2 + disp_area->x1;
        fill_area.y1 = draw_area.y1 + disp_area->y1;
        fill_area.y2 = fill_area.y1;

        lv_opa_t * mask_buf = _lv_mem_buf_get(draw_area_w);

        lv_style_int_t dash_start = 0;
        if(dashed) {
            dash_start = (vdb->area.y1 + draw_area.y1) % (dsc->dash_gap + dsc->dash_width);
        }

        lv_style_int_t dash_cnt = dash_start;

        int32_t h;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            _lv_memset_ff(mask_buf, draw_area_w);
            lv_draw_mask_res_t mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            if(dashed) {
                if(mask_res != LV_DRAW_MASK_RES_TRANSP) {
                    if(dash_cnt > dsc->dash_width) {
                        mask_res = LV_DRAW_MASK_RES_TRANSP;
                    }

                    if(dash_cnt >= dsc->dash_gap + dsc->dash_width) {
                        dash_cnt = 0;
                    }
                }
                dash_cnt ++;
            }

            _lv_blend_fill(clip, &fill_area,
                           dsc->color, mask_buf, mask_res, dsc->opa,
                           LV_BLEND_MODE_NORMAL);

            fill_area.y1++;
            fill_area.y2++;
        }
        _lv_mem_buf_release(mask_buf);
    }
}

LV_ATTRIBUTE_FAST_MEM static void draw_line_skew(const lv_point_t * point1, const lv_point_t * point2,
                                                 const lv_area_t * clip,
                                                 const lv_draw_line_dsc_t * dsc)
{
    /*Keep the great y in p1*/
    lv_point_t p1;
    lv_point_t p2;
    if(point1->y < point2->y) {
        p1.y = point1->y;
        p2.y = point2->y;
        p1.x = point1->x;
        p2.x = point2->x;
    }
    else {
        p1.y = point2->y;
        p2.y = point1->y;
        p1.x = point2->x;
        p2.x = point1->x;
    }

    int32_t xdiff = p2.x - p1.x;
    int32_t ydiff = p2.y - p1.y;
    bool flat = LV_MATH_ABS(xdiff) > LV_MATH_ABS(ydiff) ? true : false;

    static const uint8_t wcorr[] = {
        128, 128, 128, 129, 129, 130, 130, 131,
        132, 133, 134, 135, 137, 138, 140, 141,
        143, 145, 147, 149, 151, 153, 155, 158,
        160, 162, 165, 167, 170, 173, 175, 178,
        181,
    };

    int32_t w = dsc->width;
    int32_t wcorr_i = 0;
    if(flat) wcorr_i = (LV_MATH_ABS(ydiff) << 5) / LV_MATH_ABS(xdiff);
    else wcorr_i = (LV_MATH_ABS(xdiff) << 5) / LV_MATH_ABS(ydiff);

    w = (w * wcorr[wcorr_i] + 63) >> 7;     /*+ 63 for rounding*/
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t draw_area;
    draw_area.x1 = LV_MATH_MIN(p1.x, p2.x) - w;
    draw_area.x2 = LV_MATH_MAX(p1.x, p2.x) + w;
    draw_area.y1 = LV_MATH_MIN(p1.y, p2.y) - w;
    draw_area.y2 = LV_MATH_MAX(p1.y, p2.y) + w;

    /* Get the union of `coords` and `clip`*/
    /* `clip` is already truncated to the `vdb` size
     * in 'lv_refr_area' function */
    bool is_common = _lv_area_intersect(&draw_area, &draw_area, clip);
    if(is_common == false) return;

    lv_draw_mask_line_param_t mask_left_param;
    lv_draw_mask_line_param_t mask_right_param;
    lv_draw_mask_line_param_t mask_top_param;
    lv_draw_mask_line_param_t mask_bottom_param;

    if(flat) {
        if(xdiff > 0) {
            lv_draw_mask_line_points_init(&mask_left_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0,
                                          LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1,
                                          LV_DRAW_MASK_LINE_SIDE_RIGHT);
        }
        else {
            lv_draw_mask_line_points_init(&mask_left_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1,
                                          LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0,
                                          LV_DRAW_MASK_LINE_SIDE_RIGHT);
        }
    }
    else {
        lv_draw_mask_line_points_init(&mask_left_param, p1.x + w_half1, p1.y, p2.x + w_half1, p2.y,
                                      LV_DRAW_MASK_LINE_SIDE_LEFT);
        lv_draw_mask_line_points_init(&mask_right_param, p1.x - w_half0, p1.y, p2.x - w_half0, p2.y,
                                      LV_DRAW_MASK_LINE_SIDE_RIGHT);
    }

    /*Use the normal vector for the endings*/

    int16_t mask_left_id = lv_draw_mask_add(&mask_left_param, NULL);
    int16_t mask_right_id = lv_draw_mask_add(&mask_right_param, NULL);
    int16_t mask_top_id = LV_MASK_ID_INV;
    int16_t mask_bottom_id = LV_MASK_ID_INV;

    if(!dsc->raw_end) {
        lv_draw_mask_line_points_init(&mask_top_param, p1.x, p1.y, p1.x - ydiff, p1.y + xdiff, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
        lv_draw_mask_line_points_init(&mask_bottom_param, p2.x, p2.y, p2.x - ydiff, p2.y + xdiff,  LV_DRAW_MASK_LINE_SIDE_TOP);
        mask_top_id = lv_draw_mask_add(&mask_top_param, NULL);
        mask_bottom_id = lv_draw_mask_add(&mask_bottom_param, NULL);
    }

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    const lv_area_t * disp_area = &vdb->area;

    /*Store the coordinates of the `draw_a` relative to the VDB */
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /* The real draw area is around the line.
     * It's easy to calculate with steep lines, but the area can be very wide with very flat lines.
     * So deal with it only with steep lines. */
    int32_t draw_area_w = lv_area_get_width(&draw_area);

    /*Draw the background line by line*/
    int32_t h;
    uint32_t hor_res = (uint32_t)lv_disp_get_hor_res(disp);
    size_t mask_buf_size = LV_MATH_MIN(lv_area_get_size(&draw_area), hor_res);
    lv_opa_t * mask_buf = _lv_mem_buf_get(mask_buf_size);

    lv_area_t fill_area;
    fill_area.x1 = draw_area.x1 + disp_area->x1;
    fill_area.x2 = draw_area.x2 + disp_area->x1;
    fill_area.y1 = draw_area.y1 + disp_area->y1;
    fill_area.y2 = fill_area.y1;

    int32_t x = vdb->area.x1 + draw_area.x1;

    uint32_t mask_p = 0;

    _lv_memset_ff(mask_buf, mask_buf_size);
    /*Fill the first row with 'color'*/
    for(h = draw_area.y1 + disp_area->y1; h <= draw_area.y2 + disp_area->y1; h++) {

        lv_draw_mask_res_t mask_res = lv_draw_mask_apply(&mask_buf[mask_p], x, h, draw_area_w);
        if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
            _lv_memset_00(&mask_buf[mask_p], draw_area_w);
        }

        mask_p += draw_area_w;
        if((uint32_t) mask_p + draw_area_w < mask_buf_size) {
            fill_area.y2 ++;
        }
        else {
            _lv_blend_fill(&fill_area, clip,
                           dsc->color, mask_buf, LV_DRAW_MASK_RES_CHANGED, dsc->opa,
                           dsc->blend_mode);

            fill_area.y1 = fill_area.y2 + 1;
            fill_area.y2 = fill_area.y1;
            mask_p = 0;
            _lv_memset_ff(mask_buf, mask_buf_size);
        }
    }

    /*Flush the last part*/
    if(fill_area.y1 != fill_area.y2) {
        fill_area.y2--;
        _lv_blend_fill(&fill_area, clip,
                       dsc->color, mask_buf, LV_DRAW_MASK_RES_CHANGED, dsc->opa,
                       dsc->blend_mode);

    }

    _lv_mem_buf_release(mask_buf);

    lv_draw_mask_remove_id(mask_left_id);
    lv_draw_mask_remove_id(mask_right_id);
    lv_draw_mask_remove_id(mask_top_id);
    lv_draw_mask_remove_id(mask_bottom_id);
}

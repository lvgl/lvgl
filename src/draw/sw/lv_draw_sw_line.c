/**
 * @file lv_draw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "lv_draw_sw.h"
#include "../../misc/lv_math.h"
#include "../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_line_skew(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                 const lv_point_t * point1, const lv_point_t * point2);
LV_ATTRIBUTE_FAST_MEM static void draw_line_hor(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                const lv_point_t * point1, const lv_point_t * point2);
LV_ATTRIBUTE_FAST_MEM static void draw_line_ver(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                const lv_point_t * point1, const lv_point_t * point2);

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
 * @param clip the line will be drawn only in this area
 * @param dsc pointer to an initialized `lv_draw_line_dsc_t` variable
 */
LV_ATTRIBUTE_FAST_MEM void lv_draw_sw_line(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                           const lv_point_t * point1, const lv_point_t * point2)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    if(point1->x == point2->x && point1->y == point2->y) return;

    lv_area_t clip_line;
    clip_line.x1 = LV_MIN(point1->x, point2->x) - dsc->width / 2;
    clip_line.x2 = LV_MAX(point1->x, point2->x) + dsc->width / 2;
    clip_line.y1 = LV_MIN(point1->y, point2->y) - dsc->width / 2;
    clip_line.y2 = LV_MAX(point1->y, point2->y) + dsc->width / 2;

    bool is_common;
    is_common = _lv_area_intersect(&clip_line, &clip_line, draw_ctx->clip_area);
    if(!is_common) return;
    const lv_area_t * clip_area_ori = draw_ctx->clip_area;
    draw_ctx->clip_area = &clip_line;

    if(point1->y == point2->y) draw_line_hor(draw_ctx, dsc, point1, point2);
    else if(point1->x == point2->x) draw_line_ver(draw_ctx, dsc, point1, point2);
    else draw_line_skew(draw_ctx, dsc, point1, point2);

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
            lv_draw_rect(draw_ctx, &cir_dsc, &cir_area);
        }

        if(dsc->round_end) {
            cir_area.x1 = point2->x - r;
            cir_area.y1 = point2->y - r;
            cir_area.x2 = point2->x + r - r_corr;
            cir_area.y2 = point2->y + r - r_corr ;
            lv_draw_rect(draw_ctx, &cir_dsc, &cir_area);
        }
    }

    draw_ctx->clip_area = clip_area_ori;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


LV_ATTRIBUTE_FAST_MEM static void draw_line_hor(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                const lv_point_t * point1, const lv_point_t * point2)
{
    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t blend_area;
    blend_area.x1 = LV_MIN(point1->x, point2->x);
    blend_area.x2 = LV_MAX(point1->x, point2->x)  - 1;
    blend_area.y1 = point1->y - w_half1;
    blend_area.y2 = point1->y + w_half0;

    bool is_common;
    is_common = _lv_area_intersect(&blend_area, &blend_area, draw_ctx->clip_area);
    if(!is_common) return;

    bool dashed = dsc->dash_gap && dsc->dash_width ? true : false;
    bool simple_mode = true;
    if(lv_draw_mask_is_any(&blend_area)) simple_mode = false;
    else if(dashed) simple_mode = false;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;

    /*If there is no mask then simply draw a rectangle*/
    if(simple_mode) {
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }
#if LV_DRAW_COMPLEX
    /*If there other mask apply it*/
    else {

        int32_t blend_area_w = lv_area_get_width(&blend_area);

        lv_coord_t y2 = blend_area.y2;
        blend_area.y2 = blend_area.y1;

        lv_coord_t dash_start = 0;
        if(dashed) {
            dash_start = (blend_area.x1) % (dsc->dash_gap + dsc->dash_width);
        }

        lv_opa_t * mask_buf = lv_mem_buf_get(blend_area_w);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;
        int32_t h;
        for(h = blend_area.y1; h <= y2; h++) {
            lv_memset_ff(mask_buf, blend_area_w);
            blend_dsc.mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, blend_area_w);

            if(dashed) {
                if(blend_dsc.mask_res != LV_DRAW_MASK_RES_TRANSP) {
                    lv_coord_t dash_cnt = dash_start;
                    lv_coord_t i;
                    for(i = 0; i < blend_area_w; i++, dash_cnt++) {
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

                    blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
            }

            lv_draw_sw_blend(draw_ctx, &blend_dsc);

            blend_area.y1++;
            blend_area.y2++;
        }
        lv_mem_buf_release(mask_buf);
    }
#endif /*LV_DRAW_COMPLEX*/
}

LV_ATTRIBUTE_FAST_MEM static void draw_line_ver(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                const lv_point_t * point1, const lv_point_t * point2)
{
    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t blend_area;
    blend_area.x1 = point1->x - w_half1;
    blend_area.x2 = point1->x + w_half0;
    blend_area.y1 = LV_MIN(point1->y, point2->y);
    blend_area.y2 = LV_MAX(point1->y, point2->y) - 1;

    bool is_common;
    is_common = _lv_area_intersect(&blend_area, &blend_area, draw_ctx->clip_area);
    if(!is_common) return;

    bool dashed = dsc->dash_gap && dsc->dash_width ? true : false;
    bool simple_mode = true;
    if(lv_draw_mask_is_any(&blend_area)) simple_mode = false;
    else if(dashed) simple_mode = false;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;

    /*If there is no mask then simply draw a rectangle*/
    if(simple_mode) {
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }

#if LV_DRAW_COMPLEX
    /*If there other mask apply it*/
    else {
        int32_t draw_area_w = lv_area_get_width(&blend_area);

        lv_coord_t y2 = blend_area.y2;
        blend_area.y2 = blend_area.y1;

        lv_opa_t * mask_buf = lv_mem_buf_get(draw_area_w);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;

        lv_coord_t dash_start = 0;
        if(dashed) {
            dash_start = (blend_area.y1) % (dsc->dash_gap + dsc->dash_width);
        }

        lv_coord_t dash_cnt = dash_start;

        int32_t h;
        for(h = blend_area.y1; h <= y2; h++) {
            lv_memset_ff(mask_buf, draw_area_w);
            blend_dsc.mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, draw_area_w);

            if(dashed) {
                if(blend_dsc.mask_res != LV_DRAW_MASK_RES_TRANSP) {
                    if(dash_cnt > dsc->dash_width) {
                        blend_dsc.mask_res = LV_DRAW_MASK_RES_TRANSP;
                    }

                    if(dash_cnt >= dsc->dash_gap + dsc->dash_width) {
                        dash_cnt = 0;
                    }
                }
                dash_cnt ++;
            }

            lv_draw_sw_blend(draw_ctx, &blend_dsc);

            blend_area.y1++;
            blend_area.y2++;
        }
        lv_mem_buf_release(mask_buf);
    }
#endif /*LV_DRAW_COMPLEX*/
}

LV_ATTRIBUTE_FAST_MEM static void draw_line_skew(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_line_dsc_t * dsc,
                                                 const lv_point_t * point1, const lv_point_t * point2)
{
#if LV_DRAW_COMPLEX
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
    bool flat = LV_ABS(xdiff) > LV_ABS(ydiff) ? true : false;

    static const uint8_t wcorr[] = {
        128, 128, 128, 129, 129, 130, 130, 131,
        132, 133, 134, 135, 137, 138, 140, 141,
        143, 145, 147, 149, 151, 153, 155, 158,
        160, 162, 165, 167, 170, 173, 175, 178,
        181,
    };

    int32_t w = dsc->width;
    int32_t wcorr_i = 0;
    if(flat) wcorr_i = (LV_ABS(ydiff) << 5) / LV_ABS(xdiff);
    else wcorr_i = (LV_ABS(xdiff) << 5) / LV_ABS(ydiff);

    w = (w * wcorr[wcorr_i] + 63) >> 7;     /*+ 63 for rounding*/
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t blend_area;
    blend_area.x1 = LV_MIN(p1.x, p2.x) - w;
    blend_area.x2 = LV_MAX(p1.x, p2.x) + w;
    blend_area.y1 = LV_MIN(p1.y, p2.y) - w;
    blend_area.y2 = LV_MAX(p1.y, p2.y) + w;

    /*Get the union of `coords` and `clip`*/
    /*`clip` is already truncated to the `draw_buf` size
     *in 'lv_refr_area' function*/
    bool is_common = _lv_area_intersect(&blend_area, &blend_area, draw_ctx->clip_area);
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

    /*The real draw area is around the line.
     *It's easy to calculate with steep lines, but the area can be very wide with very flat lines.
     *So deal with it only with steep lines.*/
    int32_t draw_area_w = lv_area_get_width(&blend_area);

    /*Draw the background line by line*/
    int32_t h;
    uint32_t hor_res = (uint32_t)lv_disp_get_hor_res(_lv_refr_get_disp_refreshing());
    size_t mask_buf_size = LV_MIN(lv_area_get_size(&blend_area), hor_res);
    lv_opa_t * mask_buf = lv_mem_buf_get(mask_buf_size);

    lv_coord_t y2 = blend_area.y2;
    blend_area.y2 = blend_area.y1;

    uint32_t mask_p = 0;
    lv_memset_ff(mask_buf, mask_buf_size);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memset_00(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.mask_area = &blend_area;

    /*Fill the first row with 'color'*/
    for(h = blend_area.y1; h <= y2; h++) {
        blend_dsc.mask_res = lv_draw_mask_apply(&mask_buf[mask_p], blend_area.x1, h, draw_area_w);
        if(blend_dsc.mask_res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&mask_buf[mask_p], draw_area_w);
        }

        mask_p += draw_area_w;
        if((uint32_t) mask_p + draw_area_w < mask_buf_size) {
            blend_area.y2 ++;
        }
        else {
            blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
            lv_draw_sw_blend(draw_ctx, &blend_dsc);

            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1;
            mask_p = 0;
            lv_memset_ff(mask_buf, mask_buf_size);
        }
    }

    /*Flush the last part*/
    if(blend_area.y1 != blend_area.y2) {
        blend_area.y2--;
        blend_dsc.mask_res = LV_DRAW_MASK_RES_CHANGED;
        lv_draw_sw_blend(draw_ctx, &blend_dsc);
    }

    lv_mem_buf_release(mask_buf);

    lv_draw_mask_free_param(&mask_left_param);
    lv_draw_mask_free_param(&mask_right_param);
    if(mask_top_id != LV_MASK_ID_INV) lv_draw_mask_free_param(&mask_top_param);
    if(mask_bottom_id != LV_MASK_ID_INV) lv_draw_mask_free_param(&mask_bottom_param);
    lv_draw_mask_remove_id(mask_left_id);
    lv_draw_mask_remove_id(mask_right_id);
    lv_draw_mask_remove_id(mask_top_id);
    lv_draw_mask_remove_id(mask_bottom_id);
#else
    LV_UNUSED(point1);
    LV_UNUSED(point2);
    LV_UNUSED(draw_ctx);
    LV_UNUSED(dsc);
    LV_LOG_WARN("Can't draw skewed line with LV_DRAW_COMPLEX == 0");
#endif /*LV_DRAW_COMPLEX*/
}


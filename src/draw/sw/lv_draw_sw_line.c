/**
 * @file lv_draw_sw_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"

#if LV_USE_DRAW_SW

#include "../../misc/lv_math.h"
#include "../../misc/lv_types.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ draw_line_skew(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);
static void /* LV_ATTRIBUTE_FAST_MEM */ draw_line_hor(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);
static void /* LV_ATTRIBUTE_FAST_MEM */ draw_line_ver(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)
{
    if(dsc->width == 0) return;
    if(dsc->opa <= LV_OPA_MIN) return;

    if(dsc->p1.x == dsc->p2.x && dsc->p1.y == dsc->p2.y) return;

    lv_area_t clip_line;
    clip_line.x1 = (int32_t)LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width / 2;
    clip_line.x2 = (int32_t)LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width / 2;
    clip_line.y1 = (int32_t)LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width / 2;
    clip_line.y2 = (int32_t)LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width / 2;

    bool is_common;
    is_common = lv_area_intersect(&clip_line, &clip_line, &t->clip_area);
    if(!is_common) return;

    LV_PROFILER_DRAW_BEGIN;
    if((int32_t)dsc->p1.y == (int32_t)dsc->p2.y) draw_line_hor(t, dsc);
    else if((int32_t)dsc->p1.x == (int32_t)dsc->p2.x) draw_line_ver(t, dsc);
    else draw_line_skew(t, dsc);

    if(dsc->round_end || dsc->round_start) {
        lv_draw_fill_dsc_t cir_dsc;
        lv_draw_fill_dsc_init(&cir_dsc);
        cir_dsc.color = dsc->color;
        cir_dsc.radius = LV_RADIUS_CIRCLE;
        cir_dsc.opa = dsc->opa;

        int32_t r = (dsc->width >> 1);
        int32_t r_corr = (dsc->width & 1) ? 0 : 1;
        lv_area_t cir_area;

        if(dsc->round_start) {
            cir_area.x1 = (int32_t)dsc->p1.x - r;
            cir_area.y1 = (int32_t)dsc->p1.y - r;
            cir_area.x2 = (int32_t)dsc->p1.x + r - r_corr;
            cir_area.y2 = (int32_t)dsc->p1.y + r - r_corr ;
            lv_draw_sw_fill(t, &cir_dsc, &cir_area);
        }

        if(dsc->round_end) {
            cir_area.x1 = (int32_t)dsc->p2.x - r;
            cir_area.y1 = (int32_t)dsc->p2.y - r;
            cir_area.x2 = (int32_t)dsc->p2.x + r - r_corr;
            cir_area.y2 = (int32_t)dsc->p2.y + r - r_corr ;
            lv_draw_sw_fill(t, &cir_dsc, &cir_area);
        }
    }
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void LV_ATTRIBUTE_FAST_MEM draw_line_hor(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)
{
    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t blend_area;
    blend_area.x1 = (int32_t)LV_MIN(dsc->p1.x, dsc->p2.x);
    blend_area.x2 = (int32_t)LV_MAX(dsc->p1.x, dsc->p2.x)  - 1;
    blend_area.y1 = (int32_t)dsc->p1.y - w_half1;
    blend_area.y2 = (int32_t)dsc->p1.y + w_half0;

    bool is_common;
    is_common = lv_area_intersect(&blend_area, &blend_area, &t->clip_area);
    if(!is_common) return;

    bool dashed = dsc->dash_gap && dsc->dash_width;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;

    /*If there is no mask then simply draw a rectangle*/
    if(!dashed) {
        lv_draw_sw_blend(t, &blend_dsc);
    }
#if LV_DRAW_SW_COMPLEX
    /*If there other mask apply it*/
    else {

        int32_t blend_area_w = lv_area_get_width(&blend_area);

        int32_t y2 = blend_area.y2;
        blend_area.y2 = blend_area.y1;

        int32_t dash_start = blend_area.x1 % (dsc->dash_gap + dsc->dash_width);

        lv_opa_t * mask_buf = lv_malloc(blend_area_w);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        int32_t h;
        for(h = blend_area.y1; h <= y2; h++) {
            lv_memset(mask_buf, 0xff, blend_area_w);

            int32_t dash_cnt = dash_start;
            int32_t i;
            for(i = 0; i < blend_area_w; i++, dash_cnt++) {
                if(dash_cnt <= dsc->dash_width) {
                    int16_t diff = dsc->dash_width - dash_cnt;
                    i += diff;
                    dash_cnt += diff;
                }
                else if(dash_cnt > dsc->dash_gap + dsc->dash_width) {
                    dash_cnt = 0;
                }
                else {
                    mask_buf[i] = 0x00;
                }

                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            }

            lv_draw_sw_blend(t, &blend_dsc);

            blend_area.y1++;
            blend_area.y2++;
        }
        lv_free(mask_buf);
    }
#endif /*LV_DRAW_SW_COMPLEX*/
}

static void LV_ATTRIBUTE_FAST_MEM draw_line_ver(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)
{
    int32_t w = dsc->width - 1;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1); /*Compensate rounding error*/

    lv_area_t blend_area;
    blend_area.x1 = (int32_t)dsc->p1.x - w_half1;
    blend_area.x2 = (int32_t)dsc->p1.x + w_half0;
    blend_area.y1 = (int32_t)LV_MIN(dsc->p1.y, dsc->p2.y);
    blend_area.y2 = (int32_t)LV_MAX(dsc->p1.y, dsc->p2.y) - 1;

    bool is_common;
    is_common = lv_area_intersect(&blend_area, &blend_area, &t->clip_area);
    if(!is_common) return;

    bool dashed = dsc->dash_gap && dsc->dash_width;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;

    /*If there is no mask then simply draw a rectangle*/
    if(!dashed) {
        lv_draw_sw_blend(t, &blend_dsc);
    }

#if LV_DRAW_SW_COMPLEX
    /*If there other mask apply it*/
    else {
        int32_t draw_area_w = lv_area_get_width(&blend_area);

        int32_t y2 = blend_area.y2;
        blend_area.y2 = blend_area.y1;

        lv_opa_t * mask_buf = lv_malloc(draw_area_w);
        blend_dsc.mask_buf = mask_buf;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        int32_t dash_start = (blend_area.y1) % (dsc->dash_gap + dsc->dash_width);

        int32_t dash_cnt = dash_start;

        int32_t h;
        for(h = blend_area.y1; h <= y2; h++) {
            lv_memset(mask_buf, 0xff, draw_area_w);

            if(dash_cnt > dsc->dash_width) {
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_TRANSP;
            }
            else {
                blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_FULL_COVER;
            }

            if(dash_cnt >= dsc->dash_gap + dsc->dash_width) {
                dash_cnt = 0;
            }
            dash_cnt ++;

            lv_draw_sw_blend(t, &blend_dsc);

            blend_area.y1++;
            blend_area.y2++;
        }
        lv_free(mask_buf);
    }
#endif /*LV_DRAW_SW_COMPLEX*/
}

static void LV_ATTRIBUTE_FAST_MEM draw_line_skew(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)
{
#if LV_DRAW_SW_COMPLEX
    /*Keep the great y in p1*/
    lv_point_t p1;
    lv_point_t p2;
    if(dsc->p1.y < dsc->p2.y) {
        p1 = lv_point_from_precise(&dsc->p1);
        p2 = lv_point_from_precise(&dsc->p2);
    }
    else {
        p1 = lv_point_from_precise(&dsc->p2);
        p2 = lv_point_from_precise(&dsc->p1);
    }

    int32_t xdiff = p2.x - p1.x;
    int32_t ydiff = p2.y - p1.y;
    bool flat = LV_ABS(xdiff) > LV_ABS(ydiff);

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
    bool is_common = lv_area_intersect(&blend_area, &blend_area, &t->clip_area);
    if(is_common == false) return;

    lv_draw_sw_mask_line_param_t mask_left_param;
    lv_draw_sw_mask_line_param_t mask_right_param;
    lv_draw_sw_mask_line_param_t mask_top_param;
    lv_draw_sw_mask_line_param_t mask_bottom_param;

    void * masks[5] = {&mask_left_param, & mask_right_param, NULL, NULL, NULL};

    if(flat) {
        if(xdiff > 0) {
            lv_draw_sw_mask_line_points_init(&mask_left_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0,
                                             LV_DRAW_SW_MASK_LINE_SIDE_LEFT);
            lv_draw_sw_mask_line_points_init(&mask_right_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1,
                                             LV_DRAW_SW_MASK_LINE_SIDE_RIGHT);
        }
        else {
            lv_draw_sw_mask_line_points_init(&mask_left_param, p1.x, p1.y + w_half1, p2.x, p2.y + w_half1,
                                             LV_DRAW_SW_MASK_LINE_SIDE_LEFT);
            lv_draw_sw_mask_line_points_init(&mask_right_param, p1.x, p1.y - w_half0, p2.x, p2.y - w_half0,
                                             LV_DRAW_SW_MASK_LINE_SIDE_RIGHT);
        }
    }
    else {
        lv_draw_sw_mask_line_points_init(&mask_left_param, p1.x + w_half1, p1.y, p2.x + w_half1, p2.y,
                                         LV_DRAW_SW_MASK_LINE_SIDE_LEFT);
        lv_draw_sw_mask_line_points_init(&mask_right_param, p1.x - w_half0, p1.y, p2.x - w_half0, p2.y,
                                         LV_DRAW_SW_MASK_LINE_SIDE_RIGHT);

    }

    /*Use the normal vector for the endings*/

    if(!dsc->raw_end) {
        lv_draw_sw_mask_line_points_init(&mask_top_param, p1.x, p1.y, p1.x - ydiff, p1.y + xdiff,
                                         LV_DRAW_SW_MASK_LINE_SIDE_BOTTOM);
        lv_draw_sw_mask_line_points_init(&mask_bottom_param, p2.x, p2.y, p2.x - ydiff, p2.y + xdiff,
                                         LV_DRAW_SW_MASK_LINE_SIDE_TOP);
        masks[2] = &mask_top_param;
        masks[3] = &mask_bottom_param;
    }

    /*The real draw area is around the line.
     *It's easy to calculate with steep lines, but the area can be very wide with very flat lines.
     *So deal with it only with steep lines.*/
    int32_t draw_area_w = lv_area_get_width(&blend_area);

    /*Draw the background line by line*/
    int32_t h;
    uint32_t hor_res = (uint32_t)lv_display_get_horizontal_resolution(lv_refr_get_disp_refreshing());
    size_t mask_buf_size = LV_MIN(lv_area_get_size(&blend_area), hor_res);
    lv_opa_t * mask_buf = lv_malloc(mask_buf_size);

    int32_t y2 = blend_area.y2;
    blend_area.y2 = blend_area.y1;

    uint32_t mask_p = 0;
    lv_memset(mask_buf, 0xff, mask_buf_size);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.mask_area = &blend_area;

    /*Fill the first row with 'color'*/
    for(h = blend_area.y1; h <= y2; h++) {
        blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, &mask_buf[mask_p], blend_area.x1, h, draw_area_w);
        if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_TRANSP) {
            lv_memzero(&mask_buf[mask_p], draw_area_w);
        }

        mask_p += draw_area_w;
        if((uint32_t) mask_p + draw_area_w < mask_buf_size) {
            blend_area.y2 ++;
        }
        else {
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            lv_draw_sw_blend(t, &blend_dsc);

            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1;
            mask_p = 0;
            lv_memset(mask_buf, 0xff, mask_buf_size);
        }
    }

    /*Flush the last part*/
    if(blend_area.y1 != blend_area.y2) {
        blend_area.y2--;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        lv_draw_sw_blend(t, &blend_dsc);
    }

    lv_free(mask_buf);

    lv_draw_sw_mask_free_param(&mask_left_param);
    lv_draw_sw_mask_free_param(&mask_right_param);
    if(!dsc->raw_end) {
        lv_draw_sw_mask_free_param(&mask_top_param);
        lv_draw_sw_mask_free_param(&mask_bottom_param);
    }
#else
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    LV_LOG_WARN("Can't draw skewed line with LV_DRAW_SW_COMPLEX == 0");
#endif /*LV_DRAW_SW_COMPLEX*/
}

#endif /*LV_USE_DRAW_SW*/

/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "../lv_misc/lv_circ.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD  5   /*Circle segment greater then this value will be anti-aliased by a non-linear (cos) opacity mapping*/

#define SHADOW_OPA_EXTRA_PRECISION      8       /*Calculate with 2^x bigger shadow opacity values to avoid rounding errors*/
#define SHADOW_BOTTOM_AA_EXTRA_RADIUS   3       /*Add extra radius with LV_SHADOW_BOTTOM to cover anti-aliased corners*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_draw_rect_main_mid(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_rect_main_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_rect_border_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_rect_border_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);

#if USE_LV_SHADOW && LV_VDB_SIZE
static void lv_draw_shadow(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_shadow_full(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_shadow_bottom(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale);
static void lv_draw_shadow_full_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, const lv_opa_t * map);
#endif

static uint16_t lv_draw_cont_radius_corr(uint16_t r, lv_coord_t w, lv_coord_t h);

#if LV_ANTIALIAS
static lv_opa_t antialias_get_opa_circ(lv_coord_t seg, lv_coord_t px_id, lv_opa_t opa);
#endif

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
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;

#if USE_LV_SHADOW && LV_VDB_SIZE
    if(style->body.shadow.width != 0) {
        lv_draw_shadow(coords, mask, style, opa_scale);
    }
#endif
    if(style->body.empty == 0 && style->body.opa >= LV_OPA_MIN) {
        lv_draw_rect_main_mid(coords, mask, style, opa_scale);

        if(style->body.radius != 0) {
            lv_draw_rect_main_corner(coords, mask, style, opa_scale);
        }
    }

    if(style->body.border.width != 0 && style->body.border.part != LV_BORDER_NONE && style->body.border.opa >= LV_OPA_MIN) {
        lv_draw_rect_border_straight(coords, mask, style, opa_scale);

        if(style->body.radius != 0) {
            lv_draw_rect_border_corner(coords, mask, style, opa_scale);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Draw the middle part (rectangular) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa_scale scale down all opacities by the factor
 */
static void lv_draw_rect_main_mid(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    uint16_t radius = style->body.radius;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    uint8_t mix;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t) style->body.opa * opa_scale) >> 8;

    radius = lv_draw_cont_radius_corr(radius, width, height);

    /*If the radius is too big then there is no body*/
    if(radius > height / 2)  return;

    lv_area_t work_area;
    work_area.x1 = coords->x1;
    work_area.x2 = coords->x2;

    if(mcolor.full == gcolor.full) {
        work_area.y1 = coords->y1 + radius;
        work_area.y2 = coords->y2 - radius;

        if(style->body.radius != 0) {
#if LV_ANTIALIAS
            work_area.y1 += 2;
            work_area.y2 -= 2;
#else
            work_area.y1 += 1;
            work_area.y2 -= 1;
#endif
        }

        fill_fp(&work_area, mask, mcolor, opa);
    } else {
        lv_coord_t row;
        lv_coord_t row_start = coords->y1 + radius;
        lv_coord_t row_end = coords->y2 - radius;
        lv_color_t act_color;

        if(style->body.radius != 0) {
#if LV_ANTIALIAS
            row_start += 2;
            row_end -= 2;
#else
            row_start += 1;
            row_end -= 1;
#endif
        }
        if(row_start < 0) row_start = 0;

        for(row = row_start; row <= row_end; row ++) {
            work_area.y1 = row;
            work_area.y2 = row;
            mix = (uint32_t)((uint32_t)(coords->y2 - work_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);

            fill_fp(&work_area, mask, act_color, opa);
        }
    }
}
/**
 * Draw the top and bottom parts (corners) of a rectangle
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param rects_p pointer to a rectangle style
 * @param opa_scale scale down all opacities by the factor
 */
static void lv_draw_rect_main_corner(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    uint16_t radius = style->body.radius;

    lv_color_t mcolor = style->body.main_color;
    lv_color_t gcolor = style->body.grad_color;
    lv_color_t act_color;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t) style->body.opa * opa_scale) >> 8;
    uint8_t mix;
    lv_coord_t height = lv_area_get_height(coords);
    lv_coord_t width = lv_area_get_width(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    lb_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    rt_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    rb_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    lv_area_t edge_top_area;
    lv_area_t mid_top_area;
    lv_area_t mid_bot_area;
    lv_area_t edge_bot_area;

    lv_point_t cir;
    lv_coord_t cir_tmp;
    lv_circ_init(&cir, &cir_tmp, radius);

    /*Init the areas*/
    lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                lb_origo.y + LV_CIRC_OCT4_Y(cir),
                rb_origo.x + LV_CIRC_OCT1_X(cir),
                rb_origo.y + LV_CIRC_OCT1_Y(cir));

    lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                lb_origo.y + LV_CIRC_OCT3_Y(cir),
                rb_origo.x + LV_CIRC_OCT2_X(cir),
                rb_origo.y + LV_CIRC_OCT2_Y(cir));

    lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                lt_origo.y + LV_CIRC_OCT5_Y(cir),
                rt_origo.x + LV_CIRC_OCT8_X(cir),
                rt_origo.y + LV_CIRC_OCT8_Y(cir));

    lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                lt_origo.y + LV_CIRC_OCT6_Y(cir),
                rt_origo.x + LV_CIRC_OCT7_X(cir),
                rt_origo.y + LV_CIRC_OCT7_Y(cir));
#if LV_ANTIALIAS
    /*Store some internal states for anti-aliasing*/
    lv_coord_t out_y_seg_start = 0;
    lv_coord_t out_y_seg_end = 0;
    lv_coord_t out_x_last = radius;

    lv_color_t aa_color_hor_top;
    lv_color_t aa_color_hor_bottom;
    lv_color_t aa_color_ver;
#endif

    while(lv_circ_cont(&cir)) {
#if LV_ANTIALIAS != 0
        /*New step in y on the outter circle*/
        if(out_x_last != cir.x) {
            out_y_seg_end = cir.y;
            lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
            lv_point_t aa_p;

            aa_p.x = out_x_last;
            aa_p.y = out_y_seg_start;

            mix = (uint32_t)((uint32_t)(radius - out_x_last) * 255) / height;
            aa_color_hor_top = lv_color_mix(gcolor, mcolor, mix);
            aa_color_hor_bottom = lv_color_mix(mcolor, gcolor, mix);

            lv_coord_t i;
            for(i = 0; i  < seg_size; i++) {
                lv_opa_t aa_opa;
                if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                    aa_opa = antialias_get_opa_circ(seg_size, i, opa);
                } else {
                    aa_opa = opa - lv_draw_aa_get_opa(seg_size, i, opa);
                }

                px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, aa_color_hor_bottom, aa_opa);
                px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, aa_color_hor_bottom, aa_opa);
                px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, aa_color_hor_top, aa_opa);
                px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, aa_color_hor_top, aa_opa);

                mix = (uint32_t)((uint32_t)(radius - out_y_seg_start + i) * 255) / height;
                aa_color_ver = lv_color_mix(mcolor, gcolor, mix);
                px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, aa_color_ver, aa_opa);
                px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, aa_color_ver, aa_opa);

                aa_color_ver = lv_color_mix(gcolor, mcolor, mix);
                px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
                px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
            }

            out_x_last = cir.x;
            out_y_seg_start = out_y_seg_end;
        }
#endif
        uint8_t edge_top_refr = 0;
        uint8_t mid_top_refr = 0;
        uint8_t mid_bot_refr = 0;
        uint8_t edge_bot_refr = 0;

        /* If a new row coming draw the previous
         * The y coordinate can remain the same so wait for a new*/
        if(mid_bot_area.y1 != LV_CIRC_OCT4_Y(cir) + lb_origo.y) mid_bot_refr = 1;

        if(edge_bot_area.y1 != LV_CIRC_OCT2_Y(cir) + lb_origo.y) edge_bot_refr = 1;

        if(mid_top_area.y1 != LV_CIRC_OCT8_Y(cir) + lt_origo.y) mid_top_refr = 1;

        if(edge_top_area.y1 != LV_CIRC_OCT7_Y(cir) + lt_origo.y) edge_top_refr = 1;

        /*Draw the areas which are not disabled*/
        if(edge_top_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_top_area, mask, act_color, opa);
        }

        if(mid_top_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_top_area, mask, act_color, opa);
        }

        if(mid_bot_refr != 0) {
            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&mid_bot_area, mask, act_color, opa);
        }

        if(edge_bot_refr != 0) {

            if(mcolor.full == gcolor.full) act_color = mcolor;
            else {
                mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
                act_color = lv_color_mix(mcolor, gcolor, mix);
            }
            fill_fp(&edge_bot_area, mask, act_color, opa);
        }

        /*Save the current coordinates*/
        lv_area_set(&mid_bot_area,  lb_origo.x + LV_CIRC_OCT4_X(cir),
                    lb_origo.y + LV_CIRC_OCT4_Y(cir),
                    rb_origo.x + LV_CIRC_OCT1_X(cir),
                    rb_origo.y + LV_CIRC_OCT1_Y(cir));

        lv_area_set(&edge_bot_area, lb_origo.x + LV_CIRC_OCT3_X(cir),
                    lb_origo.y + LV_CIRC_OCT3_Y(cir),
                    rb_origo.x + LV_CIRC_OCT2_X(cir),
                    rb_origo.y + LV_CIRC_OCT2_Y(cir));

        lv_area_set(&mid_top_area,  lt_origo.x + LV_CIRC_OCT5_X(cir),
                    lt_origo.y + LV_CIRC_OCT5_Y(cir),
                    rt_origo.x + LV_CIRC_OCT8_X(cir),
                    rt_origo.y + LV_CIRC_OCT8_Y(cir));

        lv_area_set(&edge_top_area, lt_origo.x + LV_CIRC_OCT6_X(cir),
                    lt_origo.y + LV_CIRC_OCT6_Y(cir),
                    rt_origo.x + LV_CIRC_OCT7_X(cir),
                    rt_origo.y + LV_CIRC_OCT7_Y(cir));

        lv_circ_next(&cir, &cir_tmp);
    }

    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - edge_top_area.y1)  * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
    fill_fp(&edge_top_area, mask, act_color, opa);

    if(edge_top_area.y1 != mid_top_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - mid_top_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
        }
        fill_fp(&mid_top_area, mask, act_color, opa);
    }

    if(mcolor.full == gcolor.full) act_color = mcolor;
    else {
        mix = (uint32_t)((uint32_t)(coords->y2 - mid_bot_area.y1) * 255) / height;
        act_color = lv_color_mix(mcolor, gcolor, mix);
    }
    fill_fp(&mid_bot_area, mask, act_color, opa);

    if(edge_bot_area.y1 != mid_bot_area.y1) {

        if(mcolor.full == gcolor.full) act_color = mcolor;
        else {
            mix = (uint32_t)((uint32_t)(coords->y2 - edge_bot_area.y1) * 255) / height;
            act_color = lv_color_mix(mcolor, gcolor, mix);
        }
        fill_fp(&edge_bot_area, mask, act_color, opa);
    }


#if LV_ANTIALIAS
    /*The first and the last line is not drawn*/
    edge_top_area.x1 = coords->x1 + radius + 2;
    edge_top_area.x2 = coords->x2 - radius - 2;
    edge_top_area.y1 = coords->y1;
    edge_top_area.y2 = coords->y1;
    fill_fp(&edge_top_area, mask, style->body.main_color, opa);

    edge_top_area.y1 = coords->y2;
    edge_top_area.y2 = coords->y2;
    fill_fp(&edge_top_area, mask, style->body.grad_color, opa);

    /*Last parts of the anti-alias*/
    out_y_seg_end = cir.y;
    lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
    lv_point_t aa_p;

    aa_p.x = out_x_last;
    aa_p.y = out_y_seg_start;

    mix = (uint32_t)((uint32_t)(radius - out_x_last) * 255) / height;
    aa_color_hor_bottom = lv_color_mix(gcolor, mcolor, mix);
    aa_color_hor_top = lv_color_mix(mcolor, gcolor, mix);

    lv_coord_t i;
    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa = opa - lv_draw_aa_get_opa(seg_size, i, opa);
        px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, aa_color_hor_top, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, aa_color_hor_top, aa_opa);
        px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, aa_color_hor_bottom, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, aa_color_hor_bottom, aa_opa);

        mix = (uint32_t)((uint32_t)(radius - out_y_seg_start + i) * 255) / height;
        aa_color_ver = lv_color_mix(mcolor, gcolor, mix);
        px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, aa_color_ver, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, aa_color_ver, aa_opa);

        aa_color_ver = lv_color_mix(gcolor, mcolor, mix);
        px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, aa_color_ver, aa_opa);
    }

    /*In some cases the last pixel is not drawn*/
    if(LV_MATH_ABS(aa_p.x - aa_p.y) == seg_size) {
        aa_p.x = out_x_last;
        aa_p.y = out_x_last;

        mix = (uint32_t)((uint32_t)(out_x_last) * 255) / height;
        aa_color_hor_top = lv_color_mix(gcolor, mcolor, mix);
        aa_color_hor_bottom = lv_color_mix(mcolor, gcolor, mix);

        lv_opa_t aa_opa = opa >> 1;
        px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p), rb_origo.y + LV_CIRC_OCT2_Y(aa_p), mask, aa_color_hor_bottom, aa_opa);
        px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p), lb_origo.y + LV_CIRC_OCT4_Y(aa_p), mask, aa_color_hor_bottom, aa_opa);
        px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p), lt_origo.y + LV_CIRC_OCT6_Y(aa_p), mask, aa_color_hor_top, aa_opa);
        px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p), rt_origo.y + LV_CIRC_OCT8_Y(aa_p), mask, aa_color_hor_top, aa_opa);
    }

#endif


}

/**
 * Draw the straight parts of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask_ the rectangle will be drawn only  on this area
 * @param rstyle pointer to a rectangle style
 * @param opa_scale scale down all opacities by the factor
 */
static void lv_draw_rect_border_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    uint16_t radius = style->body.radius;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    uint16_t bwidth = style->body.border.width;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.border.opa : (uint16_t)((uint16_t) style->body.border.opa * opa_scale) >> 8;
    lv_border_part_t part = style->body.border.part;
    lv_color_t color = style->body.border.color;
    lv_area_t work_area;
    lv_coord_t length_corr = 0;
    lv_coord_t corner_size = 0;

    /*the 0 px border width drawn as 1 px, so decrement the b_width*/
    bwidth--;

    radius = lv_draw_cont_radius_corr(radius, width, height);

    if(radius < bwidth) {
        length_corr = bwidth - radius - LV_ANTIALIAS;
        corner_size = bwidth;
    } else {
        corner_size = radius + LV_ANTIALIAS;
    }

    /*If radius == 0 is a special case*/
    if(style->body.radius == 0) {
        /*Left top corner*/
        if(part & LV_BORDER_TOP) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top corner*/
        if(part & LV_BORDER_RIGHT) {
            work_area.x1 = coords->x2 - bwidth;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1 + (part & LV_BORDER_TOP ? bwidth + 1 : 0);
            work_area.y2 = coords->y2 - (part & LV_BORDER_BOTTOM ? bwidth + 1 : 0);
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom corner*/
        if(part & LV_BORDER_LEFT) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + bwidth;
            work_area.y1 = coords->y1 + (part & LV_BORDER_TOP ? bwidth + 1 : 0);
            work_area.y2 = coords->y2 - (part & LV_BORDER_BOTTOM ? bwidth + 1 : 0);
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom corner*/
        if(part & LV_BORDER_BOTTOM) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }
        return;
    }

    /* Modify the corner_size if corner is drawn */
    corner_size ++;

    /*Depending one which part's are drawn modify the area lengths */
    if(part & LV_BORDER_TOP) work_area.y1 = coords->y1 + corner_size;
    else  work_area.y1 = coords->y1 + radius;

    if(part & LV_BORDER_BOTTOM) work_area.y2 = coords->y2 - corner_size;
    else work_area.y2 = coords->y2 - radius;

    /*Left border*/
    if(part & LV_BORDER_LEFT) {
        work_area.x1 = coords->x1;
        work_area.x2 = work_area.x1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Right border*/
    if(part & LV_BORDER_RIGHT) {
        work_area.x2 = coords->x2;
        work_area.x1 = work_area.x2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    work_area.x1 = coords->x1 + corner_size - length_corr;
    work_area.x2 = coords->x2 - corner_size + length_corr;

    /*Upper border*/
    if(part & LV_BORDER_TOP) {
        work_area.y1 = coords->y1;
        work_area.y2 = coords->y1 + bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Lower border*/
    if(part & LV_BORDER_BOTTOM) {
        work_area.y2 = coords->y2;
        work_area.y1 = work_area.y2 - bwidth;
        fill_fp(&work_area, mask, color, opa);
    }

    /*Draw the a remaining rectangles if the radius is smaller then bwidth */
    if(length_corr != 0) {
        /*Left top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius + LV_ANTIALIAS;
            work_area.y1 = coords->y1 + radius + 1 + LV_ANTIALIAS;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top correction*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - radius - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1 + radius + 1 + LV_ANTIALIAS;
            work_area.y2 = coords->y1 + bwidth;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + radius + LV_ANTIALIAS;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1 - LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom correction*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - radius - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - bwidth;
            work_area.y2 = coords->y2 - radius - 1 - LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }
    }

    /*If radius == 0 one px on the corners are not drawn by main drawer*/
    if(style->body.radius == 0) {
        /*Left top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + LV_ANTIALIAS;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1 + LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right top corner*/
        if(part & (LV_BORDER_TOP | LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y1;
            work_area.y2 = coords->y1 + LV_ANTIALIAS;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Left bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_LEFT)) {
            work_area.x1 = coords->x1;
            work_area.x2 = coords->x1 + LV_ANTIALIAS;
            work_area.y1 = coords->y2 - LV_ANTIALIAS;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }

        /*Right bottom corner*/
        if(part & (LV_BORDER_BOTTOM | LV_BORDER_RIGHT)) {
            work_area.x1 = coords->x2 - LV_ANTIALIAS;
            work_area.x2 = coords->x2;
            work_area.y1 = coords->y2 - LV_ANTIALIAS;
            work_area.y2 = coords->y2;
            fill_fp(&work_area, mask, color, opa);
        }
    }
}


/**
 * Draw the corners of a rectangle border
 * @param coords the coordinates of the original rectangle
 * @param mask the rectangle will be drawn only  on this area
 * @param style pointer to a style
 * @param opa_scale scale down all opacities by the factor
 */
static void lv_draw_rect_border_corner(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style, lv_opa_t opa_scale)
{
    uint16_t radius = style->body.radius ;
    uint16_t bwidth = style->body.border.width;
    lv_color_t color = style->body.border.color;
    lv_border_part_t part = style->body.border.part;
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.border.opa : (uint16_t)((uint16_t) style->body.border.opa * opa_scale) >> 8;
    /*0 px border width drawn as 1 px, so decrement the bwidth*/
    bwidth--;

#if LV_ANTIALIAS
    bwidth--;    /*Because of anti-aliasing the border seems one pixel ticker*/
#endif

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    lv_point_t lt_origo;   /*Left  Top    origo*/
    lv_point_t lb_origo;   /*Left  Bottom origo*/
    lv_point_t rt_origo;   /*Right Top    origo*/
    lv_point_t rb_origo;   /*Left  Bottom origo*/

    lt_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    lb_origo.x = coords->x1 + radius + LV_ANTIALIAS;
    lb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    rt_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rt_origo.y = coords->y1 + radius + LV_ANTIALIAS;

    rb_origo.x = coords->x2 - radius - LV_ANTIALIAS;
    rb_origo.y = coords->y2 - radius - LV_ANTIALIAS;

    lv_point_t cir_out;
    lv_coord_t tmp_out;
    lv_circ_init(&cir_out, &tmp_out, radius);

    lv_point_t cir_in;
    lv_coord_t tmp_in;
    lv_coord_t radius_in = radius - bwidth;

    if(radius_in < 0) {
        radius_in = 0;
    }

    lv_circ_init(&cir_in, &tmp_in, radius_in);

    lv_area_t circ_area;
    lv_coord_t act_w1;
    lv_coord_t act_w2;

#if LV_ANTIALIAS
    /*Store some internal states for anti-aliasing*/
    lv_coord_t out_y_seg_start = 0;
    lv_coord_t out_y_seg_end = 0;
    lv_coord_t out_x_last = radius;


    lv_coord_t in_y_seg_start = 0;
    lv_coord_t in_y_seg_end = 0;
    lv_coord_t in_x_last = radius - bwidth;
#endif

    while(cir_out.y <= cir_out.x) {

        /*Calculate the actual width to avoid overwriting pixels*/
        if(cir_in.y < cir_in.x) {
            act_w1 = cir_out.x - cir_in.x;
            act_w2 = act_w1;
        } else {
            act_w1 = cir_out.x - cir_out.y;
            act_w2 = act_w1 - 1;
        }

#if LV_ANTIALIAS != 0
        /*New step in y on the outter circle*/
        if(out_x_last != cir_out.x) {
            out_y_seg_end = cir_out.y;
            lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
            lv_point_t aa_p;

            aa_p.x = out_x_last;
            aa_p.y = out_y_seg_start;

            lv_coord_t i;
            for(i = 0; i  < seg_size; i++) {
                lv_opa_t aa_opa;

                if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                    aa_opa = antialias_get_opa_circ(seg_size, i, opa);
                } else {
                    aa_opa = opa - lv_draw_aa_get_opa(seg_size, i, opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                    px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                    px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                    px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                }


                if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                    px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                    px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                    px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                }
            }

            out_x_last = cir_out.x;
            out_y_seg_start = out_y_seg_end;
        }

        /*New step in y on the inner circle*/
        if(in_x_last != cir_in.x) {
            in_y_seg_end = cir_out.y;
            lv_coord_t seg_size = in_y_seg_end - in_y_seg_start;
            lv_point_t aa_p;

            aa_p.x = in_x_last;
            aa_p.y = in_y_seg_start;

            lv_coord_t i;
            for(i = 0; i  < seg_size; i++) {
                lv_opa_t aa_opa;

                if(seg_size > CIRCLE_AA_NON_LINEAR_OPA_THRESHOLD) {    /*Use non-linear opa mapping on the first segment*/
                    aa_opa = opa - antialias_get_opa_circ(seg_size, i, opa);
                } else {
                    aa_opa = lv_draw_aa_get_opa(seg_size, i, opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) - 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                    px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                    px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) + 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                }

                if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                    px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                }

                /*Be sure the pixels on the middle are not drawn twice*/
                if(LV_CIRC_OCT1_X(aa_p) - 1 != LV_CIRC_OCT2_X(aa_p) + i) {
                    if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                        px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
                    }

                    if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                        px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) + 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
                    }

                    if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                        px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
                    }

                    if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                        px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) - 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
                    }
                }

            }

            in_x_last = cir_in.x;
            in_y_seg_start = in_y_seg_end;

        }

#endif


        /*Draw the octets to the right bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rb_origo.x + LV_CIRC_OCT1_X(cir_out) - act_w2;
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT1_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT1_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.x2 = rb_origo.x + LV_CIRC_OCT2_X(cir_out);
            circ_area.y1 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out) - act_w1;
            circ_area.y2 = rb_origo.y + LV_CIRC_OCT2_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the left bottom corner*/
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            circ_area.x1 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT3_X(cir_out);
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out) - act_w2;
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT3_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);

            circ_area.x1 = lb_origo.x + LV_CIRC_OCT4_X(cir_out);
            circ_area.x2 = lb_origo.x + LV_CIRC_OCT4_X(cir_out) + act_w1;
            circ_area.y1 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            circ_area.y2 = lb_origo.y + LV_CIRC_OCT4_Y(cir_out);
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the left top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            if(lb_origo.y + LV_CIRC_OCT4_Y(cir_out) > lt_origo.y + LV_CIRC_OCT5_Y(cir_out)) {
                /*Don't draw if the lines are common in the middle*/
                circ_area.x1 = lt_origo.x + LV_CIRC_OCT5_X(cir_out);
                circ_area.x2 = lt_origo.x + LV_CIRC_OCT5_X(cir_out) + act_w2;
                circ_area.y1 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                circ_area.y2 = lt_origo.y + LV_CIRC_OCT5_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }

            circ_area.x1 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.x2 = lt_origo.x + LV_CIRC_OCT6_X(cir_out);
            circ_area.y1 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out);
            circ_area.y2 = lt_origo.y + LV_CIRC_OCT6_Y(cir_out) + act_w1;
            fill_fp(&circ_area, mask, color, opa);
        }

        /*Draw the octets to the right top corner*/
        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            circ_area.x1 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.x2 = rt_origo.x + LV_CIRC_OCT7_X(cir_out);
            circ_area.y1 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out);
            circ_area.y2 = rt_origo.y + LV_CIRC_OCT7_Y(cir_out) + act_w2;
            fill_fp(&circ_area, mask, color, opa);

            /*Don't draw if the lines are common in the middle*/
            if(rb_origo.y + LV_CIRC_OCT1_Y(cir_out) > rt_origo.y + LV_CIRC_OCT8_Y(cir_out)) {
                circ_area.x1 = rt_origo.x + LV_CIRC_OCT8_X(cir_out) - act_w1;
                circ_area.x2 = rt_origo.x + LV_CIRC_OCT8_X(cir_out);
                circ_area.y1 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                circ_area.y2 = rt_origo.y + LV_CIRC_OCT8_Y(cir_out);
                fill_fp(&circ_area, mask, color, opa);
            }
        }
        lv_circ_next(&cir_out, &tmp_out);

        /*The internal circle will be ready faster
         * so check it! */
        if(cir_in.y < cir_in.x) {
            lv_circ_next(&cir_in, &tmp_in);
        }
    }


#if LV_ANTIALIAS != 0

    /*Last parts of the outer anti-alias*/
    out_y_seg_end = cir_out.y;
    lv_coord_t seg_size = out_y_seg_end - out_y_seg_start;
    lv_point_t aa_p;

    aa_p.x = out_x_last;
    aa_p.y = out_y_seg_start;

    lv_coord_t i;
    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa = opa - lv_draw_aa_get_opa(seg_size, i, opa);
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) + 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
            px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
            px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) - 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) - 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
            px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
            px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) + 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
        }
    }

    /*In some cases the last pixel in the outer middle is not drawn*/
    if(LV_MATH_ABS(aa_p.x - aa_p.y) == seg_size) {
        aa_p.x = out_x_last;
        aa_p.y = out_x_last;

        lv_opa_t aa_opa = opa >> 1;

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p), rb_origo.y + LV_CIRC_OCT2_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p), lb_origo.y + LV_CIRC_OCT4_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p), lt_origo.y + LV_CIRC_OCT6_Y(aa_p), mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p), rt_origo.y + LV_CIRC_OCT8_Y(aa_p), mask, style->body.border.color, aa_opa);
        }
    }

    /*Last parts of the inner anti-alias*/
    in_y_seg_end = cir_in.y;
    aa_p.x = in_x_last;
    aa_p.y = in_y_seg_start;
    seg_size = in_y_seg_end - in_y_seg_start;

    for(i = 0; i  < seg_size; i++) {
        lv_opa_t aa_opa =  lv_draw_aa_get_opa(seg_size, i, opa);
        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
            px_fp(rb_origo.x + LV_CIRC_OCT1_X(aa_p) - 1, rb_origo.y + LV_CIRC_OCT1_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
            px_fp(lb_origo.x + LV_CIRC_OCT3_X(aa_p) - i, lb_origo.y + LV_CIRC_OCT3_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
            px_fp(lt_origo.x + LV_CIRC_OCT5_X(aa_p) + 1, lt_origo.y + LV_CIRC_OCT5_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
        }

        if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
            px_fp(rt_origo.x + LV_CIRC_OCT7_X(aa_p) + i, rt_origo.y + LV_CIRC_OCT7_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
        }

        if(LV_CIRC_OCT1_X(aa_p) - 1 != LV_CIRC_OCT2_X(aa_p) + i) {
            if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_RIGHT)) {
                px_fp(rb_origo.x + LV_CIRC_OCT2_X(aa_p) + i, rb_origo.y + LV_CIRC_OCT2_Y(aa_p) - 1, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_BOTTOM) && (part & LV_BORDER_LEFT)) {
                px_fp(lb_origo.x + LV_CIRC_OCT4_X(aa_p) + 1, lb_origo.y + LV_CIRC_OCT4_Y(aa_p) + i, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_TOP) && (part & LV_BORDER_LEFT)) {
                px_fp(lt_origo.x + LV_CIRC_OCT6_X(aa_p) - i, lt_origo.y + LV_CIRC_OCT6_Y(aa_p) + 1, mask, style->body.border.color, aa_opa);
            }

            if((part & LV_BORDER_TOP) && (part & LV_BORDER_RIGHT)) {
                px_fp(rt_origo.x + LV_CIRC_OCT8_X(aa_p) - 1, rt_origo.y + LV_CIRC_OCT8_Y(aa_p) - i, mask, style->body.border.color, aa_opa);
            }
        }
    }

#endif

}

#if USE_LV_SHADOW && LV_VDB_SIZE

/**
 * Draw a shadow
 * @param rect pointer to rectangle object
 * @param mask pointer to a mask area (from the design functions)
 * @param opa_scale scale down all opacities by the factor
 */
static void lv_draw_shadow(const lv_area_t * coords, const lv_area_t * mask, const  lv_style_t * style, lv_opa_t opa_scale)
{
    /* If mask is in the middle of cords do not draw shadow*/
    lv_coord_t radius = style->body.radius;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);
    radius = lv_draw_cont_radius_corr(radius, width, height);
    lv_area_t area_tmp;

    /*Check horizontally without radius*/
    lv_area_copy(&area_tmp, coords);
    area_tmp.x1 += radius;
    area_tmp.x2 -= radius;
    if(lv_area_is_in(mask, &area_tmp) != false) return;

    /*Check vertically without radius*/
    lv_area_copy(&area_tmp, coords);
    area_tmp.y1 += radius;
    area_tmp.y2 -= radius;
    if(lv_area_is_in(mask, &area_tmp) != false) return;

    if(style->body.shadow.type == LV_SHADOW_FULL) {
        lv_draw_shadow_full(coords, mask, style, opa_scale);
    } else if(style->body.shadow.type == LV_SHADOW_BOTTOM) {
        lv_draw_shadow_bottom(coords, mask, style, opa_scale);
    }
}

static void lv_draw_shadow_full(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{

    /* KNOWN ISSUE
     * The algorithm calculates the shadow only above the middle point of the radius (speaking about the left top corner).
     * It causes an error because it doesn't consider how long the straight edge is which effects the value of bottom of the corner shadow.
     * In addition the straight shadow is drawn from the middles point of the radius however
     * the ends of the straight parts still should be effected by the corner shadow.
     * It also causes an issue in opacity. A smaller radius means smaller average shadow opacity.
     * The solution should be to start `line` from `- swidth` and handle if the straight part is short (or zero) and the value is taken from
     * the other corner. `col` also should start from `- swidth`
     */


    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width;

    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);

    radius += LV_ANTIALIAS;

#if LV_COMPILER_VLA_SUPPORTED
    lv_coord_t curve_x[radius + swidth + 1];     /*Stores the 'x' coordinates of a quarter circle.*/
#else
# if LV_HOR_RES > LV_VER_RES
    lv_coord_t curve_x[LV_HOR_RES];
# else
    lv_coord_t curve_x[LV_VER_RES];
# endif
#endif
    memset(curve_x, 0, sizeof(curve_x));
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        curve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        curve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }
    int16_t line;

    int16_t filter_width = 2 * swidth + 1;
#if LV_COMPILER_VLA_SUPPORTED
    uint32_t line_1d_blur[filter_width];
#else
# if LV_HOR_RES > LV_VER_RES
    uint32_t line_1d_blur[LV_HOR_RES];
# else
    uint32_t line_1d_blur[LV_VER_RES];
# endif
#endif
    /*1D Blur horizontally*/
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t) style->body.opa * opa_scale) >> 8;
    for(line = 0; line < filter_width; line++) {
        line_1d_blur[line] = (uint32_t)((uint32_t)(filter_width - line) * (opa * 2)  << SHADOW_OPA_EXTRA_PRECISION) / (filter_width * filter_width);
    }

    uint16_t col;
#if LV_COMPILER_VLA_SUPPORTED
    lv_opa_t line_2d_blur[radius + swidth + 1];
#else
# if LV_HOR_RES > LV_VER_RES
    lv_opa_t line_2d_blur[LV_HOR_RES];
# else
    lv_opa_t line_2d_blur[LV_VER_RES];
# endif
#endif

    lv_point_t point_rt;
    lv_point_t point_rb;
    lv_point_t point_lt;
    lv_point_t point_lb;
    lv_point_t ofs_rb;
    lv_point_t ofs_rt;
    lv_point_t ofs_lb;
    lv_point_t ofs_lt;
    ofs_rb.x = coords->x2 - radius - LV_ANTIALIAS;
    ofs_rb.y = coords->y2 - radius - LV_ANTIALIAS;

    ofs_rt.x = coords->x2 - radius - LV_ANTIALIAS;
    ofs_rt.y = coords->y1 + radius + LV_ANTIALIAS;

    ofs_lb.x = coords->x1 + radius + LV_ANTIALIAS;
    ofs_lb.y = coords->y2 - radius - LV_ANTIALIAS;

    ofs_lt.x = coords->x1 + radius + LV_ANTIALIAS;
    ofs_lt.y = coords->y1 + radius + LV_ANTIALIAS;
    bool line_ready;
    for(line = 0; line <= radius + swidth; line++) {        /*Check all rows and make the 1D blur to 2D*/
        line_ready = false;
        for(col = 0; col <= radius + swidth; col++) {        /*Check all pixels in a 1D blur line (from the origo to last shadow pixel (radius + swidth))*/

            /*Sum the opacities from the lines above and below this 'row'*/
            int16_t line_rel;
            uint32_t px_opa_sum = 0;
            for(line_rel = -swidth; line_rel <= swidth; line_rel ++) {
                /*Get the relative x position of the 'line_rel' to 'line'*/
                int16_t col_rel;
                if(line + line_rel < 0) {                       /*Below the radius, here is the blur of the edge */
                    col_rel = radius - curve_x[line] - col;
                } else if(line + line_rel > radius) {           /*Above the radius, here won't be more 1D blur*/
                    break;
                } else {                                        /*Blur from the curve*/
                    col_rel = curve_x[line + line_rel] - curve_x[line] - col;
                }

                /*Add the value of the 1D blur on 'col_rel' position*/
                if(col_rel < -swidth) {                         /*Outside of the blurred area. */
                    if(line_rel == -swidth) line_ready = true;  /*If no data even on the very first line then it wont't be anything else in this line*/
                    break;                                      /*Break anyway because only smaller 'col_rel' values will come */
                } else if(col_rel > swidth) px_opa_sum += line_1d_blur[0];      /*Inside the not blurred area*/
                else px_opa_sum += line_1d_blur[swidth - col_rel];              /*On the 1D blur (+ swidth to align to the center)*/
            }

            line_2d_blur[col] = px_opa_sum >> SHADOW_OPA_EXTRA_PRECISION;
            if(line_ready) {
                col++;      /*To make this line to the last one ( drawing will go to '< col')*/
                break;
            }

        }

        /*Flush the line*/
        point_rt.x = curve_x[line] + ofs_rt.x + 1;
        point_rt.y = ofs_rt.y - line;

        point_rb.x = curve_x[line] + ofs_rb.x + 1;
        point_rb.y = ofs_rb.y + line;

        point_lt.x = ofs_lt.x -  curve_x[line] - 1;
        point_lt.y = ofs_lt.y - line;

        point_lb.x = ofs_lb.x - curve_x[line] - 1;
        point_lb.y = ofs_lb.y + line;

        uint16_t d;
        for(d = 1; d < col; d++) {

            if(point_lt.x < ofs_lt.x && point_lt.y < ofs_lt.y) {
                px_fp(point_lt.x, point_lt.y, mask, style->body.shadow.color, line_2d_blur[d]);
            }

            if(point_lb.x < ofs_lb.x && point_lb.y > ofs_lb.y) {
                px_fp(point_lb.x, point_lb.y, mask, style->body.shadow.color, line_2d_blur[d]);
            }

            if(point_rt.x > ofs_rt.x && point_rt.y < ofs_rt.y) {
                px_fp(point_rt.x, point_rt.y, mask, style->body.shadow.color, line_2d_blur[d]);
            }

            if(point_rb.x > ofs_rb.x && point_rb.y > ofs_rb.y) {
                px_fp(point_rb.x, point_rb.y, mask, style->body.shadow.color, line_2d_blur[d]);
            }

            point_rb.x++;
            point_lb.x--;

            point_rt.x++;
            point_lt.x--;
        }

        /* Put the first line to the edges too.
         * It is not correct because blur should be done below the corner too
         * but is is simple, fast and gives a good enough result*/
        if(line == 0) lv_draw_shadow_full_straight(coords, mask, style, line_2d_blur);
    }
}


static void lv_draw_shadow_bottom(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);
    radius += LV_ANTIALIAS * SHADOW_BOTTOM_AA_EXTRA_RADIUS;
    swidth += LV_ANTIALIAS;
#if LV_COMPILER_VLA_SUPPORTED
    lv_coord_t curve_x[radius + 1];             /*Stores the 'x' coordinates of a quarter circle.*/
#else
# if LV_HOR_RES > LV_VER_RES
    lv_coord_t curve_x[LV_HOR_RES];
# else
    lv_coord_t curve_x[LV_VER_RES];
# endif
#endif
    lv_point_t circ;
    lv_coord_t circ_tmp;
    lv_circ_init(&circ, &circ_tmp, radius);
    while(lv_circ_cont(&circ)) {
        curve_x[LV_CIRC_OCT1_Y(circ)] = LV_CIRC_OCT1_X(circ);
        curve_x[LV_CIRC_OCT2_Y(circ)] = LV_CIRC_OCT2_X(circ);
        lv_circ_next(&circ, &circ_tmp);
    }

    int16_t col;
#if LV_COMPILER_VLA_SUPPORTED
    lv_opa_t line_1d_blur[swidth];
#else
# if LV_HOR_RES > LV_VER_RES
    lv_opa_t line_1d_blur[LV_HOR_RES];
# else
    lv_opa_t line_1d_blur[LV_VER_RES];
# endif
#endif

    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->body.opa : (uint16_t)((uint16_t) style->body.opa * opa_scale) >> 8;
    for(col = 0; col < swidth; col++) {
        line_1d_blur[col] = (uint32_t)((uint32_t)(swidth - col) * opa / 2) / (swidth);
    }

    lv_point_t point_l;
    lv_point_t point_r;
    lv_area_t area_mid;
    lv_point_t ofs_l;
    lv_point_t ofs_r;

    ofs_l.x = coords->x1 + radius;
    ofs_l.y = coords->y2 - radius + 1 - LV_ANTIALIAS;

    ofs_r.x = coords->x2 - radius;
    ofs_r.y = coords->y2 - radius + 1 - LV_ANTIALIAS;

    for(col = 0; col <= radius; col++) {
        point_l.x = ofs_l.x - col ;
        point_l.y = ofs_l.y + curve_x[col];

        point_r.x = ofs_r.x + col;
        point_r.y = ofs_r.y + curve_x[col];

        lv_opa_t px_opa;
        int16_t diff = col == 0 ? 0 : curve_x[col - 1] - curve_x[col];
        uint16_t d;
        for(d = 0; d < swidth; d++) {
            /*When stepping a pixel in y calculate the average with the pixel from the prev. column to make a blur */
            if(diff == 0) {
                px_opa = line_1d_blur[d];
            } else {
                px_opa = (uint16_t)((uint16_t)line_1d_blur[d] + line_1d_blur[d - diff]) >> 1;
            }
            px_fp(point_l.x, point_l.y, mask, style->body.shadow.color, px_opa);
            point_l.y ++;

            /*Don't overdraw the pixel on the middle*/
            if(point_r.x > ofs_l.x) {
                px_fp(point_r.x, point_r.y, mask, style->body.shadow.color, px_opa);
            }
            point_r.y ++;
        }

    }

    area_mid.x1 = ofs_l.x + 1;
    area_mid.y1 = ofs_l.y + radius;
    area_mid.x2 = ofs_r.x - 1;
    area_mid.y2 = area_mid.y1;

    uint16_t d;
    for(d = 0; d < swidth; d++) {
        fill_fp(&area_mid, mask, style->body.shadow.color, line_1d_blur[d]);
        area_mid.y1 ++;
        area_mid.y2 ++;
    }
}

static void lv_draw_shadow_full_straight(const lv_area_t * coords, const lv_area_t * mask, const lv_style_t * style, const lv_opa_t * map)
{
    lv_coord_t radius = style->body.radius;
    lv_coord_t swidth = style->body.shadow.width;// + LV_ANTIALIAS;
    lv_coord_t width = lv_area_get_width(coords);
    lv_coord_t height = lv_area_get_height(coords);

    radius = lv_draw_cont_radius_corr(radius, width, height);
    radius += LV_ANTIALIAS;

    lv_area_t right_area;
    right_area.x1 = coords->x2 + 1 - LV_ANTIALIAS;
    right_area.y1 = coords->y1 + radius + LV_ANTIALIAS;
    right_area.x2 = right_area.x1;
    right_area.y2 = coords->y2 - radius - LV_ANTIALIAS;

    lv_area_t left_area;
    left_area.x1 = coords->x1 - 1 + LV_ANTIALIAS;
    left_area.y1 = coords->y1 + radius + LV_ANTIALIAS;
    left_area.x2 = left_area.x1;
    left_area.y2 = coords->y2 - radius - LV_ANTIALIAS;

    lv_area_t top_area;
    top_area.x1 = coords->x1 + radius + LV_ANTIALIAS;
    top_area.y1 = coords->y1 - 1 + LV_ANTIALIAS;
    top_area.x2 = coords->x2 - radius - LV_ANTIALIAS;
    top_area.y2 = top_area.y1;

    lv_area_t bottom_area;
    bottom_area.x1 = coords->x1 + radius + LV_ANTIALIAS;
    bottom_area.y1 = coords->y2 + 1 - LV_ANTIALIAS;
    bottom_area.x2 = coords->x2 - radius - LV_ANTIALIAS;
    bottom_area.y2 = bottom_area.y1;

    lv_opa_t opa_act;
    int16_t d;
    for(d = 1 /*+ LV_ANTIALIAS*/; d <= swidth/* - LV_ANTIALIAS*/; d++) {
        opa_act = map[d];

        fill_fp(&right_area, mask, style->body.shadow.color, opa_act);
        right_area.x1++;
        right_area.x2++;

        fill_fp(&left_area, mask, style->body.shadow.color, opa_act);
        left_area.x1--;
        left_area.x2--;

        fill_fp(&top_area, mask, style->body.shadow.color, opa_act);
        top_area.y1--;
        top_area.y2--;

        fill_fp(&bottom_area, mask, style->body.shadow.color, opa_act);
        bottom_area.y1++;
        bottom_area.y2++;
    }
}

#endif


static uint16_t lv_draw_cont_radius_corr(uint16_t r, lv_coord_t w, lv_coord_t h)
{
    if(r >= (w >> 1)) {
        r = (w >> 1);
        if(r != 0) r--;
    }
    if(r >= (h >> 1)) {
        r = (h >> 1);
        if(r != 0) r--;
    }

    if(r > 0) r -= LV_ANTIALIAS;

    return r;
}

#if LV_ANTIALIAS

/**
 * Approximate the opacity for anti-aliasing.
 * Used  the first segment of a circle which is the longest and have the most non-linearity (cos)
 * @param seg length of the line segment
 * @param px_id index of pixel on the line segment
 * @param line_opa opacity of the lien (it will be the max opacity)
 * @return the desired opacity of the pixel
 */
static lv_opa_t antialias_get_opa_circ(lv_coord_t seg, lv_coord_t px_id, lv_opa_t opa)
{
    static const  lv_opa_t opa_map[8] = {250, 242, 221, 196, 163, 122, 74, 18};

    if(seg == 0) return LV_OPA_TRANSP;
    else if(seg == 1) return LV_OPA_80;
    else {

        uint8_t id = (uint32_t)((uint32_t)px_id * (sizeof(opa_map) - 1)) / (seg - 1);
        return (uint32_t)((uint32_t) opa_map[id] * opa) >> 8;

    }

}

#endif

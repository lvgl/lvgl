/**
 * @file lv_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "lv_draw_blend.h"
#include "lv_draw_mask.h"
#include "../lv_misc/lv_math.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_debug.h"

/*********************
 *      DEFINES
 *********************/
#define SHADOW_UPSACALE_SHIFT   6
#define SHADOW_ENHANCE          1
#define SPLIT_LIMIT             50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
LV_ATTRIBUTE_FAST_MEM static void draw_bg(const lv_area_t * coords, const lv_area_t * clip,
                                          const lv_draw_rect_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void draw_border(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc);

#if LV_USE_OUTLINE
    static void draw_outline(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);
#endif
#if LV_USE_SHADOW
LV_ATTRIBUTE_FAST_MEM static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void shadow_draw_corner_buf(const lv_area_t * coords,  uint16_t * sh_buf, lv_coord_t s,
                                                         lv_coord_t r);
LV_ATTRIBUTE_FAST_MEM static void shadow_blur_corner(lv_coord_t size, lv_coord_t sw, uint16_t * sh_ups_buf);
#endif

#if LV_USE_PATTERN
    static void draw_pattern(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);
#endif

#if LV_USE_VALUE_STR
    static void draw_value_str(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);
#endif
static void draw_full_border(const lv_area_t * area_inner, const lv_area_t * area_outer, const lv_area_t * clip,
                             lv_coord_t radius, bool radius_is_in, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);
LV_ATTRIBUTE_FAST_MEM static inline lv_color_t grad_get(const lv_draw_rect_dsc_t * dsc, lv_coord_t s, lv_coord_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_USE_SHADOW && LV_SHADOW_CACHE_SIZE
    static uint8_t sh_cache[LV_SHADOW_CACHE_SIZE * LV_SHADOW_CACHE_SIZE];
    static int32_t sh_cache_size = -1;
    static int32_t sh_cache_r = -1;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM void lv_draw_rect_dsc_init(lv_draw_rect_dsc_t * dsc)
{
    _lv_memset_00(dsc, sizeof(lv_draw_rect_dsc_t));
    dsc->bg_color = LV_COLOR_WHITE;
    dsc->bg_grad_color = LV_COLOR_BLACK;
    dsc->border_color = LV_COLOR_BLACK;
    dsc->pattern_recolor = LV_COLOR_BLACK;
    dsc->value_color = LV_COLOR_BLACK;
    dsc->shadow_color = LV_COLOR_BLACK;
    dsc->bg_grad_color_stop = 0xFF;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->outline_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
    dsc->pattern_opa = LV_OPA_COVER;
    dsc->pattern_font = LV_THEME_DEFAULT_FONT_NORMAL;
    dsc->value_opa = LV_OPA_COVER;
    dsc->value_font = LV_THEME_DEFAULT_FONT_NORMAL;
    dsc->shadow_opa = LV_OPA_COVER;
    dsc->border_side = LV_BORDER_SIDE_FULL;

}

/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;
#if LV_USE_SHADOW
    draw_shadow(coords, clip, dsc);
#endif

    draw_bg(coords, clip, dsc);

#if LV_USE_PATTERN
    draw_pattern(coords, clip, dsc);
#endif
    draw_border(coords, clip, dsc);

#if LV_USE_VALUE_STR
    draw_value_str(coords, clip, dsc);
#endif

#if LV_USE_OUTLINE
    draw_outline(coords, clip, dsc);
#endif

    LV_ASSERT_MEM_INTEGRITY();
}

/**
 * Draw a pixel
 * @param point the coordinates of the point to draw
 * @param mask the pixel will be drawn only in this mask
 * @param style pointer to a style
 * @param opa_scale scale down the opacity by the factor
 */
void lv_draw_px(const lv_point_t * point, const lv_area_t * clip_area, const lv_style_t * style)
{
    LV_UNUSED(point);
    LV_UNUSED(clip_area);
    LV_UNUSED(style);
    //    lv_opa_t opa = style->body.opa;
    //    if(opa_scale != LV_OPA_COVER) opa = (opa * opa_scale) >> 8;
    //
    //    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;
    //
    //    lv_area_t fill_area;
    //    fill_area.x1 = point->x;
    //    fill_area.y1 = point->y;
    //    fill_area.x2 = point->x;
    //    fill_area.y2 = point->y;
    //
    //    uint8_t mask_cnt = lv_draw_mask_get_cnt();
    //
    //    if(mask_cnt == 0) {
    //        lv_blend_fill(clip_area, &fill_area, style->body.main_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, style->body.blend_mode);
    //    } else {
    //        uint8_t mask_buf;
    //        lv_draw_mask_res_t mask_res;
    //        mask_res = lv_draw_mask_apply(&mask_buf, point->x, point->y, 1);
    //        lv_blend_fill(clip_area, &fill_area, style->body.main_color, &mask_buf, mask_res, opa, style->body.blend_mode);
    //    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_bg(const lv_area_t * coords, const lv_area_t * clip,
                                          const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->bg_opa <= LV_OPA_MIN) return;

    lv_area_t coords_bg;
    lv_area_copy(&coords_bg, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(dsc->border_width > 1 && dsc->border_opa >= LV_OPA_MAX && dsc->radius != 0) {
        coords_bg.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        coords_bg.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        coords_bg.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        coords_bg.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }

    lv_opa_t opa = dsc->bg_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = _lv_area_intersect(&draw_area, &coords_bg, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    int32_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create a mask if there is a radius*/
    lv_opa_t * mask_buf = _lv_mem_buf_get(draw_area_w);

    lv_grad_dir_t grad_dir = dsc->bg_grad_dir;
    if(dsc->bg_color.full == dsc->bg_grad_color.full) grad_dir = LV_GRAD_DIR_NONE;

    uint16_t other_mask_cnt = lv_draw_mask_get_cnt();
    bool simple_mode = true;
    if(other_mask_cnt) simple_mode = false;
    else if(grad_dir == LV_GRAD_DIR_HOR) simple_mode = false;

    int16_t mask_rout_id = LV_MASK_ID_INV;

    int32_t coords_w = lv_area_get_width(&coords_bg);
    int32_t coords_h = lv_area_get_height(&coords_bg);

    /*Get the real radius*/
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MATH_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Most simple case: just a plain rectangle*/
    if(simple_mode && rout == 0 && (grad_dir == LV_GRAD_DIR_NONE)) {
        _lv_blend_fill(clip, &coords_bg,
                       dsc->bg_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa,
                       dsc->bg_blend_mode);
    }
    /*More complex case: there is a radius, gradient or other mask.*/
    else {
        lv_draw_mask_radius_param_t mask_rout_param;
        if(rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, &coords_bg, rout, false);
            mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        /*Draw the background line by line*/
        int32_t h;
        lv_draw_mask_res_t mask_res = LV_DRAW_MASK_RES_FULL_COVER;
        lv_color_t grad_color = dsc->bg_color;


        lv_color_t * grad_map = NULL;
        /*In case of horizontal gradient pre-compute a line with a gradient*/
        if(grad_dir == LV_GRAD_DIR_HOR) {
            grad_map = _lv_mem_buf_get(coords_w * sizeof(lv_color_t));

            int32_t i;
            for(i = 0; i < coords_w; i++) {
                grad_map[i] = grad_get(dsc, coords_w, i);
            }
        }

        bool split = false;
        if(lv_area_get_width(&coords_bg) - 2 * rout > SPLIT_LIMIT) split = true;

        lv_opa_t opa2;

        lv_area_t fill_area;
        fill_area.x1 = coords_bg.x1;
        fill_area.x2 = coords_bg.x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            int32_t y = h + vdb->area.y1;

            opa2 = opa;

            /*In not corner areas apply the mask only if required*/
            if(y > coords_bg.y1 + rout + 1 &&
               y < coords_bg.y2 - rout - 1) {
                mask_res = LV_DRAW_MASK_RES_FULL_COVER;
                if(simple_mode == false) {
                    _lv_memset(mask_buf, opa, draw_area_w);
                    mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                }
            }
            /*In corner areas apply the mask anyway*/
            else {
                _lv_memset(mask_buf, opa, draw_area_w);
                mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
            }

            /*If mask will taken into account its base opacity was already set by memset above*/
            if(mask_res == LV_DRAW_MASK_RES_CHANGED) {
                opa2 = LV_OPA_COVER;
            }

            /*Get the current line color*/
            if(grad_dir == LV_GRAD_DIR_VER) {
                grad_color = grad_get(dsc, lv_area_get_height(&coords_bg), y - coords_bg.y1);
            }

            /* If there is not other mask and drawing the corner area split the drawing to corner and middle areas
             * because it the middle mask shouldn't be taken into account (therefore its faster)*/
            if(simple_mode && split &&
               (y < coords_bg.y1 + rout + 1 ||
                y > coords_bg.y2 - rout - 1)) {

                /*Left part*/
                lv_area_t fill_area2;
                fill_area2.x1 = coords_bg.x1;
                fill_area2.x2 = coords_bg.x1 + rout - 1;
                fill_area2.y1 = fill_area.y1;
                fill_area2.y2 = fill_area.y2;

                _lv_blend_fill(clip, &fill_area2,
                               grad_color, mask_buf, mask_res, opa2, dsc->bg_blend_mode);

                /*Center part*/
                if(grad_dir == LV_GRAD_DIR_VER) {
                    fill_area2.x1 = coords_bg.x1 + rout;
                    fill_area2.x2 = coords_bg.x2 - rout;
                    _lv_blend_fill(clip, &fill_area2,
                                   grad_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->bg_blend_mode);
                }

                /*Right part*/
                fill_area2.x1 = coords_bg.x2 - rout + 1;
                fill_area2.x2 = coords_bg.x2;

                int32_t mask_ofs = (coords_bg.x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
                if(mask_ofs < 0) mask_ofs = 0;
                _lv_blend_fill(clip, &fill_area2,
                               grad_color, mask_buf + mask_ofs, mask_res, opa2, dsc->bg_blend_mode);


            }
            else {
                if(grad_dir == LV_GRAD_DIR_HOR) {
                    _lv_blend_map(clip, &fill_area, grad_map, mask_buf, mask_res, opa2, dsc->bg_blend_mode);
                }
                else if(grad_dir == LV_GRAD_DIR_VER) {
                    _lv_blend_fill(clip, &fill_area,
                                   grad_color, mask_buf, mask_res, opa2, dsc->bg_blend_mode);
                }
                else if(other_mask_cnt != 0 || !split) {
                    _lv_blend_fill(clip, &fill_area,
                                   grad_color, mask_buf, mask_res, opa2, dsc->bg_blend_mode);
                }
            }
            fill_area.y1++;
            fill_area.y2++;
        }

        if(grad_dir == LV_GRAD_DIR_NONE && other_mask_cnt == 0 && split) {
            /*Central part*/
            fill_area.x1 = coords_bg.x1 + rout;
            fill_area.x2 = coords_bg.x2 - rout;
            fill_area.y1 = coords_bg.y1;
            fill_area.y2 = coords_bg.y1 + rout;

            _lv_blend_fill(clip, &fill_area,
                           dsc->bg_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->bg_blend_mode);

            fill_area.y1 = coords_bg.y2 - rout;
            if(fill_area.y1 <= fill_area.y2) fill_area.y1 = fill_area.y2 + 1;    /*Avoid overdrawing the last line*/
            fill_area.y2 = coords_bg.y2;


            _lv_blend_fill(clip, &fill_area,
                           dsc->bg_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->bg_blend_mode);

            fill_area.x1 = coords_bg.x1;
            fill_area.x2 = coords_bg.x2;
            fill_area.y1 = coords_bg.y1 + rout + 1;
            fill_area.y2 = coords_bg.y2 - rout - 1;

            _lv_blend_fill(clip, &fill_area,
                           dsc->bg_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->bg_blend_mode);

        }

        if(grad_map) _lv_mem_buf_release(grad_map);
    }

    lv_draw_mask_remove_id(mask_rout_id);

    _lv_mem_buf_release(mask_buf);

}

LV_ATTRIBUTE_FAST_MEM static void draw_border(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->border_opa <= LV_OPA_MIN) return;
    if(dsc->border_width == 0) return;
    if(dsc->border_side == LV_BORDER_SIDE_NONE) return;
    if(dsc->border_post) return;

    int32_t coords_w = lv_area_get_width(coords);
    int32_t coords_h = lv_area_get_height(coords);

    /*Get the real radius*/
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MATH_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : - (dsc->border_width + rout));

    if(dsc->border_side == LV_BORDER_SIDE_FULL) {
        draw_full_border(&area_inner, coords, clip, dsc->radius, false, dsc->border_color, dsc->border_opa,
                         dsc->border_blend_mode);
    }
    else {
        lv_opa_t opa = dsc->border_opa;
        if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

        lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
        lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

        /* Get clipped fill area which is the real draw area.
         * It is always the same or inside `fill_area` */
        lv_area_t draw_area;
        bool is_common;
        is_common = _lv_area_intersect(&draw_area, coords, clip);
        if(is_common == false) return;

        const lv_area_t * disp_area = &vdb->area;

        /* Now `draw_area` has absolute coordinates.
         * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
        draw_area.x1 -= disp_area->x1;
        draw_area.y1 -= disp_area->y1;
        draw_area.x2 -= disp_area->x1;
        draw_area.y2 -= disp_area->y1;

        int32_t draw_area_w = lv_area_get_width(&draw_area);

        /*Create a mask if there is a radius*/
        lv_opa_t * mask_buf = _lv_mem_buf_get(draw_area_w);

        /*Create mask for the outer area*/
        int16_t mask_rout_id = LV_MASK_ID_INV;
        lv_draw_mask_radius_param_t mask_rout_param;
        if(rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, coords, rout, false);
            mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        /*Create mask for the inner mask*/
        int32_t rin = rout - dsc->border_width;
        if(rin < 0) rin = 0;
        lv_draw_mask_radius_param_t mask_rin_param;
        lv_draw_mask_radius_init(&mask_rin_param, &area_inner, rout - dsc->border_width, true);
        int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

        int32_t corner_size = LV_MATH_MAX(rout, dsc->border_width - 1);

        int32_t h;
        lv_draw_mask_res_t mask_res;
        lv_area_t fill_area;

        lv_color_t color = dsc->border_color;
        lv_blend_mode_t blend_mode = dsc->border_blend_mode;

        fill_area.x1 = coords->x1;
        fill_area.x2 = coords->x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;

        uint32_t buf_ofs = 0;
        if(dsc->border_side == LV_BORDER_SIDE_LEFT) fill_area.x2 = coords->x1 + corner_size;
        else if(dsc->border_side == LV_BORDER_SIDE_RIGHT) {
            fill_area.x1 = coords->x2 - corner_size;
            buf_ofs = fill_area.x1 - coords->x1;
        }

        volatile bool top_only = false;
        volatile bool bottom_only = false;
        if(dsc->border_side == LV_BORDER_SIDE_TOP) top_only = true;
        if(dsc->border_side == LV_BORDER_SIDE_BOTTOM) bottom_only = true;
        if(dsc->border_side == (LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM)) {
            top_only = true;
            bottom_only = true;
        }

        volatile bool normal = !top_only && !bottom_only ? true : false;

        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            if(normal ||
               (top_only && fill_area.y1 <= coords->y1 + corner_size) ||
               (bottom_only && fill_area.y1 >= coords->y2 - corner_size)) {
                _lv_memset_ff(mask_buf, draw_area_w);
                mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);
                _lv_blend_fill(clip, &fill_area, color, mask_buf + buf_ofs, mask_res, opa, blend_mode);
            }
            fill_area.y1++;
            fill_area.y2++;

        }
        lv_draw_mask_remove_id(mask_rin_id);
        lv_draw_mask_remove_id(mask_rout_id);
        _lv_mem_buf_release(mask_buf);
    }
}

LV_ATTRIBUTE_FAST_MEM static inline lv_color_t grad_get(const lv_draw_rect_dsc_t * dsc, lv_coord_t s, lv_coord_t i)
{
    int32_t min = (dsc->bg_main_color_stop * s) >> 8;
    if(i <= min) return dsc->bg_color;

    int32_t max = (dsc->bg_grad_color_stop * s) >> 8;
    if(i >= max) return dsc->bg_grad_color;

    int32_t d = dsc->bg_grad_color_stop - dsc->bg_main_color_stop;
    d = (s * d) >> 8;
    i -= min;
    lv_opa_t mix = (i * 255) / d;
    return lv_color_mix(dsc->bg_grad_color, dsc->bg_color, mix);
}

#if LV_USE_SHADOW
LV_ATTRIBUTE_FAST_MEM static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc)
{
    /*Check whether the shadow is visible*/
    if(dsc->shadow_width == 0) return;
    if(dsc->shadow_opa <= LV_OPA_MIN) return;

    if(dsc->shadow_width == 1 && dsc->shadow_ofs_x == 0 &&
       dsc->shadow_ofs_y == 0 && dsc->shadow_spread <= 0) {
        return;
    }

    int32_t sw = dsc->shadow_width;

    lv_area_t sh_rect_area;
    sh_rect_area.x1 = coords->x1  + dsc->shadow_ofs_x - dsc->shadow_spread;
    sh_rect_area.x2 = coords->x2  + dsc->shadow_ofs_x + dsc->shadow_spread;
    sh_rect_area.y1 = coords->y1  + dsc->shadow_ofs_y - dsc->shadow_spread;
    sh_rect_area.y2 = coords->y2  + dsc->shadow_ofs_y + dsc->shadow_spread;

    lv_area_t sh_area;
    sh_area.x1 = sh_rect_area.x1 - sw / 2 - 1;
    sh_area.x2 = sh_rect_area.x2 + sw / 2 + 1;
    sh_area.y1 = sh_rect_area.y1 - sw / 2 - 1;
    sh_area.y2 = sh_rect_area.y2 + sw / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = _lv_area_intersect(&draw_area, &sh_area, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    /*Consider 1 px smaller bg to be sure the edge will be covered by the shadow*/
    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);
    bg_coords.x1 += 1;
    bg_coords.y1 += 1;
    bg_coords.x2 -= 1;
    bg_coords.y2 -= 1;

    /*Get the real radius*/
    int32_t r_bg = dsc->radius;
    int32_t short_side = LV_MATH_MIN(lv_area_get_width(&bg_coords), lv_area_get_height(&bg_coords));
    if(r_bg > short_side >> 1) r_bg = short_side >> 1;

    int32_t r_sh = dsc->radius;
    short_side = LV_MATH_MIN(lv_area_get_width(&sh_rect_area), lv_area_get_height(&sh_rect_area));
    if(r_sh > short_side >> 1) r_sh = short_side >> 1;


    int32_t corner_size = sw  + r_sh;

    lv_opa_t * sh_buf;

#if LV_SHADOW_CACHE_SIZE
    if(sh_cache_size == corner_size && sh_cache_r == r_sh) {
        /*Use the cache if available*/
        sh_buf = _lv_mem_buf_get(corner_size * corner_size);
        _lv_memcpy(sh_buf, sh_cache, corner_size * corner_size);
    }
    else {
        /*A larger buffer is required for calculation */
        sh_buf = _lv_mem_buf_get(corner_size * corner_size * sizeof(uint16_t));
        shadow_draw_corner_buf(&sh_rect_area, (uint16_t *)sh_buf, dsc->shadow_width, r_sh);

        /*Cache the corner if it fits into the cache size*/
        if(corner_size * corner_size < sizeof(sh_cache)) {
            _lv_memcpy(sh_cache, sh_buf, corner_size * corner_size);
            sh_cache_size = corner_size;
            sh_cache_r = r_sh;
        }
    }
#else
    sh_buf = _lv_mem_buf_get(corner_size * corner_size * sizeof(uint16_t));
    shadow_draw_corner_buf(&sh_rect_area, (uint16_t *)sh_buf, dsc->shadow_width, r_sh);
#endif

    lv_coord_t h_half = sh_area.y1 + lv_area_get_height(&sh_area) / 2;
    lv_coord_t w_half = sh_area.x1 + lv_area_get_width(&sh_area) / 2;

    bool simple_mode = true;
    if(lv_draw_mask_get_cnt() > 0) simple_mode = false;
    else if(dsc->shadow_ofs_x != 0 || dsc->shadow_ofs_y != 0) simple_mode = false;
    else if(dsc->shadow_spread != 0) simple_mode = false;

    /*Create a mask*/
    lv_draw_mask_res_t mask_res;
    lv_opa_t * mask_buf = _lv_mem_buf_get(lv_area_get_width(&sh_area));

    lv_draw_mask_radius_param_t mask_rout_param;
    lv_draw_mask_radius_init(&mask_rout_param, &bg_coords, r_bg, true);

    int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);

    lv_area_t a;

    /*Draw the top right corner*/
    int32_t y;
    lv_opa_t * sh_buf_tmp;
    a.x2 = sh_area.x2;
    a.x1 = a.x2 - corner_size + 1;
    a.y1 = sh_area.y1;
    a.y2 = a.y1 + corner_size - 1;

    lv_area_t ca;
    bool has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        /*Avoid overlap in the middle with large radius*/
        if(ca.y2 > h_half) ca.y2 = h_half;
        if(ca.x1 <= w_half) ca.x1 = w_half + 1;

        lv_coord_t h = lv_area_get_height(&ca);
        lv_coord_t w = lv_area_get_width(&ca);
        if(w > 0) {
            sh_buf_tmp = sh_buf + (ca.x1 - a.x1);
            sh_buf_tmp += corner_size * (ca.y1 - a.y1);

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y2 = fa.y1;

            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

                _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                               mask_res, opa, dsc->shadow_blend_mode);
                fa.y1++;
                fa.y2++;
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Draw the bottom right corner*/
    a.x2 = sh_area.x2;
    a.x1 = a.x2 - corner_size + 1;
    a.y1 = sh_area.y2 - corner_size + 1;
    a.y2 = sh_area.y2;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        /*Avoid overlap in the middle with large radius*/
        if(ca.y1 <= h_half) ca.y1 = h_half + 1;
        if(ca.x1 <= w_half) ca.x1 = w_half + 1;

        lv_coord_t h = lv_area_get_height(&ca);
        lv_coord_t w = lv_area_get_width(&ca);

        if(w > 0) {
            sh_buf_tmp = sh_buf + (ca.x1 - a.x1);
            sh_buf_tmp += corner_size * (a.y2 - ca.y2);

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y1 = fa.y2;    /*Fill from bottom to top*/

            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

                _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                               mask_res, opa, dsc->shadow_blend_mode);
                fa.y1--;
                fa.y2--;
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Fill the right side*/
    a.x2 = sh_area.x2;
    a.x1 = a.x2 - corner_size + 1;
    a.y1 = sh_area.y1 + corner_size;
    a.y2 = sh_area.y2 - corner_size;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        if(simple_mode) ca.x1 = LV_MATH_MAX(ca.x1, coords->x2);
        /*Draw horizontal lines*/
        lv_coord_t w = lv_area_get_width(&ca);
        if(w > 0) {
            lv_coord_t h = lv_area_get_height(&ca);

            /*The last line of the shadow is repeated on the side*/
            sh_buf_tmp = sh_buf + corner_size * (corner_size - 1);
            sh_buf_tmp += ca.x1 - a.x1;

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y2 = fa.y1;
            mask_res = LV_DRAW_MASK_RES_FULL_COVER;
            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);

                if(simple_mode) {
                    mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                else {
                    mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }

                _lv_blend_fill(clip, &fa,
                               dsc->shadow_color, mask_buf, mask_res, dsc->shadow_opa, dsc->shadow_blend_mode);
                fa.y1++;
                fa.y2++;
            }
        }
    }

    /*Invert the shadow corner buffer and draw the corners on the left*/
    sh_buf_tmp = sh_buf ;
    for(y = 0; y < corner_size; y++) {
        int32_t x;
        for(x = 0; x < corner_size / 2; x++) {
            lv_opa_t tmp = sh_buf_tmp[x];
            sh_buf_tmp[x] = sh_buf_tmp[corner_size - x - 1];
            sh_buf_tmp[corner_size - x - 1] = tmp;
        }
        sh_buf_tmp += corner_size;
    }

    /*Draw the top left corner*/
    a.x1 = sh_area.x1;
    a.x2 = a.x1 + corner_size - 1;
    a.y1 = sh_area.y1;
    a.y2 = a.y1 + corner_size - 1;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        /*Avoid overlap in the middle with large radius*/
        if(ca.y2 > h_half) ca.y2 = h_half;
        if(ca.x2 > w_half) ca.x2 = w_half;

        lv_coord_t h = lv_area_get_height(&ca);
        lv_coord_t w = lv_area_get_width(&ca);
        if(w > 0) {
            sh_buf_tmp = sh_buf + (ca.x1 - a.x1);
            sh_buf_tmp += corner_size * (ca.y1 - a.y1);

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y2 = fa.y1;

            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

                _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                               mask_res, opa, dsc->shadow_blend_mode);
                fa.y1++;
                fa.y2++;
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Draw the bottom left corner*/
    a.x1 = sh_area.x1;
    a.x2 = a.x1 + corner_size - 1;
    a.y1 = sh_area.y2 - corner_size + 1;
    a.y2 = sh_area.y2;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        /*Avoid overlap in the middle with large radius*/
        if(ca.y1 <= h_half) ca.y1 = h_half + 1;
        if(ca.x2 > w_half) ca.x2 = w_half;
        lv_coord_t h = lv_area_get_height(&ca);
        lv_coord_t w = lv_area_get_width(&ca);

        if(w > 0) {
            sh_buf_tmp = sh_buf + (ca.x1 - a.x1);
            sh_buf_tmp += corner_size * (a.y2 - ca.y2);

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y1 = fa.y2;    /*Fill from bottom to top*/

            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

                _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                               mask_res, opa, dsc->shadow_blend_mode);
                fa.y1--;
                fa.y2--;
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Fill the left side*/
    a.x1 = sh_area.x1;
    a.x2 = a.x1 + corner_size - 1;
    a.y1 = sh_area.y1 + corner_size;
    a.y2 = sh_area.y2 - corner_size;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        if(simple_mode) ca.x2 = LV_MATH_MIN(coords->x1, ca.x2);
        /*Draw vertical lines*/
        lv_coord_t w = lv_area_get_width(&ca);
        if(w > 0) {
            lv_coord_t h = lv_area_get_height(&ca);
            /*The last line of the shadow is repeated on the side*/
            sh_buf_tmp = sh_buf + corner_size * (corner_size - 1);
            sh_buf_tmp += ca.x1 - a.x1;

            lv_area_t fa;
            lv_area_copy(&fa, &ca);
            fa.y2 = fa.y1;
            for(y = 0; y < h; y++) {
                _lv_memcpy(mask_buf, sh_buf_tmp, w);
                if(simple_mode) {
                    mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                else {
                    mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }

                _lv_blend_fill(clip, &fa,
                               dsc->shadow_color, mask_buf, mask_res, dsc->shadow_opa, dsc->shadow_blend_mode);
                fa.y1++;
                fa.y2++;
            }
        }
    }

    /*Fill the top side*/
    a.x1 = sh_area.x1 + corner_size;
    a.x2 = sh_area.x2 - corner_size;
    a.y1 = sh_area.y1;
    a.y2 = sh_area.y1 + corner_size - 1;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        if(simple_mode) ca.y2 = LV_MATH_MIN(ca.y2, coords->y1);
        /*Draw horizontal lines*/
        lv_coord_t w = lv_area_get_width(&ca);
        lv_coord_t h = lv_area_get_height(&ca);
        sh_buf_tmp = sh_buf + corner_size - 1;
        sh_buf_tmp += corner_size * (ca.y1 - a.y1);

        lv_area_t fa;
        lv_area_copy(&fa, &ca);
        fa.y2 = fa.y1;
        mask_res = LV_DRAW_MASK_RES_FULL_COVER;
        for(y = 0; y < h; y++) {
            lv_opa_t opa_tmp = sh_buf_tmp[0];
            if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;

            _lv_memset(mask_buf, opa_tmp, w);

            if(simple_mode) {
                mask_res = LV_DRAW_MASK_RES_CHANGED;
            }
            else {
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
            }

            _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                           mask_res, LV_OPA_COVER, dsc->shadow_blend_mode);
            fa.y1++;
            fa.y2++;
            sh_buf_tmp += corner_size;
        }
    }


    /*Fill the bottom side*/
    a.x1 = sh_area.x1 + corner_size;
    a.x2 = sh_area.x2 - corner_size;
    a.y1 = sh_area.y2 - corner_size + 1;
    a.y2 = sh_area.y2;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        if(simple_mode) ca.y1 = LV_MATH_MAX(ca.y1, coords->y2);
        /*Draw horizontal lines*/
        lv_coord_t w = lv_area_get_width(&ca);
        lv_coord_t h = lv_area_get_height(&ca);
        sh_buf_tmp = sh_buf + corner_size - 1;
        sh_buf_tmp += corner_size * (a.y2 - ca.y2);

        lv_area_t fa;
        lv_area_copy(&fa, &ca);
        fa.y1 = fa.y2;
        for(y = 0; y < h; y++) {
            lv_opa_t opa_tmp = sh_buf_tmp[0];
            if(opa_tmp != LV_OPA_COVER || opa != LV_OPA_COVER) opa_tmp = (opa * opa_tmp) >> 8;

            _lv_memset(mask_buf, opa_tmp, w);
            if(simple_mode) {
                mask_res = LV_DRAW_MASK_RES_CHANGED;
            }
            else {
                mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
                if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
            }

            _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                           mask_res, LV_OPA_COVER, dsc->shadow_blend_mode);
            fa.y1--;
            fa.y2--;
            sh_buf_tmp += corner_size;
        }
    }

    /*Draw the middle area*/
    a.x1 = sh_area.x1 + corner_size;
    a.x2 = sh_area.x2 - corner_size;
    a.y1 = sh_area.y1 + corner_size;
    a.y2 = sh_area.y2 - corner_size;

    has_com = _lv_area_intersect(&ca, &a, clip);
    if(has_com && simple_mode == false &&  _lv_area_is_in(&a, &bg_coords, r_bg) == false) {
        /*Draw horizontal lines*/
        lv_coord_t w = lv_area_get_width(&ca);
        lv_coord_t h = lv_area_get_height(&ca);

        lv_area_t fa;
        lv_area_copy(&fa, &ca);
        fa.y2 = fa.y1;
        for(y = 0; y < h; y++) {
            _lv_memset(mask_buf, dsc->shadow_opa, w);
            mask_res = lv_draw_mask_apply(mask_buf, fa.x1, fa.y1, w);
            if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

            _lv_blend_fill(clip, &fa, dsc->shadow_color, mask_buf,
                           mask_res, LV_OPA_COVER, dsc->shadow_blend_mode);
            fa.y1++;
            fa.y2++;
        }
    }

    lv_draw_mask_remove_id(mask_rout_id);
    _lv_mem_buf_release(mask_buf);
    _lv_mem_buf_release(sh_buf);
}

/**
 * Calculate a blurred corner
 * @param coords Coordinates of the shadow
 * @param sh_buf a buffer to store the result. It's size should be `(sw + r)^2 * 2`
 * @param sw shadow width
 * @param r radius
 */
LV_ATTRIBUTE_FAST_MEM static void shadow_draw_corner_buf(const lv_area_t * coords, uint16_t * sh_buf, lv_coord_t sw,
                                                         lv_coord_t r)
{
    int32_t sw_ori = sw;
    int32_t size = sw_ori  + r;

    lv_area_t sh_area;
    lv_area_copy(&sh_area, coords);
    sh_area.x2 = sw / 2 + r - 1  - ((sw & 1) ? 0 : 1);
    sh_area.y1 = sw / 2 + 1;

    sh_area.x1 = sh_area.x2 - lv_area_get_width(coords);
    sh_area.y2 = sh_area.y1 + lv_area_get_height(coords);

    lv_draw_mask_radius_param_t mask_param;
    lv_draw_mask_radius_init(&mask_param, &sh_area, r, false);

#if SHADOW_ENHANCE
    /*Set half shadow width width because blur will be repeated*/
    if(sw_ori == 1) sw = 1;
    else sw = sw_ori >> 1;
#endif

    int32_t y;
    lv_opa_t * mask_line = _lv_mem_buf_get(size);
    uint16_t * sh_ups_tmp_buf = (uint16_t *)sh_buf;
    for(y = 0; y < size; y++) {
        _lv_memset_ff(mask_line, size);
        lv_draw_mask_res_t mask_res = mask_param.dsc.cb(mask_line, 0, y, size, &mask_param);
        if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
            _lv_memset_00(sh_ups_tmp_buf, size * sizeof(sh_ups_tmp_buf[0]));
        }
        else {
            int32_t i;
            sh_ups_tmp_buf[0] = (mask_line[0] << SHADOW_UPSACALE_SHIFT) / sw;
            for(i = 1; i < size; i++) {
                if(mask_line[i] == mask_line[i - 1]) sh_ups_tmp_buf[i] = sh_ups_tmp_buf[i - 1];
                else  sh_ups_tmp_buf[i] = (mask_line[i] << SHADOW_UPSACALE_SHIFT) / sw;
            }
        }

        sh_ups_tmp_buf += size;
    }
    _lv_mem_buf_release(mask_line);

    if(sw == 1) {
        int32_t i;
        lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
        for(i = 0; i < size * size; i++) {
            res_buf[i] = (sh_buf[i] >> SHADOW_UPSACALE_SHIFT);
        }
        return;
    }

    shadow_blur_corner(size, sw, sh_buf);

#if SHADOW_ENHANCE == 0
    /*The result is required in lv_opa_t not uint16_t*/
    uint32_t x;
    lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
    for(x = 0; x < size * size; x++) {
        res_buf[x] = sh_buf[x];
    }
#else
    sw += sw_ori & 1;
    if(sw > 1) {
        uint32_t i;
        sh_buf[0] = (sh_buf[0] << SHADOW_UPSACALE_SHIFT) / sw;
        for(i = 1; i < (uint32_t) size * size; i++) {
            if(sh_buf[i] == sh_buf[i - 1]) sh_buf[i] = sh_buf[i - 1];
            else  sh_buf[i] = (sh_buf[i] << SHADOW_UPSACALE_SHIFT) / sw;
        }

        shadow_blur_corner(size, sw, sh_buf);
    }
    int32_t x;
    lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
    for(x = 0; x < size * size; x++) {
        res_buf[x] = sh_buf[x];
    }
#endif

}

LV_ATTRIBUTE_FAST_MEM static void shadow_blur_corner(lv_coord_t size, lv_coord_t sw, uint16_t * sh_ups_buf)
{
    int32_t s_left = sw >> 1;
    int32_t s_right = (sw >> 1);
    if((sw & 1) == 0) s_left--;

    /*Horizontal blur*/
    uint16_t * sh_ups_blur_buf = _lv_mem_buf_get(size * sizeof(uint16_t));

    int32_t x;
    int32_t y;

    uint16_t * sh_ups_tmp_buf = sh_ups_buf;

    for(y = 0; y < size; y++) {
        int32_t v = sh_ups_tmp_buf[size - 1] * sw;
        for(x = size - 1; x >= 0; x--) {
            sh_ups_blur_buf[x] = v;

            /*Forget the right pixel*/
            uint32_t right_val = 0;
            if(x + s_right < size) right_val = sh_ups_tmp_buf[x + s_right];
            v -= right_val;

            /*Add the left pixel*/
            uint32_t left_val;
            if(x - s_left - 1 < 0) left_val = sh_ups_tmp_buf[0];
            else left_val = sh_ups_tmp_buf[x - s_left - 1];
            v += left_val;
        }
        _lv_memcpy(sh_ups_tmp_buf, sh_ups_blur_buf, size * sizeof(uint16_t));
        sh_ups_tmp_buf += size;
    }

    /*Vertical blur*/
    uint32_t i;
    sh_ups_buf[0] = sh_ups_buf[0] / sw;
    for(i = 1; i < (uint32_t)size * size; i++) {
        if(sh_ups_buf[i] == sh_ups_buf[i - 1]) sh_ups_buf[i] = sh_ups_buf[i - 1];
        else  sh_ups_buf[i] = sh_ups_buf[i] / sw;
    }

    for(x = 0; x < size; x++) {
        sh_ups_tmp_buf = &sh_ups_buf[x];
        int32_t v = sh_ups_tmp_buf[0] * sw;
        for(y = 0; y < size ; y++, sh_ups_tmp_buf += size) {
            sh_ups_blur_buf[y] = v < 0 ? 0 : (v >> SHADOW_UPSACALE_SHIFT);

            /*Forget the top pixel*/
            uint32_t top_val;
            if(y - s_right <= 0) top_val = sh_ups_tmp_buf[0];
            else top_val = sh_ups_buf[(y - s_right) * size + x];
            v -= top_val;

            /*Add the bottom pixel*/
            uint32_t bottom_val;
            if(y + s_left + 1 < size) bottom_val = sh_ups_buf[(y + s_left + 1) * size + x];
            else bottom_val = sh_ups_buf[(size - 1) * size + x];
            v += bottom_val;
        }

        /*Write back the result into `sh_ups_buf`*/
        sh_ups_tmp_buf = &sh_ups_buf[x];
        for(y = 0; y < size; y++, sh_ups_tmp_buf += size) {
            (*sh_ups_tmp_buf) = sh_ups_blur_buf[y];
        }
    }

    _lv_mem_buf_release(sh_ups_blur_buf);
}

#endif

#if LV_USE_OUTLINE
static void draw_outline(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->outline_opa <= LV_OPA_MIN) return;
    if(dsc->outline_width == 0) return;

    lv_opa_t opa = dsc->outline_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get the inner radius*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 -= dsc->outline_pad;
    area_inner.y1 -= dsc->outline_pad;
    area_inner.x2 += dsc->outline_pad;
    area_inner.y2 += dsc->outline_pad;

    lv_area_t area_outer;
    lv_area_copy(&area_outer, &area_inner);

    area_outer.x1 -= dsc->outline_width;
    area_outer.x2 += dsc->outline_width;
    area_outer.y1 -= dsc->outline_width;
    area_outer.y2 += dsc->outline_width;

    draw_full_border(&area_inner, &area_outer, clip, dsc->radius, true, dsc->outline_color, dsc->outline_opa,
                     dsc->outline_blend_mode);
}
#endif

#if LV_USE_PATTERN
static void draw_pattern(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->pattern_image == NULL) return;
    if(dsc->pattern_opa <= LV_OPA_MIN) return;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->pattern_image);

    lv_draw_img_dsc_t img_dsc;
    lv_draw_label_dsc_t label_dsc;
    int32_t img_w;
    int32_t img_h;

    if(src_type == LV_IMG_SRC_FILE || src_type == LV_IMG_SRC_VARIABLE) {
        lv_img_header_t header;
        lv_res_t res = lv_img_decoder_get_info(dsc->pattern_image, &header);
        if(res != LV_RES_OK) {
            LV_LOG_WARN("draw_img: can't get image info");
            return;
        }

        img_w = header.w;
        img_h = header.h;

        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.opa = dsc->pattern_opa;
        img_dsc.recolor_opa = dsc->pattern_recolor_opa;
        img_dsc.recolor = dsc->pattern_recolor;
    }
    else if(src_type == LV_IMG_SRC_SYMBOL) {
        lv_draw_label_dsc_init(&label_dsc);
        label_dsc.color = dsc->pattern_recolor;
        label_dsc.font = dsc->pattern_font;
        label_dsc.opa = dsc->pattern_opa;
        lv_point_t s;
        _lv_txt_get_size(&s, dsc->pattern_image, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                         LV_TXT_FLAG_NONE);
        img_w = s.x;
        img_h = s.y;

    }
    else {
        /*Trigger the error handler of image drawer*/
        LV_LOG_WARN("lv_img_design: image source type is unknown");
        lv_draw_img(coords, clip, NULL, NULL);
        return;
    }

    /*Can't draw zero sized images*/
    if(img_w == 0 || img_h == 0) return;

    lv_area_t coords_tmp;

    if(dsc->pattern_repeat) {
        lv_draw_mask_radius_param_t radius_mask_param;
        lv_draw_mask_radius_init(&radius_mask_param, coords, dsc->radius, false);
        int16_t radius_mask_id = lv_draw_mask_add(&radius_mask_param, NULL);

        /*Align the pattern to the middle*/
        int32_t ofs_x = (lv_area_get_width(coords) - (lv_area_get_width(coords) / img_w) * img_w) / 2;
        int32_t ofs_y = (lv_area_get_height(coords) - (lv_area_get_height(coords) / img_h) * img_h) / 2;

        coords_tmp.y1 = coords->y1 - ofs_y;
        coords_tmp.y2 = coords_tmp.y1 + img_h - 1;
        for(; coords_tmp.y1 <= coords->y2; coords_tmp.y1 += img_h, coords_tmp.y2 += img_h) {
            coords_tmp.x1 = coords->x1 - ofs_x;
            coords_tmp.x2 = coords_tmp.x1 + img_w - 1;
            for(; coords_tmp.x1 <= coords->x2; coords_tmp.x1 += img_w, coords_tmp.x2 += img_w) {
                if(src_type == LV_IMG_SRC_SYMBOL)  lv_draw_label(&coords_tmp, clip, &label_dsc, dsc->pattern_image, NULL);
                else lv_draw_img(&coords_tmp, clip, dsc->pattern_image, &img_dsc);
            }
        }
        lv_draw_mask_remove_id(radius_mask_id);
    }
    else {
        int32_t obj_w = lv_area_get_width(coords);
        int32_t obj_h = lv_area_get_height(coords);
        coords_tmp.x1 = coords->x1 + (obj_w - img_w) / 2;
        coords_tmp.y1 = coords->y1 + (obj_h - img_h) / 2;
        coords_tmp.x2 = coords_tmp.x1 + img_w - 1;
        coords_tmp.y2 = coords_tmp.y1 + img_h - 1;

        /* If the (obj_h - img_h) is odd there is a rounding error when divided by 2.
         * It's better round up in case of symbols because probably there is some extra space in the bottom
         * due to the base line of font*/
        if(src_type == LV_IMG_SRC_SYMBOL) {
            int32_t y_corr = (obj_h - img_h) & 0x1;
            coords_tmp.y1 += y_corr;
            coords_tmp.y2 += y_corr;
        }

        int16_t radius_mask_id = LV_MASK_ID_INV;
        if(_lv_area_is_in(&coords_tmp, coords, dsc->radius) == false) {
            lv_draw_mask_radius_param_t radius_mask_param;
            lv_draw_mask_radius_init(&radius_mask_param, coords, dsc->radius, false);
            radius_mask_id = lv_draw_mask_add(&radius_mask_param, NULL);
        }

        if(src_type == LV_IMG_SRC_SYMBOL)  lv_draw_label(&coords_tmp, clip, &label_dsc, dsc->pattern_image, NULL);
        else lv_draw_img(&coords_tmp, clip, dsc->pattern_image, &img_dsc);

        lv_draw_mask_remove_id(radius_mask_id);
    }
}
#endif


#if LV_USE_VALUE_STR
static void draw_value_str(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->value_str == NULL) return;
    if(dsc->value_opa <= LV_OPA_MIN) return;

    lv_point_t s;
    _lv_txt_get_size(&s, dsc->value_str, dsc->value_font, dsc->value_letter_space, dsc->value_line_space, LV_COORD_MAX,
                     LV_TXT_FLAG_NONE);

    lv_area_t value_area;
    value_area.x1 = 0;
    value_area.y1 = 0;
    value_area.x2 = s.x - 1;
    value_area.y2 = s.y - 1;

    lv_point_t p_align;
    _lv_area_align(coords, &value_area, dsc->value_align, &p_align);

    value_area.x1 += p_align.x + dsc->value_ofs_x;
    value_area.y1 += p_align.y + dsc->value_ofs_y;
    value_area.x2 += p_align.x + dsc->value_ofs_x;
    value_area.y2 += p_align.y + dsc->value_ofs_y;

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = dsc->value_font;
    label_dsc.letter_space = dsc->value_letter_space;
    label_dsc.line_space = dsc->value_line_space;
    label_dsc.color = dsc->value_color;
    label_dsc.opa = dsc->value_opa;

    lv_draw_label(&value_area, clip, &label_dsc, dsc->value_str, NULL);
}
#endif

static void draw_full_border(const lv_area_t * area_inner, const lv_area_t * area_outer, const lv_area_t * clip,
                             lv_coord_t radius, bool radius_is_in, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    uint8_t other_mask_cnt = lv_draw_mask_get_cnt();
    bool simple_mode = true;
    if(other_mask_cnt) simple_mode = false;

    int32_t inner_w = lv_area_get_width(area_inner);
    int32_t inner_h = lv_area_get_height(area_inner);
    lv_coord_t border_width = area_outer->x2 - area_inner->x2;

    int32_t coords_out_w = lv_area_get_width(area_outer);
    int32_t coords_out_h = lv_area_get_height(area_outer);

    int32_t rin;
    int32_t rout;
    if(radius_is_in) {
        rin = radius;
        int32_t short_side = LV_MATH_MIN(inner_w, inner_h);
        if(rin > short_side >> 1) rin = short_side >> 1;

        /*Get the outer area*/
        rout = rin + border_width;
    }
    else {
        rout = radius;
        int32_t short_side = LV_MATH_MIN(coords_out_w, coords_out_h);
        if(rout > short_side >> 1) rout = short_side >> 1;

        /*Get the outer area*/
        rin = rout - border_width;
        if(rin < 0) rin = 0;

    }

    lv_disp_t * disp    = _lv_refr_get_disp_refreshing();
    lv_disp_buf_t * vdb = lv_disp_get_buf(disp);

    /* Get clipped fill area which is the real draw area.
     * It is always the same or inside `fill_area` */
    lv_area_t draw_area;
    bool is_common;
    is_common = _lv_area_intersect(&draw_area, area_outer, clip);
    if(is_common == false) return;

    const lv_area_t * disp_area = &vdb->area;

    /* Now `draw_area` has absolute coordinates.
     * Make it relative to `disp_area` to simplify draw to `disp_buf`*/
    draw_area.x1 -= disp_area->x1;
    draw_area.y1 -= disp_area->y1;
    draw_area.x2 -= disp_area->x1;
    draw_area.y2 -= disp_area->y1;

    int32_t draw_area_w = lv_area_get_width(&draw_area);

    /*Create inner the mask*/
    lv_draw_mask_radius_param_t mask_rin_param;
    lv_draw_mask_radius_init(&mask_rin_param, area_inner, rin, true);
    int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

    lv_draw_mask_radius_param_t mask_rout_param;
    lv_draw_mask_radius_init(&mask_rout_param, area_outer, rout, false);
    int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);

    lv_opa_t * mask_buf = _lv_mem_buf_get(draw_area_w);

    int32_t corner_size = LV_MATH_MAX(rout, border_width - 1);

    int32_t h;
    lv_draw_mask_res_t mask_res;
    lv_area_t fill_area;

    /*Apply some optimization if there is no other mask*/
    if(simple_mode) {
        /*Draw the upper corner area*/
        int32_t upper_corner_end = area_outer->y1 - disp_area->y1 + corner_size;

        fill_area.x1 = area_outer->x1;
        fill_area.x2 = area_outer->x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;
        for(h = draw_area.y1; h <= upper_corner_end; h++) {
            _lv_memset_ff(mask_buf, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            lv_area_t fill_area2;
            fill_area2.y1 = fill_area.y1;
            fill_area2.y2 = fill_area.y2;

            fill_area2.x1 = area_outer->x1;
            fill_area2.x2 = area_outer->x1 + rout - 1;

            _lv_blend_fill(clip, &fill_area2, color, mask_buf, mask_res, opa, blend_mode);

            /*Draw the top horizontal line*/
            if(fill_area2.y2 < area_outer->y1 + border_width) {
                fill_area2.x1 = area_outer->x1 + rout;
                fill_area2.x2 = area_outer->x2 - rout;

                _lv_blend_fill(clip, &fill_area2, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
            }

            fill_area2.x1 = area_outer->x2 - rout + 1;
            fill_area2.x2 = area_outer->x2;

            int32_t mask_ofs = (area_outer->x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
            if(mask_ofs < 0) mask_ofs = 0;
            _lv_blend_fill(clip, &fill_area2, color, mask_buf + mask_ofs, mask_res, opa, blend_mode);

            fill_area.y1++;
            fill_area.y2++;
        }

        /*Draw the lower corner area */
        int32_t lower_corner_end = area_outer->y2 - disp_area->y1 - corner_size;
        if(lower_corner_end <= upper_corner_end) lower_corner_end = upper_corner_end + 1;
        fill_area.y1 = disp_area->y1 + lower_corner_end;
        fill_area.y2 = fill_area.y1;
        for(h = lower_corner_end; h <= draw_area.y2; h++) {
            _lv_memset_ff(mask_buf, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            lv_area_t fill_area2;
            fill_area2.x1 = area_outer->x1;
            fill_area2.x2 = area_outer->x1 + rout - 1;
            fill_area2.y1 = fill_area.y1;
            fill_area2.y2 = fill_area.y2;

            _lv_blend_fill(clip, &fill_area2, color, mask_buf, mask_res, opa, blend_mode);

            /*Draw the bottom horizontal line*/
            if(fill_area2.y2 > area_outer->y2 - border_width) {
                fill_area2.x1 = area_outer->x1 + rout;
                fill_area2.x2 = area_outer->x2 - rout;

                _lv_blend_fill(clip, &fill_area2, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
            }
            fill_area2.x1 = area_outer->x2 - rout + 1;
            fill_area2.x2 = area_outer->x2;

            int32_t mask_ofs = (area_outer->x2 - rout + 1) - (vdb->area.x1 + draw_area.x1);
            if(mask_ofs < 0) mask_ofs = 0;
            _lv_blend_fill(clip, &fill_area2, color, mask_buf + mask_ofs, mask_res, opa, blend_mode);


            fill_area.y1++;
            fill_area.y2++;
        }

        /*Draw the left vertical part*/
        fill_area.y1 = area_outer->y1 + corner_size + 1;
        fill_area.y2 = area_outer->y2 - corner_size - 1;

        fill_area.x1 = area_outer->x1;
        fill_area.x2 = area_outer->x1 + border_width - 1;
        _lv_blend_fill(clip, &fill_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);

        /*Draw the right vertical border*/
        fill_area.x1 = area_outer->x2 - border_width + 1;
        fill_area.x2 = area_outer->x2;

        _lv_blend_fill(clip, &fill_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
    }
    /*Process line by line if there is other mask too*/
    else {
        fill_area.x1 = area_outer->x1;
        fill_area.x2 = area_outer->x2;
        fill_area.y1 = disp_area->y1 + draw_area.y1;
        fill_area.y2 = fill_area.y1;

        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            _lv_memset_ff(mask_buf, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, vdb->area.x1 + draw_area.x1, vdb->area.y1 + h, draw_area_w);

            _lv_blend_fill(clip, &fill_area, color, mask_buf, mask_res, opa, blend_mode);
            fill_area.y1++;
            fill_area.y2++;

        }
    }
    lv_draw_mask_remove_id(mask_rin_id);
    lv_draw_mask_remove_id(mask_rout_id);
    _lv_mem_buf_release(mask_buf);
}


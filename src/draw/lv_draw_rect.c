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
#include "../misc/lv_math.h"
#include "../misc/lv_txt_ap.h"
#include "../core/lv_refr.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define SHADOW_UPSCALE_SHIFT   6
#define SHADOW_ENHANCE          1
#define SPLIT_LIMIT             50

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_USE_EXTERNAL_RENDERER == 0
LV_ATTRIBUTE_FAST_MEM static void draw_bg(const lv_area_t * coords, const lv_area_t * clip_area,
                                          const lv_draw_rect_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void draw_bg_img(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void draw_border(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc);

static void draw_outline(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc);

#if LV_DRAW_COMPLEX
LV_ATTRIBUTE_FAST_MEM static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc);
LV_ATTRIBUTE_FAST_MEM static void shadow_draw_corner_buf(const lv_area_t * coords,  uint16_t * sh_buf, lv_coord_t s,
                                                         lv_coord_t r);
LV_ATTRIBUTE_FAST_MEM static void shadow_blur_corner(lv_coord_t size, lv_coord_t sw, uint16_t * sh_ups_buf);
#endif

void draw_border_generic(const lv_area_t * clip_area, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode);

static void draw_border_simple(const lv_area_t * clip, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa);

#if LV_DRAW_COMPLEX
    LV_ATTRIBUTE_FAST_MEM static inline lv_color_t grad_get(const lv_draw_rect_dsc_t * dsc, lv_coord_t s, lv_coord_t i);
#endif
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
#if defined(LV_SHADOW_CACHE_SIZE) && LV_SHADOW_CACHE_SIZE > 0
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
    lv_memset_00(dsc, sizeof(lv_draw_rect_dsc_t));
    dsc->bg_color = lv_color_white();
    dsc->bg_grad_color = lv_color_black();
    dsc->border_color = lv_color_black();
    dsc->shadow_color = lv_color_black();
    dsc->bg_grad_color_stop = 0xFF;
    dsc->bg_img_symbol_font = LV_FONT_DEFAULT;
    dsc->bg_opa = LV_OPA_COVER;
    dsc->bg_img_opa = LV_OPA_COVER;
    dsc->outline_opa = LV_OPA_COVER;
    dsc->border_opa = LV_OPA_COVER;
    dsc->shadow_opa = LV_OPA_COVER;
    dsc->border_side = LV_BORDER_SIDE_FULL;
}

#if LV_USE_EXTERNAL_RENDERER == 0
/**
 * Draw a rectangle
 * @param coords the coordinates of the rectangle
 * @param mask the rectangle will be drawn only in this mask
 * @param dsc pointer to an initialized `lv_draw_rect_dsc_t` variable
 */
void lv_draw_rect(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(lv_area_get_height(coords) < 1 || lv_area_get_width(coords) < 1) return;
#if LV_DRAW_COMPLEX
    draw_shadow(coords, clip, dsc);
#endif

    draw_bg(coords, clip, dsc);
    draw_bg_img(coords, clip, dsc);

    draw_border(coords, clip, dsc);

    draw_outline(coords, clip, dsc);

    LV_ASSERT_MEM_INTEGRITY();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

LV_ATTRIBUTE_FAST_MEM static void draw_bg(const lv_area_t * coords, const lv_area_t * clip_area,
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

    lv_opa_t opa = dsc->bg_opa >= LV_OPA_MAX ? LV_OPA_COVER : dsc->bg_opa;
    lv_grad_dir_t grad_dir = dsc->bg_grad_dir;
    if(dsc->bg_color.full == dsc->bg_grad_color.full) grad_dir = LV_GRAD_DIR_NONE;

    bool mask_any = lv_draw_mask_is_any(&coords_bg);

    /*Most simple case: just a plain rectangle*/
    if(!mask_any && dsc->radius == 0 && (grad_dir == LV_GRAD_DIR_NONE)) {
        _lv_blend_fill(clip_area, &coords_bg, dsc->bg_color, NULL,
                       LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->blend_mode);
        return;
    }

    /*Complex case: there is gradient, mask, or radius*/
#if LV_DRAW_COMPLEX == 0
    LV_LOG_WARN("Can't draw complex rectangle because LV_DRAW_COMPLEX = 0");
#else
    /*Get clipped fill area which is the real draw area.
     *It is always the same or inside `fill_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &coords_bg, clip_area)) return;

    /*Get the real radius. Can't be larger than the half of the shortest side */
    lv_coord_t coords_w = lv_area_get_width(&coords_bg);
    lv_coord_t coords_h = lv_area_get_height(&coords_bg);
    int32_t short_side = LV_MIN(coords_w, coords_h);
    int32_t rout = LV_MIN(dsc->radius, short_side >> 1);

    /*Add a radius mask if there is radius*/
    int32_t draw_area_w = lv_area_get_width(&draw_area);
    int16_t mask_rout_id = LV_MASK_ID_INV;
    lv_opa_t * mask_buf = NULL;
    lv_draw_mask_radius_param_t mask_rout_param;
    if(rout > 0 || mask_any) {
        mask_buf = lv_mem_buf_get(draw_area_w);
        lv_draw_mask_radius_init(&mask_rout_param, &coords_bg, rout, false);
        mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
    }

    /*In case of horizontal gradient pre-compute a line with a gradient*/
    lv_color_t * grad_map = NULL;
    if(grad_dir == LV_GRAD_DIR_HOR) {
        grad_map = lv_mem_buf_get(coords_w * sizeof(lv_color_t));
        int32_t i;
        for(i = 0; i < coords_w; i++) {
            grad_map[i] = grad_get(dsc, coords_w, i - coords_bg.x1);
        }
    }

    int32_t h;
    lv_draw_mask_res_t mask_res;
    lv_area_t blend_area;
    blend_area.x1 = draw_area.x1;
    blend_area.x2 = draw_area.x2;

    /*There is another mask too. Draw line by line. */
    if(mask_any) {
        for(h = draw_area.y1; h <= draw_area.y2; h++) {
            blend_area.y1 = h;
            blend_area.y2 = h;

            /* Initialize the mask to opa instead of 0xFF and blend with LV_OPA_COVER.
             * It saves calculating the final opa in _lv_blend_fill*/
            lv_memset(mask_buf, opa, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
            if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;

            if(grad_dir == LV_GRAD_DIR_NONE) {
                _lv_blend_fill(clip_area, &blend_area, dsc->bg_color, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_HOR) {
                _lv_blend_map(clip_area, &blend_area, grad_map, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_VER) {
                lv_color_t c = grad_get(dsc, coords_h, h - coords_bg.y1);
                _lv_blend_fill(clip_area, &blend_area, c, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
        }
        goto bg_clean_up;
    }


    /* Draw the top of the rectangle line by line and mirror it to the bottom.
     * If there is no radius this cycle won't run because `h` is always `>= h_end`*/
    blend_area.x1 = draw_area.x1;
    blend_area.x2 = draw_area.x2;
    for(h = 0; h < rout; h++) {
        lv_coord_t top_y = coords_bg.y1 + h;
        lv_coord_t bottom_y = coords_bg.y2 - h;
        if(top_y < draw_area.y1 && bottom_y > draw_area.y2) continue;   /*This line is clipped now*/

        /* Initialize the mask to opa instead of 0xFF and blend with LV_OPA_COVER.
         * It saves calculating the final opa in _lv_blend_fill*/
        lv_memset(mask_buf, opa, draw_area_w);
        mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, top_y, draw_area_w);

        if(top_y >= draw_area.y1) {
            blend_area.y1 = top_y;
            blend_area.y2 = top_y;

            if(grad_dir == LV_GRAD_DIR_NONE) {
                _lv_blend_fill(clip_area, &blend_area, dsc->bg_color, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_HOR) {
                _lv_blend_map(clip_area, &blend_area, grad_map, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_VER) {
                lv_color_t c = grad_get(dsc, coords_h, top_y - coords_bg.y1);
                _lv_blend_fill(clip_area, &blend_area, c, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
        }

        if(bottom_y <= draw_area.y2) {
            blend_area.y1 = bottom_y;
            blend_area.y2 = bottom_y;

            if(grad_dir == LV_GRAD_DIR_NONE) {
                _lv_blend_fill(clip_area, &blend_area, dsc->bg_color, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_HOR) {
                _lv_blend_map(clip_area, &blend_area, grad_map, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_VER) {
                lv_color_t c = grad_get(dsc, coords_h, bottom_y - coords_bg.y1);
                _lv_blend_fill(clip_area, &blend_area, c, mask_buf, mask_res, LV_OPA_COVER, dsc->blend_mode);
            }
        }
    }

    /* Draw the center of the rectangle.*/

    /*If no other masks and no gradient, the center is a simple rectangle*/
    if(!mask_any && grad_dir == LV_GRAD_DIR_NONE) {
        blend_area.y1 = coords_bg.y1 + rout;
        blend_area.y2 = coords_bg.y2 - rout;
        _lv_blend_fill(clip_area, &blend_area, dsc->bg_color, mask_buf, LV_DRAW_MASK_RES_FULL_COVER, opa, dsc->blend_mode);
    }
    /*With gradient and/or mask draw line by line*/
    else {
        mask_res = LV_DRAW_MASK_RES_FULL_COVER;
        int32_t h_end = coords_bg.y2 - rout;
        for(h = coords_bg.y1 + rout; h <= h_end; h++) {
            /*If there is no other mask do not apply mask as in the center there is no radius to mask*/
            if(mask_any) {
                lv_memset_ff(mask_buf, draw_area_w);
                mask_res = lv_draw_mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
            }

            blend_area.y1 = h;
            blend_area.y2 = h;
            if(grad_dir == LV_GRAD_DIR_NONE) {
                _lv_blend_fill(clip_area, &blend_area, dsc->bg_color, mask_buf, mask_res, opa, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_HOR) {
                _lv_blend_map(clip_area, &blend_area, grad_map, mask_buf, mask_res, opa, dsc->blend_mode);
            }
            else if(grad_dir == LV_GRAD_DIR_VER) {
                lv_color_t c = grad_get(dsc, coords_h, h - coords_bg.y1);
                _lv_blend_fill(clip_area, &blend_area, c, mask_buf, mask_res, opa, dsc->blend_mode);
            }
        }
    }


bg_clean_up:
    if(grad_map) lv_mem_buf_release(grad_map);
    if(mask_buf) lv_mem_buf_release(mask_buf);
    if(mask_rout_id != LV_MASK_ID_INV) {
        lv_draw_mask_remove_id(mask_rout_id);
        lv_draw_mask_free_param(&mask_rout_param);
    }

#endif
}

LV_ATTRIBUTE_FAST_MEM static void draw_bg_img(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->bg_img_src == NULL) return;
    if(dsc->bg_img_opa <= LV_OPA_MIN) return;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->bg_img_src);
    if(src_type == LV_IMG_SRC_SYMBOL) {
        lv_point_t size;
        lv_txt_get_size(&size, dsc->bg_img_src, dsc->bg_img_symbol_font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        lv_area_t a;
        a.x1 = coords->x1 + lv_area_get_width(coords) / 2 - size.x / 2;
        a.x2 = a.x1 + size.x - 1;
        a.y1 = coords->y1 + lv_area_get_height(coords) / 2 - size.y / 2;
        a.y2 = a.y1 + size.y - 1;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);
        label_draw_dsc.font = dsc->bg_img_symbol_font;
        label_draw_dsc.color = dsc->bg_img_recolor;
        label_draw_dsc.opa = dsc->bg_img_opa;
        lv_draw_label(&a, clip, &label_draw_dsc, dsc->bg_img_src, NULL);
    }
    else {
        lv_img_header_t header;
        lv_res_t res = lv_img_decoder_get_info(dsc->bg_img_src, &header);
        if(res != LV_RES_OK) {
            LV_LOG_WARN("Couldn't read the background image");
            return;
        }

        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.blend_mode = dsc->blend_mode;
        img_dsc.recolor = dsc->bg_img_recolor;
        img_dsc.recolor_opa = dsc->bg_img_recolor_opa;
        img_dsc.opa = dsc->bg_img_opa;

        /*Center align*/
        if(dsc->bg_img_tiled == false) {
            lv_area_t area;
            area.x1 = coords->x1 + lv_area_get_width(coords) / 2 - header.w / 2;
            area.y1 = coords->y1 + lv_area_get_height(coords) / 2 - header.h / 2;
            area.x2 = area.x1 + header.w - 1;
            area.y2 = area.y1 + header.h - 1;

            lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
        }
        else {
            lv_area_t area;
            area.y1 = coords->y1;
            area.y2 = area.y1 + header.h - 1;

            for(; area.y1 <= coords->y2; area.y1 += header.h, area.y2 += header.h) {

                area.x1 = coords->x1;
                area.x2 = area.x1 + header.w - 1;
                for(; area.x1 <= coords->x2; area.x1 += header.w, area.x2 += header.w) {
                    lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
                }
            }
        }
    }
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
    int32_t rout = dsc->radius;
    int32_t short_side = LV_MIN(coords_w, coords_h);
    if(rout > short_side >> 1) rout = short_side >> 1;

    /*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);
    area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : - (dsc->border_width + rout));
    area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : - (dsc->border_width + rout));

    lv_coord_t rin = rout - dsc->border_width;
    if(rin < 0) rin = 0;

    draw_border_generic(clip, coords, &area_inner, rout, rin, dsc->border_color, dsc->border_opa, dsc->blend_mode);

}


#if LV_DRAW_COMPLEX
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

LV_ATTRIBUTE_FAST_MEM static void draw_shadow(const lv_area_t * coords, const lv_area_t * clip,
                                              const lv_draw_rect_dsc_t * dsc)
{
    /*Check whether the shadow is visible*/
    if(dsc->shadow_width == 0) return;
    if(dsc->shadow_opa <= LV_OPA_MIN) return;

    if(dsc->shadow_width == 1 && dsc->shadow_spread <= 0 &&
       dsc->shadow_ofs_x == 0 && dsc->shadow_ofs_y == 0) {
        return;
    }

    /*Calculate the rectangle which is blurred to get the shadow in `shadow_area`*/
    lv_area_t core_area;
    core_area.x1 = coords->x1  + dsc->shadow_ofs_x - dsc->shadow_spread;
    core_area.x2 = coords->x2  + dsc->shadow_ofs_x + dsc->shadow_spread;
    core_area.y1 = coords->y1  + dsc->shadow_ofs_y - dsc->shadow_spread;
    core_area.y2 = coords->y2  + dsc->shadow_ofs_y + dsc->shadow_spread;

    /*Calculate the bounding box of the shadow*/
    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - dsc->shadow_width / 2 - 1;
    shadow_area.x2 = core_area.x2 + dsc->shadow_width / 2 + 1;
    shadow_area.y1 = core_area.y1 - dsc->shadow_width / 2 - 1;
    shadow_area.y2 = core_area.y2 + dsc->shadow_width / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &shadow_area, clip)) return;

    /*Consider 1 px smaller bg to be sure the edge will be covered by the shadow*/
    lv_area_t bg_area;
    lv_area_copy(&bg_area, coords);
    lv_area_increase(&bg_area, -1, -1);

    /*Get the clamped radius*/
    int32_t r_bg = dsc->radius;
    lv_coord_t short_side = LV_MIN(lv_area_get_width(&bg_area), lv_area_get_height(&bg_area));
    if(r_bg > short_side >> 1) r_bg = short_side >> 1;

    /*Get the clamped radius*/
    int32_t r_sh = dsc->radius;
    short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side >> 1) r_sh = short_side >> 1;

    /*Get how many pixels are affected by the blur on the corners*/
    int32_t corner_size = dsc->shadow_width  + r_sh;

    lv_opa_t * sh_buf;

#if LV_SHADOW_CACHE_SIZE
    if(sh_cache_size == corner_size && sh_cache_r == r_sh) {
        /*Use the cache if available*/
        sh_buf = lv_mem_buf_get(corner_size * corner_size);
        lv_memcpy(sh_buf, sh_cache, corner_size * corner_size);
    }
    else {
        /*A larger buffer is required for calculation*/
        sh_buf = lv_mem_buf_get(corner_size * corner_size * sizeof(uint16_t));
        shadow_draw_corner_buf(&core_area, (uint16_t *)sh_buf, dsc->shadow_width, r_sh);

        /*Cache the corner if it fits into the cache size*/
        if((uint32_t)corner_size * corner_size < sizeof(sh_cache)) {
            lv_memcpy(sh_cache, sh_buf, corner_size * corner_size);
            sh_cache_size = corner_size;
            sh_cache_r = r_sh;
        }
    }
#else
    sh_buf = lv_mem_buf_get(corner_size * corner_size * sizeof(uint16_t));
    shadow_draw_corner_buf(&core_area, (uint16_t *)sh_buf, dsc->shadow_width, r_sh);
#endif

    /*Skip a lot of masking if the background will cover the shadow that would be masked out*/
    bool mask_any = lv_draw_mask_is_any(&shadow_area);
    bool simple = true;
    if(mask_any || dsc->bg_opa < LV_OPA_COVER) simple = false;

    /*Create a radius mask to clip remove shadow on the bg area*/
    lv_draw_mask_res_t mask_res;

    lv_draw_mask_radius_param_t mask_rout_param;
    int16_t mask_rout_id = LV_MASK_ID_INV;
    if(!simple) {
        lv_draw_mask_radius_init(&mask_rout_param, &bg_area, r_bg, true);
        mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
    }
    lv_opa_t * mask_buf = lv_mem_buf_get(lv_area_get_width(&shadow_area));
    lv_area_t blend_area;
    lv_area_t clip_area_sub;
    lv_opa_t ** mask_act;
    lv_opa_t * sh_buf_tmp;
    lv_coord_t y;
    bool simple_sub;

    lv_coord_t w_half = shadow_area.x1 + lv_area_get_width(&shadow_area) / 2;
    lv_coord_t h_half = shadow_area.y1 + lv_area_get_height(&shadow_area) / 2;

    /*Draw the corners if they are on the current clip area and not fully covered by the bg*/

    /*Top right corner*/
    blend_area.x2 = shadow_area.x2;
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    /*Do not overdraw the top other corners*/
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - shadow_area.y1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;
        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Bottom right corner.
     *Almost the same as top right just read the lines of `sh_buf` from then end*/
    blend_area.x2 = shadow_area.x2;
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    /*Do not overdraw the other corners*/
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);
        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;

        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Top side*/
    blend_area.x1 = shadow_area.x1 + corner_size;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - blend_area.y1) * corner_size;

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;

        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memset(mask_buf, sh_buf_tmp[0], w);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                    _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, mask_buf, mask_res, dsc->shadow_opa, dsc->blend_mode);
                }
                else {
                    lv_opa_t line_opa = opa == LV_OPA_COVER ? sh_buf_tmp[0] : (sh_buf_tmp[0] * dsc->shadow_opa) >> 8;
                    _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, line_opa,
                                   dsc->blend_mode);
                }
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Bottom side*/
    blend_area.x1 = shadow_area.x1 + corner_size;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                /*Do not mask if out of the bg*/
                if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
                else simple_sub = simple;

                if(!simple_sub) {
                    lv_memset(mask_buf, sh_buf_tmp[0], w);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                    _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, mask_buf, mask_res, dsc->shadow_opa, dsc->blend_mode);
                }
                else {
                    lv_opa_t line_opa = opa == LV_OPA_COVER ? sh_buf_tmp[0] : (sh_buf_tmp[0] * dsc->shadow_opa) >> 8;
                    _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, NULL, LV_DRAW_MASK_RES_FULL_COVER, line_opa,
                                   dsc->blend_mode);

                }
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Right side*/
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.x2 = shadow_area.x2;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MIN(blend_area.y1, h_half + 1);
    blend_area.y2 = LV_MAX(blend_area.y2, h_half);
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (corner_size - 1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;

        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, w);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
            }
        }
    }

    /*Mirror the shadow corner buffer horizontally*/
    sh_buf_tmp = sh_buf ;
    for(y = 0; y < corner_size; y++) {
        int32_t x;
        lv_opa_t * start = sh_buf_tmp;
        lv_opa_t * end = sh_buf_tmp + corner_size - 1;
        for(x = 0; x < corner_size / 2; x++) {
            lv_opa_t tmp = *start;
            *start = *end;
            *end = tmp;

            start++;
            end--;
        }
        sh_buf_tmp += corner_size;
    }

    /*Left side*/
    blend_area.x1 = shadow_area.x1;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MIN(blend_area.y1, h_half + 1);
    blend_area.y2 = LV_MAX(blend_area.y2, h_half);
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (corner_size - 1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;
        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, w);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
            }
        }
    }

    /*Top left corner*/
    blend_area.x1 = shadow_area.x1;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    /*Do not overdraw the other corners*/
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - blend_area.y1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;

        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Bottom left corner.
     *Almost the same as bottom right just read the lines of `sh_buf` from then end*/
    blend_area.x1 = shadow_area.x1 ;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && _lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        mask_act = simple_sub ? &sh_buf_tmp : &mask_buf;
        if(w > 0) {
            mask_res = LV_DRAW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                    if(mask_res == LV_DRAW_MASK_RES_FULL_COVER) mask_res = LV_DRAW_MASK_RES_CHANGED;
                }
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, *mask_act, mask_res, dsc->shadow_opa, dsc->blend_mode);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Draw the center rectangle.*/
    blend_area.x1 = shadow_area.x1 + corner_size ;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;

    if(_lv_area_intersect(&clip_area_sub, &blend_area, clip) && !_lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        lv_coord_t w = lv_area_get_width(&clip_area_sub);
        if(w > 0) {
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                lv_memset_ff(mask_buf, w);
                mask_res = lv_draw_mask_apply(mask_buf, clip_area_sub.x1, y, w);
                _lv_blend_fill(&clip_area_sub, &blend_area, dsc->shadow_color, mask_buf, mask_res, dsc->shadow_opa, dsc->blend_mode);
            }
        }
    }

    if(!simple) {
        lv_draw_mask_free_param(&mask_rout_param);
        lv_draw_mask_remove_id(mask_rout_id);
    }
    lv_mem_buf_release(sh_buf);
    lv_mem_buf_release(mask_buf);
}

/**
 * Calculate a blurred corner
 * @param coords Coordinates of the shadow
 * @param sh_buf a buffer to store the result. Its size should be `(sw + r)^2 * 2`
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
    lv_opa_t * mask_line = lv_mem_buf_get(size);
    uint16_t * sh_ups_tmp_buf = (uint16_t *)sh_buf;
    for(y = 0; y < size; y++) {
        lv_memset_ff(mask_line, size);
        lv_draw_mask_res_t mask_res = mask_param.dsc.cb(mask_line, 0, y, size, &mask_param);
        if(mask_res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(sh_ups_tmp_buf, size * sizeof(sh_ups_tmp_buf[0]));
        }
        else {
            int32_t i;
            sh_ups_tmp_buf[0] = (mask_line[0] << SHADOW_UPSCALE_SHIFT) / sw;
            for(i = 1; i < size; i++) {
                if(mask_line[i] == mask_line[i - 1]) sh_ups_tmp_buf[i] = sh_ups_tmp_buf[i - 1];
                else  sh_ups_tmp_buf[i] = (mask_line[i] << SHADOW_UPSCALE_SHIFT) / sw;
            }
        }

        sh_ups_tmp_buf += size;
    }
    lv_mem_buf_release(mask_line);

    lv_draw_mask_free_param(&mask_param);

    if(sw == 1) {
        int32_t i;
        lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
        for(i = 0; i < size * size; i++) {
            res_buf[i] = (sh_buf[i] >> SHADOW_UPSCALE_SHIFT);
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
        uint32_t max_v_div = (LV_OPA_COVER << SHADOW_UPSCALE_SHIFT) / sw;
        for(i = 0; i < (uint32_t)size * size; i++) {
            if(sh_buf[i] == 0) continue;
            else if(sh_buf[i] == LV_OPA_COVER) sh_buf[i] = max_v_div;
            else  sh_buf[i] = (sh_buf[i] << SHADOW_UPSCALE_SHIFT) / sw;
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
    uint16_t * sh_ups_blur_buf = lv_mem_buf_get(size * sizeof(uint16_t));

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
        lv_memcpy(sh_ups_tmp_buf, sh_ups_blur_buf, size * sizeof(uint16_t));
        sh_ups_tmp_buf += size;
    }

    /*Vertical blur*/
    uint32_t i;
    uint32_t max_v = LV_OPA_COVER << SHADOW_UPSCALE_SHIFT;
    uint32_t max_v_div = max_v / sw;
    for(i = 0; i < (uint32_t)size * size; i++) {
        if(sh_ups_buf[i] == 0) continue;
        else if(sh_ups_buf[i] == max_v) sh_ups_buf[i] = max_v_div;
        else sh_ups_buf[i] = sh_ups_buf[i] / sw;
    }

    for(x = 0; x < size; x++) {
        sh_ups_tmp_buf = &sh_ups_buf[x];
        int32_t v = sh_ups_tmp_buf[0] * sw;
        for(y = 0; y < size ; y++, sh_ups_tmp_buf += size) {
            sh_ups_blur_buf[y] = v < 0 ? 0 : (v >> SHADOW_UPSCALE_SHIFT);

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

    lv_mem_buf_release(sh_ups_blur_buf);
}

#endif

static void draw_outline(const lv_area_t * coords, const lv_area_t * clip, const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->outline_opa <= LV_OPA_MIN) return;
    if(dsc->outline_width == 0) return;

    lv_opa_t opa = dsc->outline_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get the inner radius*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);

    /*Extend the outline into the background area if it's overlapping the edge*/
    lv_coord_t pad = (dsc->outline_pad == 0 ? (dsc->outline_pad - 1) : dsc->outline_pad);
    area_inner.x1 -= pad;
    area_inner.y1 -= pad;
    area_inner.x2 += pad;
    area_inner.y2 += pad;

    lv_area_t area_outer;
    lv_area_copy(&area_outer, &area_inner);

    area_outer.x1 -= dsc->outline_width;
    area_outer.x2 += dsc->outline_width;
    area_outer.y1 -= dsc->outline_width;
    area_outer.y2 += dsc->outline_width;


    int32_t inner_w = lv_area_get_width(&area_inner);
    int32_t inner_h = lv_area_get_height(&area_inner);
    int32_t rin = dsc->radius;
    int32_t short_side = LV_MIN(inner_w, inner_h);
    if(rin > short_side >> 1) rin = short_side >> 1;

    lv_coord_t rout = rin + dsc->outline_width;

    draw_border_generic(clip, &area_outer, &area_inner, rout, rin, dsc->outline_color, dsc->outline_opa, dsc->blend_mode);
}

void draw_border_generic(const lv_area_t * clip_area, const lv_area_t * outer_area, const lv_area_t * inner_area,
                         lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode)
{
    opa = opa >= LV_OPA_COVER ? LV_OPA_COVER : opa;

    if(rout == 0 || rin == 0) {
        draw_border_simple(clip_area, outer_area, inner_area, color, opa);
        return;
    }

#if LV_DRAW_COMPLEX
    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `coords`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, outer_area, clip_area)) return;
    int32_t draw_area_w = lv_area_get_width(&draw_area);

    bool mask_any = lv_draw_mask_is_any(outer_area);

    /*Create a mask if there is a radius*/
    lv_opa_t * mask_buf = lv_mem_buf_get(draw_area_w);

    /*Create mask for the outer area*/
    int16_t mask_rout_id = LV_MASK_ID_INV;
    lv_draw_mask_radius_param_t mask_rout_param;
    if(rout > 0) {
        lv_draw_mask_radius_init(&mask_rout_param, outer_area, rout, false);
        mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
    }

    /*Create mask for the inner mask*/
    lv_draw_mask_radius_param_t mask_rin_param;
    lv_draw_mask_radius_init(&mask_rin_param, inner_area, rin, true);
    int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

    int32_t h;
    lv_draw_mask_res_t mask_res;
    lv_area_t blend_area;

    /*Calculate the x and y coordinates where the straight parts area*/
    lv_area_t core_area;
    core_area.x1 = LV_MAX(outer_area->x1 + rout, inner_area->x1);
    core_area.x2 = LV_MIN(outer_area->x2 - rout, inner_area->x2);
    core_area.y1 = LV_MAX(outer_area->y1 + rout, inner_area->y1);
    core_area.y2 = LV_MIN(outer_area->y2 - rout, inner_area->y2);
    lv_coord_t core_w = lv_area_get_width(&core_area);

    bool top_side = outer_area->y1 <= inner_area->y1 ? true : false;
    bool bottom_side = outer_area->y2 >= inner_area->y2 ? true : false;

    /*If there is other masks, need to draw line by line*/
    if(mask_any) {
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = draw_area.x2;
        for(h = draw_area.y1; h < draw_area.y2; h++) {
            if(!top_side && h < core_area.y1) continue;
            if(!bottom_side && h > core_area.y2) break;

            blend_area.y1 = h;
            blend_area.y2 = h;

            lv_memset_ff(mask_buf, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, draw_area.x1, h, draw_area_w);
            _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
        }

        lv_draw_mask_free_param(&mask_rin_param);
        lv_draw_mask_remove_id(mask_rin_id);
        lv_draw_mask_free_param(&mask_rout_param);
        lv_draw_mask_remove_id(mask_rout_id);
        lv_mem_buf_release(mask_buf);
        return;
    }

    /*No masks*/
    bool left_side = outer_area->x1 <= inner_area->x1 ? true : false;
    bool right_side = outer_area->x2 >= inner_area->x2 ? true : false;

    bool split_hor = true;
    if(left_side && right_side && top_side && bottom_side &&
       core_w < SPLIT_LIMIT) {
        split_hor = false;
    }

    /*Draw the straight lines first if they are long enough*/
    if(top_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = outer_area->y1;
        blend_area.y2 = inner_area->y1 - 1;
        _lv_blend_fill(clip_area, &blend_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
    }

    if(bottom_side && split_hor) {
        blend_area.x1 = core_area.x1;
        blend_area.x2 = core_area.x2;
        blend_area.y1 = inner_area->y2 + 1;
        blend_area.y2 = outer_area->y2;
        _lv_blend_fill(clip_area, &blend_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
    }

    if(left_side) {
        blend_area.x1 = outer_area->x1;
        blend_area.x2 = inner_area->x1 - 1;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        _lv_blend_fill(clip_area, &blend_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
    }

    if(right_side) {
        blend_area.x1 = inner_area->x2 + 1;
        blend_area.x2 = outer_area->x2;
        blend_area.y1 = core_area.y1;
        blend_area.y2 = core_area.y2;
        _lv_blend_fill(clip_area, &blend_area, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, blend_mode);
    }

    /*Draw the corners*/
    lv_coord_t blend_w;

    /*Left and right corner together is they close to eachother*/
    if(!split_hor) {
        /*Calculate the top corner and mirror it to the bottom*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = draw_area.x2;
        lv_coord_t max_h = LV_MAX(rout, outer_area->y1 - inner_area->y1);
        for(h = 0; h < max_h; h++) {
            lv_coord_t top_y = outer_area->y1 + h;
            lv_coord_t bottom_y = outer_area->y2 - h;
            if(top_y < draw_area.y1 && bottom_y > draw_area.y2) continue;   /*This line is clipped now*/

            lv_memset_ff(mask_buf, draw_area_w);
            mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, top_y, draw_area_w);

            if(top_y >= draw_area.y1) {
                blend_area.y1 = top_y;
                blend_area.y2 = top_y;
                _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
            }

            if(bottom_y <= draw_area.y2) {
                blend_area.y1 = bottom_y;
                blend_area.y2 = bottom_y;
                _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
            }
        }
    }
    else {
        /*Left corners*/
        blend_area.x1 = draw_area.x1;
        blend_area.x2 = LV_MIN(draw_area.x2, core_area.x1 - 1);
        blend_w = lv_area_get_width(&blend_area);
        if(blend_w > 0) {
            if(left_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset_ff(mask_buf, blend_w);
                    mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, blend_w);
                    _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
                }
            }

            if(left_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset_ff(mask_buf, blend_w);
                    mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, blend_w);
                    _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
                }
            }
        }

        /*Right corners*/
        blend_area.x1 = LV_MAX(draw_area.x1, core_area.x2 + 1);
        blend_area.x2 = draw_area.x2;
        blend_w = lv_area_get_width(&blend_area);

        if(blend_w > 0) {
            if(right_side || top_side) {
                for(h = draw_area.y1; h < core_area.y1; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset_ff(mask_buf, blend_w);
                    mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, blend_w);
                    _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
                }
            }

            if(right_side || bottom_side) {
                for(h = core_area.y2 + 1; h <= draw_area.y2; h++) {
                    blend_area.y1 = h;
                    blend_area.y2 = h;

                    lv_memset_ff(mask_buf, blend_w);
                    mask_res = lv_draw_mask_apply(mask_buf, blend_area.x1, h, blend_w);
                    _lv_blend_fill(clip_area, &blend_area, color, mask_buf, mask_res, opa, blend_mode);
                }
            }
        }
    }

    lv_draw_mask_free_param(&mask_rin_param);
    lv_draw_mask_remove_id(mask_rin_id);
    lv_draw_mask_free_param(&mask_rout_param);
    lv_draw_mask_remove_id(mask_rout_id);
    lv_mem_buf_release(mask_buf);

#else /*LV_DRAW_COMPLEX*/
    LV_UNUSED(blend_mode);
#endif /*LV_DRAW_COMPLEX*/
}

static void draw_border_simple(const lv_area_t * clip, const lv_area_t * outer_area, const lv_area_t * inner_area,
                               lv_color_t color, lv_opa_t opa)
{
    bool top_side = outer_area->y1 <= inner_area->y1 ? true : false;
    bool bottom_side = outer_area->y2 >= inner_area->y2 ? true : false;
    bool left_side = outer_area->x1 <= inner_area->x1 ? true : false;
    bool right_side = outer_area->x2 >= inner_area->x2 ? true : false;

    lv_area_t a;
    /*Top*/
    a.x1 = outer_area->x1;
    a.x2 = outer_area->x2;
    a.y1 = outer_area->y1;
    a.y2 = inner_area->y1 - 1;
    if(top_side) {
        _lv_blend_fill(clip, &a, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, LV_BLEND_MODE_NORMAL);
    }

    /*Bottom*/
    a.y1 = inner_area->y2 + 1;
    a.y2 = outer_area->y2;
    if(bottom_side) {
        _lv_blend_fill(clip, &a, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, LV_BLEND_MODE_NORMAL);
    }

    /*Left*/
    a.x1 = outer_area->x1;
    a.x2 = inner_area->x1 - 1;
    a.y1 = (top_side) ? inner_area->y1 : outer_area->y1;
    a.y2 = (bottom_side) ? inner_area->y2 : outer_area->y2;
    if(left_side) {
        _lv_blend_fill(clip, &a, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, LV_BLEND_MODE_NORMAL);
    }

    /*Right*/
    a.x1 = inner_area->x2 + 1;
    a.x2 = outer_area->x2;
    if(right_side) {
        _lv_blend_fill(clip, &a, color, NULL, LV_DRAW_MASK_RES_FULL_COVER, opa, LV_BLEND_MODE_NORMAL);
    }

}

#endif
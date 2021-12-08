/**
 * @file lv_draw_sdl_draw_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_SDL

#include "../lv_draw_rect.h"
#include "../lv_draw_img.h"
#include "../lv_draw_label.h"
#include "../lv_draw_mask.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_stack_blur.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t radius;
    lv_coord_t size;
} lv_draw_rect_bg_key_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t radius;
    lv_coord_t size;
    lv_coord_t blur;
} lv_draw_rect_shadow_key_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    lv_coord_t rout, rin;
    lv_coord_t thickness;
    lv_coord_t size;
    lv_border_side_t side;
} lv_draw_rect_border_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_bg_color(lv_draw_sdl_context_t *ctx, const lv_area_t *coords, const lv_area_t *clipped,
                          const lv_draw_rect_dsc_t *dsc);

static void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_border(lv_draw_sdl_context_t *ctx, const lv_area_t *coords, const lv_area_t *clipped,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const lv_area_t *clip,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_outline(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc);

static void draw_border_generic(const lv_area_t *outer_area, const lv_area_t *inner_area, const lv_area_t *clip,
                                lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa,
                                lv_blend_mode_t blend_mode);

static void draw_border_simple(const lv_area_t *outer_area, const lv_area_t *inner_area, lv_color_t color,
                               lv_opa_t opa);

static void frag_render_corners(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size,
                                const lv_area_t *coords, const lv_area_t *clip);

static void frag_render_borders(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size,
                                const lv_area_t *coords, const lv_area_t *clipped);

static void frag_render_center(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size,
                               const lv_area_t *coords, const lv_area_t *clipped);

static lv_draw_rect_bg_key_t rect_bg_key_create(lv_coord_t radius, lv_coord_t size);

static lv_draw_rect_shadow_key_t rect_shadow_key_create(lv_coord_t radius, lv_coord_t size, lv_coord_t blur);

static lv_draw_rect_border_key_t rect_border_key_create(lv_coord_t rout, lv_coord_t rin, lv_coord_t thickness,
                                                        lv_coord_t size, lv_border_side_t side);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define SKIP_BORDER(dsc) ((dsc)->border_opa <= LV_OPA_MIN || (dsc)->border_width == 0 || (dsc)->border_side == LV_BORDER_SIDE_NONE || (dsc)->border_post)
#define SKIP_SHADOW(dsc) ((dsc)->shadow_width == 0 || (dsc)->shadow_opa <= LV_OPA_MIN || ((dsc)->shadow_width == 1 && (dsc)->shadow_spread <= 0 && (dsc)->shadow_ofs_x == 0 && (dsc)->shadow_ofs_y == 0))
#define SKIP_IMAGE(dsc) ((dsc)->bg_img_src == NULL || (dsc)->bg_img_opa <= LV_OPA_MIN)
#define SKIP_OUTLINE(dsc) ((dsc)->outline_opa <= LV_OPA_MIN || (dsc)->outline_width == 0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_draw_rect(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc) {
    lv_area_t draw_area;
    bool has_draw_content = _lv_area_intersect(&draw_area, coords, clip);

    lv_draw_sdl_context_t *ctx = lv_draw_sdl_get_context();

    /* Coords will be translated so coords will start at (0,0) */
    lv_area_t t_coords = *coords, t_area = draw_area;
    bool has_mask = has_draw_content && lv_draw_sdl_mask_begin(&t_area, &t_coords);

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(clip, &clip_rect);
    draw_shadow(ctx->renderer, &t_coords, &t_area, dsc);
    /* Shadows and outlines will also draw in extended area */
    if (has_draw_content) {
        draw_bg_color(ctx, &t_coords, &t_area, dsc);
        draw_bg_img(&t_coords, &t_area, dsc);
        draw_border(ctx, &t_coords, &t_area, dsc);
    }
    draw_outline(&t_coords, &t_area, dsc);

    if (has_mask) {
        lv_draw_sdl_mask_end(&draw_area);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg_color(lv_draw_sdl_context_t *ctx, const lv_area_t *coords, const lv_area_t *clipped,
                          const lv_draw_rect_dsc_t *dsc) {
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);
    lv_coord_t radius = dsc->radius;
    if (radius <= 0) {
        SDL_Rect rect;
        lv_area_to_sdl_rect(clipped, &rect);
        SDL_SetRenderDrawColor(ctx->renderer, bg_color.r, bg_color.g, bg_color.b, dsc->bg_opa);
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(ctx->renderer, &rect);
        return;
    }

    /*A small texture with a quarter of the rect is enough*/
    lv_coord_t bg_w = lv_area_get_width(coords), bg_h = lv_area_get_height(coords), bg_min = LV_MIN(bg_w, bg_h);
    /* If size isn't times of 2, increase 1 px */
    lv_coord_t min_half = bg_min % 2 == 0 ? bg_min / 2 : bg_min / 2 + 1;
    lv_coord_t frag_size = radius == LV_RADIUS_CIRCLE ? min_half : LV_MIN(radius + 1, min_half);
    lv_draw_rect_bg_key_t key = rect_bg_key_create(radius, frag_size);
    lv_area_t coords_frag;
    lv_area_copy(&coords_frag, coords);
    lv_area_set_width(&coords_frag, frag_size);
    lv_area_set_height(&coords_frag, frag_size);
    SDL_Texture *texture = lv_draw_sdl_texture_cache_get(&key, sizeof(key), NULL);
    if (texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, coords, radius, false);
        int16_t mask_id = lv_draw_mask_add(&mask_rout_param, NULL);
        texture = lv_sdl_gen_mask_texture(ctx->renderer, &coords_frag, &mask_id, 1);
        lv_draw_mask_remove_id(mask_id);
        SDL_assert(texture);
        lv_draw_sdl_texture_cache_put(&key, sizeof(key), texture);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
    SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
    frag_render_corners(ctx->renderer, texture, frag_size, coords, clipped);
    frag_render_borders(ctx->renderer, texture, frag_size, coords, clipped);
    frag_render_center(ctx->renderer, texture, frag_size, coords, clipped);
}

static void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc) {
    if (SKIP_IMAGE(dsc)) return;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->bg_img_src);
    if (src_type == LV_IMG_SRC_SYMBOL) {
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
    } else {
        lv_img_header_t header;
        size_t key_size;
        lv_draw_sdl_cache_key_head_img_t *key = lv_draw_sdl_texture_img_key_create(dsc->bg_img_src, 0, &key_size);
        bool key_found;
        lv_img_header_t *cache_header = NULL;
        SDL_Texture *texture = lv_draw_sdl_texture_cache_get_with_userdata(key, key_size, &key_found,
                                                                           (void **) &cache_header);
        SDL_free(key);
        if (texture) {
            header = *cache_header;
        } else if (key_found || lv_img_decoder_get_info(dsc->bg_img_src, &header) != LV_RES_OK) {
            /* When cache hit but with negative result, use default decoder. If still fail, return.*/
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
        if (dsc->bg_img_tiled == false) {
            lv_area_t area;
            area.x1 = coords->x1 + lv_area_get_width(coords) / 2 - header.w / 2;
            area.y1 = coords->y1 + lv_area_get_height(coords) / 2 - header.h / 2;
            area.x2 = area.x1 + header.w - 1;
            area.y2 = area.y1 + header.h - 1;

            lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
        } else {
            lv_area_t area;
            area.y1 = coords->y1;
            area.y2 = area.y1 + header.h - 1;

            for (; area.y1 <= coords->y2; area.y1 += header.h, area.y2 += header.h) {

                area.x1 = coords->x1;
                area.x2 = area.x1 + header.w - 1;
                for (; area.x1 <= coords->x2; area.x1 += header.w, area.x2 += header.w) {
                    lv_draw_img(&area, clip, dsc->bg_img_src, &img_dsc);
                }
            }
        }
    }
}

static void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const lv_area_t *clip,
                        const lv_draw_rect_dsc_t *dsc) {
    /*Check whether the shadow is visible*/
    if (SKIP_SHADOW(dsc)) return;

    lv_coord_t sw = dsc->shadow_width;

    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->shadow_ofs_x - dsc->shadow_spread;
    core_area.x2 = coords->x2 + dsc->shadow_ofs_x + dsc->shadow_spread;
    core_area.y1 = coords->y1 + dsc->shadow_ofs_y - dsc->shadow_spread;
    core_area.y2 = coords->y2 + dsc->shadow_ofs_y + dsc->shadow_spread;

    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - sw / 2 - 1;
    shadow_area.x2 = core_area.x2 + sw / 2 + 1;
    shadow_area.y1 = core_area.y1 - sw / 2 - 1;
    shadow_area.y2 = core_area.y2 + sw / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;

    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t draw_area;
    if (!_lv_area_intersect(&draw_area, &shadow_area, clip)) return;

    SDL_Rect core_area_rect;
    lv_area_to_sdl_rect(&shadow_area, &core_area_rect);

    lv_coord_t radius = dsc->radius;
    lv_coord_t sh_width = lv_area_get_width(&core_area);
    lv_coord_t sh_height = lv_area_get_height(&core_area);
    lv_coord_t sh_min = LV_MIN(sh_width, sh_height);
    /* If size isn't times of 2, increase 1 px */
    lv_coord_t min_half = sh_min % 2 == 0 ? sh_min / 2 : sh_min / 2 + 1;
    /* No matter how big the shadow is, what we need is just a corner */
    lv_coord_t frag_size = radius == LV_RADIUS_CIRCLE ? min_half : LV_MIN(radius + 1, min_half);
    /* This is how big the corner is after blurring */
    lv_coord_t blur_frag_size = frag_size + sw + 2;

    lv_draw_rect_shadow_key_t key = rect_shadow_key_create(radius, frag_size, sw);

    lv_area_t blur_frag;
    lv_area_copy(&blur_frag, &shadow_area);
    lv_area_set_width(&blur_frag, blur_frag_size * 2);
    lv_area_set_height(&blur_frag, blur_frag_size * 2);
    SDL_Texture *texture = lv_draw_sdl_texture_cache_get(&key, sizeof(key), NULL);
    if (texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &core_area, radius, false);
        int16_t mask_id = lv_draw_mask_add(&mask_rout_param, NULL);
        lv_opa_t *mask_buf = lv_draw_mask_dump(&blur_frag, &mask_id, 1);
        lv_stack_blur_grayscale(mask_buf, lv_area_get_width(&blur_frag), lv_area_get_height(&blur_frag), sw / 2 + 1);
        texture = lv_sdl_create_mask_texture(renderer, mask_buf, blur_frag_size, blur_frag_size,
                                             lv_area_get_width(&blur_frag));
        lv_mem_buf_release(mask_buf);
        lv_draw_mask_remove_id(mask_id);
        SDL_assert(texture);
        lv_draw_sdl_texture_cache_put(&key, sizeof(key), texture);
    }

    SDL_Color shadow_color;
    lv_color_to_sdl_color(&dsc->shadow_color, &shadow_color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, shadow_color.r, shadow_color.g, shadow_color.b);

    frag_render_corners(renderer, texture, blur_frag_size, &shadow_area, clip);
    frag_render_borders(renderer, texture, blur_frag_size, &shadow_area, clip);
    frag_render_center(renderer, texture, blur_frag_size, &shadow_area, clip);
}


static void draw_border(lv_draw_sdl_context_t *ctx, const lv_area_t *coords, const lv_area_t *clipped,
                        const lv_draw_rect_dsc_t *dsc) {
    if (SKIP_BORDER(dsc)) return;

    SDL_Color border_color;
    lv_color_to_sdl_color(&dsc->border_color, &border_color);


    if (dsc->border_side != LV_BORDER_SIDE_FULL) {
        SDL_SetRenderDrawColor(ctx->renderer, border_color.r, border_color.g, border_color.b, dsc->border_opa);
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
        for (int w = 0; w <= dsc->border_width; w++) {
            if (dsc->border_side & LV_BORDER_SIDE_TOP) {
                SDL_RenderDrawLine(ctx->renderer, coords->x1, coords->y1 + w, coords->x2, coords->y1 + w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_BOTTOM) {
                SDL_RenderDrawLine(ctx->renderer, coords->x1, coords->y2 - w, coords->x2, coords->y2 - w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_LEFT) {
                SDL_RenderDrawLine(ctx->renderer, coords->x1 + w, coords->y1, coords->x1 + w, coords->y2);
            }
            if (dsc->border_side & LV_BORDER_SIDE_RIGHT) {
                SDL_RenderDrawLine(ctx->renderer, coords->x2 - w, coords->y1, coords->x2 - w, coords->y2);
            }
        }
    } else {
        int32_t coords_w = lv_area_get_width(coords);
        int32_t coords_h = lv_area_get_height(coords);
        lv_coord_t rout = dsc->radius;
        int32_t short_side = LV_MIN(coords_w, coords_h);
        if (rout > short_side >> 1) rout = short_side >> 1;

        /*Get the inner area*/
        lv_area_t area_inner;
        lv_area_copy(&area_inner, coords);
        area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : -(dsc->border_width + rout));
        area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : -(dsc->border_width + rout));

        lv_coord_t rin = rout - dsc->border_width;
        if (rin < 0) rin = 0;
        draw_border_generic(coords, &area_inner, clipped, rout, rin, dsc->border_color, dsc->border_opa,
                            dsc->blend_mode);
    }
}

static void draw_outline(const lv_area_t *coords, const lv_area_t *clip, const lv_draw_rect_dsc_t *dsc) {
    if (SKIP_OUTLINE(dsc)) return;

    lv_opa_t opa = dsc->outline_opa;

    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

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

    lv_area_t draw_area;
    if (!_lv_area_intersect(&draw_area, &area_outer, clip)) return;

    int32_t inner_w = lv_area_get_width(&area_inner);
    int32_t inner_h = lv_area_get_height(&area_inner);
    lv_coord_t rin = dsc->radius;
    int32_t short_side = LV_MIN(inner_w, inner_h);
    if (rin > short_side >> 1) rin = short_side >> 1;

    lv_coord_t rout = rin + dsc->outline_width;

    draw_border_generic(&area_outer, &area_inner, clip, rout, rin, dsc->outline_color, dsc->outline_opa,
                        dsc->blend_mode);
}

static void draw_border_generic(const lv_area_t *outer_area, const lv_area_t *inner_area, const lv_area_t *clip,
                                lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa,
                                lv_blend_mode_t blend_mode) {
    opa = opa >= LV_OPA_COVER ? LV_OPA_COVER : opa;

    if (rout == 0 || rin == 0) {
        draw_border_simple(outer_area, inner_area, color, opa);
        return;
    }

    lv_draw_sdl_context_t *ctx = lv_draw_sdl_get_context();
    SDL_Renderer *renderer = ctx->renderer;

    lv_coord_t border_width = lv_area_get_width(outer_area);
    lv_coord_t border_height = lv_area_get_height(outer_area);
    lv_coord_t border_min = LV_MIN(border_width, border_height);
    lv_coord_t min_half = border_min % 2 == 0 ? border_min / 2 : border_min / 2 + 1;
    lv_coord_t frag_size = rout == LV_RADIUS_CIRCLE ? min_half : LV_MIN(rout + 1, min_half);
    lv_draw_rect_border_key_t key = rect_border_key_create(rout, rin, inner_area->x1 - outer_area->x1 + 1,
                                                           frag_size, LV_BORDER_SIDE_FULL);
    SDL_Texture *texture = lv_draw_sdl_texture_cache_get(&key, sizeof(key), NULL);
    if (texture == NULL) {
        /*Create mask for the outer area*/
        int16_t mask_ids[2] = {LV_MASK_ID_INV, LV_MASK_ID_INV};
        lv_draw_mask_radius_param_t mask_rout_param;
        if (rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, outer_area, rout, false);
            mask_ids[0] = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        /*Create mask for the inner mask*/
        if (rin < 0) rin = 0;
        lv_draw_mask_radius_param_t mask_rin_param;
        lv_draw_mask_radius_init(&mask_rin_param, inner_area, rin, true);
        mask_ids[1] = lv_draw_mask_add(&mask_rin_param, NULL);

        lv_area_t frag_area;
        lv_area_copy(&frag_area, outer_area);
        lv_area_set_width(&frag_area, frag_size);
        lv_area_set_height(&frag_area, frag_size);

        texture = lv_sdl_gen_mask_texture(renderer, &frag_area, mask_ids, 2);

        lv_draw_mask_remove_id(mask_ids[1]);
        lv_draw_mask_remove_id(mask_ids[0]);
        SDL_assert(texture);
        lv_draw_sdl_texture_cache_put(&key, sizeof(key), texture);
    }

    SDL_Rect outer_rect;
    lv_area_to_sdl_rect(outer_area, &outer_rect);
    SDL_Color color_sdl;
    lv_color_to_sdl_color(&color, &color_sdl);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color_sdl.r, color_sdl.g, color_sdl.b);

    frag_render_corners(renderer, texture, frag_size, outer_area, clip);
    frag_render_borders(renderer, texture, frag_size, outer_area, clip);
}

static void draw_border_simple(const lv_area_t *outer_area, const lv_area_t *inner_area, lv_color_t color,
                               lv_opa_t opa) {

    lv_draw_sdl_context_t *ctx = lv_draw_sdl_get_context();
    SDL_Renderer *renderer = ctx->renderer;

    SDL_Color color_sdl;
    lv_color_to_sdl_color(&color, &color_sdl);

    SDL_SetRenderDrawColor(renderer, color_sdl.r, color_sdl.g, color_sdl.b, opa);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Rect simple_rect;
    simple_rect.w = inner_area->x2 - outer_area->x1 + 1;
    simple_rect.h = inner_area->y1 - outer_area->y1 + 1;
    /*Top border*/
    simple_rect.x = outer_area->x1;
    simple_rect.y = outer_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);
    /*Bottom border*/
    simple_rect.x = inner_area->x1;
    simple_rect.y = inner_area->y2;
    SDL_RenderFillRect(renderer, &simple_rect);

    simple_rect.w = inner_area->x1 - outer_area->x1 + 1;
    simple_rect.h = inner_area->y2 - outer_area->y1 + 1;
    /*Left border*/
    simple_rect.x = outer_area->x1;
    simple_rect.y = inner_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);
    /*Right border*/
    simple_rect.x = inner_area->x2;
    simple_rect.y = outer_area->y1;
    SDL_RenderFillRect(renderer, &simple_rect);

}

static void frag_render_corners(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size,
                                const lv_area_t *coords, const lv_area_t *clipped) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    lv_area_t corner_area, dst_area;
    /* Upper left */
    corner_area.x1 = coords->x1;
    corner_area.y1 = coords->y1;
    corner_area.x2 = coords->x1 + frag_size - 1;
    corner_area.y2 = coords->y1 + frag_size - 1;
    if (_lv_area_intersect(&dst_area, &corner_area, clipped)) {
        SDL_Rect src_rect = {
                .x = dst_area.x1 - coords->x1,
                .y = dst_area.y1 - coords->y1,
                .w = lv_area_get_width(&dst_area),
                .h = lv_area_get_height(&dst_area),
        };
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Upper right, clip right edge if too big */
    corner_area.x1 = LV_MAX(coords->x2 - frag_size, coords->x1 + frag_size);
    corner_area.x2 = coords->x2;
    if (_lv_area_intersect(&dst_area, &corner_area, clipped)) {
        lv_coord_t dw = lv_area_get_width(&dst_area), dh = lv_area_get_height(&dst_area);
        SDL_Rect src_rect = {
                .x = coords->x2 - dst_area.x2,
                .y = dst_area.y1 - coords->y1,
                .w = dw,
                .h = dh,
        };
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Lower right, clip bottom edge if too big */
    corner_area.y1 = LV_MAX(coords->y2 - frag_size, coords->y1 + frag_size);
    corner_area.y2 = coords->y2;
    if (_lv_area_intersect(&dst_area, &corner_area, clipped)) {
        SDL_Rect src_rect = {
                .x = coords->x2 - dst_area.x2,
                .y = coords->y2 - dst_area.y2,
                .w = lv_area_get_width(&dst_area),
                .h = lv_area_get_height(&dst_area),
        };
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Lower left, right edge should not be clipped */
    corner_area.x1 = coords->x1;
    corner_area.x2 = coords->x1 + frag_size - 1;
    if (_lv_area_intersect(&dst_area, &corner_area, clipped)) {
        SDL_Rect src_rect = {
                .x = dst_area.x1 - coords->x1,
                .y = coords->y2 - dst_area.y2,
                .w = lv_area_get_width(&dst_area),
                .h = lv_area_get_height(&dst_area),
        };
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_VERTICAL);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
}

static void frag_render_borders(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size,
                                const lv_area_t *coords, const lv_area_t *clipped) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    lv_area_t border_area, dst_area;
    /* Top border */
    border_area.x1 = coords->x1 + frag_size;
    border_area.y1 = coords->y1;
    border_area.x2 = coords->x2 - frag_size - 1;
    border_area.y2 = coords->y1 + frag_size - 1;
    if (_lv_area_intersect(&dst_area, &border_area, clipped)) {
        lv_coord_t dh = lv_area_get_height(&dst_area);
        SDL_Rect src_rect = {.x = frag_size - 1, .y = dst_area.y1 - border_area.y1, .w = 1, .h = dh};
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Bottom border */
    border_area.y1 = LV_MAX(coords->y2 - frag_size, coords->y1 + frag_size);
    border_area.y2 = coords->y2;
    if (_lv_area_intersect(&dst_area, &border_area, clipped)) {
        lv_coord_t dh = lv_area_get_height(&dst_area), bh = lv_area_get_height(&border_area);
        SDL_Rect src_rect = {.x = frag_size - 1, .y = (bh - dh) - (dst_area.y2 - border_area.y2), .w = 1, .h = dh};
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        LV_ASSERT(src_rect.h == dst_rect.h);
        SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_VERTICAL);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Left border */
    border_area.x1 = coords->x1;
    border_area.y1 = coords->y1 + frag_size;
    border_area.x2 = coords->x1 + frag_size - 1;
    border_area.y2 = coords->y2 - frag_size - 1;
    if (_lv_area_intersect(&dst_area, &border_area, clipped)) {
        lv_coord_t dw = lv_area_get_width(&dst_area);
        SDL_Rect src_rect = {.x = 0, .y = frag_size - 1, .w = dw, .h = 1};
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
    /* Right border */
    border_area.x1 = LV_MAX(coords->x2 - frag_size, coords->x1 + frag_size);
    border_area.x2 = coords->x2;
    if (_lv_area_intersect(&dst_area, &border_area, clipped)) {
        lv_coord_t dw = lv_area_get_width(&dst_area), bw = lv_area_get_width(&border_area);
        SDL_Rect src_rect = {.x =(bw - dw) - (dst_area.x2 - border_area.x2), .y = frag_size - 1, .w = dw, .h = 1};
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);
        SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
//        SDL_RenderDrawRect(renderer, &dst_rect);
    }
}

static void frag_render_center(SDL_Renderer *renderer, SDL_Texture *frag, lv_coord_t frag_size, const lv_area_t *coords,
                               const lv_area_t *clipped) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    lv_area_t center_area = {
            coords->x1 + frag_size,
            coords->y1 + frag_size,
            coords->x2 - frag_size - 1,
            coords->y2 - frag_size - 1,
    };
    if (center_area.x2 < center_area.x1 || center_area.y2 < center_area.y1) return;
    lv_area_t draw_area;
    if (!_lv_area_intersect(&draw_area, &center_area, clipped)) {
        return;
    }
    SDL_Rect dst_rect;
    lv_area_to_sdl_rect(&draw_area, &dst_rect);
    SDL_Rect src_rect = {frag_size - 1, frag_size - 1, 1, 1};
    SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
}

static lv_draw_rect_bg_key_t rect_bg_key_create(lv_coord_t radius, lv_coord_t size) {
    lv_draw_rect_bg_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_BG;
    key.radius = radius;
    key.size = size;
    return key;
}

static lv_draw_rect_shadow_key_t rect_shadow_key_create(lv_coord_t radius, lv_coord_t size, lv_coord_t blur) {
    lv_draw_rect_shadow_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW;
    key.radius = radius;
    key.size = size;
    key.blur = blur;
    return key;
}

static lv_draw_rect_border_key_t rect_border_key_create(lv_coord_t rout, lv_coord_t rin, lv_coord_t thickness,
                                                        lv_coord_t size, lv_border_side_t side) {
    lv_draw_rect_border_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER;
    key.rout = rout;
    key.rin = rin;
    key.thickness = thickness;
    key.size = size;
    key.side = side;
    return key;
}

#endif /*LV_USE_DRAW_SDL*/

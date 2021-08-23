//
// Created by Mariotaku on 2021/08/21.
//
#include "draw/lv_draw_rect.h"
#include "hal/lv_hal_disp.h"
#include "core/lv_refr.h"
#include "lv_gpu_sdl2_utils.h"
#include "lv_gpu_sdl2_lru.h"
#include "lv_gpu_draw_cache.h"
#include "lv_gpu_sdl2_mask.h"

typedef struct {
    lv_coord_t width;
    lv_coord_t height;
    lv_coord_t radius;
} lv_draw_rect_bg_key_t;

typedef struct {
    lv_point_t size;
    lv_coord_t radius;
    lv_coord_t blur;
    lv_point_t offset;
} lv_draw_rect_shadow_key_t;

typedef struct {
    lv_draw_rect_bg_key_t bg;
    lv_coord_t thickness;
    lv_border_side_t side;
} lv_draw_rect_border_key_t;

static void draw_bg_color(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect,
                          const SDL_Rect *mask_rect, const lv_draw_rect_dsc_t *dsc);

static void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_border(SDL_Renderer *renderer, const lv_area_t *coords,
                        const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                        const lv_draw_rect_dsc_t *dsc);

static void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                        const lv_draw_rect_dsc_t *dsc);

LV_ATTRIBUTE_FAST_MEM static void draw_bg_compat(SDL_Renderer *renderer, const lv_area_t *coords,
                                                 const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                                                 const lv_draw_rect_dsc_t *dsc);

void lv_draw_rect(const lv_area_t *coords, const lv_area_t *mask, const lv_draw_rect_dsc_t *dsc) {
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    SDL_Rect coords_rect, mask_rect, render_rect;
    lv_area_to_sdl_rect(coords, &coords_rect);
    lv_area_to_sdl_rect(mask, &mask_rect);
    SDL_IntersectRect(&coords_rect, &mask_rect, &render_rect);
    if (SDL_RectEmpty(&render_rect)) {
        return;
    }
    if (lv_draw_mask_get_cnt() > 0) {
        draw_bg_compat(renderer, coords, &coords_rect, &mask_rect, dsc);
    } else {
        draw_shadow(renderer, coords, &mask_rect, dsc);
        draw_bg_color(renderer, coords, &coords_rect, &mask_rect, dsc);
        draw_bg_img(coords, mask, dsc);
        draw_border(renderer, coords, &coords_rect, &mask_rect, dsc);
    }
}

void draw_bg_color(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect,
                   const SDL_Rect *mask_rect, const lv_draw_rect_dsc_t *dsc) {
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);
    if (dsc->radius > 0) {
        lv_draw_rect_bg_key_t key = {
                .width = (lv_coord_t) coords_rect->w,
                .height =(lv_coord_t) coords_rect->h,
                .radius = dsc->radius,
        };
        SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
        if (texture == NULL) {
            lv_draw_mask_radius_param_t mask_rout_param;
            lv_draw_mask_radius_init(&mask_rout_param, coords, dsc->radius, false);
            int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
            texture = lv_sdl2_gen_mask_texture(renderer, coords);
            lv_draw_mask_remove_id(mask_rout_id);
            SDL_assert(texture);
            lv_gpu_draw_cache_put(&key, sizeof(key), texture);
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
        SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_RenderCopy(renderer, texture, NULL, coords_rect);
    } else {
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, dsc->bg_opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_RenderFillRect(renderer, coords_rect);
    }
}

void draw_bg_img(const lv_area_t *coords, const lv_area_t *clip,
                 const lv_draw_rect_dsc_t *dsc) {
    if (dsc->bg_img_src == NULL) return;
    if (dsc->bg_img_opa <= LV_OPA_MIN) return;

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
        lv_res_t res = lv_img_decoder_get_info(dsc->bg_img_src, &header);
        if (res != LV_RES_OK) {
            LV_LOG_WARN("Coudn't read the background image");
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

void draw_shadow(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *mask_rect,
                 const lv_draw_rect_dsc_t *dsc) {
    /*Check whether the shadow is visible*/
    if (dsc->shadow_width == 0) return;
    if (dsc->shadow_opa <= LV_OPA_MIN) return;

    if (dsc->shadow_width == 1 && dsc->shadow_ofs_x == 0 &&
        dsc->shadow_ofs_y == 0 && dsc->shadow_spread <= 0) {
        return;
    }

    int32_t sw = dsc->shadow_width;

    lv_area_t sh_rect_area;
    sh_rect_area.x1 = coords->x1 + dsc->shadow_ofs_x - dsc->shadow_spread;
    sh_rect_area.x2 = coords->x2 + dsc->shadow_ofs_x + dsc->shadow_spread;
    sh_rect_area.y1 = coords->y1 + dsc->shadow_ofs_y - dsc->shadow_spread;
    sh_rect_area.y2 = coords->y2 + dsc->shadow_ofs_y + dsc->shadow_spread;

    lv_area_t sh_area;
    sh_area.x1 = sh_rect_area.x1 - sw / 2 - 1;
    sh_area.x2 = sh_rect_area.x2 + sw / 2 + 1;
    sh_area.y1 = sh_rect_area.y1 - sw / 2 - 1;
    sh_area.y2 = sh_rect_area.y2 + sw / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;

    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    SDL_Rect sh_area_rect;
    lv_area_to_sdl_rect(&sh_area, &sh_area_rect);

    SDL_Color shadow_color;
    lv_color_to_sdl_color(&dsc->shadow_color, &shadow_color);
    uint16_t shadow_radius = dsc->radius;
    lv_draw_rect_shadow_key_t key = {
            .size= {lv_area_get_width(&sh_area), lv_area_get_height(&sh_area)},
            .radius = shadow_radius,
            .blur = dsc->shadow_width,
            .offset = {dsc->shadow_ofs_x, dsc->shadow_ofs_y}
    };
    SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
    if (texture == NULL) {
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &sh_rect_area, shadow_radius, false);
        int16_t mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
        lv_opa_t *mask_buf = lv_draw_mask_dump(&sh_area);
        lv_draw_mask_blur(mask_buf, lv_area_get_width(&sh_area), lv_area_get_height(&sh_area), sw / 2 + 1);
        texture = lv_sdl2_create_mask_texture(renderer, mask_buf, lv_area_get_width(&sh_area),
                                              lv_area_get_height(&sh_area));
        lv_mem_buf_release(mask_buf);
        lv_draw_mask_remove_id(mask_rout_id);
        SDL_assert(texture);
        lv_gpu_draw_cache_put(&key, sizeof(key), texture);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->shadow_opa);
    SDL_SetTextureColorMod(texture, shadow_color.r, shadow_color.g, shadow_color.b);
    SDL_RenderSetClipRect(renderer, mask_rect);
    SDL_RenderCopy(renderer, texture, NULL, &sh_area_rect);
}


void draw_border(SDL_Renderer *renderer, const lv_area_t *coords,
                 const SDL_Rect *coords_rect, const SDL_Rect *mask_rect,
                 const lv_draw_rect_dsc_t *dsc) {
    if (dsc->border_opa <= LV_OPA_MIN) return;
    if (dsc->border_width == 0) return;
    if (dsc->border_side == LV_BORDER_SIDE_NONE) return;
    if (dsc->border_post) return;

    SDL_Color border_color;
    lv_color_to_sdl_color(&dsc->border_color, &border_color);


    if (dsc->border_side != LV_BORDER_SIDE_FULL) {
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, dsc->border_opa);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        for (int w = 0; w <= dsc->border_width; w++) {
            if (dsc->border_side & LV_BORDER_SIDE_TOP) {
                SDL_RenderDrawLine(renderer, coords->x1, coords->y1 + w, coords->x2, coords->y1 + w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_BOTTOM) {
                SDL_RenderDrawLine(renderer, coords->x1, coords->y2 - w, coords->x2, coords->y2 - w);
            }
            if (dsc->border_side & LV_BORDER_SIDE_LEFT) {
                SDL_RenderDrawLine(renderer, coords->x1 + w, coords->y1, coords->x1 + w, coords->y2);
            }
            if (dsc->border_side & LV_BORDER_SIDE_RIGHT) {
                SDL_RenderDrawLine(renderer, coords->x2 - w, coords->y1, coords->x2 - w, coords->y2);
            }
        }
    } else if (dsc->radius > 0) {
        lv_draw_rect_border_key_t key = {
                .bg = {
                        .width = (lv_coord_t) coords_rect->w,
                        .height =(lv_coord_t) coords_rect->h,
                        .radius = dsc->radius,
                },
                .side = dsc->border_side,
                .thickness = dsc->border_width
        };
        SDL_Texture *texture = lv_gpu_draw_cache_get(&key, sizeof(key));
        if (texture == NULL) {
            /*Get the real radius*/
            int32_t rout = dsc->radius;
            int32_t short_side = LV_MIN(coords_rect->w, coords_rect->h);
            if (rout > short_side >> 1) rout = short_side >> 1;

            /*Create mask for the outer area*/
            int16_t mask_rout_id = LV_MASK_ID_INV;
            lv_draw_mask_radius_param_t mask_rout_param;
            if (rout > 0) {
                lv_draw_mask_radius_init(&mask_rout_param, coords, rout, false);
                mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);
            }

            /*Get the inner area*/
            lv_area_t area_inner;
            lv_area_copy(&area_inner, coords);
            area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : -(dsc->border_width +
                                                                                               rout));
            area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : -(dsc->border_width +
                                                                                                rout));
            area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : -(dsc->border_width +
                                                                                              rout));
            area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : -(dsc->border_width +
                                                                                                 rout));

            /*Create mask for the inner mask*/
            int32_t rin = rout - dsc->border_width;
            if (rin < 0) rin = 0;
            lv_draw_mask_radius_param_t mask_rin_param;
            lv_draw_mask_radius_init(&mask_rin_param, &area_inner, rout - dsc->border_width, true);
            int16_t mask_rin_id = lv_draw_mask_add(&mask_rin_param, NULL);

            texture = lv_sdl2_gen_mask_texture(renderer, coords);

            lv_draw_mask_remove_id(mask_rin_id);
            lv_draw_mask_remove_id(mask_rout_id);
            SDL_assert(texture);
            lv_gpu_draw_cache_put(&key, sizeof(key), texture);
        }

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureAlphaMod(texture, dsc->border_opa);
        SDL_SetTextureColorMod(texture, border_color.r, border_color.g, border_color.b);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderCopy(renderer, texture, NULL, coords_rect);
    } else {
        SDL_SetRenderDrawColor(renderer, border_color.r, border_color.g, border_color.b, dsc->border_opa);
        SDL_RenderSetClipRect(renderer, mask_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_Rect simple_rect;
        for (int w = 0; w <= dsc->border_width; w++) {
            simple_rect.x = coords_rect->x + w;
            simple_rect.y = coords_rect->y + w;
            simple_rect.w = coords_rect->w - w * 2;
            simple_rect.h = coords_rect->h - w * 2;
            SDL_RenderDrawRect(renderer, &simple_rect);
        }
    }
}

// Slow draw function
void draw_bg_compat(SDL_Renderer *renderer, const lv_area_t *coords, const SDL_Rect *coords_rect,
                    const SDL_Rect *mask_rect, const lv_draw_rect_dsc_t *dsc) {
    SDL_Color bg_color;
    lv_color_to_sdl_color(&dsc->bg_color, &bg_color);

    SDL_Surface *indexed = lv_sdl2_apply_mask_surface(coords);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, indexed);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
    SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
    SDL_RenderSetClipRect(renderer, mask_rect);
    SDL_RenderCopy(renderer, texture, NULL, coords_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(indexed);
}

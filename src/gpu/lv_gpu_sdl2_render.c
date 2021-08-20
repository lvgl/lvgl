//
// Created by Mariotaku on 2021/08/20.
//

#include <gpu/sdl2/lv_gpu_sdl2_utils.h>
#include "font/lv_font_fmt_txt.h"
#include "core/lv_refr.h"
#include "sdl2/lv_gpu_draw_cache.h"
#include "lv_gpu_sdl2_render.h"

lv_res_t lv_gpu_sdl2_renderer_init() {
    lv_gpu_draw_cache_init();
}

void lv_gpu_sdl2_renderer_deinit() {
    lv_gpu_draw_cache_deinit();
}

void lv_gpu_sdl_render_fill_color(lv_disp_drv_t *drv, const lv_area_t *disp_area, const lv_area_t *draw_area,
                                  lv_color_t color, lv_opa_t opa, const lv_opa_t *mask, lv_draw_mask_res_t mask_res,
                                  lv_blend_mode_t mode) {
    SDL_Rect rect = {
            .x = draw_area->x1 + disp_area->x1,
            .y = draw_area->y1 + disp_area->y1,
            .w = draw_area->x2 - draw_area->x1 + 1,
            .h = draw_area->y2 - draw_area->y1 + 1,
    };
    SDL_Renderer *renderer = drv->draw_buf->buf_act;
    if (mask && mask_res != LV_DRAW_MASK_RES_TRANSP) {
        SDL_Texture *texture = (SDL_Texture *) drv->user_data;
        uint8_t *pixels = NULL;
        int pitch = 0;
        SDL_Rect lock_area = {
                .x = 0,
                .y = 0,
                .w = draw_area->x2 - draw_area->x1 + 1,
                .h = draw_area->y2 - draw_area->y1 + 1,
        };
        SDL_LockTexture(texture, &lock_area, (void *) &pixels, &pitch);
        for (int y = 0; y < lock_area.h; y++) {
            for (int x = 0; x < lock_area.w; x++) {
                pixels[y * pitch + x * 4 + 0] = mask[y * lock_area.w + x];
                pixels[y * pitch + x * 4 + 1] = 0xFF;
                pixels[y * pitch + x * 4 + 2] = 0xFF;
                pixels[y * pitch + x * 4 + 3] = 0xFF;
            }
        }
        SDL_UnlockTexture(texture);

        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
        SDL_RenderCopy(renderer, texture, &lock_area, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, color.ch.red, color.ch.green, color.ch.blue, opa);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &rect);
    }
}

typedef struct _lv_sdl_font_userdata_t {
    const lv_font_t *key;
    uint8_t supported;
    SDL_Texture *texture;
    SDL_Rect *pos;
    uint32_t start;
    struct _lv_sdl_font_userdata_t *next;
} lv_sdl_font_userdata_t;

static lv_sdl_font_userdata_t *font_sprites = NULL;

static lv_sdl_font_userdata_t *_lv_font_bake(const lv_font_t *font_p, SDL_Renderer *renderer);

void lv_gpu_sdl_render_draw_letter(lv_coord_t pos_x, lv_coord_t pos_y, lv_font_glyph_dsc_t *g,
                                   const lv_area_t *clip_area,
                                   const lv_font_t *font_p, uint32_t letter, lv_color_t color, lv_opa_t opa,
                                   lv_blend_mode_t blend_mode) {
    if (letter > 255) return;
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->draw_buf->buf1;
    lv_sdl_font_userdata_t *userdata = NULL;
    for (lv_sdl_font_userdata_t *cur = font_sprites; cur != NULL; cur = cur->next) {
        if (cur->key == font_p) {
            userdata = cur;
            break;
        }
    }
    if (!userdata) {
        userdata = _lv_font_bake(font_p, renderer);
        if (font_sprites) {
            lv_sdl_font_userdata_t *cur = font_sprites;
            while (cur->next != NULL) {
                cur = cur->next;
            }
            cur->next = userdata;
        } else {
            font_sprites = userdata;
        }
    }
    if (!userdata->supported) {
        return;
    }
    SDL_Rect dstrect = {.x = pos_x, .y = pos_y, .w = g->box_w, .h = g->box_h};

    SDL_SetTextureBlendMode(userdata->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(userdata->texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderCopy(renderer, userdata->texture, &userdata->pos[letter - userdata->start], &dstrect);
}

lv_sdl_font_userdata_t *_lv_font_bake(const lv_font_t *font_p, SDL_Renderer *renderer) {
    const lv_font_fmt_txt_dsc_t *dsc = (lv_font_fmt_txt_dsc_t *) font_p->dsc;
    lv_sdl_font_userdata_t *userdata = malloc(sizeof(lv_sdl_font_userdata_t));
    userdata->key = font_p;
    userdata->supported = 0;
    userdata->next = NULL;
    for (int cmap_idx = 0; cmap_idx < dsc->cmap_num; cmap_idx++) {
        const lv_font_fmt_txt_cmap_t *cmap = &dsc->cmaps[cmap_idx];
        if (cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            if (cmap->range_length > 256) break;
            userdata->supported = 1;
            userdata->start = cmap->range_start;
            int sprite_w = font_p->line_height * 16;
            userdata->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC,
                                                  sprite_w,
                                                  font_p->line_height * ((cmap->range_length / 16) + 1));
            userdata->pos = malloc(sizeof(SDL_Rect) * cmap->range_length);
            int sprite_x = 0, sprite_y = 0;
            uint8_t *tmpglyph = malloc(font_p->line_height * font_p->line_height * 4);
            for (int i = 0; i < cmap->range_length; i++) {
                const lv_font_fmt_txt_glyph_dsc_t *gd = &dsc->glyph_dsc[cmap->glyph_id_start + i];
                const uint8_t *bmp = &dsc->glyph_bitmap[gd->bitmap_index];
                if (sprite_x + gd->box_w >= sprite_w) {
                    sprite_x = 0;
                    sprite_y += font_p->line_height;
                }
                SDL_Rect *rect = &userdata->pos[i];
                rect->x = sprite_x;
                rect->y = sprite_y;
                rect->w = gd->box_w;
                rect->h = gd->box_h;
                if (rect->w <= 0 || rect->h <= 0) continue;
                switch (dsc->bpp) {
                    case 4: {
                        for (int tmp_idx = 0; tmp_idx < gd->box_w * gd->box_h; tmp_idx++) {
                            if (tmp_idx % 2 == 0) {
                                tmpglyph[tmp_idx * 4 + 3] = _lv_bpp4_opa_table[(0xF0 & bmp[tmp_idx / 2]) >> 0x4];
                            } else {
                                tmpglyph[tmp_idx * 4 + 3] = _lv_bpp4_opa_table[0x0F & bmp[tmp_idx / 2]];
                            }
                            tmpglyph[tmp_idx * 4 + 1] = tmpglyph[tmp_idx * 4 + 2] = tmpglyph[tmp_idx * 4 + 0] = 0xFF;
                        }
                        break;
                    }
                }
                SDL_UpdateTexture(userdata->texture, rect, tmpglyph, gd->box_w * 4);
                sprite_x += gd->box_w;
            }
            free(tmpglyph);
            break;
        }
    }
    return userdata;
}

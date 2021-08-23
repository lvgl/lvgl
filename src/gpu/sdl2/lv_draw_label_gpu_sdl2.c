//
// Created by Mariotaku on 2021/08/23.
//
#include <font/lv_font_fmt_txt.h>
#include "SDL.h"
#include "core/lv_refr.h"
#include "draw/lv_draw_label.h"
#include "lv_gpu_sdl2_utils.h"

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

void lv_draw_letter(const lv_point_t *pos_p, const lv_area_t *clip_area,
                    const lv_font_t *font_p,
                    uint32_t letter,
                    lv_color_t color, lv_opa_t opa, lv_blend_mode_t blend_mode) {
    if (opa < LV_OPA_MIN) return;
    if (opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if (font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if (g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if (letter >= 0x20 &&
            letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
            letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%X", letter);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if ((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    /*If the letter is completely out of mask don't draw it*/
    if (pos_x + g.box_w < clip_area->x1 ||
        pos_x > clip_area->x2 ||
        pos_y + g.box_h < clip_area->y1 ||
        pos_y > clip_area->y2) {
        return;
    }

    if (letter > 255) return;
    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;
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
    SDL_Rect dstrect = {.x = pos_x, .y = pos_y, .w = g.box_w, .h = g.box_h};

    SDL_Rect clip_area_rect;
    lv_area_to_sdl_rect(clip_area, &clip_area_rect);

    SDL_SetTextureBlendMode(userdata->texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureColorMod(userdata->texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderSetClipRect(renderer, &clip_area_rect);
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

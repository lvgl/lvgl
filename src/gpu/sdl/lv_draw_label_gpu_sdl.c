#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "SDL.h"
#include "../../font/lv_font_fmt_txt.h"
#include "../../core/lv_refr.h"
#include "../../draw/lv_draw_label.h"
#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_mask.h"
#include "lv_gpu_draw_cache.h"

typedef struct lv_sdl_font_atlas_t {
    SDL_Rect *pos;
    uint32_t start;
} lv_sdl_font_atlas_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    const lv_font_t *font_p;
} lv_font_key_t;

static void font_atlas_free(lv_sdl_font_atlas_t *atlas);

static SDL_Texture *font_atlas_bake(const lv_font_t *font_p, lv_sdl_font_atlas_t *atlas, SDL_Renderer *renderer);

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
    lv_font_key_t key = {.magic = LV_GPU_CACHE_KEY_MAGIC_FONT, .font_p = font_p};
    lv_sdl_font_atlas_t *atlas = NULL;
    bool found = false;
    SDL_Texture *texture = lv_gpu_draw_cache_get_with_userdata(&key, sizeof(key), &found, (void **) &atlas);

    lv_disp_t *disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer *renderer = (SDL_Renderer *) disp->driver->user_data;

    if (!found) {
        atlas = SDL_malloc(sizeof(lv_sdl_font_atlas_t));
        texture = font_atlas_bake(font_p, atlas, renderer);
        lv_gpu_draw_cache_put_with_userdata(&key, sizeof(key), texture, atlas, (lv_lru_free_t *) font_atlas_free);
    }
    if (letter > 255 || texture == NULL) return;
    SDL_Rect dstrect = {.x = pos_x, .y = pos_y, .w = g.box_w, .h = g.box_h};

    SDL_Rect clip_area_rect;
    lv_area_to_sdl_rect(clip_area, &clip_area_rect);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderSetClipRect(renderer, &clip_area_rect);
    SDL_RenderCopy(renderer, texture, &atlas->pos[letter - atlas->start], &dstrect);
}

SDL_Texture *font_atlas_bake(const lv_font_t *font_p, lv_sdl_font_atlas_t *atlas, SDL_Renderer *renderer) {
    SDL_memset(atlas, 0, sizeof(lv_sdl_font_atlas_t));
    const lv_font_fmt_txt_dsc_t *dsc = (lv_font_fmt_txt_dsc_t *) font_p->dsc;
    SDL_Texture *result = NULL;
    for (int cmap_idx = 0; cmap_idx < dsc->cmap_num; cmap_idx++) {
        const lv_font_fmt_txt_cmap_t *cmap = &dsc->cmaps[cmap_idx];
        if (cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            if (cmap->range_length > 256) break;
            atlas->start = cmap->range_start;
            int sprite_w = font_p->line_height * 16;
            int sprite_h = font_p->line_height * ((cmap->range_length / 16) + 1);
            lv_opa_t *s1 = lv_mem_buf_get(
                    sprite_w * font_p->line_height * ((cmap->range_length / 16) + 1) * sizeof(lv_opa_t));
            atlas->pos = SDL_malloc(sizeof(SDL_Rect) * cmap->range_length);
            int sprite_x = 0, sprite_y = 0;
            for (int i = 0; i < cmap->range_length; i++) {
                const lv_font_fmt_txt_glyph_dsc_t *gd = &dsc->glyph_dsc[cmap->glyph_id_start + i];
                if (sprite_x + gd->box_w >= sprite_w) {
                    sprite_x = 0;
                    sprite_y += font_p->line_height;
                }
                SDL_Rect *rect = &atlas->pos[i];
                rect->x = sprite_x;
                rect->y = sprite_y;
                rect->w = gd->box_w;
                rect->h = gd->box_h;
                if (gd->box_w <= 0 || gd->box_h <= 0) {
                    continue;
                }
                lv_sdl_to_8bpp(&s1[rect->y * sprite_w + rect->x], &dsc->glyph_bitmap[gd->bitmap_index], rect->w,
                               rect->h, sprite_w, dsc->bpp);
                sprite_x += gd->box_w;
            }
#if SDL_VERSION_ATLEAST(2, 0, 5)
            SDL_Surface *indexed = SDL_CreateRGBSurfaceWithFormatFrom(s1, sprite_w, sprite_h, 8, sprite_w,
                                                                      SDL_PIXELFORMAT_INDEX8);
#else
            SDL_Surface *indexed = SDL_CreateRGBSurfaceFrom(s1, sprite_w, sprite_h, 8, sprite_w, 0, 0, 0,
                                                            0xFF);
#endif
            SDL_SetSurfacePalette(indexed, lv_sdl_get_grayscale_palette(dsc->bpp));
            result = SDL_CreateTextureFromSurface(renderer, indexed);
            SDL_FreeSurface(indexed);
            lv_mem_buf_release(s1);
            break;
        }
    }
    if (!result) {
        if (atlas->pos) {
            SDL_free(atlas->pos);
        }
        SDL_memset(atlas, 0, sizeof(lv_sdl_font_atlas_t));
    }
    return result;
}

static void font_atlas_free(lv_sdl_font_atlas_t *atlas) {
    if (atlas->pos) {
        SDL_free(atlas->pos);
    }
    SDL_free(atlas);
}

#endif /*LV_USE_GPU_SDL*/

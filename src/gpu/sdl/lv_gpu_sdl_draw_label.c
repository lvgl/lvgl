/**
 * @file lv_gpu_sdl_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../draw/lv_draw_label.h"
#include "../../font/lv_font_fmt_txt.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_utils.h"

#include LV_GPU_SDL_INCLUDE_PATH

#include "lv_gpu_sdl_utils.h"
#include "lv_gpu_sdl_texture_cache.h"
#include "lv_gpu_sdl_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_sdl_font_atlas_t {
    SDL_Rect * pos;
} lv_sdl_font_atlas_t;

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    const lv_font_t * font_p;
    uint32_t cmap_index;
} lv_font_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_letter_masked(SDL_Renderer * renderer, SDL_Texture * atlas, SDL_Rect * src, SDL_Rect * dst,
                               SDL_Rect * clip, lv_color_t color, lv_opa_t opa);

static void font_atlas_free(lv_sdl_font_atlas_t * atlas);

static SDL_Texture * font_atlas_bake(SDL_Renderer * renderer, const lv_font_t * font_p, uint32_t cmap_idx,
                                     lv_sdl_font_atlas_t * atlas);

static int32_t unicode_list_compare(const void * ref, const void * element);

static bool font_cmap_find_index(const lv_font_fmt_txt_dsc_t * dsc, uint32_t letter, uint32_t * cmap_index,
                                 uint32_t * char_index);

static lv_font_key_t font_key_create(const lv_font_t * font_p, uint32_t cmap_index);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
                    const lv_font_t * font_p, uint32_t letter, lv_color_t color, lv_opa_t opa,
                    lv_blend_mode_t blend_mode)
{
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
           letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
           letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%X", letter);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    /*If the letter is completely out of mask don't draw it*/
    if(pos_x + g.box_w < clip_area->x1 ||
       pos_x > clip_area->x2 ||
       pos_y + g.box_h < clip_area->y1 ||
       pos_y > clip_area->y2) {
        return;
    }
    lv_area_t dst = {pos_x, pos_y, pos_x + g.box_w - 1, pos_y + g.box_h - 1};
    uint32_t atlas_index;
    uint32_t cmap_index;
    if(!font_cmap_find_index(font_p->dsc, letter, &cmap_index, &atlas_index)) {
        return;
    }
    lv_font_key_t key = font_key_create(font_p, cmap_index);
    lv_sdl_font_atlas_t * atlas = NULL;
    bool found = false;
    SDL_Texture * texture = lv_gpu_draw_cache_get_with_userdata(&key, sizeof(key), &found, (void **) &atlas);

    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    SDL_Renderer * renderer = (SDL_Renderer *) disp->driver->user_data;

    if(!found) {
        atlas = SDL_malloc(sizeof(lv_sdl_font_atlas_t));
        texture = font_atlas_bake(renderer, font_p, cmap_index, atlas);
        lv_gpu_draw_cache_put_advanced(&key, sizeof(key), texture, atlas, (lv_lru_free_t *) font_atlas_free, 0);
    }
    if(texture == NULL) return;
    SDL_Rect dstrect = {.x = pos_x, .y = pos_y, .w = g.box_w, .h = g.box_h};

    SDL_Rect clip_area_rect;
    lv_area_to_sdl_rect(clip_area, &clip_area_rect);

    if(lv_draw_mask_is_any(&dst)) {
        draw_letter_masked(renderer, texture, &atlas->pos[atlas_index], &dstrect, &clip_area_rect, color, opa);
        return;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderSetClipRect(renderer, &clip_area_rect);
    SDL_RenderCopy(renderer, texture, &atlas->pos[atlas_index], &dstrect);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_letter_masked(SDL_Renderer * renderer, SDL_Texture * atlas, SDL_Rect * src, SDL_Rect * dst,
                               SDL_Rect * clip, lv_color_t color, lv_opa_t opa)
{
    SDL_Texture * screen = SDL_GetRenderTarget(renderer);

    lv_area_t mask_area = {.x1 = dst->x, .x2 = dst->x + dst->w - 1, .y1 = dst->y, .y2 = dst->y + dst->h - 1};
    SDL_Texture * content = lv_gpu_temp_texture_obtain(renderer, dst->w, dst->h);
    SDL_SetTextureBlendMode(content, SDL_BLENDMODE_NONE);
    SDL_SetRenderTarget(renderer, content);
    SDL_RenderSetClipRect(renderer, NULL);

    /* Replace texture with clip mask */
    SDL_Rect mask_rect = {.w = dst->w, .h = dst->h, .x = 0, .y = 0};
    SDL_Texture * mask = lv_sdl_gen_mask_texture(renderer, &mask_area, NULL, 0);
    SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_NONE);
    SDL_RenderCopy(renderer, mask, NULL, &mask_rect);

    /* Multiply with font atlas */
    SDL_SetTextureAlphaMod(atlas, 0xFF);
    SDL_SetTextureColorMod(atlas, 0xFF, 0xFF, 0xFF);
#if SDL_VERSION_ATLEAST(2, 0, 6)
    SDL_BlendMode mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_ONE, SDL_BLENDFACTOR_ZERO,
                                                    SDL_BLENDOPERATION_ADD, SDL_BLENDFACTOR_ZERO,
                                                    SDL_BLENDFACTOR_SRC_ALPHA, SDL_BLENDOPERATION_ADD);
    SDL_SetTextureBlendMode(atlas, mode);
#else
    SDL_SetTextureBlendMode(atlas, SDL_BLENDMODE_BLEND);
#endif
    SDL_RenderCopy(renderer, atlas, src, &mask_rect);

    /* Draw composited part on screen */
    SDL_SetTextureBlendMode(content, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(content, opa);
    SDL_SetTextureColorMod(content, color.ch.red, color.ch.green, color.ch.blue);

    SDL_SetRenderTarget(renderer, screen);
    SDL_RenderSetClipRect(renderer, clip);
    SDL_RenderCopy(renderer, content, &mask_rect, dst);
    SDL_DestroyTexture(mask);
}

SDL_Texture * font_atlas_bake(SDL_Renderer * renderer, const lv_font_t * font_p, uint32_t cmap_idx,
                              lv_sdl_font_atlas_t * atlas)
{
    /* Clear atlas struct */
    SDL_memset(atlas, 0, sizeof(lv_sdl_font_atlas_t));
    const lv_font_fmt_txt_dsc_t * dsc = (lv_font_fmt_txt_dsc_t *) font_p->dsc;
    if(dsc->bitmap_format != LV_FONT_FMT_TXT_PLAIN) {
        /* we don't support compressed font at this moment */
        return NULL;
    }
    const lv_font_fmt_txt_cmap_t * cmap = &dsc->cmaps[cmap_idx];
    int glyph_count = cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY ? cmap->range_length : cmap->list_length;
    int atlas_size = 0;
    while(atlas_size * atlas_size < glyph_count) {
        atlas_size++;
    }
    int atlas_w = font_p->line_height * atlas_size;
    int atlas_h = font_p->line_height * (glyph_count / atlas_size + 1);
    if(atlas_w > 2048 || atlas_h > 2048) {
        /*This atlas texture will be too large to load*/
        return NULL;
    }
    lv_opa_t * s1 = lv_mem_buf_get(atlas_w * atlas_h * sizeof(lv_opa_t));
    atlas->pos = SDL_malloc(sizeof(SDL_Rect) * glyph_count);
    int atlas_y = 0;
    int atlas_x = 0;
    for(int i = 0; i < glyph_count; i++) {
        int glyph_idx = cmap->glyph_id_start + i;
        switch(cmap->type) {
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL: {
                    const uint8_t * gid_ofs_8 = cmap->glyph_id_ofs_list;
                    glyph_idx = gid_ofs_8[i];
                    break;
                }
            case LV_FONT_FMT_TXT_CMAP_SPARSE_FULL: {
                    const uint16_t * gid_ofs_16 = cmap->glyph_id_ofs_list;
                    glyph_idx = gid_ofs_16[i];
                    break;
                }
        }
        const lv_font_fmt_txt_glyph_dsc_t * gd = &dsc->glyph_dsc[glyph_idx];
        if(atlas_x + gd->box_w >= atlas_w) {
            atlas_x = 0;
            atlas_y += font_p->line_height;
        }
        SDL_Rect * rect = &atlas->pos[i];
        rect->x = atlas_x;
        rect->y = atlas_y;
        rect->w = gd->box_w;
        rect->h = gd->box_h;
        if(gd->box_w <= 0 || gd->box_h <= 0) {
            continue;
        }
        lv_sdl_to_8bpp(&s1[rect->y * atlas_w + rect->x], &dsc->glyph_bitmap[gd->bitmap_index], rect->w,
                       rect->h, atlas_w, dsc->bpp);
        atlas_x += gd->box_w;
    }
    SDL_Surface * mask = lv_sdl_create_mask_surface(s1, atlas_w, atlas_h, atlas_w);
    SDL_Texture * result = SDL_CreateTextureFromSurface(renderer, mask);
    SDL_FreeSurface(mask);
    lv_mem_buf_release(s1);

    if(!result) {
        if(atlas->pos) {
            SDL_free(atlas->pos);
        }
        SDL_memset(atlas, 0, sizeof(lv_sdl_font_atlas_t));
    }
    return result;
}

static void font_atlas_free(lv_sdl_font_atlas_t * atlas)
{
    if(atlas->pos) {
        SDL_free(atlas->pos);
    }
    SDL_free(atlas);
}

static bool font_cmap_find_index(const lv_font_fmt_txt_dsc_t * dsc, uint32_t letter, uint32_t * cmap_index,
                                 uint32_t * char_index)
{
    for(int i = 0; i < dsc->cmap_num; i++) {
        const lv_font_fmt_txt_cmap_t * cmap = &dsc->cmaps[i];
        /*Relative code point*/
        uint32_t rcp = letter - cmap->range_start;
        if(rcp > cmap->range_length) continue;
        if(cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            *cmap_index = i;
            *char_index = rcp;
            return true;
        }
        else if(cmap->type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            *cmap_index = i;
            *char_index = rcp;
            return true;
        }
        else if(cmap->type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            uint16_t key = rcp;
            uint16_t * p = _lv_utils_bsearch(&key, cmap->unicode_list, cmap->list_length,
                                             sizeof(cmap->unicode_list[0]), unicode_list_compare);
            if(!p) continue;

            *cmap_index = i;
            *char_index = p - cmap->unicode_list;
            return true;
        }
        else if(cmap->type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            uint16_t key = rcp;
            uint16_t * p = _lv_utils_bsearch(&key, cmap->unicode_list, cmap->list_length,
                                             sizeof(cmap->unicode_list[0]), unicode_list_compare);
            if(!p) continue;

            *cmap_index = i;
            *char_index = p - cmap->unicode_list;
            return true;
        }
    }
    return 0;
}

static int32_t unicode_list_compare(const void * ref, const void * element)
{
    return ((int32_t)(*(uint16_t *) ref)) - ((int32_t)(*(uint16_t *) element));
}

static lv_font_key_t font_key_create(const lv_font_t * font_p, uint32_t cmap_index)
{
    lv_font_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_FONT;
    key.font_p = font_p;
    key.cmap_index = cmap_index;
    return key;
}

#endif /*LV_USE_GPU_SDL*/

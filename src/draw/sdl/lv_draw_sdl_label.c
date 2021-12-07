/**
 * @file lv_draw_sdl_draw_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../../draw/lv_draw_label.h"
#include "../../draw/lv_draw_mask.h"
#include "../../misc/lv_utils.h"

#include LV_GPU_SDL_INCLUDE_PATH

#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_mask.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_gpu_cache_key_magic_t magic;
    const lv_font_t * font_p;
    uint32_t letter;
} lv_font_glyph_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_letter_masked(SDL_Renderer * renderer, SDL_Texture * atlas, SDL_Rect * src, SDL_Rect * dst,
                               SDL_Rect * clip, lv_color_t color, lv_opa_t opa);

static lv_font_glyph_key_t font_key_glyph_create(const lv_font_t * font_p, uint32_t letter);


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_draw_letter(const lv_point_t * pos_p, const lv_area_t * clip_area,
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

    lv_draw_sdl_backend_context_t * ctx = lv_draw_sdl_get_context();
    SDL_Renderer * renderer = ctx->renderer;

    lv_font_glyph_key_t glyph_key = font_key_glyph_create(font_p, letter);
    bool glyph_found = false;
    SDL_Texture * texture = lv_gpu_draw_cache_get(&glyph_key, sizeof(glyph_key), &glyph_found);
    if(!glyph_found) {
        if(g.resolved_font) {
            font_p = g.resolved_font;
        }
        const uint8_t * bmp = lv_font_get_glyph_bitmap(font_p, letter);
        uint8_t * buf = lv_mem_alloc(g.box_w * g.box_h);
        lv_sdl_to_8bpp(buf, bmp, g.box_w, g.box_h, g.box_w, g.bpp);
        SDL_Surface * mask = lv_sdl_create_mask_surface(buf, g.box_w, g.box_h, g.box_w);
        texture = SDL_CreateTextureFromSurface(renderer, mask);
        SDL_FreeSurface(mask);
        lv_mem_free(buf);
        lv_draw_sdl_draw_cache_put(&glyph_key, sizeof(glyph_key), texture);
    }
    if(!texture) {
        return;
    }
    lv_area_t dst = {pos_x, pos_y, pos_x + g.box_w - 1, pos_y + g.box_h - 1};
    SDL_Rect dstrect;
    lv_area_to_sdl_rect(&dst, &dstrect);

    SDL_Rect clip_area_rect;
    lv_area_to_sdl_rect(clip_area, &clip_area_rect);

    if(lv_draw_mask_is_any(&dst)) {
        draw_letter_masked(renderer, texture, NULL, &dstrect, &clip_area_rect, color, opa);
        return;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderSetClipRect(renderer, &clip_area_rect);
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderSetClipRect(renderer, NULL);
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
    SDL_RenderSetClipRect(renderer, NULL);
    SDL_DestroyTexture(mask);
}

static lv_font_glyph_key_t font_key_glyph_create(const lv_font_t * font_p, uint32_t letter)
{
    lv_font_glyph_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_FONT_GLYPH;
    key.font_p = font_p;
    key.letter = letter;
    return key;
}

#endif /*LV_USE_GPU_SDL*/

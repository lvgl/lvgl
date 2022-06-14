/**
 * @file lv_draw_sdl_label.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include LV_GPU_SDL_INCLUDE_PATH

#include "../lv_draw_label.h"
#include "../../misc/lv_utils.h"

#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_layer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    const lv_font_t * font_p;
    uint32_t letter;
} lv_font_glyph_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

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

void lv_draw_sdl_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                             uint32_t letter)
{
    const lv_area_t * clip_area = draw_ctx->clip_area;
    const lv_font_t * font_p = dsc->font;
    lv_opa_t opa = dsc->opa;
    lv_color_t color = dsc->color;
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

            /* draw placeholder */
            lv_area_t glyph_coords;
            lv_draw_rect_dsc_t glyph_dsc;
            lv_coord_t begin_x = pos_p->x + g.ofs_x;
            lv_coord_t begin_y = pos_p->y + g.ofs_y;
            lv_area_set(&glyph_coords, begin_x, begin_y, begin_x + g.box_w, begin_y + g.box_h);
            lv_draw_rect_dsc_init(&glyph_dsc);
            glyph_dsc.bg_opa = LV_OPA_MIN;
            glyph_dsc.outline_opa = LV_OPA_MIN;
            glyph_dsc.shadow_opa = LV_OPA_MIN;
            glyph_dsc.bg_img_opa = LV_OPA_MIN;
            glyph_dsc.border_color = dsc->color;
            glyph_dsc.border_width = 1;
            draw_ctx->draw_rect(draw_ctx, &glyph_dsc, &glyph_coords);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    const lv_area_t letter_area = {pos_x, pos_y, pos_x + g.box_w - 1, pos_y + g.box_h - 1};
    lv_area_t draw_area;

    /*If the letter is completely out of mask don't draw it*/
    if(!_lv_area_intersect(&draw_area, &letter_area, clip_area)) {
        return;
    }

    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = ctx->renderer;

    lv_font_glyph_key_t glyph_key = font_key_glyph_create(font_p, letter);
    bool glyph_found = false;
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &glyph_key, sizeof(glyph_key), &glyph_found);
    if(!glyph_found) {
        if(g.resolved_font) {
            font_p = g.resolved_font;
        }
        const uint8_t * bmp = lv_font_get_glyph_bitmap(font_p, letter);
        uint8_t * buf = lv_mem_alloc(g.box_w * g.box_h);
        lv_sdl_to_8bpp(buf, bmp, g.box_w, g.box_h, g.box_w, g.bpp);
        SDL_Surface * mask = lv_sdl_create_opa_surface(buf, g.box_w, g.box_h, g.box_w);
        texture = SDL_CreateTextureFromSurface(renderer, mask);
        SDL_FreeSurface(mask);
        lv_mem_free(buf);
        lv_draw_sdl_texture_cache_put(ctx, &glyph_key, sizeof(glyph_key), texture);
    }
    if(!texture) {
        return;
    }

    lv_area_t t_letter = letter_area, t_clip = *clip_area, apply_area;
    bool has_composite = lv_draw_sdl_composite_begin(ctx, &letter_area, clip_area, NULL, dsc->blend_mode, &t_letter,
                                                     &t_clip, &apply_area);

    lv_draw_sdl_transform_areas_offset(ctx, has_composite, &apply_area, &t_letter, &t_clip);

    /*If the letter is completely out of mask don't draw it*/
    if(!_lv_area_intersect(&draw_area, &t_letter, &t_clip)) {
        return;
    }
    SDL_Rect srcrect, dstrect;
    lv_area_to_sdl_rect(&draw_area, &dstrect);
    srcrect.x = draw_area.x1 - t_letter.x1;
    srcrect.y = draw_area.y1 - t_letter.y1;
    srcrect.w = dstrect.w;
    srcrect.h = dstrect.h;
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color.ch.red, color.ch.green, color.ch.blue);
    SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);

    lv_draw_sdl_composite_end(ctx, &apply_area, dsc->blend_mode);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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

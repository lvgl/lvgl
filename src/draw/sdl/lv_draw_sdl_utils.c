/**
 * @file lv_draw_sdl_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl_utils.h"

#include "../lv_draw.h"
#include "../lv_draw_label.h"
#include "../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
/**********************
 *  STATIC VARIABLES
 **********************/
extern const uint8_t _lv_bpp1_opa_table[2];
extern const uint8_t _lv_bpp2_opa_table[4];
extern const uint8_t _lv_bpp4_opa_table[16];
extern const uint8_t _lv_bpp8_opa_table[256];

static int utils_init_count = 0;
static SDL_Palette * lv_sdl_palette_grayscale8 = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_draw_sdl_utils_init()
{
    utils_init_count++;
    if(utils_init_count > 1) {
        return;
    }
    lv_sdl_palette_grayscale8 = lv_sdl_alloc_palette_for_bpp(_lv_bpp8_opa_table, 8);
}

void _lv_draw_sdl_utils_deinit()
{
    if(utils_init_count == 0) {
        return;
    }
    utils_init_count--;
    if(utils_init_count == 0) {
        SDL_FreePalette(lv_sdl_palette_grayscale8);
        lv_sdl_palette_grayscale8 = NULL;
    }
}

void lv_area_to_sdl_rect(const lv_area_t * in, SDL_Rect * out)
{
    out->x = in->x1;
    out->y = in->y1;
    out->w = in->x2 - in->x1 + 1;
    out->h = in->y2 - in->y1 + 1;
}

void lv_color_to_sdl_color(const lv_color_t * in, SDL_Color * out)
{
#if LV_COLOR_DEPTH == 32
    out->a = in->ch.alpha;
    out->r = in->ch.red;
    out->g = in->ch.green;
    out->b = in->ch.blue;
#else
    uint32_t color32 = lv_color_to32(*in);
    lv_color32_t * color32_t = (lv_color32_t *) &color32;
    out->a = color32_t->ch.alpha;
    out->r = color32_t->ch.red;
    out->g = color32_t->ch.green;
    out->b = color32_t->ch.blue;
#endif
}

void lv_area_zoom_to_sdl_rect(const lv_area_t * in, SDL_Rect * out, uint16_t zoom, const lv_point_t * pivot)
{
    if(zoom == LV_IMG_ZOOM_NONE) {
        lv_area_to_sdl_rect(in, out);
        return;
    }
    lv_area_t tmp;
    _lv_img_buf_get_transformed_area(&tmp, lv_area_get_width(in), lv_area_get_height(in), 0, zoom, pivot);
    lv_area_move(&tmp, in->x1, in->y1);
    lv_area_to_sdl_rect(&tmp, out);
}

SDL_Palette * lv_sdl_alloc_palette_for_bpp(const uint8_t * mapping, uint8_t bpp)
{
    SDL_assert(bpp >= 1 && bpp <= 8);
    int color_cnt = 1 << bpp;
    SDL_Palette * result = SDL_AllocPalette(color_cnt);
    SDL_Color palette[256];
    for(int i = 0; i < color_cnt; i++) {
        palette[i].r = palette[i].g = palette[i].b = 0xFF;
        palette[i].a = mapping ? mapping[i] : i;
    }
    SDL_SetPaletteColors(result, palette, 0, color_cnt);
    return result;
}

SDL_Surface * lv_sdl_create_opa_surface(lv_opa_t * opa, lv_coord_t width, lv_coord_t height, lv_coord_t stride)
{
    SDL_Surface * indexed = SDL_CreateRGBSurfaceFrom(opa, width, height, 8, stride, 0, 0, 0, 0);
    SDL_SetSurfacePalette(indexed, lv_sdl_palette_grayscale8);
    SDL_Surface * converted = SDL_ConvertSurfaceFormat(indexed, LV_DRAW_SDL_TEXTURE_FORMAT, 0);
    SDL_FreeSurface(indexed);
    return converted;
}

SDL_Texture * lv_sdl_create_opa_texture(SDL_Renderer * renderer, lv_opa_t * pixels, lv_coord_t width,
                                        lv_coord_t height, lv_coord_t stride)
{
    SDL_Surface * indexed = lv_sdl_create_opa_surface(pixels, width, height, stride);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, indexed);
    SDL_FreeSurface(indexed);
    return texture;
}

void lv_sdl_to_8bpp(uint8_t * dest, const uint8_t * src, int width, int height, int stride, uint8_t bpp)
{
    int src_len = width * height;
    int cur = 0;
    int curbit;
    uint8_t opa_mask;
    const uint8_t * opa_table;
    switch(bpp) {
        case 1:
            opa_mask = 0x1;
            opa_table = _lv_bpp1_opa_table;
            break;
        case 2:
            opa_mask = 0x4;
            opa_table = _lv_bpp2_opa_table;
            break;
        case 4:
            opa_mask = 0xF;
            opa_table = _lv_bpp4_opa_table;
            break;
        case 8:
            opa_mask = 0xFF;
            opa_table = _lv_bpp8_opa_table;
            break;
        default:
            return;
    }
    /* Does this work well on big endian systems? */
    while(cur < src_len) {
        curbit = 8 - bpp;
        uint8_t src_byte = src[cur * bpp / 8];
        while(curbit >= 0 && cur < src_len) {
            uint8_t src_bits = opa_mask & (src_byte >> curbit);
            dest[(cur / width * stride) + (cur % width)] = opa_table[src_bits];
            curbit -= bpp;
            cur++;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL*/

/**
 * @file lv_draw_sdl_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/src/lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl_utils.h"

#include "lvgl/src/draw/lv_draw.h"
#include "lvgl/src/draw/lv_draw_label.h"

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
extern const uint8_t _lv_bpp3_opa_table[8];
extern const uint8_t _lv_bpp4_opa_table[16];
extern const uint8_t _lv_bpp8_opa_table[256];

static SDL_Palette * lv_sdl_palette_grayscale1 = NULL;
static SDL_Palette * lv_sdl_palette_grayscale2 = NULL;
static SDL_Palette * lv_sdl_palette_grayscale3 = NULL;
static SDL_Palette * lv_sdl_palette_grayscale4 = NULL;
static SDL_Palette * lv_sdl_palette_grayscale8 = NULL;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_draw_sdl_utils_init()
{
    lv_sdl_palette_grayscale1 = lv_sdl_alloc_palette_for_bpp(_lv_bpp1_opa_table, 1);
    lv_sdl_palette_grayscale2 = lv_sdl_alloc_palette_for_bpp(_lv_bpp2_opa_table, 2);
    lv_sdl_palette_grayscale3 = lv_sdl_alloc_palette_for_bpp(_lv_bpp3_opa_table, 3);
    lv_sdl_palette_grayscale4 = lv_sdl_alloc_palette_for_bpp(_lv_bpp4_opa_table, 4);
    lv_sdl_palette_grayscale8 = lv_sdl_alloc_palette_for_bpp(_lv_bpp8_opa_table, 8);
}

void _lv_draw_sdl_utils_deinit()
{
    SDL_FreePalette(lv_sdl_palette_grayscale1);
    SDL_FreePalette(lv_sdl_palette_grayscale2);
    SDL_FreePalette(lv_sdl_palette_grayscale3);
    SDL_FreePalette(lv_sdl_palette_grayscale4);
    SDL_FreePalette(lv_sdl_palette_grayscale8);
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
    uint32_t color32 = lv_color_to32(*in);
    lv_color32_t * color32_t = (lv_color32_t *) &color32;
    out->a = color32_t->ch.alpha;
    out->r = color32_t->ch.red;
    out->g = color32_t->ch.green;
    out->b = color32_t->ch.blue;
}

void lv_area_zoom_to_sdl_rect(const lv_area_t * in, SDL_Rect * out, uint16_t zoom, const lv_point_t * pivot)
{
    if(zoom == LV_IMG_ZOOM_NONE) {
        lv_area_to_sdl_rect(in, out);
        return;
    }
    int h = in->y2 - in->y1 + 1;
    int w = in->x2 - in->x1 + 1;
    int sh = h * zoom >> 8;
    int sw = w * zoom >> 8;
    out->x = in->x1 - (sw / 2 - pivot->x);
    out->y = in->y1 - (sh / 2 - pivot->y);
    out->w = sw;
    out->h = sh;
}

double lv_sdl_round(double d)
{
    return (d - (long) d) < 0.5 ? SDL_floor(d) : SDL_ceil(d);
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

SDL_Palette * lv_sdl_get_grayscale_palette(uint8_t bpp)
{
    SDL_Palette * palette;
    switch(bpp) {
        case 1:
            palette = lv_sdl_palette_grayscale1;
            break;
        case 2:
            palette = lv_sdl_palette_grayscale2;
            break;
        case 3:
            palette = lv_sdl_palette_grayscale3;
            break;
        case 4:
            palette = lv_sdl_palette_grayscale4;
            break;
        case 8:
            palette = lv_sdl_palette_grayscale8;
            break;
        default:
            return NULL;
    }
    LV_ASSERT_MSG(lv_sdl_palette_grayscale8, "lv_draw_sdl was not initialized properly");
    return palette;
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

lv_draw_sdl_backend_context_t * lv_draw_sdl_get_context()
{
    return lv_draw_backend_get()->ctx;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_SDL*/

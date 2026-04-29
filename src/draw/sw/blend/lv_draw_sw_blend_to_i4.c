/**
 * @file lv_draw_sw_blend_to_i4.c
 *
 * Software rasterizer for the I4 (4-bit indexed) destination color format.
 *
 * Pixel packing follows the wire convention used by indexed-color AMOLED
 * controllers like the CO5300 (datasheet §7.5.32 / §7.5.50): two pixels
 * per byte, with the lower-x pixel in the upper nibble (D[7:4]) and the
 * higher-x pixel in the lower nibble (D[3:0]). Stride is rounded up to
 * whole bytes — for an N-pixel-wide row the natural stride is `(N+1)/2`.
 *
 * Because I4 has 16 freely defined palette entries (rather than the
 * fixed black/white of I1), a palette must be supplied so that incoming
 * RGB565/ARGB8888/etc. pixels can be quantized to the closest entry.
 * The blender pulls the palette from the display via
 * `lv_display_get_palette()`. If none is set, a default 16-entry
 * grayscale palette is used (handy for development; production code
 * should set its own palette to match the panel's COLSET registers).
 *
 * Quantization is a 16-entry linear search over squared RGB distance.
 * On RV32 / Cortex-M this is well under a microsecond per pixel; for
 * panels driven at typical refresh rates it is not the bottleneck.
 * A faster lookup table can be added later if profiling demands it.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_i4.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_I4

#include "lv_draw_sw_blend_private.h"
#include "../../../misc/lv_math.h"
#include "../../../display/lv_display.h"
#include "../../../core/lv_refr.h"
#include "../../../core/lv_refr_private.h"
#include "../../../misc/lv_color.h"
#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

#define I4_PALETTE_SIZE 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ i4_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

#if LV_DRAW_SW_SUPPORT_L8
    static void /* LV_ATTRIBUTE_FAST_MEM */ l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_AL88
    static void /* LV_ATTRIBUTE_FAST_MEM */ al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565
    static void /* LV_ATTRIBUTE_FAST_MEM */ rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    static void /* LV_ATTRIBUTE_FAST_MEM */ rgb565_swapped_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888
    static void /* LV_ATTRIBUTE_FAST_MEM */ rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               const uint8_t src_px_size);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
    static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

static inline void /* LV_ATTRIBUTE_FAST_MEM */ blend_non_normal_pixel(uint8_t * dest_buf, int32_t dest_x,
                                                                      lv_color32_t src,
                                                                      lv_blend_mode_t mode,
                                                                      const lv_color32_t * palette);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ set_nibble(uint8_t * buf, int32_t idx, uint8_t val);
static inline uint8_t /* LV_ATTRIBUTE_FAST_MEM */ get_nibble(const uint8_t * buf, int32_t idx);

static inline uint8_t /* LV_ATTRIBUTE_FAST_MEM */ quantize_to_palette(lv_color32_t c,
                                                                      const lv_color32_t * palette);

static inline lv_color32_t blend_alpha_rgb(lv_color32_t src, lv_color32_t dst, uint8_t mix);

static inline const lv_color32_t * get_active_palette(void);

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    static inline lv_color16_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color16_from_u16(uint16_t raw);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Default 16-entry grayscale palette used when the display has no
 * `lv_display_set_palette()` registered. Index N is `(N * 17, N * 17, N * 17)`
 * — i.e. a uniform ramp from black (0) through 0x88 to white (0xFF).
 */
static const lv_color32_t default_grayscale_palette[I4_PALETTE_SIZE] = {
    {.blue = 0x00, .green = 0x00, .red = 0x00, .alpha = 0xFF},
    {.blue = 0x11, .green = 0x11, .red = 0x11, .alpha = 0xFF},
    {.blue = 0x22, .green = 0x22, .red = 0x22, .alpha = 0xFF},
    {.blue = 0x33, .green = 0x33, .red = 0x33, .alpha = 0xFF},
    {.blue = 0x44, .green = 0x44, .red = 0x44, .alpha = 0xFF},
    {.blue = 0x55, .green = 0x55, .red = 0x55, .alpha = 0xFF},
    {.blue = 0x66, .green = 0x66, .red = 0x66, .alpha = 0xFF},
    {.blue = 0x77, .green = 0x77, .red = 0x77, .alpha = 0xFF},
    {.blue = 0x88, .green = 0x88, .red = 0x88, .alpha = 0xFF},
    {.blue = 0x99, .green = 0x99, .red = 0x99, .alpha = 0xFF},
    {.blue = 0xAA, .green = 0xAA, .red = 0xAA, .alpha = 0xFF},
    {.blue = 0xBB, .green = 0xBB, .red = 0xBB, .alpha = 0xFF},
    {.blue = 0xCC, .green = 0xCC, .red = 0xCC, .alpha = 0xFF},
    {.blue = 0xDD, .green = 0xDD, .red = 0xDD, .alpha = 0xFF},
    {.blue = 0xEE, .green = 0xEE, .red = 0xEE, .alpha = 0xFF},
    {.blue = 0xFF, .green = 0xFF, .red = 0xFF, .alpha = 0xFF},
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_i4(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;
    int32_t dest_stride = dsc->dest_stride;
    uint8_t * dest_buf = dsc->dest_buf;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;

    const lv_color32_t * palette = get_active_palette();

    lv_color32_t src_argb = lv_color_to_32(dsc->color, 0xFF);
    uint8_t src_idx = quantize_to_palette(src_argb, palette);

    /* Simple fill */
    if(mask == NULL && opa >= LV_OPA_MAX) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                set_nibble(dest_buf, x + nibble_ofs, src_idx);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Opacity only */
    else if(mask == NULL && opa < LV_OPA_MAX) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t cur_idx = get_nibble(dest_buf, x + nibble_ofs);
                lv_color32_t mixed = blend_alpha_rgb(src_argb, palette[cur_idx], opa);
                set_nibble(dest_buf, x + nibble_ofs, quantize_to_palette(mixed, palette));
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Masked with full opacity */
    else if(mask && opa >= LV_OPA_MAX) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask[x];
                if(mask_val == LV_OPA_TRANSP) continue;
                if(mask_val == LV_OPA_COVER) {
                    set_nibble(dest_buf, x + nibble_ofs, src_idx);
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_argb, palette[cur_idx], mask_val);
                    set_nibble(dest_buf, x + nibble_ofs, quantize_to_palette(mixed, palette));
                }
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
    /* Masked with opacity */
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask[x];
                if(mask_val == LV_OPA_TRANSP) continue;
                uint8_t cur_idx = get_nibble(dest_buf, x + nibble_ofs);
                uint8_t blended_opa = LV_OPA_MIX2(mask_val, opa);
                lv_color32_t mixed = blend_alpha_rgb(src_argb, palette[cur_idx], blended_opa);
                set_nibble(dest_buf, x + nibble_ofs, quantize_to_palette(mixed, palette));
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_i4(lv_draw_sw_blend_image_dsc_t * dsc)
{
    switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_RGB565
        case LV_COLOR_FORMAT_RGB565:
            rgb565_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            rgb565_swapped_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
        case LV_COLOR_FORMAT_RGB888:
            rgb888_image_blend(dsc, 3);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_XRGB8888
        case LV_COLOR_FORMAT_XRGB8888:
            rgb888_image_blend(dsc, 4);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_L8
        case LV_COLOR_FORMAT_L8:
            l8_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_AL88
        case LV_COLOR_FORMAT_AL88:
            al88_image_blend(dsc);
            break;
#endif
        case LV_COLOR_FORMAT_I4:
            i4_image_blend(dsc);
            break;
        default:
            LV_LOG_WARN("Not supported source color format");
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void LV_ATTRIBUTE_FAST_MEM i4_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_i4 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_nibble_ofs = dsc->relative_area.x1 & 1;
    int32_t src_nibble_ofs = dsc->src_area.x1 & 1;

    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            /* I4 → I4: fast path, palette is identical, just copy nibbles. */
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x++) {
                    set_nibble(dest_buf_i4, x + dest_nibble_ofs,
                               get_nibble(src_buf_i4, x + src_nibble_ofs));
                }
                dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
                src_buf_i4 = drawbuf_next_row(src_buf_i4, src_stride);
            }
        }
        else {
            for(int32_t y = 0; y < h; y++) {
                for(int32_t x = 0; x < w; x++) {
                    uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                    if(mask_val == LV_OPA_TRANSP) continue;
                    uint8_t mix = LV_OPA_MIX2(mask_val, opa);
                    if(mix == 0) continue;
                    lv_color32_t src_c = palette[get_nibble(src_buf_i4, x + src_nibble_ofs)];
                    if(mix >= LV_OPA_MAX) {
                        set_nibble(dest_buf_i4, x + dest_nibble_ofs,
                                   quantize_to_palette(src_c, palette));
                    }
                    else {
                        uint8_t cur_idx = get_nibble(dest_buf_i4, x + dest_nibble_ofs);
                        lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                        set_nibble(dest_buf_i4, x + dest_nibble_ofs,
                                   quantize_to_palette(mixed, palette));
                    }
                }
                dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
                src_buf_i4 = drawbuf_next_row(src_buf_i4, src_stride);
                if(mask_buf) mask_buf += mask_stride;
            }
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                lv_color32_t src_c = palette[get_nibble(src_buf_i4, x + src_nibble_ofs)];
                if(mask_buf == NULL) src_c.alpha = opa;
                else src_c.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + dest_nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_i4 = drawbuf_next_row(src_buf_i4, src_stride);
        }
    }
}

#if LV_DRAW_SW_SUPPORT_L8
static void LV_ATTRIBUTE_FAST_MEM l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                if(mask_val == LV_OPA_TRANSP) continue;
                uint8_t mix = LV_OPA_MIX2(mask_val, opa);
                if(mix == 0) continue;

                lv_color32_t src_c = {
                    .blue = src_buf_l8[x],
                    .green = src_buf_l8[x],
                    .red = src_buf_l8[x],
                    .alpha = 0xFF,
                };
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                lv_color32_t src_c = {
                    .blue = src_buf_l8[x],
                    .green = src_buf_l8[x],
                    .red = src_buf_l8[x],
                    .alpha = 0xFF,
                };
                if(mask_buf == NULL) src_c.alpha = opa;
                else src_c.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_AL88
static void LV_ATTRIBUTE_FAST_MEM al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16a_t * src_buf_al88 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                uint8_t mix = LV_OPA_MIX3(src_buf_al88[x].alpha, mask_val, opa);
                if(mix == 0) continue;

                lv_color32_t src_c = {
                    .blue = src_buf_al88[x].lumi,
                    .green = src_buf_al88[x].lumi,
                    .red = src_buf_al88[x].lumi,
                    .alpha = 0xFF,
                };
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                lv_color32_t src_c = {
                    .blue = src_buf_al88[x].lumi,
                    .green = src_buf_al88[x].lumi,
                    .red = src_buf_al88[x].lumi,
                    .alpha = 0xFF,
                };
                if(mask_buf == NULL) src_c.alpha = LV_OPA_MIX2(src_buf_al88[x].alpha, opa);
                else src_c.alpha = LV_OPA_MIX3(src_buf_al88[x].alpha, mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
static void LV_ATTRIBUTE_FAST_MEM argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color32_t * src_buf_c32 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                uint8_t mix = LV_OPA_MIX3(src_buf_c32[x].alpha, mask_val, opa);
                if(mix == 0) continue;

                lv_color32_t src_c = src_buf_c32[x];
                src_c.alpha = 0xFF;
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                lv_color32_t src_c = src_buf_c32[x];
                if(mask_buf == NULL) src_c.alpha = LV_OPA_MIX2(src_c.alpha, opa);
                else src_c.alpha = LV_OPA_MIX3(src_c.alpha, mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888
static void LV_ATTRIBUTE_FAST_MEM rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                     const uint8_t src_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_rgb888 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        for(int32_t y = 0; y < h; y++) {
            int32_t src_x;
            int32_t dest_x;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                uint8_t mask_val = mask_buf ? mask_buf[dest_x] : LV_OPA_COVER;
                if(mask_val == LV_OPA_TRANSP) continue;
                uint8_t mix = LV_OPA_MIX2(mask_val, opa);
                if(mix == 0) continue;

                lv_color32_t src_c = {
                    .blue = src_buf_rgb888[src_x + 0],
                    .green = src_buf_rgb888[src_x + 1],
                    .red = src_buf_rgb888[src_x + 2],
                    .alpha = 0xFF,
                };
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, dest_x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, dest_x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, dest_x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_rgb888 = drawbuf_next_row(src_buf_rgb888, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            int32_t src_x;
            int32_t dest_x;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                lv_color32_t src_c = {
                    .blue = src_buf_rgb888[src_x + 0],
                    .green = src_buf_rgb888[src_x + 1],
                    .red = src_buf_rgb888[src_x + 2],
                    .alpha = 0xFF,
                };
                if(mask_buf == NULL) src_c.alpha = opa;
                else src_c.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);
                blend_non_normal_pixel(dest_buf_i4, dest_x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_rgb888 = drawbuf_next_row(src_buf_rgb888, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB565
static void LV_ATTRIBUTE_FAST_MEM rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16_t * src_buf_c16 = (const lv_color16_t *)dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                if(mask_val == LV_OPA_TRANSP) continue;
                uint8_t mix = LV_OPA_MIX2(mask_val, opa);
                if(mix == 0) continue;

                lv_color32_t src_c = {
                    .blue = (uint8_t)((src_buf_c16[x].blue * 2106) >> 8),
                    .green = (uint8_t)((src_buf_c16[x].green * 1037) >> 8),
                    .red = (uint8_t)((src_buf_c16[x].red * 2106) >> 8),
                    .alpha = 0xFF,
                };
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
    else {
        for(int32_t y = 0; y < h; y++) {
            for(int32_t x = 0; x < w; x++) {
                lv_color32_t src_c = {
                    .blue = (uint8_t)((src_buf_c16[x].blue * 2106) >> 8),
                    .green = (uint8_t)((src_buf_c16[x].green * 1037) >> 8),
                    .red = (uint8_t)((src_buf_c16[x].red * 2106) >> 8),
                    .alpha = 0xFF,
                };
                if(mask_buf == NULL) src_c.alpha = opa;
                else src_c.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
            src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static void LV_ATTRIBUTE_FAST_MEM rgb565_swapped_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_i4 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t nibble_ofs = dsc->relative_area.x1 & 1;
    const lv_color32_t * palette = get_active_palette();

    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            lv_color16_t px = lv_color16_from_u16(lv_color_swap_16(src_buf_u16[x]));
            lv_color32_t src_c = {
                .blue = (uint8_t)((px.blue * 2106) >> 8),
                .green = (uint8_t)((px.green * 1037) >> 8),
                .red = (uint8_t)((px.red * 2106) >> 8),
                .alpha = 0xFF,
            };

            if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
                uint8_t mask_val = mask_buf ? mask_buf[x] : LV_OPA_COVER;
                if(mask_val == LV_OPA_TRANSP) continue;
                uint8_t mix = LV_OPA_MIX2(mask_val, opa);
                if(mix == 0) continue;
                if(mix >= LV_OPA_MAX) {
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(src_c, palette));
                }
                else {
                    uint8_t cur_idx = get_nibble(dest_buf_i4, x + nibble_ofs);
                    lv_color32_t mixed = blend_alpha_rgb(src_c, palette[cur_idx], mix);
                    set_nibble(dest_buf_i4, x + nibble_ofs,
                               quantize_to_palette(mixed, palette));
                }
            }
            else {
                if(mask_buf == NULL) src_c.alpha = opa;
                else src_c.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(dest_buf_i4, x + nibble_ofs, src_c,
                                       dsc->blend_mode, palette);
            }
        }
        if(mask_buf) mask_buf += mask_stride;
        dest_buf_i4 = drawbuf_next_row(dest_buf_i4, dest_stride);
        src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
    }
}
#endif

static inline void LV_ATTRIBUTE_FAST_MEM blend_non_normal_pixel(uint8_t * dest_buf, int32_t dest_x,
                                                                lv_color32_t src,
                                                                lv_blend_mode_t mode,
                                                                const lv_color32_t * palette)
{
    uint8_t cur_idx = get_nibble(dest_buf, dest_x);
    lv_color32_t dst = palette[cur_idx];
    lv_color32_t res = dst;

    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            res.red = (uint8_t)LV_MIN((int32_t)dst.red + src.red, 255);
            res.green = (uint8_t)LV_MIN((int32_t)dst.green + src.green, 255);
            res.blue = (uint8_t)LV_MIN((int32_t)dst.blue + src.blue, 255);
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            res.red = (uint8_t)LV_MAX((int32_t)dst.red - src.red, 0);
            res.green = (uint8_t)LV_MAX((int32_t)dst.green - src.green, 0);
            res.blue = (uint8_t)LV_MAX((int32_t)dst.blue - src.blue, 0);
            break;
        case LV_BLEND_MODE_MULTIPLY:
            res.red = (uint8_t)((dst.red * src.red) >> 8);
            res.green = (uint8_t)((dst.green * src.green) >> 8);
            res.blue = (uint8_t)((dst.blue * src.blue) >> 8);
            break;
        case LV_BLEND_MODE_DIFFERENCE:
            res.red = (uint8_t)LV_ABS((int32_t)dst.red - src.red);
            res.green = (uint8_t)LV_ABS((int32_t)dst.green - src.green);
            res.blue = (uint8_t)LV_ABS((int32_t)dst.blue - src.blue);
            break;
        default:
            LV_LOG_WARN("Not supported blend mode: %d", mode);
            return;
    }
    res.alpha = 0xFF;

    /*Mix the post-blend result back into the destination by src.alpha.*/
    lv_color32_t mixed = (src.alpha >= LV_OPA_MAX) ? res : blend_alpha_rgb(res, dst, src.alpha);
    set_nibble(dest_buf, dest_x, quantize_to_palette(mixed, palette));
}

static inline lv_color32_t blend_alpha_rgb(lv_color32_t src, lv_color32_t dst, uint8_t mix)
{
    /*src over dst, mix is the effective opacity (0..255).*/
    lv_color32_t out;
    uint8_t inv = 255 - mix;
    out.red = (uint8_t)(((uint32_t)src.red * mix + (uint32_t)dst.red * inv) >> 8);
    out.green = (uint8_t)(((uint32_t)src.green * mix + (uint32_t)dst.green * inv) >> 8);
    out.blue = (uint8_t)(((uint32_t)src.blue * mix + (uint32_t)dst.blue * inv) >> 8);
    out.alpha = 0xFF;
    return out;
}

static inline uint8_t LV_ATTRIBUTE_FAST_MEM quantize_to_palette(lv_color32_t c,
                                                                const lv_color32_t * palette)
{
    uint32_t best_dist = 0xFFFFFFFFu;
    uint8_t best_idx = 0;
    for(uint8_t i = 0; i < I4_PALETTE_SIZE; i++) {
        int32_t dr = (int32_t)c.red - palette[i].red;
        int32_t dg = (int32_t)c.green - palette[i].green;
        int32_t db = (int32_t)c.blue - palette[i].blue;
        uint32_t d = (uint32_t)(dr * dr + dg * dg + db * db);
        if(d < best_dist) {
            best_dist = d;
            best_idx = i;
            if(d == 0) break;
        }
    }
    return best_idx;
}

static inline const lv_color32_t * get_active_palette(void)
{
    lv_display_t * disp = lv_refr_get_disp_refreshing();
    if(disp != NULL) {
        const lv_color32_t * p = lv_display_get_palette(disp);
        uint32_t n = lv_display_get_palette_size(disp);
        if(p != NULL && n >= I4_PALETTE_SIZE) return p;
    }
    return default_grayscale_palette;
}

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

static inline void LV_ATTRIBUTE_FAST_MEM set_nibble(uint8_t * buf, int32_t idx, uint8_t val)
{
    uint8_t * b = &buf[idx >> 1];
    if(idx & 1) {
        *b = (uint8_t)((*b & 0xF0) | (val & 0x0F));
    }
    else {
        *b = (uint8_t)((*b & 0x0F) | ((val & 0x0F) << 4));
    }
}

static inline uint8_t LV_ATTRIBUTE_FAST_MEM get_nibble(const uint8_t * buf, int32_t idx)
{
    uint8_t b = buf[idx >> 1];
    return (idx & 1) ? (uint8_t)(b & 0x0F) : (uint8_t)(b >> 4);
}

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
static inline lv_color16_t LV_ATTRIBUTE_FAST_MEM lv_color16_from_u16(uint16_t raw)
{
    lv_color16_t c;
    c.red = (raw >> 11) & 0x1F;
    c.green = (raw >> 5) & 0x3F;
    c.blue = raw & 0x1F;
    return c;
}
#endif

#endif /*LV_DRAW_SW_SUPPORT_I4*/

#endif /*LV_USE_DRAW_SW*/

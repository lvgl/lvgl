/**
 * @file lv_draw_sw_blend_to_rgb565.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_rgb565.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_RGB565

#include "lv_draw_sw_blend_private.h"
#include "../../../misc/lv_math.h"
#include "../../../display/lv_display.h"
#include "../../../core/lv_refr.h"
#include "../../../misc/lv_color.h"
#include "../../../stdlib/lv_string.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON
    #include "neon/lv_blend_neon.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "helium/lv_blend_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if LV_DRAW_SW_SUPPORT_AL88
    static void /* LV_ATTRIBUTE_FAST_MEM */ al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_I1
    static void /* LV_ATTRIBUTE_FAST_MEM */ i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

    static inline uint8_t /* LV_ATTRIBUTE_FAST_MEM */ get_bit(const uint8_t * buf, int32_t bit_idx);
#endif

#if LV_DRAW_SW_SUPPORT_L8
    static void /* LV_ATTRIBUTE_FAST_MEM */ l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

static void /* LV_ATTRIBUTE_FAST_MEM */ rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

#if LV_DRAW_SW_SUPPORT_RGB888
static void /* LV_ATTRIBUTE_FAST_MEM */ rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                           const uint8_t src_px_size);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
    static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

static inline uint16_t /* LV_ATTRIBUTE_FAST_MEM */ l8_to_rgb565(const uint8_t c1);

static inline uint16_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color_8_16_mix(const uint8_t c1, uint16_t c2, uint8_t mix);

static inline uint16_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color_24_16_mix(const uint8_t * c1, uint16_t c2, uint8_t mix);

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(...)                           LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(...)                  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(...)              LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565(...)                       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)              LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565(...)                       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)              LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)        LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_MASK
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_MASK(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Fill an area with a color.
 * Supports normal fill, fill with opacity, fill with mask, and fill with mask and opacity.
 * dest_buf and color have native color depth. (RGB565, RGB888, XRGB8888)
 * The background (dest_buf) cannot have alpha channel
 * @param dest_buf
 * @param dest_area
 * @param dest_stride
 * @param color
 * @param opa
 * @param mask
 * @param mask_stride
 */
void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_rgb565(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    uint16_t color16 = lv_color_to_u16(dsc->color);
    lv_opa_t opa = dsc->opa;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;

    int32_t x;
    int32_t y;

    LV_UNUSED(w);
    LV_UNUSED(h);
    LV_UNUSED(x);
    LV_UNUSED(y);
    LV_UNUSED(opa);
    LV_UNUSED(mask);
    LV_UNUSED(color16);
    LV_UNUSED(mask_stride);
    LV_UNUSED(dest_stride);
    LV_UNUSED(dest_buf_u16);

    /*Simple fill*/
    if(mask == NULL && opa >= LV_OPA_MAX)  {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB565(dsc)) {
            for(y = 0; y < h; y++) {
                uint16_t * dest_end_final = dest_buf_u16 + w;
                uint32_t * dest_end_mid = (uint32_t *)((uint16_t *) dest_buf_u16 + ((w - 1) & ~(0xF)));
                if((lv_uintptr_t)&dest_buf_u16[0] & 0x3) {
                    dest_buf_u16[0] = color16;
                    dest_buf_u16++;
                }

                uint32_t c32 = (uint32_t)color16 + ((uint32_t)color16 << 16);
                uint32_t * dest32 = (uint32_t *)dest_buf_u16;
                while(dest32 < dest_end_mid) {
                    dest32[0] = c32;
                    dest32[1] = c32;
                    dest32[2] = c32;
                    dest32[3] = c32;
                    dest32[4] = c32;
                    dest32[5] = c32;
                    dest32[6] = c32;
                    dest32[7] = c32;
                    dest32 += 8;
                }

                dest_buf_u16 = (uint16_t *)dest32;

                while(dest_buf_u16 < dest_end_final) {
                    *dest_buf_u16 = color16;
                    dest_buf_u16++;
                }

                dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                dest_buf_u16 -= w;
            }
        }

    }
    /*Opacity only*/
    else if(mask == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(dsc)) {
            uint32_t last_dest32_color = dest_buf_u16[0] + 1; /*Set to value which is not equal to the first pixel*/
            uint32_t last_res32_color = 0;

            for(y = 0; y < h; y++) {
                x = 0;
                if((lv_uintptr_t)&dest_buf_u16[0] & 0x3) {
                    dest_buf_u16[0] = lv_color_16_16_mix(color16, dest_buf_u16[0], opa);
                    x = 1;
                }

                for(; x < w - 2; x += 2) {
                    if(dest_buf_u16[x] != dest_buf_u16[x + 1]) {
                        dest_buf_u16[x + 0] = lv_color_16_16_mix(color16, dest_buf_u16[x + 0], opa);
                        dest_buf_u16[x + 1] = lv_color_16_16_mix(color16, dest_buf_u16[x + 1], opa);
                    }
                    else {
                        volatile uint32_t * dest32 = (uint32_t *)&dest_buf_u16[x];
                        if(last_dest32_color == *dest32) {
                            *dest32 = last_res32_color;
                        }
                        else {
                            last_dest32_color =  *dest32;

                            dest_buf_u16[x] = lv_color_16_16_mix(color16, dest_buf_u16[x + 0], opa);
                            dest_buf_u16[x + 1] = dest_buf_u16[x];

                            last_res32_color = *dest32;
                        }
                    }
                }

                for(; x < w ; x++) {
                    dest_buf_u16[x] = lv_color_16_16_mix(color16, dest_buf_u16[x], opa);
                }
                dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            }
        }

    }

    /*Masked with full opacity*/
    else if(mask && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                x = 0;
                if((lv_uintptr_t)(mask) & 0x1) {
                    dest_buf_u16[x] = lv_color_16_16_mix(color16, dest_buf_u16[x], mask[x]);
                    x++;
                }

                for(; x <= w - 2; x += 2) {
                    uint16_t mask16 = *((uint16_t *)&mask[x]);
                    if(mask16 == 0xFFFF) {
                        dest_buf_u16[x + 0] = color16;
                        dest_buf_u16[x + 1] = color16;
                    }
                    else if(mask16 != 0) {
                        dest_buf_u16[x + 0] = lv_color_16_16_mix(color16, dest_buf_u16[x + 0], mask[x + 0]);
                        dest_buf_u16[x + 1] = lv_color_16_16_mix(color16, dest_buf_u16[x + 1], mask[x + 1]);
                    }
                }

                for(; x < w ; x++) {
                    dest_buf_u16[x] = lv_color_16_16_mix(color16, dest_buf_u16[x], mask[x]);
                }
                dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                mask += mask_stride;
            }
        }

    }
    /*Masked with opacity*/
    else if(mask && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    dest_buf_u16[x] = lv_color_16_16_mix(color16, dest_buf_u16[x], LV_OPA_MIX2(mask[x], opa));
                }
                dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                mask += mask_stride;
            }
        }
    }
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc)
{
    switch(dsc->src_color_format) {
        case LV_COLOR_FORMAT_RGB565:
            rgb565_image_blend(dsc);
            break;
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
#if LV_DRAW_SW_SUPPORT_I1
        case LV_COLOR_FORMAT_I1:
            i1_image_blend(dsc);
            break;
#endif
        default:
            LV_LOG_WARN("Not supported source color format");
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_SW_SUPPORT_I1
static void LV_ATTRIBUTE_FAST_MEM i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_i1 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_u16[dest_x] = l8_to_rgb565(chan_val);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(chan_val, dest_buf_u16[dest_x], opa);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)) {

                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(chan_val, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(chan_val, dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                uint16_t res = 0;
                uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        // Additive blending mode
                        res = (LV_MIN(dest_buf_u16[dest_x] + l8_to_rgb565(chan_val), 0xFFFF));
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        // Subtractive blending mode
                        res = (LV_MAX(dest_buf_u16[dest_x] - l8_to_rgb565(chan_val), 0));
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        // Multiply blending mode
                        res = ((((dest_buf_u16[dest_x] >> 11) * (l8_to_rgb565(chan_val) >> 3)) & 0x1F) << 11) |
                              ((((dest_buf_u16[dest_x] >> 5) & 0x3F) * ((l8_to_rgb565(chan_val) >> 2) & 0x3F) >> 6) << 5) |
                              (((dest_buf_u16[dest_x] & 0x1F) * (l8_to_rgb565(chan_val) & 0x1F)) >> 5);
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }

                if(mask_buf == NULL && opa >= LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = res;
                }
                else if(mask_buf == NULL && opa < LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], opa);
                }
                else {
                    if(opa >= LV_OPA_MAX)
                        dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    else
                        dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                }
            }

            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
            if(mask_buf) mask_buf += mask_stride;
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
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16a_t * src_buf_al88 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_al88[src_x].lumi, dest_buf_u16[dest_x], src_buf_al88[src_x].alpha);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_al88[src_x].lumi, dest_buf_u16[dest_x],
                                                                 LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_al88[src_x].lumi, dest_buf_u16[dest_x],
                                                                 LV_OPA_MIX2(src_buf_al88[src_x].alpha, mask_buf[dest_x]));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_al88[src_x].lumi, dest_buf_u16[dest_x],
                                                                 LV_OPA_MIX3(src_buf_al88[src_x].alpha, mask_buf[dest_x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        uint16_t res = 0;
        for(y = 0; y < h; y++) {
            lv_color16_t * dest_buf_c16 = (lv_color16_t *)dest_buf_u16;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                uint8_t rb = src_buf_al88[src_x].lumi >> 3;
                uint8_t g = src_buf_al88[src_x].lumi >> 2;
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        res = (LV_MIN(dest_buf_c16[dest_x].red + rb, 31)) << 11;
                        res += (LV_MIN(dest_buf_c16[dest_x].green + g, 63)) << 5;
                        res += LV_MIN(dest_buf_c16[dest_x].blue + rb, 31);
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        res = (LV_MAX(dest_buf_c16[dest_x].red - rb, 0)) << 11;
                        res += (LV_MAX(dest_buf_c16[dest_x].green - g, 0)) << 5;
                        res += LV_MAX(dest_buf_c16[dest_x].blue - rb, 0);
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        res = ((dest_buf_c16[dest_x].red * rb) >> 5) << 11;
                        res += ((dest_buf_c16[dest_x].green * g) >> 6) << 5;
                        res += (dest_buf_c16[dest_x].blue * rb) >> 5;
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }
                if(mask_buf == NULL && opa >= LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], src_buf_al88[src_x].alpha);
                }
                else if(mask_buf == NULL && opa < LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX2(opa, src_buf_al88[src_x].alpha));
                }
                else {
                    if(opa >= LV_OPA_MAX) dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    else dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX3(mask_buf[dest_x], opa,
                                                                                                              src_buf_al88[src_x].alpha));
                }
            }

            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_L8

static void LV_ATTRIBUTE_FAST_MEM l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_l8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = l8_to_rgb565(src_buf_l8[src_x]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_l8 += src_stride;
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_l8[src_x], dest_buf_u16[dest_x], opa);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_l8 += src_stride;
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_l8[src_x], dest_buf_u16[dest_x], mask_buf[dest_x]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_l8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x++) {
                        dest_buf_u16[dest_x] = lv_color_8_16_mix(src_buf_l8[src_x], dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_l8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        uint16_t res = 0;
        for(y = 0; y < h; y++) {
            lv_color16_t * dest_buf_c16 = (lv_color16_t *)dest_buf_u16;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                uint8_t rb = src_buf_l8[src_x] >> 3;
                uint8_t g = src_buf_l8[src_x] >> 2;
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        res = (LV_MIN(dest_buf_c16[dest_x].red + rb, 31)) << 11;
                        res += (LV_MIN(dest_buf_c16[dest_x].green + g, 63)) << 5;
                        res += LV_MIN(dest_buf_c16[dest_x].blue + rb, 31);
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        res = (LV_MAX(dest_buf_c16[dest_x].red - rb, 0)) << 11;
                        res += (LV_MAX(dest_buf_c16[dest_x].green - g, 0)) << 5;
                        res += LV_MAX(dest_buf_c16[dest_x].blue - rb, 0);
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        res = ((dest_buf_c16[dest_x].red * rb) >> 5) << 11;
                        res += ((dest_buf_c16[dest_x].green * g) >> 6) << 5;
                        res += (dest_buf_c16[dest_x].blue * rb) >> 5;
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }

                if(mask_buf == NULL && opa >= LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = res;
                }
                else if(mask_buf == NULL && opa < LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], opa);
                }
                else {
                    if(opa >= LV_OPA_MAX) dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    else dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                }
            }

            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_l8 += src_stride;
            if(mask_buf) mask_buf += mask_stride;
        }
    }
}

#endif

static void LV_ATTRIBUTE_FAST_MEM rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(dsc)) {
                uint32_t line_in_bytes = w * 2;
                for(y = 0; y < h; y++) {
                    lv_memcpy(dest_buf_u16, src_buf_u16, line_in_bytes);
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_u16[x] = lv_color_16_16_mix(src_buf_u16[x], dest_buf_u16[x], opa);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_u16[x] = lv_color_16_16_mix(src_buf_u16[x], dest_buf_u16[x], mask_buf[x]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_u16[x] = lv_color_16_16_mix(src_buf_u16[x], dest_buf_u16[x], LV_OPA_MIX2(mask_buf[x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        uint16_t res = 0;
        for(y = 0; y < h; y++) {
            lv_color16_t * dest_buf_c16 = (lv_color16_t *) dest_buf_u16;
            lv_color16_t * src_buf_c16 = (lv_color16_t *) src_buf_u16;
            for(x = 0; x < w; x++) {
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        if(src_buf_u16[x] == 0x0000) continue;   /*Do not add pure black*/
                        res = (LV_MIN(dest_buf_c16[x].red + src_buf_c16[x].red, 31)) << 11;
                        res += (LV_MIN(dest_buf_c16[x].green + src_buf_c16[x].green, 63)) << 5;
                        res += LV_MIN(dest_buf_c16[x].blue + src_buf_c16[x].blue, 31);
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        if(src_buf_u16[x] == 0x0000) continue;   /*Do not subtract pure black*/
                        res = (LV_MAX(dest_buf_c16[x].red - src_buf_c16[x].red, 0)) << 11;
                        res += (LV_MAX(dest_buf_c16[x].green - src_buf_c16[x].green, 0)) << 5;
                        res += LV_MAX(dest_buf_c16[x].blue - src_buf_c16[x].blue, 0);
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        if(src_buf_u16[x] == 0xffff) continue;   /*Do not multiply with pure white (considered as 1)*/
                        res = ((dest_buf_c16[x].red * src_buf_c16[x].red) >> 5) << 11;
                        res += ((dest_buf_c16[x].green * src_buf_c16[x].green) >> 6) << 5;
                        res += (dest_buf_c16[x].blue * src_buf_c16[x].blue) >> 5;
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }

                if(mask_buf == NULL) {
                    dest_buf_u16[x] = lv_color_16_16_mix(res, dest_buf_u16[x], opa);
                }
                else {
                    if(opa >= LV_OPA_MAX) dest_buf_u16[x] = lv_color_16_16_mix(res, dest_buf_u16[x], mask_buf[x]);
                    else dest_buf_u16[x] = lv_color_16_16_mix(res, dest_buf_u16[x], LV_OPA_MIX2(mask_buf[x], opa));
                }
            }

            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
            if(mask_buf) mask_buf += mask_stride;
        }
    }
}

#if LV_DRAW_SW_SUPPORT_RGB888

static void LV_ATTRIBUTE_FAST_MEM rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, const uint8_t src_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        dest_buf_u16[dest_x]  = ((src_buf_u8[src_x + 2] & 0xF8) << 8) +
                                                ((src_buf_u8[src_x + 1] & 0xFC) << 3) +
                                                ((src_buf_u8[src_x + 0] & 0xF8) >> 3);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x], opa);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                }
            }
        }
        if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x], mask_buf[dest_x]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
        if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        uint16_t res = 0;
        for(y = 0; y < h; y++) {
            lv_color16_t * dest_buf_c16 = (lv_color16_t *) dest_buf_u16;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        res = (LV_MIN(dest_buf_c16[dest_x].red + (src_buf_u8[src_x + 2] >> 3), 31)) << 11;
                        res += (LV_MIN(dest_buf_c16[dest_x].green + (src_buf_u8[src_x + 1] >> 2), 63)) << 5;
                        res += LV_MIN(dest_buf_c16[dest_x].blue + (src_buf_u8[src_x + 0] >> 3), 31);
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        res = (LV_MAX(dest_buf_c16[dest_x].red - (src_buf_u8[src_x + 2] >> 3), 0)) << 11;
                        res += (LV_MAX(dest_buf_c16[dest_x].green - (src_buf_u8[src_x + 1] >> 2), 0)) << 5;
                        res += LV_MAX(dest_buf_c16[dest_x].blue - (src_buf_u8[src_x + 0] >> 3), 0);
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        res = ((dest_buf_c16[dest_x].red * (src_buf_u8[src_x + 2] >> 3)) >> 5) << 11;
                        res += ((dest_buf_c16[dest_x].green * (src_buf_u8[src_x + 1] >> 2)) >> 6) << 5;
                        res += (dest_buf_c16[dest_x].blue * (src_buf_u8[src_x + 0] >> 3)) >> 5;
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }

                if(mask_buf == NULL) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], opa);
                }
                else {
                    if(opa >= LV_OPA_MAX) dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    else dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX2(mask_buf[dest_x], opa));
                }
            }
            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_u8 += src_stride;
            if(mask_buf) mask_buf += mask_stride;
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
    uint16_t * dest_buf_u16 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_u8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x], src_buf_u8[src_x + 3]);
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x], LV_OPA_MIX2(src_buf_u8[src_x + 3],
                                                                                                                        opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x],
                                                                  LV_OPA_MIX2(src_buf_u8[src_x + 3], mask_buf[dest_x]));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                        dest_buf_u16[dest_x] = lv_color_24_16_mix(&src_buf_u8[src_x], dest_buf_u16[dest_x],
                                                                  LV_OPA_MIX3(src_buf_u8[src_x + 3], mask_buf[dest_x], opa));
                    }
                    dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
                    src_buf_u8 += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        uint16_t res = 0;
        for(y = 0; y < h; y++) {
            lv_color16_t * dest_buf_c16 = (lv_color16_t *) dest_buf_u16;
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 4) {
                switch(dsc->blend_mode) {
                    case LV_BLEND_MODE_ADDITIVE:
                        res = (LV_MIN(dest_buf_c16[dest_x].red + (src_buf_u8[src_x + 2] >> 3), 31)) << 11;
                        res += (LV_MIN(dest_buf_c16[dest_x].green + (src_buf_u8[src_x + 1] >> 2), 63)) << 5;
                        res += LV_MIN(dest_buf_c16[dest_x].blue + (src_buf_u8[src_x + 0] >> 3), 31);
                        break;
                    case LV_BLEND_MODE_SUBTRACTIVE:
                        res = (LV_MAX(dest_buf_c16[dest_x].red - (src_buf_u8[src_x + 2] >> 3), 0)) << 11;
                        res += (LV_MAX(dest_buf_c16[dest_x].green - (src_buf_u8[src_x + 1] >> 2), 0)) << 5;
                        res += LV_MAX(dest_buf_c16[dest_x].blue - (src_buf_u8[src_x + 0] >> 3), 0);
                        break;
                    case LV_BLEND_MODE_MULTIPLY:
                        res = ((dest_buf_c16[dest_x].red * (src_buf_u8[src_x + 2] >> 3)) >> 5) << 11;
                        res += ((dest_buf_c16[dest_x].green * (src_buf_u8[src_x + 1] >> 2)) >> 6) << 5;
                        res += (dest_buf_c16[dest_x].blue * (src_buf_u8[src_x + 0] >> 3)) >> 5;
                        break;
                    default:
                        LV_LOG_WARN("Not supported blend mode: %d", dsc->blend_mode);
                        return;
                }

                if(mask_buf == NULL && opa >= LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], src_buf_u8[src_x + 3]);
                }
                else if(mask_buf == NULL && opa < LV_OPA_MAX) {
                    dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX2(opa, src_buf_u8[src_x + 3]));
                }
                else {
                    if(opa >= LV_OPA_MAX) dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], mask_buf[dest_x]);
                    else dest_buf_u16[dest_x] = lv_color_16_16_mix(res, dest_buf_u16[dest_x], LV_OPA_MIX3(mask_buf[dest_x], opa,
                                                                                                              src_buf_u8[src_x + 3]));
                }
            }

            dest_buf_u16 = drawbuf_next_row(dest_buf_u16, dest_stride);
            src_buf_u8 += src_stride;
            if(mask_buf) mask_buf += mask_stride;
        }
    }
}

#endif

static inline uint16_t LV_ATTRIBUTE_FAST_MEM l8_to_rgb565(const uint8_t c1)
{
    return ((c1 & 0xF8) << 8) + ((c1 & 0xFC) << 3) + ((c1 & 0xF8) >> 3);
}

static inline uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_8_16_mix(const uint8_t c1, uint16_t c2, uint8_t mix)
{

    if(mix == 0) {
        return c2;
    }
    else if(mix == 255) {
        return ((c1 & 0xF8) << 8) + ((c1 & 0xFC) << 3) + ((c1 & 0xF8) >> 3);
    }
    else {
        lv_opa_t mix_inv = 255 - mix;

        return ((((c1 >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
               ((((c1 >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
               (((c1 >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8);
    }
}

static inline uint16_t LV_ATTRIBUTE_FAST_MEM lv_color_24_16_mix(const uint8_t * c1, uint16_t c2, uint8_t mix)
{
    if(mix == 0) {
        return c2;
    }
    else if(mix == 255) {
        return ((c1[2] & 0xF8) << 8)  + ((c1[1] & 0xFC) << 3) + ((c1[0] & 0xF8) >> 3);
    }
    else {
        lv_opa_t mix_inv = 255 - mix;

        return ((((c1[2] >> 3) * mix + ((c2 >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
               ((((c1[1] >> 2) * mix + ((c2 >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
               (((c1[0] >> 3) * mix + (c2 & 0x1F) * mix_inv) >> 8);
    }
}

#if LV_DRAW_SW_SUPPORT_I1

static inline uint8_t LV_ATTRIBUTE_FAST_MEM get_bit(const uint8_t * buf, int32_t bit_idx)
{
    return (buf[bit_idx / 8] >> (7 - (bit_idx % 8))) & 1;
}

#endif

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

#endif

#endif

/**
 * @file lv_draw_sw_blend_to_rgb888.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_rgb888.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_RGB888

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
    static void /* LV_ATTRIBUTE_FAST_MEM */ al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
#endif

#if LV_DRAW_SW_SUPPORT_I1
    static void /* LV_ATTRIBUTE_FAST_MEM */ i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);

    static inline uint8_t /* LV_ATTRIBUTE_FAST_MEM */ get_bit(const uint8_t * buf, int32_t bit_idx);
#endif

#if LV_DRAW_SW_SUPPORT_L8
    static void /* LV_ATTRIBUTE_FAST_MEM */ l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
#endif

#if LV_DRAW_SW_SUPPORT_RGB565
    static void /* LV_ATTRIBUTE_FAST_MEM */ rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
#endif

static void /* LV_ATTRIBUTE_FAST_MEM */ rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                           const uint8_t dest_px_size,
                                                           uint32_t src_px_size);

#if LV_DRAW_SW_SUPPORT_ARGB8888
static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                             uint32_t dest_px_size);
#endif

static inline void /* LV_ATTRIBUTE_FAST_MEM */ lv_color_8_24_mix(const uint8_t src, uint8_t * dest, uint8_t mix);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ lv_color_24_24_mix(const uint8_t * src, uint8_t * dest, uint8_t mix);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ blend_non_normal_pixel(uint8_t * dest, lv_color32_t src,
                                                                      lv_blend_mode_t mode);
static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(...)                           LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(...)                  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(...)              LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888(...)                       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_OPA(...)              LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_MASK
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(...)         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(...)          LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(...)         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(...)        LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(...)    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_888
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_888(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_MASK
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_MASK(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_MIX_MASK_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;
    int32_t dest_stride = dsc->dest_stride;

    int32_t x;
    int32_t y;

    LV_UNUSED(w);
    LV_UNUSED(h);
    LV_UNUSED(x);
    LV_UNUSED(y);
    LV_UNUSED(opa);
    LV_UNUSED(mask);
    LV_UNUSED(mask_stride);
    LV_UNUSED(dest_stride);

    /*Simple fill*/
    if(mask == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dest_px_size)) {
            if(dest_px_size == 3) {
                uint8_t * dest_buf_u8 = dsc->dest_buf;
                uint8_t * dest_buf_ori = dsc->dest_buf;
                w *= dest_px_size;

                for(x = 0; x < w; x += 3) {
                    dest_buf_u8[x + 0] = dsc->color.blue;
                    dest_buf_u8[x + 1] = dsc->color.green;
                    dest_buf_u8[x + 2] = dsc->color.red;
                }

                dest_buf_u8 += dest_stride;

                for(y = 1; y < h; y++) {
                    lv_memcpy(dest_buf_u8, dest_buf_ori, w);
                    dest_buf_u8 += dest_stride;
                }
            }
            if(dest_px_size == 4) {
                uint32_t color32 = lv_color_to_u32(dsc->color);
                uint32_t * dest_buf_u32 = dsc->dest_buf;
                for(y = 0; y < h; y++) {
                    for(x = 0; x <= w - 16; x += 16) {
                        dest_buf_u32[x + 0] = color32;
                        dest_buf_u32[x + 1] = color32;
                        dest_buf_u32[x + 2] = color32;
                        dest_buf_u32[x + 3] = color32;

                        dest_buf_u32[x + 4] = color32;
                        dest_buf_u32[x + 5] = color32;
                        dest_buf_u32[x + 6] = color32;
                        dest_buf_u32[x + 7] = color32;

                        dest_buf_u32[x + 8] = color32;
                        dest_buf_u32[x + 9] = color32;
                        dest_buf_u32[x + 10] = color32;
                        dest_buf_u32[x + 11] = color32;

                        dest_buf_u32[x + 12] = color32;
                        dest_buf_u32[x + 13] = color32;
                        dest_buf_u32[x + 14] = color32;
                        dest_buf_u32[x + 15] = color32;
                    }
                    for(; x < w; x ++) {
                        dest_buf_u32[x] = color32;
                    }

                    dest_buf_u32 = drawbuf_next_row(dest_buf_u32, dest_stride);
                }
            }
        }
    }
    /*Opacity only*/
    else if(mask == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dest_px_size)) {
            uint32_t color32 = lv_color_to_u32(dsc->color);
            uint8_t * dest_buf = dsc->dest_buf;
            w *= dest_px_size;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x += dest_px_size) {
                    lv_color_24_24_mix((const uint8_t *)&color32, &dest_buf[x], opa);
                }

                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            }
        }
    }
    /*Masked with full opacity*/
    else if(mask && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dest_px_size)) {
            uint32_t color32 = lv_color_to_u32(dsc->color);
            uint8_t * dest_buf = dsc->dest_buf;
            w *= dest_px_size;

            for(y = 0; y < h; y++) {
                uint32_t mask_x;
                for(x = 0, mask_x = 0; x < w; x += dest_px_size, mask_x++) {
                    lv_color_24_24_mix((const uint8_t *)&color32, &dest_buf[x], mask[mask_x]);
                }
                dest_buf += dest_stride;
                mask += mask_stride;
            }
        }
    }
    /*Masked with opacity*/
    else {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size)) {
            uint32_t color32 = lv_color_to_u32(dsc->color);
            uint8_t * dest_buf = dsc->dest_buf;
            w *= dest_px_size;

            for(y = 0; y < h; y++) {
                uint32_t mask_x;
                for(x = 0, mask_x = 0; x < w; x += dest_px_size, mask_x++) {
                    lv_color_24_24_mix((const uint8_t *) &color32, &dest_buf[x], LV_OPA_MIX2(opa, mask[mask_x]));
                }
                dest_buf += dest_stride;
                mask += mask_stride;
            }
        }
    }
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{

    switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_RGB565
        case LV_COLOR_FORMAT_RGB565:
            rgb565_image_blend(dsc, dest_px_size);
            break;
#endif
        case LV_COLOR_FORMAT_RGB888:
            rgb888_image_blend(dsc, dest_px_size, 3);
            break;
#if LV_DRAW_SW_SUPPORT_XRGB8888
        case LV_COLOR_FORMAT_XRGB8888:
            rgb888_image_blend(dsc, dest_px_size, 4);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc, dest_px_size);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_L8
        case LV_COLOR_FORMAT_L8:
            l8_image_blend(dsc, dest_px_size);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_AL88
        case LV_COLOR_FORMAT_AL88:
            al88_image_blend(dsc, dest_px_size);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_I1
        case LV_COLOR_FORMAT_I1:
            i1_image_blend(dsc, dest_px_size);
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
static void LV_ATTRIBUTE_FAST_MEM i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_u8[dest_x + 2] = chan_val;
                        dest_buf_u8[dest_x + 1] = chan_val;
                        dest_buf_u8[dest_x + 0] = chan_val;
                    }
                    dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_24_mix(chan_val, &dest_buf_u8[dest_x], opa);
                    }
                    dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_24_mix(chan_val, &dest_buf_u8[dest_x], mask_buf[src_x]);
                    }
                    dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_24_mix(chan_val, &dest_buf_u8[dest_x], LV_OPA_MIX2(opa, mask_buf[src_x]));
                    }
                    dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                lv_color32_t src_argb;
                src_argb.red = get_bit(src_buf_i1, src_x) * 255;
                src_argb.green = src_argb.red;
                src_argb.blue = src_argb.red;
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[src_x], opa);
                blend_non_normal_pixel(&dest_buf_u8[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_u8 = drawbuf_next_row(dest_buf_u8, dest_stride);
            src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
        }
    }
}
#endif

#if LV_DRAW_SW_SUPPORT_AL88
static void LV_ATTRIBUTE_FAST_MEM al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_al88[src_x].lumi, &dest_buf_u8[dest_x], src_buf_al88[src_x].alpha);
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_al88[src_x].lumi, &dest_buf_u8[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa));
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_al88[src_x].lumi, &dest_buf_u8[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha,
                                                                                                      mask_buf[src_x]));
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_al88[src_x].lumi, &dest_buf_u8[dest_x], LV_OPA_MIX3(src_buf_al88[src_x].alpha,
                                                                                                      mask_buf[src_x], opa));
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                lv_color32_t src_argb;
                src_argb.red = src_argb.green = src_argb.blue = src_buf_al88[src_x].lumi;
                if(mask_buf == NULL) src_argb.alpha = LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa);
                else src_argb.alpha = LV_OPA_MIX3(src_buf_al88[src_x].alpha, mask_buf[dest_x], opa);
                blend_non_normal_pixel(&dest_buf_u8[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_u8 += dest_stride;
            src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_L8

static void LV_ATTRIBUTE_FAST_MEM l8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        dest_buf_u8[dest_x + 2] = src_buf_l8[src_x];
                        dest_buf_u8[dest_x + 1] = src_buf_l8[src_x];
                        dest_buf_u8[dest_x + 0] = src_buf_l8[src_x];
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_l8[src_x], &dest_buf_u8[dest_x], opa);
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_l8[src_x], &dest_buf_u8[dest_x], mask_buf[src_x]);
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_8_24_mix(src_buf_l8[src_x], &dest_buf_u8[dest_x], LV_OPA_MIX2(opa, mask_buf[src_x]));
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                src_argb.red = src_buf_l8[src_x];
                src_argb.green = src_buf_l8[src_x];
                src_argb.blue = src_buf_l8[src_x];
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);
                blend_non_normal_pixel(&dest_buf_u8[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_u8 += dest_stride;
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB565

static void LV_ATTRIBUTE_FAST_MEM rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16_t * src_buf_c16 = (const lv_color16_t *) dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t src_x;
    int32_t dest_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(src_x = 0, dest_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        dest_buf_u8[dest_x + 2] = (src_buf_c16[src_x].red * 2106) >> 8;  /*To make it rounded*/
                        dest_buf_u8[dest_x + 1] = (src_buf_c16[src_x].green * 1037) >> 8;
                        dest_buf_u8[dest_x + 0] = (src_buf_c16[src_x].blue * 2106) >> 8;
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size)) {
                uint8_t res[3];
                for(y = 0; y < h; y++) {
                    for(src_x = 0, dest_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        res[2] = (src_buf_c16[src_x].red * 2106) >> 8; /*To make it rounded*/
                        res[1] = (src_buf_c16[src_x].green * 1037) >> 8;
                        res[0] = (src_buf_c16[src_x].blue * 2106) >> 8;
                        lv_color_24_24_mix(res, &dest_buf_u8[dest_x], opa);
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size)) {
                uint8_t res[3];
                for(y = 0; y < h; y++) {
                    for(src_x = 0, dest_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        res[2] = (src_buf_c16[src_x].red * 2106) >> 8;  /*To make it rounded*/
                        res[1] = (src_buf_c16[src_x].green * 1037) >> 8;
                        res[0] = (src_buf_c16[src_x].blue * 2106) >> 8;
                        lv_color_24_24_mix(res, &dest_buf_u8[dest_x], mask_buf[src_x]);
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size)) {
                uint8_t res[3];
                for(y = 0; y < h; y++) {
                    for(src_x = 0, dest_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        res[2] = (src_buf_c16[src_x].red * 2106) >> 8;  /*To make it rounded*/
                        res[1] = (src_buf_c16[src_x].green * 1037) >> 8;
                        res[0] = (src_buf_c16[src_x].blue * 2106) >> 8;
                        lv_color_24_24_mix(res, &dest_buf_u8[dest_x], LV_OPA_MIX2(opa, mask_buf[src_x]));
                    }
                    dest_buf_u8 += dest_stride;
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(src_x = 0, dest_x = 0; src_x < w; src_x++, dest_x += dest_px_size) {
                src_argb.red = (src_buf_c16[src_x].red * 2106) >> 8;
                src_argb.green = (src_buf_c16[src_x].green * 1037) >> 8;
                src_argb.blue = (src_buf_c16[src_x].blue * 2106) >> 8;
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[src_x], opa);
                blend_non_normal_pixel(&dest_buf_u8[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_u8 += dest_stride;
            src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
        }
    }
}

#endif

static void LV_ATTRIBUTE_FAST_MEM rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, const uint8_t dest_px_size,
                                                     uint32_t src_px_size)
{
    int32_t w = dsc->dest_w * dest_px_size;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        /*Special case*/
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size, src_px_size)) {
                if(src_px_size == dest_px_size) {
                    for(y = 0; y < h; y++) {
                        lv_memcpy(dest_buf, src_buf, w);
                        dest_buf += dest_stride;
                        src_buf += src_stride;
                    }
                }
                else {
                    for(y = 0; y < h; y++) {
                        for(dest_x = 0, src_x = 0; dest_x < w; dest_x += dest_px_size, src_x += src_px_size) {
                            dest_buf[dest_x + 0] = src_buf[src_x + 0];
                            dest_buf[dest_x + 1] = src_buf[src_x + 1];
                            dest_buf[dest_x + 2] = src_buf[src_x + 2];
                        }
                        dest_buf += dest_stride;
                        src_buf += src_stride;
                    }
                }
            }
        }
        if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x += dest_px_size, src_x += src_px_size) {
                        lv_color_24_24_mix(&src_buf[src_x], &dest_buf[dest_x], opa);
                    }
                    dest_buf += dest_stride;
                    src_buf += src_stride;
                }
            }
        }
        if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size, src_px_size)) {
                uint32_t mask_x;
                for(y = 0; y < h; y++) {
                    for(mask_x = 0, dest_x = 0, src_x = 0; dest_x < w; mask_x++, dest_x += dest_px_size, src_x += src_px_size) {
                        lv_color_24_24_mix(&src_buf[src_x], &dest_buf[dest_x], mask_buf[mask_x]);
                    }
                    dest_buf += dest_stride;
                    src_buf += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
        if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size, src_px_size)) {
                uint32_t mask_x;
                for(y = 0; y < h; y++) {
                    for(mask_x = 0, dest_x = 0, src_x = 0; dest_x < w; mask_x++, dest_x += dest_px_size, src_x += src_px_size) {
                        lv_color_24_24_mix(&src_buf[src_x], &dest_buf[dest_x], LV_OPA_MIX2(opa, mask_buf[mask_x]));
                    }
                    dest_buf += dest_stride;
                    src_buf += src_stride;
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x += dest_px_size, src_x += src_px_size) {
                src_argb.red = src_buf[src_x + 2];
                src_argb.green = src_buf[src_x + 1];
                src_argb.blue = src_buf[src_x + 0];
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);

                blend_non_normal_pixel(&dest_buf[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf += dest_stride;
            src_buf += src_stride;
        }
    }
}

#if LV_DRAW_SW_SUPPORT_ARGB8888

static void LV_ATTRIBUTE_FAST_MEM argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color32_t * src_buf_c32 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_24_24_mix((const uint8_t *)&src_buf_c32[src_x], &dest_buf[dest_x], src_buf_c32[src_x].alpha);
                    }
                    dest_buf += dest_stride;
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_24_24_mix((const uint8_t *)&src_buf_c32[src_x], &dest_buf[dest_x], LV_OPA_MIX2(src_buf_c32[src_x].alpha, opa));
                    }
                    dest_buf += dest_stride;
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_24_24_mix((const uint8_t *)&src_buf_c32[src_x], &dest_buf[dest_x],
                                           LV_OPA_MIX2(src_buf_c32[src_x].alpha, mask_buf[src_x]));
                    }
                    dest_buf += dest_stride;
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x++) {
                        lv_color_24_24_mix((const uint8_t *)&src_buf_c32[src_x], &dest_buf[dest_x],
                                           LV_OPA_MIX3(src_buf_c32[src_x].alpha, mask_buf[src_x], opa));
                    }
                    dest_buf += dest_stride;
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x += dest_px_size, src_x ++) {
                src_argb = src_buf_c32[src_x];
                if(mask_buf == NULL) src_argb.alpha = LV_OPA_MIX2(src_argb.alpha, opa);
                else src_argb.alpha = LV_OPA_MIX3(src_argb.alpha, mask_buf[dest_x], opa);

                blend_non_normal_pixel(&dest_buf[dest_x], src_argb, dsc->blend_mode);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf += dest_stride;
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}

#endif

static inline void LV_ATTRIBUTE_FAST_MEM blend_non_normal_pixel(uint8_t * dest, lv_color32_t src, lv_blend_mode_t mode)
{
    uint8_t res[3] = {0, 0, 0};
    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            res[0] = LV_MIN(dest[0] + src.blue, 255);
            res[1] = LV_MIN(dest[1] + src.green, 255);
            res[2] = LV_MIN(dest[2] + src.red, 255);
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            res[0] = LV_MAX(dest[0] - src.blue, 0);
            res[1] = LV_MAX(dest[1] - src.green, 0);
            res[2] = LV_MAX(dest[2] - src.red, 0);
            break;
        case LV_BLEND_MODE_MULTIPLY:
            res[0] = (dest[0] * src.blue) >> 8;
            res[1] = (dest[1] * src.green) >> 8;
            res[2] = (dest[2] * src.red) >> 8;
            break;
        default:
            LV_LOG_WARN("Not supported blend mode: %d", mode);
            return;
    }
    lv_color_24_24_mix(res, dest, src.alpha);
}

static inline void LV_ATTRIBUTE_FAST_MEM lv_color_8_24_mix(const uint8_t src, uint8_t * dest, uint8_t mix)
{

    if(mix == 0) return;

    if(mix >= LV_OPA_MAX) {
        dest[0] = src;
        dest[1] = src;
        dest[2] = src;
    }
    else {
        lv_opa_t mix_inv = 255 - mix;
        dest[0] = (uint32_t)((uint32_t)src * mix + dest[0] * mix_inv) >> 8;
        dest[1] = (uint32_t)((uint32_t)src * mix + dest[1] * mix_inv) >> 8;
        dest[2] = (uint32_t)((uint32_t)src * mix + dest[2] * mix_inv) >> 8;
    }
}

static inline void LV_ATTRIBUTE_FAST_MEM lv_color_24_24_mix(const uint8_t * src, uint8_t * dest, uint8_t mix)
{

    if(mix == 0) return;

    if(mix >= LV_OPA_MAX) {
        dest[0] = src[0];
        dest[1] = src[1];
        dest[2] = src[2];
    }
    else {
        lv_opa_t mix_inv = 255 - mix;
        dest[0] = (uint32_t)((uint32_t)src[0] * mix + dest[0] * mix_inv) >> 8;
        dest[1] = (uint32_t)((uint32_t)src[1] * mix + dest[1] * mix_inv) >> 8;
        dest[2] = (uint32_t)((uint32_t)src[2] * mix + dest[2] * mix_inv) >> 8;
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

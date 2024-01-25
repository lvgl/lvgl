/**
 * @file lv_draw_sw_blend.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_argb8888.h"
#if LV_USE_DRAW_SW

#include "lv_draw_sw_blend.h"
#include "../../../misc/lv_math.h"
#include "../../../display/lv_display.h"
#include "../../../core/lv_refr.h"
#include "../../../misc/lv_color.h"
#include "../../../stdlib/lv_string.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON
    #include "neon/lv_blend_neon.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_blend_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_color32_t fg_saved;
    lv_color32_t bg_saved;
    lv_color32_t res_saved;
    lv_opa_t res_alpha_saved;
    lv_opa_t ratio_saved;
} lv_color_mix_alpha_cache_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void /* LV_ATTRIBUTE_FAST_MEM */ rgb565_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc);

static void /* LV_ATTRIBUTE_FAST_MEM */ rgb888_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                           const uint8_t src_px_size);

static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc);

static inline lv_color32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color_32_32_mix(lv_color32_t fg, lv_color32_t bg,
                                                                          lv_color_mix_alpha_cache_t * cache);

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ blend_non_normal_pixel(lv_color32_t * dest, lv_color32_t src,
                                                                      lv_blend_mode_t mode, lv_color_mix_alpha_cache_t * cache);
static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888
    #define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA(...)                LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA(...)            LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)        LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)        LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)     LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_argb8888(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    const lv_opa_t * mask = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;
    int32_t dest_stride = dsc->dest_stride;

    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

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
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888(dsc)) {
            uint32_t color32 = lv_color_to_u32(dsc->color);
            uint32_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w - 16; x += 16) {
                    dest_buf[x + 0] = color32;
                    dest_buf[x + 1] = color32;
                    dest_buf[x + 2] = color32;
                    dest_buf[x + 3] = color32;

                    dest_buf[x + 4] = color32;
                    dest_buf[x + 5] = color32;
                    dest_buf[x + 6] = color32;
                    dest_buf[x + 7] = color32;

                    dest_buf[x + 8] = color32;
                    dest_buf[x + 9] = color32;
                    dest_buf[x + 10] = color32;
                    dest_buf[x + 11] = color32;

                    dest_buf[x + 12] = color32;
                    dest_buf[x + 13] = color32;
                    dest_buf[x + 14] = color32;
                    dest_buf[x + 15] = color32;
                }
                for(; x < w; x ++) {
                    dest_buf[x] = color32;
                }

                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            }
        }

    }
    /*Opacity only*/
    else if(mask == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA(dsc)) {
            lv_color32_t color_argb = lv_color_to_32(dsc->color, opa);
            lv_color32_t * dest_buf = dsc->dest_buf;

            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    dest_buf[x] = lv_color_32_32_mix(color_argb, dest_buf[x], &cache);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            }
        }

    }
    /*Masked with full opacity*/
    else if(mask && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK(dsc)) {
            lv_color32_t color_argb = lv_color_to_32(dsc->color, 0xff);
            lv_color32_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    color_argb.alpha = mask[x];
                    dest_buf[x] = lv_color_32_32_mix(color_argb, dest_buf[x], &cache);
                }

                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                mask += mask_stride;
            }
        }

    }
    /*Masked with opacity*/
    else {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
            lv_color32_t color_argb = lv_color_to_32(dsc->color, opa);
            lv_color32_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    color_argb.alpha = LV_OPA_MIX2(mask[x], opa);
                    dest_buf[x] = lv_color_32_32_mix(color_argb, dest_buf[x], &cache);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                mask += mask_stride;
            }
        }
    }
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_argb8888(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    switch(dsc->src_color_format) {
        case LV_COLOR_FORMAT_RGB565:
            rgb565_image_blend(dsc);
            break;
        case LV_COLOR_FORMAT_RGB888:
            rgb888_image_blend(dsc, 3);
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            rgb888_image_blend(dsc, 4);
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc);
            break;
        default:
            LV_LOG_WARN("Not supported source color format");
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void LV_ATTRIBUTE_FAST_MEM rgb565_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16_t * src_buf_c16 = (const lv_color16_t *) dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color32_t color_argb;
    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    int32_t x;
    int32_t y;

    LV_UNUSED(color_argb);

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL) {
            lv_result_t accelerated;
            if(opa >= LV_OPA_MAX) {
                accelerated = LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888(dsc);
            }
            else {
                accelerated = LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc);
            }
            if(LV_RESULT_INVALID == accelerated) {
                color_argb.alpha = opa;
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb.red = (src_buf_c16[x].red * 2106) >> 8;  /*To make it rounded*/
                        color_argb.green = (src_buf_c16[x].green * 1037) >> 8;
                        color_argb.blue = (src_buf_c16[x].blue * 2106) >> 8;
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb.alpha = mask_buf[x];
                        color_argb.red = (src_buf_c16[x].red * 2106) >> 8;  /*To make it rounded*/
                        color_argb.green = (src_buf_c16[x].green * 1037) >> 8;
                        color_argb.blue = (src_buf_c16[x].blue * 2106) >> 8;
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                        color_argb.red = (src_buf_c16[x].red * 2106) >> 8;  /*To make it rounded*/
                        color_argb.green = (src_buf_c16[x].green * 1037) >> 8;
                        color_argb.blue = (src_buf_c16[x].blue * 2106) >> 8;
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                src_argb.red = (src_buf_c16[x].red * 2106) >> 8;
                src_argb.green = (src_buf_c16[x].green * 1037) >> 8;
                src_argb.blue = (src_buf_c16[x].blue * 2106) >> 8;
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(&dest_buf_c32[x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
        }
    }
}

static void LV_ATTRIBUTE_FAST_MEM rgb888_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc, const uint8_t src_px_size)
{

    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color32_t color_argb;
    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    LV_UNUSED(color_argb);

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        /*Special case*/
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888(dsc, src_px_size)) {
                if(src_px_size == 4) {
                    uint32_t line_in_bytes = w * 4;
                    for(y = 0; y < h; y++) {
                        lv_memcpy(dest_buf_c32, src_buf, line_in_bytes);
                        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                        src_buf = drawbuf_next_row(src_buf, src_stride);
                    }
                }
                else if(src_px_size == 3) {
                    for(y = 0; y < h; y++) {
                        for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += 3) {
                            dest_buf_c32[dest_x].red = src_buf[src_x + 2];
                            dest_buf_c32[dest_x].green = src_buf[src_x + 1];
                            dest_buf_c32[dest_x].blue = src_buf[src_x + 0];
                            dest_buf_c32[dest_x].alpha = 0xff;
                        }
                        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                        src_buf = drawbuf_next_row(src_buf, src_stride);
                    }
                }
            }

        }
        if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc, src_px_size)) {
                color_argb.alpha = opa;
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.red = src_buf[src_x + 2];
                        color_argb.green = src_buf[src_x + 1];
                        color_argb.blue = src_buf[src_x + 0];
                        dest_buf_c32[dest_x] = lv_color_32_32_mix(color_argb, dest_buf_c32[dest_x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf = drawbuf_next_row(src_buf, src_stride);
                }
            }

        }
        if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.alpha = mask_buf[dest_x];
                        color_argb.red = src_buf[src_x + 2];
                        color_argb.green = src_buf[src_x + 1];
                        color_argb.blue = src_buf[src_x + 0];
                        dest_buf_c32[dest_x] = lv_color_32_32_mix(color_argb, dest_buf_c32[dest_x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf = drawbuf_next_row(src_buf, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.alpha = (opa * mask_buf[dest_x]) >> 8;
                        color_argb.red = src_buf[src_x + 2];
                        color_argb.green = src_buf[src_x + 1];
                        color_argb.blue = src_buf[src_x + 0];
                        dest_buf_c32[dest_x] = lv_color_32_32_mix(color_argb, dest_buf_c32[dest_x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf = drawbuf_next_row(src_buf, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                src_argb.red = src_buf[src_x + 2];
                src_argb.green = src_buf[src_x + 1];
                src_argb.blue = src_buf[src_x + 0];
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);

                blend_non_normal_pixel(&dest_buf_c32[dest_x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
}

static void LV_ATTRIBUTE_FAST_MEM argb8888_image_blend(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color32_t * src_buf_c32 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color32_t color_argb;
    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    int32_t x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_c32[x] = lv_color_32_32_mix(src_buf_c32[x], dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, mask_buf[x]);
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, opa, mask_buf[x]);
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                color_argb = src_buf_c32[x];
                if(mask_buf == NULL) color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                else color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, mask_buf[x], opa);
                blend_non_normal_pixel(&dest_buf_c32[x], color_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}

static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color_32_32_mix(lv_color32_t fg, lv_color32_t bg,
                                                                    lv_color_mix_alpha_cache_t * cache)
{
    /*Pick the foreground if it's fully opaque or the Background is fully transparent*/
    if(fg.alpha >= LV_OPA_MAX || bg.alpha <= LV_OPA_MIN) {
        return fg;
    }
    /*Transparent foreground: use the Background*/
    else if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    /*Opaque background: use simple mix*/
    else if(bg.alpha == 255) {
        return lv_color_mix32(fg, bg);
    }
    /*Both colors have alpha. Expensive calculation need to be applied*/
    else {
        /*Save the parameters and the result. If they will be asked again don't compute again*/

        /*Update the ratio and the result alpha value if the input alpha values change*/
        if(bg.alpha != cache->bg_saved.alpha || fg.alpha != cache->fg_saved.alpha) {
            /*Info:
             * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
            cache->res_alpha_saved  = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha);
            LV_ASSERT(cache->ratio_saved != 0);
            cache->ratio_saved = (uint32_t)((uint32_t)fg.alpha * 255) / cache->res_alpha_saved;
        }

        if(!lv_color32_eq(bg, cache->bg_saved) || !lv_color32_eq(fg, cache->fg_saved)) {
            cache->fg_saved = fg;
            cache->bg_saved = bg;
            fg.alpha = cache->ratio_saved;
            cache->res_saved = lv_color_mix32(fg, bg);
            cache->res_saved.alpha = cache->res_alpha_saved;
        }

        return cache->res_saved;
    }
}

void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache)
{
    lv_memzero(&cache->fg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->bg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->res_saved, sizeof(lv_color32_t));
    cache->res_alpha_saved = 255;
    cache->ratio_saved = 255;
}

static inline void LV_ATTRIBUTE_FAST_MEM blend_non_normal_pixel(lv_color32_t * dest, lv_color32_t src,
                                                                lv_blend_mode_t mode, lv_color_mix_alpha_cache_t * cache)
{
    lv_color32_t res;
    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            res.red = LV_MIN(dest->red + src.red, 255);
            res.green = LV_MIN(dest->green + src.green, 255);
            res.blue = LV_MIN(dest->blue + src.blue, 255);
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            res.red = LV_MAX(dest->red - src.red, 0);
            res.green = LV_MAX(dest->green - src.green, 0);
            res.blue = LV_MAX(dest->blue - src.blue, 0);
            break;
        case LV_BLEND_MODE_MULTIPLY:
            res.red = (dest->red * src.red) >> 8;
            res.green = (dest->green * src.green) >> 8;
            res.blue = (dest->blue * src.blue) >> 8;
            break;
        default:
            LV_LOG_WARN("Not supported blend mode: %d", mode);
            return;
    }
    res.alpha = src.alpha;
    *dest = lv_color_32_32_mix(res, *dest, cache);
}

static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}

#endif

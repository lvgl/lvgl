/**
 * @file lv_draw_sw_blend_to_argb8888_premultiplied.c
 * @brief Implementation of ARGB8888 Premultiplied blending for LVGL.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_argb8888_premultiplied.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED

#include "lv_draw_sw_blend_private.h"
#include "../../../misc/lv_math.h"
#include "../../../display/lv_display.h"
#include "../../../core/lv_refr.h"
#include "../../../misc/lv_color.h"

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

#if LV_DRAW_SW_SUPPORT_ARGB8888
    static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888
static void /* LV_ATTRIBUTE_FAST_MEM */ rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc,
                                                           const uint8_t src_px_size);
#endif

static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_premultiplied_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ blend_non_normal_pixel_premultiplied(
    lv_color32_t * dest, lv_color32_t src, lv_blend_mode_t mode, lv_color_mix_alpha_cache_t * cache);

static inline lv_color32_t lv_color_32_32_mix_premul(lv_color32_t fg, lv_color32_t bg,
                                                     lv_color_mix_alpha_cache_t * cache);

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache);

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED
    #define   LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA
    #define   LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK
    #define   LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA
    #define   LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED
    #define   LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA
    #define   LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK
    #define   LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA
    #define   LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(...)             LV_RESULT_INVALID
#endif


#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED
    #define   LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA
    #define   LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK
    #define   LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(...)             LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA
    #define   LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(...)             LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * @brief Blend a solid color into an ARGB8888 premultiplied buffer.
 *
 * This function applies a solid color to the destination buffer with optional
 * opacity and masking. The input color is first converted to a premultiplied
 * alpha format before blending.
 *
 * @param dsc Blending descriptor containing destination buffer, color, and opacity
 */
void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_argb8888_premultiplied(lv_draw_sw_blend_fill_dsc_t * dsc)
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

    /* Convert source color to premultiplied */
    if(opa >= LV_OPA_MAX) opa = 0xff;
    lv_color32_t color_argb = lv_color_to_32(dsc->color, opa);
    lv_color32_t color_argb_premul;
    if(opa == 0xff) {
        color_argb_premul = color_argb;
    }
    else {
        color_argb_premul.alpha = opa;
        color_argb_premul.red   = (color_argb.red   * opa) >> 8;
        color_argb_premul.green = (color_argb.green * opa) >> 8;
        color_argb_premul.blue  = (color_argb.blue  * opa) >> 8;
    }

    /* Simple fill */
    if(mask == NULL && opa >= LV_OPA_MAX) {
        uint32_t color32 = lv_color_to_u32(dsc->color);
        uint32_t * dest_buf = dsc->dest_buf;

        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                dest_buf[x] = color32;
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Opacity only */
    else if(mask == NULL && opa < LV_OPA_MAX) {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                dest_buf[x] = lv_color_32_32_mix_premul(color_argb_premul, dest_buf[x], &cache);
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
        }
    }
    /* Masked fill */
    else if(mask && opa >= LV_OPA_MAX) {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                lv_color32_t color_premul = color_argb;
                if(mask[x] >= LV_OPA_MAX) {
                    dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
                }
                else if(mask[x] > LV_OPA_MIN) {
                    color_premul.alpha = mask[x];
                    color_premul.red   = (color_premul.red   * color_premul.alpha) >> 8;
                    color_premul.green = (color_premul.green * color_premul.alpha) >> 8;
                    color_premul.blue  = (color_premul.blue  * color_premul.alpha) >> 8;
                    dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
                }
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
    /* Masked with opacity */
    else {
        lv_color32_t * dest_buf = dsc->dest_buf;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                lv_color32_t color_premul = color_argb;
                lv_opa_t alpha = LV_OPA_MIX2(mask[x], opa);
                if(alpha >= LV_OPA_MAX) {
                    dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
                }
                else if(mask[x] > LV_OPA_MIN) {
                    color_premul.alpha = alpha;
                    color_premul.red   = (color_premul.red   * color_premul.alpha) >> 8;
                    color_premul.green = (color_premul.green * color_premul.alpha) >> 8;
                    color_premul.blue  = (color_premul.blue  * color_premul.alpha) >> 8;
                    dest_buf[x] = lv_color_32_32_mix_premul(color_premul, dest_buf[x], &cache);
                }
            }
            dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            mask += mask_stride;
        }
    }
}

/**
 * @brief Blend an image into an ARGB8888 premultiplied buffer.
 *
 * This function blends an image stored in ARGB8888 premultiplied format
 * into the destination buffer. It accounts for opacity and optional masking.
 *
 * @param dsc Blending descriptor containing source and destination buffer information
 */
void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_argb8888_premultiplied(lv_draw_sw_blend_image_dsc_t * dsc)
{
    switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_ARGB8888
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc);
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

        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            argb8888_premultiplied_image_blend(dsc);
            break;

        default:
            LV_LOG_WARN("Not supported source color format");
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if LV_DRAW_SW_SUPPORT_ARGB8888

static void LV_ATTRIBUTE_FAST_MEM argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
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

    int32_t x, y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        if(color_argb.alpha >= LV_OPA_MAX) {
                            color_argb.alpha = 0xff;
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(color_argb.alpha > LV_OPA_MIN) {
                            /*Premultiplication can cause loss of precision which can result slightly
                             *darker color when blending the same color to the background.*/
                            if(dest_buf_c32[x].red != color_argb.red ||
                               dest_buf_c32[x].green != color_argb.green ||
                               dest_buf_c32[x].blue != color_argb.blue ||
                               dest_buf_c32[x].alpha != color_argb.alpha) {

                                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;
                                dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                            }
                        }
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Apply global opacity */
                        lv_opa_t alpha = LV_OPA_MIX2(color_argb.alpha, opa);

                        if(alpha >= LV_OPA_MAX) {
                            color_argb.alpha = 0xff;
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(alpha > LV_OPA_MIN) {
                            /*Premultiplication can cause loss of precision which can result slightly
                             *darker color when blending the same color to the background.*/
                            if(dest_buf_c32[x].red != color_argb.red ||
                               dest_buf_c32[x].green != color_argb.green ||
                               dest_buf_c32[x].blue != color_argb.blue ||
                               dest_buf_c32[x].alpha != color_argb.alpha) {
                                color_argb.alpha = alpha;
                                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;
                                dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                            }
                        }
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Apply mask opacity */
                        lv_opa_t alpha = LV_OPA_MIX2(color_argb.alpha, mask_buf[x]);

                        if(alpha >= LV_OPA_MAX) {
                            color_argb.alpha = 0xff;
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(alpha > LV_OPA_MIN) {
                            /*Premultiplication can cause loss of precision which can result slightly
                             *darker color when blending the same color to the background.*/
                            if(dest_buf_c32[x].red != color_argb.red ||
                               dest_buf_c32[x].green != color_argb.green ||
                               dest_buf_c32[x].blue != color_argb.blue ||
                               dest_buf_c32[x].alpha != color_argb.alpha) {
                                color_argb.alpha = alpha;
                                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;
                                dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                            }
                        }
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Apply both mask and global opacity */
                        lv_opa_t alpha = LV_OPA_MIX3(color_argb.alpha, opa, mask_buf[x]);

                        if(alpha >= LV_OPA_MAX) {
                            color_argb.alpha = 0xff;
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(alpha > LV_OPA_MIN) {
                            /*Premultiplication can cause loss of precision which can result slightly
                             *darker color when blending the same color to the background.*/
                            if(dest_buf_c32[x].red != color_argb.red ||
                               dest_buf_c32[x].green != color_argb.green ||
                               dest_buf_c32[x].blue != color_argb.blue ||
                               dest_buf_c32[x].alpha != color_argb.alpha) {
                                color_argb.alpha = alpha;
                                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;
                                dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                            }
                        }
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

                /* Apply mask and/or opacity */
                if(mask_buf == NULL)
                    color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                else
                    color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, mask_buf[x], opa);

                /* Premultiply alpha */
                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;

                blend_non_normal_pixel_premultiplied(&dest_buf_c32[x], color_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

static void LV_ATTRIBUTE_FAST_MEM rgb888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc, const uint8_t src_px_size)
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(dsc, src_px_size)) {
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(dsc, src_px_size)) {
                color_argb.alpha = opa;
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.red = (src_buf[src_x + 2] * color_argb.alpha) >> 8;
                        color_argb.green = (src_buf[src_x + 1] * color_argb.alpha) >> 8;
                        color_argb.blue = (src_buf[src_x + 0] * color_argb.alpha) >> 8;
                        dest_buf_c32[dest_x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[dest_x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf = drawbuf_next_row(src_buf, src_stride);
                }
            }
        }
        if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.alpha = mask_buf[dest_x];
                        color_argb.red = (src_buf[src_x + 2] * color_argb.alpha) >> 8;
                        color_argb.green = (src_buf[src_x + 1] * color_argb.alpha) >> 8;
                        color_argb.blue = (src_buf[src_x + 0] * color_argb.alpha) >> 8;
                        dest_buf_c32[dest_x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[dest_x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf = drawbuf_next_row(src_buf, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(dsc, src_px_size)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; dest_x < w; dest_x++, src_x += src_px_size) {
                        color_argb.alpha = (opa * mask_buf[dest_x]) >> 8;
                        color_argb.red = (src_buf[src_x + 2] * color_argb.alpha) >> 8;
                        color_argb.green = (src_buf[src_x + 1] * color_argb.alpha) >> 8;
                        color_argb.blue = (src_buf[src_x + 0] * color_argb.alpha) >> 8;
                        dest_buf_c32[dest_x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[dest_x], &cache);
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

                blend_non_normal_pixel_premultiplied(&dest_buf_c32[dest_x], src_argb, dsc->blend_mode, &cache);
            }

            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf = drawbuf_next_row(src_buf, src_stride);
        }
    }
}

#endif


static void LV_ATTRIBUTE_FAST_MEM argb8888_premultiplied_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
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

    int32_t x, y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_c32[x] = lv_color_32_32_mix_premul(src_buf_c32[x], dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Unpremultiply the source color by using the reciprocal of the alpha */
                        if(color_argb.alpha != 0) {
                            uint16_t reciprocal_alpha = (255 * 256) / color_argb.alpha;
                            color_argb.red = (color_argb.red * reciprocal_alpha) >> 8;
                            color_argb.green = (color_argb.green * reciprocal_alpha) >> 8;
                            color_argb.blue = (color_argb.blue * reciprocal_alpha) >> 8;
                        }

                        /* Apply global opacity */
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);

                        /* Premultiply alpha */
                        color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                        color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                        color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;

                        dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Unpremultiply the source color by using the reciprocal of the alpha */
                        if(color_argb.alpha != 0) {
                            uint16_t reciprocal_alpha = (255 * 256) / color_argb.alpha;
                            color_argb.red = (color_argb.red * reciprocal_alpha) >> 8;
                            color_argb.green = (color_argb.green * reciprocal_alpha) >> 8;
                            color_argb.blue = (color_argb.blue * reciprocal_alpha) >> 8;
                        }
                        /* Adjust alpha using mask */
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, mask_buf[x]);

                        /* Premultiply alpha */
                        color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                        color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                        color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;

                        dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_PREMULTIPLIED_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb = src_buf_c32[x];

                        /* Unpremultiply the source color by using the reciprocal of the alpha */
                        if(color_argb.alpha != 0) {
                            uint16_t reciprocal_alpha = (255 * 256) / color_argb.alpha;
                            color_argb.red = (color_argb.red * reciprocal_alpha) >> 8;
                            color_argb.green = (color_argb.green * reciprocal_alpha) >> 8;
                            color_argb.blue = (color_argb.blue * reciprocal_alpha) >> 8;
                        }

                        /* Adjust alpha using both mask and opacity */
                        color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, opa, mask_buf[x]);

                        /* Premultiply alpha */
                        color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                        color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                        color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;

                        dest_buf_c32[x] = lv_color_32_32_mix_premul(color_argb, dest_buf_c32[x], &cache);
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

                /* Unpremultiply the source color by using the reciprocal of the alpha */
                if(color_argb.alpha != 0) {
                    uint16_t reciprocal_alpha = (255 * 256) / color_argb.alpha;
                    color_argb.red = (color_argb.red * reciprocal_alpha) >> 8;
                    color_argb.green = (color_argb.green * reciprocal_alpha) >> 8;
                    color_argb.blue = (color_argb.blue * reciprocal_alpha) >> 8;
                }

                /* Adjust alpha if needed */
                if(mask_buf == NULL)
                    color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                else
                    color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, mask_buf[x], opa);

                /* Premultiply alpha */
                color_argb.red   = (color_argb.red   * color_argb.alpha) >> 8;
                color_argb.green = (color_argb.green * color_argb.alpha) >> 8;
                color_argb.blue  = (color_argb.blue  * color_argb.alpha) >> 8;

                blend_non_normal_pixel_premultiplied(&dest_buf_c32[x], color_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}

/**
 * @brief Mix two ARGB8888 premultiplied colors.
 *
 * This function blends the foreground (`fg`) and background (`bg`) colors.
 * The foreground color is assumed to be premultiplied.
 *
 * @param fg Foreground color (premultiplied alpha)
 * @param bg Background color
 * @param cache Alpha blending cache for optimization
 * @return lv_color32_t Blended color result
 */
static inline lv_color32_t lv_color_32_32_mix_premul(lv_color32_t fg, lv_color32_t bg,
                                                     lv_color_mix_alpha_cache_t * cache)
{
    /*Pick the foreground if it's fully opaque or the background is fully transparent*/
    if(fg.alpha >= LV_OPA_MAX || bg.alpha <= LV_OPA_MIN) {
        return fg;
    }
    /* Transparent foreground: use the background */
    else if(fg.alpha <= LV_OPA_MIN) {
        return bg;
    }
    /* Opaque background: use simple mix */
    else if(bg.alpha == 255) {
        return lv_color_mix32_premultiplied(fg, bg);
    }
    else {
        /* Check cache to avoid redundant calculations */
        if(bg.alpha != cache->bg_saved.alpha || fg.alpha != cache->fg_saved.alpha) {
            /* Compute final alpha value */
            cache->res_alpha_saved = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha);
            LV_ASSERT(cache->res_alpha_saved != 0);

            /* Compute premultiplied blending ratio */
            cache->ratio_saved = (uint32_t)((uint32_t)fg.alpha * 255) / cache->res_alpha_saved;
        }

        /* Check if color blending is already cached */
        if(!lv_color32_eq(bg, cache->bg_saved) || !lv_color32_eq(fg, cache->fg_saved)) {
            cache->fg_saved = fg;
            cache->bg_saved = bg;

            /* Blend using premultiplied alpha */
            uint32_t inv_fg_alpha = 255 - fg.alpha;
            cache->res_saved.red   = fg.red   + ((bg.red   * inv_fg_alpha) >> 8);
            cache->res_saved.green = fg.green + ((bg.green * inv_fg_alpha) >> 8);
            cache->res_saved.blue  = fg.blue  + ((bg.blue  * inv_fg_alpha) >> 8);
            cache->res_saved.alpha = cache->res_alpha_saved;
        }

        return cache->res_saved;
    }
}

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache)
{
    lv_memzero(&cache->fg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->bg_saved, sizeof(lv_color32_t));
    lv_memzero(&cache->res_saved, sizeof(lv_color32_t));
    cache->res_alpha_saved = 255;
    cache->ratio_saved = 255;
}


static inline void * LV_ATTRIBUTE_FAST_MEM drawbuf_next_row(const void * buf, uint32_t stride)
{
    return (void *)((uint8_t *)buf + stride);
}


static inline void LV_ATTRIBUTE_FAST_MEM blend_non_normal_pixel_premultiplied(
    lv_color32_t * dest, lv_color32_t src, lv_blend_mode_t mode, lv_color_mix_alpha_cache_t * cache)
{
    lv_color32_t res;
    uint8_t src_alpha = src.alpha;  /* Premultiplied alpha of source */
    uint8_t dest_alpha = dest->alpha;

    switch(mode) {
        case LV_BLEND_MODE_ADDITIVE:
            /* Ensure RGB remains premultiplied */
            res.red   = LV_MIN(dest->red + src.red, dest_alpha);
            res.green = LV_MIN(dest->green + src.green, dest_alpha);
            res.blue  = LV_MIN(dest->blue + src.blue, dest_alpha);
            break;

        case LV_BLEND_MODE_SUBTRACTIVE:
            /* Ensure RGB remains non-negative and premultiplied */
            res.red   = LV_MAX(dest->red - src.red, 0);
            res.green = LV_MAX(dest->green - src.green, 0);
            res.blue  = LV_MAX(dest->blue - src.blue, 0);
            break;

        case LV_BLEND_MODE_MULTIPLY:
            /* Adjusted for premultiplied alpha: scale the result properly */
            res.red   = ((dest->red * src.red) / LV_MAX(src_alpha, 1));
            res.green = ((dest->green * src.green) / LV_MAX(src_alpha, 1));
            res.blue  = ((dest->blue * src.blue) / LV_MAX(src_alpha, 1));
            break;

        case LV_BLEND_MODE_DIFFERENCE:
            res.red   = LV_ABS(dest->red - src.red);
            res.green = LV_ABS(dest->green - src.green);
            res.blue  = LV_ABS(dest->blue - src.blue);
            break;

        default:
            LV_LOG_WARN("Not supported blend mode: %d", mode);
            return;
    }

    res.alpha = src_alpha;  /* Keep the alpha from premultiplied source */
    *dest = lv_color_32_32_mix_premul(res, *dest, cache);
}

#endif /* LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED */

#endif /* LV_USE_DRAW_SW */

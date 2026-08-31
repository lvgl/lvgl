/**
 * @file lv_draw_sw_blend_to_argb8888.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_argb8888.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_ARGB8888

#include "lv_draw_sw_blend_private.h"


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

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
    static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_premultiplied_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ lv_color_8_32_mix(const uint8_t src, lv_color32_t * dest, uint8_t mix);

static inline lv_color32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color_32_32_mix(lv_color32_t fg, lv_color32_t bg,
                                                                          lv_color_mix_alpha_cache_t * cache);

static inline lv_color32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color_32_32_mix_mid(lv_color32_t fg, lv_color32_t bg,
                                                                              lv_color_mix_alpha_cache_t * cache);

static void lv_color_mix_with_alpha_cache_init(lv_color_mix_alpha_cache_t * cache);

static inline void /* LV_ATTRIBUTE_FAST_MEM */ blend_non_normal_pixel(lv_color32_t * dest, lv_color32_t src,
                                                                      lv_blend_mode_t mode, lv_color_mix_alpha_cache_t * cache);

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    static inline lv_color16_t /* LV_ATTRIBUTE_FAST_MEM */ lv_color16_from_u16(uint16_t raw);
#endif

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

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888(...)                            LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)                  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888(...)                            LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)                   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)                  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)               LV_RESULT_INVALID
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

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
    #define LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

/**
 * One masked pixel: leave it alone, store the color, or mix.
 * A macro because -Os doesn't inline helpers, and this runs on every pixel.
 *
 * A transparent mask leaves the destination alone. lv_color_32_32_mix() would store the
 * color if the destination is transparent too, but such a pixel is invisible wherever it
 * ends up, so the extra test is not worth a branch here.
 */
#define ARGB8888_MASK_FILL_PX(dest_px, mask_val)                                \
    do {                                                                        \
        uint32_t a_ = (mask_val);                                               \
        if(a_ > LV_OPA_MIN) {                                                   \
            color_argb.alpha = (uint8_t)a_;                                     \
            if(a_ >= LV_OPA_MAX || (dest_px).alpha <= LV_OPA_MIN) {             \
                (dest_px) = color_argb;                                         \
            }                                                                   \
            else {                                                              \
                (dest_px) = lv_color_32_32_mix_mid(color_argb, dest_px, &cache);\
            }                                                                   \
        }                                                                       \
    } while(0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_argb8888(lv_draw_sw_blend_fill_dsc_t * dsc)
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
            /*Back to back rows are one long run, so the row setup and the tail are paid once*/
            if(dest_stride == w * 4) {
                w *= h;
                h = 1;
            }
            for(y = 0; y < h; y++) {
                for(x = 0; x < w - 15; x += 16) {
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
            /*Glyphs are a few pixels wide, where the alignment and tail handling of the
             *word based loop would cost more than it saves. Rounded corners and borders are
             *as wide as the widget and hold long runs of 0 and 255, where it pays off.*/
            if(w < 32) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        ARGB8888_MASK_FILL_PX(dest_buf[x], mask[x]);
                    }
                    dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                    mask += mask_stride;
                }
            }
            else {
                lv_color32_t color_opaque = color_argb;
                color_opaque.alpha = 0xff;
                for(y = 0; y < h; y++) {
                    /*Align the mask so that the word reads below are aligned too*/
                    for(x = 0; x < w && ((lv_uintptr_t)(mask + x) & 0x3); x++) {
                        ARGB8888_MASK_FILL_PX(dest_buf[x], mask[x]);
                    }
                    for(; x <= w - 4; x += 4) {
                        uint32_t mask32;
                        LV_LOAD_U32(mask32, mask + x);
                        if(mask32 == 0) continue;   /*Four transparent pixels*/
                        if(mask32 == 0xFFFFFFFF) {  /*Four opaque pixels*/
                            dest_buf[x + 0] = color_opaque;
                            dest_buf[x + 1] = color_opaque;
                            dest_buf[x + 2] = color_opaque;
                            dest_buf[x + 3] = color_opaque;
                            continue;
                        }
                        /*Read the bytes again rather than split `mask32`, so the byte
                         *order doesn't matter*/
                        ARGB8888_MASK_FILL_PX(dest_buf[x + 0], mask[x + 0]);
                        ARGB8888_MASK_FILL_PX(dest_buf[x + 1], mask[x + 1]);
                        ARGB8888_MASK_FILL_PX(dest_buf[x + 2], mask[x + 2]);
                        ARGB8888_MASK_FILL_PX(dest_buf[x + 3], mask[x + 3]);
                    }
                    for(; x < w; x++) {
                        ARGB8888_MASK_FILL_PX(dest_buf[x], mask[x]);
                    }
                    dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                    mask += mask_stride;
                }
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
                    ARGB8888_MASK_FILL_PX(dest_buf[x], LV_OPA_MIX2(mask[x], opa));
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                mask += mask_stride;
            }
        }
    }
}

#undef ARGB8888_MASK_FILL_PX

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_argb8888(lv_draw_sw_blend_image_dsc_t * dsc)
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
#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            argb8888_premultiplied_image_blend(dsc);
            break;
#endif
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc);
            break;
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
            LV_LOG_WARN("Not supported source color format 0x%02X", dsc->src_color_format);
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
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        dest_buf_c32[dest_x].alpha = chan_val;
                        dest_buf_c32[dest_x].red = chan_val;
                        dest_buf_c32[dest_x].green = chan_val;
                        dest_buf_c32[dest_x].blue = chan_val;
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_32_mix(chan_val, &dest_buf_c32[dest_x], opa);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_32_mix(chan_val, &dest_buf_c32[dest_x], mask_buf[src_x]);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        uint8_t chan_val = get_bit(src_buf_i1, src_x) * 255;
                        lv_color_8_32_mix(chan_val, &dest_buf_c32[dest_x], LV_OPA_MIX2(mask_buf[src_x], opa));
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        lv_color_mix_alpha_cache_t cache;
        lv_color_mix_with_alpha_cache_init(&cache);
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                src_argb.red = get_bit(src_buf_i1, src_x) * 255;
                src_argb.green = src_argb.red;
                src_argb.blue = src_argb.red;
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);
                blend_non_normal_pixel(&dest_buf_c32[dest_x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
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
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        /*
                        dest_buf_c32[dest_x].alpha = src_buf_al88[src_x].alpha;
                        dest_buf_c32[dest_x].red = src_buf_al88[src_x].lumi;
                        dest_buf_c32[dest_x].green = src_buf_al88[src_x].lumi;
                        dest_buf_c32[dest_x].blue = src_buf_al88[src_x].lumi;
                        */
                        lv_color_8_32_mix(src_buf_al88[src_x].lumi, &dest_buf_c32[dest_x], src_buf_al88[src_x].alpha);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_al88[src_x].lumi, &dest_buf_c32[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa));
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_al88[src_x].lumi, &dest_buf_c32[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha,
                                                                                                       mask_buf[src_x]));
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_al88[src_x].lumi, &dest_buf_c32[dest_x], LV_OPA_MIX3(src_buf_al88[src_x].alpha,
                                                                                                       mask_buf[src_x], opa));
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        lv_color_mix_alpha_cache_t cache;
        lv_color_mix_with_alpha_cache_init(&cache);
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                src_argb.red = src_buf_al88[src_x].lumi;
                src_argb.green = src_buf_al88[src_x].lumi;
                src_argb.blue = src_buf_al88[src_x].lumi;
                if(mask_buf == NULL) src_argb.alpha = LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa);
                else src_argb.alpha = LV_OPA_MIX3(src_buf_al88[src_x].alpha, mask_buf[dest_x], opa);
                blend_non_normal_pixel(&dest_buf_c32[dest_x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
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
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
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
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        dest_buf_c32[dest_x].alpha = 0xff;
                        dest_buf_c32[dest_x].red = src_buf_l8[src_x];
                        dest_buf_c32[dest_x].green = src_buf_l8[src_x];
                        dest_buf_c32[dest_x].blue = src_buf_l8[src_x];
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_l8[src_x], &dest_buf_c32[dest_x], opa);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_l8[src_x], &dest_buf_c32[dest_x], mask_buf[src_x]);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_L8_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                        lv_color_8_32_mix(src_buf_l8[src_x], &dest_buf_c32[dest_x], LV_OPA_MIX2(mask_buf[src_x], opa));
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        lv_color_mix_alpha_cache_t cache;
        lv_color_mix_with_alpha_cache_init(&cache);
        for(y = 0; y < h; y++) {
            for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                src_argb.red = src_buf_l8[src_x];
                src_argb.green = src_buf_l8[src_x];
                src_argb.blue = src_buf_l8[src_x];
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[dest_x], opa);
                blend_non_normal_pixel(&dest_buf_c32[dest_x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_l8 = drawbuf_next_row(src_buf_l8, src_stride);
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB565

/**
 * One masked pixel of an RGB565 source. The color conversion is only worth doing once the
 * pixel is known to be visible.
 */
#define RGB565_TO_ARGB8888_MASK_PX(dest_px, src_px, mask_val)                       \
    do {                                                                            \
        uint32_t a_ = (mask_val);                                                   \
        if(a_ > LV_OPA_MIN) {                                                       \
            color_argb.alpha = (uint8_t)a_;                                         \
            color_argb.red = ((src_px).red * 2106) >> 8;   /*To make it rounded*/   \
            color_argb.green = ((src_px).green * 1037) >> 8;                        \
            color_argb.blue = ((src_px).blue * 2106) >> 8;                          \
            if(a_ >= LV_OPA_MAX || (dest_px).alpha <= LV_OPA_MIN) {                 \
                (dest_px) = color_argb;                                             \
            }                                                                       \
            else {                                                                  \
                (dest_px) = lv_color_32_32_mix_mid(color_argb, dest_px, &cache);    \
            }                                                                       \
        }                                                                           \
    } while(0)

/**
 * The masked rows of an RGB565 source. Kept out of rgb565_image_blend() on purpose: inlined
 * there it pushes that function over the compiler's inline budget, and the unmasked loops
 * beside it lose their own inlining, which costs far more than this path saves.
 */
static void LV_ATTRIBUTE_FAST_MEM rgb565_masked_rows(lv_color32_t * dest_buf_c32, int32_t dest_stride,
                                                     const lv_color16_t * src_buf_c16, int32_t src_stride,
                                                     const lv_opa_t * mask_buf, int32_t mask_stride,
                                                     int32_t w, int32_t h,
                                                     lv_color_mix_alpha_cache_t * cache_p)
{
    lv_color32_t color_argb;
    lv_color_mix_alpha_cache_t cache = *cache_p;
    int32_t x;
    int32_t y;

    color_argb.alpha = 0xff;
    for(y = 0; y < h; y++) {
        /*The mask is the image's alpha plane, mostly 0 or 255. Wide rows test four mask bytes
         *with one word read; the four are spelled out because a `for(i = 0; i < 4; i++)` here
         *costs more than it saves.*/
        x = 0;
        if(w >= 32) {
            for(; x < w && ((lv_uintptr_t)(mask_buf + x) & 0x3); x++) {
                RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x], src_buf_c16[x], mask_buf[x]);
            }
            for(; x <= w - 4; x += 4) {
                uint32_t m32;
                LV_LOAD_U32(m32, mask_buf + x);
                if(m32 == 0) continue;
                RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x + 0], src_buf_c16[x + 0], mask_buf[x + 0]);
                RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x + 1], src_buf_c16[x + 1], mask_buf[x + 1]);
                RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x + 2], src_buf_c16[x + 2], mask_buf[x + 2]);
                RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x + 3], src_buf_c16[x + 3], mask_buf[x + 3]);
            }
        }
        for(; x < w; x++) {
            RGB565_TO_ARGB8888_MASK_PX(dest_buf_c32[x], src_buf_c16[x], mask_buf[x]);
        }
        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
        src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
        mask_buf += mask_stride;
    }
    *cache_p = cache;
}

static void LV_ATTRIBUTE_FAST_MEM rgb565_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
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
                if(opa >= LV_OPA_MAX) {
                    /*An opaque foreground always wins, whatever the background is, so this is
                     *a plain convert and store. Keeping the mix out of the loop also keeps the
                     *loop free of calls, which is what lets the compiler vectorize it.*/
                    for(y = 0; y < h; y++) {
                        for(x = 0; x < w; x++) {
                            color_argb.red = (src_buf_c16[x].red * 2106) >> 8;  /*To make it rounded*/
                            color_argb.green = (src_buf_c16[x].green * 1037) >> 8;
                            color_argb.blue = (src_buf_c16[x].blue * 2106) >> 8;
                            dest_buf_c32[x] = color_argb;
                        }
                        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                        src_buf_c16 = drawbuf_next_row(src_buf_c16, src_stride);
                    }
                }
                else {
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
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                rgb565_masked_rows(dest_buf_c32, dest_stride, src_buf_c16, src_stride,
                                   mask_buf, mask_stride, w, h, &cache);
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

#endif

#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED

static void LV_ATTRIBUTE_FAST_MEM rgb565_swapped_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    lv_color32_t * dest_buf_c32 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint16_t * src_buf_u16 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    lv_color32_t color_argb;
    lv_color_mix_alpha_cache_t cache;
    lv_color_mix_with_alpha_cache_init(&cache);

    int32_t x;
    int32_t y;

    LV_UNUSED(color_argb);

    uint16_t raw;
    lv_color16_t px;

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
                if(opa >= LV_OPA_MAX) {
                    /*An opaque foreground always wins, whatever the background is, so this is
                     *a plain convert and store. Keeping the mix out of the loop also keeps the
                     *loop free of calls, which is what lets the compiler vectorize it.*/
                    for(y = 0; y < h; y++) {
                        for(x = 0; x < w; x++) {
                            raw = lv_color_swap_16(src_buf_u16[x]);                        /* swap byte order */
                            px = lv_color16_from_u16(raw);
                            color_argb.red = (px.red * 2106) >> 8;  /*To make it rounded*/
                            color_argb.green = (px.green * 1037) >> 8;
                            color_argb.blue = (px.blue * 2106) >> 8;
                            dest_buf_c32[x] = color_argb;
                        }
                        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                        src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    }
                }
                else {
                    for(y = 0; y < h; y++) {
                        for(x = 0; x < w; x++) {
                            raw = lv_color_swap_16(src_buf_u16[x]);                        /* swap byte order */
                            px = lv_color16_from_u16(raw);
                            color_argb.red = (px.red * 2106) >> 8;  /*To make it rounded*/
                            color_argb.green = (px.green * 1037) >> 8;
                            color_argb.blue = (px.blue * 2106) >> 8;
                            dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                        }
                        dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                        src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    }
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb.alpha = mask_buf[x];
                        raw = lv_color_swap_16(src_buf_u16[x]);                        /* swap byte order */
                        px = lv_color16_from_u16(raw);
                        color_argb.red = (px.red * 2106) >> 8;  /*To make it rounded*/
                        color_argb.green = (px.green * 1037) >> 8;
                        color_argb.blue = (px.blue * 2106) >> 8;
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else {
            if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        color_argb.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                        raw = lv_color_swap_16(src_buf_u16[x]);                        /* swap byte order */
                        px = lv_color16_from_u16(raw);
                        color_argb.red = (px.red * 2106) >> 8;  /*To make it rounded*/
                        color_argb.green = (px.green * 1037) >> 8;
                        color_argb.blue = (px.blue * 2106) >> 8;
                        dest_buf_c32[x] = lv_color_32_32_mix(color_argb, dest_buf_c32[x], &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
    }
    else {
        lv_color32_t src_argb;
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                raw = lv_color_swap_16(src_buf_u16[x]);                        /* swap byte order */
                px = lv_color16_from_u16(raw);
                src_argb.red = (px.red * 2106) >> 8;
                src_argb.green = (px.green * 1037) >> 8;
                src_argb.blue = (px.blue * 2106) >> 8;
                if(mask_buf == NULL) src_argb.alpha = opa;
                else src_argb.alpha = LV_OPA_MIX2(mask_buf[x], opa);
                blend_non_normal_pixel(&dest_buf_c32[x], src_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_u16 = drawbuf_next_row(src_buf_u16, src_stride);
        }
    }
}

#endif

#if LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

#undef RGB565_TO_ARGB8888_MASK_PX

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

#endif

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

    int32_t x;
    int32_t y;

    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        /*An opaque source or an empty destination is a plain store and a
                         *transparent source changes nothing. A fresh layer is all empty,
                         *so this covers nearly every pixel.*/
                        color_argb = src_buf_c32[x];
                        if(color_argb.alpha >= LV_OPA_MAX || dest_buf_c32[x].alpha <= LV_OPA_MIN) {
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(color_argb.alpha > LV_OPA_MIN) {
                            dest_buf_c32[x] = lv_color_32_32_mix_mid(color_argb, dest_buf_c32[x], &cache);
                        }
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
                        /*An opaque source or an empty destination is a plain store and a
                         *transparent source changes nothing. A fresh layer is all empty,
                         *so this covers nearly every pixel.*/
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                        if(color_argb.alpha >= LV_OPA_MAX || dest_buf_c32[x].alpha <= LV_OPA_MIN) {
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(color_argb.alpha > LV_OPA_MIN) {
                            dest_buf_c32[x] = lv_color_32_32_mix_mid(color_argb, dest_buf_c32[x], &cache);
                        }
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
                        /*An opaque source or an empty destination is a plain store and a
                         *transparent source changes nothing. A fresh layer is all empty,
                         *so this covers nearly every pixel.*/
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, mask_buf[x]);
                        if(color_argb.alpha >= LV_OPA_MAX || dest_buf_c32[x].alpha <= LV_OPA_MIN) {
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(color_argb.alpha > LV_OPA_MIN) {
                            dest_buf_c32[x] = lv_color_32_32_mix_mid(color_argb, dest_buf_c32[x], &cache);
                        }
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
                        /*An opaque source or an empty destination is a plain store and a
                         *transparent source changes nothing. A fresh layer is all empty,
                         *so this covers nearly every pixel.*/
                        color_argb = src_buf_c32[x];
                        color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, opa, mask_buf[x]);
                        if(color_argb.alpha >= LV_OPA_MAX || dest_buf_c32[x].alpha <= LV_OPA_MIN) {
                            dest_buf_c32[x] = color_argb;
                        }
                        else if(color_argb.alpha > LV_OPA_MIN) {
                            dest_buf_c32[x] = lv_color_32_32_mix_mid(color_argb, dest_buf_c32[x], &cache);
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

#if LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED

/**
 * Blend a premultiplied foreground pixel to a straight alpha background.
 * `scale` is the extra opacity for the channels, `ae` the matching effective alpha. Passing
 * `ae` in lets the caller combine opa and mask with a single rounding step.
 * An opaque background, the common case, needs no unpremultiplication and so no division.
 */
static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color_32_32_mix_premult_scaled(lv_color32_t fg_premult,
                                                                                   lv_color32_t bg, uint8_t scale,
                                                                                   uint8_t ae,
                                                                                   lv_color_mix_alpha_cache_t * cache)
{
    if(ae <= LV_OPA_MIN) return bg;

    if(bg.alpha >= LV_OPA_MAX) {
        /*The premultiplied channels can be added as they are,
         *the background is weighted with the remaining alpha*/
        uint32_t ae_inv = 255 - ae;
        if(scale == 255) {
            bg.red = (uint8_t)(fg_premult.red + (((uint32_t)bg.red * ae_inv) >> 8));
            bg.green = (uint8_t)(fg_premult.green + (((uint32_t)bg.green * ae_inv) >> 8));
            bg.blue = (uint8_t)(fg_premult.blue + (((uint32_t)bg.blue * ae_inv) >> 8));
        }
        else {
            bg.red = (uint8_t)(LV_OPA_MIX2(fg_premult.red, scale) + (((uint32_t)bg.red * ae_inv) >> 8));
            bg.green = (uint8_t)(LV_OPA_MIX2(fg_premult.green, scale) + (((uint32_t)bg.green * ae_inv) >> 8));
            bg.blue = (uint8_t)(LV_OPA_MIX2(fg_premult.blue, scale) + (((uint32_t)bg.blue * ae_inv) >> 8));
        }
        return bg; /*The alpha of the background is kept*/
    }

    /*Semi-transparent background: unpremultiply and use the generic mix (rare case).
     *`fg_premult.alpha` can't be 0 here, the effective alpha was checked above.
     *For a fully opaque foreground the reciprocal is 256, so it's a no-op.*/
    uint16_t reciprocal = (255 * 256) / fg_premult.alpha;
    fg_premult.red = (fg_premult.red * reciprocal) >> 8;
    fg_premult.green = (fg_premult.green * reciprocal) >> 8;
    fg_premult.blue = (fg_premult.blue * reciprocal) >> 8;
    fg_premult.alpha = ae;
    return lv_color_32_32_mix(fg_premult, bg, cache);
}

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

    int32_t x;
    int32_t y;

    /* Process the normal blend mode (for premultiplied alpha) */
    if(dsc->blend_mode == LV_BLEND_MODE_NORMAL) {
        if(mask_buf == NULL && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        if(src_buf_c32[x].alpha != 0) {
                            dest_buf_c32[x] = lv_color_32_32_mix_premult_scaled(src_buf_c32[x], dest_buf_c32[x], 255,
                                                                                src_buf_c32[x].alpha, &cache);
                        }
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf == NULL && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_c32[x] = lv_color_32_32_mix_premult_scaled(src_buf_c32[x], dest_buf_c32[x], opa,
                                                                            LV_OPA_MIX2(src_buf_c32[x].alpha, opa), &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                }
            }
        }
        else if(mask_buf && opa >= LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_c32[x] = lv_color_32_32_mix_premult_scaled(src_buf_c32[x], dest_buf_c32[x], mask_buf[x],
                                                                            LV_OPA_MIX2(src_buf_c32[x].alpha, mask_buf[x]), &cache);
                    }
                    dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
                    src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                    mask_buf += mask_stride;
                }
            }
        }
        else if(mask_buf && opa < LV_OPA_MAX) {
            if(LV_RESULT_INVALID == LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)) {
                for(y = 0; y < h; y++) {
                    for(x = 0; x < w; x++) {
                        dest_buf_c32[x] = lv_color_32_32_mix_premult_scaled(src_buf_c32[x], dest_buf_c32[x],
                                                                            LV_OPA_MIX2(opa, mask_buf[x]),
                                                                            LV_OPA_MIX3(src_buf_c32[x].alpha, mask_buf[x], opa),
                                                                            &cache);
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
                /* Unpremultiply the source color by using the reciprocal of the alpha */
                color_argb = src_buf_c32[x];
                if(color_argb.alpha != 0) {
                    uint16_t reciprocal_alpha = (255 * 256) / color_argb.alpha;
                    color_argb.red = (color_argb.red * reciprocal_alpha) >> 8;
                    color_argb.green = (color_argb.green * reciprocal_alpha) >> 8;
                    color_argb.blue = (color_argb.blue * reciprocal_alpha) >> 8;
                }
                if(mask_buf == NULL) {
                    color_argb.alpha = LV_OPA_MIX2(color_argb.alpha, opa);
                }
                else {
                    color_argb.alpha = LV_OPA_MIX3(color_argb.alpha, mask_buf[x], opa);
                }

                /* Blend with destination using non-normal blend mode */
                blend_non_normal_pixel(&dest_buf_c32[x], color_argb, dsc->blend_mode, &cache);
            }
            if(mask_buf) mask_buf += mask_stride;
            dest_buf_c32 = drawbuf_next_row(dest_buf_c32, dest_stride);
            src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
        }
    }
}

#endif

static inline void LV_ATTRIBUTE_FAST_MEM lv_color_8_32_mix(const uint8_t src, lv_color32_t * dest, uint8_t mix)
{

    if(mix == 0) return;

    dest->alpha = 255;
    if(mix >= LV_OPA_MAX) {
        dest->red = src;
        dest->green = src;
        dest->blue = src;
    }
    else {
        lv_opa_t mix_inv = 255 - mix;
        dest->red = (uint32_t)((uint32_t)src * mix + dest->red * mix_inv) >> 8;
        dest->green = (uint32_t)((uint32_t)src * mix + dest->green * mix_inv) >> 8;
        dest->blue = (uint32_t)((uint32_t)src * mix + dest->blue * mix_inv) >> 8;
    }
}

/**
 * The alpha compositing case of lv_color_32_32_mix(), for a background that is partly
 * transparent. Rare and bulky, so it stays out of line on purpose: that is what keeps
 * the mix itself small enough for the compiler to inline it into the blend loops.
 */
static LV_ATTRIBUTE_FAST_MEM lv_color32_t lv_color_32_32_mix_semi_transparent_bg(lv_color32_t fg, lv_color32_t bg,
                                                                                 lv_color_mix_alpha_cache_t * cache)
{
    /*Save the parameters and the result. If they will be asked again don't compute again*/

    /*Update the ratio and the result alpha value if the input alpha values change*/
    if(bg.alpha != cache->bg_saved.alpha || fg.alpha != cache->fg_saved.alpha) {
        /*Info:
         * https://en.wikipedia.org/wiki/Alpha_compositing#Analytical_derivation_of_the_over_operator*/
        cache->res_alpha_saved = 255 - LV_OPA_MIX2(255 - fg.alpha, 255 - bg.alpha);
        LV_ASSERT(cache->res_alpha_saved != 0);
        cache->ratio_saved = (uint32_t)((uint32_t)fg.alpha * 255) / cache->res_alpha_saved;
    }

    if(!lv_color32_eq(bg, cache->bg_saved) || !lv_color32_eq(fg, cache->fg_saved)) {
        cache->fg_saved = fg;
        cache->bg_saved = bg;
        fg.alpha = cache->ratio_saved;
        cache->res_saved = lv_color_mix32_inlined(fg, bg);
        cache->res_saved.alpha = cache->res_alpha_saved;
    }

    return cache->res_saved;
}

/**
 * lv_color_32_32_mix() without its two leading shortcuts, for the callers that already
 * checked them. Repeating the tests here would only add instructions once inlined.
 */
static inline lv_color32_t LV_ATTRIBUTE_FAST_MEM lv_color_32_32_mix_mid(lv_color32_t fg, lv_color32_t bg,
                                                                        lv_color_mix_alpha_cache_t * cache)
{
    /*Opaque background: use simple mix*/
    if(bg.alpha >= LV_OPA_MAX) {
        return lv_color_mix32_inlined(fg, bg);
    }

    /*Both colors have alpha: the expensive case, kept out of line so that the callers of
     *lv_color_32_32_mix() stay small enough for the compiler to inline them*/
    return lv_color_32_32_mix_semi_transparent_bg(fg, bg, cache);
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
    else if(bg.alpha >= LV_OPA_MAX) {
        return lv_color_mix32_inlined(fg, bg);
    }
    /*Both colors have alpha: the expensive case, kept out of line so that this function
     *stays small enough for the compiler to inline it into the blend loops*/
    else {
        return lv_color_32_32_mix_semi_transparent_bg(fg, bg, cache);
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

#if LV_DRAW_SW_SUPPORT_I1

static inline uint8_t LV_ATTRIBUTE_FAST_MEM get_bit(const uint8_t * buf, int32_t bit_idx)
{
    return (buf[bit_idx / 8] >> (7 - (bit_idx % 8))) & 1;
}

#endif

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
        case LV_BLEND_MODE_DIFFERENCE:
            res.red = LV_ABS(dest->red - src.red);
            res.green = LV_ABS(dest->green - src.green);
            res.blue = LV_ABS(dest->blue - src.blue);
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

#endif /* LV_DRAW_SW_SUPPORT_ARGB8888 */

#endif /* LV_USE_DRAW_SW */

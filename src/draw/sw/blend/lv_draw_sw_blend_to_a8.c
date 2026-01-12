/**
 * @file lv_draw_sw_blend_to_a8.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_blend_to_a8.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_SUPPORT_A8

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

static void /* LV_ATTRIBUTE_FAST_MEM */ a8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

#if LV_DRAW_SW_SUPPORT_L8 || LV_DRAW_SW_SUPPORT_RGB565 || LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

    static void LV_ATTRIBUTE_FAST_MEM mask_only_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

#endif

#if LV_DRAW_SW_SUPPORT_I1
    static void /* LV_ATTRIBUTE_FAST_MEM */ i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);

    static inline uint8_t /* LV_ATTRIBUTE_FAST_MEM */ get_bit(const uint8_t * buf, int32_t bit_idx);
#endif

#if LV_DRAW_SW_SUPPORT_AL88
    static void /* LV_ATTRIBUTE_FAST_MEM */ al88_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

#if LV_DRAW_SW_SUPPORT_ARGB8888
    static void /* LV_ATTRIBUTE_FAST_MEM */ argb8888_image_blend(lv_draw_sw_blend_image_dsc_t * dsc);
#endif

static inline void * /* LV_ATTRIBUTE_FAST_MEM */ drawbuf_next_row(const void * buf, uint32_t stride);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define BLEND_A8(dest_a, src_a) if(dest_a < src_a) dest_a = src_a

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_A8
    #define LV_DRAW_SW_COLOR_BLEND_TO_A8(...)                         LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_OPA(...)                LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_MASK(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_COLOR_BLEND_TO_A8_MIX_MASK_OPA(...)            LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8
    #define LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8(...)                 LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_OPA(...)        LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_MASK(...)       LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_MIX_MASK_OPA(...)    LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_A8_BLEND_TO_A8
    #define LV_DRAW_SW_A8_BLEND_TO_A8(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_A8_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_A8_BLEND_TO_A8_WITH_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_A8_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_A8_BLEND_TO_A8_WITH_MASK(...)     LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_A8_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_A8_BLEND_TO_A8_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_TO_A8
    #define LV_DRAW_SW_AL88_BLEND_TO_A8(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_MASK(...)     LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_AL88_BLEND_TO_A8_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_TO_A8
    #define LV_DRAW_SW_ARGB8888_BLEND_TO_A8(...)               LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_TO_A8_WITH_OPA(...)      LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_ARGB8888_BLEND_TO_A8_WITH_MASK(...)     LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_ARGB8888_BLEND_TO_A8_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif



#ifndef LV_DRAW_SW_I1_BLEND_TO_A8
    #define LV_DRAW_SW_I1_BLEND_TO_A8(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_TO_A8_WITH_OPA
    #define LV_DRAW_SW_I1_BLEND_TO_A8_WITH_OPA(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_TO_A8_WITH_MASK
    #define LV_DRAW_SW_I1_BLEND_TO_A8_WITH_MASK(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_I1_BLEND_TO_A8_MIX_MASK_OPA
    #define LV_DRAW_SW_I1_BLEND_TO_A8_MIX_MASK_OPA(...)  LV_RESULT_INVALID
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_color_to_a8(lv_draw_sw_blend_fill_dsc_t * dsc)
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
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_A8(dsc)) {
            uint8_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                lv_memset(dest_buf, 0xff, w);
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            }
        }
    }
    /*Opacity only*/
    else if(mask == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_OPA(dsc)) {
            uint8_t * dest_buf = dsc->dest_buf;

            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf[x], opa);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
            }
        }

    }
    /*Masked with full opacity*/
    else if(mask && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_A8_WITH_MASK(dsc)) {
            uint8_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf[x], mask[x]);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                mask += mask_stride;
            }
        }

    }
    /*Masked with opacity*/
    else {
        if(LV_RESULT_INVALID == LV_DRAW_SW_COLOR_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            uint8_t * dest_buf = dsc->dest_buf;
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    lv_opa_t opa_res = LV_OPA_MIX2(mask[x], opa);
                    BLEND_A8(dest_buf[x], opa_res);
                }
                dest_buf = drawbuf_next_row(dest_buf, dest_stride);
                mask += mask_stride;
            }
        }
    }
}

void LV_ATTRIBUTE_FAST_MEM lv_draw_sw_blend_image_to_a8(lv_draw_sw_blend_image_dsc_t * dsc)
{
    switch(dsc->src_color_format) {
#if LV_DRAW_SW_SUPPORT_RGB565
        case LV_COLOR_FORMAT_RGB565:
            mask_only_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        case LV_COLOR_FORMAT_RGB565_SWAPPED:
            mask_only_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_RGB888
        case LV_COLOR_FORMAT_RGB888:
            mask_only_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_XRGB8888
        case LV_COLOR_FORMAT_XRGB8888:
            mask_only_image_blend(dsc);
            break;
#endif
#if LV_DRAW_SW_SUPPORT_ARGB8888
        case LV_COLOR_FORMAT_ARGB8888:
            argb8888_image_blend(dsc);
            break;
#endif
        case LV_COLOR_FORMAT_A8:
            a8_image_blend(dsc);
            break;
#if LV_DRAW_SW_SUPPORT_L8
        case LV_COLOR_FORMAT_L8:
            mask_only_image_blend(dsc);
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

static void LV_ATTRIBUTE_FAST_MEM a8_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_a8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_a8 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(mask_buf == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], src_buf_a8[src_x]);
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_a8 = drawbuf_next_row(src_buf_a8, src_stride);
            }
        }
    }
    else if(mask_buf == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_WITH_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX2(opa, src_buf_a8[src_x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_a8 = drawbuf_next_row(src_buf_a8, src_stride);
            }
        }
    }
    else if(mask_buf && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX2(mask_buf[src_x], src_buf_a8[src_x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_a8 = drawbuf_next_row(src_buf_a8, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else if(mask_buf && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX3(opa, mask_buf[src_x], src_buf_a8[src_x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_a8 = drawbuf_next_row(src_buf_a8, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
}

#if LV_DRAW_SW_SUPPORT_L8 || LV_DRAW_SW_SUPPORT_RGB565 || LV_DRAW_SW_SUPPORT_RGB888 || LV_DRAW_SW_SUPPORT_XRGB8888

static void LV_ATTRIBUTE_FAST_MEM mask_only_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_u8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t src_x;
    int32_t dest_x;
    int32_t y;

    if(mask_buf == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8(dsc)) {
            for(y = 0; y < h; y++) {
                lv_memset(dest_buf_u8, 0xff, w);
                dest_buf_u8 += dest_stride;
            }
        }
    }
    else if(mask_buf == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(src_x = 0, dest_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_u8[dest_x], opa);
                }
                dest_buf_u8 += dest_stride;
            }
        }
    }
    else if(mask_buf && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                for(src_x = 0, dest_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_u8[dest_x], mask_buf[src_x]);
                }
                dest_buf_u8 += dest_stride;
                mask_buf += mask_stride;
            }
        }
    }
    else {
        if(LV_RESULT_INVALID == LV_DRAW_SW_MASK_ONLY_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(src_x = 0, dest_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_u8[dest_x], LV_OPA_MIX2(opa, mask_buf[src_x]));
                }
                dest_buf_u8 += dest_stride;
                mask_buf += mask_stride;
            }
        }
    }
}

#endif


#if LV_DRAW_SW_SUPPORT_I1

static void LV_ATTRIBUTE_FAST_MEM i1_image_blend(lv_draw_sw_blend_image_dsc_t * dsc)
{
    int32_t w = dsc->dest_w;
    int32_t h = dsc->dest_h;
    lv_opa_t opa = dsc->opa;
    uint8_t * dest_buf_a8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const uint8_t * src_buf_i1 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(mask_buf == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_TO_A8(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    if(get_bit(src_buf_i1, src_x))  dest_buf_a8[dest_x] = 0xff;
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
            }
        }
    }
    else if(mask_buf == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_TO_A8_WITH_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    if(get_bit(src_buf_i1, src_x)) {
                        BLEND_A8(dest_buf_a8[dest_x], opa);
                    }
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
            }
        }
    }
    else if(mask_buf && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_TO_A8_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    if(get_bit(src_buf_i1, src_x)) {
                        BLEND_A8(dest_buf_a8[dest_x], mask_buf[src_x]);
                    }
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else if(mask_buf && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_I1_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    if(get_bit(src_buf_i1, src_x)) {
                        lv_opa_t opa_res = LV_OPA_MIX2(mask_buf[src_x], opa);
                        BLEND_A8(dest_buf_a8[dest_x], opa_res);

                    }
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_i1 = drawbuf_next_row(src_buf_i1, src_stride);
                mask_buf += mask_stride;
            }
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
    uint8_t * dest_buf_a8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color16a_t * src_buf_al88 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t dest_x;
    int32_t src_x;
    int32_t y;

    if(mask_buf == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_TO_A8(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], src_buf_al88[src_x].alpha);
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
            }
        }
    }
    else if(mask_buf == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha, opa));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
            }
        }
    }
    else if(mask_buf && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_TO_A8_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX2(src_buf_al88[src_x].alpha, mask_buf[src_x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else if(mask_buf && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_AL88_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(dest_x = 0, src_x = 0; src_x < w; dest_x++, src_x++) {
                    BLEND_A8(dest_buf_a8[dest_x], LV_OPA_MIX3(src_buf_al88[src_x].alpha, mask_buf[src_x],
                                                              opa));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_al88 = drawbuf_next_row(src_buf_al88, src_stride);
                mask_buf += mask_stride;
            }
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
    uint8_t * dest_buf_a8 = dsc->dest_buf;
    int32_t dest_stride = dsc->dest_stride;
    const lv_color32_t * src_buf_c32 = dsc->src_buf;
    int32_t src_stride = dsc->src_stride;
    const lv_opa_t * mask_buf = dsc->mask_buf;
    int32_t mask_stride = dsc->mask_stride;

    int32_t x;
    int32_t y;

    if(mask_buf == NULL && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8(dsc)) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf_a8[x], src_buf_c32[x].alpha);
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
            }
        }
    }
    else if(mask_buf == NULL && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_WITH_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf_a8[x], LV_OPA_MIX2(src_buf_c32[x].alpha, opa));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
            }
        }
    }
    else if(mask_buf && opa >= LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_WITH_MASK(dsc)) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf_a8[x], LV_OPA_MIX2(src_buf_c32[x].alpha, mask_buf[x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
    else if(mask_buf && opa < LV_OPA_MAX) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_A8_BLEND_TO_A8_MIX_MASK_OPA(dsc)) {
            for(y = 0; y < h; y++) {
                for(x = 0; x < w; x++) {
                    BLEND_A8(dest_buf_a8[x], LV_OPA_MIX3(src_buf_c32[x].alpha, opa,
                                                         mask_buf[x]));
                }
                dest_buf_a8 = drawbuf_next_row(dest_buf_a8, dest_stride);
                src_buf_c32 = drawbuf_next_row(src_buf_c32, src_stride);
                mask_buf += mask_stride;
            }
        }
    }
}

#endif


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

/**
 * @file lv_blend_helium.h
 *
 */

#ifndef LV_BLEND_HELIUM_H
#define LV_BLEND_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#if defined(_RTE_)
#include "Pre_Include_Global.h"
#include "lv_conf_cmsis.h"
#endif

#include "../../../../lv_conf_internal.h"

/* detect whether helium is available based on arm compilers' standard */
#if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE

#ifdef LV_DRAW_SW_HELIUM_CUSTOM_INCLUDE
#include LV_DRAW_SW_HELIUM_CUSTOM_INCLUDE
#endif

#if !defined(__ASSEMBLY__)

/* Use arm2d functions if present */
#include "../arm2d/lv_blend_arm2d.h"

/*********************
 *      DEFINES
 *********************/

#if LV_USE_NATIVE_HELIUM_ASM

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(dsc) \
    _lv_color_blend_to_rgb565_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(dsc) \
    _lv_color_blend_to_rgb565_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(dsc) \
    _lv_color_blend_to_rgb565_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(dsc) \
    _lv_color_blend_to_rgb565_mix_mask_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_with_opa_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_with_mask_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_with_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_with_mask_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_helium(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888
#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888(dsc) \
    _lv_color_blend_to_argb8888_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA(dsc) \
    _lv_color_blend_to_argb8888_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK(dsc) \
    _lv_color_blend_to_argb8888_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA(dsc) \
    _lv_color_blend_to_argb8888_mix_mask_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_with_opa_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_with_mask_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_with_opa_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_with_mask_helium(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(dsc)
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t opa;
    void * dst_buf;
    uint32_t dst_w;
    uint32_t dst_h;
    uint32_t dst_stride;
    const void * src_buf;
    uint32_t src_stride;
    const lv_opa_t * mask_buf;
    uint32_t mask_stride;
} asm_dsc_t;
/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern void lv_color_blend_to_rgb565_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb565_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };

    lv_color_blend_to_rgb565_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb565_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb565_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    lv_color_blend_to_rgb565_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb565_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb565_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_color_blend_to_rgb565_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb565_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_color_blend_to_rgb565_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb565_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_rgb565_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb565_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_rgb565_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb565_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_rgb565_blend_normal_to_rgb565_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb565_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb565_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                   uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_rgb565_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_rgb565_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb565_with_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb565_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                            uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_rgb565_with_opa_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_rgb565_with_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb565_with_mask_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb565_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                             uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_rgb565_with_mask_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_rgb565_with_mask_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb565_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_rgb565_mix_mask_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb565_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_rgb565_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb565_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_rgb565_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb565_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_argb8888_blend_normal_to_rgb565_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb888_helium(asm_dsc_t * dsc);
extern void lv_color_blend_to_xrgb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    if(dst_px_size == 3) {
        lv_color_blend_to_rgb888_helium(&asm_dsc);
    }
    else {
        lv_color_blend_to_xrgb8888_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_color_blend_to_xrgb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                    uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    if(dst_px_size == 3) {
        lv_color_blend_to_rgb888_with_opa_helium(&asm_dsc);
    }
    else {
        lv_color_blend_to_xrgb8888_with_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_color_blend_to_xrgb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                     uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_color_blend_to_rgb888_with_mask_helium(&asm_dsc);
    }
    else {
        lv_color_blend_to_xrgb8888_with_mask_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_rgb888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_color_blend_to_xrgb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                        uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_color_blend_to_rgb888_mix_mask_opa_helium(&asm_dsc);
    }
    else {
        lv_color_blend_to_xrgb8888_mix_mask_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb888_helium(asm_dsc_t * dsc);
extern void lv_rgb565_blend_normal_to_xrgb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                   uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        lv_rgb565_blend_normal_to_rgb888_helium(&asm_dsc);
    }
    else {
        lv_rgb565_blend_normal_to_xrgb8888_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_rgb565_blend_normal_to_xrgb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                            uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        lv_rgb565_blend_normal_to_rgb888_with_opa_helium(&asm_dsc);
    }
    else {
        lv_rgb565_blend_normal_to_xrgb8888_with_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_rgb565_blend_normal_to_xrgb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                             uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_rgb565_blend_normal_to_rgb888_with_mask_helium(&asm_dsc);
    }
    else {
        lv_rgb565_blend_normal_to_xrgb8888_with_mask_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_rgb565_blend_normal_to_xrgb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(&asm_dsc);
    }
    else {
        lv_rgb565_blend_normal_to_xrgb8888_mix_mask_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb888_helium(asm_dsc_t * dsc);
extern void lv_rgb888_blend_normal_to_xrgb8888_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb888_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_xrgb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                   uint32_t dst_px_size,
                                                                   uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_rgb888_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_rgb888_helium(&asm_dsc);
        }
    }
    else {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_xrgb8888_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_xrgb8888_helium(&asm_dsc);
        }
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_rgb888_blend_normal_to_xrgb8888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_xrgb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                            uint32_t dst_px_size, uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_rgb888_with_opa_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_rgb888_with_opa_helium(&asm_dsc);
        }
    }
    else {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_xrgb8888_with_opa_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_xrgb8888_with_opa_helium(&asm_dsc);
        }
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_rgb888_blend_normal_to_xrgb8888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_xrgb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                             uint32_t dst_px_size, uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_rgb888_with_mask_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_rgb888_with_mask_helium(&asm_dsc);
        }
    }
    else {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_xrgb8888_with_mask_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_xrgb8888_with_mask_helium(&asm_dsc);
        }
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_rgb888_blend_normal_to_xrgb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_rgb888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_xrgb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t dst_px_size, uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_rgb888_mix_mask_opa_helium(&asm_dsc);
        }
    }
    else {
        if(src_px_size == 3) {
            lv_rgb888_blend_normal_to_xrgb8888_mix_mask_opa_helium(&asm_dsc);
        }
        else {
            lv_xrgb8888_blend_normal_to_xrgb8888_mix_mask_opa_helium(&asm_dsc);
        }
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb888_helium(asm_dsc_t * dsc);
extern void lv_argb8888_blend_normal_to_xrgb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_helium(&asm_dsc);
    }
    else {
        lv_argb8888_blend_normal_to_xrgb8888_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_argb8888_blend_normal_to_xrgb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(dst_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_with_opa_helium(&asm_dsc);
    }
    else {
        lv_argb8888_blend_normal_to_xrgb8888_with_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_argb8888_blend_normal_to_xrgb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                               uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_with_mask_helium(&asm_dsc);
    }
    else {
        lv_argb8888_blend_normal_to_xrgb8888_with_mask_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_argb8888_blend_normal_to_xrgb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                  uint32_t dst_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(dst_px_size == 3) {
        lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(&asm_dsc);
    }
    else {
        lv_argb8888_blend_normal_to_xrgb8888_mix_mask_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_argb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_argb8888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };

    lv_color_blend_to_argb8888_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_argb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_argb8888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    lv_color_blend_to_argb8888_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_argb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_argb8888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_color_blend_to_argb8888_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_color_blend_to_argb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_color_blend_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_color_blend_to_argb8888_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_argb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_argb8888_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_argb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_argb8888_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_argb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_rgb565_blend_normal_to_argb8888_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_argb8888_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_argb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_argb8888_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_argb8888_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_argb8888_with_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_argb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_argb8888_with_opa_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_argb8888_with_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_argb8888_with_mask_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_argb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                               uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_argb8888_with_mask_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_argb8888_with_mask_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
extern void lv_xrgb8888_blend_normal_to_argb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                  uint32_t src_px_size)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_argb8888_mix_mask_opa_helium(&asm_dsc);
    }
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_argb8888_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_argb8888_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_argb8888_with_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_argb8888_with_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_argb8888_with_mask_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_argb8888_blend_normal_to_argb8888_with_mask_helium(&asm_dsc);
    return LV_RESULT_OK;
}

extern void lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(asm_dsc_t * dsc);
static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride,
        .mask_buf = dsc->mask_buf,
        .mask_stride = dsc->mask_stride
    };
    lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(&asm_dsc);
    return LV_RESULT_OK;
}

#endif /* LV_USE_NATIVE_HELIUM_ASM */

#endif /* !defined(__ASSEMBLY__) */

#endif /* defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE */

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_HELIUM_H*/

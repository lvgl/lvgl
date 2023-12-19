/**
 * @file lv_blend_to_helium.h
 *
 */

#ifndef LV_BLEND_TO_HELIUM_H
#define LV_BLEND_TO_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && !defined(__ASSEMBLY__) && defined(LV_USE_GPU_ARM2D)

#define __ARM_2D_IMPL__
#include "arm_2d.h"
#include "__arm_2d_impl.h"

#if defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174,177,188,68,513,144,1296
#elif defined(__IS_COMPILER_IAR__)
#pragma diag_suppress=Pa093
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif
/*********************
 *      DEFINES
 *********************/

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(dsc) \
    _lv_color_blend_to_rgb565_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(dsc) \
    _lv_color_blend_to_rgb565_with_opa_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(dsc) \
    _lv_color_blend_to_rgb565_with_mask_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(dsc) \
    _lv_color_blend_to_rgb565_mix_mask_opa_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_with_opa_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_with_mask_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(dsc)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_with_opa_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_with_mask_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(dsc, src_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_with_opa_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_with_mask_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_helium(dsc, dst_px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_with_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_with_mask_helium(dsc, dst_px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size) \
    _lv_color_blend_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_helium(dsc, dst_px_size)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    _lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_helium(dsc, dst_px_size, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size, src_px_size)  \
    _lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size, src_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_helium(dsc, dst_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_with_mask_helium(dsc, dst_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(dsc, dst_px_size)

#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888(dsc) \
    _lv_color_blend_to_argb8888_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_OPA(dsc) \
    _lv_color_blend_to_argb8888_with_opa_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_WITH_MASK(dsc) \
    _lv_color_blend_to_argb8888_with_mask_helium(dsc)

#define LV_DRAW_SW_COLOR_BLEND_TO_ARGB8888_MIX_MASK_OPA(dsc) \
    _lv_color_blend_to_argb8888_mix_mask_opa_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_with_opa_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_with_mask_helium(dsc)

#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)  \
    _lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(dsc)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_with_opa_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_with_mask_helium(dsc, src_px_size)

#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc, src_px_size)  \
    _lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(dsc, src_px_size)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_with_opa_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_WITH_MASK(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_with_mask_helium(dsc)

#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_ARGB8888_MIX_MASK_OPA(dsc)  \
    _lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(dsc)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void _lv_color_blend_to_rgb565_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };

    lv_color_blend_to_rgb565_helium(&asm_dsc);
#endif
}

static inline void _lv_color_blend_to_rgb565_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    lv_color_blend_to_rgb565_with_opa_helium(&asm_dsc);
#endif
}

static inline void _lv_color_blend_to_rgb565_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_rgb565_helium(&asm_dsc);
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                         uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_rgb565_helium(&asm_dsc);
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_rgb888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_rgb888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_rgb888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                 uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                         uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dst_px_size,
                                                            uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t dst_px_size, uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dst_px_size, uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                         uint32_t dst_px_size, uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                       uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                        uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t dst_px_size)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_argb8888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };

    lv_color_blend_to_argb8888_helium(&asm_dsc);
#endif
}

static inline void _lv_color_blend_to_argb8888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .opa = dsc->opa,
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = &dsc->color
    };
    lv_color_blend_to_argb8888_with_opa_helium(&asm_dsc);
#endif
}

static inline void _lv_color_blend_to_argb8888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_color_blend_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_rgb565_blend_normal_to_argb8888_helium(&asm_dsc);
#endif
}

static inline void _lv_rgb565_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc, uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                       uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                        uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t src_px_size)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
    asm_dsc_t asm_dsc = {
        .dst_buf = dsc->dest_buf,
        .dst_w = dsc->dest_w,
        .dst_h = dsc->dest_h,
        .dst_stride = dsc->dest_stride,
        .src_buf = dsc->src_buf,
        .src_stride = dsc->src_stride
    };
    lv_argb8888_blend_normal_to_argb8888_helium(&asm_dsc);
#endif
}

static inline void _lv_argb8888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

static inline void _lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
#if 0
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
#endif
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && !defined(__ASSEMBLY__)*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_TO_HELIUM_H*/

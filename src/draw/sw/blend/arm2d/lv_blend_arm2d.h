/**
 * @file lv_blend_arm2d.h
 *
 */

#ifndef LV_BLEND_ARM2D_H
#define LV_BLEND_ARM2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"

#if LV_USE_DRAW_ARM2D_SYNC

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


#if ARM_2D_VERSION < 10106ul
#error Please upgrade to Arm-2D v1.1.6 or above
#endif

#ifndef LV_ARM2D_XRGB888_ALPHA_ALWAYS_FF
#define LV_ARM2D_XRGB888_ALPHA_ALWAYS_FF 1
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(dsc) \
    lv_color_blend_to_rgb565_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(dsc) \
    lv_color_blend_to_rgb565_with_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(dsc) \
    lv_color_blend_to_rgb565_with_mask_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(dsc) \
    lv_color_blend_to_rgb565_mix_mask_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(dsc)  \
    lv_rgb565_blend_normal_to_rgb565_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    lv_rgb565_blend_normal_to_rgb565_with_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    lv_rgb565_blend_normal_to_rgb565_with_mask_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(dsc, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb565_arm2d(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb565_with_opa_arm2d(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb565_with_mask_arm2d(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_arm2d(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(dsc)  \
    lv_argb8888_blend_normal_to_rgb565_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc)  \
    lv_argb8888_blend_normal_to_rgb565_with_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc)  \
    lv_argb8888_blend_normal_to_rgb565_with_mask_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc)  \
    lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_arm2d(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dst_px_size) \
    lv_color_blend_to_rgb888_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dst_px_size) \
    lv_color_blend_to_rgb888_with_opa_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dst_px_size) \
    lv_color_blend_to_rgb888_with_mask_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size) \
    lv_color_blend_to_rgb888_mix_mask_opa_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    lv_rgb565_blend_normal_to_rgb888_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    lv_rgb565_blend_normal_to_rgb888_with_opa_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    lv_rgb565_blend_normal_to_rgb888_with_mask_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb888_arm2d(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb888_with_opa_arm2d(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb888_with_mask_arm2d(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size, src_px_size)  \
    lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_arm2d(dsc, dst_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dst_px_size)  \
    lv_argb8888_blend_normal_to_rgb888_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dst_px_size)  \
    lv_argb8888_blend_normal_to_rgb888_with_opa_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dst_px_size)  \
    lv_argb8888_blend_normal_to_rgb888_with_mask_arm2d(dsc, dst_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dst_px_size)  \
    lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_arm2d(dsc, dst_px_size)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline lv_result_t lv_color_blend_to_rgb565_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint16_t);
    __arm_2d_impl_rgb16_colour_filling((uint16_t *)dsc->dest_buf,
                                       stride,
                                       &draw_size,
                                       lv_color_to_u16(dsc->color));
    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint16_t);
    __arm_2d_impl_rgb565_colour_filling_with_opacity((uint16_t *)dsc->dest_buf,
                                                     stride,
                                                     &draw_size,
                                                     lv_color_to_u16(dsc->color),
                                                     dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint16_t);
    __arm_2d_impl_rgb565_colour_filling_mask((uint16_t *)dsc->dest_buf,
                                             stride,
                                             (uint8_t *)dsc->mask_buf,
                                             dsc->mask_stride,
                                             &draw_size,
                                             lv_color_to_u16(dsc->color));
    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint16_t);
    __arm_2d_impl_rgb565_colour_filling_mask_opacity((uint16_t *)dsc->dest_buf,
                                                     stride,
                                                     (uint8_t *)dsc->mask_buf,
                                                     dsc->mask_stride,
                                                     &draw_size,
                                                     lv_color_to_u16(dsc->color),
                                                     dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);
    __arm_2d_impl_rgb16_copy((uint16_t *)dsc->src_buf,
                             src_stride,
                             (uint16_t *)dsc->dest_buf,
                             des_stride,
                             &draw_size);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);
    __arm_2d_impl_rgb565_tile_copy_opacity((uint16_t *)dsc->src_buf,
                                           src_stride,
                                           (uint16_t *)dsc->dest_buf,
                                           des_stride,
                                           &draw_size,
                                           dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);
    __arm_2d_impl_rgb565_src_msk_copy((uint16_t *)dsc->src_buf,
                                      src_stride,
                                      (uint8_t *)dsc->mask_buf,
                                      dsc->mask_stride,
                                      &draw_size,
                                      (uint16_t *)dsc->dest_buf,
                                      des_stride,
                                      &draw_size);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    __arm_2d_impl_rgb565_tile_copy_with_src_mask_and_opacity((uint16_t *)dsc->src_buf,
                                                             src_stride,
                                                             (uint8_t *)dsc->mask_buf,
                                                             dsc->mask_stride,
                                                             &draw_size,
                                                             (uint16_t *)dsc->dest_buf,
                                                             des_stride,
                                                             &draw_size,
                                                             dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t src_px_size)
{
    if(src_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)dsc->dest_buf,
                                    des_stride,
                                    &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                          uint32_t src_px_size)
{
    if(src_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

#if !LV_ARM2D_XRGB888_ALPHA_ALWAYS_FF
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_rgb565_tile_copy_opacity(tmp_buf,
                                           des_stride,
                                           (uint16_t *)dsc->dest_buf,
                                           des_stride,
                                           &draw_size,
                                           dsc->opa);
#else
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_opacity((uint32_t *)dsc->src_buf,
                                                            src_stride,
                                                            (uint16_t *)dsc->dest_buf,
                                                            des_stride,
                                                            &draw_size,
                                                            dsc->opa);
#endif
    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t src_px_size)
{
    if(src_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

#if !LV_ARM2D_XRGB888_ALPHA_ALWAYS_FF
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_rgb565_src_msk_copy(tmp_buf,
                                      des_stride,
                                      (uint8_t *)dsc->mask_buf,
                                      dsc->mask_stride,
                                      &draw_size,
                                      (uint16_t *)dsc->dest_buf,
                                      des_stride,
                                      &draw_size);
#else
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask((uint32_t *)dsc->src_buf,
                                                             src_stride,
                                                             (uint8_t *)dsc->mask_buf,
                                                             dsc->mask_stride,
                                                             &draw_size,
                                                             (uint16_t *)dsc->dest_buf,
                                                             des_stride,
                                                             &draw_size);
#endif

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t src_px_size)
{
    if(src_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

#if !LV_ARM2D_XRGB888_ALPHA_ALWAYS_FF
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_rgb565_tile_copy_with_src_mask_and_opacity(tmp_buf,
                                                             des_stride,
                                                             (uint8_t *)dsc->mask_buf,
                                                             dsc->mask_stride,
                                                             &draw_size,
                                                             (uint16_t *)dsc->dest_buf,
                                                             des_stride,
                                                             &draw_size,
                                                             dsc->opa);
#else
    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity((uint32_t *)dsc->src_buf,
                                                                         src_stride,
                                                                         (uint8_t *)dsc->mask_buf,
                                                                         dsc->mask_stride,
                                                                         &draw_size,
                                                                         (uint16_t *)dsc->dest_buf,
                                                                         des_stride,
                                                                         &draw_size,
                                                                         dsc->opa);
#endif

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_to_rgb565((uint32_t *)dsc->src_buf,
                                     src_stride,
                                     (uint16_t *)dsc->dest_buf,
                                     des_stride,
                                     &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_opacity((uint32_t *)dsc->src_buf,
                                                            src_stride,
                                                            (uint16_t *)dsc->dest_buf,
                                                            des_stride,
                                                            &draw_size,
                                                            dsc->opa);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask((uint32_t *)dsc->src_buf,
                                                             src_stride,
                                                             (uint8_t *)dsc->mask_buf,
                                                             dsc->mask_stride,
                                                             &draw_size,
                                                             (uint16_t *)dsc->dest_buf,
                                                             des_stride,
                                                             &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_rgb565_with_src_mask_and_opacity((uint32_t *)dsc->src_buf,
                                                                         src_stride,
                                                                         (uint8_t *)dsc->mask_buf,
                                                                         dsc->mask_stride,
                                                                         &draw_size,
                                                                         (uint16_t *)dsc->dest_buf,
                                                                         des_stride,
                                                                         &draw_size,
                                                                         dsc->opa);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb888_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint32_t);
    __arm_2d_impl_rgb32_colour_filling((uint32_t *)dsc->dest_buf,
                                       stride,
                                       &draw_size,
                                       lv_color_to_u32(dsc->color));
    return LV_RESULT_OK;

}

static inline lv_result_t lv_color_blend_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                  uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint32_t);
    __arm_2d_impl_cccn888_colour_filling_with_opacity((uint32_t *)dsc->dest_buf,
                                                      stride,
                                                      &draw_size,
                                                      lv_color_to_u32(dsc->color),
                                                      dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                   uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint32_t);
    __arm_2d_impl_cccn888_colour_filling_mask((uint32_t *)dsc->dest_buf,
                                              stride,
                                              (uint8_t *)dsc->mask_buf,
                                              dsc->mask_stride,
                                              &draw_size,
                                              lv_color_to_u32(dsc->color));
    return LV_RESULT_OK;
}

static inline lv_result_t lv_color_blend_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                      uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint32_t);
    __arm_2d_impl_cccn888_colour_filling_mask_opacity((uint32_t *)dsc->dest_buf,
                                                      stride,
                                                      (uint8_t *)dsc->mask_buf,
                                                      dsc->mask_stride,
                                                      &draw_size,
                                                      lv_color_to_u32(dsc->color),
                                                      dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    __arm_2d_impl_rgb565_to_cccn888((uint16_t *)dsc->src_buf,
                                    src_stride,
                                    (uint32_t *)dsc->dest_buf,
                                    des_stride,
                                    &draw_size);

    return LV_RESULT_OK;

}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                          uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    uint32_t * tmp_buf = (uint32_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

    /* get rgb565 */
    __arm_2d_impl_rgb565_to_cccn888((uint16_t *)dsc->src_buf,
                                    src_stride,
                                    (uint32_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_cccn888_tile_copy_opacity(tmp_buf,
                                            des_stride,
                                            (uint32_t *)dsc->dest_buf,
                                            des_stride,
                                            &draw_size,
                                            dsc->opa);

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    uint32_t * tmp_buf = (uint32_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

    __arm_2d_impl_rgb565_to_cccn888((uint16_t *)dsc->src_buf,
                                    src_stride,
                                    (uint32_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_cccn888_src_msk_copy(tmp_buf,
                                       des_stride,
                                       (uint8_t *)dsc->mask_buf,
                                       dsc->mask_stride,
                                       &draw_size,
                                       (uint32_t *)dsc->dest_buf,
                                       des_stride,
                                       &draw_size);

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    uint32_t * tmp_buf = (uint32_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

    __arm_2d_impl_rgb565_to_cccn888((uint16_t *)dsc->src_buf,
                                    src_stride,
                                    (uint32_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_cccn888_tile_copy_with_src_mask_and_opacity(tmp_buf,
                                                              des_stride,
                                                              (uint8_t *)dsc->mask_buf,
                                                              dsc->mask_stride,
                                                              &draw_size,
                                                              (uint32_t *)dsc->dest_buf,
                                                              des_stride,
                                                              &draw_size,
                                                              dsc->opa);

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dst_px_size,
                                                                 uint32_t src_px_size)
{
    if((dst_px_size == 3) || (src_px_size == 3)) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_rgb32_copy((uint32_t *)dsc->src_buf,
                             src_stride,
                             (uint32_t *)dsc->dest_buf,
                             des_stride,
                             &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                          uint32_t dst_px_size, uint32_t src_px_size)
{
    if((dst_px_size == 3) || (src_px_size == 3)) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_tile_copy_opacity((uint32_t *)dsc->src_buf,
                                            src_stride,
                                            (uint32_t *)dsc->dest_buf,
                                            des_stride,
                                            &draw_size,
                                            dsc->opa);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                           uint32_t dst_px_size, uint32_t src_px_size)
{
    if((dst_px_size == 3) || (src_px_size == 3)) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_src_msk_copy((uint32_t *)dsc->src_buf,
                                       src_stride,
                                       (uint8_t *)dsc->mask_buf,
                                       dsc->mask_stride,
                                       &draw_size,
                                       (uint32_t *)dsc->dest_buf,
                                       des_stride,
                                       &draw_size);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t dst_px_size, uint32_t src_px_size)
{
    if((dst_px_size == 3) || (src_px_size == 3)) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_tile_copy_with_src_mask_and_opacity((uint32_t *)dsc->src_buf,
                                                              src_stride,
                                                              (uint8_t *)dsc->mask_buf,
                                                              dsc->mask_stride,
                                                              &draw_size,
                                                              (uint32_t *)dsc->dest_buf,
                                                              des_stride,
                                                              &draw_size,
                                                              dsc->opa);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                   uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_to_cccn888((uint32_t *)dsc->src_buf,
                                      src_stride,
                                      (uint32_t *)dsc->dest_buf,
                                      des_stride,
                                      &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                            uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_opacity((uint32_t *)dsc->src_buf,
                                                             src_stride,
                                                             (uint32_t *)dsc->dest_buf,
                                                             des_stride,
                                                             &draw_size,
                                                             dsc->opa);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                             uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask((uint32_t *)dsc->src_buf,
                                                              src_stride,
                                                              (uint8_t *)dsc->mask_buf,
                                                              dsc->mask_stride,
                                                              &draw_size,
                                                              (uint32_t *)dsc->dest_buf,
                                                              des_stride,
                                                              &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_ccca8888_tile_copy_to_cccn888_with_src_mask_and_opacity((uint32_t *)dsc->src_buf,
                                                                          src_stride,
                                                                          (uint8_t *)dsc->mask_buf,
                                                                          dsc->mask_stride,
                                                                          &draw_size,
                                                                          (uint32_t *)dsc->dest_buf,
                                                                          des_stride,
                                                                          &draw_size,
                                                                          dsc->opa);

    return LV_RESULT_OK;
}

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_ARM2D_SYNC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_ARM2D_H*/

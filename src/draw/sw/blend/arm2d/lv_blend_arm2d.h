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
/*********************
 *      DEFINES
 *********************/

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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline lv_result_t _lv_color_blend_to_rgb565_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t stride = (dsc->dest_stride) / sizeof(uint16_t);
    __arm_2d_impl_rgb16_colour_filling((uint16_t *)dsc->dest_buf,
                                       stride,
                                       &draw_size,
                                       lv_color_to_u16(dsc->color));
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_color_blend_to_rgb565_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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

static inline lv_result_t _lv_color_blend_to_rgb565_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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

static inline lv_result_t _lv_color_blend_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint16_t);

    __arm_2d_impl_gray8_colour_filling_with_opacity((uint8_t *)dsc->mask_buf,
                                                    dsc->mask_stride,
                                                    &draw_size,
                                                    0x00,
                                                    255 - dsc->opa);

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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_gray8_colour_filling_with_opacity((uint8_t *)dsc->mask_buf,
                                                    dsc->mask_stride,
                                                    &draw_size,
                                                    0x00,
                                                    255 - dsc->opa);

    __arm_2d_impl_rgb565_src_msk_copy(tmp_buf,
                                      des_stride,
                                      (uint8_t *)dsc->mask_buf,
                                      dsc->mask_stride,
                                      &draw_size,
                                      (uint16_t *)dsc->dest_buf,
                                      des_stride,
                                      &draw_size);

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }

    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    __arm_2d_impl_rgb565_src_chn_msk_copy(tmp_buf,
                                          des_stride,
                                          (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                          src_stride,
                                          &draw_size,
                                          (uint16_t *)dsc->dest_buf,
                                          des_stride,
                                          &draw_size);

    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }
    uint8_t * tmp_msk = (uint8_t *)lv_malloc(des_stride * dsc->dest_h);
    if(NULL == tmp_msk) {
        lv_free(tmp_buf);
        return LV_RESULT_INVALID;
    }

    /* get rgb565 */
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    lv_memzero(tmp_msk, des_stride * dsc->dest_h);

    /* get mask */
    __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(tmp_msk,
                                                            des_stride,
                                                            (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                            src_stride,
                                                            &draw_size,
                                                            0xFF,
                                                            dsc->opa);

    __arm_2d_impl_rgb565_src_msk_copy(tmp_buf,
                                      des_stride,
                                      tmp_msk,
                                      des_stride,
                                      &draw_size,
                                      (uint16_t *)dsc->dest_buf,
                                      des_stride,
                                      &draw_size);

    lv_free(tmp_msk);
    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }
    uint8_t * tmp_msk = (uint8_t *)lv_malloc(des_stride * dsc->dest_h);
    if(NULL == tmp_msk) {
        lv_free(tmp_buf);
        return LV_RESULT_INVALID;
    }

    /* get rgb565 */
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    lv_memzero(tmp_msk, des_stride * dsc->dest_h);

    /* get mask */
    __arm_2d_impl_gray8_colour_filling_channel_mask(tmp_msk,
                                                    des_stride,
                                                    (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                    src_stride,
                                                    &draw_size,
                                                    0xFF);

    __arm_2d_impl_rgb565_masks_copy(tmp_buf,
                                    des_stride,
                                    tmp_msk,
                                    des_stride,
                                    &draw_size,
                                    (uint16_t *)dsc->dest_buf,
                                    des_stride,
                                    (uint8_t *)dsc->mask_buf,
                                    dsc->mask_stride,
                                    &draw_size,
                                    &draw_size);

    lv_free(tmp_msk);
    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
{
    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint16_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint16_t * tmp_buf = (uint16_t *)lv_malloc(dsc->dest_stride * dsc->dest_h);
    if(NULL == tmp_buf) {
        return LV_RESULT_INVALID;
    }
    uint8_t * tmp_msk = (uint8_t *)lv_malloc(des_stride * dsc->dest_h);
    if(NULL == tmp_msk) {
        lv_free(tmp_buf);
        return LV_RESULT_INVALID;
    }

    /* get rgb565 */
    __arm_2d_impl_cccn888_to_rgb565((uint32_t *)dsc->src_buf,
                                    src_stride,
                                    (uint16_t *)tmp_buf,
                                    des_stride,
                                    &draw_size);

    lv_memzero(tmp_msk, des_stride * dsc->dest_h);

    /* get mask */
    __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(tmp_msk,
                                                            des_stride,
                                                            (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                            src_stride,
                                                            &draw_size,
                                                            0xFF,
                                                            dsc->opa);

    __arm_2d_impl_rgb565_masks_copy(tmp_buf,
                                    des_stride,
                                    tmp_msk,
                                    des_stride,
                                    &draw_size,
                                    (uint16_t *)dsc->dest_buf,
                                    des_stride,
                                    (uint8_t *)dsc->mask_buf,
                                    dsc->mask_stride,
                                    &draw_size,
                                    &draw_size);

    lv_free(tmp_msk);
    lv_free(tmp_buf);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_color_blend_to_rgb888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size)
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

static inline lv_result_t _lv_color_blend_to_rgb888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
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

static inline lv_result_t _lv_color_blend_to_rgb888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
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

static inline lv_result_t _lv_color_blend_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

    __arm_2d_impl_gray8_colour_filling_with_opacity((uint8_t *)dsc->mask_buf,
                                                    dsc->mask_stride,
                                                    &draw_size,
                                                    0x00,
                                                    255 - dsc->opa);

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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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

static inline lv_result_t _lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                uint32_t dst_px_size, uint32_t src_px_size)
{
    if((dst_px_size == 3) || (src_px_size == 3)) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_gray8_colour_filling_with_opacity((uint8_t *)dsc->mask_buf,
                                                    dsc->mask_stride,
                                                    &draw_size,
                                                    0x00,
                                                    255 - dsc->opa);

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

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                     uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_src_chn_msk_copy((uint32_t *)dsc->src_buf,
                                           src_stride,
                                           (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                           src_stride,
                                           &draw_size,
                                           (uint32_t *)dsc->dest_buf,
                                           des_stride,
                                           &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                              uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    uint8_t * tmp_msk = (uint8_t *)lv_malloc(des_stride * dsc->dest_h);
    if(NULL == tmp_msk) {
        return LV_RESULT_INVALID;
    }

    lv_memzero(tmp_msk, des_stride * dsc->dest_h);

    /* get mask */
    __arm_2d_impl_gray8_colour_filling_channel_mask_opacity(tmp_msk,
                                                            des_stride,
                                                            (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                            src_stride,
                                                            &draw_size,
                                                            0xFF,
                                                            dsc->opa);

    __arm_2d_impl_cccn888_src_msk_copy((uint32_t *)dsc->src_buf,
                                       src_stride,
                                       tmp_msk,
                                       des_stride,
                                       &draw_size,
                                       (uint32_t *)dsc->dest_buf,
                                       des_stride,
                                       &draw_size);

    lv_free(tmp_msk);
    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                               uint32_t dst_px_size)
{

    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy((uint32_t *)dsc->src_buf,
                                                   src_stride,
                                                   (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                   src_stride,
                                                   &draw_size,
                                                   (uint32_t *)dsc->dest_buf,
                                                   des_stride,
                                                   (uint8_t *)dsc->mask_buf,
                                                   dsc->mask_stride,
                                                   &draw_size,
                                                   &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
                                                                                  uint32_t dst_px_size)
{
    if(dst_px_size == 3) {
        return LV_RESULT_INVALID;
    }

    arm_2d_size_t draw_size = {dsc->dest_w, dsc->dest_h};
    int16_t des_stride = dsc->dest_stride / sizeof(uint32_t);
    int16_t src_stride = dsc->src_stride / sizeof(uint32_t);

    __arm_2d_impl_gray8_colour_filling_with_opacity((uint8_t *)dsc->mask_buf,
                                                    dsc->mask_stride,
                                                    &draw_size,
                                                    0x00,
                                                    255 - dsc->opa);

    __arm_2d_impl_cccn888_src_chn_msk_des_msk_copy((uint32_t *)dsc->src_buf,
                                                   src_stride,
                                                   (uint32_t *)((uintptr_t)(dsc->src_buf) + 3),
                                                   src_stride,
                                                   &draw_size,
                                                   (uint32_t *)dsc->dest_buf,
                                                   des_stride,
                                                   (uint8_t *)dsc->mask_buf,
                                                   dsc->mask_stride,
                                                   &draw_size,
                                                   &draw_size);

    return LV_RESULT_OK;
}

static inline lv_result_t _lv_color_blend_to_argb8888_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_color_blend_to_argb8888_with_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_color_blend_to_argb8888_with_mask_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_color_blend_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_fill_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb565_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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
    if(src_px_size == 3) {
        lv_rgb888_blend_normal_to_argb8888_helium(&asm_dsc);
    }
    else {
        lv_xrgb8888_blend_normal_to_argb8888_helium(&asm_dsc);
    }
#endif
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_rgb888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc,
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_with_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_with_mask_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

static inline lv_result_t _lv_argb8888_blend_normal_to_argb8888_mix_mask_opa_helium(_lv_draw_sw_blend_image_dsc_t * dsc)
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
    return LV_RESULT_INVALID;
}

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_ARM2D_SYNC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_ARM2D_H*/

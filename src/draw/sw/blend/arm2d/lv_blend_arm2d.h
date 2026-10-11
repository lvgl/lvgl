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

#include "../../../../lvgl_public.h"

#if LV_USE_DRAW_ARM2D_SYNC

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

lv_result_t lv_color_blend_to_rgb565_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc);

lv_result_t lv_color_blend_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc);

lv_result_t lv_color_blend_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc);

lv_result_t lv_color_blend_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc);

lv_result_t lv_rgb565_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_rgb565_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_rgb565_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_rgb565_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_rgb888_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                   uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                            uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                             uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t src_px_size);

lv_result_t lv_argb8888_blend_normal_to_rgb565_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_argb8888_blend_normal_to_rgb565_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_argb8888_blend_normal_to_rgb565_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_argb8888_blend_normal_to_rgb565_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_color_blend_to_rgb888_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dst_px_size);

lv_result_t lv_color_blend_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                    uint32_t dst_px_size);

lv_result_t lv_color_blend_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                     uint32_t dst_px_size);

lv_result_t lv_color_blend_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                        uint32_t dst_px_size);

lv_result_t lv_rgb565_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                   uint32_t dst_px_size);

lv_result_t lv_rgb565_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                            uint32_t dst_px_size);

lv_result_t lv_rgb565_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                             uint32_t dst_px_size);

lv_result_t lv_rgb565_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dst_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                   uint32_t dst_px_size,
                                                   uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                            uint32_t dst_px_size,
                                                            uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                             uint32_t dst_px_size,
                                                             uint32_t src_px_size);

lv_result_t lv_rgb888_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dst_px_size,
                                                                uint32_t src_px_size);

lv_result_t lv_argb8888_blend_normal_to_rgb888_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                     uint32_t dst_px_size);

lv_result_t lv_argb8888_blend_normal_to_rgb888_with_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                              uint32_t dst_px_size);

lv_result_t lv_argb8888_blend_normal_to_rgb888_with_mask_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dst_px_size);

lv_result_t lv_argb8888_blend_normal_to_rgb888_mix_mask_opa_arm2d(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dst_px_size);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_ARM2D_SYNC */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BLEND_ARM2D_H*/

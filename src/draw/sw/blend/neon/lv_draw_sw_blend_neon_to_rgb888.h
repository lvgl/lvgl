/**
 * @file lv_draw_sw_blend_neon_to_rgb888.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_NEON_TO_RGB888_H
#define LV_DRAW_SW_BLEND_NEON_TO_RGB888_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON

#include "../../../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dest_px_size) lv_draw_sw_blend_neon_color_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_color_to_rgb888_with_opa(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dest_px_size) lv_draw_sw_blend_neon_color_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_color_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif

#ifdef __aarch64__ /* This function uses a special intrinsic only available for arm64 */
#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) lv_draw_sw_blend_neon_l8_to_rgb888(dsc, dest_px_size)
#endif
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) lv_draw_sw_blend_neon_rgb565_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size) lv_draw_sw_blend_neon_rgb565_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb888(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb888_with_mask(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa_mask(dsc, dest_px_size, src_px_size)
#endif

#if 0 /* These seem to produce worse results than sw rendering, also RGB888 is not implemented, only XRGB8888. So they are disabled for now*/
#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) lv_draw_sw_blend_neon_argb888_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa(dsc, dest_px_size)

#endif
#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size) lv_draw_sw_blend_neon_argb888_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb888(dsc, dest_px_size)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_result_t lv_draw_sw_blend_neon_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                uint32_t dest_px_size);

#ifdef __aarch64__
lv_result_t lv_draw_sw_blend_neon_l8_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
#endif /*__aarch64__*/

lv_result_t lv_draw_sw_blend_neon_al88_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size);

lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                   uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                            uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                             uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size, uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                              uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_NEON_TO_RGB888_H*/

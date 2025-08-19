/**
 * @file lv_draw_sw_blend_neon_to_rgb565.h
 *
 */

#ifndef LV_DRAW_SW_BLEND_NEON_TO_RGB565_H
#define LV_DRAW_SW_BLEND_NEON_TO_RGB565_H

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

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565(dsc) lv_draw_sw_blend_neon_color_to_rgb565(dsc)

#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_OPA(dsc) lv_draw_sw_blend_neon_color_to_rgb565_with_opa(dsc)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_WITH_MASK(dsc) lv_draw_sw_blend_neon_color_to_rgb565_with_mask(dsc)

#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB565_MIX_MASK_OPA(dsc) lv_draw_sw_blend_neon_color_to_rgb565_with_opa_mask(dsc)
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565(dsc) lv_draw_sw_blend_neon_l8_to_rgb565(dsc)
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_OPA(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_WITH_MASK(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_L8_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565(dsc) lv_draw_sw_blend_neon_al88_to_rgb565(dsc)
#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc) lv_draw_sw_blend_neon_al88_to_rgb565_with_opa(dsc)
#endif

#if 0 /* Disabled as it's not tested */
#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc) lv_draw_sw_blend_neon_al88_to_rgb565_with_mask(dsc)

#endif

#ifndef LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_AL88_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc) lv_draw_sw_blend_neon_al88_to_rgb565_with_opa_mask(dsc)
#endif
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565(dsc) lv_draw_sw_blend_neon_rgb565_to_rgb565(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc) lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc) lv_draw_sw_blend_neon_rgb565_to_rgb565_with_mask(dsc)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc) lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa_mask(dsc)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565(dsc, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb565(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb565_with_mask(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc, src_px_size) lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa_mask(dsc, src_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565(dsc) lv_draw_sw_blend_neon_argb888_to_rgb565(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_OPA(dsc) lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_WITH_MASK(dsc) lv_draw_sw_blend_neon_argb888_to_rgb565_with_mask(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(dsc) lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa_mask(dsc)
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565(dsc) lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb565(dsc)
#endif

/*
 * Bleding operations with premultiplied argb8888 require division.
 * As division is not supported for integer values in neon we don't define these functions
 */
#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_OPA
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_OPA(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_MASK
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_WITH_MASK(...) LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB565_MIX_MASK_OPA(...) LV_RESULT_INVALID
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_result_t lv_draw_sw_blend_neon_color_to_rgb565(lv_draw_sw_blend_fill_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_color_to_rgb565_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_l8_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_al88_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc);

lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_rgb565_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc, uint8_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_rgb888_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint8_t src_px_size);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_argb888_premultiplied_to_rgb565(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_mask(lv_draw_sw_blend_image_dsc_t * dsc);
lv_result_t lv_draw_sw_blend_neon_argb888_to_rgb565_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_NEON */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_BLEND_NEON_TO_RGB565_H*/

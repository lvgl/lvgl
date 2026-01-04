/**
 * @file lv_draw_sw_blend_riscv_v_to_rgb888.h
 */

#ifndef LV_DRAW_SW_BLEND_RISCV_V_TO_RGB888_H
#define LV_DRAW_SW_BLEND_RISCV_V_TO_RGB888_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"
#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V

#include "../../../../misc/lv_types.h"
/*********************
 *      DEFINES
 *********************/

/* Color fill to RGB888/XRGB8888 */
#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_color_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_WITH_MASK(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_color_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_COLOR_BLEND_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif


/* RGB565 image blend to RGB888/XRGB8888 */
#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_rgb565_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB565_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif

/* RGB888/XRGB8888 image blend to RGB888/XRGB8888 */
#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size, src_px_size) \
    lv_draw_sw_blend_riscv_v_rgb888_to_rgb888(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size, src_px_size) \
    lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size, src_px_size) \
    lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_mask(dsc, dest_px_size, src_px_size)
#endif

#ifndef LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_RGB888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size, src_px_size) \
    lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa_mask(dsc, dest_px_size, src_px_size)
#endif

/* ARGB8888 image blend to RGB888/XRGB8888 */
#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_argb8888_to_rgb888(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_WITH_MASK(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA
#define LV_DRAW_SW_ARGB8888_BLEND_NORMAL_TO_RGB888_MIX_MASK_OPA(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa_mask(dsc, dest_px_size)
#endif

#ifndef LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB888
#define LV_DRAW_SW_ARGB8888_PREMULTIPLIED_BLEND_NORMAL_TO_RGB888(dsc, dest_px_size) \
    lv_draw_sw_blend_riscv_v_argb8888_premultiplied_to_rgb888(dsc, dest_px_size)
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/* Color fill functions */
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa(lv_draw_sw_blend_fill_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                               uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_color_to_rgb888_with_opa_mask(lv_draw_sw_blend_fill_dsc_t * dsc,
                                                                   uint32_t dest_px_size);

/* RGB565 to RGB888/XRGB8888 blend functions */
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb565_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size);

/* RGB888/XRGB8888 to RGB888/XRGB8888 blend functions */
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size,
                                                      uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                               uint32_t dest_px_size, uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                uint32_t dest_px_size, uint32_t src_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_rgb888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                    uint32_t dest_px_size, uint32_t src_px_size);

/* ARGB8888 to RGB888/XRGB8888 blend functions */
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc, uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                 uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                  uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_to_rgb888_with_opa_mask(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size);
lv_result_t lv_draw_sw_blend_riscv_v_argb8888_premultiplied_to_rgb888(lv_draw_sw_blend_image_dsc_t * dsc,
                                                                      uint32_t dest_px_size);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_RISCV_V */

#ifdef __cplusplus
}
#endif

#endif /* LV_DRAW_SW_BLEND_RISCV_V_TO_RGB888_H */

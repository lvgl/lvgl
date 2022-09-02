/**
 * @file lv_gpu_gd32_ipa.h
 *
 */

#ifndef LV_GPU_GD32_IPA_H
#define LV_GPU_GD32_IPA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"
#include "../sw/lv_draw_sw.h"

#if LV_USE_GPU_GD32_IPA

/*********************
 *      DEFINES
 *********************/

#define LV_IPA_ARGB8888 0
#define LV_IPA_RGB888 1
#define LV_IPA_RGB565 2
#define LV_IPA_ARGB1555 3
#define LV_IPA_ARGB4444 4

/**********************
 *      TYPEDEFS
 **********************/
typedef lv_draw_sw_ctx_t lv_draw_gd32_ipa_ctx_t;

struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_gd32_ipa_init(void);

void lv_draw_gd32_ipa_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_gd32_ipa_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

void lv_draw_gd32_ipa_blend(lv_draw_ctx_t * draw_ctx, const lv_draw_sw_blend_dsc_t * dsc);

void lv_draw_gd32_ipa_buffer_copy(lv_draw_ctx_t * draw_ctx,
                                  void * dest_buf, lv_coord_t dest_stride, const lv_area_t * dest_area,
                                  void * src_buf, lv_coord_t src_stride, const lv_area_t * src_area);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_GPU_GD32_IPA*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_GD32_IPA_H*/

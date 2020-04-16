/**
 * @file lv_gpu_stm32_dma2d.h
 *
 */

#ifndef LV_GPU_STM32_DMA2D_H
#define LV_GPU_STM32_DMA2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_gpu_stm32_dma2d_fill(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, lv_coord_t fill_w, lv_coord_t fill_h);
void lv_gpu_stm32_dma2d_copy(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h);
void lv_gpu_stm32_dma2d_blend(lv_color_t * buf, lv_coord_t buf_w, const lv_color_t * map, lv_opa_t opa, lv_coord_t map_w, lv_coord_t copy_w, lv_coord_t copy_h);
void lv_gpu_stm32_dma2d_fill_mask(lv_color_t * buf, lv_coord_t buf_w, lv_color_t color, const lv_opa_t * mask,  lv_opa_t opa, lv_coord_t fill_w, lv_coord_t fill_h);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GPU_STM32_DMA2D_H*/

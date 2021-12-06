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
#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/

#define LV_DMA2D_ARGB8888 0
#define LV_DMA2D_RGB888 1
#define LV_DMA2D_RGB565 2
#define LV_DMA2D_ARGB1555 3
#define LV_DMA2D_ARGB4444 4

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Turn on the peripheral and set output color mode, this only needs to be done once
 */
void lv_draw_stm32_dma2d_init(void);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_STM32_DMA2D_H*/

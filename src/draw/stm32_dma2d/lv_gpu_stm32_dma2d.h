/**
 * @file lv_gpu_stm32_dma2d.h
 *
 */

#ifndef LV_GPU_STM32_DMA2D_H
#define LV_GPU_STM32_DMA2D_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../misc/lv_color.h"
#include "../../hal/lv_hal_disp.h"
#include "../sw/lv_draw_sw.h"

#if LV_USE_GPU_STM32_DMA2D

/*********************
 *      INCLUDES
 *********************/
#include LV_GPU_DMA2D_CMSIS_INCLUDE

/*********************
 *      DEFINES
 *********************/
#if defined(LV_STM32_DMA2D_TEST)
// removes "static" modifier for some internal methods in order to test them
#define LV_STM32_DMA2D_STATIC
#else
#define LV_STM32_DMA2D_STATIC static
#endif

/**********************
 *      TYPEDEFS
 **********************/
enum dma2d_color_format {
    ARGB8888 = 0x0,
    RGB888 = 0x01,
    RGB565 = 0x02,
    ARGB1555 = 0x03,
    ARGB4444 = 0x04,
    A8 = 0x09,
    UNSUPPORTED = 0xff,
};
typedef enum dma2d_color_format dma2d_color_format_t;
typedef lv_draw_sw_ctx_t lv_draw_stm32_dma2d_ctx_t;
struct _lv_disp_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_stm32_dma2d_init(void);
void lv_draw_stm32_dma2d_ctx_init(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);
void lv_draw_stm32_dma2d_ctx_deinit(struct _lv_disp_drv_t * drv, lv_draw_ctx_t * draw_ctx);

/**********************
 *      MACROS
 **********************/

#endif  /*LV_USE_GPU_STM32_DMA2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_STM32_DMA2D_H*/

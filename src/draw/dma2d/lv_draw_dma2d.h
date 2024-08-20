/**
 * @file lv_draw_dma2d.h
 *
 */

#ifndef LV_DRAW_DMA2D_H
#define LV_DRAW_DMA2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_DMA2D

/*********************
 *      DEFINES
 *********************/

#if LV_USE_DRAW_DMA2D_INTERRUPT && LV_USE_OS
#define LV_DRAW_DMA2D_ASYNC 1
#else
#define LV_DRAW_DMA2D_ASYNC 0
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_dma2d_init(void);
void lv_draw_dma2d_deinit(void);

#if LV_USE_DRAW_DMA2D_INTERRUPT
void lv_draw_dma2d_transfer_complete_interrupt_handler(void);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_DMA2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DMA2D_H*/

/**
 * @file lv_draw_esp_dma2d.h
 *
 */

#ifndef LV_DRAW_ESP_DMA2D_H
#define LV_DRAW_ESP_DMA2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"

#if LV_USE_ESP_DMA2D

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_esp_dma2d_init(void);
void lv_draw_esp_dma2d_deinit(void);

#endif /* LV_USE_ESP_DMA2D */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_ESP_DMA2D_H*/

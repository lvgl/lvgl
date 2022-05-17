/**
 * @file lv_drv_stm32_fb.h
 *
 */

#ifndef LV_DRV_STM32_FB_H
#define LV_DRV_STM32_FB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_drv.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_drv_t base;
    lv_color_t * frame_buffer;
    lv_area_t area;
    uint8_t index;
    void * ltdc_handler;	/*LTDC_HandleTypeDef*/
} lv_drv_stm32_fb_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_drv_stm32_ltdc_init(lv_drv_stm32_ltdc_t * drv);

void lv_drv_stm32_ltdc_create(lv_drv_stm32_ltdc_t * drv);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRV_ESP_H*/

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
    uint16_t width;
    uint16_t height;
    uint16_t hsync;
    uint16_t hbp;
    uint16_t hfp;
    uint16_t vsync;
    uint16_t vbp;
    uint16_t vfp;
    uint16_t hs_polarity : 1;
    uint16_t vs_polarity : 1;
    uint16_t de_polarity : 1;
    uint16_t pc_polarity : 1;
    void * ltdc;
} lv_drv_stm32_ltdc_t;


typedef struct {
    lv_color_t * frame_buffer;
    lv_area_t area;
    uint8_t index;
    lv_drv_stm32_ltdc_t * ltdc_drv;
} lv_drv_stm32_layer_t;

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

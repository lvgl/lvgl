/**
 * @file lv_esp_tick.h
 *
 */

#ifndef LV_ESP_TICK_H
#define LV_ESP_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../disp/lv_disp.h"

#if LV_USE_ESP_TICK

#include "esp_timer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create tick timer for LVGL using esp_timer to generate 2ms periodic event
 * @return          the created timer
 */
esp_timer_handle_t lv_esp_tick_init(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ESP_LCD */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_ESP_TICK_H */

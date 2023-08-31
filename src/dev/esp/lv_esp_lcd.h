/**
 * @file lv_esp_lcd.h
 *
 */

#ifndef LV_ESP_LCD_H
#define LV_ESP_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../disp/lv_disp.h"

#if LV_USE_ESP_LCD

//#include "esp_lcd_panel_io.h"
//#include "esp_lcd_panel_vendor.h"

#include "c:\Espressif\frameworks\esp-idf\components\esp_lcd\include\esp_lcd_panel_io.h"
#include "c:\Espressif\frameworks\esp-idf\components\esp_lcd\include\esp_lcd_panel_vendor.h"

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
 * Create LCD driver
 * @param parent    pointer to a calendar object.
 * @return          the created header
 */
lv_disp_t * lv_esp_lcd_create(uint32_t hor_res, uint32_t ver_res, esp_lcd_panel_handle_t panel_handle);

/**
 * Create a calendar header with drop-drowns to select the year and month
 * @param parent    pointer to a calendar object.
 * @return          the created header
 */
bool lv_esp_lcd_notify_color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t * edata,
                                        void * user_ctx);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_ESP_LCD */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_ESP_LCD_H */

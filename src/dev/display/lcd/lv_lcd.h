/*
 * lv_lcd_driver.h
 *
 */

#ifndef LV_LCD_DRIVER_H
#define LV_LCD_DRIVER_H

/*********************
 *      INCLUDES
 *********************/

#include "src/display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_lcd_send_cmd_cb_t)(uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);
typedef void (*lv_lcd_delay_cb_t)(uint32_t delay);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 * 		OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_LCD_DRIVER_H */

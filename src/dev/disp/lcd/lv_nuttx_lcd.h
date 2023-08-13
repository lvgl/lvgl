/**
 * @file lv_nuttx_lcd.h
 *
 */

#ifndef LV_NUTTX_LCD_H
#define LV_NUTTX_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../disp/lv_disp.h"

#if LV_USE_NUTTX_LCD

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_disp_t * lv_nuttx_lcd_create(const char * dev_path);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX_LCD */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_NUTTX_LCD_H */

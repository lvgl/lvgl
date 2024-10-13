/**
 * @file lv_ili9341.h
 *
 * This driver is just a wrapper around the generic MIPI compatible LCD controller driver
 *
 */

#ifndef LV_ILI9341_H
#define LV_ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lcd/lv_lcd_generic_mipi.h"

#if LV_USE_ILI9341

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_lcd_send_cmd_cb_t lv_ili9341_send_cmd_cb_t;
typedef lv_lcd_send_color_cb_t lv_ili9341_send_color_cb_t;

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Create an LCD display with ILI9341 driver
 * @param hor_res       horizontal resolution
 * @param ver_res       vertical resolution
 * @param flags         default configuration settings (mirror, RGB ordering, etc.)
 * @param send_cmd      platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color    platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must implement a 'ready' callback)
 * @return              pointer to the created display
 */
lv_display_t * lv_ili9341_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_flag_t flags,
                                 lv_ili9341_send_cmd_cb_t send_cmd_cb, lv_ili9341_send_color_cb_t send_color_cb);

/**
 * Set gap, i.e., the offset of the (0,0) pixel in the VRAM
 * @param disp          display object
 * @param x             x offset
 * @param y             y offset
 */
void lv_ili9341_set_gap(lv_display_t * disp, uint16_t x, uint16_t y);

/**
 * Set color inversion
 * @param disp          display object
 * @param invert        false: normal, true: invert
 */
void lv_ili9341_set_invert(lv_display_t * disp, bool invert);

/**
 * Set gamma curve
 * @param disp          display object
 * @param gamma         gamma curve
 */
void lv_ili9341_set_gamma_curve(lv_display_t * disp, uint8_t gamma);

/**
 * Send list of commands.
 * @param disp          display object
 * @param cmd_list      controller and panel-specific commands
 */
void lv_ili9341_send_cmd_list(lv_display_t * disp, const uint8_t * cmd_list);

/**********************
 *      OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_ILI9341*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ILI9341_H*/

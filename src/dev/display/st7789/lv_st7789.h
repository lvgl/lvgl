/*
 * lv_st7789.h
 *
 */

#ifndef LV_ST7789_H
#define LV_ST7789_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "src/display/lv_display.h"
#include "../lcd/lv_lcd_panel_commands.h"	/* common LCD controller commands */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Prototype of a platform-dependent callback to transfer commands and data to the LCD controller.
 * @param disp			display object
 * @param cmd			command buffer (can handle 16 bit commands as well)
 * @param cmd_size		number of bytes of the command
 * @param param			parameter buffer
 * @param param_size	number of bytes of the parameters
 */
typedef void (*lv_st7789_send_cmd_cb_t)(lv_display_t *disp, uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**
 * Create an LCD display with ST7789 driver
 * @param hor_res		horizontal resolution
 * @param ver_res		vertical resolution
 * @param send_cmd		platform-dependent function to send a command to the LCD controller (usually uses polling transfer)
 * @param send_color	platform-dependent function to send pixel data to the LCD controller (usually uses DMA transfer: must implement a 'ready' callback)
 * @return          	pointer to the created display
 */
lv_display_t *lv_st7789_create(uint32_t hor_res, uint32_t ver_res, lv_st7789_send_cmd_cb_t send_cmd, lv_st7789_send_cmd_cb_t send_color);

/**
 * Set BGR color ordering
 * @param disp			display object
 * @param bgr			false: RGB ordering, true: BGR ordering
 */
void lv_st7789_set_bgr(lv_display_t *disp, bool bgr);

/**
 * Set gap, i.e., the offset of the (0,0) pixel in the VRAM
 * @param disp			display object
 * @param x				x offset
 * @param y				y offset
 */
void lv_st7789_set_gap(lv_display_t *disp, uint16_t x, uint16_t y);

/**
 * Set color inversion
 * @param disp			display object
 * @param invert		false: normal, true: invert colors
 */
void lv_st7789_set_invert(lv_display_t *disp, bool invert);

/**********************
 * 		OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_ST7789_H */

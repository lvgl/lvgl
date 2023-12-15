/*
 * lv_lcd_st7789.h
 *
 */

#ifndef LV_LCD_ST7789_H
#define LV_LCD_ST7789_H

/*********************
 *      INCLUDES
 *********************/

#include "lv_lcd.h"
#include "lv_lcd_panel_commands.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	lv_display_t			*disp;		/* the LVGL display object */
	uint16_t				hor_res;	/* horizontal resolution */
	uint16_t				ver_res;	/* vertical resolution */
	uint8_t					bpp;		/* bytes per pixel */
#ifdef USE_FLAG_AS_SEMAPHORE
	bool					transfer_in_progress;	/* NOTE: could be a proper semaphore */
#endif
	lv_lcd_send_cmd_cb_t	send_cmd;	/* platform-specific implementation to send a command to the LCD controller */
	lv_lcd_send_cmd_cb_t	send_color;	/* platform-specific implementation to send pixel data to the LCD controller */
    uint16_t				x_gap;
    uint16_t				y_gap;
    uint8_t 				madctl_val;	/* save current value of LCD_CMD_MADCTL register */
    uint8_t 				colmod_cal;	/* save surrent value of LCD_CMD_COLMOD register */
} lv_st7789_driver_t;


/**********************
 *  GLOBAL PROTOTYPES
 **********************/

lv_display_t *lv_st7789_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_send_cmd_cb_t send_cmd, lv_lcd_send_cmd_cb_t send_color);
lv_st7789_driver_t *lv_st7789_get_driver(lv_display_t *disp);
void lv_st7789_send_cmd(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size);
void lv_st7789_send_color(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size);

void lv_st7789_init(lv_st7789_driver_t * drv);
void lv_st7789_invert(lv_st7789_driver_t * drv, bool invert);
void lv_st7789_onoff(lv_st7789_driver_t * drv, bool on);
void lv_st7789_mirror_x(lv_st7789_driver_t * drv, bool mirror);
void lv_st7789_mirror_y(lv_st7789_driver_t * drv, bool mirror);
void lv_st7789_swap_xy(lv_st7789_driver_t * drv, bool swap);

/**********************
 * 		OTHERS
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_LCD_ST7789_H */

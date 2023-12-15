/**
 * @file lv_st7789.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_st7789.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	lv_display_t			*disp;			/* the associated LVGL display object */
	lv_st7789_send_cmd_cb_t	send_cmd;		/* platform-specific implementation to send a command to the LCD controller */
	lv_st7789_send_cmd_cb_t	send_color;		/* platform-specific implementation to send pixel data to the LCD controller */
    uint16_t				x_gap;			/* x offset of the (0,0) pixel in VRAM */
    uint16_t				y_gap;			/* y offset of the (0,0) pixel in VRAM */
    uint8_t 				madctl_reg;		/* current value of MADCTL register */
    uint8_t 				colmod_reg;		/* current value of COLMOD register */
} lv_st7789_driver_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void send_cmd(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size);
static void send_color(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size);
static void init(lv_st7789_driver_t * drv);
static void set_mirror(lv_st7789_driver_t * drv, bool mirror_x, bool mirror_y);
static void set_swap_xy(lv_st7789_driver_t * drv, bool swap);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void res_chg_event_cb(lv_event_t * e);
static lv_st7789_driver_t *lv_st7789_get_driver(lv_display_t *disp);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t *lv_st7789_create(uint32_t hor_res, uint32_t ver_res, lv_st7789_send_cmd_cb_t send_cmd_cb, lv_st7789_send_cmd_cb_t send_color_cb)
{
	lv_display_t * disp = lv_display_create(hor_res, ver_res);
	if (disp == NULL) {
		return NULL;
	}

	lv_st7789_driver_t *drv = (lv_st7789_driver_t *)lv_malloc(sizeof(lv_st7789_driver_t));
    if(drv == NULL) {
    	lv_display_delete(disp);
    	return NULL;
    }

    /* init driver struct */
	drv->disp = disp;
	drv->send_cmd = send_cmd_cb;
	drv->send_color = send_color_cb;
    lv_display_set_driver_data(disp, (void*)drv);

	/* init controller */
	init(drv);

    /* register resolution change callback (NOTE: this handles screen rotation as well) */
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /* register flush callback */
    lv_display_set_flush_cb(disp, flush_cb);

    return disp;
}

void lv_st7789_set_bgr(lv_display_t *disp, bool bgr)
{
    lv_st7789_driver_t *drv = lv_st7789_get_driver(disp);
    if (bgr) {
    	drv->madctl_reg |= LCD_CMD_BGR_BIT;
    } else {
    	drv->madctl_reg &= ~LCD_CMD_BGR_BIT;
    }
    send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_reg,
    	}, 1);
}

void lv_st7789_set_gap(lv_display_t *disp, uint16_t x, uint16_t y)
{
    lv_st7789_driver_t *drv = lv_st7789_get_driver(disp);
    drv->x_gap = x;
    drv->y_gap = y;
}

void lv_st7789_set_invert(lv_display_t *disp, bool invert)
{
    lv_st7789_driver_t *drv = lv_st7789_get_driver(disp);
    send_cmd(drv, invert ? LCD_CMD_INVON : LCD_CMD_INVOFF, NULL, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Helper function to call the user-supplied 'send_cmd' function
 * @param drv			LCD driver object
 * @param cmd			command byte
 * @param param			parameter buffer
 * @param param_size	number of bytes of the parameters
 */
static void send_cmd(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size)
{
    uint8_t cmdbuf = cmd;		/* the ST7789 uses 8 bit commands */
    drv->send_cmd(drv->disp, &cmdbuf, 1, param, param_size);
}

/**
 * Helper function to call the user-supplied 'send_color' function
 * @param drv			LCD driver object
 * @param cmd			command byte
 * @param param			parameter buffer
 * @param param_size	number of bytes of the parameters
 */
static void send_color(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size)
{
    uint8_t cmdbuf = cmd;		/* the ST7789 uses 8 bit commands */
    drv->send_color(drv->disp, &cmdbuf, 1, param, param_size);
}

/**
 * Initialize LCD driver after a hard reset
 * @param drv			LCD driver object
 */
static void init(lv_st7789_driver_t * drv)
{
	drv->x_gap = 0;
	drv->y_gap = 0;

    drv->madctl_reg = 0;
    drv->colmod_reg = 0x55;	/* RGB565 */
    /* enter sleep mode first */
    send_cmd(drv, LCD_CMD_SLPIN, NULL, 0);
    lv_delay_ms(10);

    /* perform software reset */
    send_cmd(drv, LCD_CMD_SWRESET, NULL, 0);
    lv_delay_ms(200);

    /* LCD goes into sleep mode and display will be turned off after power on reset, exit sleep mode first */
    send_cmd(drv, LCD_CMD_SLPOUT, NULL, 0);
    lv_delay_ms(300);

    send_cmd(drv, LCD_CMD_NORON, NULL, 0);

    send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_reg,
    	}, 1);
    send_cmd(drv, LCD_CMD_COLMOD, (uint8_t[]) {
        drv->colmod_reg,
    	}, 1);
    send_cmd(drv, LCD_CMD_DISPON, NULL, 0);
}

/**
 * Set readout directions (used for rotating the display)
 * @param drv			LCD driver object
 * @param mirror_x		false: normal, true: mirrored
 * @param mirror_y		false: normal, true: mirrored
 */
static void set_mirror(lv_st7789_driver_t * drv, bool mirror_x, bool mirror_y)
{
    if (mirror_x) {
        drv->madctl_reg |= LCD_CMD_MX_BIT;
    } else {
        drv->madctl_reg &= ~LCD_CMD_MX_BIT;
    }
    if (mirror_y) {
        drv->madctl_reg |= LCD_CMD_MY_BIT;
    } else {
        drv->madctl_reg &= ~LCD_CMD_MY_BIT;
    }
    send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_reg
    	}, 1);
}

/**
 * Swap horizontal and vertical readout (used for rotating the display)
 * @param drv			LCD driver object
 * @param swap			false: normal, true: swapped
 */
static void set_swap_xy(lv_st7789_driver_t * drv, bool swap)
{
	if (swap) {
		drv->madctl_reg |= LCD_CMD_MV_BIT;
	} else {
		drv->madctl_reg &= ~LCD_CMD_MV_BIT;
	}
	send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
		drv->madctl_reg
		}, 1);
}

#if 0	// left here for reference
void lv_st7789_ioctl(lv_st7789_driver_t *drv, lv_lcd_driver_ioctl_cmd_t cmd, const uintptr_t param, size_t param_size)
{
	switch (cmd) {
	case LV_LCD_INIT:
		init(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_INVERT:
		lv_st7789_set_invert(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_MIRROR:
		mirror(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_SETOFFS:
		drv->x_gap = (lv_lcd_flag_t)param & 0xffff;
		drv->x_gap = ((lv_lcd_flag_t)param >> 16) & 0xffff;
		break;
	case LV_LCD_ONOFF:
		lv_st7789_onoff(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_SETRES:
		break;
	default:
		break;
	}
}
#endif


/**
 * Flush display buffer to the LCD
 * @param disp			display object
 * @param hor_res		horizontal resolution
 * @param area			area stored in the buffer
 * @param px_map		buffer containing pixel data
 * @note  				transfers pixel data to the LCD controller using the callbacks 'send_cmd' and 'send_color', which were
 *        				passed to the 'lv_st7789_create()' function
 */
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_st7789_driver_t *drv = lv_st7789_get_driver(disp);

    int32_t x_start = area->x1;
    int32_t x_end = area->x2 + 1;
    int32_t y_start = area->y1;
    int32_t y_end = area->y2 + 1;

    LV_ASSERT((x_start < x_end) && (y_start < y_end) && "start position must be smaller than end position");

    x_start += drv->x_gap;
    x_end += drv->x_gap;
    y_start += drv->y_gap;
    y_end += drv->y_gap;

    /* define an area of frame memory where MCU can access */
    send_cmd(drv, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    send_cmd(drv, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    /* transfer frame buffer */
    size_t len = (x_end - x_start) * (y_end - y_start) * lv_color_format_get_size(lv_display_get_color_format(disp));
    send_color(drv, LCD_CMD_RAMWR, px_map, len);
}


/**
 * Handle LV_EVENT_RESOLUTION_CHANGED event (handles both resolution and rotation change)
 * @param e				LV_EVENT_RESOLUTION_CHANGED event
 */
static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_st7789_driver_t *drv = lv_st7789_get_driver(disp);

    uint16_t hor_res = lv_display_get_horizontal_resolution(disp);
    uint16_t ver_res = lv_display_get_vertical_resolution(disp);
    uint16_t rot = lv_display_get_rotation(disp);

    /* TODO: implement resolution change */
    LV_UNUSED(hor_res);
    LV_UNUSED(ver_res);

    /* handle rotation */
    switch (rot) {
	case LV_DISPLAY_ROTATION_0:
		set_swap_xy(drv, false);
		set_mirror(drv, false, false);
		break;
	case LV_DISPLAY_ROTATION_90:
		set_swap_xy(drv, true);
		set_mirror(drv, true, false);
		break;
	case LV_DISPLAY_ROTATION_180:
		set_swap_xy(drv, false);
		set_mirror(drv, true, true);
		break;
	case LV_DISPLAY_ROTATION_270:
		set_swap_xy(drv, true);
		set_mirror(drv, false, true);
		break;
    }

}

static inline lv_st7789_driver_t *lv_st7789_get_driver(lv_display_t *disp)
{
	return (lv_st7789_driver_t *)lv_display_get_driver_data(disp);
}


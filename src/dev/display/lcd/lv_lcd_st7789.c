#include <src/dev/display/lcd/lv_lcd_st7789.h>

extern void HAL_Delay(uint32_t Delay);

static void lv_delay(uint32_t delay)
{
#if 0
	uint32_t time;
	time = lv_tick_get();
	while (lv_tick_elaps(time) < delay)
	{
	}
#else
	HAL_Delay(delay);
#endif
}

lv_st7789_driver_t *lv_st7789_get_driver(lv_display_t *disp)
{
	return (lv_st7789_driver_t *)lv_display_get_driver_data(disp);
}

/* This helper function calls the user-supplied 'send_cmd' function */
void lv_st7789_send_cmd(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size)
{
    /* NOTE: this should use a proper semaphore */
#ifdef USE_FLAG_AS_SEMAPHORE
    do {
    } while (base->transfer_in_progress);
#endif
    uint8_t cmdbuf = cmd;		// the ST7789 uses 8 bit commands
    drv->send_cmd(&cmdbuf, 1, param, param_size);
}

/* This helper function calls the user-supplied 'send_color' function */
void lv_st7789_send_color(lv_st7789_driver_t *drv, uint8_t cmd, uint8_t *param, size_t param_size)
{
    /* NOTE: this should use a proper semaphore */
#ifdef USE_FLAG_AS_SEMAPHORE
    do {
    } while (base->transfer_in_progress);
#endif
    uint8_t cmdbuf = cmd;		// the ST7789 uses 8 bit commands
    drv->send_color(&cmdbuf, 1, param, param_size);
}


void lv_st7789_init(lv_st7789_driver_t * drv)
{
	drv->x_gap = 0;
	drv->y_gap = 0;

    drv->madctl_val = 0;
#if 0
    if (flags & LV_LCD_FLAG_BGR) {
        drv->madctl_val |= LCD_CMD_BGR_BIT;
    }
	if (flags & LV_LCD_FLAG_SWAPXY) {
		drv->madctl_val |= LCD_CMD_MV_BIT;
	}
	if (flags & LV_LCD_FLAG_SWAPXY) {
		drv->madctl_val |= LCD_CMD_MV_BIT;
	}
    base->bpp = 2;
    drv->colmod_cal = 0x55;	/* RGB565 */
    if (flags & LV_LCD_FLAG_RGB666) {
        drv->colmod_cal = 0x66;
        /* each color component (R/G/B) should occupy the 6 high bits of a byte, which means 3 full bytes are required for a pixel */
        base->bpp = 3;
    }
#else
    drv->colmod_cal = 0x55;	/* RGB565 */
#endif
    /* enter sleep mode first */
    lv_st7789_send_cmd(drv, LCD_CMD_SLPIN, NULL, 0);
    lv_delay(10);

    /* perform software reset */
    lv_st7789_send_cmd(drv, LCD_CMD_SWRESET, NULL, 0);
    lv_delay(200);

    /* LCD goes into sleep mode and display will be turned off after power on reset, exit sleep mode first */
    lv_st7789_send_cmd(drv, LCD_CMD_SLPOUT, NULL, 0);
    lv_delay(300);

    lv_st7789_send_cmd(drv, LCD_CMD_NORON, NULL, 0);

    lv_st7789_send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_val,
    	}, 1);
    lv_st7789_send_cmd(drv, LCD_CMD_COLMOD, (uint8_t[]) {
        drv->colmod_cal,
    	}, 1);
    lv_st7789_send_cmd(drv, LCD_CMD_DISPON, NULL, 0);
}

void lv_st7789_invert(lv_st7789_driver_t * drv, bool invert)
{
    lv_st7789_send_cmd(drv, invert ? LCD_CMD_INVON : LCD_CMD_INVOFF, NULL, 0);
}

void lv_st7789_onoff(lv_st7789_driver_t * drv, bool on)
{
    lv_st7789_send_cmd(drv, on ? LCD_CMD_DISPON : LCD_CMD_DISPOFF, NULL, 0);
}

void lv_st7789_mirror_x(lv_st7789_driver_t * drv, bool mirror)
{
    if (mirror) {
        drv->madctl_val |= LCD_CMD_MX_BIT;
    } else {
        drv->madctl_val &= ~LCD_CMD_MX_BIT;
    }
    lv_st7789_send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_val
    	}, 1);
}

void lv_st7789_mirror_y(lv_st7789_driver_t * drv, bool mirror)
{
    if (mirror) {
        drv->madctl_val |= LCD_CMD_MY_BIT;
    } else {
        drv->madctl_val &= ~LCD_CMD_MY_BIT;
    }
    lv_st7789_send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
        drv->madctl_val
    	}, 1);
}

void lv_st7789_swap_xy(lv_st7789_driver_t * drv, bool swap)
{
	if (swap) {
		drv->madctl_val |= LCD_CMD_MV_BIT;
	} else {
		drv->madctl_val &= ~LCD_CMD_MV_BIT;
	}
	lv_st7789_send_cmd(drv, LCD_CMD_MADCTL, (uint8_t[]) {
		drv->madctl_val
		}, 1);
}

#if 0
void lv_st7789_ioctl(lv_st7789_driver_t *drv, lv_lcd_driver_ioctl_cmd_t cmd, const uintptr_t param, size_t param_size)
{
	switch (cmd) {
	case LV_LCD_INIT:
		drv_init(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_INVERT:
		lv_st7789_invert(drv, (lv_lcd_flag_t)param);
		break;
	case LV_LCD_MIRROR:
		lv_st7789_mirror(drv, (lv_lcd_flag_t)param);
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


void lv_st7789_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
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
    lv_st7789_send_cmd(drv, LCD_CMD_CASET, (uint8_t[]) {
        (x_start >> 8) & 0xFF,
        x_start & 0xFF,
        ((x_end - 1) >> 8) & 0xFF,
        (x_end - 1) & 0xFF,
    }, 4);
    lv_st7789_send_cmd(drv, LCD_CMD_RASET, (uint8_t[]) {
        (y_start >> 8) & 0xFF,
        y_start & 0xFF,
        ((y_end - 1) >> 8) & 0xFF,
        (y_end - 1) & 0xFF,
    }, 4);
    /* transfer frame buffer */
    size_t len = (x_end - x_start) * (y_end - y_start) * drv->bpp;
    lv_st7789_send_color(drv, LCD_CMD_RAMWR, px_map, len);
}


void lv_st7789_res_chg_event_cb(lv_event_t * e)
{
#if 0
    lv_display_t * disp = lv_event_get_target(e);
    lv_lcd_driver_t *drv = lv_lcd_display_get_driver(disp);

    uint16_t hor_res = lv_display_get_horizontal_resolution(disp);
    uint16_t ver_res = lv_display_get_vertical_resolution(disp);

    lv_st7789_set_resolution(drv, LV_LCD_SETRES, (hor_res << 16) | ver_res, 0);
#endif
}

lv_display_t *lv_st7789_create(uint32_t hor_res, uint32_t ver_res, lv_lcd_send_cmd_cb_t send_cmd, lv_lcd_send_cmd_cb_t send_color)
{
	lv_display_t * disp = lv_display_create(hor_res, ver_res);
	if (disp == NULL) {
		return NULL;
	}

	lv_st7789_driver_t *drv = (lv_st7789_driver_t *)lv_malloc(sizeof(lv_st7789_driver_t));
    if(drv == NULL) {
    	lv_display_remove(disp);
    	return NULL;
    }

    /* init driver struct */
	drv->disp = disp;
	drv->hor_res = hor_res;
	drv->ver_res = ver_res;
	drv->bpp = 2;			/* default color format is RGB565 */
	drv->send_cmd = send_cmd;
	drv->send_color = send_color;
    lv_display_set_driver_data(disp, (void*)drv);

	/* init controller */
	lv_st7789_init(drv);

    /* register resolution change callback (NOTE: this handles screen rotation as well) */
    lv_display_add_event(disp, lv_st7789_res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /* register flush callback */
    lv_display_set_flush_cb(disp, lv_st7789_flush_cb);

    return disp;
}


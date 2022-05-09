/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <lvgl.h>
#include "lvgl_display.h"

void lvgl_flush_cb_32bit(lv_disp_drv_t *disp_drv,
		const lv_area_t *area, lv_color_t *color_p)
{
	const struct device *display_dev = (const struct device *)disp_drv->user_data;
	uint16_t w = area->x2 - area->x1 + 1;
	uint16_t h = area->y2 - area->y1 + 1;
	struct display_buffer_descriptor desc;

	desc.buf_size = w * 4U * h;
	desc.width = w;
	desc.pitch = w;
	desc.height = h;
	display_write(display_dev, area->x1, area->y1, &desc, (void *) color_p);

	lv_disp_flush_ready(disp_drv);
}

#ifndef CONFIG_LV_COLOR_DEPTH_32
void lvgl_set_px_cb_32bit(lv_disp_drv_t *disp_drv,
		uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
		lv_color_t color, lv_opa_t opa)
{
	uint32_t *buf_xy = (uint32_t *)(buf + x * 4U + y * 4U * buf_w);

	if (opa == LV_OPA_COVER) {
		/* Do not mix if not required */
		*buf_xy = lv_color_to32(color);
	} else {
		lv_color_t bg_color = *((lv_color_t *)buf_xy);
		*buf_xy = lv_color_to32(lv_color_mix(color, bg_color, opa));
	}
}
#endif

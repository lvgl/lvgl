/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <lvgl.h>
#include "lvgl_display.h"

void lvgl_flush_cb_24bit(lv_disp_drv_t *disp_drv,
		const lv_area_t *area, lv_color_t *color_p)
{
	struct lvgl_disp_data *data =
		(struct lvgl_disp_data *)disp_drv->user_data;
	uint16_t w = area->x2 - area->x1 + 1;
	uint16_t h = area->y2 - area->y1 + 1;
	struct display_buffer_descriptor desc;

	desc.buf_size = w * 3U * h;
	desc.width = w;
	desc.pitch = w;
	desc.height = h;
	display_write(data->display_dev, area->x1, area->y1, &desc,
		      (void *) color_p);

	lv_disp_flush_ready(disp_drv);
}

void lvgl_set_px_cb_24bit(lv_disp_drv_t *disp_drv,
		uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
		lv_color_t color, lv_opa_t opa)
{
	uint8_t *buf_xy = buf + x * 3U + y * 3U * buf_w;
	lv_color32_t converted_color;

#ifdef CONFIG_LV_COLOR_DEPTH_32
	if (opa != LV_OPA_COVER) {
		lv_color_t mix_color;

		mix_color.ch.red = *buf_xy;
		mix_color.ch.green = *(buf_xy+1);
		mix_color.ch.blue = *(buf_xy+2);
		color = lv_color_mix(color, mix_color, opa);
	}
#endif

	converted_color.full = lv_color_to32(color);
	*buf_xy = converted_color.ch.red;
	*(buf_xy + 1) = converted_color.ch.green;
	*(buf_xy + 2) = converted_color.ch.blue;
}

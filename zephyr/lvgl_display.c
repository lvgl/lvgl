/*
 * Copyright (c) 2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "lvgl_display.h"

int set_lvgl_rendering_cb(lv_disp_drv_t *disp_drv)
{
	int err = 0;
	struct lvgl_disp_data *data =
		(struct lvgl_disp_data *)disp_drv->user_data;

	switch (data->cap.current_pixel_format) {
	case PIXEL_FORMAT_ARGB_8888:
		disp_drv->flush_cb = lvgl_flush_cb_32bit;
		disp_drv->rounder_cb = NULL;
#ifdef CONFIG_LV_COLOR_DEPTH_32
		disp_drv->set_px_cb = NULL;
#else
		disp_drv->set_px_cb = lvgl_set_px_cb_32bit;
#endif
		break;
	case PIXEL_FORMAT_RGB_888:
		disp_drv->flush_cb = lvgl_flush_cb_24bit;
		disp_drv->rounder_cb = NULL;
		disp_drv->set_px_cb = lvgl_set_px_cb_24bit;
		break;
	case PIXEL_FORMAT_RGB_565:
	case PIXEL_FORMAT_BGR_565:
		disp_drv->flush_cb = lvgl_flush_cb_16bit;
		disp_drv->rounder_cb = NULL;
#ifdef CONFIG_LV_COLOR_DEPTH_16
		disp_drv->set_px_cb = NULL;
#else
		disp_drv->set_px_cb = lvgl_set_px_cb_16bit;
#endif
		break;
	case PIXEL_FORMAT_MONO01:
	case PIXEL_FORMAT_MONO10:
		disp_drv->flush_cb = lvgl_flush_cb_mono;
		disp_drv->rounder_cb = lvgl_rounder_cb_mono;
		disp_drv->set_px_cb = lvgl_set_px_cb_mono;
		break;
	default:
		disp_drv->flush_cb = NULL;
		disp_drv->rounder_cb = NULL;
		disp_drv->set_px_cb = NULL;
		err = -ENOTSUP;
		break;

	}

	return err;
}

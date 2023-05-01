/*
 * Copyright (c) 2018-2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include "lvgl_display.h"
#ifdef CONFIG_LV_Z_USE_FILESYSTEM
#include "lvgl_fs.h"
#endif
#ifdef CONFIG_LV_Z_POINTER_KSCAN
#include <zephyr/drivers/kscan.h>
#endif
#include LV_MEM_CUSTOM_INCLUDE

#define LOG_LEVEL CONFIG_LV_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(lvgl);

static lv_disp_drv_t disp_drv;
struct lvgl_disp_data disp_data = {
	.blanking_on = false,
};
#ifdef CONFIG_LV_Z_POINTER_KSCAN
static lv_indev_drv_t indev_drv;
#endif /* CONFIG_LV_Z_POINTER_KSCAN */

#define DISPLAY_NODE DT_CHOSEN(zephyr_display)
#define KSCAN_NODE DT_CHOSEN(zephyr_keyboard_scan)

#ifdef CONFIG_LV_Z_BUFFER_ALLOC_STATIC

static lv_disp_draw_buf_t disp_buf;

#define DISPLAY_WIDTH DT_PROP(DISPLAY_NODE, width)
#define DISPLAY_HEIGHT DT_PROP(DISPLAY_NODE, height)

#define BUFFER_SIZE (CONFIG_LV_Z_BITS_PER_PIXEL * ((CONFIG_LV_Z_VDB_SIZE * \
			DISPLAY_WIDTH * DISPLAY_HEIGHT) / 100) / 8)

#define NBR_PIXELS_IN_BUFFER (BUFFER_SIZE * 8 / CONFIG_LV_Z_BITS_PER_PIXEL)

/* NOTE: depending on chosen color depth buffer may be accessed using uint8_t *,
 * uint16_t * or uint32_t *, therefore buffer needs to be aligned accordingly to
 * prevent unaligned memory accesses.
 */
static uint8_t buf0[BUFFER_SIZE]
#ifdef CONFIG_LV_Z_VBD_CUSTOM_SECTION
	Z_GENERIC_SECTION(.lvgl_buf)
#endif
	__aligned(CONFIG_LV_Z_VDB_ALIGN);

#ifdef CONFIG_LV_Z_DOUBLE_VDB
static uint8_t buf1[BUFFER_SIZE]
#ifdef CONFIG_LV_Z_VBD_CUSTOM_SECTION
	Z_GENERIC_SECTION(.lvgl_buf)
#endif
	__aligned(CONFIG_LV_Z_VDB_ALIGN);
#endif /* CONFIG_LV_Z_DOUBLE_VDB */

#endif /* CONFIG_LV_Z_BUFFER_ALLOC_STATIC */

#if CONFIG_LV_LOG_LEVEL != 0
/*
 * In LVGLv8 the signature of the logging callback has changes and it no longer
 * takes the log level as an integer argument. Instead, the log level is now
 * already part of the buffer passed to the logging callback. It's not optimal
 * but we need to live with it and parse the buffer manually to determine the
 * level and then truncate the string we actually pass to the logging framework.
 */
static void lvgl_log(const char *buf)
{
	/*
	 * This is ugly and should be done in a loop or something but as it
	 * turned out, Z_LOG()'s first argument (that specifies the log level)
	 * cannot be an l-value...
	 *
	 * We also assume lvgl is sane and always supplies the level string.
	 */
	switch (buf[1]) {
	case 'E':
		LOG_ERR("%s", buf + strlen("[Error] "));
		break;
	case 'W':
		LOG_WRN("%s", buf + strlen("Warn] "));
		break;
	case 'I':
		LOG_INF("%s", buf + strlen("[Info] "));
		break;
	case 'T':
		LOG_DBG("%s", buf + strlen("[Trace] "));
		break;
	}
}
#endif

#ifdef CONFIG_LV_Z_BUFFER_ALLOC_STATIC

static int lvgl_allocate_rendering_buffers(lv_disp_drv_t *disp_drv)
{
	struct lvgl_disp_data *data =
		(struct lvgl_disp_data *)disp_drv->user_data;
	int err = 0;

	if (data->cap.x_resolution <= DISPLAY_WIDTH) {
		disp_drv->hor_res = data->cap.x_resolution;
	} else {
		LOG_ERR("Horizontal resolution is larger than maximum");
		err = -ENOTSUP;
	}

	if (data->cap.y_resolution <= DISPLAY_HEIGHT) {
		disp_drv->ver_res = data->cap.y_resolution;
	} else {
		LOG_ERR("Vertical resolution is larger than maximum");
		err = -ENOTSUP;
	}

	disp_drv->draw_buf = &disp_buf;
#ifdef CONFIG_LV_Z_DOUBLE_VDB
	lv_disp_draw_buf_init(disp_drv->draw_buf, &buf0, &buf1, NBR_PIXELS_IN_BUFFER);
#else
	lv_disp_draw_buf_init(disp_drv->draw_buf, &buf0, NULL, NBR_PIXELS_IN_BUFFER);
#endif /* CONFIG_LV_Z_DOUBLE_VDB  */

	return err;
}

#else

static int lvgl_allocate_rendering_buffers(lv_disp_drv_t *disp_drv)
{
	void *buf0 = NULL;
	void *buf1 = NULL;
	uint16_t buf_nbr_pixels;
	uint32_t buf_size;
	struct lvgl_disp_data *data =
		(struct lvgl_disp_data *)disp_drv->user_data;

	disp_drv->hor_res = data->cap.x_resolution;
	disp_drv->ver_res = data->cap.y_resolution;

	buf_nbr_pixels = (CONFIG_LV_Z_VDB_SIZE * disp_drv->hor_res *
			disp_drv->ver_res) / 100;
	/* one horizontal line is the minimum buffer requirement for lvgl */
	if (buf_nbr_pixels < disp_drv->hor_res) {
		buf_nbr_pixels = disp_drv->hor_res;
	}

	switch (data->cap.current_pixel_format) {
	case PIXEL_FORMAT_ARGB_8888:
		buf_size = 4 * buf_nbr_pixels;
		break;
	case PIXEL_FORMAT_RGB_888:
		buf_size = 3 * buf_nbr_pixels;
		break;
	case PIXEL_FORMAT_RGB_565:
		buf_size = 2 * buf_nbr_pixels;
		break;
	case PIXEL_FORMAT_MONO01:
	case PIXEL_FORMAT_MONO10:
		buf_size = buf_nbr_pixels / 8;
		buf_size += (buf_nbr_pixels % 8) == 0 ? 0 : 1;
		break;
	default:
		return -ENOTSUP;
	}

	buf0 = LV_MEM_CUSTOM_ALLOC(buf_size);
	if (buf0 == NULL) {
		LOG_ERR("Failed to allocate memory for rendering buffer");
		return -ENOMEM;
	}

#ifdef CONFIG_LV_Z_DOUBLE_VDB
	buf1 = LV_MEM_CUSTOM_ALLOC(buf_size);
	if (buf1 == NULL) {
		LV_MEM_CUSTOM_FREE(buf0);
		LOG_ERR("Failed to allocate memory for rendering buffer");
		return -ENOMEM;
	}
#endif

	disp_drv->draw_buf = LV_MEM_CUSTOM_ALLOC(sizeof(lv_disp_draw_buf_t));
	if (disp_drv->draw_buf == NULL) {
		LV_MEM_CUSTOM_FREE(buf0);
		LV_MEM_CUSTOM_FREE(buf1);
		LOG_ERR("Failed to allocate memory to store rendering buffers");
		return -ENOMEM;
	}

	lv_disp_draw_buf_init(disp_drv->draw_buf, buf0, buf1, buf_nbr_pixels);
	return 0;
}
#endif /* CONFIG_LV_Z_BUFFER_ALLOC_STATIC */

#ifdef CONFIG_LV_Z_POINTER_KSCAN
K_MSGQ_DEFINE(kscan_msgq, sizeof(lv_indev_data_t),
	      CONFIG_LV_Z_POINTER_KSCAN_MSGQ_COUNT, 4);

static void lvgl_pointer_kscan_callback(const struct device *dev,
					uint32_t row,
					uint32_t col, bool pressed)
{
	lv_indev_data_t data = {
		.point.x = col,
		.point.y = row,
		.state = pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL,
	};

	if (k_msgq_put(&kscan_msgq, &data, K_NO_WAIT) != 0) {
		LOG_DBG("Could not put input data into queue");
	}
}

static void lvgl_pointer_kscan_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
	lv_disp_t *disp;
	struct lvgl_disp_data *disp_data;
	struct display_capabilities *cap;
	lv_indev_data_t curr;

	static lv_indev_data_t prev = {
		.point.x = 0,
		.point.y = 0,
		.state = LV_INDEV_STATE_REL,
	};

	if (k_msgq_get(&kscan_msgq, &curr, K_NO_WAIT) != 0) {
		goto set_and_release;
	}

	prev = curr;

	disp = lv_disp_get_default();
	disp_data = disp->driver->user_data;
	cap = &disp_data->cap;

	/* adjust kscan coordinates */
	if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN_SWAP_XY)) {
		lv_coord_t x;

		x = prev.point.x;
		prev.point.x = prev.point.y;
		prev.point.y = x;
	}

	if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN_INVERT_X)) {
		if (cap->current_orientation == DISPLAY_ORIENTATION_NORMAL ||
		    cap->current_orientation == DISPLAY_ORIENTATION_ROTATED_180) {
			prev.point.x = cap->x_resolution - prev.point.x;
		} else {
			prev.point.x = cap->y_resolution - prev.point.x;
		}
	}

	if (IS_ENABLED(CONFIG_LV_Z_POINTER_KSCAN_INVERT_Y)) {
		if (cap->current_orientation == DISPLAY_ORIENTATION_NORMAL ||
		    cap->current_orientation == DISPLAY_ORIENTATION_ROTATED_180) {
			prev.point.y = cap->y_resolution - prev.point.y;
		} else {
			prev.point.y = cap->x_resolution - prev.point.y;
		}
	}

	/* rotate touch point to match display rotation */
	if (cap->current_orientation == DISPLAY_ORIENTATION_ROTATED_90) {
		lv_coord_t x;

		x = prev.point.x;
		prev.point.x = prev.point.y;
		prev.point.y = cap->y_resolution - x;
	} else if (cap->current_orientation == DISPLAY_ORIENTATION_ROTATED_180) {
		prev.point.x = cap->x_resolution - prev.point.x;
		prev.point.y = cap->y_resolution - prev.point.y;
	} else if (cap->current_orientation == DISPLAY_ORIENTATION_ROTATED_270) {
		lv_coord_t x;

		x = prev.point.x;
		prev.point.x = cap->x_resolution - prev.point.y;
		prev.point.y = x;
	}

    /* filter readings within display */
    if (prev.point.x <= 0) {
        prev.point.x = 0;
    }
    else if (prev.point.x >= cap->x_resolution) {
        prev.point.x = cap->x_resolution - 1;
    }

    if (prev.point.y <= 0) {
        prev.point.y = 0;
    }
    else if (prev.point.y >= cap->y_resolution) {
        prev.point.y = cap->y_resolution - 1;
    }

set_and_release:
	*data = prev;

	data->continue_reading = k_msgq_num_used_get(&kscan_msgq) > 0;
}

static int lvgl_pointer_kscan_init(void)
{
	const struct device *kscan_dev = DEVICE_DT_GET(KSCAN_NODE);

	if (!device_is_ready(kscan_dev)) {
		LOG_ERR("Keyboard scan device not ready.");
		return -ENODEV;
	}

	if (kscan_config(kscan_dev, lvgl_pointer_kscan_callback) < 0) {
		LOG_ERR("Could not configure keyboard scan device.");
		return -ENODEV;
	}

	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = lvgl_pointer_kscan_read;

	if (lv_indev_drv_register(&indev_drv) == NULL) {
		LOG_ERR("Failed to register input device.");
		return -EPERM;
	}

	kscan_enable_callback(kscan_dev);

	return 0;
}
#endif /* CONFIG_LV_Z_POINTER_KSCAN */

static int lvgl_init(void)
{

	const struct device *display_dev = DEVICE_DT_GET(DISPLAY_NODE);

	int err = 0;

	if (!device_is_ready(display_dev)) {
		LOG_ERR("Display device not ready.");
		return -ENODEV;
	}

#if CONFIG_LV_LOG_LEVEL != 0
	lv_log_register_print_cb(lvgl_log);
#endif

	lv_init();

#ifdef CONFIG_LV_Z_USE_FILESYSTEM
	lvgl_fs_init();
#endif

	disp_data.display_dev = display_dev;
	display_get_capabilities(display_dev, &disp_data.cap);

	lv_disp_drv_init(&disp_drv);
	disp_drv.user_data = (void *)&disp_data;

#ifdef CONFIG_LV_Z_FULL_REFRESH
	disp_drv.full_refresh = 1;
#endif

	err = lvgl_allocate_rendering_buffers(&disp_drv);
	if (err != 0) {
		return err;
	}

	if (set_lvgl_rendering_cb(&disp_drv) != 0) {
		LOG_ERR("Display not supported.");
		return -ENOTSUP;
	}

	if (lv_disp_drv_register(&disp_drv) == NULL) {
		LOG_ERR("Failed to register display device.");
		return -EPERM;
	}

#ifdef CONFIG_LV_Z_POINTER_KSCAN
	lvgl_pointer_kscan_init();
#endif /* CONFIG_LV_Z_POINTER_KSCAN */

	return 0;
}

SYS_INIT(lvgl_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

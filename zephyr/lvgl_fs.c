/*
 * Copyright (c) 2018-2019 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <lvgl.h>
#include <zephyr.h>
#include <fs/fs.h>
#include <stdlib.h>
#include "lvgl_fs.h"

static bool lvgl_fs_ready(struct _lv_fs_drv_t *drv)
{
	return true;
}

static lv_fs_res_t errno_to_lv_fs_res(int err)
{
	switch (err) {
	case 0:
		return LV_FS_RES_OK;
	case -EIO:
		/*Low level hardware error*/
		return LV_FS_RES_HW_ERR;
	case -EBADF:
		/*Error in the file system structure */
		return LV_FS_RES_FS_ERR;
	case -ENOENT:
		/*Driver, file or directory is not exists*/
		return LV_FS_RES_NOT_EX;
	case -EFBIG:
		/*Disk full*/
		return LV_FS_RES_FULL;
	case -EACCES:
		/*Access denied. Check 'fs_open' modes and write protect*/
		return LV_FS_RES_DENIED;
	case -EBUSY:
		/*The file system now can't handle it, try later*/
		return LV_FS_RES_BUSY;
	case -ENOMEM:
		/*Not enough memory for an internal operation*/
		return LV_FS_RES_OUT_OF_MEM;
	case -EINVAL:
		/*Invalid parameter among arguments*/
		return LV_FS_RES_INV_PARAM;
	default:
		return LV_FS_RES_UNKNOWN;
	}
}

static void *lvgl_fs_open(struct _lv_fs_drv_t *drv, const char *path,
			  lv_fs_mode_t mode)
{
	int err;
	int zmode = FS_O_CREATE;
	void *file;

	/* LVGL is passing absolute paths without the root slash add it back
	 * by decrementing the path pointer.
	 */
	path--;

	zmode |= (mode & LV_FS_MODE_WR) ? FS_O_WRITE : 0;
	zmode |= (mode & LV_FS_MODE_RD) ? FS_O_READ : 0;

	file = malloc(sizeof(struct fs_file_t));
	if (!file)
		return NULL;

	err = fs_open((struct fs_file_t *)file, path, zmode);
	if (err)
		return NULL;

	return file;
}

static lv_fs_res_t lvgl_fs_close(struct _lv_fs_drv_t *drv, void *file)
{
	int err;

	err = fs_close((struct fs_file_t *)file);
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_read(struct _lv_fs_drv_t *drv, void *file,
		void *buf, uint32_t btr, uint32_t *br)
{
	int err;

	err = fs_read((struct fs_file_t *)file, buf, btr);
	if (err > 0) {
		if (br != NULL) {
			*br = err;
		}
		err = 0;
	} else if (br != NULL) {
		*br = 0U;
	}
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_write(struct _lv_fs_drv_t *drv, void *file,
		const void *buf, uint32_t btw, uint32_t *bw)
{
	int err;

	err = fs_write((struct fs_file_t *)file, buf, btw);
	if (err == btw) {
		if (bw != NULL) {
			*bw = btw;
		}
		err = 0;
	} else if (err < 0) {
		if (bw != NULL) {
			*bw = 0U;
		}
	} else {
		if (bw != NULL) {
			*bw = err;
		}
		err = -EFBIG;
	}
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_seek(struct _lv_fs_drv_t *drv, void *file,
				uint32_t pos, lv_fs_whence_t whence)
{
	int err;

	err = fs_seek((struct fs_file_t *)file, pos, FS_SEEK_SET);
	return errno_to_lv_fs_res(err);
}

static lv_fs_res_t lvgl_fs_tell(struct _lv_fs_drv_t *drv, void *file,
		uint32_t *pos_p)
{
	*pos_p = fs_tell((struct fs_file_t *)file);
	return LV_FS_RES_OK;
}

static void *lvgl_fs_dir_open(struct _lv_fs_drv_t *drv, const char *path)
{
	void *dir;
	int err;

	/* LVGL is passing absolute paths without the root slash add it back
	 * by decrementing the path pointer.
	 */
	path--;

	dir = malloc(sizeof(struct fs_dir_t));
	if (!dir)
		return NULL;

	fs_dir_t_init((struct fs_dir_t *)dir);
	err = fs_opendir((struct fs_dir_t *)dir, path);
	if (err)
		return NULL;

	return dir;
}

static lv_fs_res_t lvgl_fs_dir_read(struct _lv_fs_drv_t *drv, void *dir,
		char *fn)
{
	/* LVGL expects a string as return parameter but the format of the
	 * string is not documented.
	 */
	return LV_FS_RES_NOT_IMP;
}

static lv_fs_res_t lvgl_fs_dir_close(struct _lv_fs_drv_t *drv, void *dir)
{
	int err;

	err = fs_closedir((struct fs_dir_t *)dir);
	free(dir);
	return errno_to_lv_fs_res(err);
}

static lv_fs_drv_t fs_drv;

void lvgl_fs_init(void)
{
	lv_fs_drv_init(&fs_drv);

	/* LVGL uses letter based mount points, just pass the root slash as a
	 * letter. Note that LVGL will remove the drive letter, or in this case
	 * the root slash, from the path passed via the FS callbacks.
	 * Zephyr FS API assumes this slash is present so we will need to add
	 * it back.
	 */
	fs_drv.letter = '/';
	fs_drv.ready_cb = lvgl_fs_ready;

	fs_drv.open_cb = lvgl_fs_open;
	fs_drv.close_cb = lvgl_fs_close;
	fs_drv.read_cb = lvgl_fs_read;
	fs_drv.write_cb = lvgl_fs_write;
	fs_drv.seek_cb = lvgl_fs_seek;
	fs_drv.tell_cb = lvgl_fs_tell;

	fs_drv.dir_open_cb = lvgl_fs_dir_open;
	fs_drv.dir_read_cb = lvgl_fs_dir_read;
	fs_drv.dir_close_cb = lvgl_fs_dir_close;

	lv_fs_drv_register(&fs_drv);
}

/**
 * @file lv_nuttx_lcd.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_lcd.h"

#if LV_USE_NUTTX

#if LV_USE_NUTTX_LCD

#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <debug.h>
#include <errno.h>
#include <fcntl.h>
#include <nuttx/lcd/lcd_dev.h>

#include "../../../lvgl.h"
#include "../../lvgl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* fd should be defined at the beginning */
    int fd;
    lv_display_t * disp;
    struct lcddev_area_s area;
    struct lcddev_area_align_s align_info;
} lv_nuttx_lcd_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t align_round_up(int32_t v, uint16_t align);
static void rounder_cb(lv_event_t * e);
static void flush_cb(lv_display_t * disp, const lv_area_t * area_p,
                     uint8_t * color_p);
static lv_display_t * lcd_init(int fd, int hor_res, int ver_res);
static void display_release_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_nuttx_lcd_create(const char * dev_path)
{
    struct fb_videoinfo_s vinfo;
    struct lcd_planeinfo_s pinfo;
    lv_display_t * disp;
    int fd;
    int ret;

    LV_ASSERT_NULL(dev_path);

    LV_LOG_USER("lcd %s opening", dev_path);
    fd = open(dev_path, 0);
    if(fd < 0) {
        perror("Error: cannot open lcd device");
        return NULL;
    }

    LV_LOG_USER("lcd %s open success", dev_path);

    ret = ioctl(fd, LCDDEVIO_GETVIDEOINFO,
                (unsigned long)((uintptr_t)&vinfo));
    if(ret < 0) {
        perror("Error: ioctl(LCDDEVIO_GETVIDEOINFO) failed");
        close(fd);
        return NULL;
    }

    ret = ioctl(fd, LCDDEVIO_GETPLANEINFO,
                (unsigned long)((uintptr_t)&pinfo));
    if(ret < 0) {
        perror("ERROR: ioctl(LCDDEVIO_GETPLANEINFO) failed");
        close(fd);
        return NULL;
    }

    disp = lcd_init(fd, vinfo.xres, vinfo.yres);
    if(disp == NULL) {
        close(fd);
    }

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t align_round_up(int32_t v, uint16_t align)
{
    return (v + align - 1) & ~(align - 1);
}

static void rounder_cb(lv_event_t * e)
{
    lv_nuttx_lcd_t * lcd = lv_event_get_user_data(e);
    lv_area_t * area = lv_event_get_param(e);
    struct lcddev_area_align_s * align_info = &lcd->align_info;
    int32_t w;
    int32_t h;

    area->x1 &= ~(align_info->col_start_align - 1);
    area->y1 &= ~(align_info->row_start_align - 1);

    w = align_round_up(lv_area_get_width(area), align_info->width_align);
    h = align_round_up(lv_area_get_height(area), align_info->height_align);

    area->x2 = area->x1 + w - 1;
    area->y2 = area->y1 + h - 1;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area_p,
                     uint8_t * color_p)
{
    lv_nuttx_lcd_t * lcd = disp->driver_data;

    lcd->area.row_start = area_p->y1;
    lcd->area.row_end = area_p->y2;
    lcd->area.col_start = area_p->x1;
    lcd->area.col_end = area_p->x2;
    lcd->area.data = (uint8_t *)color_p;
    ioctl(lcd->fd, LCDDEVIO_PUTAREA, (unsigned long) & (lcd->area));
    lv_display_flush_ready(disp);
}

static lv_display_t * lcd_init(int fd, int hor_res, int ver_res)
{
    lv_color_t * draw_buf = NULL;
    lv_color_t * draw_buf_2 = NULL;
    lv_nuttx_lcd_t * lcd = lv_malloc_zeroed(sizeof(lv_nuttx_lcd_t));
    LV_ASSERT_MALLOC(lcd);
    if(lcd == NULL) {
        LV_LOG_ERROR("lv_nuttx_lcd_t malloc failed");
        return NULL;
    }

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(lcd);
        return NULL;
    }

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
#if LV_NUTTX_LCD_BUFFER_COUNT > 0
    uint32_t buf_size = hor_res * ver_res * px_size;
    lv_display_render_mode_t render_mode = LV_DISPLAY_RENDER_MODE_FULL;
#else
    uint32_t buf_size = hor_res * LV_NUTTX_LCD_BUFFER_SIZE * px_size;
    lv_display_render_mode_t render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
#endif

    draw_buf = lv_malloc(buf_size);
    if(draw_buf == NULL) {
        LV_LOG_ERROR("display draw_buf malloc failed");
        lv_free(lcd);
        return NULL;
    }

#if LV_NUTTX_LCD_BUFFER_COUNT == 2
    draw_buf_2 = lv_malloc(buf_size);
    if(draw_buf_2 == NULL) {
        LV_LOG_ERROR("display draw_buf_2 malloc failed");
        lv_free(lcd);
        lv_free(draw_buf);
        return NULL;
    }
#endif

    lcd->fd = fd;
    if(ioctl(fd, LCDDEVIO_GETAREAALIGN, &lcd->align_info) < 0) {
        perror("Error: ioctl(LCDDEVIO_GETAREAALIGN) failed");
    }

    lcd->disp = disp;
    lv_display_set_buffers(lcd->disp, draw_buf, draw_buf_2, buf_size, render_mode);
    lv_display_set_flush_cb(lcd->disp, flush_cb);
    lv_display_add_event_cb(lcd->disp, rounder_cb, LV_EVENT_INVALIDATE_AREA, lcd);
    lv_display_add_event_cb(lcd->disp, display_release_cb, LV_EVENT_DELETE, lcd->disp);
    lv_display_set_driver_data(lcd->disp, lcd);

    return lcd->disp;
}

static void display_release_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_nuttx_lcd_t * dsc = lv_display_get_driver_data(disp);
    if(dsc) {
        lv_display_set_driver_data(disp, NULL);
        lv_display_set_flush_cb(disp, NULL);

        /* clear display buffer */
        if(disp->buf_1) {
            lv_free(disp->buf_1);
            disp->buf_1 = NULL;
        }
        if(disp->buf_2) {
            lv_free(disp->buf_2);
            disp->buf_2 = NULL;
        }

        /* close device fb */
        if(dsc->fd >= 0) {
            close(dsc->fd);
            dsc->fd = -1;
        }
        lv_free(dsc);
        LV_LOG_USER("Done");
    }
}
#endif /*LV_USE_NUTTX_LCD*/

#endif /* LV_USE_NUTTX*/

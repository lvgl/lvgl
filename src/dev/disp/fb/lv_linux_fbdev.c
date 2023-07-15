/**
 * @file lv_linux_fbdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_fbdev.h"
#if LV_USE_LINUX_FBDEV

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#if LV_LINUX_FBDEV_BSD
    #include <sys/fcntl.h>
    #include <sys/time.h>
    #include <sys/consio.h>
    #include <sys/fbio.h>
#elif LV_LINUX_FBDEV_NUTTX
    #include <nuttx/video/fb.h>
#else
    #include <linux/fb.h>
#endif /* LV_LINUX_FBDEV_BSD */

#include <lvgl/lvgl.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct bsd_fb_var_info {
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t xres;
    uint32_t yres;
    int bits_per_pixel;
};

struct bsd_fb_fix_info {
    long int line_length;
    long int smem_len;
};

typedef struct {
    const char * devname;
    lv_color_format_t color_format;
#if LV_LINUX_FBDEV_BSD
    struct bsd_fb_var_info vinfo;
    struct bsd_fb_fix_info finfo;
#else
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
#endif /* LV_LINUX_FBDEV_BSD */
    char * fbp;
    long int screensize;
    int fbfd;
} lv_linux_fb_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#if LV_LINUX_FBDEV_BSD
    #define FBIOBLANK FBIO_BLANK
#endif /* LV_LINUX_FBDEV_BSD */

#ifndef DIV_ROUND_UP
    #define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_disp_t * lv_linux_fbdev_create(void)
{
    lv_linux_fb_t * dsc = lv_malloc(sizeof(lv_linux_fb_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;
    lv_memzero(dsc, sizeof(lv_linux_fb_t));

    lv_disp_t * disp = lv_disp_create(800, 480);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    dsc->fbfd = -1;
    lv_disp_set_driver_data(disp, dsc);
    lv_disp_set_flush_cb(disp, flush_cb);

    return disp;
}

void lv_linux_fbdev_set_file(lv_disp_t * disp, const char * file)
{
    char * devname = lv_malloc(lv_strlen(file) + 1);
    LV_ASSERT_MALLOC(devname);
    if(devname == NULL) return;
    lv_strcpy(devname, file);

    lv_linux_fb_t * dsc = lv_disp_get_driver_data(disp);
    dsc->devname = devname;

    if(dsc->fbfd > 0) close(dsc->fbfd);

    /* Open the file for reading and writing*/
    dsc->fbfd = open(dsc->devname, O_RDWR);
    if(dsc->fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }
    LV_LOG_INFO("The framebuffer device was opened successfully");

#if !LV_LINUX_FBDEV_NUTTX
    /* Make sure that the display is on.*/
    if(ioctl(dsc->fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        perror("ioctl(FBIOBLANK)");
        /* Don't return. Some framebuffer drivers like efifb or simplefb don't implement FBIOBLANK.*/
    }
#endif /* !LV_LINUX_FBDEV_NUTTX */

#if LV_LINUX_FBDEV_BSD
    struct fbtype fb;
    unsigned line_length;

    /*Get fb type*/
    if(ioctl(dsc->fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return;
    }

    /*Get screen width*/
    if(ioctl(dsc->fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        perror("ioctl(FBIO_GETLINEWIDTH)");
        return;
    }

    dsc->vinfo.xres = (unsigned) fb.fb_width;
    dsc->vinfo.yres = (unsigned) fb.fb_height;
    dsc->vinfo.bits_per_pixel = fb.fb_depth;
    dsc->vinfo.xoffset = 0;
    dsc->vinfo.yoffset = 0;
    dsc->finfo.line_length = line_length;
    dsc->finfo.smem_len = dsc->finfo.line_length * dsc->vinfo.yres;
#else /* LV_LINUX_FBDEV_BSD */

    /* Get fixed screen information*/
    if(ioctl(dsc->fbfd, FBIOGET_FSCREENINFO, &dsc->finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    /* Get variable screen information*/
    if(ioctl(dsc->fbfd, FBIOGET_VSCREENINFO, &dsc->vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }
#endif /* LV_LINUX_FBDEV_BSD */

    LV_LOG_INFO("%dx%d, %dbpp", dsc->vinfo.xres, dsc->vinfo.yres, dsc->vinfo.bits_per_pixel);

    /* Figure out the size of the screen in bytes*/
    dsc->screensize =  dsc->finfo.smem_len; //finfo.line_length * vinfo.yres;

    /* Map the device to memory*/
    dsc->fbp = (char *)mmap(0, dsc->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, dsc->fbfd, 0);
    if((intptr_t)dsc->fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }

    /* Don't initialise the memory to retain what's currently displayed / avoid clearing the screen.
     * This is important for applications that only draw to a subsection of the full framebuffer.*/

    LV_LOG_INFO("The framebuffer device was mapped to memory successfully");

    lv_coord_t hor_res = dsc->vinfo.xres;
    lv_coord_t ver_res = dsc->vinfo.yres;
    lv_coord_t width = dsc->vinfo.width;
    uint32_t draw_buf_size = hor_res * dsc->vinfo.bits_per_pixel >> 3;
    if(LV_LINUX_FBDEV_BUFFER_COUNT < 1) {
        draw_buf_size *= LV_LINUX_FBDEV_BUFFER_SIZE;
    }
    else {
        draw_buf_size *= ver_res;
    }

    lv_color_t * draw_buf = lv_malloc(draw_buf_size);
    lv_color_t * draw_buf_2 = NULL;
    if(LV_LINUX_FBDEV_BUFFER_COUNT == 2) {
        draw_buf_2 = lv_malloc(draw_buf_size);
    }
    lv_disp_set_draw_buffers(disp, draw_buf, draw_buf_2, draw_buf_size, LV_LINUX_FBDEV_RENDER_MODE);
    lv_disp_set_res(disp, hor_res, ver_res);

    if(width) {
        lv_disp_set_dpi(disp, DIV_ROUND_UP(hor_res * 254, width * 10));
    }

    LV_LOG_INFO("Resolution is set to %dx%d at %ddpi", hor_res, ver_res, lv_disp_get_dpi(disp));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    lv_linux_fb_t * dsc = lv_disp_get_driver_data(disp);

    if(dsc->fbp == NULL ||
       area->x2 < 0 || area->y2 < 0 ||
       area->x1 > (int32_t)dsc->vinfo.xres - 1 || area->y1 > (int32_t)dsc->vinfo.yres - 1) {
        lv_disp_flush_ready(disp);
        return;
    }

    lv_coord_t w = lv_area_get_width(area);
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    uint32_t color_pos = (area->x1 + dsc->vinfo.xoffset) * px_size + area->y1 * dsc->finfo.line_length;
    uint32_t fb_pos = color_pos + dsc->vinfo.yoffset * dsc->finfo.line_length;

    uint8_t * fbp = (uint8_t *)dsc->fbp;
    int32_t y;
    if(LV_LINUX_FBDEV_RENDER_MODE == LV_DISP_RENDER_MODE_DIRECT) {
        for(y = area->y1; y <= area->y2; y++) {
            lv_memcpy(&fbp[fb_pos], &color_p[color_pos], w * px_size);
            fb_pos += dsc->finfo.line_length;
            color_pos += dsc->finfo.line_length;
        }
    }
    else {
        for(y = area->y1; y <= area->y2; y++) {
            lv_memcpy(&fbp[fb_pos], color_p, w * px_size);
            fb_pos += dsc->finfo.line_length;
            color_p += w * px_size;
        }
    }

#if LV_LINUX_FBDEV_NUTTX && defined(CONFIG_FB_UPDATE)
    /*May be some direct update command is required*/
    struct fb_area_s fb_area;
    fb_area.x = area->x1;
    fb_area.y = area->y1;
    fb_area.w = lv_area_get_width(area);
    fb_area.h = lv_area_get_height(area);
    ioctl(dsc->fbfd, FBIO_UPDATE, (unsigned long)((uintptr_t)&fb_area));
#endif

    lv_disp_flush_ready(disp);
}

#endif /*LV_USE_LINUX_FBDEV*/

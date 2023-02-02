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
#else  /* LV_LINUX_FBDEV_BSD */
    #include <linux/fb.h>
#endif /* LV_LINUX_FBDEV_BSD */

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
static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_LINUX_FBDEV_BSD
    #define FBIOBLANK FBIO_BLANK
#endif /* LV_LINUX_FBDEV_BSD */

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
    char * devname = lv_malloc(strlen(file));
    LV_ASSERT_MALLOC(devname);
    if(devname == NULL) return;
    strcpy(devname, file);

    lv_linux_fb_t * dsc = lv_disp_get_driver_data(disp);
    dsc->devname = devname;

    if(dsc->fbfd > 0) close(dsc->fbfd);

    // Open the file for reading and writing
    dsc->fbfd = open(dsc->devname, O_RDWR);
    if(dsc->fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }
    LV_LOG_INFO("The framebuffer device was opened successfully");

    // Make sure that the display is on.
    if(ioctl(dsc->fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        perror("ioctl(FBIOBLANK)");
        // Don't return. Some framebuffer drivers like efifb or simplefb don't implement FBIOBLANK.
    }

#if LV_LINUX_FBDEV_BSD
    struct fbtype fb;
    unsigned line_length;

    //Get fb type
    if(ioctl(dsc->fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return;
    }

    //Get screen width
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

    // Get fixed screen information
    if(ioctl(dsc->fbfd, FBIOGET_FSCREENINFO, &dsc->finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    // Get variable screen information
    if(ioctl(dsc->fbfd, FBIOGET_VSCREENINFO, &dsc->vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }
#endif /* LV_LINUX_FBDEV_BSD */

    LV_LOG_INFO("%dx%d, %dbpp", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    dsc->screensize =  dsc->finfo.smem_len; //finfo.line_length * vinfo.yres;

    // Map the device to memory
    dsc->fbp = (char *)mmap(0, dsc->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, dsc->fbfd, 0);
    if((intptr_t)dsc->fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }

    // Don't initialise the memory to retain what's currently displayed / avoid clearing the screen.
    // This is important for applications that only draw to a subsection of the full framebuffer.

    LV_LOG_INFO("The framebuffer device was mapped to memory successfully");

    lv_coord_t hor_res = dsc->vinfo.xres;
    lv_coord_t ver_res = dsc->vinfo.yres;
    uint32_t draw_buf_size = hor_res * ver_res / 4; /*1/4 screen sized buffer has the same performance */
    lv_color_t * draw_buf = malloc(draw_buf_size * sizeof(lv_color_t));
    lv_disp_set_draw_buffers(disp, draw_buf, NULL, draw_buf_size, LV_DISP_RENDER_MODE_PARTIAL);
    lv_disp_set_res(disp, hor_res, ver_res);

    LV_LOG_INFO("Resolution is set to %dx%d", hor_res, ver_res);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    lv_linux_fb_t * dsc = lv_disp_get_driver_data(disp);

    if(dsc->fbp == NULL ||
       area->x2 < 0 || area->y2 < 0 ||
       area->x1 > (int32_t)dsc->vinfo.xres - 1 || area->y1 > (int32_t)dsc->vinfo.yres - 1) {
        lv_disp_flush_ready(disp);
        return;
    }

    lv_coord_t w = lv_area_get_width(area);
    long int location = 0;

    /*32 bit per pixel*/
    if(dsc->vinfo.bits_per_pixel == 32 && LV_COLOR_DEPTH == 32) {
        uint32_t * fbp32 = (uint32_t *)dsc->fbp;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            location = (area->x1 + dsc->vinfo.xoffset) + (y + dsc->vinfo.yoffset) * dsc->finfo.line_length / 4;
            lv_memcpy(&fbp32[location], (uint32_t *)color_p, (area->x2 - area->x1 + 1) * 4);
            color_p += w;
        }
    }
    /*24 bit per pixel*/
    else if(dsc->vinfo.bits_per_pixel == 24 && LV_COLOR_DEPTH == 32) {
        uint8_t * fbp8 = (uint8_t *)dsc->fbp;
        lv_coord_t x;
        int32_t y;
        uint8_t * pixel;
        for(y = area->y1; y <= area->y2; y++) {
            location = (area->x1 + dsc->vinfo.xoffset) + (y + dsc->vinfo.yoffset) * dsc->finfo.line_length / 3;
            for(x = 0; x < w; ++x) {
                pixel = (uint8_t *)(&color_p[x]);
                fbp8[3 * (location + x)] = pixel[0];
                fbp8[3 * (location + x) + 1] = pixel[1];
                fbp8[3 * (location + x) + 2] = pixel[2];
            }
            color_p += w;
        }
    }
    /*16 bit per pixel*/
    else if(dsc->vinfo.bits_per_pixel == 16 && LV_COLOR_DEPTH == 16) {
        uint16_t * fbp16 = (uint16_t *)dsc->fbp;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            location = (area->x1 + dsc->vinfo.xoffset) + (y + dsc->vinfo.yoffset) * dsc->finfo.line_length / 2;
            lv_memcpy(&fbp16[location], (uint32_t *)color_p, (area->x2 - area->x1 + 1) * 2);
            color_p += w;
        }
    }
    /*8 bit per pixel*/
    else if(dsc->vinfo.bits_per_pixel == 8 && LV_COLOR_DEPTH == 8) {
        uint8_t * fbp8 = (uint8_t *)dsc->fbp;
        int32_t y;
        for(y = area->y1; y <= area->y2; y++) {
            location = (area->x1 + dsc->vinfo.xoffset) + (y + dsc->vinfo.yoffset) * dsc->finfo.line_length;
            lv_memcpy(&fbp8[location], (uint32_t *)color_p, (area->x2 - area->x1 + 1));
            color_p += w;
        }
    }
    else {
        LV_LOG_ERROR("Not supported color format. LV_COLOR_DEPTH == %d but FB color depth is %d\n", LV_COLOR_DEPTH,
                     dsc->vinfo.bits_per_pixel);
    }

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

    lv_disp_flush_ready(disp);
}

#endif /*LV_USE_LINUX_FBDEV*/

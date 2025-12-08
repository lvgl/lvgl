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
#include <time.h>

#if LV_LINUX_FBDEV_BSD
    #include <sys/fcntl.h>
    #include <sys/consio.h>
    #include <sys/fbio.h>
#else
    #include <linux/fb.h>
#endif /* LV_LINUX_FBDEV_BSD */

#include "../../../display/lv_display_private.h"
#include "../../../draw/sw/lv_draw_sw.h"
#include "../../../misc/lv_area_private.h"

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
#if LV_LINUX_FBDEV_MMAP
    char * fbp;
#endif
    uint8_t * rotated_buf;
    size_t rotated_buf_size;
    long int screensize;
    int fbfd;
    bool force_refresh;
    uint8_t * draw_buf_1;
    uint8_t * draw_buf_2;
} lv_linux_fb_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void del_event_cb(lv_event_t * e);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static uint32_t tick_get_cb(void);

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

lv_display_t * lv_linux_fbdev_create(void)
{
    lv_tick_set_cb(tick_get_cb);

    lv_linux_fb_t * dsc = lv_malloc_zeroed(sizeof(lv_linux_fb_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(800, 480);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    dsc->fbfd = -1;
    lv_display_set_driver_data(disp, dsc);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_add_event_cb(disp, del_event_cb, LV_EVENT_DELETE, NULL);

    return disp;
}

lv_result_t lv_linux_fbdev_set_file(lv_display_t * disp, const char * file)
{
    char * devname = lv_strdup(file);
    LV_ASSERT_MALLOC(devname);
    if(devname == NULL) {
        return LV_RESULT_INVALID;
    }

    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    dsc->devname = devname;

    if(dsc->fbfd > 0) close(dsc->fbfd);

    /* Open the file for reading and writing*/
    dsc->fbfd = open(dsc->devname, O_RDWR);
    if(dsc->fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return LV_RESULT_INVALID;
    }
    LV_LOG_INFO("The framebuffer device was opened successfully");

    /* Make sure that the display is on.*/
    if(ioctl(dsc->fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
        perror("ioctl(FBIOBLANK)");
        /* Don't return. Some framebuffer drivers like efifb or simplefb don't implement FBIOBLANK.*/
    }

#if LV_LINUX_FBDEV_BSD
    struct fbtype fb;
    unsigned line_length;

    /*Get fb type*/
    if(ioctl(dsc->fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return LV_RESULT_INVALID;
    }

    /*Get screen width*/
    if(ioctl(dsc->fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        perror("ioctl(FBIO_GETLINEWIDTH)");
        return LV_RESULT_INVALID;
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
        return LV_RESULT_INVALID;
    }

    /* Get variable screen information*/
    if(ioctl(dsc->fbfd, FBIOGET_VSCREENINFO, &dsc->vinfo) == -1) {
        perror("Error reading variable information");
        return LV_RESULT_INVALID;
    }
#endif /* LV_LINUX_FBDEV_BSD */

    LV_LOG_INFO("%dx%d, %dbpp", dsc->vinfo.xres, dsc->vinfo.yres, dsc->vinfo.bits_per_pixel);

    /* Figure out the size of the screen in bytes*/
    dsc->screensize =  dsc->finfo.smem_len;/*finfo.line_length * vinfo.yres;*/

#if LV_LINUX_FBDEV_MMAP
    /* Map the device to memory*/
    dsc->fbp = (char *)mmap(0, dsc->screensize, PROT_READ | PROT_WRITE, MAP_SHARED, dsc->fbfd, 0);
    if((intptr_t)dsc->fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return LV_RESULT_INVALID;
    }
#endif

    /* Don't initialise the memory to retain what's currently displayed / avoid clearing the screen.
     * This is important for applications that only draw to a subsection of the full framebuffer.*/

    LV_LOG_INFO("The framebuffer device was mapped to memory successfully");

    switch(dsc->vinfo.bits_per_pixel) {
        case 16:
            lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
            break;
        case 24:
            lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB888);
            break;
        case 32:
            lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
            break;
        default:
            LV_LOG_WARN("Not supported color format (%d bits)", dsc->vinfo.bits_per_pixel);
            return LV_RESULT_INVALID;
    }

    int32_t hor_res = dsc->vinfo.xres;
    int32_t ver_res = dsc->vinfo.yres;
    int32_t width = dsc->vinfo.width;
    uint32_t draw_buf_size = hor_res * (dsc->vinfo.bits_per_pixel >> 3);
    if(LV_LINUX_FBDEV_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        draw_buf_size *= LV_LINUX_FBDEV_BUFFER_SIZE;
    }
    else {
        draw_buf_size *= ver_res;
    }

    uint8_t * draw_buf = NULL;
    uint8_t * draw_buf_2 = NULL;
    draw_buf = lv_malloc(draw_buf_size);

    if(LV_LINUX_FBDEV_BUFFER_COUNT == 2) {
        draw_buf_2 = lv_malloc(draw_buf_size);
    }

    dsc->draw_buf_1 = draw_buf;
    dsc->draw_buf_2 = draw_buf_2;

    lv_display_set_resolution(disp, hor_res, ver_res);
    lv_display_set_buffers(disp, draw_buf, draw_buf_2, draw_buf_size, LV_LINUX_FBDEV_RENDER_MODE);

    if(width > 0) {
        lv_display_set_dpi(disp, DIV_ROUND_UP(hor_res * 254, width * 10));
    }

    LV_LOG_INFO("Resolution is set to %" LV_PRId32 "x%" LV_PRId32 " at %" LV_PRId32 "dpi",
                hor_res, ver_res, lv_display_get_dpi(disp));

    return LV_RESULT_OK;
}

void lv_linux_fbdev_set_force_refresh(lv_display_t * disp, bool enabled)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    dsc->force_refresh = enabled;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void write_to_fb(lv_linux_fb_t * dsc, uint32_t fb_pos, const void * data, size_t sz)
{
#if LV_LINUX_FBDEV_MMAP
    uint8_t * fbp = (uint8_t *)dsc->fbp;
    lv_memcpy(&fbp[fb_pos], data, sz);
#else
    if(pwrite(dsc->fbfd, data, sz, fb_pos) < 0)
        LV_LOG_ERROR("write failed: %d", errno);
#endif
}

static void del_event_cb(lv_event_t * e)
{
    if(LV_EVENT_DELETE != lv_event_get_code(e))
        return;

    lv_display_t * disp = lv_event_get_target(e);
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    if(!dsc) return;

#if LV_LINUX_FBDEV_MMAP
    if(MAP_FAILED != dsc->fbp) {
        munmap(dsc->fbp, dsc->screensize);
        dsc->fbp = MAP_FAILED;
    }
#endif
    if(dsc->fbfd >= 0) {
        close(dsc->fbfd);
        dsc->fbfd = -1;
    }
    if(dsc->rotated_buf) lv_free(dsc->rotated_buf);
    if(dsc->draw_buf_1) lv_free(dsc->draw_buf_1);
    if(dsc->draw_buf_2) lv_free(dsc->draw_buf_2);
    if(dsc->devname) lv_free((void *)dsc->devname);

    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);

#if LV_LINUX_FBDEV_MMAP
    if(dsc->fbp == NULL) {
        lv_display_flush_ready(disp);
        return;
    }
#endif

    const bool wait_for_last_flush = LV_LINUX_FBDEV_RENDER_MODE == LV_DISPLAY_RENDER_MODE_FULL;
    const bool is_last_flush = lv_display_flush_is_last(disp);
    const bool skip_flush = wait_for_last_flush && !is_last_flush;

    if(skip_flush) {
        lv_display_flush_ready(disp);
        return;
    }

    const lv_color_format_t cf = lv_display_get_color_format(disp);
    const uint32_t px_size = lv_color_format_get_size(cf);

    lv_area_t rotated_area;
    const lv_display_rotation_t rotation = lv_display_get_rotation(disp);

    /* Not all framebuffer kernel drivers support hardware rotation, so we need to handle it in software here */
    if(rotation != LV_DISPLAY_ROTATION_0) {
        int32_t src_w;
        int32_t src_h;
        uint32_t src_stride;

        /* Direct render mode rotation only works if we rotate the whole screen at the same time
         * To do that, we use the display's resolution and as the area
         *  we also grab the current draw buffer so that we can rotate the whole display */
        if(LV_LINUX_FBDEV_RENDER_MODE == LV_DISPLAY_RENDER_MODE_DIRECT) {
            if(!is_last_flush) {
                /* We need to wait for the last flush when using direct render mode with rotation*/
                lv_display_flush_ready(disp);
                return;
            }
            lv_draw_buf_t * draw_buf = lv_display_get_buf_active(disp);
            src_w = lv_display_get_horizontal_resolution(disp);
            src_h = lv_display_get_vertical_resolution(disp);
            src_stride = lv_draw_buf_width_to_stride(src_w, cf);
            color_p = draw_buf->data;
            rotated_area.x1 = rotated_area.y1 =  0;
            lv_area_set_width(&rotated_area, src_w);
            lv_area_set_height(&rotated_area, src_h);
        }
        else {
            /* For partial and full render modes, we need to rotate the current area
             * In Full mode we will rotate the whole display just like with direct render mode
             * but we don't need to do anything special since the area is already the full area of the display
             * For Partial mode we will rotate just the part we're currently displaying*/
            src_w = lv_area_get_width(area);
            src_h = lv_area_get_height(area);
            src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
            rotated_area = *area;
        }

        lv_display_rotate_area(disp, &rotated_area);
        const uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&rotated_area), cf);
        const size_t buf_size = dest_stride * lv_area_get_height(&rotated_area);
        if(!dsc->rotated_buf || dsc->rotated_buf_size != buf_size) {
            dsc->rotated_buf = lv_realloc(dsc->rotated_buf, buf_size);
            LV_ASSERT_MALLOC(dsc->rotated_buf);
            dsc->rotated_buf_size = buf_size;
        }
        lv_draw_sw_rotate(color_p, dsc->rotated_buf, src_w, src_h, src_stride, dest_stride, rotation, cf);
        area = &rotated_area;
        color_p = dsc->rotated_buf;
    }

    lv_area_t display_area;
    /* vinfo.xres and vinfo.yres will already be 1 less than the actual resolution. i.e: 1023x767 on a 1024x768 screen */
    lv_area_set(&display_area, 0, 0, dsc->vinfo.xres, dsc->vinfo.yres);

    /* TODO: Consider rendering the clipped area*/
    if(!lv_area_is_in(area, &display_area, 0)) {
        lv_display_flush_ready(disp);
        return;
    }

    uint32_t fb_pos =
        (area->x1 + dsc->vinfo.xoffset) * px_size +
        (area->y1 + dsc->vinfo.yoffset) * dsc->finfo.line_length;


    const int32_t w = lv_area_get_width(area);
    if(LV_LINUX_FBDEV_RENDER_MODE == LV_DISPLAY_RENDER_MODE_DIRECT && rotation == LV_DISPLAY_ROTATION_0) {
        uint32_t color_pos =
            area->x1 * px_size +
            area->y1 * disp->hor_res * px_size;

        for(int32_t y = area->y1; y <= area->y2; y++) {
            write_to_fb(dsc, fb_pos, &color_p[color_pos], w * px_size);
            fb_pos += dsc->finfo.line_length;
            color_pos += disp->hor_res * px_size;
        }
    }
    else {
        const int32_t stride = lv_draw_buf_width_to_stride(w, cf);
        for(int32_t y = area->y1; y <= area->y2; y++) {
            write_to_fb(dsc, fb_pos, color_p, w * px_size);
            fb_pos += dsc->finfo.line_length;
            color_p += stride;
        }
    }

    if(dsc->force_refresh) {
        dsc->vinfo.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
        if(ioctl(dsc->fbfd, FBIOPUT_VSCREENINFO, &(dsc->vinfo)) == -1) {
            perror("Error setting var screen info");
        }
    }

    lv_display_flush_ready(disp);
}

static uint32_t tick_get_cb(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t time_ms = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return time_ms;
}

#endif /*LV_USE_LINUX_FBDEV*/

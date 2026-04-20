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
#include <errno.h>

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
    bool skip_unblank;
    bool swap_rb;
    uint8_t * swap_line_buf;
    size_t swap_line_buf_size;
} lv_linux_fb_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void del_event_cb(lv_event_t * e);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static uint32_t tick_get_cb(void);
static void swap_rb_line_16(uint8_t * dst, const uint8_t * src, int32_t pixel_count);
static void swap_rb_line_24(uint8_t * dst, const uint8_t * src, int32_t pixel_count);
static void swap_rb_line_32(uint8_t * dst, const uint8_t * src, int32_t pixel_count);

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

    /* Make sure that the display is on (unless another process already owns the framebuffer). */
    if(!dsc->skip_unblank) {
        if(ioctl(dsc->fbfd, FBIOBLANK, FB_BLANK_UNBLANK) != 0) {
            perror("ioctl(FBIOBLANK)");
            /* Don't return. Some framebuffer drivers like efifb or simplefb don't implement FBIOBLANK.*/
        }
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

    /* AD5M and some other devices report incorrect bits_per_pixel via VSCREENINFO.
     * The stride (line_length) is always correct, so calculate true bpp from it.
     * Example: AD5M reports 16bpp but stride=3200 for 800px width = 4 bytes/pixel = 32bpp */
    uint32_t stride_bpp = 0;
    if(dsc->vinfo.xres > 0) {
        stride_bpp = (dsc->finfo.line_length * 8) / dsc->vinfo.xres;
    }

    /* Only override if stride_bpp is a standard depth (avoids false positives
     * from alignment-padded strides or virtual-width framebuffers). */
    if(stride_bpp > 0 && stride_bpp != dsc->vinfo.bits_per_pixel
       && (stride_bpp == 8 || stride_bpp == 16 || stride_bpp == 24 || stride_bpp == 32)) {
        LV_LOG_WARN("bits_per_pixel mismatch: vinfo says %d, stride indicates %d. Using stride value.",
                    dsc->vinfo.bits_per_pixel, stride_bpp);
        dsc->vinfo.bits_per_pixel = stride_bpp;
    }
    else if(stride_bpp > 0 && stride_bpp == dsc->vinfo.bits_per_pixel) {
        LV_LOG_INFO("bits_per_pixel %d matches stride calculation", dsc->vinfo.bits_per_pixel);
    }
    else {
        LV_LOG_INFO("bits_per_pixel %d retained; stride-derived bpp %u ignored (zero or non-standard)",
                    dsc->vinfo.bits_per_pixel, stride_bpp);
    }

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

#if !LV_LINUX_FBDEV_BSD
    /* Auto-detect BGR framebuffer layout from vinfo field offsets.
     * Standard RGB: red.offset > blue.offset (e.g., red=16, blue=0 for 32bpp)
     * BGR layout:   red.offset < blue.offset (e.g., red=0, blue=16 for 32bpp)
     * When BGR is detected, R/B channels are swapped during flush. */
    if(dsc->vinfo.red.length > 0 && dsc->vinfo.blue.length > 0) {
        if(dsc->vinfo.red.offset < dsc->vinfo.blue.offset) {
            dsc->swap_rb = true;
            LV_LOG_INFO("BGR framebuffer detected (red.offset=%d, blue.offset=%d) — enabling R/B swap",
                        dsc->vinfo.red.offset, dsc->vinfo.blue.offset);
        }
        else {
            dsc->swap_rb = false;
            LV_LOG_INFO("RGB framebuffer layout (red.offset=%d, blue.offset=%d)",
                        dsc->vinfo.red.offset, dsc->vinfo.blue.offset);
        }
    }
#endif

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

    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint8_t * draw_buf = NULL;
    uint8_t * draw_buf_2 = NULL;

    /* Over-allocate to guarantee LV_DRAW_BUF_ALIGN alignment.
     * Store raw pointers in dsc for lv_free(), pass aligned pointers to LVGL. */
    dsc->draw_buf_1 = lv_malloc(draw_buf_size + LV_DRAW_BUF_ALIGN - 1);
    LV_ASSERT_MALLOC(dsc->draw_buf_1);
    if(dsc->draw_buf_1 == NULL) {
        LV_LOG_ERROR("Failed to allocate draw buffer 1 (%u bytes)", draw_buf_size + LV_DRAW_BUF_ALIGN - 1);
        return LV_RESULT_INVALID;
    }
    draw_buf = lv_draw_buf_align(dsc->draw_buf_1, cf);

    if(LV_LINUX_FBDEV_BUFFER_COUNT == 2) {
        dsc->draw_buf_2 = lv_malloc(draw_buf_size + LV_DRAW_BUF_ALIGN - 1);
        LV_ASSERT_MALLOC(dsc->draw_buf_2);
        if(dsc->draw_buf_2 == NULL) {
            LV_LOG_ERROR("Failed to allocate draw buffer 2 (%u bytes)", draw_buf_size + LV_DRAW_BUF_ALIGN - 1);
            lv_free(dsc->draw_buf_1);
            dsc->draw_buf_1 = NULL;
            return LV_RESULT_INVALID;
        }
        draw_buf_2 = lv_draw_buf_align(dsc->draw_buf_2, cf);
    }

    lv_display_set_resolution(disp, hor_res, ver_res);
    lv_display_set_buffers(disp, draw_buf, draw_buf_2, draw_buf_size, LV_LINUX_FBDEV_RENDER_MODE);

    if(width > 0) {
        lv_display_set_dpi(disp, DIV_ROUND_UP(hor_res * 254, width * 10));
    }

    LV_LOG_INFO("Resolution is set to %" LV_PRId32 "x%" LV_PRId32 " at %" LV_PRId32 "dpi",
                hor_res, ver_res, lv_display_get_dpi(disp));

    return LV_RESULT_OK;
}

void lv_linux_fbdev_set_skip_unblank(lv_display_t * disp, bool skip)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    dsc->skip_unblank = skip;
}

void lv_linux_fbdev_set_force_refresh(lv_display_t * disp, bool enabled)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    dsc->force_refresh = enabled;
}

void lv_linux_fbdev_set_swap_rb(lv_display_t * disp, bool enabled)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    dsc->swap_rb = enabled;
}

bool lv_linux_fbdev_get_swap_rb(lv_display_t * disp)
{
    lv_linux_fb_t * dsc = lv_display_get_driver_data(disp);
    return dsc->swap_rb;
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
    if(dsc->swap_line_buf) lv_free(dsc->swap_line_buf);
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
    lv_area_set(&display_area, 0, 0, dsc->vinfo.xres - 1, dsc->vinfo.yres - 1);

    /* Clip the area to the display bounds */
    lv_area_t clipped_area;
    if(!lv_area_intersect(&clipped_area, area, &display_area)) {
        /* No intersection at all, nothing to render */
        lv_display_flush_ready(disp);
        return;
    }

    uint32_t fb_pos =
        (clipped_area.x1 + dsc->vinfo.xoffset) * px_size +
        (clipped_area.y1 + dsc->vinfo.yoffset) * dsc->finfo.line_length;
    const int32_t w = lv_area_get_width(&clipped_area);

    /* Prepare swap line buffer if R/B swap is active */
    const size_t line_bytes = w * px_size;
    uint8_t * swap_buf = NULL;
    if(dsc->swap_rb) {
        if(!dsc->swap_line_buf || dsc->swap_line_buf_size < line_bytes) {
            dsc->swap_line_buf = lv_realloc(dsc->swap_line_buf, line_bytes);
            LV_ASSERT_MALLOC(dsc->swap_line_buf);
            dsc->swap_line_buf_size = line_bytes;
        }
        swap_buf = dsc->swap_line_buf;
    }

    if(LV_LINUX_FBDEV_RENDER_MODE == LV_DISPLAY_RENDER_MODE_DIRECT && rotation == LV_DISPLAY_ROTATION_0) {
        uint32_t color_pos =
            (clipped_area.x1 - disp->offset_x) * px_size +
            (clipped_area.y1 - disp->offset_y) * disp->hor_res * px_size;
        for(int32_t y = clipped_area.y1; y <= clipped_area.y2; y++) {
            const uint8_t * src = &color_p[color_pos];
            if(swap_buf) {
                if(px_size == 4)      swap_rb_line_32(swap_buf, src, w);
                else if(px_size == 3) swap_rb_line_24(swap_buf, src, w);
                else                  swap_rb_line_16(swap_buf, src, w);
                src = swap_buf;
            }
            write_to_fb(dsc, fb_pos, src, line_bytes);
            fb_pos += dsc->finfo.line_length;
            color_pos += disp->hor_res * px_size;
        }
    }
    else {
        /* Calculate offset into color_p buffer based on original area */
        const int32_t x_offset = clipped_area.x1 - area->x1;
        const int32_t y_offset = clipped_area.y1 - area->y1;
        const int32_t stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);

        color_p += y_offset * stride + x_offset * px_size;

        for(int32_t y = clipped_area.y1; y <= clipped_area.y2; y++) {
            const uint8_t * src = color_p;
            if(swap_buf) {
                if(px_size == 4)      swap_rb_line_32(swap_buf, src, w);
                else if(px_size == 3) swap_rb_line_24(swap_buf, src, w);
                else                  swap_rb_line_16(swap_buf, src, w);
                src = swap_buf;
            }
            write_to_fb(dsc, fb_pos, src, line_bytes);
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

/* Swap red and blue channels for BGR framebuffers.
 * LVGL renders as RGB; these convert to BGR during the line copy. */

static void swap_rb_line_16(uint8_t * dst, const uint8_t * src, int32_t pixel_count)
{
    const uint16_t * s = (const uint16_t *)src;
    uint16_t * d = (uint16_t *)dst;
    for(int32_t i = 0; i < pixel_count; i++) {
        uint16_t px = s[i];
        /* RGB565: RRRRRGGG_GGGBBBBB -> BGR565: BBBBBGGG_GGGRRRRR */
        uint16_t r = (px >> 11) & 0x1F;
        uint16_t g = (px >> 5) & 0x3F;
        uint16_t b = px & 0x1F;
        d[i] = (b << 11) | (g << 5) | r;
    }
}

static void swap_rb_line_24(uint8_t * dst, const uint8_t * src, int32_t pixel_count)
{
    for(int32_t i = 0; i < pixel_count; i++) {
        int32_t off = i * 3;
        dst[off + 0] = src[off + 2];
        dst[off + 1] = src[off + 1];
        dst[off + 2] = src[off + 0];
    }
}

static void swap_rb_line_32(uint8_t * dst, const uint8_t * src, int32_t pixel_count)
{
    const uint32_t * s = (const uint32_t *)src;
    uint32_t * d = (uint32_t *)dst;
    for(int32_t i = 0; i < pixel_count; i++) {
        uint32_t px = s[i];
        /* xRGB -> xBGR: swap bits [23:16] (R) and bits [7:0] (B), keep G and x */
        d[i] = (px & 0xFF00FF00u) | ((px >> 16) & 0xFFu) | ((px & 0xFFu) << 16);
    }
}

#endif /*LV_USE_LINUX_FBDEV*/

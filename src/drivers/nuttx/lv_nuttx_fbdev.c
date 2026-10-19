/**
 * @file lv_nuttx_fbdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"
#if LV_USE_NUTTX

#include <stdlib.h>
#include <unistd.h>
#include LV_STDDEF_INCLUDE
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#ifdef __NuttX__
    #include <nuttx/video/fb.h>
#else
    #include "mock/nuttx_video_fb.h"
#endif

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
    struct fb_videoinfo_s vinfo;
    struct fb_planeinfo_s pinfo;

    void * mem;
    void * mem2;
    void * mem3;
    void * mem_off_screen;
    uint32_t mem2_yoffset;
    uint32_t mem3_yoffset;
    uint32_t mem2_fblen;

    lv_draw_buf_t buf1;
    lv_draw_buf_t buf2;
    lv_draw_buf_t buf3;
} lv_nuttx_fb_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static lv_color_format_t fb_fmt_to_color_format(int fmt);
static int fbdev_get_pinfo(int fd, struct fb_planeinfo_s * pinfo);
static int fbdev_init_mem2(lv_nuttx_fb_t * dsc);
static int fbdev_init_mem3(lv_nuttx_fb_t * dsc);
static void fbdev_cleanup(lv_display_t * disp, lv_nuttx_fb_t * dsc);
static void display_refr_timer_cb(lv_timer_t * tmr);
static void display_release_cb(lv_event_t * e);
#if defined(CONFIG_FB_UPDATE)
    static void fbdev_join_inv_areas(lv_display_t * disp, lv_area_t * final_inv_area);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_nuttx_fbdev_create(void)
{
    lv_nuttx_fb_t * dsc = lv_malloc_zeroed(sizeof(lv_nuttx_fb_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(800, 480);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    dsc->fd = -1;
    lv_display_set_driver_data(disp, dsc);
    lv_display_add_event_cb(disp, display_release_cb, LV_EVENT_DELETE, disp);
    lv_display_set_flush_cb(disp, flush_cb);
    return disp;
}

int lv_nuttx_fbdev_set_file(lv_display_t * disp, const char * file)
{
    LV_CHECK_ARG(disp != NULL, return -EINVAL);
    LV_CHECK_ARG(file != NULL, return -EINVAL);
    int ret;
    LV_ASSERT(disp && file);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);

    fbdev_cleanup(disp, dsc);

    /* Open the file for reading and writing*/

    dsc->fd = open(file, O_RDWR);
    if(dsc->fd < 0) {
        LV_LOG_ERROR("Error: cannot open framebuffer device");
        return -errno;
    }
    LV_LOG_USER("The framebuffer device was opened successfully");

    if(ioctl(dsc->fd, FBIOGET_VIDEOINFO, (unsigned long)((uintptr_t)&dsc->vinfo)) < 0) {
        LV_LOG_ERROR("ioctl(FBIOGET_VIDEOINFO) failed: %d", errno);
        ret = -errno;
        goto errout;
    }

    LV_LOG_USER("VideoInfo:");
    LV_LOG_USER("      fmt: %u", dsc->vinfo.fmt);
    LV_LOG_USER("     xres: %u", dsc->vinfo.xres);
    LV_LOG_USER("     yres: %u", dsc->vinfo.yres);
    LV_LOG_USER("  nplanes: %u", dsc->vinfo.nplanes);

    if((ret = fbdev_get_pinfo(dsc->fd, &dsc->pinfo)) < 0) {
        goto errout;
    }

    lv_color_format_t color_format = fb_fmt_to_color_format(dsc->vinfo.fmt);
    if(color_format == LV_COLOR_FORMAT_UNKNOWN) {
        ret = -EINVAL;
        goto errout;
    }

    dsc->mem = mmap(NULL, dsc->pinfo.fblen, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_FILE, dsc->fd, 0);
    if(dsc->mem == MAP_FAILED) {
        LV_LOG_ERROR("mmap failed: %d", errno);
        ret = -errno;
        goto errout;
    }

    uint32_t w = dsc->vinfo.xres;
    uint32_t h = dsc->vinfo.yres;
    uint32_t stride = dsc->pinfo.stride;
    uint32_t data_size = h * stride;
    uint32_t px_size = lv_color_format_get_size(color_format);

    if(w == 0 || h == 0 || stride == 0) {
        LV_LOG_ERROR("Invalid display geometry: %" LV_PRIu32 "x%" LV_PRIu32 ", stride: %" LV_PRIu32, w, h, stride);
        ret = -EINVAL;
        goto errout;
    }

    if((dsc->pinfo.xoffset + w) * px_size > stride) {
        LV_LOG_ERROR("Framebuffer geometry exceeds stride (%zu > %zu)",
                     (size_t)(dsc->pinfo.xoffset + w) * px_size, (size_t)stride);
        ret = -EINVAL;
        goto errout;
    }

    /* Check buffer mode */
    bool double_buffer = dsc->pinfo.yres_virtual >= (dsc->vinfo.yres * 2);
    if(double_buffer) {
        dsc->pinfo.yoffset = 0;
    }

    /* Check for overflow before multiplication */
    if(stride > 0 && dsc->pinfo.yoffset > SIZE_MAX / stride) {
        LV_LOG_ERROR("yoffset * stride overflows size_t");
        ret = -EINVAL;
        goto errout;
    }
    size_t y_part = (size_t)dsc->pinfo.yoffset * stride;
    size_t x_part = (size_t)dsc->pinfo.xoffset * px_size;
    if(SIZE_MAX - y_part < x_part) {
        LV_LOG_ERROR("fb_offset calculation overflows size_t");
        ret = -EINVAL;
        goto errout;
    }
    size_t fb_offset = y_part + x_part;
    if(SIZE_MAX - fb_offset < data_size || fb_offset + data_size > dsc->pinfo.fblen) {
        LV_LOG_ERROR("Framebuffer geometry exceeds fblen (%zu > %zu)",
                     fb_offset + data_size, (size_t)dsc->pinfo.fblen);
        ret = -EINVAL;
        goto errout;
    }
    lv_draw_buf_init(&dsc->buf1, w, h, color_format, stride, (uint8_t *)dsc->mem + fb_offset, data_size);

    if(double_buffer) {
        if((ret = fbdev_init_mem2(dsc)) < 0) {
            goto errout;
        }

        if(SIZE_MAX - fb_offset < data_size || fb_offset + data_size > dsc->mem2_fblen) {
            LV_LOG_ERROR("Framebuffer mem2 geometry exceeds mem2_fblen (%zu > %zu)",
                         fb_offset + data_size, (size_t)dsc->mem2_fblen);
            ret = -EINVAL;
            goto errout;
        }

        lv_draw_buf_init(&dsc->buf2, w, h, color_format, stride, (uint8_t *)dsc->mem2 + fb_offset, data_size);
        lv_display_set_draw_buffers(disp, &dsc->buf1, &dsc->buf2);

        bool triple_buffer = dsc->pinfo.yres_virtual >= (dsc->vinfo.yres * 3);
        if(triple_buffer) {
            if((ret = fbdev_init_mem3(dsc)) < 0) {
                goto errout;
            }

            lv_draw_buf_init(&dsc->buf3, w, h, color_format, stride, (uint8_t *)dsc->mem3 + fb_offset, data_size);
            lv_display_set_3rd_draw_buffer(disp, &dsc->buf3);
        }
    }
    else {
        dsc->mem_off_screen = malloc(data_size);
        LV_ASSERT_MALLOC(dsc->mem_off_screen);
        if(!dsc->mem_off_screen) {
            ret = -ENOMEM;
            LV_LOG_ERROR("Failed to allocate memory for off-screen buffer");
            goto errout;
        }

        LV_LOG_USER("Use off-screen mode, memory: %p, size: %" LV_PRIu32, dsc->mem_off_screen, data_size);
        lv_draw_buf_init(&dsc->buf2, w, h, color_format, stride, dsc->mem_off_screen, data_size);
        lv_display_set_draw_buffers(disp, &dsc->buf2, NULL);
    }

    lv_display_set_color_format(disp, color_format);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_resolution(disp, dsc->vinfo.xres, dsc->vinfo.yres);
    lv_timer_set_cb(disp->refr_timer, display_refr_timer_cb);
    lv_timer_resume(disp->refr_timer);

#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_display_set_matrix_rotation(disp, true);
#endif

    LV_LOG_USER("Resolution is set to %dx%d at %" LV_PRId32 "dpi",
                dsc->vinfo.xres, dsc->vinfo.yres, lv_display_get_dpi(disp));
    return 0;

errout:
    fbdev_cleanup(disp, dsc);
    return ret;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if defined(CONFIG_FB_UPDATE)
static void fbdev_join_inv_areas(lv_display_t * disp, lv_area_t * final_inv_area)
{
    uint16_t inv_index;

    bool area_joined = false;

    for(inv_index = 0; inv_index < disp->inv_p; inv_index++) {
        if(disp->inv_area_joined[inv_index] == 0) {
            const lv_area_t * area_p = &disp->inv_areas[inv_index];

            /* Join to final_area */

            if(!area_joined) {
                /* copy first area */
                *final_inv_area = *area_p;
                area_joined = true;
            }
            else {
                lv_area_join(final_inv_area,
                             final_inv_area,
                             area_p);
            }
        }
    }
}
#endif

static void display_refr_timer_cb(lv_timer_t * tmr)
{
    lv_display_t * disp = lv_timer_get_user_data(tmr);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);
    struct pollfd pfds[1];

    lv_memzero(pfds, sizeof(pfds));
    pfds[0].fd = dsc->fd;
    pfds[0].events = POLLOUT;

    /* Query free fb to draw */

    if(poll(pfds, 1, 0) < 0) {
        return;
    }

    if(pfds[0].revents & POLLOUT) {
        lv_display_refr_timer(tmr);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(color_p);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);

    lv_color_format_t cf = lv_display_get_color_format(disp);

    if(dsc->mem_off_screen) {
        if(cf == LV_COLOR_FORMAT_ARGB8888) {
            /* Ensure opaque alpha (0xFF000000) during line-by-line copy */
            int32_t y;
            int32_t w = lv_area_get_width(area);
            uint32_t stride = dsc->pinfo.stride;
            for(y = area->y1; y <= area->y2; y++) {
                uint32_t * dst = (uint32_t *)((uint8_t *)dsc->buf1.data + (y * stride)) + area->x1;
                uint32_t * src = (uint32_t *)((uint8_t *)dsc->buf2.data + (y * stride)) + area->x1;
                int32_t x;
                for(x = 0; x < w; x++) {
                    dst[x] = src[x] | 0xff000000u;
                }
            }
        }
        else {
            /* Fallback to direct draw buffer copy */
            lv_draw_buf_copy(&dsc->buf1, area, &dsc->buf2, area);
        }
    }
    else if(cf == LV_COLOR_FORMAT_ARGB8888) {
        /* In double/triple buffer mode, ensure opaque alpha even if bg_opa < 255 */
        lv_draw_buf_t * act_buf = disp->buf_act;
        if(act_buf && act_buf->data) {
            int32_t y;
            int32_t w = lv_area_get_width(area);
            uint32_t stride = dsc->pinfo.stride;
            for(y = area->y1; y <= area->y2; y++) {
                uint32_t * line = (uint32_t *)((uint8_t *)act_buf->data + (y * stride)) + area->x1;
                int32_t x;
                for(x = 0; x < w; x++) {
                    line[x] |= 0xff000000u;
                }
            }
        }
    }

    /* Skip the non-last flush */

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

#if defined(CONFIG_FB_UPDATE)
    /*May be some direct update command is required*/
    int yoffset = 0;
    if(disp->buf_act == disp->buf_2) {
        yoffset = dsc->mem2_yoffset;
    }
    else if(disp->buf_act == disp->buf_3) {
        yoffset = dsc->mem3_yoffset;
    }
    /* Join the areas to update */
    lv_area_t final_inv_area;
    lv_memzero(&final_inv_area, sizeof(final_inv_area));
    fbdev_join_inv_areas(disp, &final_inv_area);

    struct fb_area_s fb_area;
    fb_area.x = final_inv_area.x1;
    fb_area.y = final_inv_area.y1 + yoffset;
    fb_area.w = lv_area_get_width(&final_inv_area);
    fb_area.h = lv_area_get_height(&final_inv_area);
    if(ioctl(dsc->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)&fb_area)) < 0) {
        LV_LOG_ERROR("ioctl(FBIO_UPDATE) failed: %d", errno);
    }
#endif

    /* double framebuffer */

    if(dsc->mem2 != NULL) {
        if(disp->buf_act == disp->buf_1) {
            dsc->pinfo.yoffset = 0;
        }
        else if(disp->buf_act == disp->buf_2) {
            dsc->pinfo.yoffset = dsc->mem2_yoffset;
        }
        else if(disp->buf_act == disp->buf_3) {
            dsc->pinfo.yoffset = dsc->mem3_yoffset;
        }

        if(ioctl(dsc->fd, FBIOPAN_DISPLAY, (unsigned long)((uintptr_t) & (dsc->pinfo))) < 0) {
            LV_LOG_ERROR("ioctl(FBIOPAN_DISPLAY) failed: %d", errno);
        }
    }
    lv_display_flush_ready(disp);
}

static lv_color_format_t fb_fmt_to_color_format(int fmt)
{
    switch(fmt) {
        case FB_FMT_RGB16_565:
            return LV_COLOR_FORMAT_RGB565;
        case FB_FMT_RGB24:
            return LV_COLOR_FORMAT_RGB888;
        case FB_FMT_RGB32:
            /* Use ARGB8888 instead of XRGB8888 so that LVGL renders an opaque (0xFF)
             * alpha channel. Hardware display controllers and HDMI bridges in 32-bpp mode
             * treat alpha=0x00 as transparent, resulting in black or distorted displays.
             */
            return LV_COLOR_FORMAT_ARGB8888;
        case FB_FMT_RGBA32:
            return LV_COLOR_FORMAT_ARGB8888;
        default:
            break;
    }

    LV_LOG_ERROR("Unsupported color format: %d", fmt);

    return LV_COLOR_FORMAT_UNKNOWN;
}

static int fbdev_get_pinfo(int fd, struct fb_planeinfo_s * pinfo)
{
    if(ioctl(fd, FBIOGET_PLANEINFO, (unsigned long)((uintptr_t)pinfo)) < 0) {
        LV_LOG_ERROR("ERROR: ioctl(FBIOGET_PLANEINFO) failed: %d", errno);
        return -errno;
    }

    LV_LOG_USER("PlaneInfo (plane %d):", pinfo->display);
    LV_LOG_USER("    mem: %p", pinfo->fbmem);
    LV_LOG_USER("    fblen: %zu", pinfo->fblen);
    LV_LOG_USER("   stride: %u", pinfo->stride);
    LV_LOG_USER("  display: %u", pinfo->display);
    LV_LOG_USER("      bpp: %u", pinfo->bpp);

    return 0;
}

static int fbdev_init_mem2(lv_nuttx_fb_t * dsc)
{
    struct fb_planeinfo_s pinfo;
    void * phy_mem1;
    void * phy_mem2;

    int ret;

    /* Get display[0] planeinfo */
    lv_memzero(&pinfo, sizeof(pinfo));
    pinfo.display = dsc->pinfo.display;
    if((ret = fbdev_get_pinfo(dsc->fd, &pinfo)) < 0) return ret;
    phy_mem1 = pinfo.fbmem;

    /* Get display[1] planeinfo */
    lv_memzero(&pinfo, sizeof(pinfo));
    pinfo.display = dsc->pinfo.display + 1;
    if((ret = fbdev_get_pinfo(dsc->fd, &pinfo)) < 0) return ret;
    phy_mem2 = pinfo.fbmem;

    lv_uintptr_t offset = (lv_uintptr_t)phy_mem2 - (lv_uintptr_t)phy_mem1;
    bool is_consecutive = offset == 0;

    /* Check bpp */

    if(pinfo.bpp != dsc->pinfo.bpp) {
        LV_LOG_WARN("mem2 is incorrect");
        return -EINVAL;
    }

    /* Check the buffer address offset,
     * It needs to be divisible by pinfo.stride
     */

    if((offset % dsc->pinfo.stride) != 0) {
        LV_LOG_WARN("It is detected that buf_offset(%" PRIuPTR ") "
                    "and stride(%d) are not divisible, please ensure "
                    "that the driver handles the address offset by itself.",
                    offset, dsc->pinfo.stride);
    }

    /* Calculate the address and yoffset of mem2 */

    if(is_consecutive) {
        dsc->mem2_yoffset = dsc->vinfo.yres;
        offset = dsc->vinfo.yres * pinfo.stride;
    }
    else {
        dsc->mem2_yoffset = offset / dsc->pinfo.stride;
    }

    uint32_t fblen = is_consecutive ? pinfo.fblen / 2 : pinfo.fblen;
    void * mem2 = mmap(NULL, fblen, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FILE, dsc->fd, offset);
    if(mem2 == MAP_FAILED) {
        LV_LOG_ERROR("ERROR: mmap failed: %d, offset: %" PRIuPTR, errno, offset);
        return -errno;
    }
    dsc->mem2 = mem2;
    dsc->mem2_fblen = fblen;

    LV_LOG_USER("Use of %sconsecutive mem2 = %p, yoffset = %" LV_PRIu32, is_consecutive ? "" : "non-", dsc->mem2,
                dsc->mem2_yoffset);

    return 0;
}

static int fbdev_init_mem3(lv_nuttx_fb_t * dsc)
{
    uintptr_t buf_offset;
    struct fb_planeinfo_s pinfo;
    int ret;

    lv_memzero(&pinfo, sizeof(pinfo));

    /* Get display[2] planeinfo */

    pinfo.display = dsc->pinfo.display + 2;

    if((ret = fbdev_get_pinfo(dsc->fd, &pinfo)) < 0) {
        return ret;
    }

    /* Check bpp */

    if(pinfo.bpp != dsc->pinfo.bpp) {
        LV_LOG_WARN("mem3 is incorrect");
        return -EINVAL;
    }

    /* Check the buffer address offset,
     * It needs to be divisible by pinfo.stride
     */

    buf_offset = (uintptr_t)pinfo.fbmem - (uintptr_t)dsc->mem;

    if((buf_offset % dsc->pinfo.stride) != 0) {
        LV_LOG_WARN("It is detected that buf_offset(%" PRIuPTR ") "
                    "and stride(%d) are not divisible, please ensure "
                    "that the driver handles the address offset by itself.",
                    buf_offset, dsc->pinfo.stride);
    }

    /* Calculate the address and yoffset of mem3 */

    if(buf_offset == 0) {
        dsc->mem3_yoffset = dsc->vinfo.yres * 2;
        dsc->mem3 = (uint8_t *)pinfo.fbmem + dsc->mem3_yoffset * pinfo.stride;
        LV_LOG_USER("Use consecutive mem3 = %p, yoffset = %" LV_PRIu32,
                    dsc->mem3, dsc->mem3_yoffset);
    }
    else {
        dsc->mem3_yoffset = buf_offset / dsc->pinfo.stride;
        dsc->mem3 = pinfo.fbmem;
        LV_LOG_USER("Use non-consecutive mem3 = %p, yoffset = %" LV_PRIu32,
                    dsc->mem3, dsc->mem3_yoffset);
    }

    return 0;
}

static void fbdev_cleanup(lv_display_t * disp, lv_nuttx_fb_t * dsc)
{
    if(disp) {
        if(disp->refr_timer) {
            lv_timer_pause(disp->refr_timer);
        }
        disp->buf_1 = NULL;
        disp->buf_2 = NULL;
        disp->buf_3 = NULL;
        disp->buf_act = NULL;
    }

    if(dsc->mem_off_screen) {
        free(dsc->mem_off_screen);
        dsc->mem_off_screen = NULL;
    }

    if(dsc->mem2 != NULL && dsc->mem2 != MAP_FAILED) {
        munmap(dsc->mem2, dsc->mem2_fblen);
        dsc->mem2 = NULL;
        dsc->mem2_fblen = 0;
    }

    if(dsc->mem != NULL && dsc->mem != MAP_FAILED) {
        munmap(dsc->mem, dsc->pinfo.fblen);
        dsc->mem = NULL;
    }

    if(dsc->fd >= 0) {
        close(dsc->fd);
        dsc->fd = -1;
    }

    dsc->mem3 = NULL;
}

static void display_release_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);
    if(dsc) {
        lv_display_set_driver_data(disp, NULL);
        lv_display_set_flush_cb(disp, NULL);

        fbdev_cleanup(disp, dsc);

        lv_free(dsc);
    }
    LV_LOG_USER("Done");
}

#endif /*LV_USE_NUTTX*/

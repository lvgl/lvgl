/**
 * @file lv_nuttx_fbdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_nuttx_fbdev.h"
#if LV_USE_NUTTX

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <nuttx/video/fb.h>

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
    struct fb_videoinfo_s vinfo;
    struct fb_planeinfo_s pinfo;

    void * mem;
    void * mem2;
    uint32_t mem2_yoffset;
} lv_nuttx_fb_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static int fbdev_get_pinfo(int fd, struct fb_planeinfo_s * pinfo);
static int fbdev_init_mem2(lv_nuttx_fb_t * dsc);
static void display_refr_timer_cb(lv_timer_t * tmr);
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
    int ret;
    LV_ASSERT(disp && file);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);

    if(dsc->fd >= 0) close(dsc->fd);

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

    dsc->mem = mmap(NULL, dsc->pinfo.fblen, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_FILE, dsc->fd, 0);
    if(dsc->mem == MAP_FAILED) {
        LV_LOG_ERROR("ioctl(FBIOGET_PLANEINFO) failed: %d", errno);
        ret = -errno;
        goto errout;
    }

    /* double buffer mode */

    if(dsc->pinfo.yres_virtual == (dsc->vinfo.yres * 2)) {
        if((ret = fbdev_init_mem2(dsc)) < 0) {
            goto errout;
        }
    }

    lv_display_set_buffers(disp, dsc->mem, dsc->mem2,
                           (dsc->pinfo.stride * dsc->vinfo.yres), LV_DISP_RENDER_MODE_DIRECT);
    lv_display_set_resolution(disp, dsc->vinfo.xres, dsc->vinfo.yres);
    lv_timer_set_cb(disp->refr_timer, display_refr_timer_cb);

    LV_LOG_USER("Resolution is set to %dx%d at %" LV_PRId32 "dpi",
                dsc->vinfo.xres, dsc->vinfo.yres, lv_display_get_dpi(disp));
    return 0;

errout:
    close(dsc->fd);
    dsc->fd = -1;
    return ret;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

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
        _lv_display_refr_timer(tmr);
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);

    /* Skip the non-last flush */

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    if(dsc->mem == NULL ||
       area->x2 < 0 || area->y2 < 0 ||
       area->x1 > (int32_t)dsc->vinfo.xres - 1 || area->y1 > (int32_t)dsc->vinfo.yres - 1) {
        lv_display_flush_ready(disp);
        return;
    }

#if defined(CONFIG_FB_UPDATE)
    /*May be some direct update command is required*/

    struct fb_area_s fb_area;
    fb_area.x = area->x1;
    fb_area.y = area->y1;
    fb_area.w = lv_area_get_width(area);
    fb_area.h = lv_area_get_height(area);
    if(ioctl(dsc->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)&fb_area)) < 0) {
        LV_LOG_ERROR("ioctl(FBIO_UPDATE) failed: %d", errno);
    }
#endif

    /* double framebuffer */

    if(dsc->mem2 != NULL) {
        if(disp->buf_act == &disp->buf_1) {
            dsc->pinfo.yoffset = 0;
        }
        else {
            dsc->pinfo.yoffset = dsc->mem2_yoffset;
        }

        if(ioctl(dsc->fd, FBIOPAN_DISPLAY, (unsigned long)((uintptr_t) & (dsc->pinfo))) < 0) {
            LV_LOG_ERROR("ioctl(FBIOPAN_DISPLAY) failed: %d", errno);
        }
    }
    lv_display_flush_ready(disp);
}

static int fbdev_get_pinfo(int fd, FAR struct fb_planeinfo_s * pinfo)
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

    /* Only these pixel depths are supported.  vinfo.fmt is ignored, only
     * certain color formats are supported.
     */

    if(pinfo->bpp != 32 && pinfo->bpp != 16 &&
       pinfo->bpp != 8  && pinfo->bpp != 1) {
        LV_LOG_ERROR("bpp = %u not supported", pinfo->bpp);
        return -EINVAL;
    }

    return 0;
}

static int fbdev_init_mem2(lv_nuttx_fb_t * dsc)
{
    uintptr_t buf_offset;
    struct fb_planeinfo_s pinfo;
    int ret;

    lv_memzero(&pinfo, sizeof(pinfo));

    /* Get display[1] planeinfo */

    pinfo.display = dsc->pinfo.display + 1;

    if((ret = fbdev_get_pinfo(dsc->fd, &pinfo)) < 0) {
        return ret;
    }

    /* Check bpp */

    if(pinfo.bpp != dsc->pinfo.bpp) {
        LV_LOG_WARN("mem2 is incorrect");
        return -EINVAL;
    }

    /* Check the buffer address offset,
     * It needs to be divisible by pinfo.stride
     */

    buf_offset = pinfo.fbmem - dsc->mem;

    if((buf_offset % dsc->pinfo.stride) != 0) {
        LV_LOG_WARN("It is detected that buf_offset(%" PRIuPTR ") "
                    "and stride(%d) are not divisible, please ensure "
                    "that the driver handles the address offset by itself.",
                    buf_offset, dsc->pinfo.stride);
    }

    /* Calculate the address and yoffset of mem2 */

    if(buf_offset == 0) {
        dsc->mem2_yoffset = dsc->vinfo.yres;
        dsc->mem2 = pinfo.fbmem + dsc->mem2_yoffset * pinfo.stride;
        LV_LOG_USER("Use consecutive mem2 = %p, yoffset = %" LV_PRIu32,
                    dsc->mem2, dsc->mem2_yoffset);
    }
    else {
        dsc->mem2_yoffset = buf_offset / dsc->pinfo.stride;
        dsc->mem2 = pinfo.fbmem;
        LV_LOG_USER("Use non-consecutive mem2 = %p, yoffset = %" LV_PRIu32,
                    dsc->mem2, dsc->mem2_yoffset);
    }

    return 0;
}

static void display_release_cb(lv_event_t * e)
{
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_nuttx_fb_t * dsc = lv_display_get_driver_data(disp);
    if(dsc) {
        lv_display_set_driver_data(disp, NULL);
        lv_display_set_flush_cb(disp, NULL);

        if(dsc->fd >= 0) {
            close(dsc->fd);
            dsc->fd = -1;
        }
        lv_free(dsc);
    }
    LV_LOG_USER("Done");
}

#endif /*LV_USE_NUTTX*/

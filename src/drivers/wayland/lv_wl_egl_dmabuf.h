/**
 * @file lv_wl_egl_dmabuf.h
 *
 */

/**
 * Modified by Renesas in 2025
 */

#ifndef LV_WL_EGL_DMABUF_H
#define LV_WL_EGL_DMABUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "lv_wl_egl.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL && LV_WAYLAND_USE_EGL_DMABUF
#include <wayland_linux_dmabuf.h>
#include <linux/dma-buf.h>
#include <gbm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <drm/drm_fourcc.h>
#include <xf86drm.h>

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define LV_WL_DMABUF_NUM_BUFFERS 3  /* Triple buffering */

/* First define the base image type */
#ifndef GL_OES_EGL_image
typedef void * GLeglImageOES;
#endif

/* Then define EGLImageKHR */
#ifndef EGL_KHR_image
typedef void * EGLImageKHR;
#define EGL_NO_IMAGE_KHR ((EGLImageKHR)0)
#endif

/* EGL DMA-BUF extensions */
#ifndef EGL_LINUX_DMA_BUF_EXT
#define EGL_LINUX_DMA_BUF_EXT              0x3270
#define EGL_LINUX_DRM_FOURCC_EXT           0x3271
#define EGL_DMA_BUF_PLANE0_FD_EXT          0x3272
#define EGL_DMA_BUF_PLANE0_OFFSET_EXT      0x3273
#define EGL_DMA_BUF_PLANE0_PITCH_EXT       0x3274
#define EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT 0x3443
#define EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT 0x3444
#endif

/* Function pointer types */
typedef EGLImageKHR(EGLAPIENTRYP PFNEGLCREATEIMAGEKHRPROC)(EGLDisplay dpy, EGLContext ctx,
                                                           EGLenum target, EGLClientBuffer buffer,
                                                           const EGLint * attrib_list);
typedef EGLBoolean(EGLAPIENTRYP PFNEGLDESTROYIMAGEKHRPROC)(EGLDisplay dpy, EGLImageKHR image);
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)(GLenum target, GLeglImageOES image);

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/

struct buffer {
    struct gbm_device * gbm_device;
    struct gbm_bo * bo;
    GLuint texture_id;
    EGLImageKHR egl_image;

    int dmabuf_fd;
    uint32_t stride;
    uint32_t offset;
    struct wl_buffer * wl_buffer;

    bool busy;
};

/*****************************************************************************
 * GLOBAL FUNCTION PROTOTYPES
 *****************************************************************************/

/**
 * Initialize DMA-BUF support for the window
 * @param window Pointer to window structure
 * @return true on success, false on failure
 */
void lv_wl_egl_dmabuf_initalize_context(lv_wl_egl_window_t * window);

/**
 * Create draw buffer
 * @param window Pointer to window structure
 * @return true on success, false on failure
 */
bool lv_wl_egl_dmabuf_create_draw_buffers(lv_wl_egl_window_t * window);

/**
 * Cleanup DMA-BUF resources
 * @param window Pointer to window structure
 */
void lv_wl_egl_dmabuf_cleanup(lv_wl_egl_window_t * window);

/**
 * LVGL flush callback for DMA-BUF mode
 * @param disp LVGL display
 * @param area Area to flush
 * @param px_map Pixel data
 */
void lv_wl_egl_dmabuf_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**
 * Handle Wayland registry for DMA-BUF protocol
 * @param window Pointer to window structure
 * @param registry Wayland registry
 * @param id Object ID
 * @param interface Interface name
 * @param version Interface version
 */
void lv_wl_egl_dmabuf_registry_handler(dmabuf_ctx_t * window,
                                       struct wl_registry * registry,
                                       uint32_t id,
                                       const char * interface,
                                       uint32_t version);

#endif

#ifdef __cplusplus
}
#endif

#endif /* LV_WL_EGL_DMABUF_H */

/**
 * @file lv_wl_egl_dmabuf.c
 *
 */

/**
 * Modified by Renesas in 2025
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "lv_wl_egl_dmabuf.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL && LV_WAYLAND_USE_EGL_DMABUF
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define BYTES_PER_PIXEL ((LV_COLOR_DEPTH + 7) / 8)

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/
/* GBM and DRM functions */
static bool init_gbm_device(lv_wl_egl_window_t * window);
static void cleanup_gbm_device(lv_wl_egl_window_t * window);
static int open_drm_device(void);

/* DMA-BUF functions */
static bool load_egl_dmabuf_extensions(EGLDisplay display);
static bool create_dmabuf_buffers(lv_wl_egl_window_t * window, dmabuf_ctx_t * ctx, int num_buffers);
static bool create_dmabuf_buffer(lv_wl_egl_window_t * window, dmabuf_ctx_t * ctx,
                                 struct buffer * buffer, int32_t width, int32_t height);
static void destroy_dmabuf_buffer(lv_wl_egl_window_t * window,
                                  struct buffer * buffer);
static void cleanup_dmabuf_buffers(lv_wl_egl_window_t * window, dmabuf_ctx_t * ctx);
static struct buffer * get_next_buffer(lv_wl_egl_window_t * window);

/* Wayland DMA-BUF listeners */
static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf,
                          uint32_t format);
static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf,
                             uint32_t format, uint32_t modifier_hi, uint32_t modifier_lo);
static void buffer_release(void * data, struct wl_buffer * wl_buffer);

/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/
static PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHR = NULL;
static PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHR = NULL;
static PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES = NULL;

static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener = {
    .format = dmabuf_format,
    .modifier = dmabuf_modifiers,
};

static const struct wl_buffer_listener buffer_listener = {
    .release = buffer_release,
};

/*****************************************************************************
 * WAYLAND LISTENER IMPLEMENTATIONS
 *****************************************************************************/

static void dmabuf_format(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf,
                          uint32_t format)
{
    dmabuf_ctx_t * ctx = data;

    LV_UNUSED(zwp_linux_dmabuf);

    if(LV_COLOR_DEPTH == 32 && format == DRM_FORMAT_ARGB8888) {
        /* Wayland compositors MUST support ARGB8888 */
        ctx->format = format;
    }
    else if(LV_COLOR_DEPTH == 32 && format == DRM_FORMAT_XRGB8888 && ctx->format != DRM_FORMAT_ARGB8888) {
        /* Select XRGB only if the compositor doesn't support transprancy */
        ctx->format = format;
    }
    else if(LV_COLOR_DEPTH == 16 && format == DRM_FORMAT_RGB565) {
        ctx->format = format;
    }
}

static void dmabuf_modifiers(void * data, struct zwp_linux_dmabuf_v1 * zwp_linux_dmabuf,
                             uint32_t format, uint32_t modifier_hi, uint32_t modifier_lo)
{
    LV_UNUSED(modifier_hi);
    LV_UNUSED(modifier_lo);
    dmabuf_format(data, zwp_linux_dmabuf, format);
}

static void buffer_release(void * data, struct wl_buffer * wl_buffer)
{
    LV_UNUSED(wl_buffer);
    struct buffer * buf = data;
    buf->busy           = false;
}

/*****************************************************************************
 * HELPER FUNCTIONS
 *****************************************************************************/

static int open_drm_device(void)
{
    drmDevicePtr devices[64];
    int num_devices, fd = -1;

    num_devices = drmGetDevices2(0, devices, 64);
    if(num_devices < 0) {
        LV_LOG_ERROR("drmGetDevices2 failed: %s", strerror(-num_devices));
        return -1;
    }

    /* Try to find a primary GPU device */
    for(int i = 0; i < num_devices; i++) {
        drmDevicePtr device = devices[i];

        if(!(device->available_nodes & (1 << DRM_NODE_PRIMARY)))
            continue;

        fd = open(device->nodes[DRM_NODE_PRIMARY], O_RDWR);
        if(fd >= 0) {
            LV_LOG_INFO("Opened DRM device: %s", device->nodes[DRM_NODE_PRIMARY]);
            break;
        }
    }

    drmFreeDevices(devices, num_devices);

    if(fd < 0) {
        LV_LOG_ERROR("Failed to open DRM device");
    }

    return fd;
}

/*****************************************************************************
 * GBM DEVICE FUNCTIONS
 *****************************************************************************/

static bool init_gbm_device(lv_wl_egl_window_t * window)
{
    dmabuf_ctx_t * ctx = window->dmabuf_ctx;

    /* Allocate buffer array */
    ctx->buffers = lv_malloc_zeroed(sizeof(struct buffer) * LV_WL_DMABUF_NUM_BUFFERS);
    if(!ctx->buffers) {
        LV_LOG_ERROR("Failed to allocate buffer array");
        return false;
    }

    /* Open DRM device */
    window->drm_fd = open_drm_device();
    if(window->drm_fd < 0) {
        LV_LOG_ERROR("Failed to open DRM device");
        lv_free(ctx->buffers);
        ctx->buffers = NULL;
        return false;
    }

    /* Create GBM device */
    window->gbm_device = gbm_create_device(window->drm_fd);
    if(!window->gbm_device) {
        LV_LOG_ERROR("Failed to create GBM device");
        close(window->drm_fd);
        window->drm_fd = -1;
        lv_free(ctx->buffers);
        ctx->buffers = NULL;
        return false;
    }

    /* Initialize buffer structures */
    for(int i = 0; i < LV_WL_DMABUF_NUM_BUFFERS; i++) {
        ctx->buffers[i].dmabuf_fd = -1;
        ctx->buffers[i].bo = NULL;
        ctx->buffers[i].wl_buffer = NULL;
        ctx->buffers[i].egl_image = EGL_NO_IMAGE_KHR;
        ctx->buffers[i].texture_id = 0;
        ctx->buffers[i].busy = false;
        ctx->buffers[i].stride = 0;
        ctx->buffers[i].offset = 0;
    }

    window->current_buffer = 0;

    LV_LOG_INFO("GBM device initialized successfully");
    return true;
}

static void cleanup_gbm_device(lv_wl_egl_window_t * window)
{
    /* Destroy GBM device */
    if(window->gbm_device) {
        gbm_device_destroy(window->gbm_device);
        window->gbm_device = NULL;
    }

    /* Close DRM device */
    if(window->drm_fd >= 0) {
        close(window->drm_fd);
        window->drm_fd = -1;
    }
}

/*****************************************************************************
 * EGL EXTENSION FUNCTIONS
 *****************************************************************************/

static bool load_egl_dmabuf_extensions(EGLDisplay display)
{
    const char * extensions = eglQueryString(display, EGL_EXTENSIONS);
    if(!extensions) {
        LV_LOG_ERROR("Failed to query EGL extensions");
        return false;
    }

    /* Check for required extensions */
    if(!strstr(extensions, "EGL_KHR_image_base") &&
       !strstr(extensions, "EGL_KHR_image")) {
        LV_LOG_ERROR("EGL_KHR_image not supported");
        return false;
    }

    if(!strstr(extensions, "EGL_EXT_image_dma_buf_import")) {
        LV_LOG_ERROR("EGL_EXT_image_dma_buf_import not supported");
        return false;
    }

    /* Load function pointers */
    eglCreateImageKHR = (PFNEGLCREATEIMAGEKHRPROC)
                        eglGetProcAddress("eglCreateImageKHR");
    eglDestroyImageKHR = (PFNEGLDESTROYIMAGEKHRPROC)
                         eglGetProcAddress("eglDestroyImageKHR");
    glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
                                   eglGetProcAddress("glEGLImageTargetTexture2DOES");

    if(!eglCreateImageKHR || !eglDestroyImageKHR ||
       !glEGLImageTargetTexture2DOES) {
        LV_LOG_ERROR("Failed to load required EGL functions");
        return false;
    }

    LV_LOG_INFO("EGL DMA-BUF extensions loaded successfully");
    return true;
}

/*****************************************************************************
 * DMA-BUF BUFFER FUNCTIONS
 *****************************************************************************/

static void destroy_dmabuf_buffer(lv_wl_egl_window_t * window,
                                  struct buffer * buffer)
{
    if(!buffer) return;

    if(buffer->wl_buffer) {
        wl_buffer_destroy(buffer->wl_buffer);
        buffer->wl_buffer = NULL;
    }

    /* Delete OpenGL texture */
    if(buffer->texture_id) {
        glDeleteTextures(1, &buffer->texture_id);
        buffer->texture_id = 0;
    }

    /* Destroy EGL image */
    if(buffer->egl_image != EGL_NO_IMAGE_KHR) {
        eglDestroyImageKHR(window->egl_display, buffer->egl_image);
        buffer->egl_image = EGL_NO_IMAGE_KHR;
    }

    /* Close DMA-BUF file descriptors */
    if(buffer->dmabuf_fd >= 0) {
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
    }

    /* Destroy GBM buffer object */
    if(buffer->bo) {
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
    }

    buffer->busy = false;
    buffer->stride = 0;
    buffer->offset = 0;

    LV_LOG_INFO("DMA-BUF buffer destroyed");
}

static bool create_dmabuf_buffer(lv_wl_egl_window_t * window, dmabuf_ctx_t * ctx,
                                 struct buffer * buffer, int32_t width, int32_t height)
{
    /* Determine GBM format based on color depth */
#if LV_COLOR_DEPTH == 32
    uint32_t gbm_format = GBM_FORMAT_ARGB8888;
#elif LV_COLOR_DEPTH == 16
    uint32_t gbm_format = GBM_FORMAT_RGB565;
#else
#error "Unsupported color depth for DMA-BUF"
#endif

    /* Create GBM buffer object with linear layout for CPU access */
    uint32_t gbm_flags = GBM_BO_USE_RENDERING | GBM_BO_USE_LINEAR;

    /* Use the GBM device from the window, not from the buffer */
    buffer->bo = gbm_bo_create(window->gbm_device, width, height, gbm_format, gbm_flags);
    if(!buffer->bo) {
        LV_LOG_ERROR("Failed to create GBM buffer object");
        return false;
    }

    /* Get buffer properties */
    buffer->stride = gbm_bo_get_stride_for_plane(buffer->bo, 0);
    buffer->offset = gbm_bo_get_offset(buffer->bo, 0);

    /* Get DMA-BUF file descriptor */
    int fd = gbm_bo_get_fd(buffer->bo);
    if(fd < 0) {
        LV_LOG_ERROR("Failed to get DMA-BUF fd: %s", strerror(errno));
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
        return false;
    }

    buffer->dmabuf_fd = fd;

    LV_LOG_INFO("Created GBM BO: %dx%d, stride=%u, offset=%u, fd=%d",
                width, height, buffer->stride, buffer->offset, buffer->dmabuf_fd);

    /* Create EGL image from DMA-BUF */
    EGLint attribs[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_LINUX_DRM_FOURCC_EXT, (EGLint)ctx->format,
        EGL_DMA_BUF_PLANE0_FD_EXT, buffer->dmabuf_fd,
        EGL_DMA_BUF_PLANE0_OFFSET_EXT, (EGLint)buffer->offset,
        EGL_DMA_BUF_PLANE0_PITCH_EXT, (EGLint)buffer->stride,
        EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT, (EGLint)(DRM_FORMAT_MOD_LINEAR & 0xFFFFFFFF),
        EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT, (EGLint)(DRM_FORMAT_MOD_LINEAR >> 32),
        EGL_NONE
    };

    buffer->egl_image = eglCreateImageKHR(window->egl_display, EGL_NO_CONTEXT,
                                          EGL_LINUX_DMA_BUF_EXT, (EGLClientBuffer)NULL, attribs);

    if(buffer->egl_image == EGL_NO_IMAGE_KHR) {
        EGLint egl_error = eglGetError();
        LV_LOG_ERROR("Failed to create EGL image: 0x%x", egl_error);
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
        return false;
    }

    LV_LOG_INFO("Created EGL image successfully");

    /* Create OpenGL texture from EGL image */
    glGenTextures(1, &buffer->texture_id);
    glBindTexture(GL_TEXTURE_2D, buffer->texture_id);

    /* Set texture parameters */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Bind EGL image to texture */
    glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, buffer->egl_image);

    GLenum gl_error = glGetError();
    if(gl_error != GL_NO_ERROR) {
        LV_LOG_ERROR("Failed to bind EGL image to texture: 0x%x", gl_error);
        glDeleteTextures(1, &buffer->texture_id);
        buffer->texture_id = 0;
        eglDestroyImageKHR(window->egl_display, buffer->egl_image);
        buffer->egl_image = EGL_NO_IMAGE_KHR;
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    LV_LOG_INFO("Created GL texture successfully (ID: %u)", buffer->texture_id);

    /* Create Wayland buffer using zwp_linux_dmabuf protocol */
    struct zwp_linux_buffer_params_v1 * params;
    params = zwp_linux_dmabuf_v1_create_params(ctx->handler);
    if(!params) {
        LV_LOG_ERROR("Failed to create buffer params");
        glDeleteTextures(1, &buffer->texture_id);
        buffer->texture_id = 0;
        eglDestroyImageKHR(window->egl_display, buffer->egl_image);
        buffer->egl_image = EGL_NO_IMAGE_KHR;
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
        return false;
    }

    /* Add plane to buffer params */
    zwp_linux_buffer_params_v1_add(params,
                                   buffer->dmabuf_fd,
                                   0,  /* plane index */
                                   buffer->offset,
                                   buffer->stride,
                                   DRM_FORMAT_MOD_LINEAR >> 32,  /* modifier hi */
                                   DRM_FORMAT_MOD_LINEAR & 0xFFFFFFFF);  /* modifier lo */

    /* Create buffer immediately */
    buffer->wl_buffer = zwp_linux_buffer_params_v1_create_immed(params,
                                                                width,
                                                                height,
                                                                ctx->format,
                                                                0);  /* flags */

    zwp_linux_buffer_params_v1_destroy(params);

    if(!buffer->wl_buffer) {
        LV_LOG_ERROR("Failed to create Wayland buffer");
        glDeleteTextures(1, &buffer->texture_id);
        buffer->texture_id = 0;
        eglDestroyImageKHR(window->egl_display, buffer->egl_image);
        buffer->egl_image = EGL_NO_IMAGE_KHR;
        close(buffer->dmabuf_fd);
        buffer->dmabuf_fd = -1;
        gbm_bo_destroy(buffer->bo);
        buffer->bo = NULL;
        return false;
    }

    /* Add listener to track buffer release */
    wl_buffer_add_listener(buffer->wl_buffer, &buffer_listener, buffer);

    buffer->busy = false;

    LV_LOG_INFO("Created DMA-BUF buffer: %dx%d, stride=%u, fd=%d, texture_id=%u",
                width, height, buffer->stride, buffer->dmabuf_fd, buffer->texture_id);

    return true;
}

static bool create_dmabuf_buffers(lv_wl_egl_window_t * window,
                                  dmabuf_ctx_t * ctx, int num_buffers)
{
    LV_UNUSED(num_buffers);

    for(int i = 0; i < LV_WL_DMABUF_NUM_BUFFERS; i++) {
        if(!create_dmabuf_buffer(window, ctx, &ctx->buffers[i], window->width, window->height)) {
            /* Cleanup on failure */
            for(int j = 0; j < i; j++) {
                destroy_dmabuf_buffer(window, &ctx->buffers[j]);
            }
            lv_free(ctx->buffers);
            ctx->buffers = NULL;
            return false;
        }
    }

    LV_LOG_INFO("Successfully created %d DMA-BUF buffers", num_buffers);
    return true;
}

static void cleanup_dmabuf_buffers(lv_wl_egl_window_t * window, dmabuf_ctx_t * ctx)
{
    if(!ctx || !ctx->buffers) return;

    for(int i = 0; i < LV_WL_DMABUF_NUM_BUFFERS; i++) {
        while(ctx->buffers[i].busy) {
            wl_display_dispatch(window->display);
        }
    }

    /* Destroy all buffers */
    for(int i = 0; i < LV_WL_DMABUF_NUM_BUFFERS; i++) {
        destroy_dmabuf_buffer(window, &ctx->buffers[i]);
    }

    /* Free buffer array */
    lv_free(ctx->buffers);
    ctx->buffers = NULL;
}

static struct buffer * get_next_buffer(lv_wl_egl_window_t * window)
{
    if(!window || !window->dmabuf_ctx || !window->dmabuf_ctx->buffers) {
        return NULL;
    }

    dmabuf_ctx_t * ctx = window->dmabuf_ctx;

    /* Find a non-busy buffer */
    for(int i = 0; i < LV_WL_DMABUF_NUM_BUFFERS; i++) {
        int index = (window->current_buffer + i) % LV_WL_DMABUF_NUM_BUFFERS;
        if(!ctx->buffers[index].busy) {
            window->current_buffer = index;
            return &ctx->buffers[index];
        }
    }

    /* All buffers busy, wait for one to become available */
    LV_LOG_WARN("All DMA-BUF buffers busy, waiting...");
    while(ctx->buffers[window->current_buffer].busy) {
        wl_display_dispatch(window->display);
    }

    return &ctx->buffers[window->current_buffer];
}

/*****************************************************************************
 * PUBLIC API FUNCTIONS
 *****************************************************************************/

void lv_wl_egl_dmabuf_registry_handler(dmabuf_ctx_t * context,
                                       struct wl_registry * registry,
                                       uint32_t id,
                                       const char * interface,
                                       uint32_t version)
{
    LV_UNUSED(version);
    if(strcmp(interface, "zwp_linux_dmabuf_v1") == 0) {
        context->handler = wl_registry_bind(registry, id,
                                            &zwp_linux_dmabuf_v1_interface, 3);
        zwp_linux_dmabuf_v1_add_listener(context->handler, &dmabuf_listener, context);
        LV_LOG_INFO("DMA-BUF protocol available");
    }
}

void lv_wl_egl_dmabuf_initalize_context(lv_wl_egl_window_t * window)
{
    if(!window) {
        LV_LOG_ERROR("Invalid window or GL context");
        return;
    }

    /* Allocate DMA-BUF context */
    dmabuf_ctx_t * ctx = lv_malloc_zeroed(sizeof(dmabuf_ctx_t));
    if(!ctx) {
        LV_LOG_ERROR("Failed to allocate DMA-BUF context");
        return;
    }

    /* Initialize context */
    ctx->format = DRM_FORMAT_INVALID;
    ctx->handler = NULL;

    /* Store context in OpenGL ES context */
    window->dmabuf_ctx = ctx;
    window->use_dmabuf = true;
}

bool lv_wl_egl_dmabuf_create_draw_buffers(lv_wl_egl_window_t * window)
{
    if(!window || !window->dmabuf_ctx) {
        LV_LOG_ERROR("Invalid window or DMA-BUF context");
        return false;
    }

    dmabuf_ctx_t * ctx = window->dmabuf_ctx;

    /* Check if DMA-BUF protocol is available */
    if(!ctx->handler) {
        LV_LOG_WARN("DMA-BUF protocol not available");
        window->use_dmabuf = false;
        return false;
    }

    /* Wait for format negotiation */
    wl_display_roundtrip(window->display);

    /* Check if a suitable format was found */
    if(ctx->format == DRM_FORMAT_INVALID) {
        LV_LOG_ERROR("No suitable DMA-BUF format found");
        window->use_dmabuf = false;
        return false;
    }

    LV_LOG_INFO("Using DMA-BUF format: 0x%x", ctx->format);

    /* Initialize GBM device */
    if(!init_gbm_device(window)) {
        LV_LOG_ERROR("Failed to initialize GBM device");
        window->use_dmabuf = false;
        return false;
    }

    /* Load EGL DMA-BUF extensions */
    if(!load_egl_dmabuf_extensions(window->egl_display)) {
        LV_LOG_ERROR("Failed to load EGL DMA-BUF extensions");
        cleanup_gbm_device(window);
        window->use_dmabuf = false;
        return false;
    }

    /* Create DMA-BUF buffers */
    if(!create_dmabuf_buffers(window, ctx, LV_WL_DMABUF_NUM_BUFFERS)) {
        LV_LOG_ERROR("Failed to create DMA-BUF buffers");
        cleanup_gbm_device(window);
        window->use_dmabuf = false;
        return false;
    }

    LV_LOG_INFO("DMA-BUF support initialized successfully with %d buffers",
                LV_WL_DMABUF_NUM_BUFFERS);

    return true;
}

void lv_wl_egl_dmabuf_cleanup(lv_wl_egl_window_t * window)
{
    if(!window || !window->dmabuf_ctx) return;

    dmabuf_ctx_t * ctx = (dmabuf_ctx_t *)window->dmabuf_ctx;

    /* Cleanup buffers */
    cleanup_dmabuf_buffers(window, ctx);

    /* Cleanup DMA-BUF protocol */
    if(ctx->handler) {
        zwp_linux_dmabuf_v1_destroy(ctx->handler);
        window->dmabuf_ctx = NULL;
        window->use_dmabuf = false;
    }

    /* Cleanup GBM device */
    cleanup_gbm_device(window);

    /* Free context */
    lv_free(ctx);
    window->dmabuf_ctx = NULL;
    window->use_dmabuf = false;

    LV_LOG_INFO("DMA-BUF cleanup complete");
}

void lv_wl_egl_dmabuf_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)lv_display_get_user_data(disp);
    if(!window || !window->dmabuf_ctx) {
        lv_display_flush_ready(disp);
        return;
    }

    dmabuf_ctx_t * ctx = (dmabuf_ctx_t *)window->dmabuf_ctx;
    if(!ctx || !ctx->buffers) {
        lv_display_flush_ready(disp);
        return;
    }

    /* Get next available buffer */
    struct buffer * buffer = get_next_buffer(window);
    if(!buffer) {
        LV_LOG_ERROR("Failed to get DMA-BUF buffer");
        lv_display_flush_ready(disp);
        return;
    }

    int32_t x = area->x1;
    int32_t y = area->y1;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    /* Upload texture data */
    glBindTexture(GL_TEXTURE_2D, buffer->texture_id);

#if LV_COLOR_DEPTH == 16
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                    GL_RGB, GL_UNSIGNED_SHORT_5_6_5, px_map);
#elif LV_COLOR_DEPTH == 32
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                    GL_BGRA, GL_UNSIGNED_BYTE, px_map);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    /* Present buffer if this is the last flush */
    if(lv_display_flush_is_last(disp)) {
        wl_surface_attach(window->surface, buffer->wl_buffer, 0, 0);
        wl_surface_damage_buffer(window->surface, 0, 0, window->width, window->height);
        wl_surface_commit(window->surface);
        buffer->busy = true;
        window->current_buffer = (window->current_buffer + 1) % LV_WL_DMABUF_NUM_BUFFERS;
    }

    lv_display_flush_ready(disp);
}

#endif /* LV_USE_WAYLAND && LV_WAYLAND_USE_EGL && LV_WAYLAND_USE_EGL_DMABUF */

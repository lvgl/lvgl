/**
 * @file lv_linux_drm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"
#if LV_USE_LINUX_DRM

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

#include "../../../stdlib/lv_sprintf.h"
#include "../../../draw/lv_draw_buf.h"

#if LV_LINUX_DRM_USE_EGL && !LV_USE_LINUX_DRM_GBM_BUFFERS
    #error LV_USE_LINUX_DRM_GBM_BUFFERS is required to use LV_LINUX_DRM_USE_EGL
#endif

#if LV_USE_LINUX_DRM_GBM_BUFFERS

    #include <gbm.h>
    #include <linux/dma-buf.h>
    #include <sys/ioctl.h>

#endif

#if LV_LINUX_DRM_USE_EGL

    #include "../../opengles/lv_opengles_egl.h"
    #include "../../opengles/lv_opengles_texture.h"
    #include "../../opengles/lv_opengles_window.h"
    #include "../../opengles/lv_opengles_private.h"
    #include <EGL/egl.h>
    #include <EGL/eglext.h>

#endif

/*********************
 *      DEFINES
 *********************/
#if LV_COLOR_DEPTH == 32
    #define DRM_FOURCC DRM_FORMAT_XRGB8888
#elif LV_COLOR_DEPTH == 16
    #define DRM_FOURCC DRM_FORMAT_RGB565
#else
    #error LV_COLOR_DEPTH not supported
#endif

#define BUFFER_CNT 2

#define USE_EGL_EXPERIMENTAL_DIRECT_WINDOW_RENDER 0

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    uint32_t handle;
    uint32_t pitch;
    uint32_t offset;
    unsigned long int size;
    uint8_t * map;
    uint32_t fb_handle;
#if LV_LINUX_DRM_USE_EGL
    uint32_t fb_id;
#endif
} drm_buffer_t;

#if LV_LINUX_DRM_USE_EGL
    typedef EGLint(*egl_wait_sync_khr_t)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags);
    typedef EGLint(*egl_dup_native_fence_fd_android_t)(EGLDisplay dpy, EGLSyncKHR sync);
    typedef EGLSyncKHR(*egl_create_sync_khr_t)(EGLDisplay dpy, EGLenum type, const EGLint * attrib_list);
    typedef EGLBoolean(*egl_destroy_sync_khr_t)(EGLDisplay dpy, EGLSyncKHR sync);
    typedef EGLint(*egl_client_wait_sync_khr_t)(EGLDisplay dpy, EGLSyncKHR sync, EGLint flags, EGLTimeKHR timeout);
#endif

typedef struct {
    int fd;
    uint32_t conn_id, enc_id, crtc_id, plane_id, crtc_idx;
    uint32_t width, height;
    uint32_t mmWidth, mmHeight;
    uint32_t fourcc;
    drmModeModeInfo mode;
    uint32_t blob_id;
    drmModeCrtc * saved_crtc;
    drmModeAtomicReq * req;
    drmEventContext drm_event_ctx;
    drmModePlane * plane;
    drmModeCrtc * crtc;
    drmModeConnector * conn;
    uint32_t count_plane_props;
    uint32_t count_crtc_props;
    uint32_t count_conn_props;
    drmModePropertyPtr plane_props[128];
    drmModePropertyPtr crtc_props[128];
    drmModePropertyPtr conn_props[128];
    drm_buffer_t drm_bufs[BUFFER_CNT];
    drm_buffer_t * act_buf;
#if LV_USE_LINUX_DRM_GBM_BUFFERS
    struct gbm_device * gbm_device;
#endif
#if LV_LINUX_DRM_USE_EGL
    struct gbm_surface * surface;
    int kms_in_fence_fd;
    int kms_out_fence_fd;
    EGLSyncKHR kms_fence;
    EGLSyncKHR gpu_fence;
    struct gbm_bo * bo;

    egl_wait_sync_khr_t egl_wait_sync_khr;
    egl_dup_native_fence_fd_android_t egl_dup_native_fence_fd_android;
    egl_create_sync_khr_t egl_create_sync_khr;
    egl_destroy_sync_khr_t egl_destroy_sync_khr;
    egl_client_wait_sync_khr_t egl_client_wait_sync_khr;
#endif
} drm_dev_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static uint32_t get_plane_property_id(drm_dev_t * drm_dev, const char * name);
static uint32_t get_crtc_property_id(drm_dev_t * drm_dev, const char * name);
static uint32_t get_conn_property_id(drm_dev_t * drm_dev, const char * name);
static void page_flip_handler(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec,
                              void * user_data);
static int drm_get_plane_props(drm_dev_t * drm_dev);
static int drm_get_crtc_props(drm_dev_t * drm_dev);
static int drm_get_conn_props(drm_dev_t * drm_dev);
static int drm_add_plane_property(drm_dev_t * drm_dev, const char * name, uint64_t value);
static int drm_add_crtc_property(drm_dev_t * drm_dev, const char * name, uint64_t value);
static int drm_add_conn_property(drm_dev_t * drm_dev, const char * name, uint64_t value);
static int find_plane(drm_dev_t * drm_dev, unsigned int fourcc, uint32_t * plane_id, uint32_t crtc_id,
                      uint32_t crtc_idx);
static int drm_find_connector(drm_dev_t * drm_dev, int64_t connector_id);
static int drm_open(const char * path);
static int drm_setup(drm_dev_t * drm_dev, const char * device_path, int64_t connector_id, unsigned int fourcc);

static uint32_t tick_get_cb(void);

#if !LV_USE_LINUX_DRM_GBM_BUFFERS
    static int drm_allocate_dumb(drm_dev_t * drm_dev, drm_buffer_t * buf);
#elif LV_USE_LINUX_DRM_GBM_BUFFERS && !LV_LINUX_DRM_USE_EGL
    static int create_gbm_buffer(drm_dev_t * drm_dev, drm_buffer_t * buf);
#endif

#if LV_LINUX_DRM_USE_EGL
    static drm_buffer_t * drm_fb_get_from_bo(struct gbm_bo * bo);
    static void drm_fb_destroy_callback(struct gbm_bo * bo, void * data);
    static void drm_gbm_egl_pre(lv_opengles_window_t * window);
    static void drm_gbm_egl_post1(lv_opengles_window_t * window);
    static void drm_gbm_egl_post2(lv_opengles_window_t * window);
    static int drm_atomic_commit(drm_dev_t * drm_dev, uint32_t fb_id, uint32_t flags);
    static EGLSyncKHR create_fence(drm_dev_t * drm_dev, EGLDisplay display, int fd);
#else
    static int drm_setup_buffers(drm_dev_t * drm_dev);
    static int drm_dmabuf_set_plane(drm_dev_t * drm_dev, drm_buffer_t * buf);
    static void drm_flush_wait(lv_display_t * drm_dev);
    static void drm_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
    static void drm_dmabuf_set_active_buf(lv_event_t * event);
#endif


/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#ifndef DIV_ROUND_UP
    #define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_linux_drm_create(void)
{
    lv_display_t * disp;

    lv_tick_set_cb(tick_get_cb);

    drm_dev_t * drm_dev = lv_malloc_zeroed(sizeof(drm_dev_t));
    LV_ASSERT_MALLOC(drm_dev);
    if(drm_dev == NULL) return NULL;

    drm_dev->fd = -1;
#if !LV_LINUX_DRM_USE_EGL
    disp = lv_display_create(800, 480);
    if(disp == NULL) {
        lv_free(drm_dev);
        return NULL;
    }
    lv_display_set_driver_data(disp, drm_dev);
    lv_display_set_flush_wait_cb(disp, drm_flush_wait);
    lv_display_set_flush_cb(disp, drm_flush);

#else /*LV_LINUX_DRM_USE_EGL*/
    disp = (lv_display_t *) drm_dev;

    drm_dev->kms_in_fence_fd = -1;
    drm_dev->kms_out_fence_fd = -1;

#endif

    return disp;
}

#if !LV_LINUX_DRM_USE_EGL
/* Called by LVGL when there is something that needs redrawing
 * it sets the active buffer. if GBM buffers are used, it issues a DMA_BUF_SYNC
 * ioctl call to lock the buffer for CPU access, the buffer is unlocked just
 * before the atomic commit */
static void drm_dmabuf_set_active_buf(lv_event_t * event)
{

    drm_dev_t * drm_dev;
    lv_display_t * disp;
    lv_draw_buf_t * act_buf;
    int i;

    disp = (lv_display_t *) lv_event_get_current_target(event);
    drm_dev = (drm_dev_t *) lv_display_get_driver_data(disp);
    act_buf = lv_display_get_buf_active(disp);

    if(drm_dev->act_buf == NULL) {

        for(i = 0; i < BUFFER_CNT; i++) {
            if(act_buf->unaligned_data == drm_dev->drm_bufs[i].map) {
                drm_dev->act_buf = &drm_dev->drm_bufs[i];
                LV_LOG_TRACE("Set active buffer idx: %d", i);
                break;
            }
        }


#if LV_USE_LINUX_DRM_GBM_BUFFERS

        struct dma_buf_sync sync_req;
        sync_req.flags = DMA_BUF_SYNC_START | DMA_BUF_SYNC_RW;
        int res;

        if((res = ioctl(drm_dev->act_buf->handle, DMA_BUF_IOCTL_SYNC, &sync_req)) != 0) {
            LV_LOG_ERROR("Failed to start DMA-BUF R/W SYNC res: %d", res);
        }
#endif

    }
    else {

        LV_LOG_TRACE("active buffer already set");
    }

}
#endif /*!LV_LINUX_DRM_USE_EGL*/

void lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id)
{
    int ret;

#if !LV_LINUX_DRM_USE_EGL
    drm_dev_t * drm_dev = lv_display_get_driver_data(disp);
#else
    drm_dev_t * drm_dev = (drm_dev_t *) disp;
#endif

    ret = drm_setup(drm_dev, file, connector_id, DRM_FOURCC);
    if(ret) {
        return;
    }

    int32_t hor_res = drm_dev->width;
    int32_t ver_res = drm_dev->height;

#if !LV_LINUX_DRM_USE_EGL
    ret = drm_setup_buffers(drm_dev);
    if(ret) {
        LV_LOG_ERROR("DRM buffer allocation failed");
        close(drm_dev->fd);
        drm_dev->fd = -1;
        return;
    }

    LV_LOG_INFO("DRM subsystem and buffer mapped successfully");

    int32_t width = drm_dev->mmWidth;

    size_t buf_size = LV_MIN(drm_dev->drm_bufs[1].size, drm_dev->drm_bufs[0].size);
    /* Resolution must be set first because if the screen is smaller than the size passed
     * to lv_display_create then the buffers aren't big enough for LV_DISPLAY_RENDER_MODE_DIRECT.
     */
    lv_display_set_resolution(disp, hor_res, ver_res);
    lv_display_set_buffers(disp, drm_dev->drm_bufs[1].map, drm_dev->drm_bufs[0].map, buf_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);


    /* Set the handler that is called before a redraw occurs to set the active buffer/plane
     * when GBM buffers are used the DMA_BUF_SYNC_START is issued there */
    lv_display_add_event_cb(disp, drm_dmabuf_set_active_buf, LV_EVENT_REFR_START, drm_dev);

    if(width) {
        lv_display_set_dpi(disp, DIV_ROUND_UP(hor_res * 25400, width * 1000));
    }

    LV_LOG_INFO("Resolution is set to %" LV_PRId32 "x%" LV_PRId32 " at %" LV_PRId32 "dpi",
                hor_res, ver_res, lv_display_get_dpi(disp));

#else /*LV_LINUX_DRM_USE_EGL*/
    lv_opengles_window_t * window = lv_opengles_egl_window_create(hor_res, ver_res, drm_dev->surface, drm_dev->gbm_device,
                                                                  drm_gbm_egl_pre, drm_gbm_egl_post1, drm_gbm_egl_post2);
    lv_opengles_egl_window_set_user_data(window, drm_dev);

#if !USE_EGL_EXPERIMENTAL_DIRECT_WINDOW_RENDER
    /* create a display that flushes to a texture */
    lv_display_t * texture = lv_opengles_texture_create(hor_res, ver_res);
    lv_display_set_default(texture);

    /* add the texture to the window */
    unsigned int texture_id = lv_opengles_texture_get_texture_id(texture);
    lv_opengles_window_add_texture(window, texture_id, hor_res, ver_res);
#else
    /* render directly to the window */
    lv_opengles_window_display_create(window, hor_res, ver_res);
#endif

    drm_dev->egl_wait_sync_khr = (egl_wait_sync_khr_t) eglGetProcAddress("eglWaitSyncKHR");
    drm_dev->egl_dup_native_fence_fd_android = (egl_dup_native_fence_fd_android_t)
                                               eglGetProcAddress("eglDupNativeFenceFDANDROID");
    drm_dev->egl_create_sync_khr = (egl_create_sync_khr_t) eglGetProcAddress("eglCreateSyncKHR");
    drm_dev->egl_destroy_sync_khr = (egl_destroy_sync_khr_t) eglGetProcAddress("eglDestroySyncKHR");
    drm_dev->egl_client_wait_sync_khr = (egl_client_wait_sync_khr_t) eglGetProcAddress("eglClientWaitSyncKHR");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t get_plane_property_id(drm_dev_t * drm_dev, const char * name)
{
    uint32_t i;

    LV_LOG_TRACE("Find plane property: %s", name);

    for(i = 0; i < drm_dev->count_plane_props; ++i)
        if(!lv_strcmp(drm_dev->plane_props[i]->name, name))
            return drm_dev->plane_props[i]->prop_id;

    LV_LOG_TRACE("Unknown plane property: %s", name);

    return 0;
}

static uint32_t get_crtc_property_id(drm_dev_t * drm_dev, const char * name)
{
    uint32_t i;

    LV_LOG_TRACE("Find crtc property: %s", name);

    for(i = 0; i < drm_dev->count_crtc_props; ++i)
        if(!lv_strcmp(drm_dev->crtc_props[i]->name, name))
            return drm_dev->crtc_props[i]->prop_id;

    LV_LOG_TRACE("Unknown crtc property: %s", name);

    return 0;
}

static uint32_t get_conn_property_id(drm_dev_t * drm_dev, const char * name)
{
    uint32_t i;

    LV_LOG_TRACE("Find conn property: %s", name);

    for(i = 0; i < drm_dev->count_conn_props; ++i)
        if(!lv_strcmp(drm_dev->conn_props[i]->name, name))
            return drm_dev->conn_props[i]->prop_id;

    LV_LOG_TRACE("Unknown conn property: %s", name);

    return 0;
}

static void page_flip_handler(int fd, unsigned int sequence, unsigned int tv_sec, unsigned int tv_usec,
                              void * user_data)
{
    LV_UNUSED(fd);
    LV_UNUSED(sequence);
    LV_UNUSED(tv_sec);
    LV_UNUSED(tv_usec);
    LV_LOG_TRACE("flip");
    drm_dev_t * drm_dev = user_data;
    if(drm_dev->req) {
        drmModeAtomicFree(drm_dev->req);
        drm_dev->req = NULL;
    }
}

static int drm_get_plane_props(drm_dev_t * drm_dev)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev->fd, drm_dev->plane_id,
                                                                  DRM_MODE_OBJECT_PLANE);
    if(!props) {
        LV_LOG_ERROR("drmModeObjectGetProperties failed");
        return -1;
    }
    LV_LOG_TRACE("Found %u plane props", props->count_props);
    drm_dev->count_plane_props = props->count_props;
    for(i = 0; i < props->count_props; i++) {
        drm_dev->plane_props[i] = drmModeGetProperty(drm_dev->fd, props->props[i]);
        LV_LOG_TRACE("Added plane prop %u:%s", drm_dev->plane_props[i]->prop_id, drm_dev->plane_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_crtc_props(drm_dev_t * drm_dev)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev->fd, drm_dev->crtc_id,
                                                                  DRM_MODE_OBJECT_CRTC);
    if(!props) {
        LV_LOG_ERROR("drmModeObjectGetProperties failed");
        return -1;
    }
    LV_LOG_TRACE("Found %u crtc props", props->count_props);
    drm_dev->count_crtc_props = props->count_props;
    for(i = 0; i < props->count_props; i++) {
        drm_dev->crtc_props[i] = drmModeGetProperty(drm_dev->fd, props->props[i]);
        LV_LOG_TRACE("Added crtc prop %u:%s", drm_dev->crtc_props[i]->prop_id, drm_dev->crtc_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_get_conn_props(drm_dev_t * drm_dev)
{
    uint32_t i;

    drmModeObjectPropertiesPtr props = drmModeObjectGetProperties(drm_dev->fd, drm_dev->conn_id,
                                                                  DRM_MODE_OBJECT_CONNECTOR);
    if(!props) {
        LV_LOG_ERROR("drmModeObjectGetProperties failed");
        return -1;
    }
    LV_LOG_TRACE("Found %u connector props", props->count_props);
    drm_dev->count_conn_props = props->count_props;
    for(i = 0; i < props->count_props; i++) {
        drm_dev->conn_props[i] = drmModeGetProperty(drm_dev->fd, props->props[i]);
        LV_LOG_TRACE("Added connector prop %u:%s", drm_dev->conn_props[i]->prop_id, drm_dev->conn_props[i]->name);
    }
    drmModeFreeObjectProperties(props);

    return 0;
}

static int drm_add_plane_property(drm_dev_t * drm_dev, const char * name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_plane_property_id(drm_dev, name);

    if(!prop_id) {
        LV_LOG_ERROR("Couldn't find plane prop %s", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev->req, drm_dev->plane_id, get_plane_property_id(drm_dev, name), value);
    if(ret < 0) {
        LV_LOG_ERROR("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed: %d", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_crtc_property(drm_dev_t * drm_dev, const char * name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_crtc_property_id(drm_dev, name);

    if(!prop_id) {
        LV_LOG_ERROR("Couldn't find crtc prop %s", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev->req, drm_dev->crtc_id, get_crtc_property_id(drm_dev, name), value);
    if(ret < 0) {
        LV_LOG_ERROR("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed: %d", name, value, ret);
        return ret;
    }

    return 0;
}

static int drm_add_conn_property(drm_dev_t * drm_dev, const char * name, uint64_t value)
{
    int ret;
    uint32_t prop_id = get_conn_property_id(drm_dev, name);

    if(!prop_id) {
        LV_LOG_ERROR("Couldn't find conn prop %s", name);
        return -1;
    }

    ret = drmModeAtomicAddProperty(drm_dev->req, drm_dev->conn_id, get_conn_property_id(drm_dev, name), value);
    if(ret < 0) {
        LV_LOG_ERROR("drmModeAtomicAddProperty (%s:%" PRIu64 ") failed: %d", name, value, ret);
        return ret;
    }

    return 0;
}

#if !LV_LINUX_DRM_USE_EGL
static int drm_dmabuf_set_plane(drm_dev_t * drm_dev, drm_buffer_t * buf)
{
    int ret;
    static int first = 1;
    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;

#if LV_USE_LINUX_DRM_GBM_BUFFERS && !LV_LINUX_DRM_USE_EGL

    struct dma_buf_sync sync_req;

    sync_req.flags = DMA_BUF_SYNC_END | DMA_BUF_SYNC_RW;
    if(ioctl(buf->handle, DMA_BUF_IOCTL_SYNC, &sync_req) != 0) {
        LV_LOG_ERROR("Failed to end DMA-BUF R/W SYNC");
    }

#endif

    drm_dev->req = drmModeAtomicAlloc();

    /* On first Atomic commit, do a modeset */
    if(first) {
        drm_add_conn_property(drm_dev, "CRTC_ID", drm_dev->crtc_id);

        drm_add_crtc_property(drm_dev, "MODE_ID", drm_dev->blob_id);
        drm_add_crtc_property(drm_dev, "ACTIVE", 1);

        flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;

        first = 0;
    }

    drm_add_plane_property(drm_dev, "FB_ID", buf->fb_handle);
    drm_add_plane_property(drm_dev, "CRTC_ID", drm_dev->crtc_id);
    drm_add_plane_property(drm_dev, "SRC_X", 0);
    drm_add_plane_property(drm_dev, "SRC_Y", 0);
    drm_add_plane_property(drm_dev, "SRC_W", drm_dev->width << 16);
    drm_add_plane_property(drm_dev, "SRC_H", drm_dev->height << 16);
    drm_add_plane_property(drm_dev, "CRTC_X", 0);
    drm_add_plane_property(drm_dev, "CRTC_Y", 0);
    drm_add_plane_property(drm_dev, "CRTC_W", drm_dev->width);
    drm_add_plane_property(drm_dev, "CRTC_H", drm_dev->height);

    ret = drmModeAtomicCommit(drm_dev->fd, drm_dev->req, flags, drm_dev);
    if(ret) {
        LV_LOG_ERROR("drmModeAtomicCommit failed: %s (%d)", strerror(errno), errno);
        drmModeAtomicFree(drm_dev->req);
        return ret;
    }

    return 0;
}
#endif /*!LV_LINUX_DRM_USE_EGL*/

static int find_plane(drm_dev_t * drm_dev, unsigned int fourcc, uint32_t * plane_id, uint32_t crtc_id,
                      uint32_t crtc_idx)
{
    LV_UNUSED(crtc_id);
    drmModePlaneResPtr planes;
    drmModePlanePtr plane;
    unsigned int i;
    unsigned int j;
    int ret = 0;
    unsigned int format = fourcc;

    planes = drmModeGetPlaneResources(drm_dev->fd);
    if(!planes) {
        LV_LOG_ERROR("drmModeGetPlaneResources failed");
        return -1;
    }

    LV_LOG_TRACE("drm: found planes %u", planes->count_planes);

    for(i = 0; i < planes->count_planes; ++i) {
        plane = drmModeGetPlane(drm_dev->fd, planes->planes[i]);
        if(!plane) {
            LV_LOG_ERROR("drmModeGetPlane failed: %s", strerror(errno));
            break;
        }

        if(!(plane->possible_crtcs & (1 << crtc_idx))) {
            drmModeFreePlane(plane);
            continue;
        }

        for(j = 0; j < plane->count_formats; ++j) {
            if(plane->formats[j] == format)
                break;
        }

        if(j == plane->count_formats) {
            drmModeFreePlane(plane);
            continue;
        }

        *plane_id = plane->plane_id;
        drmModeFreePlane(plane);

        LV_LOG_TRACE("found plane %d", *plane_id);

        break;
    }

    if(i == planes->count_planes)
        ret = -1;

    drmModeFreePlaneResources(planes);

    return ret;
}

static int drm_find_connector(drm_dev_t * drm_dev, int64_t connector_id)
{
    drmModeConnector * conn = NULL;
    drmModeEncoder * enc = NULL;
    drmModeRes * res;
    int i;

    if((res = drmModeGetResources(drm_dev->fd)) == NULL) {
        LV_LOG_ERROR("drmModeGetResources() failed");
        return -1;
    }

    if(res->count_crtcs <= 0) {
        LV_LOG_ERROR("no Crtcs");
        goto free_res;
    }

    /* find all available connectors */
    for(i = 0; i < res->count_connectors; i++) {
        conn = drmModeGetConnector(drm_dev->fd, res->connectors[i]);
        if(!conn)
            continue;

        if(connector_id >= 0 && conn->connector_id != connector_id) {
            drmModeFreeConnector(conn);
            continue;
        }

        if(conn->connection == DRM_MODE_CONNECTED) {
            LV_LOG_TRACE("drm: connector %d: connected", conn->connector_id);
        }
        else if(conn->connection == DRM_MODE_DISCONNECTED) {
            LV_LOG_TRACE("drm: connector %d: disconnected", conn->connector_id);
        }
        else if(conn->connection == DRM_MODE_UNKNOWNCONNECTION) {
            LV_LOG_TRACE("drm: connector %d: unknownconnection", conn->connector_id);
        }
        else {
            LV_LOG_TRACE("drm: connector %d: unknown", conn->connector_id);
        }

        if(conn->connection == DRM_MODE_CONNECTED && conn->count_modes > 0)
            break;

        drmModeFreeConnector(conn);
        conn = NULL;
    };

    if(!conn) {
        LV_LOG_ERROR("suitable connector not found");
        goto free_res;
    }

    drm_dev->conn_id = conn->connector_id;
    LV_LOG_TRACE("conn_id: %d", drm_dev->conn_id);
    drm_dev->mmWidth = conn->mmWidth;
    drm_dev->mmHeight = conn->mmHeight;

    lv_memcpy(&drm_dev->mode, &conn->modes[0], sizeof(drmModeModeInfo));

    if(drmModeCreatePropertyBlob(drm_dev->fd, &drm_dev->mode, sizeof(drm_dev->mode),
                                 &drm_dev->blob_id)) {
        LV_LOG_ERROR("error creating mode blob");
        goto free_res;
    }

    drm_dev->width = conn->modes[0].hdisplay;
    drm_dev->height = conn->modes[0].vdisplay;

    for(i = 0 ; i < res->count_encoders; i++) {
        enc = drmModeGetEncoder(drm_dev->fd, res->encoders[i]);
        if(!enc)
            continue;

        LV_LOG_TRACE("enc%d enc_id %d conn enc_id %d", i, enc->encoder_id, conn->encoder_id);

        if(enc->encoder_id == conn->encoder_id)
            break;

        drmModeFreeEncoder(enc);
        enc = NULL;
    }

    if(enc) {
        drm_dev->enc_id = enc->encoder_id;
        LV_LOG_TRACE("enc_id: %d", drm_dev->enc_id);
        drm_dev->crtc_id = enc->crtc_id;
        LV_LOG_TRACE("crtc_id: %d", drm_dev->crtc_id);
        drmModeFreeEncoder(enc);
    }
    else {
        /* Encoder hasn't been associated yet, look it up */
        for(i = 0; i < conn->count_encoders; i++) {
            int crtc, crtc_id = -1;

            enc = drmModeGetEncoder(drm_dev->fd, conn->encoders[i]);
            if(!enc)
                continue;

            for(crtc = 0 ; crtc < res->count_crtcs; crtc++) {
                uint32_t crtc_mask = 1 << crtc;

                crtc_id = res->crtcs[crtc];

                LV_LOG_TRACE("enc_id %d crtc%d id %d mask %x possible %x", enc->encoder_id, crtc, crtc_id, crtc_mask,
                             enc->possible_crtcs);

                if(enc->possible_crtcs & crtc_mask)
                    break;
            }

            if(crtc_id > 0) {
                drm_dev->enc_id = enc->encoder_id;
                LV_LOG_TRACE("enc_id: %d", drm_dev->enc_id);
                drm_dev->crtc_id = crtc_id;
                LV_LOG_TRACE("crtc_id: %d", drm_dev->crtc_id);
                break;
            }

            drmModeFreeEncoder(enc);
            enc = NULL;
        }

        if(!enc) {
            LV_LOG_ERROR("suitable encoder not found");
            goto free_res;
        }

        drmModeFreeEncoder(enc);
    }

    drm_dev->crtc_idx = UINT32_MAX;

    for(i = 0; i < res->count_crtcs; ++i) {
        if(drm_dev->crtc_id == res->crtcs[i]) {
            drm_dev->crtc_idx = i;
            break;
        }
    }

    if(drm_dev->crtc_idx == UINT32_MAX) {
        LV_LOG_ERROR("drm: CRTC not found");
        goto free_res;
    }

    LV_LOG_TRACE("crtc_idx: %d", drm_dev->crtc_idx);

    return 0;

free_res:
    drmModeFreeResources(res);

    return -1;
}

static int drm_open(const char * path)
{
    int fd, flags;
    uint64_t has_dumb;
    int ret;

    fd = open(path, O_RDWR);
    if(fd < 0) {
        LV_LOG_ERROR("cannot open \"%s\"", path);
        return -1;
    }

    /* set FD_CLOEXEC flag */
    if((flags = fcntl(fd, F_GETFD)) < 0 ||
       fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0) {
        LV_LOG_ERROR("fcntl FD_CLOEXEC failed");
        goto err;
    }

    /* check capability */
    ret = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
    if(ret < 0 || has_dumb == 0) {
        LV_LOG_ERROR("drmGetCap DRM_CAP_DUMB_BUFFER failed or \"%s\" doesn't have dumb "
                     "buffer", path);
        goto err;
    }

    return fd;
err:
    close(fd);
    return -1;
}

static int drm_setup(drm_dev_t * drm_dev, const char * device_path, int64_t connector_id, unsigned int fourcc)
{
    int ret;

    drm_dev->fd = drm_open(device_path);
    if(drm_dev->fd < 0)
        return -1;

    ret = drmSetClientCap(drm_dev->fd, DRM_CLIENT_CAP_ATOMIC, 1);
    if(ret) {
        LV_LOG_ERROR("No atomic modesetting support: %s", strerror(errno));
        goto err;
    }

    ret = drm_find_connector(drm_dev, connector_id);
    if(ret) {
        LV_LOG_ERROR("available drm devices not found");
        goto err;
    }

    ret = find_plane(drm_dev, fourcc, &drm_dev->plane_id, drm_dev->crtc_id, drm_dev->crtc_idx);
    if(ret) {
        LV_LOG_ERROR("Cannot find plane");
        goto err;
    }

    drm_dev->plane = drmModeGetPlane(drm_dev->fd, drm_dev->plane_id);
    if(!drm_dev->plane) {
        LV_LOG_ERROR("Cannot get plane");
        goto err;
    }

    drm_dev->crtc = drmModeGetCrtc(drm_dev->fd, drm_dev->crtc_id);
    if(!drm_dev->crtc) {
        LV_LOG_ERROR("Cannot get crtc");
        goto err;
    }

    drm_dev->conn = drmModeGetConnector(drm_dev->fd, drm_dev->conn_id);
    if(!drm_dev->conn) {
        LV_LOG_ERROR("Cannot get connector");
        goto err;
    }

    ret = drm_get_plane_props(drm_dev);
    if(ret) {
        LV_LOG_ERROR("Cannot get plane props");
        goto err;
    }

    ret = drm_get_crtc_props(drm_dev);
    if(ret) {
        LV_LOG_ERROR("Cannot get crtc props");
        goto err;
    }

    ret = drm_get_conn_props(drm_dev);
    if(ret) {
        LV_LOG_ERROR("Cannot get connector props");
        goto err;
    }

    drm_dev->drm_event_ctx.version = DRM_EVENT_CONTEXT_VERSION;
    drm_dev->drm_event_ctx.page_flip_handler = page_flip_handler;
    drm_dev->fourcc = fourcc;

    LV_LOG_INFO("drm: Found plane_id: %u connector_id: %d crtc_id: %d",
                drm_dev->plane_id, drm_dev->conn_id, drm_dev->crtc_id);

    LV_LOG_INFO("drm: %dx%d (%dmm X% dmm) pixel format %c%c%c%c",
                drm_dev->width, drm_dev->height, drm_dev->mmWidth, drm_dev->mmHeight,
                (fourcc >> 0) & 0xff, (fourcc >> 8) & 0xff, (fourcc >> 16) & 0xff, (fourcc >> 24) & 0xff);


#if LV_USE_LINUX_DRM_GBM_BUFFERS

    /* Create GBM device and buffer */
    drm_dev->gbm_device = gbm_create_device(drm_dev->fd);

    if(drm_dev->gbm_device == NULL) {
        LV_LOG_ERROR("Failed to create GBM device");
        goto err;
    }

    LV_LOG_INFO("GBM device backend: %s", gbm_device_get_backend_name(drm_dev->gbm_device));
#endif

#if LV_LINUX_DRM_USE_EGL

    /* Add support to create a surface with modifiers */
    drm_dev->surface = gbm_surface_create(drm_dev->gbm_device,
                                          drm_dev->width, drm_dev->height, GBM_BO_FORMAT_ARGB8888,
                                          GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    if(!drm_dev->surface) {
        LV_LOG_ERROR("failed to create gbm surface");
        gbm_device_destroy(drm_dev->gbm_device);
        goto err;
    }

#endif

    return 0;

err:
    close(drm_dev->fd);
    return -1;
}

#if !LV_USE_LINUX_DRM_GBM_BUFFERS
static int drm_allocate_dumb(drm_dev_t * drm_dev, drm_buffer_t * buf)
{
    struct drm_mode_create_dumb creq;
    struct drm_mode_map_dumb mreq;
    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};
    int ret;

    /* create dumb buffer */
    lv_memzero(&creq, sizeof(creq));
    creq.width = drm_dev->width;
    creq.height = drm_dev->height;
    creq.bpp = LV_COLOR_DEPTH;
    ret = drmIoctl(drm_dev->fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if(ret < 0) {
        LV_LOG_ERROR("DRM_IOCTL_MODE_CREATE_DUMB fail");
        return -1;
    }

    buf->handle = creq.handle;
    buf->pitch = creq.pitch;
    buf->size = creq.size;

    /* prepare buffer for memory mapping */
    lv_memzero(&mreq, sizeof(mreq));
    mreq.handle = creq.handle;
    ret = drmIoctl(drm_dev->fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if(ret) {
        LV_LOG_ERROR("DRM_IOCTL_MODE_MAP_DUMB fail");
        return -1;
    }

    buf->offset = mreq.offset;
    LV_LOG_INFO("size %lu pitch %u offset %u", buf->size, buf->pitch, buf->offset);

    /* perform actual memory mapping */
    buf->map = mmap(0, creq.size, PROT_READ | PROT_WRITE, MAP_SHARED, drm_dev->fd, mreq.offset);
    if(buf->map == MAP_FAILED) {
        LV_LOG_ERROR("mmap fail");
        return -1;
    }

    /* clear the framebuffer to 0 (= full transparency in ARGB8888) */
    lv_memzero(buf->map, creq.size);

    /* create framebuffer object for the dumb-buffer */
    handles[0] = creq.handle;
    pitches[0] = creq.pitch;
    offsets[0] = 0;
    ret = drmModeAddFB2(drm_dev->fd, drm_dev->width, drm_dev->height, drm_dev->fourcc,
                        handles, pitches, offsets, &buf->fb_handle, 0);
    if(ret) {
        LV_LOG_ERROR("drmModeAddFB fail");
        return -1;
    }

    return 0;
}
#endif /*!LV_USE_LINUX_DRM_GBM_BUFFERS*/

#if LV_USE_LINUX_DRM_GBM_BUFFERS && !LV_LINUX_DRM_USE_EGL

static int create_gbm_buffer(drm_dev_t * drm_dev, drm_buffer_t * buf)
{

    struct gbm_bo * gbm_bo;
    int prime_fd;
    uint32_t handles[4] = {0}, pitches[4] = {0}, offsets[4] = {0};
    uint32_t n_planes;
    int res;

    /* gbm_bo_format does not define anything other than ARGB8888 or XRGB8888 */
    if(LV_COLOR_DEPTH != 32) {
        LV_LOG_ERROR("Unsupported color format");
        return -1;
    }

    /* Create a linear GBM buffer object - best practice when modifiers are not used */
    if(!(gbm_bo = gbm_bo_create(drm_dev->gbm_device,
                                drm_dev->width, drm_dev->height, GBM_BO_FORMAT_XRGB8888,
                                GBM_BO_USE_SCANOUT | GBM_BO_USE_LINEAR))) {

        LV_LOG_ERROR("Unable to create gbm buffer object");
        return -1;
    }

    /* Currently only, one plane per dma-buf/prime fd is supported - but some GPUs feature
     * several planes (multiple fds or sometimes a single fd for multiple planes).
     * current implementation is kept simple for now */

    n_planes = gbm_bo_get_plane_count(gbm_bo);

    if(n_planes != 1) {
        LV_LOG_ERROR("The current implementation only supports a single plane per fd");
        return -1;
    }

    uint32_t h = gbm_bo_get_height(gbm_bo);
    pitches[0] = buf->pitch = gbm_bo_get_stride_for_plane(gbm_bo, 0);
    offsets[0] = buf->offset = gbm_bo_get_offset(gbm_bo, 0);
    buf->size = h * buf->pitch;

    LV_LOG_INFO("Created GBM BO of size: %lu pitch: %u offset: %u format: %s",
                buf->size, buf->pitch, buf->offset, drmGetFormatName(format));

    prime_fd = gbm_bo_get_fd_for_plane(gbm_bo, 0);

    if(prime_fd < 0) {
        LV_LOG_ERROR("Failed to get prime fd for plane 0");
        return -1;

    }

    buf->map = mmap(NULL, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED, prime_fd, 0);

    if(buf->map == MAP_FAILED) {
        LV_LOG_ERROR("Failed to mmap dma-buf fd.");
        return -1;
    }

    /* Used to perform DMA_BUF_SYNC ioctl calls during the rendering cycle */
    buf->handle = prime_fd;

    /* Convert prime fd to a libdrm buffer handle */
    drmPrimeFDToHandle(drm_dev->fd, buf->handle, &handles[0]);

    /* create libdrm framebuffer */
    res = drmModeAddFB2(drm_dev->fd, drm_dev->width, drm_dev->height, drm_dev->fourcc,
                        handles, pitches, offsets, &buf->fb_handle, 0);

    if(res) {
        LV_LOG_ERROR("drmModeAddFB2 failed");
        return -1;
    }

    if(drmCloseBufferHandle(drm_dev->fd, handles[0]) != 0) {
        LV_LOG_ERROR("drmCloseBufferHandle failed");
        return -1;
    }

    return 0;

}

#endif /* LV_USE_LINUX_DRM_GBM_BUFFERS && !LV_LINUX_DRM_USE_EGL*/

#if !LV_LINUX_DRM_USE_EGL
static int drm_setup_buffers(drm_dev_t * drm_dev)
{
    int ret;

#if LV_USE_LINUX_DRM_GBM_BUFFERS

    ret = create_gbm_buffer(drm_dev, &drm_dev->drm_bufs[0]);
    if(ret < 0) {
        return ret;
    }

    ret = create_gbm_buffer(drm_dev, &drm_dev->drm_bufs[1]);
    if(ret < 0) {
        return ret;
    }

#else

    /* Use dumb buffers */
    ret = drm_allocate_dumb(drm_dev, &drm_dev->drm_bufs[0]);
    if(ret)
        return ret;

    ret = drm_allocate_dumb(drm_dev, &drm_dev->drm_bufs[1]);
    if(ret)
        return ret;

#endif

    return 0;
}

static void drm_flush_wait(lv_display_t * disp)
{
    drm_dev_t * drm_dev = lv_display_get_driver_data(disp);

    struct pollfd pfd;
    pfd.fd = drm_dev->fd;
    pfd.events = POLLIN;

    while(drm_dev->req) {
        int ret;
        do {
            ret = poll(&pfd, 1, -1);
        } while(ret == -1 && errno == EINTR);

        if(ret > 0)
            drmHandleEvent(drm_dev->fd, &drm_dev->drm_event_ctx);
        else {
            LV_LOG_ERROR("poll failed: %s", strerror(errno));
            return;
        }
    }
}

static void drm_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{

    if(!lv_display_flush_is_last(disp)) return;

    LV_UNUSED(area);
    LV_UNUSED(px_map);
    drm_dev_t * drm_dev = lv_display_get_driver_data(disp);

    LV_ASSERT(drm_dev->act_buf != NULL);

    if(drm_dmabuf_set_plane(drm_dev, drm_dev->act_buf)) {
        LV_LOG_ERROR("Flush fail");
        return;
    }

    drm_dev->act_buf = NULL;

}
#endif /*!LV_LINUX_DRM_USE_EGL*/

#if LV_LINUX_DRM_USE_EGL

static void drm_gbm_egl_pre(lv_opengles_window_t * window)
{
    drm_dev_t * drm_dev = lv_opengles_egl_window_get_user_data(window);
    if(drm_dev->kms_out_fence_fd != -1) {
        EGLDisplay display = lv_opengles_egl_window_get_display(window);
        drm_dev->kms_fence = create_fence(drm_dev, display, drm_dev->kms_out_fence_fd);
        LV_ASSERT_NULL(drm_dev->kms_fence);

        drm_dev->kms_out_fence_fd = -1;

        int result = drm_dev->egl_wait_sync_khr(display, drm_dev->kms_fence, 0);
        LV_ASSERT(result == 1);
    }
}

static void drm_gbm_egl_post1(lv_opengles_window_t * window)
{
    drm_dev_t * drm_dev = lv_opengles_egl_window_get_user_data(window);
    EGLDisplay display = lv_opengles_egl_window_get_display(window);
    drm_dev->gpu_fence = create_fence(drm_dev, display, EGL_NO_NATIVE_FENCE_FD_ANDROID);
    LV_ASSERT_NULL(drm_dev->gpu_fence);
}

static void drm_gbm_egl_post2(lv_opengles_window_t * window)
{
    drm_dev_t * drm_dev = lv_opengles_egl_window_get_user_data(window);
    EGLDisplay display = lv_opengles_egl_window_get_display(window);

    drm_dev->kms_in_fence_fd = drm_dev->egl_dup_native_fence_fd_android(display, drm_dev->gpu_fence);
    bool res = drm_dev->egl_destroy_sync_khr(display, drm_dev->gpu_fence);
    LV_ASSERT(res);
    drm_dev->gpu_fence = NULL;
    LV_ASSERT(drm_dev->kms_in_fence_fd != -1);

    drm_buffer_t * fb;
    uint32_t flags = DRM_MODE_ATOMIC_NONBLOCK;
    int ret;

    /* Get the next bo to display */
    struct gbm_bo * next_bo = gbm_surface_lock_front_buffer(drm_dev->surface);
    if(!next_bo) {
        LV_LOG_ERROR("Failed to lock frontbuffer");
    }

    fb = drm_fb_get_from_bo(next_bo);
    if(!fb) {
        LV_LOG_ERROR("Failed to get a new framebuffer BO");
    }

    if(drm_dev->kms_fence) {
        EGLint status;

        do {
            status = drm_dev->egl_client_wait_sync_khr(display,
                                                       drm_dev->kms_fence,
                                                       0,
                                                       EGL_FOREVER_KHR);
        } while(status != EGL_CONDITION_SATISFIED_KHR);

        bool res = drm_dev->egl_destroy_sync_khr(display, drm_dev->kms_fence);
        LV_ASSERT(res);
        drm_dev->kms_fence = NULL;
    }

    ret = drm_atomic_commit(drm_dev, fb->fb_id, flags);
    if(ret) {
        LV_LOG_ERROR("failed to commit: %s", strerror(errno));
    }

    /* release last buffer to render on again: */
    if(drm_dev->bo)
        gbm_surface_release_buffer(drm_dev->surface, drm_dev->bo);
    drm_dev->bo = next_bo;

}


static drm_buffer_t * drm_fb_get_from_bo(struct gbm_bo * bo)
{
    int drm_fd = gbm_device_get_fd(gbm_bo_get_device(bo));
    drm_buffer_t * fb = gbm_bo_get_user_data(bo);
    uint32_t width, height, format, strides[4] = {0}, handles[4] = {0}, offsets[4] = {0};
    int ret = -1;

    if(fb)
        return fb;

    fb = lv_malloc_zeroed(sizeof(drm_buffer_t));

    width = gbm_bo_get_width(bo);
    height = gbm_bo_get_height(bo);
    format = gbm_bo_get_format(bo);
    const int num_planes = gbm_bo_get_plane_count(bo);
    for(int i = 0; i < num_planes; i++) {
        handles[i] = gbm_bo_get_handle_for_plane(bo, i).u32;
        strides[i] = gbm_bo_get_stride_for_plane(bo, i);
        offsets[i] = gbm_bo_get_offset(bo, i);
    }

    memcpy(handles, (uint32_t [4]) {
        gbm_bo_get_handle(bo).u32, 0, 0, 0
    }, 16);
    memcpy(strides, (uint32_t [4]) {
        gbm_bo_get_stride(bo), 0, 0, 0
    }, 16);
    memset(offsets, 0, 16);
    ret = drmModeAddFB2(drm_fd, width, height, format,
                        handles, strides, offsets, &fb->fb_id, 0);
    fb->fb_handle = fb->fb_id;

    if(ret) {
        LV_LOG_ERROR("failed to create fb: %s", strerror(errno));
        lv_free(fb);
        return NULL;
    }

    gbm_bo_set_user_data(bo, fb, drm_fb_destroy_callback);

    return fb;
}

static void drm_fb_destroy_callback(struct gbm_bo * bo, void * data)
{
    int drm_fd = gbm_device_get_fd(gbm_bo_get_device(bo));
    drm_buffer_t * fb = data;

    if(fb->fb_id)
        drmModeRmFB(drm_fd, fb->fb_id);

    lv_free(fb);
}

static int drm_atomic_commit(drm_dev_t * drm_dev, uint32_t fb_id, uint32_t flags)
{
    int ret;
    static int first = 1;

    drm_dev->req = drmModeAtomicAlloc();

    /* On first Atomic commit, do a modeset */
    if(first) {
        drm_add_conn_property(drm_dev, "CRTC_ID", drm_dev->crtc_id);

        drm_add_crtc_property(drm_dev, "MODE_ID", drm_dev->blob_id);
        drm_add_crtc_property(drm_dev, "ACTIVE", 1);

        flags |= DRM_MODE_ATOMIC_ALLOW_MODESET;

        first = 0;
    }

    drm_add_plane_property(drm_dev, "FB_ID", fb_id);
    drm_add_plane_property(drm_dev, "CRTC_ID", drm_dev->crtc_id);
    drm_add_plane_property(drm_dev, "SRC_X", 0);
    drm_add_plane_property(drm_dev, "SRC_Y", 0);
    drm_add_plane_property(drm_dev, "SRC_W", drm_dev->width << 16);
    drm_add_plane_property(drm_dev, "SRC_H", drm_dev->height << 16);
    drm_add_plane_property(drm_dev, "CRTC_X", 0);
    drm_add_plane_property(drm_dev, "CRTC_Y", 0);
    drm_add_plane_property(drm_dev, "CRTC_W", drm_dev->width);
    drm_add_plane_property(drm_dev, "CRTC_H", drm_dev->height);

    if(drm_dev->kms_in_fence_fd != -1) {
        drm_add_crtc_property(drm_dev, "OUT_FENCE_PTR", (uintptr_t) &drm_dev->kms_out_fence_fd);
        drm_add_plane_property(drm_dev, "IN_FENCE_FD", drm_dev->kms_in_fence_fd);
    }

    ret = drmModeAtomicCommit(drm_dev->fd, drm_dev->req, flags, NULL);
    if(ret) {
        LV_LOG_ERROR("drmModeAtomicCommit failed: %s (%d)", strerror(errno), errno);
        drmModeAtomicFree(drm_dev->req);
        return ret;
    }

    if(drm_dev->kms_in_fence_fd != -1) {
        close(drm_dev->kms_in_fence_fd);
        drm_dev->kms_in_fence_fd = -1;
    }

    return 0;
}

static EGLSyncKHR create_fence(drm_dev_t * drm_dev, EGLDisplay display, int fd)
{
    EGLint attrib_list[] = {
        EGL_SYNC_NATIVE_FENCE_FD_ANDROID, fd,
        EGL_NONE,
    };
    EGLSyncKHR fence = drm_dev->egl_create_sync_khr(display, EGL_SYNC_NATIVE_FENCE_ANDROID, attrib_list);
    LV_ASSERT_NULL(fence);
    return fence;
}

#endif /*LV_LINUX_DRM_USE_EGL*/

static uint32_t tick_get_cb(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    uint64_t time_ms = t.tv_sec * 1000 + (t.tv_nsec / 1000000);
    return time_ms;
}

#endif /*LV_USE_LINUX_DRM*/

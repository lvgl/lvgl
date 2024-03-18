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
#include <unistd.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm_fourcc.h>

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
} drm_buffer_t;

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
    drm_buffer_t drm_bufs[2]; /*DUMB buffers*/
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
static int drm_dmabuf_set_plane(drm_dev_t * drm_dev, drm_buffer_t * buf);
static int find_plane(drm_dev_t * drm_dev, unsigned int fourcc, uint32_t * plane_id, uint32_t crtc_id,
                      uint32_t crtc_idx);
static int drm_find_connector(drm_dev_t * drm_dev, int64_t connector_id);
static int drm_open(const char * path);
static int drm_setup(drm_dev_t * drm_dev, const char * device_path, int64_t connector_id, unsigned int fourcc);
static int drm_allocate_dumb(drm_dev_t * drm_dev, drm_buffer_t * buf);
static int drm_setup_buffers(drm_dev_t * drm_dev);
static void drm_flush_wait(lv_display_t * drm_dev);
static void drm_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

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
    drm_dev_t * drm_dev = lv_malloc_zeroed(sizeof(drm_dev_t));
    LV_ASSERT_MALLOC(drm_dev);
    if(drm_dev == NULL) return NULL;

    lv_display_t * disp = lv_display_create(800, 480);
    if(disp == NULL) {
        lv_free(drm_dev);
        return NULL;
    }
    drm_dev->fd = -1;
    lv_display_set_driver_data(disp, drm_dev);
    lv_display_set_flush_wait_cb(disp, drm_flush_wait);
    lv_display_set_flush_cb(disp, drm_flush);

    return disp;
}

void lv_linux_drm_set_file(lv_display_t * disp, const char * file, int64_t connector_id)
{
    drm_dev_t * drm_dev = lv_display_get_driver_data(disp);
    int ret;

    ret = drm_setup(drm_dev, file, connector_id, DRM_FOURCC);
    if(ret) {
        close(drm_dev->fd);
        drm_dev->fd = -1;
        return;
    }

    ret = drm_setup_buffers(drm_dev);
    if(ret) {
        LV_LOG_ERROR("DRM buffer allocation failed");
        close(drm_dev->fd);
        drm_dev->fd = -1;
        return;
    }

    LV_LOG_INFO("DRM subsystem and buffer mapped successfully");

    int32_t hor_res = drm_dev->width;
    int32_t ver_res = drm_dev->height;
    int32_t width = drm_dev->mmWidth;

    size_t buf_size = LV_MIN(drm_dev->drm_bufs[1].size, drm_dev->drm_bufs[0].size);
    lv_display_set_buffers(disp, drm_dev->drm_bufs[1].map, drm_dev->drm_bufs[0].map, buf_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_resolution(disp, hor_res, ver_res);

    if(width) {
        lv_display_set_dpi(disp, DIV_ROUND_UP(hor_res * 25400, width * 1000));
    }

    LV_LOG_INFO("Resolution is set to %" LV_PRId32 "x%" LV_PRId32 " at %" LV_PRId32 "dpi",
                hor_res, ver_res, lv_display_get_dpi(disp));
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

static int drm_dmabuf_set_plane(drm_dev_t * drm_dev, drm_buffer_t * buf)
{
    int ret;
    static int first = 1;
    uint32_t flags = DRM_MODE_PAGE_FLIP_EVENT | DRM_MODE_ATOMIC_NONBLOCK;

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

    return 0;

err:
    close(drm_dev->fd);
    return -1;
}

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

static int drm_setup_buffers(drm_dev_t * drm_dev)
{
    int ret;

    /*Allocate DUMB buffers*/
    ret = drm_allocate_dumb(drm_dev, &drm_dev->drm_bufs[0]);
    if(ret)
        return ret;

    ret = drm_allocate_dumb(drm_dev, &drm_dev->drm_bufs[1]);
    if(ret)
        return ret;

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

    for(int idx = 0; idx < 2; idx++) {
        if(drm_dev->drm_bufs[idx].map == px_map) {
            /*Request buffer swap*/
            if(drm_dmabuf_set_plane(drm_dev, &drm_dev->drm_bufs[idx])) {
                LV_LOG_ERROR("Flush fail");
                return;
            }
            else
                LV_LOG_TRACE("Flush done");
        }
    }
}

#endif /*LV_USE_LINUX_DRM*/

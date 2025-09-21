/**
 * @file lv_egl_adapter_outmod_drm.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"
#include "../../../../misc/lv_array.h"
#include "../../../../misc/lv_log.h"

#include "../../../opengles/egl_adapter_runtime_defaults.h"
#include "../../../opengles/egl_adapter_runtime_defaults_internal.h"

#include "lv_egl_adapter_outmod_drm-helpers.h"
#include "../../../opengles/egl_adapter/private/lv_egl_adapter_output_core.h"
#include "lv_egl_adapter_outmod_drm.h"
#include <drm_fourcc.h> /* for drm_format definitions if needed */
#include <errno.h>
#include <fcntl.h>
//#include <libudev.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/input.h>  /* for input_event */

#include <drm.h>
#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <libgen.h> /* not strictly needed */

/*********************
 *      DEFINES
 *********************/

/* width of interior content area (adjust if you want wider box) */
#define BOX_WIDTH 23
#define MAX_LINE_LEN 256

/**********************
 *      TYPEDEFS
 **********************/

typedef enum { DISP_MINIMAL = 0, DISP_LIST = 1, DISP_TABLED = 2 } display_mode_t;

struct drmfb_state {
    int fd;
    struct gbm_bo * bo;
    uint32_t fb_id;
};

typedef struct drmfb_state * drmfb_state_t;

/* internal struct definition (matches header's opaque typedef) */
struct lv_egl_adapter_outmod_drm {
    lv_egl_adapter_output_core_t core;
    int fd;
    drmModeRes * drm_resources;
    drmModeConnector * drm_connector;
    drmModeEncoder * drm_encoder;
    drmModeCrtcPtr drm_crtc;
    drmModeModeInfo * drm_mode;
    struct gbm_device * gbm_dev;
    struct gbm_surface * gbm_surface;
    struct gbm_bo * gbm_bo_pending;
    struct gbm_bo * gbm_bo_flipped;
    struct gbm_bo * gbm_bo_presented;
    bool crtc_isset;
    bool async_flip;
    bool quit_flag;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void print_grouped_modes(drmModeConnectorPtr conn);
static void print_grouped_modes_ex(drmModeConnectorPtr conn, uint32_t current_idx, display_mode_t mode,
                                   const char ** banner_lines, int banner_count);
static size_t distance_uint32_array(uint32_t * arr, size_t len, uint32_t value);
static void populate_output_core(void * outmod_ptr);

/* See also: lv_egl_adapter_outmod_drm-helpers.h */

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_egl_adapter_outmod_drm_page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec,
                                                 void * data)
{
    (void)fd;
    (void)frame;
    (void)sec;
    (void)usec;
    //(void)data;

    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)data;

    if(drm_out->gbm_bo_presented)
        gbm_surface_release_buffer(drm_out->gbm_surface, drm_out->gbm_bo_presented);
    drm_out->gbm_bo_presented = drm_out->gbm_bo_flipped;
    drm_out->gbm_bo_flipped = NULL;

}

void lv_egl_adapter_outmod_drm_cleanup(void * nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out) return;

    if(drm_out->drm_crtc) {
        int status = drmModeSetCrtc(drm_out->fd,
                                    drm_out->drm_crtc->crtc_id,
                                    drm_out->drm_crtc->buffer_id,
                                    drm_out->drm_crtc->x,
                                    drm_out->drm_crtc->y,
                                    &drm_out->drm_connector->connector_id,
                                    1,
                                    &drm_out->drm_crtc->mode);
        if(status < 0) {
            /* Log.error expected externally; keep silent if not present */
        }
        drmModeFreeCrtc(drm_out->drm_crtc);
        drm_out->drm_crtc = 0;
    }

    if(drm_out->gbm_surface) {
        gbm_surface_destroy(drm_out->gbm_surface);
        drm_out->gbm_surface = 0;
    }

    if(drm_out->gbm_dev) {
        gbm_device_destroy(drm_out->gbm_dev);
        drm_out->gbm_dev = 0;
    }

    if(drm_out->drm_connector) {
        drmModeFreeConnector(drm_out->drm_connector);
        drm_out->drm_connector = 0;
    }

    if(drm_out->drm_encoder) {
        drmModeFreeEncoder(drm_out->drm_encoder);
        drm_out->drm_encoder = 0;
    }

    if(drm_out->drm_resources) {
        drmModeFreeResources(drm_out->drm_resources);
        drm_out->drm_resources = 0;
    }

    if(drm_out->fd > 0) {
        drmClose(drm_out->fd);
    }
    free(drm_out->core);
    drm_out->fd = 0;
    drm_out->drm_mode = 0;
}

void lv_egl_adapter_outmod_drm_destroy(void ** nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = * (lv_egl_adapter_outmod_drm_t *) nativedrm_ptr;
    if(!drm_out) return;
    lv_egl_adapter_outmod_drm_cleanup(drm_out);
    free(drm_out);
    *nativedrm_ptr = NULL;
}

lv_egl_adapter_output_core_t lv_egl_adapter_outmod_drm_get_core(void * nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    return drm_out->core;
}

/* display: return internal gbm_device pointer as void* */
void * lv_egl_adapter_outmod_drm_display(void * nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out) return NULL;
    return (void *) drm_out->gbm_dev;
}

/* window: populate properties with mode size and return gbm_surface* as void* */
void * lv_egl_adapter_outmod_drm_window(void * nativedrm_ptr, native_window_properties_t const properties)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out || !properties) return NULL;
    if(drm_out->drm_mode) {
        properties->width = drm_out->drm_mode->hdisplay;
        properties->height = drm_out->drm_mode->vdisplay;
        properties->fullscreen = true;
    }
    else {
        properties->width = 0;
        properties->height = 0;
        properties->fullscreen = false;
    }
    properties->visual_id = 0;
    lv_array_init(&properties->modifiers, 1, sizeof(uint64_t));
    return (void *) drm_out->gbm_surface;
}

/* visible: no-op */
void lv_egl_adapter_outmod_drm_visible(void * nativedrm_ptr, bool v)
{
    (void)nativedrm_ptr;
    (void)v;
}

void lv_egl_adapter_outmod_drm_fb_destroy_callback(struct gbm_bo * bo, void * data)
{
    struct drmfb_state * fb = (struct drmfb_state *) data;
    if(fb && fb->fb_id) drmModeRmFB(fb->fd, fb->fb_id);
    free(fb);
    (void)bo;
}

/* init_gbm: create gbm device from fd */
bool lv_egl_adapter_outmod_drm_init_gbm(void * nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out) return false;
    drm_out->gbm_dev = gbm_create_device(drm_out->fd);
    if(!drm_out->gbm_dev) {
        /* logging expected to be provided externally */
        return false;
    }
    return true;
}

/* check_for_page_flip: select() on fd and handle drm events */
int lv_egl_adapter_outmod_drm_check_for_page_flip(void * nativedrm_ptr, int timeout_ms)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out) return -1;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(drm_out->fd, &fds);

    drmEventContext evCtx;
    memset(&evCtx, 0, sizeof(evCtx));
    evCtx.version = 2;
    /* page_flip_handler must be implemented elsewhere as:
       void lv_egl_adapter_outmod_drm_page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data);
       and registered here by setting evCtx.page_flip_handler = lv_egl_adapter_outmod_drm_page_flip_handler;
    */
    //extern void lv_egl_adapter_outmod_drm_page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void *data);
    evCtx.page_flip_handler = lv_egl_adapter_outmod_drm_page_flip_handler;

    struct timeval timeout;
    struct timeval * timeout_ptr = NULL;
    if(timeout_ms >= 0) {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        timeout_ptr = &timeout;
    }

    int status = select(drm_out->fd + 1, &fds, NULL, NULL, timeout_ptr);
    if(status == 1) {
        drmHandleEvent(drm_out->fd, &evCtx);
    }
    return status;
}

drmfb_state_t lv_egl_adapter_outmod_drm_fb_get_from_bo(void * nativedrm_ptr, struct gbm_bo * bo)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)nativedrm_ptr;
    if(!bo) {
        LV_LOG_ERROR("variable bo is null\n");
        return NULL;
    }
    drmfb_state_t fb = (drmfb_state_t)gbm_bo_get_user_data(bo);
    if(fb) {
        unsigned int width = gbm_bo_get_width(bo);
        unsigned int height = gbm_bo_get_height(bo);
        return fb;
    }

    unsigned int width = gbm_bo_get_width(bo);
    unsigned int height = gbm_bo_get_height(bo);
    unsigned int handles[4] = { 0, }, strides[4] = { 0, }, offsets[4] = { 0, };
    uint64_t modifiers[4] = {0};
    unsigned int format = gbm_bo_get_format(bo);
    uint64_t modifier = gbm_bo_get_modifier(bo);
    unsigned int fb_id = 0;
    uint64_t addfb2_mods = 0;
    int status;

    drmGetCap(drm_out->fd, DRM_CAP_ADDFB2_MODIFIERS, &addfb2_mods);

    for(int i = 0; i < gbm_bo_get_plane_count(bo); i++) {
        handles[i] = gbm_bo_get_handle_for_plane(bo, i).u32;
        strides[i] = gbm_bo_get_stride_for_plane(bo, i);
        offsets[i] = gbm_bo_get_offset(bo, i);
        modifiers[i] = modifier;
    }

    if(addfb2_mods && modifier != DRM_FORMAT_MOD_INVALID) {
        status = drmModeAddFB2WithModifiers(drm_out->fd, width, height, format,
                                            handles, strides, offsets, modifiers,
                                            &fb_id, DRM_MODE_FB_MODIFIERS);
    }
    else {
        status = drmModeAddFB2(drm_out->fd, width, height, format,
                               handles, strides, offsets, &fb_id, 0);
    }

    if(status < 0) {
        LV_LOG_ERROR("Failed to create FB: %d", status);
        return NULL;
    }

    fb = (drmfb_state_t)malloc(sizeof(*fb));
    if(!fb) {
        LV_LOG_ERROR("Failed to allocate drmfb_state");
        return NULL;
    }

    fb->fd = drm_out->fd;
    fb->bo = bo;
    fb->fb_id = fb_id;

    gbm_bo_set_user_data(bo, fb, lv_egl_adapter_outmod_drm_fb_destroy_callback);
    //gbm_bo_set_user_data(bo, fb, (void *)lv_egl_adapter_outmod_drm_fb_destroy_callback);
    LV_LOG_USER("Allocated new drmfb_state, fb_id = %d", fb_id);

    return fb;
}

void lv_egl_adapter_outmod_drm_flip(void * nativedrm_ptr, bool vsync)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;

    if(!drm_out->crtc_isset && drmSetMaster(drm_out->fd) < 0) {
        LV_LOG_ERROR("Failed to become DRM master (hint: must be run in a VT, shut down Wayland / X first )");
        drm_out->quit_flag = true;
        return;
    }

    if(drm_out->gbm_bo_pending) gbm_surface_release_buffer(drm_out->gbm_surface, drm_out->gbm_bo_pending);
    drm_out->gbm_bo_pending = gbm_surface_lock_front_buffer(drm_out->gbm_surface);

    if(!drm_out->gbm_bo_pending)
        LV_LOG_ERROR("gbm_bo_pending = NULL");

    drmfb_state_t pending_fb = lv_egl_adapter_outmod_drm_fb_get_from_bo(drm_out, drm_out->gbm_bo_pending);
    if(!drm_out->gbm_bo_pending || !pending_fb) {
        LV_LOG_ERROR("Failed to get gbm front buffer");
        return;
    }

    if(vsync || drm_out->async_flip) {
        while(drm_out->gbm_bo_flipped && lv_egl_adapter_outmod_drm_check_for_page_flip(drm_out, -1) >= 0)
            continue;
    }
    else {
        lv_egl_adapter_outmod_drm_check_for_page_flip(drm_out, 0);
    }

    if(!drm_out->gbm_bo_flipped) {
        if(!drm_out->crtc_isset) {
            int status = drmModeSetCrtc(drm_out->fd, drm_out->drm_encoder->crtc_id, pending_fb->fb_id, 0, 0,
                                        &(drm_out->drm_connector->connector_id), 1, drm_out->drm_mode);
            if(status >= 0) {
                drm_out->crtc_isset = true;
                if(drm_out->gbm_bo_presented)
                    gbm_surface_release_buffer(drm_out->gbm_surface, drm_out->gbm_bo_presented);
                drm_out->gbm_bo_presented = drm_out->gbm_bo_pending;
                drm_out->gbm_bo_flipped = NULL;
                drm_out->gbm_bo_pending = NULL;
            }
            else {
                LV_LOG_ERROR("Failed to set crtc: %d", status);
            }
            return;
        }

        uint32_t flip_flags = DRM_MODE_PAGE_FLIP_EVENT;
        if(drm_out->async_flip)
            flip_flags |= DRM_MODE_PAGE_FLIP_ASYNC;

        int status = drmModePageFlip(drm_out->fd, drm_out->drm_encoder->crtc_id, pending_fb->fb_id, flip_flags, drm_out);
        if(status < 0) {
            LV_LOG_ERROR("Failed to enqueue page flip: %d", status);
            return;
        }


        drm_out->gbm_bo_flipped = drm_out->gbm_bo_pending;
        drm_out->gbm_bo_pending = NULL;
    }

    /* We need to ensure our surface has a free buffer, otherwise GL will
     * have no buffer to render on. */
    while(!gbm_surface_has_free_buffers(drm_out->gbm_surface) &&
          lv_egl_adapter_outmod_drm_check_for_page_flip(drm_out, -1) >= 0) continue;
}

bool lv_egl_adapter_outmod_drm_create_window(void * nativedrm_ptr, native_window_properties_t const properties)
{

    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)nativedrm_ptr;
    if(!drm_out->gbm_dev) {
        LV_LOG_ERROR("DRM device has not been initialized!");
        return false;
    }

    int crtc_index = (int)(distance_uint32_array(drm_out->drm_resources->crtcs,
                                                 drm_out->drm_resources->count_crtcs,
                                                 drm_out->drm_crtc->crtc_id));

    uint32_t format = properties->visual_id;
    size_t drm_mods_count = 0;
    uint64_t * drm_mods_buffer = drm_get_format_mods(drm_out->fd, format, crtc_index, &drm_mods_count);
    lv_array_t validated_mods_struct;
    lv_array_t * validated_mods = &validated_mods_struct;
    lv_array_init(validated_mods, 1, sizeof(uint64_t));

    if(lv_array_size(&properties->modifiers) != 0) {
        for(size_t di = 0; di < drm_mods_count; ++di) {
            uint64_t mod = drm_mods_buffer[di];
            for(size_t i = 0; i < lv_array_size(&properties->modifiers); ++i) {
                uint64_t pm = *(uint64_t *)lv_array_at(&properties->modifiers, i);
                if(mod == pm) {
                    lv_array_push_back(validated_mods, lv_array_at(&properties->modifiers, i));
                }
            }
        }
    }
    free(drm_mods_buffer);

    if(lv_array_is_empty(validated_mods) ||
       (lv_array_size(validated_mods) == 1 && (*(uint64_t *)lv_array_at(validated_mods, 0)) == DRM_FORMAT_MOD_INVALID)) {
        drm_out->gbm_surface = gbm_surface_create(drm_out->gbm_dev, drm_out->drm_mode->hdisplay, drm_out->drm_mode->vdisplay,
                                                  format, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    else {
#if HAVE_GBM_SURFACE_CREATE_WITH_MODIFIERS2
        drm_out->gbm_surface = gbm_surface_create_with_modifiers2(
                                   drm_out->gbm_dev, drm_out->drm_mode->hdisplay, drm_out->drm_mode->vdisplay,
                                   format, (const uint64_t *)lv_array_at(validated_mods, 0), lv_array_size(validated_mods),
                                   GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
#else
        drm_out->gbm_surface = gbm_surface_create_with_modifiers(drm_out->gbm_dev, drm_out->drm_mode->hdisplay,
                                                                 drm_out->drm_mode->vdisplay,
                                                                 format, (const uint64_t *)lv_array_at(validated_mods, 0), lv_array_size(validated_mods));
#endif
    }

    if(!drm_out->gbm_surface) {
        LV_LOG_ERROR("Failed to create GBM surface");
        lv_array_deinit(validated_mods);
        return false;
    }

    LV_LOG_USER("Created gbm_surface with format=0x%x and %s modifiers",
                format, lv_array_is_empty(validated_mods) ? "implicit" : "explicit");

    lv_array_deinit(validated_mods);
    return true;
}

inline static int node_path_is_valid(const char * path)
{
    return path && path[0] != '\0';
}

bool lv_egl_adapter_outmod_drm_init(void * nativedrm_ptr, int x_res, int y_res, float refr_rate)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)nativedrm_ptr;

    int fd = -1;
    char * drm_device = TEMP_get_default_drm_device_path(drm_out);
    if(!node_path_is_valid(drm_device)) {
        drm_device = find_drm_device_path();
    }
    if(node_path_is_valid(drm_device)) {
        fd = open(drm_device, O_RDWR);
        if(!(fd >= 0)) {
            LV_LOG_WARN("Tried to use '%s' but failed.\nReason : %s", drm_device, strerror(errno));
            return false;
        }
        else {
            LV_LOG_USER("Using DRM path: %s", drm_device);
        }
    }
    if(!(fd >= 0)) fd = open_drm_with_module_checking();
    if(!(fd >= 0)) {
        LV_LOG_ERROR("Failed to find a suitable DRM device");
        return false;
    }
    free(drm_device);
    drm_device = 0;

    drm_out->fd = fd;
    drmSetClientCap(drm_out->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    drm_out->drm_resources = drmModeGetResources(drm_out->fd);
    if(!drm_out->drm_resources) {
        LV_LOG_ERROR("drmModeGetResources failed");
        return false;
    }

    /* Find a connected connector */
    for(int c = 0; c < drm_out->drm_resources->count_connectors; c++) {
        drm_out->drm_connector = drmModeGetConnector(drm_out->fd, drm_out->drm_resources->connectors[c]);
        if(DRM_MODE_CONNECTED == drm_out->drm_connector->connection) break;
        drmModeFreeConnector(drm_out->drm_connector);
        drm_out->drm_connector = NULL;
    }

    if(!drm_out->drm_connector) {
        LV_LOG_ERROR("Failed to find a suitable connector");
        return false;
    }

    /* Find the best resolution (we will always operate full-screen). */
    unsigned int bestArea = 0;
    bool found_match = false;
    int match_mode_num = -1;
    float best_refr_diff = 999999.f;

    for(int m = 0; m < drm_out->drm_connector->count_modes; m++) {
        drmModeModeInfo * curMode = &drm_out->drm_connector->modes[m];
        float refresh_hertz = (curMode->clock * 1000.f) / (float)(curMode->htotal * curMode->vtotal);
        unsigned int curArea = curMode->hdisplay * curMode->vdisplay;
        if((curMode->hdisplay == x_res) && (curMode->vdisplay == y_res)) {
            found_match = true;
            float cur_refr_diff = refresh_hertz - refr_rate;
            cur_refr_diff = cur_refr_diff > 0.f ? cur_refr_diff : -cur_refr_diff;
            if(cur_refr_diff < best_refr_diff) {
                best_refr_diff = cur_refr_diff;
                drm_out->drm_mode = curMode;
                match_mode_num = m;
            }
        }
        else {
            if(!found_match && (curArea > bestArea)) {
                drm_out->drm_mode = curMode;
                bestArea = curArea;
                match_mode_num = m;
            }
        }
    }


    if(!drm_out->drm_mode) {
        LV_LOG_ERROR("Failed to find a suitable mode");
        print_grouped_modes(drm_out->drm_connector);
        return false;
    }
    else {
        float refresh_hertz = (drm_out->drm_mode->clock * 1000.f) / (float)(drm_out->drm_mode->htotal *
                                                                            drm_out->drm_mode->vtotal);
        LV_LOG_USER("Using display mode #%d: %d x %d @ %.1fhz", (match_mode_num + 1), drm_out->drm_mode->hdisplay,
                    drm_out->drm_mode->vdisplay, refresh_hertz);
        /*const char * banner[] = {
            ".____ ____   ____________.____         .__        ",
            "|    |\\   \\ /   /  _____/|    |        |__| ____  ",
            "|    | \\   Y   /   \\  ___|    |        |  |/  _ \\ ",
            "|    |__\\     /\\    \\_\\  \\    |___     |  (  <_> )",
            "|_______ \\___/  \\______  /_______ \\ /\\ |__|\\____/ ",
            "        \\/             \\/        \\/ \\/            "
        };*/
        //print_grouped_modes_ex(drm_out->drm_connector, (uint32_t)match_mode_num, DISP_MINIMAL, banner, 6);
    }

    /* Find a suitable encoder */
    for(int e = 0; e < drm_out->drm_resources->count_encoders; e++) {
        bool found = false;
        drm_out->drm_encoder = drmModeGetEncoder(drm_out->fd, drm_out->drm_resources->encoders[e]);
        for(int ce = 0; ce < drm_out->drm_connector->count_encoders; ce++) {
            if(drm_out->drm_encoder && drm_out->drm_encoder->encoder_id == drm_out->drm_connector->encoders[ce]) {
                found = true;
                break;
            }
        }
        if(found)
            break;
        drmModeFreeEncoder(drm_out->drm_encoder);
        drm_out->drm_encoder = NULL;
    }

    /* If encoder is not connected to the connector try to find a suitable one */
    if(!drm_out->drm_encoder) {
        for(int e = 0; e < drm_out->drm_connector->count_encoders; e++) {
            drm_out->drm_encoder = drmModeGetEncoder(drm_out->fd, drm_out->drm_connector->encoders[e]);
            for(int c = 0; c < drm_out->drm_resources->count_crtcs; c++) {
                if(drm_out->drm_encoder->possible_crtcs & (1 << c)) {
                    drm_out->drm_encoder->crtc_id = drm_out->drm_resources->crtcs[c];
                    break;
                }
            }
            if(drm_out->drm_encoder->crtc_id) break;
            drmModeFreeEncoder(drm_out->drm_encoder);
            drm_out->drm_encoder = NULL;
        }
    }

    if(!drm_out->drm_encoder) {
        LV_LOG_ERROR("Failed to find a suitable encoder");
        return false;
    }

    if(!lv_egl_adapter_outmod_drm_init_gbm(drm_out)) {
        return false;
    }

    drm_out->drm_crtc = drmModeGetCrtc(drm_out->fd, drm_out->drm_encoder->crtc_id);

    /* if there is no current CRTC, make sure to attach a suitable one */
    if(!drm_out->drm_crtc) {
        for(int c = 0; c < drm_out->drm_resources->count_crtcs; c++) {
            if(drm_out->drm_encoder->possible_crtcs & (1 << c)) {
                drm_out->drm_encoder->crtc_id = drm_out->drm_resources->crtcs[c];
                break;
            }
        }
    }

#if 0
    uint64_t cap_async;
    if(Options_swap_mode() == OPTIONS_SWAP_MODE_DEFAULT ||
       Options_swap_mode() == OPTIONS_SWAP_MODE_IMMEDIATE) {
        drm_out->async_flip = drmGetCap(drm_out->fd, DRM_CAP_ASYNC_PAGE_FLIP, &cap_async) == 0 && cap_async == 1;
        if(!drm_out->async_flip) {
            LV_LOG_WARN("DRM_CAP_ASYNC_PAGE_FLIP not supported, falling back to 'mailbox' mode for SwapInterval(0).");
        }
    }
    else {
        drm_out->async_flip = false;
    }
#else
    drm_out->async_flip = false;
#endif

    //signal(SIGINT, lv_egl_adapter_outmod_drm_quit_handler);
    return true;
}

/* init_display: call internal init if gbm_dev not set. Returns whether device exists. */
bool lv_egl_adapter_outmod_drm_init_display(void * nativedrm_ptr, int * x_res, int * y_res, float refr_rate)
{
    int twidth = *x_res;
    int theight = *y_res;
    *x_res = -1;
    *y_res = -1;
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t) nativedrm_ptr;
    if(!drm_out) return false;
    if(drm_out->gbm_dev == NULL) {
        lv_egl_adapter_outmod_drm_init(drm_out, twidth, theight, refr_rate);
    }
    return (drm_out->gbm_dev != NULL);
}

lv_egl_adapter_outmod_drm_t lv_egl_adapter_outmod_drm_create(void)
{
    lv_egl_adapter_outmod_drm_t drm_out;
    drm_out = (lv_egl_adapter_outmod_drm_t)calloc(1, sizeof(*drm_out));
    if(!drm_out) return NULL;
    populate_output_core(drm_out);
    drm_out->fd = 0;
    drm_out->drm_resources = NULL;
    drm_out->drm_connector = NULL;
    drm_out->drm_encoder = NULL;
    drm_out->drm_crtc = NULL;
    drm_out->drm_mode = NULL;
    drm_out->gbm_dev = NULL;
    drm_out->gbm_surface = NULL;
    drm_out->gbm_bo_pending = NULL;
    drm_out->gbm_bo_flipped = NULL;
    drm_out->gbm_bo_presented = NULL;
    drm_out->crtc_isset = false;
    drm_out->async_flip = false;
    drm_out->quit_flag = false;
    return drm_out;
}



/**********************
 *   STATIC FUNCTIONS
 **********************/

static void populate_output_core(void * outmod_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)outmod_ptr;
    drm_out->core = (lv_egl_adapter_output_core_t)malloc(sizeof(*(drm_out->core)));
    drm_out->core->destroy            = lv_egl_adapter_outmod_drm_destroy;
    drm_out->core->init_display       = lv_egl_adapter_outmod_drm_init_display;
    drm_out->core->display            = lv_egl_adapter_outmod_drm_display;
    drm_out->core->create_window      = lv_egl_adapter_outmod_drm_create_window;
    drm_out->core->window             = lv_egl_adapter_outmod_drm_window;
    drm_out->core->visible            = lv_egl_adapter_outmod_drm_visible;
    drm_out->core->flip               = lv_egl_adapter_outmod_drm_flip;
}

static size_t distance_uint32_array(uint32_t * arr, size_t len, uint32_t value)
{
    for(size_t i = 0; i < len; i++) if(arr[i] == value) return i;
    return len;
}

/* To automatically select the output card, return empty string from this
 * function.  Otherwise the return value is the only card path that will
 * be tried and if it is not valid the app will shutdown.  Start-up happens
 * fastest if this return value is explicitly specified (and valid).
 */
static char * TEMP_get_default_drm_device_path(void * nativedrm_ptr)
{
    lv_egl_adapter_outmod_drm_t drm_out = (lv_egl_adapter_outmod_drm_t)nativedrm_ptr;
    (void)drm_out;

    const char * s = LV_EGL_DEVICE;
    char * out = malloc(strlen(s) + 1);
    if(!out) return NULL;
    strcpy(out, s);
    return out;
}

/* AI generated this alternate quick and dirty way to find the currently connected
 * display device, but it's not really necessary with the other ways below.  It seems
 * to work though, and it's fairly minimal.
 */

static char * find_drm_device_path()
{
    const int max_cards = 8;
    char path[64];
    int found_index = 0;
    for(int ci = 0; ci < max_cards; ++ci) {
        snprintf(path, sizeof(path), "/sys/class/drm");
        DIR * d = opendir(path);
        if(!d) continue;
        struct dirent * ent;
        int found_connected = 0;
        while((ent = readdir(d)) != NULL) {
            if(strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            // connector dirs look like card0-HDMI-A-1, card0-eDP-1, etc.
            if(strncmp(ent->d_name, "card", 4) != 0 || strchr(ent->d_name, '-') == NULL) continue;
            found_connected = 1;
            found_index = ent->d_name[4] - '0';
            break;
        }
        closedir(d);
        if(found_connected) {
            char * out = malloc(strlen("/dev/dri/card") + 4);
            if(!out) return NULL;
            snprintf(out, strlen("/dev/dri/card") + 4, "/dev/dri/card%d", found_index);
            return out;
        }
    }
    // fallback: prefer card0 if exists, else first cardN
    for(int ci = 0; ci < max_cards; ++ci) {
        snprintf(path, sizeof(path), "/dev/dri/card%d", ci);
        if(access(path, R_OK) == 0) {
            char * out = malloc(strlen(path) + 1);
            if(!out) return NULL;
            strcpy(out, path);
            return out;
        }
    }
    return NULL;
}


/* Search props for property named `name`. If found, write value to out_value and return 1.
   Returns 0 if not found or on error.
   Note: drmModeGetProperty / drmModeFreeProperty are used and require linking with libdrm.
*/
static int drm_get_prop_value(int drm_fd, drmModeObjectProperties * props, const char * name, uint64_t * out_value)
{
    if(!props || !name || !out_value) return 0;

    for(uint32_t i = 0; i < props->count_props; ++i) {
        drmModePropertyRes * prop = drmModeGetProperty(drm_fd, props->props[i]);
        if(!prop) continue;

        if(strcmp(prop->name, name) == 0) {
            *out_value = props->prop_values[i];
            drmModeFreeProperty(prop);
            return 1;
        }

        drmModeFreeProperty(prop);
    }

    return 0;
}

/* Returns malloc'd array of modifiers with count in out_count. Caller must free() the returned pointer.
   Returns NULL and sets *out_count = 0 on error or no modifiers found.
*/
static uint64_t * drm_get_format_mods(int drm_fd, uint32_t format, uint32_t crtc_index,
                                      size_t * out_count)
{
    if(!out_count) return NULL;
    *out_count = 0;

    drmModePlaneRes * res = drmModeGetPlaneResources(drm_fd);
    if(!res) return NULL;

    uint64_t * mods_arr = NULL;
    size_t mods_cap = 0;
    size_t mods_len = 0;

    for(uint32_t i = 0; i < res->count_planes; ++i) {
        drmModePlane * plane = drmModeGetPlane(drm_fd, res->planes[i]);
        if(!plane) continue;

        if(!(plane->possible_crtcs & (1u << crtc_index))) {
            drmModeFreePlane(plane);
            continue;
        }

        drmModeObjectProperties * props = drmModeObjectGetProperties(drm_fd, res->planes[i], DRM_MODE_OBJECT_PLANE);
        if(!props) {
            drmModeFreePlane(plane);
            continue;
        }

        uint64_t type = 0;
        uint64_t blob_id = 0;
        /* Use helper to fetch properties */
        {
            uint64_t val = 0;
            if(drm_get_prop_value(drm_fd, props, "type", &val))
                type = val;
            if(type != DRM_PLANE_TYPE_PRIMARY) {
                drmModeFreeObjectProperties(props);
                drmModeFreePlane(plane);
                continue;
            }
            if(drm_get_prop_value(drm_fd, props, "IN_FORMATS", &val))
                blob_id = val;
        }

        drmModeFreeObjectProperties(props);

        if(!blob_id) {
            drmModeFreePlane(plane);
            continue;
        }

        drmModePropertyBlobRes * blob = drmModeGetPropertyBlob(drm_fd, (uint32_t)blob_id);
        if(!blob) {
            drmModeFreePlane(plane);
            continue;
        }

        if(!blob->data) {
            drmModeFreePropertyBlob(blob);
            drmModeFreePlane(plane);
            continue;
        }

        struct drm_format_modifier_blob * data = (struct drm_format_modifier_blob *)blob->data;
        uint32_t * fmts = (uint32_t *)((char *)data + data->formats_offset);
        struct drm_format_modifier * modifiers = (struct drm_format_modifier *)((char *)data + data->modifiers_offset);

        uint32_t * fmt_p = NULL;
        for(uint32_t fi = 0; fi < data->count_formats; ++fi) {
            if(fmts[fi] == format) {
                fmt_p = &fmts[fi];
                break;
            }
        }
        if(!fmt_p) {
            drmModeFreePropertyBlob(blob);
            drmModeFreePlane(plane);
            continue;
        }

        uint32_t fmt_mask = 1u << (uint32_t)(fmt_p - fmts);

        for(uint32_t m = 0; m < data->count_modifiers; ++m) {
            if(modifiers[m].formats & fmt_mask) {
                if(mods_len + 1 > mods_cap) {
                    size_t new_cap = mods_cap == 0 ? 8 : mods_cap * 2;
                    uint64_t * tmp = realloc(mods_arr, new_cap * sizeof(uint64_t));
                    if(!tmp) {
                        free(mods_arr);
                        mods_arr = NULL;
                        mods_cap = mods_len = 0;
                        break;
                    }
                    mods_arr = tmp;
                    mods_cap = new_cap;
                }
                mods_arr[mods_len++] = modifiers[m].modifier;
            }
        }

        drmModeFreePropertyBlob(blob);
        drmModeFreePlane(plane);

        /* behavior in original: break after first matching primary plane */
        if(mods_len > 0) break;
    }

    drmModeFreePlaneResources(res);

    if(mods_len == 0) {
        free(mods_arr);
        *out_count = 0;
        return NULL;
    }

    /* shrink to fit */
    {
        uint64_t * tmp = realloc(mods_arr, mods_len * sizeof(uint64_t));
        if(tmp) mods_arr = tmp;
    }

    *out_count = mods_len;
    return mods_arr;
}

/*
 * This method is there to take care of cases that would not be handled
 * by open_using_udev_scan. This should not happen.
 *
 * If your driver defines a /dev/dri/cardX node and open_using_udev_scan
 * were not able to detect it, you should probably file an issue.
 *
 */
static int open_drm_with_module_checking(void)
{
    static const char * drm_modules[] = {
        "vc4",
        "i915",
        "imx-drm",
        "nouveau",
        "radeon",
        "vmgfx",
        "omapdrm",
        "exynos",
        "pl111",
        "msm",
        "meson",
        "rockchip",
        "sun4i-drm",
        "stm",
    };

    int fd = -1;
    size_t num_modules = sizeof(drm_modules) / sizeof(drm_modules[0]);

    for(size_t m = 0; m < num_modules; ++m) {
        fd = drmOpen(drm_modules[m], NULL);
        if(fd < 0) {
            LV_LOG_INFO("Failed to open DRM module '%s'", drm_modules[m]);
            continue;
        }
        LV_LOG_INFO("Opened DRM module '%s'", drm_modules[m]);
        break;
    }

    return fd;
}


// ########################################################################

/* TEMPORARY MODE SUMMARY PRINT FUNCTIONS */

/* helper structure for a resolution block */
typedef struct {
    int w, h;
    int start_idx;      /* first mode index in original list */
    int count;          /* number of modes with this resolution */
    char ** lines;      /* NULL-terminated array of allocated strings representing the box+subrows */
    int height;         /* number of lines in lines[] */
    /* for minimal summary */
    int * refreshes;    /* integer refresh rates (rounded) unique per block */
    int refresh_count;
} res_block_t;

/* Begin temp test of alternate formatting the resolution list support functions */
static int gcd(int a, int b)
{
    while(b) {
        int t = a % b;
        a = b;
        b = t;
    }
    return a;
}

/* round to two decimals */
static void ftoa2(char * buf, size_t bufsz, float v)
{
    snprintf(buf, bufsz, "%.2f", v);
}


static void print_grouped_modes(drmModeConnectorPtr conn)
{
    int count = conn->count_modes;
    if(count == 0) return;

    /* helper line builders */
    char topbot[128], onlybot[128], sep[128];
    snprintf(topbot, sizeof topbot, "+%.*s+", BOX_WIDTH, "----------------------------------");
    snprintf(onlybot, sizeof topbot, "+%.*s+", BOX_WIDTH - 2, "----------------------------------");
    snprintf(sep, sizeof sep, "+%.*s+", BOX_WIDTH, "----------------------------------");

    for(int m = 0; m < count;) {
        drmModeModeInfo * cur = &conn->modes[m];
        int w = cur->hdisplay, h = cur->vdisplay;
        int g = gcd(w, h);
        int ar_w = w / g, ar_h = h / g;

        /* print boxed resolution header (centered-ish) */
        char hdr[128];
        snprintf(hdr, sizeof hdr, "  %dx%d  (%d:%d)", w, h, ar_w, ar_h);

        int pad_left = 1;
        int content_len = (int)strlen(hdr);
        int pad_right = BOX_WIDTH - content_len - pad_left;
        if(pad_right < 0) pad_right = 0;

        printf("%s\n", topbot);
        printf("|%*s%s%*s|\n", pad_left, "", hdr, pad_right, "");
        printf("%s\n", sep);

        /* print subrows for every mode that matches this resolution */
        while(m < count && conn->modes[m].hdisplay == w && conn->modes[m].vdisplay == h) {
            drmModeModeInfo * mm = &conn->modes[m];
            float refresh_hertz = (mm->clock * 1000.f) / (float)(mm->htotal * mm->vtotal);

            char idxbuf[32], refbuf[32];
            snprintf(idxbuf, sizeof idxbuf, "#%d", m + 1);
            ftoa2(refbuf, sizeof refbuf, refresh_hertz);
            /* build a two-column style line: |  idx   |   ref   | within the BOX_WIDTH */
            /* column widths (adjust as desired) */
            int col1w = 4;
            int col2w = BOX_WIDTH - col1w - 8; /* 3+2+_hz for spaces/dividers */

            printf("  | %*s | %*s hz |\n", col1w - 1, idxbuf, col2w - 1, refbuf);

            m++;
        }
        /* end of this resolution block */
    }
    printf("  %s\n", onlybot);
}

static void print_with_banner_lines(char ** left_lines, int left_count,
                                    const char ** banner_lines, int banner_count)
{
    int total = left_count;//left_count > banner_count ? left_count : banner_count;
    for(int r = 0; r < total; ++r) {
        const char * L = (r < left_count) ? left_lines[r] : "";
        const char * R = (banner_lines && r < banner_count) ? banner_lines[r] : "";
        printf("%s", L);
        if(banner_lines) {
            int pad = 4;
            for(int p = 0; p < pad; ++p) putchar(' ');
            printf("%s", R);
        }
        putchar('\n');
    }
}

/* banner_lines: array of strings (not modified), banner_count the number of lines.
   current_idx is 0-based index of current mode. */
static void print_grouped_modes_ex(drmModeConnectorPtr conn, uint32_t current_idx,
                                   display_mode_t mode,
                                   const char ** banner_lines, int banner_count)
{
    int count = conn->count_modes;
    if(count == 0) return;

    /* Build blocks by scanning modes */
    res_block_t * blocks = NULL;
    int nblocks = 0;

    char empty_bar[128], cap_bar[128];
    snprintf(empty_bar, sizeof empty_bar, " %.*s ", BOX_WIDTH, "                                 ");
    snprintf(cap_bar, sizeof cap_bar, "    '%.*s'", BOX_WIDTH - 4, "__________________________________");

    for(int i = 0; i < count;) {
        drmModeModeInfo * mi = &conn->modes[i];
        int w = mi->hdisplay, h = mi->vdisplay;
        int start = i;
        /* count block size */
        while(i < count && conn->modes[i].hdisplay == w && conn->modes[i].vdisplay == h) i++;
        int blk_count = i - start;

        /* allocate and init block */
        blocks = realloc(blocks, sizeof(*blocks) * (nblocks + 1));
        res_block_t * b = &blocks[nblocks++];
        b->w = w;
        b->h = h;
        b->start_idx = start;
        b->count = blk_count;
        b->lines = NULL;
        b->height = 0;
        b->refreshes = NULL;
        b->refresh_count = 0;

        /* Collect unique rounded refresh rates for summary */
        for(int j = 0; j < blk_count; ++j) {
            drmModeModeInfo * mm = &conn->modes[start + j];
            float refresh = (mm->clock * 1000.f) / (float)(mm->htotal * mm->vtotal);
            int r = (int)(refresh + 0.5f);
            int seen = 0;
            for(int k = 0; k < b->refresh_count; ++k) if(b->refreshes[k] == r) {
                    seen = 1;
                    break;
                }
            if(!seen) {
                b->refreshes = realloc(b->refreshes, sizeof(int) * (b->refresh_count + 1));
                b->refreshes[b->refresh_count++] = r;
            }
        }
        /* sort refreshes ascending (simple insertion sort) */
        for(int p = 1; p < b->refresh_count; ++p) {
            int key = b->refreshes[p];
            int q = p - 1;
            while(q >= 0 && b->refreshes[q] > key) {
                b->refreshes[q + 1] = b->refreshes[q];
                q--;
            }
            b->refreshes[q + 1] = key;
        }

        /* Build string lines for this block (box header + rows) */
        char topbot[128], sep[128];
        snprintf(topbot, sizeof topbot, "+%.*s+", BOX_WIDTH, "----------------------------------");
        snprintf(sep, sizeof sep, "+%.*s+", BOX_WIDTH, "----------------------------------");

        /* header */
        char hdr[128];
        int g = gcd(w, h);
        int ar_w = w / g, ar_h = h / g;
        snprintf(hdr, sizeof hdr, "  %dx%d  (%d:%d)", w, h, ar_w, ar_h);

        int content_len = (int)strlen(hdr);
        int pad_left = 1;
        int pad_right = BOX_WIDTH - content_len - pad_left;
        if(pad_right < 0) pad_right = 0;

        /* push header lines */
        b->lines = realloc(b->lines, sizeof(char *) * (b->height + 3));
        b->lines[b->height++] = strdup(topbot);

        char * line = malloc(MAX_LINE_LEN);
        snprintf(line, MAX_LINE_LEN, "|%*s%s%*s|", pad_left, "", hdr, pad_right, "");
        b->lines[b->height++] = line;

        b->lines[b->height++] = strdup(sep);

        /* subrows */
        for(int j = 0; j < blk_count; ++j) {
            drmModeModeInfo * mm = &conn->modes[start + j];
            float refresh_hertz = (mm->clock * 1000.f) / (float)(mm->htotal * mm->vtotal);
            char idxbuf[32], refbuf[32];
            snprintf(idxbuf, sizeof idxbuf, "#%d", start + j + 1);
            ftoa2(refbuf, sizeof refbuf, refresh_hertz);
            /* marker for current */
            char marker[8] = "   ";
            if((uint32_t)(start + j) == current_idx) strcpy(marker, "[*]");

            /* format: "  | [*] #N |  XX.X hz |" fitting BOX_WIDTH */
            int col1w = 6; /* space for marker + index */
            int col2w = BOX_WIDTH - col1w - 8; /* leave room for ' hz ' and dividers */
            if(col2w < 4) col2w = 4;
            char buf[MAX_LINE_LEN];
            snprintf(buf, sizeof buf, "%s | %*s | %*s hz |", marker, col1w - 3, idxbuf, col2w - 1, refbuf);
            b->lines = realloc(b->lines, sizeof(char *) * (b->height + 1));
            b->lines[b->height++] = strdup(buf);
        }

        /* leave closing topbot out here; we'll print separator lines between blocks when rendering list/tabled.
           But store closing line so layouts that print full boxes can use it. */
        //b->lines = realloc(b->lines, sizeof(char*) * (b->height + 1));
        //b->lines[b->height++] = strdup(topbot);
    }

    /* For MINIMAL mode, determine which block contains current_idx; if not found, fall back to LIST */
    int current_block = -1;
    for(int i = 0; i < nblocks; ++i) {
        if(current_idx >= (uint32_t)blocks[i].start_idx &&
           current_idx < (uint32_t)(blocks[i].start_idx + blocks[i].count)) {
            current_block = i;
            break;
        }
    }
    if(mode == DISP_MINIMAL && current_block == -1) mode = DISP_LIST;

    if(mode == DISP_LIST) {
        /* Print each block sequentially; ensure overall height accomodates banner by adding extra newlines at end if needed */
        int printed_lines = 0;
        for(int i = 0; i < nblocks; ++i) {
            //print_with_banner(blocks[i].lines, blocks[i].height);
            print_with_banner_lines(blocks[i].lines, blocks[i].height, banner_lines, banner_count);
            printed_lines += blocks[i].height;
        }
        if(banner_count > printed_lines) {
            for(int r = printed_lines; r < banner_count; ++r) {
                /* print padding for left area then banner line */
                int left_width = BOX_WIDTH + 4; /* approximate width including leading spaces used in lines */
                for(int p = 0; p < left_width; ++p) putchar(' ');
                printf("%s\n", banner_lines[r]);
            }
        }
    }
    else if(mode == DISP_MINIMAL) {
        /* Print only current_block lines, plus short summary of other valid resolutions on one line below.
           If banner_lines present, print to right of the block; pad so banner fits. */
        res_block_t * cb = &blocks[current_block];
        /* Print block with banner */
        //print_with_banner(cb->lines, cb->height);
        print_with_banner_lines(cb->lines, cb->height, banner_lines, banner_count);
        printf("%s    ", cap_bar);

        /* Ensure banner is fully shown: if banner has extra lines beyond block height we've already padded in print_with_banner */

        /* Build "Other valid modes are: ..." single-line summary */
        /* We will list each block once with rounded refreshes (integers) joined by '/' and trailing "hz" after the group. */
        char summary[1024];
        const uint8_t max_line_width =
            80;  // Note it may occasionally go slightly over this, up to the end of the current resolution block

        summary[0] = '\0';
        uint8_t per_line_count = 0;//strlen(summary);
        for(int i = 0; i < nblocks; ++i) {
            if(!((i == current_block) && (blocks[i].refresh_count == 1))) {
                if(i) strcat(summary, ", ");
                per_line_count += 2; // add the end cap in too
                if(per_line_count >= max_line_width) {
                    per_line_count = 0;
                    strcat(summary, "\n");
                }

                char tmp[256];
                snprintf(tmp, sizeof tmp, "%dx%d", blocks[i].w, blocks[i].h);
                per_line_count += strlen(tmp);
                strcat(summary, tmp);
                if(blocks[i].refresh_count > 1) {
                    strcat(summary, " (");
                    per_line_count += 5; // add the end cap in too

                    for(int r = 0; r < blocks[i].refresh_count; ++r) {
                        char rt[16];
                        snprintf(rt, sizeof rt, "%d", blocks[i].refreshes[r]);
                        per_line_count += strlen(rt);
                        strcat(summary, rt);
                        if(r + 1 < blocks[i].refresh_count) strcat(summary, "/");
                    }
                    strcat(summary, "hz)");
                }
            }
        }
        /* print summary to the next line; if banner exists and is taller than block, it will already be printed; we just print summary on next available left line */
        /* Align summary with left area and optionally print banner line if present at that row */
        const char * summary_header = "Other valid resolutions:";
        bool printed_summary_header = false;
        bool at_least_one_blank_line = false;
        if(banner_count > cb->height) {
            /* print summary and the corresponding banner line */
            int pad = 2;
            int left_width = pad + BOX_WIDTH + 4;
            // first pad added by cap_bar bottom of only shown resolution block
            printf("%s\n", banner_lines[cb->height]);
            /* print remaining banner lines if any */
            for(int r = cb->height + 1; r < banner_count; ++r) {
                /* pad left area */
                if((r == banner_count - 1) && (at_least_one_blank_line)) {
                    printf("%s     ", summary_header);
                    printed_summary_header = true;
                }
                else {
                    for(int p = 0; p < left_width; ++p) putchar(' ');
                    at_least_one_blank_line = true;
                }
                printf("%s\n", banner_lines[r]);
            }
        }
        if(!printed_summary_header) {
            if(!at_least_one_blank_line) {
                putchar('\n');
            }
            printf("%s\n", summary_header);
        }
        printf("%s\n", summary);
    }
    else {   /* DISP_TABLED */
        /* Distribute blocks into 3 columns by total height (greedy fill left->right) */
        int cols = 3;
        /* compute heights */
        int * col_heights = calloc(cols, sizeof(int));
        res_block_t *** col_blocks = calloc(cols, sizeof(res_block_t **));
        int * col_counts = calloc(cols, sizeof(int));
        for(int i = 0; i < nblocks; ++i) {
            /* pick column with smallest height */
            int pick = 0;
            for(int c = 1; c < cols; ++c) if(col_heights[c] < col_heights[pick]) pick = c;
            /* append block i to column pick */
            col_blocks[pick] = realloc(col_blocks[pick], sizeof(res_block_t *) * (col_counts[pick] + 1));
            col_blocks[pick][col_counts[pick]++] = &blocks[i];
            col_heights[pick] += blocks[i].height + 1; /* +1 for spacing between blocks */
        }
        /* find max column height in lines */
        int max_lines = 0;
        for(int c = 0; c < cols; ++c) if(col_heights[c] > max_lines) max_lines = col_heights[c];

        /* Build arrays of strings for each column by concatenating block.lines with spacing */
        char *** col_lines = calloc(cols, sizeof(char **));
        int * col_line_counts = calloc(cols, sizeof(int));
        for(int c = 0; c < cols; ++c) {
            int cap = 0, used = 0;
            for(int bidx = 0; bidx < col_counts[c]; ++bidx) {
                res_block_t * rb = col_blocks[c][bidx];
                for(int ln = 0; ln < rb->height; ++ln) {
                    if(used >= cap) {
                        cap = cap ? cap * 2 : 16;
                        col_lines[c] = realloc(col_lines[c], sizeof(char *) * cap);
                    }
                    col_lines[c][used++] = strdup(rb->lines[ln]);
                }
                /* blank line between blocks */
                //if (bidx + 1 < col_counts[c]) {
                //    if (used >= cap) { cap = cap ? cap * 2 : 16; col_lines[c] = realloc(col_lines[c], sizeof(char*) * cap); }
                //    col_lines[c][used++] = strdup("");
                //}
            }
            col_line_counts[c] = used;
            if(used > max_lines) max_lines = used;
        }

        /* Now print row by row combining up to three columns with spacing */
        int gap = 4;
        bool col_capped[cols];
        for(int c = 0; c < cols; ++c) {
            col_capped[c] = false;
        }
        for(int r = 0; r < max_lines; ++r) {
            for(int c = 0; c < cols; ++c) {
                bool has_contents = (r < col_line_counts[c]);
                const char * s = has_contents ? col_lines[c][r] : (col_capped[c] ? empty_bar :
                                                                   cap_bar); //"                         " : "    '___________________'");
                if(!has_contents) col_capped[c] = true;
                printf("%s", s);
                if(c + 1 < cols) {
                    /* pad to gap */
                    for(int p = 0; p < gap; ++p) putchar(' ');
                }
            }
            /* banner: attach banner lines to the rightmost side only when present; we'll print banner in-line if counts match */
            if(banner_lines && r < banner_count) {
                for(int p = 0; p < gap; ++p) putchar(' ');
                printf("%s", banner_lines[r]);
            }
            putchar('\n');
        }

        /* free per-column lines */
        for(int c = 0; c < cols; ++c) {
            for(int i = 0; i < col_line_counts[c]; ++i) {
                LV_LOG_WARN("Freeing column line #%d", i);
                free(col_lines[c][i]);
            }
            free(col_lines[c]);
        }
        LV_LOG_WARN("Freeing column lines collection");
        free(col_lines);
        LV_LOG_WARN("Freeing column line counts");
        free(col_line_counts);
        LV_LOG_WARN("Freeing column blocks");
        free(col_blocks);
        LV_LOG_WARN("Freeing column counts");
        free(col_counts);
        LV_LOG_WARN("Freeing column heights");
        free(col_heights);
    }

    /* cleanup blocks */
    for(int i = 0; i < nblocks; ++i) {
        for(int j = 0; j < blocks[i].height; ++j) {
            LV_LOG_WARN("Freeing block #%d, line #%d", i, j);
            free(blocks[i].lines[j]);
        }
        LV_LOG_WARN("Freeing block #%d lines", i);
        free(blocks[i].lines);
        LV_LOG_WARN("Freeing block #%d refreshes", i);
        free(blocks[i].refreshes);
    }
    LV_LOG_WARN("Freeing blocks");
    free(blocks);
}

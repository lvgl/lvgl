/**
 * @file lv_linux_drm_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"

#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include <fcntl.h>
#include <string.h>
#include <xf86drmMode.h>
#include <stdlib.h>
#include <stdint.h>
#include <gbm.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include <time.h>
#include <unistd.h>
#include "lv_linux_drm_egl_private.h"
#include "../../../draw/lv_draw_buf.h"
#include "../../opengles/lv_opengles_debug.h"

#include "../../opengles/lv_opengles_driver.h"
#include "../../opengles/lv_opengles_texture.h"
#include "../../opengles/lv_opengles_private.h"

#include "../../../stdlib/lv_string.h"
#include "../../../display/lv_display.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int fd;
    struct gbm_bo * bo;
    uint32_t fb_id;
} drm_fb_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t tick_cb(void);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void event_cb(lv_event_t * e);

static lv_result_t drm_device_init(lv_drm_ctx_t * ctx, const char * path);
static void drm_device_deinit(lv_drm_ctx_t * ctx);

static lv_egl_interface_t drm_get_egl_interface(lv_drm_ctx_t * ctx);
static drmModeConnector * drm_get_connector(lv_drm_ctx_t * ctx);
static drmModeModeInfo * drm_get_mode(lv_drm_ctx_t * ctx);
static drmModeEncoder * drm_get_encoder(lv_drm_ctx_t * ctx);
static drmModeCrtc * drm_get_crtc(lv_drm_ctx_t * ctx);
static void drm_on_page_flip(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void * data);
static drm_fb_state_t * drm_fb_state_create(lv_drm_ctx_t * ctx, struct gbm_bo * bo);
static void drm_fb_state_destroy_cb(struct gbm_bo * bo, void * data);
static void drm_flip_cb(void * driver_data, bool vsync);

static void * drm_create_window(void * driver_data, const lv_egl_native_window_properties_t * properties);
static void drm_destroy_window(void * driver_data, void * native_window);
static size_t drm_egl_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static inline void set_viewport(lv_display_t * display);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_linux_drm_create(void)
{
    lv_tick_set_cb(tick_cb);
    lv_drm_ctx_t * ctx = lv_zalloc(sizeof(*ctx));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        LV_LOG_ERROR("Failed to create drm context");
        return NULL;
    }

    ctx->display = lv_display_create(1, 1);

    if(!ctx->display) {
        LV_LOG_ERROR("Failed to create display");
        lv_free(ctx);
        return NULL;
    }

    lv_display_set_driver_data(ctx->display, ctx);
    lv_display_add_event_cb(ctx->display, event_cb, LV_EVENT_DELETE, NULL);
    return ctx->display;
}

void lv_linux_drm_set_file(lv_display_t * display, const char * file, int64_t connector_id)
{
    LV_UNUSED(connector_id);
    lv_drm_ctx_t * ctx = lv_display_get_driver_data(display);

    lv_result_t err = drm_device_init(ctx, file);
    if(err != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize DRM device");
        return;
    }

    lv_display_set_resolution(display, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay);

    ctx->egl_interface = drm_get_egl_interface(ctx);
    ctx->egl_ctx = lv_opengles_egl_context_create(&ctx->egl_interface);
    if(!ctx->egl_ctx) {
        LV_LOG_ERROR("Failed to create egl context");
        return;
    }

    /* Let the opengles texture driver handle the texture lifetime */
    ctx->texture.is_texture_owner = true;
    lv_result_t res = lv_opengles_texture_create_draw_buffers(&ctx->texture, display);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create draw buffers");
        lv_opengles_egl_context_destroy(ctx->egl_ctx);
        ctx->egl_ctx = NULL;
        return;
    }
    /* This creates the texture for the first time*/
    lv_opengles_texture_reshape(display, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay);

    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_DIRECT);

    lv_display_add_event_cb(ctx->display, event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(ctx->display, event_cb, LV_EVENT_DELETE, NULL);
}

void lv_linux_drm_set_mode_cb(lv_display_t * disp, lv_linux_drm_select_mode_cb_t callback)
{
    if(!disp) {
        LV_LOG_ERROR("Cannot set a mode select callback on a NULL display");
        return;
    }
    lv_drm_ctx_t * ctx = lv_display_get_driver_data(disp);
    ctx->mode_select_cb = callback;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_display_t * display = (lv_display_t *) lv_event_get_target(e);
    lv_drm_ctx_t * ctx = lv_display_get_driver_data(display);
    switch(code) {
        case LV_EVENT_DELETE:
            if(ctx) {
                lv_opengles_egl_context_destroy(ctx->egl_ctx);
                ctx->egl_ctx = NULL;
                lv_opengles_texture_deinit(&ctx->texture);
                drm_device_deinit(ctx);
                lv_display_set_driver_data(display, NULL);
            }
            break;
        case LV_EVENT_RESOLUTION_CHANGED:
            lv_opengles_texture_reshape(display, lv_display_get_horizontal_resolution(display),
                                        lv_display_get_vertical_resolution(display));
            break;
        default:
            return;
    }
}

static uint32_t tick_cb(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + (ts.tv_nsec / 1000000);;
}

static inline void set_viewport(lv_display_t * display)
{
    const lv_display_rotation_t rotation = lv_display_get_rotation(display);
    int32_t disp_width, disp_height;
    if(rotation == LV_DISPLAY_ROTATION_0 || rotation == LV_DISPLAY_ROTATION_180) {
        disp_width = lv_display_get_horizontal_resolution(display);
        disp_height = lv_display_get_vertical_resolution(display);
    }
    else {
        disp_width = lv_display_get_vertical_resolution(display) ;
        disp_height = lv_display_get_horizontal_resolution(display) ;
    }
    lv_opengles_viewport(0, 0, disp_width, disp_height);
}

#if LV_USE_DRAW_OPENGLES

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    if(lv_display_flush_is_last(disp)) {
        set_viewport(disp);
        lv_drm_ctx_t * ctx = lv_display_get_driver_data(disp);
        lv_opengles_render_display_texture(disp, false, true);
        lv_opengles_egl_update(ctx->egl_ctx);
    }
    lv_display_flush_ready(disp);
}

#else

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(px_map);
    LV_UNUSED(area);
    if(lv_display_flush_is_last(disp)) {
        lv_drm_ctx_t * ctx = lv_display_get_driver_data(disp);
        int32_t disp_width = lv_display_get_horizontal_resolution(disp);
        int32_t disp_height = lv_display_get_vertical_resolution(disp);

        set_viewport(disp);

        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ctx->texture.texture_id));

        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / lv_color_format_get_size(cf)));
        /*Color depth: 16 (RGB565), 32 (ARGB8888)*/
#if LV_COLOR_DEPTH == 16
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp_width, disp_height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                             ctx->texture.fb1));
#elif LV_COLOR_DEPTH == 32
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, disp_width, disp_height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                             ctx->texture.fb1));
#else
#error("Unsupported color format")
#endif

        lv_opengles_render_display_texture(disp, false, false);
        lv_opengles_egl_update(ctx->egl_ctx);
    }
    lv_display_flush_ready(disp);
}
#endif

void drm_device_deinit(lv_drm_ctx_t * ctx)
{
    if(ctx->drm_crtc) {
        drmModeSetCrtc(ctx->fd,
                       ctx->drm_crtc->crtc_id,
                       ctx->drm_crtc->buffer_id,
                       ctx->drm_crtc->x,
                       ctx->drm_crtc->y,
                       &ctx->drm_connector->connector_id,
                       1,
                       &ctx->drm_crtc->mode);
        drmModeFreeCrtc(ctx->drm_crtc);
        ctx->drm_crtc = 0;
    }
    drm_destroy_window(ctx, ctx->gbm_surface);

    if(ctx->gbm_dev) {
        gbm_device_destroy(ctx->gbm_dev);
        ctx->gbm_dev = NULL;
    }
    if(ctx->drm_connector) {
        drmModeFreeConnector(ctx->drm_connector);
        ctx->drm_connector = NULL;
    }
    if(ctx->drm_encoder) {
        drmModeFreeEncoder(ctx->drm_encoder);
        ctx->drm_encoder = NULL;
    }
    if(ctx->drm_resources) {
        drmModeFreeResources(ctx->drm_resources);
        ctx->drm_resources = NULL;
    }
    if(ctx->fd > 0) {
        drmClose(ctx->fd);
    }
    ctx->fd = 0;
    ctx->drm_mode = NULL;
    lv_free(ctx);
}

static void drm_fb_state_destroy_cb(struct gbm_bo * bo, void * data)
{
    LV_UNUSED(bo);
    drm_fb_state_t * fb = (drm_fb_state_t *) data;
    if(fb && fb->fb_id) {
        drmModeRmFB(fb->fd, fb->fb_id);
    }
    lv_free(fb);
}

static int drm_do_page_flip(lv_drm_ctx_t * ctx, int timeout_ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(ctx->fd, &fds);

    drmEventContext event_ctx;
    lv_memset(&event_ctx, 0, sizeof(event_ctx));
    event_ctx.version = 2;
    event_ctx.page_flip_handler = drm_on_page_flip;

    struct timeval timeout;
    int status;
    if(timeout_ms >= 0) {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        status = select(ctx->fd + 1, &fds, NULL, NULL, &timeout);
    }
    else {
        status = select(ctx->fd + 1, &fds, NULL, NULL, NULL);
    }

    if(status == 1) {
        drmHandleEvent(ctx->fd, &event_ctx);
    }
    return status;
}

static void drm_on_page_flip(int fd, unsigned int frame, unsigned int sec, unsigned int usec, void * data)
{
    LV_UNUSED(fd);
    LV_UNUSED(frame);
    LV_UNUSED(sec);
    LV_UNUSED(usec);
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *) data;

    if(ctx->gbm_bo_presented) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_presented);
    }
    ctx->gbm_bo_presented = ctx->gbm_bo_flipped;
    ctx->gbm_bo_flipped = NULL;
}

static drm_fb_state_t * drm_fb_state_create(lv_drm_ctx_t * ctx, struct gbm_bo * bo)
{
    LV_ASSERT_NULL(bo);
    drm_fb_state_t * fb = (drm_fb_state_t *)gbm_bo_get_user_data(bo);

    if(fb) {
        return fb;
    }

    uint32_t width = gbm_bo_get_width(bo);
    uint32_t height = gbm_bo_get_height(bo);
    uint32_t handles[4] = {0};
    uint32_t strides[4] = {0};
    uint32_t offsets[4] = {0};
    uint64_t modifiers[4] = {0};
    uint32_t format = gbm_bo_get_format(bo);
    uint64_t modifier = gbm_bo_get_modifier(bo);
    uint32_t fb_id = 0;
    uint64_t addfb2_mods = 0;
    int32_t status;

    drmGetCap(ctx->fd, DRM_CAP_ADDFB2_MODIFIERS, &addfb2_mods);

    for(int i = 0; i < gbm_bo_get_plane_count(bo); i++) {
        handles[i] = gbm_bo_get_handle_for_plane(bo, i).u32;
        strides[i] = gbm_bo_get_stride_for_plane(bo, i);
        offsets[i] = gbm_bo_get_offset(bo, i);
        modifiers[i] = modifier;
    }

    if(addfb2_mods && modifier != DRM_FORMAT_MOD_INVALID) {
        status = drmModeAddFB2WithModifiers(ctx->fd, width, height, format,
                                            handles, strides, offsets, modifiers,
                                            &fb_id, DRM_MODE_FB_MODIFIERS);
    }
    else {
        status = drmModeAddFB2(ctx->fd, width, height, format,
                               handles, strides, offsets, &fb_id, 0);
    }

    if(status < 0) {
        LV_LOG_ERROR("Failed to create drm_fb_state: %d", status);
        return NULL;
    }

    fb = (drm_fb_state_t *)lv_malloc(sizeof(*fb));
    if(!fb) {
        LV_LOG_ERROR("Failed to allocate drmfb_state");
        return NULL;
    }

    fb->fd = ctx->fd;
    fb->bo = bo;
    fb->fb_id = fb_id;

    gbm_bo_set_user_data(bo, fb, drm_fb_state_destroy_cb);
    return fb;
}

static void drm_flip_cb(void * driver_data, bool vsync)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *) driver_data;

    if(ctx->gbm_bo_pending) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_pending);
    }
    ctx->gbm_bo_pending = gbm_surface_lock_front_buffer(ctx->gbm_surface);

    if(!ctx->gbm_bo_pending) {
        LV_LOG_ERROR("Failed to lock front buffer");
        return;
    }

    drm_fb_state_t * pending_fb = drm_fb_state_create(ctx, ctx->gbm_bo_pending);

    if(!ctx->gbm_bo_pending || !pending_fb) {
        LV_LOG_ERROR("Failed to get gbm front buffer");
        return;
    }

    if(vsync) {
        while(ctx->gbm_bo_flipped && drm_do_page_flip(ctx, -1) >= 0)
            continue;
    }
    else {
        drm_do_page_flip(ctx, 0);
    }

    if(!ctx->gbm_bo_flipped) {
        if(!ctx->crtc_isset) {
            int status = drmModeSetCrtc(ctx->fd, ctx->drm_encoder->crtc_id, pending_fb->fb_id, 0, 0,
                                        &(ctx->drm_connector->connector_id), 1, ctx->drm_mode);
            if(status < 0) {
                LV_LOG_ERROR("Failed to set crtc: %d", status);
                return;
            }
            ctx->crtc_isset = true;
            if(ctx->gbm_bo_presented)
                gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_presented);
            ctx->gbm_bo_presented = ctx->gbm_bo_pending;
            ctx->gbm_bo_flipped = NULL;
            ctx->gbm_bo_pending = NULL;
            return;
        }


        uint32_t flip_flags = DRM_MODE_PAGE_FLIP_EVENT;
        int status = drmModePageFlip(ctx->fd, ctx->drm_encoder->crtc_id, pending_fb->fb_id, flip_flags, ctx);
        if(status < 0) {
            LV_LOG_ERROR("Failed to enqueue page flip: %d", status);
            return;
        }
        ctx->gbm_bo_flipped = ctx->gbm_bo_pending;
        ctx->gbm_bo_pending = NULL;
    }

    /* We need to ensure our surface has a free buffer, otherwise GL will
     * have no buffer to render on. */
    while(!gbm_surface_has_free_buffers(ctx->gbm_surface) &&
          drm_do_page_flip(ctx, -1) >= 0) {
        continue;
    }
}

static lv_result_t drm_device_init(lv_drm_ctx_t * ctx, const char * path)
{
    if(!path) {
        LV_LOG_ERROR("Device path must not be NULL");
        return LV_RESULT_INVALID;
    }
    int ret = open(path, O_RDWR);
    if(ret < 0) {
        LV_LOG_ERROR("Failed to open device path '%s'", path);
        goto open_err;
    }
    ctx->fd = ret;

    ret = drmSetClientCap(ctx->fd, DRM_CLIENT_CAP_UNIVERSAL_PLANES, 1);
    if(ret < 0) {
        LV_LOG_ERROR("Failed to set universal planes capability");
        goto set_client_cap_err;
    }

    ctx->drm_resources = drmModeGetResources(ctx->fd);
    if(!ctx->drm_resources) {
        LV_LOG_ERROR("Failed to get card resources");
        goto get_resources_err;
    }

    ctx->drm_connector = drm_get_connector(ctx);
    if(!ctx->drm_connector) {
        LV_LOG_ERROR("Failed to find a suitable connector");
        goto get_connector_err;
    }

    ctx->drm_mode = drm_get_mode(ctx);
    if(!ctx->drm_mode) {
        LV_LOG_ERROR("Failed to find a suitable drm mode");
        goto get_mode_err;
    }

    ctx->drm_encoder = drm_get_encoder(ctx);
    if(!ctx->drm_encoder) {
        LV_LOG_ERROR("Failed to find a suitable encoder");
        goto get_encoder_err;
    }

    ctx->gbm_dev = gbm_create_device(ctx->fd);
    if(!ctx->gbm_dev) {
        LV_LOG_ERROR("Failed to create gbm device");
        goto gbm_create_device_err;
    }

    if(drmSetMaster(ctx->fd) < 0) {
        LV_LOG_ERROR("Failed to become DRM master");
        goto set_master_err;
    }

    ctx->drm_crtc = drm_get_crtc(ctx);
    if(!ctx->drm_crtc) {
        LV_LOG_ERROR("Failed to get crtc");
        goto get_crtc_err;
    }

    return LV_RESULT_OK;

get_crtc_err:
    gbm_device_destroy(ctx->gbm_dev);
    ctx->gbm_dev = NULL;
set_master_err:
    /* Nothing special to do */
gbm_create_device_err:
    drmModeFreeEncoder(ctx->drm_encoder);
    ctx->drm_encoder = NULL;
get_encoder_err:
    drmModeFreeConnector(ctx->drm_connector);
    ctx->drm_connector = NULL;
get_mode_err:
    /* Nothing special to do */
get_connector_err:
    drmModeFreeResources(ctx->drm_resources);
    ctx->drm_resources = NULL;
get_resources_err:
    /* Nothing special to do */
set_client_cap_err:
    close(ctx->fd);
    ctx->fd = 0;
open_err:
    return LV_RESULT_INVALID;
}

static size_t drm_egl_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *)driver_data;
    int32_t target_w = lv_display_get_horizontal_resolution(ctx->display);
    int32_t target_h = lv_display_get_vertical_resolution(ctx->display);

#if LV_COLOR_DEPTH == 16
    lv_color_format_t target_cf = LV_COLOR_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
    lv_color_format_t target_cf = LV_COLOR_FORMAT_ARGB8888;
#else
#error("Unsupported color format")
#endif


    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_TRACE("Got config %zu %#x %dx%d %d %d %d %d buffer size %d depth %d  samples %d stencil %d surface type %d",
                     i, configs[i].id,
                     configs[i].max_width, configs[i].max_height, configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                     configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].stencil, configs[i].surface_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        lv_color_format_t config_cf = lv_opengles_egl_color_format_from_egl_config(&configs[i]);
        if(configs[i].max_width >= target_w &&
           configs[i].max_height >= target_h &&
           config_cf == target_cf &&
           configs[i].surface_type & EGL_WINDOW_BIT
          ) {
            LV_LOG_TRACE("Choosing config %zu", i);
            return i;
        }
    }
    return config_count;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_egl_interface_t drm_get_egl_interface(lv_drm_ctx_t * ctx)
{
    return (lv_egl_interface_t) {
        .driver_data = ctx,
        .native_display = ctx->gbm_dev,
        .egl_platform = EGL_PLATFORM_GBM_KHR,
        .select_config = drm_egl_select_config_cb,
        .flip_cb = drm_flip_cb,
        .create_window_cb = drm_create_window,
        .destroy_window_cb = drm_destroy_window,
    };
}

static drmModeConnector * drm_get_connector(lv_drm_ctx_t * ctx)
{
    drmModeConnector * connector = NULL;

    LV_ASSERT_NULL(ctx->drm_resources);
    for(int i = 0; i < ctx->drm_resources->count_connectors; i++) {
        connector = drmModeGetConnector(ctx->fd, ctx->drm_resources->connectors[i]);
        if(connector->connection == DRM_MODE_CONNECTED && connector->count_modes > 0) {
            return connector;
        }
        drmModeFreeConnector(connector);
        connector = NULL;
    }
    return connector;
}

static drmModeModeInfo * drm_get_mode(lv_drm_ctx_t * ctx)
{
    LV_ASSERT_NULL(ctx->drm_connector);
    if(ctx->mode_select_cb) {
        size_t mode_index = ctx->mode_select_cb(ctx->display, (lv_linux_drm_mode_t *)ctx->drm_connector->modes,
                                                (size_t)ctx->drm_connector->count_modes);
        if(mode_index >= (size_t)ctx->drm_connector->count_modes) {
            LV_LOG_ERROR("Failed to select drm mode. User select callback return an invalid mode index");
            return NULL;
        }
        return &ctx->drm_connector->modes[mode_index];
    }

    drmModeModeInfo * best_mode = NULL;
    uint32_t best_area = 0;

    for(int i = 0 ; i < ctx->drm_connector->count_modes; ++i) {
        drmModeModeInfo * mode = &ctx->drm_connector->modes[i];
        if(mode->type & DRM_MODE_TYPE_PREFERRED) {
            return mode;
        }
        uint32_t area = mode->hdisplay * mode->vdisplay;
        if(area > best_area) {
            best_area = area;
            best_mode = mode;
        }
    }
    LV_LOG_WARN("Failed to find a drm mode with the TYPE_PREFERRED flag. Using the one with the biggest area");
    return best_mode;
}

static drmModeCrtc * drm_get_crtc(lv_drm_ctx_t * ctx)
{
    drmModeCrtc * crtc = drmModeGetCrtc(ctx->fd, ctx->drm_encoder->crtc_id);
    if(crtc) {
        return crtc;
    }

    /* if there is no current CRTC, attach a suitable one */
    for(int i = 0; i < ctx->drm_resources->count_crtcs; i++) {
        if(ctx->drm_encoder->possible_crtcs & (1 << i)) {
            ctx->drm_encoder->crtc_id = ctx->drm_resources->crtcs[i];
            crtc = drmModeGetCrtc(ctx->fd, ctx->drm_encoder->crtc_id);
            break;
        }
    }
    return crtc;
}

static drmModeEncoder * drm_get_encoder(lv_drm_ctx_t * ctx)
{
    LV_ASSERT_NULL(ctx->drm_connector);
    drmModeEncoder * encoder = NULL;
    for(int i = 0; i < ctx->drm_resources->count_encoders; i++) {
        encoder = drmModeGetEncoder(ctx->fd, ctx->drm_resources->encoders[i]);
        if(!encoder) {
            continue;
        }
        for(int j = 0; j < ctx->drm_connector->count_encoders; j++) {
            if(encoder->encoder_id == ctx->drm_connector->encoders[j]) {
                return encoder;
            }
        }
        drmModeFreeEncoder(encoder);
        encoder = NULL;
    }
    return encoder;
}

static void * drm_create_window(void * driver_data, const lv_egl_native_window_properties_t * properties)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *)driver_data;
    LV_ASSERT_NULL(ctx->gbm_dev);

    uint32_t format = properties->visual_id;

    if(format == 0) {
        LV_LOG_ERROR("Invalid format requested");
        return NULL;
    }
    ctx->gbm_surface = gbm_surface_create(ctx->gbm_dev, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay, format,
                                          GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    if(!ctx->gbm_surface) {
        LV_LOG_ERROR("Failed to create GBM surface");
        return NULL;
    }
    return (void *)ctx->gbm_surface;

}

static void drm_destroy_window(void * driver_data, void * native_window)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *)driver_data;
    LV_ASSERT(native_window == ctx->gbm_surface);

    if(!ctx->gbm_surface) {
        return;
    }

    if(ctx->gbm_bo_pending) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_pending);
        ctx->gbm_bo_pending = NULL;
    }
    if(ctx->gbm_bo_flipped) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_flipped);
        ctx->gbm_bo_flipped = NULL;
    }
    if(ctx->gbm_bo_presented) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_presented);
        ctx->gbm_bo_presented = NULL;
    }
    gbm_surface_destroy(ctx->gbm_surface);
    ctx->gbm_surface = NULL;
}


#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

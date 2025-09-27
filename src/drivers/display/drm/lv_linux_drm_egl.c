/**
 * @file lv_linux_drm_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"
#include <errno.h>
#include <fcntl.h>
#include <src/display/lv_display.h>
#include <src/drivers/opengles/egl_adapter/private/glad/include/glad/egl.h>
#include <src/drivers/opengles/lv_opengles_egl.h>
#include <src/drivers/opengles/lv_opengles_texture_private.h>
#include <src/lv_conf_internal.h>
#include <src/misc/lv_array.h>
#include <src/misc/lv_color.h>
#include <src/misc/lv_event.h>
#include <src/misc/lv_types.h>
#include <string.h>
#include <xf86drmMode.h>

#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include <stdlib.h>
#include <stdint.h>
#include <gbm.h>
#include <drm_fourcc.h>
#include <xf86drm.h>
#include "lv_linux_drm_egl_private.h"

#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_string.h"
#include "../../../core/lv_global.h"
#include "../../../display/lv_display_private.h"
#include "../../../indev/lv_indev.h"
#include "../../../lv_init.h"
#include "../../../misc/lv_area_private.h"
#include "../../opengles/lv_opengles_driver.h"
#include "../../opengles/lv_opengles_texture.h"
#include "../../../drivers/opengles/lv_opengles_debug.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t tick_cb(void);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void event_cb(lv_event_t * e);
lv_result_t drm_device_init(lv_drm_ctx_t * ctx, const char * path);
static lv_egl_interface_t drm_get_egl_interface(lv_drm_ctx_t * ctx);
static drmModeConnector * drm_get_connector(lv_drm_ctx_t * ctx);
static drmModeEncoder * drm_get_encoder(lv_drm_ctx_t * ctx);
static drmModeCrtc * drm_get_crtc(lv_drm_ctx_t * ctx);
static lv_color_format_t drm_get_default_color_format(lv_drm_ctx_t * ctx);
static uint32_t drm_color_format(lv_color_format_t cf);
static void * drm_create_window(void * driver_data, const lv_native_window_properties_t * properties);
static void drm_destroy_window(void * driver_data, void * native_window);
static void drm_page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec,
                                  void * data);
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
    LV_LOG_USER("Create ctx");
    lv_drm_ctx_t * ctx = lv_zalloc(sizeof(*ctx));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        LV_LOG_ERROR("Failed to create lv_drm_egl_ output");
        return NULL;
    }

    LV_LOG_USER("Create display");

    ctx->display = lv_display_create(1024, 768);
    if(!ctx->display) {
        LV_LOG_ERROR("Failed to create display texture");
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

    lv_color_format_t cf = drm_get_default_color_format(ctx);
    if(cf == LV_COLOR_FORMAT_UNKNOWN) {
        LV_LOG_ERROR("Unsupported color format %#x", lv_display_get_color_format(display));
        return;
    }
    LV_LOG_USER("Setting color format to %d", cf);
    lv_display_set_color_format(display, cf);


    LV_LOG_USER("Initializing drm device");
    lv_result_t err = drm_device_init(ctx, file);
    if(err != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize DRM device");
        return;
    }

    LV_LOG_USER("Setting display resolution");
    lv_display_set_resolution(display, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay);

    LV_LOG_USER("Initializing egl context");
    ctx->egl_interface = drm_get_egl_interface(ctx);
    ctx->egl_ctx = lv_opengles_egl_context_create(&ctx->egl_interface);
    if(!ctx->egl_ctx) {
        LV_LOG_ERROR("Failed to create egl context");
        return;
    }

    LV_LOG_USER("Initializing egl context");
    lv_result_t res = lv_opengles_texture_create_from_display(display);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize opengl texture");
        lv_opengles_egl_context_deinit(ctx->egl_ctx);
        ctx->egl_ctx = NULL;
        return;
    }
    lv_opengles_texture_reshape(display, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay);

    /* Driver data gets set by opengles_texture. We overwrite it with our own by keeping the first attribute the same*/
    lv_opengles_texture_t * texture = lv_display_get_driver_data(display);
    ctx->texture = *texture;
    lv_display_set_driver_data(display, ctx);

    ctx->h_flip = false;
    ctx->v_flip = false;
    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_add_event_cb(ctx->display, event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
}

void lv_drm_egl_set_flip(lv_drm_ctx_t * window, bool h_flip, bool v_flip)
{
    window->h_flip = h_flip;
    window->v_flip = v_flip;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    LV_LOG_USER("Event");
    lv_display_t * display = (lv_display_t *) lv_event_get_target(e);
    lv_drm_ctx_t * ctx = lv_display_get_driver_data(display);
    switch(code) {
        case LV_EVENT_DELETE:
            // lv_egl_adapter_reset(ctx->egl_interface->egl_adapter);
            // lv_egl_adapter_interface_deinit(ctx->egl_interface);
            lv_free(ctx);
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
    if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    uint64_t ms = (uint64_t)ts.tv_sec * 1000ull + (uint64_t)ts.tv_nsec / 1000000ull;
    return (uint32_t)ms;
}

#if LV_USE_DRAW_OPENGLES

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    if(lv_display_flush_is_last(disp)) {
        const int32_t disp_width = lv_display_get_horizontal_resolution(disp);
        const int32_t disp_height = lv_display_get_vertical_resolution(disp);
        lv_area_t full_area;
        lv_area_set(&full_area, 0, 0, disp_width, disp_height);
        lv_drm_ctx_t * ctx = lv_display_get_driver_data(disp);
        LV_LOG_USER("Texture id is %d %d %d %d %d %d %d", ctx->texture.texture_id, full_area.x1, full_area.x2,
                    full_area.y1,
                    full_area.y2, disp_width, disp_height);
        lv_opengles_viewport(0, 0, disp_width, disp_height);
        lv_opengles_egl_clear(ctx->egl_ctx);

        lv_opengles_render_texture(ctx->texture.texture_id, &full_area, LV_OPA_COVER, disp_width, disp_height,
                                   area, ctx->h_flip,
                                   !ctx->v_flip);
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
        const int32_t disp_width = lv_display_get_horizontal_resolution(disp);
        const int32_t disp_height = lv_display_get_vertical_resolution(disp);
        lv_opengles_viewport(0, 0, disp_width, disp_height);
        lv_opengles_egl_clear(ctx->egl_ctx);

        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ctx->texture.texture_id));

        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / lv_color_format_get_size(cf)));
        /*Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, disp->hor_res, disp->ver_res, 0, GL_RED, GL_UNSIGNED_BYTE, dsc->fb1));
#elif LV_COLOR_DEPTH == 16
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp->hor_res, disp->ver_res, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                             dsc->fb1));
#elif LV_COLOR_DEPTH == 24
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, disp->hor_res, disp->ver_res, 0, GL_BGR, GL_UNSIGNED_BYTE, dsc->fb1));
#elif LV_COLOR_DEPTH == 32
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, disp->hor_res, disp->ver_res, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                             ctx->texture.fb1));
#else
#error("Unsupported color format")
#endif

        lv_area_t full_area;
        lv_area_set(&full_area, 0, 0, disp_width, disp_height);
        lv_opengles_render_texture(ctx->texture.texture_id, &full_area, LV_OPA_COVER, disp_width, disp_height,
                                   &full_area, ctx->h_flip,
                                   ctx->v_flip);

        lv_opengles_egl_update(ctx->egl_ctx);
    }
    lv_display_flush_ready(disp);
}
#endif

#if 0
#define BOX_WIDTH 23
#define MAX_LINE_LEN 256


typedef enum { DISP_MINIMAL = 0, DISP_LIST = 1, DISP_TABLED = 2 } display_mode_t;

#endif
struct drmfb_state {
    int fd;
    struct gbm_bo * bo;
    uint32_t fb_id;
};

typedef struct drmfb_state * drmfb_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0
static void print_grouped_modes(drmModeConnectorPtr conn);
static void print_grouped_modes_ex(drmModeConnectorPtr conn, uint32_t current_idx, display_mode_t mode,
                                   const char ** banner_lines, int banner_count);
static size_t distance_uint32_array(uint32_t * arr, size_t len, uint32_t value);
static void populate_output_core(void * outmod_ptr);
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


void lv_egl_adapter_outmod_drm_cleanup(lv_drm_ctx_t * ctx)
{

    if(!ctx) return;

    if(ctx->drm_crtc) {
        int status = drmModeSetCrtc(ctx->fd,
                                    ctx->drm_crtc->crtc_id,
                                    ctx->drm_crtc->buffer_id,
                                    ctx->drm_crtc->x,
                                    ctx->drm_crtc->y,
                                    &ctx->drm_connector->connector_id,
                                    1,
                                    &ctx->drm_crtc->mode);
        if(status < 0) {
            /* Log.error expected externally; keep silent if not present */
        }
        drmModeFreeCrtc(ctx->drm_crtc);
        ctx->drm_crtc = 0;
    }

    if(ctx->gbm_surface) {
        gbm_surface_destroy(ctx->gbm_surface);
        ctx->gbm_surface = 0;
    }

    if(ctx->gbm_dev) {
        gbm_device_destroy(ctx->gbm_dev);
        ctx->gbm_dev = 0;
    }

    if(ctx->drm_connector) {
        drmModeFreeConnector(ctx->drm_connector);
        ctx->drm_connector = 0;
    }

    if(ctx->drm_encoder) {
        drmModeFreeEncoder(ctx->drm_encoder);
        ctx->drm_encoder = 0;
    }

    if(ctx->drm_resources) {
        drmModeFreeResources(ctx->drm_resources);
        ctx->drm_resources = 0;
    }

    if(ctx->fd > 0) {
        drmClose(ctx->fd);
    }
    // free(ctx->core);
    ctx->fd = 0;
    ctx->drm_mode = 0;
}

void * drm_get_display_cb(void * driver_data)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *) driver_data;
    if(!ctx) {
        return NULL;
    }
    return (void *)ctx->gbm_dev;
}

#if 0 /* TODO: Figure out if this is used */
/* window: populate properties with mode size and return gbm_surface* as void* */
void * lv_egl_adapter_outmod_drm_window(void * nativedrm_ptr, native_window_properties_t const properties)
{
    lv_egl_adapter_outmod_drm_t * drm_out = (lv_egl_adapter_outmod_drm_t *) nativedrm_ptr;
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
#endif
void drm_fb_destroy_cb(struct gbm_bo * bo, void * data)
{
    LV_UNUSED(bo);
    struct drmfb_state * fb = (struct drmfb_state *) data;
    if(fb && fb->fb_id) drmModeRmFB(fb->fd, fb->fb_id);
    free(fb);
}

/* check_for_page_flip: select() on fd and handle drm events */
int drm_check_for_page_flip(lv_drm_ctx_t * ctx, int timeout_ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(ctx->fd, &fds);

    drmEventContext evCtx;
    lv_memset(&evCtx, 0, sizeof(evCtx));
    evCtx.version = 2;
    evCtx.page_flip_handler = drm_page_flip_handler;

    struct timeval timeout;
    struct timeval * timeout_ptr = NULL;
    if(timeout_ms >= 0) {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        timeout_ptr = &timeout;
    }

    int status = select(ctx->fd + 1, &fds, NULL, NULL, timeout_ptr);
    if(status == 1) {
        drmHandleEvent(ctx->fd, &evCtx);
    }
    return status;
}

drmfb_state_t lv_egl_adapter_outmod_drm_fb_get_from_bo(lv_drm_ctx_t * ctx, struct gbm_bo * bo)
{
    if(!bo) {
        LV_LOG_ERROR("variable bo is null\n");
        return NULL;
    }
    drmfb_state_t fb = (drmfb_state_t)gbm_bo_get_user_data(bo);
    if(fb) {
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
        LV_LOG_ERROR("Failed to create FB: %d", status);
        return NULL;
    }

    fb = (drmfb_state_t)malloc(sizeof(*fb));
    if(!fb) {
        LV_LOG_ERROR("Failed to allocate drmfb_state");
        return NULL;
    }

    fb->fd = ctx->fd;
    fb->bo = bo;
    fb->fb_id = fb_id;

    gbm_bo_set_user_data(bo, fb, drm_fb_destroy_cb);
    return fb;
}

void drm_flip_cb(void * driver_data, bool vsync)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *) driver_data;
    if(!ctx->crtc_isset && drmSetMaster(ctx->fd) < 0) {
        LV_LOG_ERROR("Failed to become DRM master (hint: must be run in a VT, shut down Wayland / X first )");
        return;
    }

    if(ctx->gbm_bo_pending) {
        gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_pending);
    }
    ctx->gbm_bo_pending = gbm_surface_lock_front_buffer(ctx->gbm_surface);

    if(!ctx->gbm_bo_pending) {
        LV_LOG_USER("Failed to lock front buffer");
        return;
    }

    drmfb_state_t pending_fb = lv_egl_adapter_outmod_drm_fb_get_from_bo(ctx, ctx->gbm_bo_pending);

    if(!ctx->gbm_bo_pending || !pending_fb) {
        LV_LOG_ERROR("Failed to get gbm front buffer");
        return;
    }

    if(vsync) {
        while(ctx->gbm_bo_flipped && drm_check_for_page_flip(ctx, -1) >= 0)
            continue;
    }
    else {
        drm_check_for_page_flip(ctx, 0);
    }

    if(!ctx->gbm_bo_flipped) {
        if(!ctx->crtc_isset) {
            int status = drmModeSetCrtc(ctx->fd, ctx->drm_encoder->crtc_id, pending_fb->fb_id, 0, 0,
                                        &(ctx->drm_connector->connector_id), 1, ctx->drm_mode);
            if(status >= 0) {
                ctx->crtc_isset = true;
                if(ctx->gbm_bo_presented)
                    gbm_surface_release_buffer(ctx->gbm_surface, ctx->gbm_bo_presented);
                ctx->gbm_bo_presented = ctx->gbm_bo_pending;
                ctx->gbm_bo_flipped = NULL;
                ctx->gbm_bo_pending = NULL;
            }
            else {
                LV_LOG_ERROR("Failed to set crtc: %d", status);
            }
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
          drm_check_for_page_flip(ctx, -1) >= 0) continue;
}

#if 0
bool lv_egl_adapter_outmod_drm_create_window(void * nativedrm_ptr, native_window_properties_t const properties)
{

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
#endif

inline static int node_path_is_valid(const char * path)
{
    return path && path[0] != '\0';
}

lv_result_t drm_device_init(lv_drm_ctx_t * ctx, const char * path)
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
    LV_ASSERT(ctx->drm_connector->count_modes > 0);
    /* During testing, the first mode is always the screen resolution so we use that*/
    ctx->drm_mode = &ctx->drm_connector->modes[0];

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

    ctx->drm_crtc = drm_get_crtc(ctx);
    if(!ctx->drm_crtc) {
        LV_LOG_ERROR("Failed to get crtc");
        goto get_crtc_err;
    }

    return LV_RESULT_OK;

get_crtc_err:
    gbm_device_destroy(ctx->gbm_dev);
    ctx->gbm_dev = NULL;
gbm_create_device_err:
    drmModeFreeEncoder(ctx->drm_encoder);
    ctx->drm_encoder = NULL;
get_encoder_err:
    drmModeFreeConnector(ctx->drm_connector);
    ctx->drm_connector = NULL;
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

void lv_drm_ctx_create(lv_drm_ctx_t * ctx)
{
    lv_memset(ctx, 0, sizeof(*ctx));
}

size_t drm_egl_select_config_cb(void * driver_data, lv_egl_config_t * configs, size_t config_count)
{
    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *)driver_data;
    int32_t target_w = lv_display_get_horizontal_resolution(ctx->display);
    int32_t target_h = lv_display_get_vertical_resolution(ctx->display);
    lv_color_format_t target_cf = lv_display_get_color_format(ctx->display);
    if(target_cf == LV_COLOR_FORMAT_NATIVE) {

    }

    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_USER("Got config %zu %dx%d cf %d  buffer size %d depth %d  samples %d stencil %d surface type %d",
                    i,
                    configs[i].max_width, configs[i].max_height, configs[i].cf,
                    configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].stencil, configs[i].surface_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        if(configs[i].max_width >= target_w &&
           configs[i].max_height >= target_h &&
           configs[i].cf == target_cf &&
           configs[i].surface_type & EGL_WINDOW_BIT
          ) {
            LV_LOG_USER("Choosing config %zu", i);
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
        .select_config = drm_egl_select_config_cb,
        .driver_data = ctx,
        .native_display = ctx->gbm_dev,
        .egl_platform = EGL_PLATFORM_GBM_KHR,
        .flip_cb = drm_flip_cb,
        .create_window_cb = drm_create_window,
        .destroy_window_cb = drm_destroy_window,
    };
}

static size_t distance_uint32_array(uint32_t * arr, size_t len, uint32_t value)
{
    for(size_t i = 0; i < len; i++) if(arr[i] == value) return i;
    return len;
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
                    uint64_t * tmp = lv_realloc(mods_arr, new_cap * sizeof(uint64_t));
                    if(!tmp) {
                        lv_free(mods_arr);
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
        lv_free(mods_arr);
        *out_count = 0;
        return NULL;
    }

    /* shrink to fit */
    {
        uint64_t * tmp = lv_realloc(mods_arr, mods_len * sizeof(uint64_t));
        if(tmp) mods_arr = tmp;
    }

    *out_count = mods_len;
    return mods_arr;
}

static lv_color_format_t drm_get_default_color_format(lv_drm_ctx_t * ctx)
{
    lv_display_t * display = ctx->display;

    lv_color_format_t cf = lv_display_get_color_format(display);
    if(cf != LV_COLOR_FORMAT_NATIVE) {
        /* Color format has been set by user keep it*/
        return cf;
    }

    switch(LV_COLOR_DEPTH) {
        case 16:
            return LV_COLOR_FORMAT_RGB565;
        case 24:
            return LV_COLOR_FORMAT_RGB888;
        case 32:
            return LV_COLOR_FORMAT_ARGB8888;
        default:
            LV_LOG_ERROR("Unsupported color format");
            return LV_COLOR_FORMAT_UNKNOWN;
    }

}

static void * drm_create_window(void * driver_data, const lv_native_window_properties_t * properties)
{

    lv_drm_ctx_t * ctx = (lv_drm_ctx_t *)driver_data;
    LV_ASSERT_NULL(ctx->gbm_dev);
    // uint32_t format = properties->visual_id;

    int crtc_index = (int)(distance_uint32_array(ctx->drm_resources->crtcs,
                                                 ctx->drm_resources->count_crtcs,
                                                 ctx->drm_crtc->crtc_id));

    uint32_t format = properties->visual_id;
    lv_array_t valid_mods;
    lv_array_init(&valid_mods, 0, sizeof(uint64_t));
    if(properties->mod_count != 0) {
        size_t drm_mods_count = 0;
        uint64_t * drm_mods = drm_get_format_mods(ctx->fd, format, crtc_index, &drm_mods_count);


        for(size_t di = 0; di < drm_mods_count; ++di) {
            uint64_t mod = drm_mods[di];
            for(size_t i = 0; i < properties->mod_count; ++i) {
                if(mod == properties->mods[i]) {
                    lv_array_push_back(&valid_mods, &properties->mods[i]);
                }
            }
        }
        free(drm_mods);
    }
    const bool has_valid_mods = lv_array_size(&valid_mods) >= 1 &&
                                *(uint64_t *)lv_array_at(&valid_mods, 0) != DRM_FORMAT_MOD_INVALID;

    if(!has_valid_mods) {
        LV_LOG_USER("gbm_surface_create without modifiers");
        ctx->gbm_surface = gbm_surface_create(ctx->gbm_dev, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay, format,
                                              GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    else {
        LV_LOG_USER("gbm_surface_create with modifiers");
        ctx->gbm_surface = gbm_surface_create_with_modifiers2(
                               ctx->gbm_dev, ctx->drm_mode->hdisplay, ctx->drm_mode->vdisplay,
                               format, (uint64_t *)valid_mods.data, lv_array_size(&valid_mods),
                               GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
    }
    lv_array_deinit(&valid_mods);

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
    gbm_surface_destroy(ctx->gbm_surface);
    ctx->gbm_surface = NULL;
}

static void drm_page_flip_handler(int fd, unsigned int frame, unsigned int sec, unsigned int usec,
                                  void * data)
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

#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

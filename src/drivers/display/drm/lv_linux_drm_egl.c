/**
 * @file lv_linux_drm_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_linux_drm.h"
#include <src/display/lv_display.h>
#include <src/drivers/display/drm/lv_linux_drm_egl.h>
#include <src/drivers/opengles/egl_adapter/lv_egl_adapter_interface.h>
#include <src/drivers/opengles/lv_opengles_texture_private.h>
#include <src/lv_conf_internal.h>
#include <src/misc/lv_event.h>
#include <src/misc/lv_types.h>

#if LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL

#include <stdlib.h>
#include <stdint.h>
#include <gbm.h>
#include <xf86drm.h>

#include "lv_linux_drm_egl_private.h"
#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_string.h"
#include "../../../core/lv_global.h"
#include "../../../display/lv_display_private.h"
#include "../../../indev/lv_indev.h"
#include "../../../lv_init.h"
#include "../../../misc/lv_area_private.h"
#include "../../opengles/egl_adapter/common/opengl_headers.h"
#include "../../opengles/egl_adapter/lv_egl_adapter.h"
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
    lv_drm_egl_t * ctx = lv_zalloc(sizeof(*ctx));
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
    LV_UNUSED(file);
    LV_UNUSED(connector_id);

    lv_drm_egl_t * ctx = lv_display_get_driver_data(display);
    LV_LOG_USER("Ctx is %p", ctx);

    lv_egl_adapter_config_t * egl_options = lv_egl_adapter_config_create();
    lv_egl_adapter_config_set_vsync(egl_options, true);
    lv_egl_adapter_t * egl_adapter = lv_egl_adapter_create(egl_options);

    ctx->egl_interface = lv_egl_adapter_interface_create(egl_adapter, lv_display_get_horizontal_resolution(display),
                                                         lv_display_get_vertical_resolution(display),
                                                         1000. / LV_DEF_REFR_PERIOD);
    if(!ctx->egl_interface) {
        LV_LOG_ERROR("Failed to create egl interface");
        return;
    }
    int32_t h_res = lv_display_get_horizontal_resolution(ctx->display);
    int32_t v_res = lv_display_get_vertical_resolution(ctx->display);
    LV_LOG_USER("Using %dx%d", h_res, v_res);

    lv_result_t res = lv_opengles_texture_create_from_display(display, h_res, v_res);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize opengl texture");
        lv_egl_adapter_interface_deinit(ctx->egl_interface);
        ctx->egl_interface = NULL;
        return;
    }

    /* Driver data gets set by opengles_texture. We overwrite it with our own by keeping the first attribute the same*/
    lv_opengles_texture_t * texture = lv_display_get_driver_data(display);
    ctx->texture = *texture;
    lv_display_set_driver_data(display, ctx);

    LV_LOG_USER("Ctx is %p", ctx);

    ctx->h_flip = false;
    ctx->v_flip = false;
    lv_display_set_flush_cb(display, flush_cb);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_add_event_cb(ctx->display, event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
}

void lv_drm_egl_set_flip(lv_drm_egl_t * window, bool h_flip, bool v_flip)
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
    lv_drm_egl_t * ctx = lv_display_get_driver_data(display);
    switch(code) {
        case LV_EVENT_DELETE:
            lv_egl_adapter_reset(ctx->egl_interface->egl_adapter);
            lv_egl_adapter_interface_deinit(ctx->egl_interface);
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
        lv_drm_egl_t * ctx = lv_display_get_driver_data(disp);
        lv_opengles_viewport(0, 0, disp_width, disp_height);
        lv_egl_adapter_interface_clear();

        LV_LOG_USER("Texture id is %d %d %d %d %d %d %d", ctx->opengl_texture.texture_id, full_area.x1, full_area.x2,
                    full_area.y1,
                    full_area.y2, disp_width, disp_height);
        lv_opengles_render_texture(ctx->texture.texture_id, &full_area, LV_OPA_COVER, disp_width, disp_height,
                                   area, ctx->h_flip,
                                   !ctx->v_flip);
        lv_egl_adapter_interface_update(ctx->egl_interface);
    }
    lv_display_flush_ready(disp);
}

#else

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(px_map);
    LV_UNUSED(area);
    if(lv_display_flush_is_last(disp)) {
        lv_drm_egl_t * ctx = lv_display_get_driver_data(disp);
        LV_LOG_USER("Ctx is %p", ctx);
        const int32_t disp_width = lv_display_get_horizontal_resolution(disp);
        const int32_t disp_height = lv_display_get_vertical_resolution(disp);

        LV_LOG_USER("Viewport");
        lv_opengles_viewport(0, 0, disp_width, disp_height);
        LV_LOG_USER("Clear");
        lv_egl_adapter_interface_clear();

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

        LV_LOG_USER("Render texture");
        lv_area_t full_area;
        lv_area_set(&full_area, 0, 0, disp_width, disp_height);
        lv_opengles_render_texture(ctx->texture.texture_id, &full_area, LV_OPA_COVER, disp_width, disp_height,
                                   &full_area, ctx->h_flip,
                                   ctx->v_flip);

        LV_LOG_USER("Update");
        lv_egl_adapter_interface_update(ctx->egl_interface);

        LV_LOG_USER("update done");
    }
    lv_display_flush_ready(disp);
}
#endif




#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_USE_EGL*/

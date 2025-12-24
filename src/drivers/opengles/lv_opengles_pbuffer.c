
/**
 * @file lv_opengles_pbuffer.c
 * @brief PBuffer-based EGL display for offscreen rendering
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_opengles_pbuffer.h"
#if LV_USE_EGL

#include "lv_opengles_egl.h"
#include "lv_opengles_egl_private.h"
#include "lv_opengles_driver.h"
#include "lv_opengles_texture_private.h"
#include "lv_opengles_debug.h"
#include "lv_opengles_private.h"
#include "../../draw/lv_draw_buf.h"
#include "../../display/lv_display.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_opengles_texture_t texture;
    lv_opengles_egl_t * egl_ctx;
    lv_egl_interface_t egl_interface;
    int32_t width;
    int32_t height;
} lv_pbuffer_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void pbuffer_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void pbuffer_event_cb(lv_event_t * e);
static void * pbuffer_create_surface(void * driver_data, const lv_egl_create_surface_params_t * params);
static size_t pbuffer_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static void pbuffer_flip_cb(void * driver_data, bool vsync);
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

lv_display_t * lv_opengles_pbuffer_create(int32_t hor_res, int32_t ver_res)
{
    lv_pbuffer_ctx_t * ctx = lv_zalloc(sizeof(*ctx));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        LV_LOG_ERROR("Failed to create pbuffer context");
        return NULL;
    }

    ctx->width = hor_res;
    ctx->height = ver_res;

    /* Setup EGL interface for PBuffer */
    ctx->egl_interface = (lv_egl_interface_t) {
        .driver_data = ctx,
        .native_display = EGL_DEFAULT_DISPLAY,
        .egl_platform = GLAD_EGL_MESA_platform_surfaceless,
        .select_config = pbuffer_select_config_cb,
        .flip_cb = pbuffer_flip_cb,
        .create_surface_cb = pbuffer_create_surface,
        .destroy_surface_cb = NULL, /* Default surface destroyer works*/
    };

    /* Create EGL context */
    ctx->egl_ctx = lv_opengles_egl_context_create(&ctx->egl_interface);
    if(!ctx->egl_ctx) {
        LV_LOG_ERROR("Failed to create EGL context");
        lv_free(ctx);
        return NULL;
    }

    lv_opengles_init();

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(!disp) {
        LV_LOG_ERROR("Failed to create display");
        lv_opengles_egl_context_destroy(ctx->egl_ctx);
        lv_free(ctx);
        return NULL;
    }

    ctx->texture.is_texture_owner = true;
    lv_result_t res = lv_opengles_texture_reshape(&ctx->texture, disp, hor_res, ver_res);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create opengl texture");
        lv_opengles_egl_context_destroy(ctx->egl_ctx);
        lv_free(ctx);
        lv_display_delete(disp);
        return NULL;
    }

    lv_display_set_driver_data(disp, ctx);
    lv_display_set_flush_cb(disp, pbuffer_flush_cb);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_add_event_cb(disp, pbuffer_event_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);
    lv_display_add_event_cb(disp, pbuffer_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, pbuffer_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, pbuffer_event_cb, LV_EVENT_DELETE, NULL);

    return disp;
}

lv_result_t lv_opengles_pbuffer_read_to_draw_buf(lv_display_t * display, lv_draw_buf_t * draw_buf)
{
    lv_pbuffer_ctx_t * ctx = lv_display_get_driver_data(display);
    if(!ctx || !draw_buf) {
        return LV_RESULT_INVALID;
    }

    lv_color_format_t cf = draw_buf->header.cf;
    int32_t width = draw_buf->header.w;
    int32_t height = draw_buf->header.h;
    uint8_t * data = draw_buf->data;

    if(width != ctx->width || height != ctx->height) {
        LV_LOG_ERROR("Draw buffer size mismatch: expected %dx%d, got %dx%d",
                     ctx->width, ctx->height, width, height);
        return LV_RESULT_INVALID;
    }

    if(cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data));
    }
    else if(cf == LV_COLOR_FORMAT_RGB888) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data));
    }
    else if(cf == LV_COLOR_FORMAT_RGB565) {
        GL_CALL(glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data));
    }
    else {
        LV_LOG_ERROR("Unsupported color format for readback: %d", cf);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void set_viewport(lv_display_t * display)
{
    const lv_display_rotation_t rotation = lv_display_get_rotation(display);
    int32_t disp_width, disp_height;
    if(rotation == LV_DISPLAY_ROTATION_0 || rotation == LV_DISPLAY_ROTATION_180) {
        disp_width = lv_display_get_horizontal_resolution(display);
        disp_height = lv_display_get_vertical_resolution(display);
    }
    else {
        disp_width = lv_display_get_vertical_resolution(display);
        disp_height = lv_display_get_horizontal_resolution(display);
    }
    lv_opengles_viewport(0, 0, disp_width, disp_height);
}

static void pbuffer_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);

    if(lv_display_flush_is_last(disp)) {
        set_viewport(disp);
        lv_opengles_render_params_t params  = {
            .h_flip = false,
            .v_flip = false,
            .rb_swap = false
        };
        lv_opengles_render_display(disp, &params);
    }
    lv_display_flush_ready(disp);
}

static void pbuffer_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_display_t * display = (lv_display_t *)lv_event_get_target(e);
    lv_pbuffer_ctx_t * ctx = lv_display_get_driver_data(display);

    switch(code) {
        case LV_EVENT_DELETE:
            if(ctx) {
                lv_opengles_texture_deinit(&ctx->texture);
                lv_opengles_egl_context_destroy(ctx->egl_ctx);
                lv_display_set_driver_data(display, NULL);
                lv_free(ctx);
            }
            break;
        case LV_EVENT_RESOLUTION_CHANGED: {
                lv_result_t res = lv_opengles_texture_reshape(&ctx->texture, display,
                                                              lv_display_get_horizontal_resolution(display),
                                                              lv_display_get_vertical_resolution(display));
                if(res != LV_RESULT_OK) {
                    LV_LOG_ERROR("Failed to resize display");
                }
            }
            break;
        default:
            LV_LOG_USER("Unhandled event '%s'", lv_event_code_get_name(code));
            return;
    }
}

static void * pbuffer_create_surface(void * driver_data, const lv_egl_create_surface_params_t * params)
{
    lv_pbuffer_ctx_t * ctx = (lv_pbuffer_ctx_t *)driver_data;
    LV_UNUSED(params);

    const EGLint pbuffer_attribs[] = {
        EGL_WIDTH, ctx->width,
        EGL_HEIGHT, ctx->height,
        EGL_NONE
    };

    return eglCreatePbufferSurface(params->display,
                                   params->config,
                                   pbuffer_attribs);
}

static size_t pbuffer_select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
{
    lv_pbuffer_ctx_t * ctx = (lv_pbuffer_ctx_t *)driver_data;

#if LV_COLOR_DEPTH == 16
    lv_color_format_t target_cf = LV_COLOR_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
    lv_color_format_t target_cf = LV_COLOR_FORMAT_ARGB8888;
#else
#error("Unsupported color format")
#endif

    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_TRACE("Config %zu: %#x %dx%d RGBA(%d,%d,%d,%d) buffer:%d depth:%d samples:%d surface:%d",
                     i, configs[i].id,
                     configs[i].max_width, configs[i].max_height,
                     configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                     configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].surface_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        lv_color_format_t config_cf = lv_opengles_egl_color_format_from_egl_config(&configs[i]);

        if(configs[i].max_width >= ctx->width &&
           configs[i].max_height >= ctx->height &&
           config_cf == target_cf &&
           (configs[i].surface_type & EGL_PBUFFER_BIT)) {
            LV_LOG_INFO("Selected PBuffer config %zu", i);
            return i;
        }
    }

    LV_LOG_ERROR("No suitable PBuffer config found");
    return config_count;
}

static void pbuffer_flip_cb(void * driver_data, bool vsync)
{
    LV_UNUSED(driver_data);
    LV_UNUSED(vsync);
    /* PBuffer doesn't need flipping - it's offscreen */
}

#endif /*LV_USE_EGL*/

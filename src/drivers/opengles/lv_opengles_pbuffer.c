/**
 * @file lv_opengles_pbuffer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#if LV_USE_OPENGLES_PBUFFER

#include "lv_opengles_pbuffer.h"
#include "glad/include/glad/egl.h"
#include "lv_opengles_debug.h"
#include "lv_opengles_private.h"
#include "lv_opengles_egl_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_opengles_pbuffer {
    lv_opengles_egl_t * egl_ctx;
    lv_egl_interface_t egl_interface;
    lv_draw_buf_t * draw_buf;
    GLuint fbo;
    GLuint color_tex;
    GLuint stencil_rbo;
    int32_t width;
    int32_t height;
};

typedef struct _lv_opengles_pbuffer lv_pbuffer_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t init_render_target(lv_pbuffer_ctx_t * ctx);
static void deinit_render_target(lv_pbuffer_ctx_t * ctx);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void * create_surface_cb(void * driver_data, const lv_egl_create_surface_params_t * params);
static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count);
static void flip_cb(void * driver_data, bool vsync);

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
        LV_LOG_ERROR("Failed to allocate the pbuffer context");
        return NULL;
    }

    ctx->width = hor_res;
    ctx->height = ver_res;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(!disp) {
        LV_LOG_ERROR("Failed to create the pbuffer display");
        lv_free(ctx);
        return NULL;
    }

    ctx->egl_interface = (lv_egl_interface_t) {
        .driver_data = ctx,
        .native_display = EGL_DEFAULT_DISPLAY,
        .egl_platform = EGL_PLATFORM_SURFACELESS_MESA,
        .select_config = select_config_cb,
        .flip_cb = flip_cb,
        .create_surface_cb = create_surface_cb,
        .destroy_surface_cb = NULL, /* default eglDestroySurface */
        .create_window_cb = NULL,
        .destroy_window_cb = NULL,
    };

    ctx->egl_ctx = lv_opengles_egl_context_create(&ctx->egl_interface);
    if(!ctx->egl_ctx) {
        LV_LOG_ERROR("Failed to create the EGL context for the pbuffer display");
        lv_display_delete(disp);
        lv_free(ctx);
        return NULL;
    }

    if(init_render_target(ctx) != LV_RESULT_OK) {
        lv_display_delete(disp);
        lv_opengles_pbuffer_destroy(ctx);
        return NULL;
    }

    ctx->draw_buf = lv_draw_buf_create(hor_res, ver_res, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    if(!ctx->draw_buf) {
        LV_LOG_ERROR("Failed to create the pbuffer draw buffer");
        lv_display_delete(disp);
        lv_opengles_pbuffer_destroy(ctx);
        return NULL;
    }

    lv_display_set_driver_data(disp, ctx);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_draw_buffers(disp, ctx->draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush_cb);

    LV_LOG_INFO("EGL pbuffer display created (%" LV_PRId32 "x%" LV_PRId32 ", OpenGL ES %d)",
                hor_res, ver_res, (int)lv_opengles_egl_get_gles_version(ctx->egl_ctx));

    return disp;
}

lv_result_t lv_opengles_pbuffer_read_to_draw_buf(lv_display_t * display, lv_draw_buf_t * draw_buf)
{
    lv_pbuffer_ctx_t * ctx = lv_display_get_driver_data(display);
    if(!ctx || !draw_buf) {
        return LV_RESULT_INVALID;
    }

    const int32_t w = draw_buf->header.w;
    const int32_t h = draw_buf->header.h;

    if(w != ctx->width || h != ctx->height) {
        LV_LOG_ERROR("Draw buffer size mismatch: expected %" LV_PRId32 "x%" LV_PRId32
                     ", got %" LV_PRId32 "x%" LV_PRId32,
                     ctx->width, ctx->height, w, h);
        return LV_RESULT_INVALID;
    }

    const lv_color_format_t cf = draw_buf->header.cf;
    if(cf != LV_COLOR_FORMAT_XRGB8888 && cf != LV_COLOR_FORMAT_ARGB8888) {
        LV_LOG_ERROR("Unsupported color format for readback: %d", (int)cf);
        return LV_RESULT_INVALID;
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo));

    /* OpenGL's origin is bottom-left and LVGL's is top-left,
     * so the rows are read back in reverse.
     */
    for(int32_t y = 0; y < h; y++) {
        uint8_t * row = lv_draw_buf_goto_xy(draw_buf, 0, h - 1 - y);
        GL_CALL(glReadPixels(0, y, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, row));

        /* GL returns RGBA byte order
         * LVGL XRGB8888/ARGB8888 is BGRA
         */
        uint8_t * px = row;
        for(int32_t x = 0; x < w; x++) {
            const uint8_t r = px[0];
            px[0] = px[2];
            px[2] = r;
            px += 4;
        }
    }

    return LV_RESULT_OK;
}

uint8_t lv_opengles_pbuffer_get_gles_version(lv_display_t * display)
{
    lv_pbuffer_ctx_t * ctx = lv_display_get_driver_data(display);
    if(!ctx) {
        return 0;
    }
    return lv_opengles_egl_get_gles_version(ctx->egl_ctx);
}

lv_opengles_pbuffer_t * lv_opengles_pbuffer_get_handle(lv_display_t * display)
{
    return lv_display_get_driver_data(display);
}

void lv_opengles_pbuffer_destroy(lv_opengles_pbuffer_t * pbuffer)
{
    if(!pbuffer) {
        return;
    }

    lv_opengles_deinit();
    deinit_render_target(pbuffer);
    lv_opengles_egl_context_destroy(pbuffer->egl_ctx);

    if(pbuffer->draw_buf) {
        lv_draw_buf_destroy(pbuffer->draw_buf);
        pbuffer->draw_buf = NULL;
    }
    lv_free(pbuffer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t init_render_target(lv_pbuffer_ctx_t * ctx)
{
    /* Render into an FBO rather than straight into the pbuffer. Mesa refuses
     * glReadPixels on a multisampled pbuffer, and the FBO carries its own stencil
     * buffer for NanoVG's clipping regardless of the config.
     */
    GL_CALL(glGenFramebuffers(1, &ctx->fbo));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo));

    GL_CALL(glGenTextures(1, &ctx->color_tex));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ctx->color_tex));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ctx->width, ctx->height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->color_tex, 0));

    GL_CALL(glGenRenderbuffers(1, &ctx->stencil_rbo));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, ctx->stencil_rbo));
    GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, ctx->width, ctx->height));
    GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ctx->stencil_rbo));

    const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        LV_LOG_ERROR("Pbuffer framebuffer is incomplete: %#x", status);
        deinit_render_target(ctx);
        return LV_RESULT_INVALID;
    }

    GL_CALL(glViewport(0, 0, ctx->width, ctx->height));

    return LV_RESULT_OK;
}

static void deinit_render_target(lv_pbuffer_ctx_t * ctx)
{
    if(ctx->stencil_rbo) {
        GL_CALL(glDeleteRenderbuffers(1, &ctx->stencil_rbo));
        ctx->stencil_rbo = 0;
    }
    if(ctx->color_tex) {
        GL_CALL(glDeleteTextures(1, &ctx->color_tex));
        ctx->color_tex = 0;
    }
    if(ctx->fbo) {
        GL_CALL(glDeleteFramebuffers(1, &ctx->fbo));
        ctx->fbo = 0;
    }
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);

    if(!lv_display_flush_is_last(disp)) {
        lv_display_flush_ready(disp);
        return;
    }

    lv_pbuffer_ctx_t * ctx = lv_display_get_driver_data(disp);

    lv_opengles_pbuffer_read_to_draw_buf(disp, ctx->draw_buf);

    lv_display_flush_ready(disp);
}

static void * create_surface_cb(void * driver_data, const lv_egl_create_surface_params_t * params)
{
    lv_pbuffer_ctx_t * ctx = (lv_pbuffer_ctx_t *)driver_data;

    const EGLint pbuffer_attribs[] = {
        EGL_WIDTH, ctx->width,
        EGL_HEIGHT, ctx->height,
        EGL_NONE
    };

    return eglCreatePbufferSurface(params->display, params->config, pbuffer_attribs);
}

static size_t select_config_cb(void * driver_data, const lv_egl_config_t * configs, size_t config_count)
{
    lv_pbuffer_ctx_t * ctx = (lv_pbuffer_ctx_t *)driver_data;

    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_TRACE("Got config %zu %#x %dx%d RGBA(%d,%d,%d,%d) buffer size %d depth %d samples %d stencil %d "
                     "surface type %#x renderable type %#x",
                     i, configs[i].id, configs[i].max_width, configs[i].max_height,
                     configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                     configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].stencil,
                     configs[i].surface_type, configs[i].renderable_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        const bool is_pbuffer = (configs[i].surface_type & EGL_PBUFFER_BIT) != 0;
        const bool resolution_fits = configs[i].max_width >= ctx->width &&
                                     configs[i].max_height >= ctx->height;
        const bool is_rgb8 = configs[i].r_bits == 8 && configs[i].g_bits == 8 && configs[i].b_bits == 8;
        const bool is_es2_capable = (configs[i].renderable_type & EGL_OPENGL_ES2_BIT) != 0;

        if(is_pbuffer && resolution_fits && is_rgb8 && is_es2_capable && configs[i].samples == 0) {
            LV_LOG_INFO("Choosing pbuffer config %zu (%#x)", i, configs[i].id);
            return i;
        }
    }

    LV_LOG_ERROR("Failed to find a suitable pbuffer EGL config");
    return config_count;
}

static void flip_cb(void * driver_data, bool vsync)
{
    LV_UNUSED(driver_data);
    LV_UNUSED(vsync);
    /* Nothing to present: the pbuffer is off-screen. */
}

#endif /*LV_USE_OPENGLES_PBUFFER*/

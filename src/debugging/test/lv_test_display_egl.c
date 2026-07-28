/**
 * @file lv_test_display_egl.c
 *
 * Headless EGL display for NanoVG unit testing.
 * Creates an EGL context with pbuffer surface and FBO for off-screen rendering.
 * After each frame, pixels are read back from the FBO into the LVGL draw buffer
 * so that screenshot comparison works identically to the SW renderer tests.
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl_public.h"

#ifndef LV_USE_NANOVG_TEST_HEADLESS
    #define LV_USE_NANOVG_TEST_HEADLESS 0
#endif

#if LV_USE_TEST && LV_USE_DRAW_NANOVG && LV_USE_NANOVG_TEST_HEADLESS

#include "../../draw/nanovg/lv_draw_nanovg.h"

/* Include EGL and GLES2 directly - do NOT include lvgl_private.h here
 * because it pulls in glad headers that conflict with system EGL/GLES2. */
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    EGLDisplay egl_display;
    EGLContext egl_context;
    EGLSurface egl_surface;
    GLuint fbo;
    GLuint color_tex;
    GLuint stencil_rbo;
    lv_draw_buf_t * draw_buf;
    int32_t width;
    int32_t height;
} egl_test_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static void egl_delete_event_cb(lv_event_t * e);
static void egl_resolution_changed_cb(lv_event_t * e);
static void egl_color_format_changed_cb(lv_event_t * e);
static bool init_egl(egl_test_ctx_t * ctx);
static bool init_fbo(egl_test_ctx_t * ctx);
static void deinit_egl(egl_test_ctx_t * ctx);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_test_display_egl_create(int32_t hor_res, int32_t ver_res)
{
    egl_test_ctx_t * ctx = lv_malloc_zeroed(sizeof(egl_test_ctx_t));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        return NULL;
    }
    ctx->width = hor_res;
    ctx->height = ver_res;

    /* Initialize EGL headless context */
    if(!init_egl(ctx)) {
        LV_LOG_ERROR("Failed to initialize EGL for NanoVG headless testing");
        lv_test_display_egl_cleanup(ctx);
        return NULL;
    }

    /* Create FBO as render target */
    if(!init_fbo(ctx)) {
        LV_LOG_ERROR("Failed to create FBO for NanoVG headless testing");
        lv_test_display_egl_cleanup(ctx);
        return NULL;
    }

    /* Bind the FBO as default render target */
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);
    glViewport(0, 0, hor_res, ver_res);

    /* Create LVGL display */
    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    lv_display_set_driver_data(disp, ctx);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_XRGB8888);

    /* Full-screen draw buffer for glReadPixels readback */
    ctx->draw_buf = lv_draw_buf_create(hor_res, ver_res, LV_COLOR_FORMAT_XRGB8888, LV_STRIDE_AUTO);
    if(!ctx->draw_buf) {
        LV_LOG_ERROR("Failed to create draw buffer");
        lv_display_delete(disp);
        lv_test_display_egl_cleanup(ctx); /* deletes FBO/texture/stencil then EGL */
        return NULL;
    }

    lv_display_set_draw_buffers(disp, ctx->draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_FULL);

    lv_display_set_flush_cb(disp, egl_flush_cb);
    lv_display_add_event_cb(disp, egl_delete_event_cb, LV_EVENT_DELETE, NULL);

    /* The FBO and readback path are fixed to the initial resolution and a 32bpp
     * BGRA layout. Guard against tests that mutate these on the default display,
     * which would silently desync the FBO/draw buffer and corrupt screenshots. */
    lv_display_add_event_cb(disp, egl_resolution_changed_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_display_add_event_cb(disp, egl_color_format_changed_cb, LV_EVENT_COLOR_FORMAT_CHANGED, NULL);

    /* Initialize NanoVG draw unit - this requires an active GL context */
    lv_draw_nanovg_init();

    LV_LOG_INFO("EGL headless test display created (%dx%d)", (int)hor_res, (int)ver_res);
    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Flush callback: read pixels from FBO back into the LVGL draw buffer.
 * This makes the rendered content available for screenshot comparison.
 */
static void egl_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    egl_test_ctx_t * ctx = lv_display_get_driver_data(disp);

    /* Ensure we're reading from our FBO */
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);

    /* Read the full display buffer back from GPU */
    int32_t w = ctx->width;
    int32_t h = ctx->height;
    lv_draw_buf_t * draw_buf = ctx->draw_buf;

    /* OpenGL has origin at bottom-left, LVGL at top-left.
     * Read row by row with Y-flip. */
    for(int32_t y = 0; y < h; y++) {
        uint8_t * row = (uint8_t *)lv_draw_buf_goto_xy(draw_buf, 0, h - 1 - y);
        glReadPixels(0, y, w, 1, GL_RGBA, GL_UNSIGNED_BYTE, row);

        /* Swizzle RGBA -> BGRA (XRGB8888 in LVGL is actually BGRX in memory on little-endian) */
        uint8_t * px = row;
        for(int32_t x = 0; x < w; x++) {
            uint8_t r = px[0];
            px[0] = px[2]; /* B */
            px[2] = r;     /* R */
            px += 4;
        }
    }

    lv_display_flush_ready(disp);
}

static void egl_delete_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    egl_test_ctx_t * ctx = lv_display_get_driver_data(disp);
    if(ctx && ctx->draw_buf) {
        lv_draw_buf_destroy(ctx->draw_buf);
        ctx->draw_buf = NULL;
    }

    /* NOTE: Do NOT destroy EGL context or GL resources here.
     * lv_deinit() deletes displays before draw units, so the NanoVG
     * draw unit's delete_cb may still execute GL calls after this.
     * Cleanup is done via lv_test_display_egl_cleanup() called
     * from lv_test_deinit() after lv_deinit(). */
}

static void egl_resolution_changed_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    egl_test_ctx_t * ctx = lv_display_get_driver_data(disp);
    int32_t hor_res = lv_display_get_original_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_original_vertical_resolution(disp);

    /* The FBO/viewport and draw buffer are fixed to the size used at creation.
     * Resizing would require recreating them, which this headless test display
     * does not support. Fail loudly so the offending test is caught rather than
     * silently producing corrupt screenshots. */
    LV_ASSERT_FORMAT_MSG(hor_res == ctx->width && ver_res == ctx->height,
                         "NanoVG headless test display does not support changing resolution "
                         "(%" LV_PRId32 "x%" LV_PRId32 " -> %" LV_PRId32 "x%" LV_PRId32 "); "
                         "skip or adjust this test for NanoVG",
                         ctx->width, ctx->height, hor_res, ver_res);
}

static void egl_color_format_changed_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_target(e);
    lv_color_format_t cf = lv_display_get_color_format(disp);

    /* The readback path always reads the FBO back as straight-alpha 32bpp BGRA
     * (XRGB8888/ARGB8888) using a fixed draw buffer, ignoring disp->color_format.
     * Other formats (incl. premultiplied) can't be represented, so reject them. */
    LV_ASSERT_FORMAT_MSG(cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888,
                         "NanoVG headless test display only supports XRGB8888/ARGB8888 "
                         "(got color format %d); skip or adjust this test for NanoVG",
                         (int)cf);
}

void lv_test_display_egl_cleanup(void * egl_ctx)
{
    /* Called after lv_deinit() when NanoVG draw unit is already destroyed */
    egl_test_ctx_t * ctx = egl_ctx;
    if(ctx == NULL) {
        return;
    }

    if(ctx->fbo) {
        glDeleteFramebuffers(1, &ctx->fbo);
        ctx->fbo = 0;
    }
    if(ctx->color_tex) {
        glDeleteTextures(1, &ctx->color_tex);
        ctx->color_tex = 0;
    }
    if(ctx->stencil_rbo) {
        glDeleteRenderbuffers(1, &ctx->stencil_rbo);
        ctx->stencil_rbo = 0;
    }
    deinit_egl(ctx);

    lv_free(ctx);
}

static bool init_egl(egl_test_ctx_t * ctx)
{
    /* Try EGL device platform first (works without display server).
     * Use eglGetProcAddress to get extension functions dynamically,
     * avoiding conflicts with glad macro redefinitions. */
    PFNEGLQUERYDEVICESEXTPROC query_devices_fn =
        (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
    PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display_fn =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    ctx->egl_display = EGL_NO_DISPLAY;

    if(query_devices_fn && get_platform_display_fn) {
        EGLDeviceEXT devices[4];
        EGLint num_devices = 0;
        if(query_devices_fn(4, devices, &num_devices) && num_devices > 0) {
            ctx->egl_display = get_platform_display_fn(EGL_PLATFORM_DEVICE_EXT, devices[0], NULL);
        }
    }

    /* Fallback: try surfaceless platform */
    if(ctx->egl_display == EGL_NO_DISPLAY) {
#ifdef EGL_PLATFORM_SURFACELESS_MESA
        if(get_platform_display_fn) {
            ctx->egl_display = get_platform_display_fn(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
        }
#endif
    }

    /* Last resort: default display */
    if(ctx->egl_display == EGL_NO_DISPLAY) {
        ctx->egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    }

    if(ctx->egl_display == EGL_NO_DISPLAY) {
        LV_LOG_ERROR("No EGL display available");
        return false;
    }

    EGLint major, minor;
    if(!eglInitialize(ctx->egl_display, &major, &minor)) {
        LV_LOG_ERROR("eglInitialize failed: 0x%x", eglGetError());
        return false;
    }
    LV_LOG_INFO("EGL %d.%d initialized", (int)major, (int)minor);

    eglBindAPI(EGL_OPENGL_ES_API);

    /* Choose config with pbuffer support, stencil for NanoVG clipping,
     * and 4x MSAA for GPU antialiasing (matching lv_sdl_egl.c config) */
    EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_SAMPLE_BUFFERS, 1,
        EGL_SAMPLES, 4,
        EGL_NONE
    };

    EGLConfig config;
    EGLint num_configs;
    if(!eglChooseConfig(ctx->egl_display, config_attribs, &config, 1, &num_configs) || num_configs == 0) {
        /* Fallback: try without surface type constraint */
        EGLint config_attribs_fallback[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };
        if(!eglChooseConfig(ctx->egl_display, config_attribs_fallback, &config, 1, &num_configs) || num_configs == 0) {
            LV_LOG_ERROR("No suitable EGL config found");
            eglTerminate(ctx->egl_display);
            return false;
        }
    }

    /* Create pbuffer surface */
    EGLint pbuf_attribs[] = {
        EGL_WIDTH, ctx->width,
        EGL_HEIGHT, ctx->height,
        EGL_NONE
    };
    ctx->egl_surface = eglCreatePbufferSurface(ctx->egl_display, config, pbuf_attribs);
    if(ctx->egl_surface == EGL_NO_SURFACE) {
        LV_LOG_WARN("Pbuffer surface creation failed, using surfaceless");
    }

    /* Create GLES2 context */
    EGLint ctx_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    ctx->egl_context = eglCreateContext(ctx->egl_display, config, EGL_NO_CONTEXT, ctx_attribs);
    if(ctx->egl_context == EGL_NO_CONTEXT) {
        LV_LOG_ERROR("eglCreateContext failed: 0x%x", eglGetError());
        eglTerminate(ctx->egl_display);
        return false;
    }

    /* Make current */
    if(!eglMakeCurrent(ctx->egl_display, ctx->egl_surface, ctx->egl_surface, ctx->egl_context)) {
        /* Try surfaceless */
        if(!eglMakeCurrent(ctx->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx->egl_context)) {
            LV_LOG_ERROR("eglMakeCurrent failed: 0x%x", eglGetError());
            eglDestroyContext(ctx->egl_display, ctx->egl_context);
            eglTerminate(ctx->egl_display);
            return false;
        }
    }

    LV_LOG_INFO("GL Version: %s", glGetString(GL_VERSION));
    LV_LOG_INFO("GL Renderer: %s", glGetString(GL_RENDERER));

    return true;
}

static bool init_fbo(egl_test_ctx_t * ctx)
{
    /* Create FBO with color texture and stencil renderbuffer */
    glGenFramebuffers(1, &ctx->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);

    /* KNOWN ISSUE: the color attachment is a single-sampled texture, so the 4x MSAA
     * requested on the EGL config does not apply to this FBO. Antialiasing therefore
     * relies on NanoVG's shader-based geometry AA, which is slower than hardware MSAA.
     * A proper fix would render into a multisampled color renderbuffer and blit-resolve
     * it into this texture before glReadPixels (needs GLES3 or the MSAA FBO extension). */
    /* Color attachment (RGBA texture) */
    glGenTextures(1, &ctx->color_tex);
    glBindTexture(GL_TEXTURE_2D, ctx->color_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ctx->width, ctx->height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->color_tex, 0);

    /* Stencil attachment (required by NanoVG for clipping) */
    glGenRenderbuffers(1, &ctx->stencil_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, ctx->stencil_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, ctx->width, ctx->height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ctx->stencil_rbo);

    /* Verify completeness */
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) {
        LV_LOG_ERROR("FBO incomplete: 0x%x", status);
        /* Clean up partially created GL objects */
        if(ctx->stencil_rbo) {
            glDeleteRenderbuffers(1, &ctx->stencil_rbo);
            ctx->stencil_rbo = 0;
        }
        if(ctx->color_tex) {
            glDeleteTextures(1, &ctx->color_tex);
            ctx->color_tex = 0;
        }
        if(ctx->fbo) {
            glDeleteFramebuffers(1, &ctx->fbo);
            ctx->fbo = 0;
        }
        return false;
    }

    LV_LOG_INFO("NanoVG test FBO created (%dx%d)", (int)ctx->width, (int)ctx->height);
    return true;
}

static void deinit_egl(egl_test_ctx_t * ctx)
{
    if(ctx->egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(ctx->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(ctx->egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(ctx->egl_display, ctx->egl_context);
        }
        if(ctx->egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(ctx->egl_display, ctx->egl_surface);
        }
        eglTerminate(ctx->egl_display);
    }
    lv_memzero(ctx, sizeof(*ctx));
}

#endif /* LV_USE_TEST && LV_USE_DRAW_NANOVG && LV_USE_NANOVG_TEST_HEADLESS */

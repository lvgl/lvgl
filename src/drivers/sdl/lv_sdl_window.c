/**
 * @file lv_sdl_window.c
 *
 */

/**
 * Modified by NXP in 2025
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL
#include <stdbool.h>
#include "../../core/lv_refr.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../lv_init.h"
#include "../../draw/lv_draw_buf.h"
#include "../../draw/nanovg/lv_draw_nanovg.h"
#include "../../drivers/opengles/lv_opengles_driver.h"

/* for aligned_alloc */
#ifndef __USE_ISOC11
    #define __USE_ISOC11
#endif
#ifndef _WIN32
    #include <stdlib.h>
#else
    #include <malloc.h>
#endif /* _WIN32 */

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include "lv_sdl_private.h"

#ifdef LV_SDL_USE_EGL
    #include <SDL2/SDL_syswm.h>
    #include <EGL/egl.h>
    #include <EGL/eglext.h>
    #include "../../drivers/opengles/glad/include/glad/gles2.h"
    #include <dlfcn.h>
#else
    #define LV_SDL_USE_EGL 0

    #if LV_USE_DRAW_NANOVG
        #undef LV_USE_DRAW_NANOVG
        #define LV_USE_DRAW_NANOVG 0
    #endif
#endif

#if LV_COLOR_DEPTH == 1 && LV_SDL_RENDER_MODE != LV_DISPLAY_RENDER_MODE_PARTIAL
    #error SDL LV_COLOR_DEPTH 1 requires LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
#endif

/*********************
 *      DEFINES
 *********************/
#define lv_deinit_in_progress  LV_GLOBAL_DEFAULT()->deinit_in_progress

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    SDL_Window * window;
    SDL_Renderer * renderer;
#if LV_USE_DRAW_SDL == 0
    SDL_Texture * texture;
    uint8_t * fb1;
    uint8_t * fb2;
    uint8_t * fb_act;
    uint8_t * buf1;
    uint8_t * buf2;
    uint8_t * rotated_buf;
    size_t rotated_buf_size;
#endif
    float zoom;
    uint8_t ignore_size_chg;

#if LV_SDL_USE_EGL
    struct {
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;
        EGLConfig config;
        void * gl_lib_handle;
    } egl;
#endif
} lv_sdl_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline int sdl_render_mode(void);
static lv_result_t window_create(lv_display_t * disp);
static void sdl_event_handler(lv_timer_t * t);
static void release_disp_cb(lv_event_t * e);
static void res_chg_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool inited = false;
static lv_timer_t * event_handler_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_sdl_window_create(int32_t hor_res, int32_t ver_res)
{
    if(!inited) {
#if LV_SDL_USE_EGL && defined(SDL_VIDEO_DRIVER_X11)
        SDL_SetHintWithPriority("SDL_VIDEODRIVER", "x11", SDL_HINT_OVERRIDE);
        SDL_SetHint(SDL_HINT_VIDEO_X11_FORCE_EGL, "1");
#endif
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
        lv_tick_set_cb(SDL_GetTicks);
        lv_delay_set_cb(SDL_Delay);

        inited = true;
    }

    lv_sdl_window_t * dsc = lv_malloc_zeroed(sizeof(lv_sdl_window_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;

    lv_display_t * disp = lv_display_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    lv_display_set_driver_data(disp, dsc);
    window_create(disp);

    lv_display_set_flush_cb(disp, flush_cb);

#if LV_USE_DRAW_NANOVG
#if !LV_SDL_USE_EGL
#error "NANOVG requires LV_SDL_USE_EGL"
#endif
#if LV_USE_OPENGLES
    lv_opengles_init();
#endif
    lv_draw_nanovg_init();
#endif

#if LV_USE_DRAW_SDL == 0
    if(sdl_render_mode() == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(lv_display_get_color_format(disp)) * 4;
        uint32_t buffer_size_bytes = 32 * 1024 + palette_size;
        dsc->buf1 = sdl_draw_buf_realloc_aligned(NULL, buffer_size_bytes);
#if LV_SDL_BUF_COUNT == 2
        dsc->buf2 = sdl_draw_buf_realloc_aligned(NULL, buffer_size_bytes);
#endif
        lv_display_set_buffers(disp, dsc->buf1, dsc->buf2, buffer_size_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);
    }

    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    /*Process the initial events*/
    sdl_event_handler(NULL);

    return disp;
}

void lv_sdl_window_set_resizeable(lv_display_t * disp, bool value)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    SDL_SetWindowResizable(dsc->window, value);
}

void lv_sdl_window_set_size(lv_display_t * disp, int32_t hor_res, int32_t ver_res)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    SDL_SetWindowSize(dsc->window, hor_res, ver_res);
}

void lv_sdl_window_set_zoom(lv_display_t * disp, float zoom)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = zoom;
    lv_display_send_event(disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_refr_now(disp);
}

float lv_sdl_window_get_zoom(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    return dsc->zoom;
}

lv_display_t * lv_sdl_get_disp_from_win_id(uint32_t win_id)
{
    lv_display_t * disp = lv_display_get_next(NULL);
    if(win_id == UINT32_MAX) return disp;

    while(disp) {
        lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
        if(dsc != NULL && SDL_GetWindowID(dsc->window) == win_id) {
            return disp;
        }
        disp = lv_display_get_next(disp);
    }
    return NULL;
}

void lv_sdl_window_set_title(lv_display_t * disp, const char * title)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    SDL_SetWindowTitle(dsc->window, title);
}

void lv_sdl_window_set_icon(lv_display_t * disp, void * icon, int32_t width, int32_t height)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    SDL_Surface * iconSurface = SDL_CreateRGBSurfaceWithFormatFrom(icon, width, height, 32, width * 4,
                                                                   SDL_PIXELFORMAT_ARGB8888);
    SDL_SetWindowIcon(dsc->window, iconSurface);
    SDL_FreeSurface(iconSurface);
}

void * lv_sdl_window_get_renderer(lv_display_t * display)
{
    if(!display) {
        return NULL;
    }
    return lv_sdl_backend_ops.get_renderer(display);
}

struct SDL_Window * lv_sdl_window_get_window(lv_display_t * disp)
{
    if(!disp) {
        LV_LOG_ERROR("invalid display pointer");
        return NULL;
    }
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(!dsc) {
        LV_LOG_ERROR("invalid driver data");
        return NULL;
    }
    return dsc->window;
}

void lv_sdl_quit(void)
{
    if(inited) {
        SDL_Quit();
        lv_timer_delete(event_handler_timer);
        event_handler_timer = NULL;
        inited = false;
    }
}

void lv_sdl_backend_set_display_data(lv_display_t * display, void * backend_display_data)
{
    LV_ASSERT_NULL(display);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(display);
    dsc->backend_data = backend_display_data;
}
void * lv_sdl_backend_get_display_data(lv_display_t * display)
{
    LV_ASSERT_NULL(display);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(display);
    return dsc->backend_data;
}

int32_t lv_sdl_window_get_horizontal_resolution(lv_display_t * display)
{
    /* Private function, fine to assert here*/
    LV_ASSERT_NULL(display);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(display);
    LV_ASSERT_NULL(dsc);
    return (int32_t)((float)(display->hor_res) * dsc->zoom);
}
int32_t lv_sdl_window_get_vertical_resolution(lv_display_t * display)
{
    /* Private function, fine to assert here*/
    LV_ASSERT_NULL(display);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(display);
    LV_ASSERT_NULL(dsc);
    return (int32_t)((float)(display->ver_res) * dsc->zoom);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline int sdl_render_mode(void)
{
    return LV_SDL_RENDER_MODE;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#if LV_USE_DRAW_SDL == 0
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    lv_color_format_t cf = lv_display_get_color_format(disp);
    uint32_t * argb_px_map = NULL;

#if LV_SDL_USE_EGL
    /* EGL rendering path */
    if(lv_display_flush_is_last(disp)) {
        eglSwapBuffers(dsc->egl.display, dsc->egl.surface);
    }
    lv_display_flush_ready(disp);
    return;
#endif

    if(sdl_render_mode() == LV_DISPLAY_RENDER_MODE_PARTIAL) {

        if(cf == LV_COLOR_FORMAT_RGB565_SWAPPED) {
            uint32_t width = lv_area_get_width(area);
            uint32_t height = lv_area_get_height(area);
            lv_draw_sw_rgb565_swap(px_map, width * height);
        }
        /*Update values in a special OLED I1 --> ARGB8888 case
          We render everything in I1, but display it in ARGB8888*/
        if(cf == LV_COLOR_FORMAT_I1) {
            /*I1 uses 1 bit wide pixels, ARGB8888 uses 4 byte wide pixels*/
            cf = LV_COLOR_FORMAT_ARGB8888;
            uint32_t width = lv_area_get_width(area);
            uint32_t height = lv_area_get_height(area);
            uint32_t argb_px_map_size = width * height * 4;
            argb_px_map = malloc(argb_px_map_size);
            if(argb_px_map == NULL) {
                LV_LOG_ERROR("malloc failed");
                lv_display_flush_ready(disp);
                return;
            }
            /* skip the palette */
            px_map += LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I1) * 4;
            lv_draw_sw_i1_to_argb8888(px_map, argb_px_map, width, height, width / 8, width * 4, 0xFF000000u, 0xFFFFFFFFu);
            px_map = (uint8_t *)argb_px_map;
        }

        lv_area_t rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);

        int32_t px_map_w = lv_area_get_width(area);
        int32_t px_map_h = lv_area_get_height(area);
        uint32_t px_map_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
        uint32_t px_size = lv_color_format_get_size(cf);

        int32_t fb_stride = lv_draw_buf_width_to_stride(disp->hor_res, cf);
        uint8_t * fb_start = dsc->fb_act;
        fb_start += rotated_area.y1 * fb_stride + rotated_area.x1 * px_size;
        lv_display_rotation_t rotation = lv_display_get_rotation(disp);

        if(rotation == LV_DISPLAY_ROTATION_0) {
            uint32_t px_map_line_bytes = lv_area_get_width(area) * px_size;

            int32_t y;
            for(y = area->y1; y <= area->y2; y++) {
                lv_memcpy(fb_start, px_map, px_map_line_bytes);
                px_map += px_map_stride;
                fb_start += fb_stride;
            }
        }
        else {
            lv_draw_sw_rotate(px_map, fb_start, px_map_w, px_map_h, px_map_stride, fb_stride, rotation, cf);
        }
    }

    if(lv_display_flush_is_last(disp)) {
        if(sdl_render_mode() != LV_DISPLAY_RENDER_MODE_PARTIAL) {
            dsc->fb_act = px_map;
        }

        window_update(disp);
    }
    free(argb_px_map);
#else
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    if(lv_display_flush_is_last(disp)) {
        window_update(disp);
    }
#endif /*LV_USE_DRAW_SDL == 0*/

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_display_flush_ready(disp);
}

/**
 * SDL main thread. All SDL related task have to be handled here!
 * It initializes SDL, handles drawing and the mouse.
 */
static void sdl_event_handler(lv_timer_t * t)
{
    LV_UNUSED(t);

    /*Refresh handling*/
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        lv_sdl_mouse_handler(&event);
#if LV_SDL_MOUSEWHEEL_MODE == LV_SDL_MOUSEWHEEL_MODE_ENCODER
        lv_sdl_mousewheel_handler(&event);
#endif
        lv_sdl_keyboard_handler(&event);

        if(event.type == SDL_WINDOWEVENT) {
            lv_display_t * disp = lv_sdl_get_disp_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
            switch(event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    lv_sdl_backend_ops.redraw(disp);
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    dsc->ignore_size_chg = 1;
                    int32_t hres = (int32_t)((float)(event.window.data1) / dsc->zoom);
                    int32_t vres = (int32_t)((float)(event.window.data2) / dsc->zoom);
                    lv_display_set_resolution(disp, hres, vres);
                    dsc->ignore_size_chg = 0;
                    lv_refr_now(disp);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    lv_display_delete(disp);
                    break;
                default:
                    break;
            }
        }
        if(event.type == SDL_QUIT) {
            SDL_Quit();
            lv_deinit();
            inited = false;
#if LV_SDL_DIRECT_EXIT
            exit(0);
#endif
        }
    }
}

#if LV_SDL_USE_EGL
static void * s_gl_lib_handle = NULL;

static GLADapiproc glad_egl_load_cb(void * userptr, const char * name)
{
    LV_UNUSED(userptr);
    GLADapiproc proc = NULL;

    /* First try eglGetProcAddress for extension functions */
    proc = (GLADapiproc)eglGetProcAddress(name);
    if(proc) return proc;

    /* Fall back to dlsym for core functions */
    if(s_gl_lib_handle) {
        proc = (GLADapiproc)dlsym(s_gl_lib_handle, name);
    }
    return proc;
}

static bool init_egl(lv_sdl_window_t * dsc)
{
    /* Load OpenGL ES library */
    dsc->egl.gl_lib_handle = dlopen("libGLESv2.so", RTLD_LAZY);
    if(!dsc->egl.gl_lib_handle) {
        dsc->egl.gl_lib_handle = dlopen("libGLESv2.so.2", RTLD_LAZY);
    }
    if(!dsc->egl.gl_lib_handle) {
        LV_LOG_ERROR("Failed to load OpenGL ES library: %s", dlerror());
        return false;
    }

    dsc->egl.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(dsc->egl.display == EGL_NO_DISPLAY) {
        LV_LOG_ERROR("Failed to get EGL display");
        return false;
    }

    if(!eglInitialize(dsc->egl.display, NULL, NULL)) {
        LV_LOG_ERROR("Failed to initialize EGL");
        return false;
    }

    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_SAMPLES, 4, /* 4x MSAA */
        EGL_NONE
    };

    EGLint num_configs;
    if(!eglChooseConfig(dsc->egl.display, config_attribs, &dsc->egl.config, 1, &num_configs)) {
        LV_LOG_ERROR("Failed to choose EGL config");
        return false;
    }

    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(dsc->window, &wmInfo);

    EGLNativeWindowType native_window;
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    native_window = wmInfo.info.win.window;
#elif defined(SDL_VIDEO_DRIVER_X11)
    native_window = wmInfo.info.x11.window;
#elif defined(SDL_VIDEO_DRIVER_WAYLAND)
    native_window = wmInfo.info.wl.surface;
#else
    LV_LOG_ERROR("Unsupported platform for EGL");
    return false;
#endif

    dsc->egl.surface = eglCreateWindowSurface(dsc->egl.display, dsc->egl.config, native_window, NULL);
    if(dsc->egl.surface == EGL_NO_SURFACE) {
        LV_LOG_ERROR("Failed to create EGL surface");
        return false;
    }

    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    dsc->egl.context = eglCreateContext(dsc->egl.display, dsc->egl.config, EGL_NO_CONTEXT, context_attribs);
    if(dsc->egl.context == EGL_NO_CONTEXT) {
        LV_LOG_ERROR("Failed to create EGL context");
        return false;
    }

    if(!eglMakeCurrent(dsc->egl.display, dsc->egl.surface, dsc->egl.surface, dsc->egl.context)) {
        LV_LOG_ERROR("Failed to make EGL context current");
        return false;
    }

    /* Load GLES2 functions via GLAD */
    s_gl_lib_handle = dsc->egl.gl_lib_handle;
    if(!gladLoadGLES2UserPtr(glad_egl_load_cb, NULL)) {
        LV_LOG_ERROR("Failed to load OpenGL ES2 functions via GLAD");
        return false;
    }

    LV_LOG_INFO("OpenGL ES version: %s", glGetString(GL_VERSION));
    LV_LOG_INFO("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    LV_LOG_INFO("GL extensions: %s", glGetString(GL_EXTENSIONS));

    return true;
}

static void deinit_egl(lv_sdl_window_t * dsc)
{
    if(dsc->egl.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(dsc->egl.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(dsc->egl.context != EGL_NO_CONTEXT) {
            eglDestroyContext(dsc->egl.display, dsc->egl.context);
        }
        if(dsc->egl.surface != EGL_NO_SURFACE) {
            eglDestroySurface(dsc->egl.display, dsc->egl.surface);
        }
        eglTerminate(dsc->egl.display);
    }
    if(dsc->egl.gl_lib_handle) {
        dlclose(dsc->egl.gl_lib_handle);
        dsc->egl.gl_lib_handle = NULL;
    }
    dsc->egl.display = EGL_NO_DISPLAY;
    dsc->egl.context = EGL_NO_CONTEXT;
    dsc->egl.surface = EGL_NO_SURFACE;
}
#endif

static void window_create(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = 1.0;

    int flag = 0;
#if LV_SDL_USE_EGL
    flag |= SDL_WINDOW_OPENGL;
#endif

#if LV_SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

#if LV_SDL_USE_EGL
    flag |= SDL_WINDOW_OPENGL;
#endif

    int32_t hor_res = (int32_t)((float)(disp->hor_res) * dsc->zoom);
    int32_t ver_res = (int32_t)((float)(disp->ver_res) * dsc->zoom);
    dsc->window = SDL_CreateWindow("LVGL Simulator",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   hor_res, ver_res, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/
    if(!dsc->window) {
        LV_LOG_ERROR("Failed to create SDL window");
        return LV_RESULT_INVALID;
    }
    if(lv_sdl_backend_ops.init_display(disp) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize SDL backend");
        SDL_DestroyWindow(dsc->window);
        return LV_RESULT_INVALID;
    }

#if LV_SDL_USE_EGL
    if(!init_egl(dsc)) {
        LV_LOG_ERROR("Failed to initialize EGL, falling back to SDL renderer");
    }
#else
    dsc->renderer = SDL_CreateRenderer(dsc->window, -1,
                                       LV_SDL_ACCELERATED ? SDL_RENDERER_ACCELERATED : SDL_RENDERER_SOFTWARE);
#endif /*LV_SDL_USE_EGL*/

#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    lv_memset(dsc->fb1, 0xff, hor_res * ver_res * px_size);
    if(dsc->fb2) lv_memset(dsc->fb2, 0xff, hor_res * ver_res * px_size);

#endif /*LV_USE_DRAW_SDL == 0*/
    /*Some platforms (e.g. Emscripten) seem to require setting the size again */
    SDL_SetWindowSize(dsc->window, hor_res, ver_res);
    return LV_RESULT_OK;
}

static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);
    if(lv_sdl_backend_ops.resize_display(disp) != LV_RESULT_OK) {
        LV_LOG_WARN("Failed to resize display");
        return;
    }

    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(!dsc->ignore_size_chg) {
        SDL_SetWindowSize(dsc->window,
                          lv_sdl_window_get_horizontal_resolution(disp),
                          lv_sdl_window_get_vertical_resolution(disp));
    }
}

static void release_disp_cb(lv_event_t * e)
{
    if(lv_deinit_in_progress) {
        lv_sdl_quit();
    }
    lv_display_t * disp = (lv_display_t *) lv_event_get_user_data(e);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
#if LV_USE_DRAW_SDL == 0
    SDL_DestroyTexture(dsc->texture);
#endif
#if LV_SDL_USE_EGL
    deinit_egl(dsc);
#endif
    if(dsc->renderer) {
        SDL_DestroyRenderer(dsc->renderer);
    }
    SDL_DestroyWindow(dsc->window);
    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

#endif /*LV_USE_SDL*/

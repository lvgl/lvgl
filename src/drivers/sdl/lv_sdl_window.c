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

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include "lv_sdl_private.h"

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
    lv_result_t res = window_create(disp);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to initialize window");
        lv_free(dsc);
        lv_display_delete(disp);
        return NULL;
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

static lv_result_t window_create(lv_display_t * disp)
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

    lv_sdl_backend_ops.deinit_display(disp);
    SDL_DestroyWindow(dsc->window);
    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

#endif /*LV_USE_SDL*/

/**
 * @file lv_sdl_window.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL
#include <stdbool.h>
#include "../../core/lv_refr.h"
#include "../../stdlib/lv_string.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include LV_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    uint8_t * fb;
    uint8_t zoom;
    uint8_t ignore_size_chg;
} lv_sdl_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * color_p);
static void window_create(lv_disp_t * disp);
static void window_update(lv_disp_t * disp);
static void clean_up(lv_disp_t * disp);
static void texture_resize(lv_disp_t * disp);
static void sdl_event_handler(lv_timer_t * t);
static void release_disp_cb(lv_event_t * e);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);
void _lv_sdl_keyboard_handler(SDL_Event * event);
static void res_chg_event_cb(lv_event_t * e);

#if !LV_TICK_CUSTOM
    static int tick_thread(void * ptr);
#endif
static bool inited = false;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_timer_t * event_handler_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_disp_t * lv_sdl_window_create(lv_coord_t hor_res, lv_coord_t ver_res)
{
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
#if !LV_TICK_CUSTOM
        SDL_CreateThread(tick_thread, "LVGL thread", NULL);
#endif
        inited = true;
    }

    lv_sdl_window_t * dsc = lv_malloc(sizeof(lv_sdl_window_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) return NULL;
    lv_memzero(dsc, sizeof(lv_sdl_window_t));

    lv_disp_t * disp = lv_disp_create(hor_res, ver_res);
    if(disp == NULL) {
        lv_free(dsc);
        return NULL;
    }
    lv_disp_add_event(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_disp_set_driver_data(disp, dsc);
    window_create(disp);

    lv_disp_set_flush_cb(disp, flush_cb);
#if LV_SDL_PARTIAL_MODE
    uint8_t * buf = malloc(32 * 1024);
    lv_disp_set_draw_buffers(disp, buf, NULL,
                             32 * 1024, LV_DISP_RENDER_MODE_PARTIAL);
#else
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    lv_disp_set_draw_buffers(disp, dsc->fb, NULL,
                             lv_disp_get_hor_res(disp) * lv_disp_get_hor_res(disp) * px_size, LV_DISP_RENDER_MODE_DIRECT);
#endif
    lv_disp_add_event(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    return disp;
}

void lv_sdl_window_set_zoom(lv_disp_t * disp, uint8_t zoom)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    dsc->zoom = zoom;
    lv_disp_send_event(disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_refr_now(disp);
}

uint8_t lv_sdl_window_get_zoom(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    return dsc->zoom;
}

lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    if(win_id == UINT32_MAX) return disp;

    while(disp) {
        lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
        if(SDL_GetWindowID(dsc->window) == win_id) {
            return disp;
        }
        disp = lv_disp_get_next(disp);
    }
    return NULL;
}

void lv_sdl_window_set_title(lv_disp_t * disp, const char * title)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    SDL_SetWindowTitle(dsc->window, title);
}

void lv_sdl_quit()
{
    if(inited) {
        SDL_Quit();
        lv_timer_del(event_handler_timer);
        inited = false;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void flush_cb(lv_disp_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#if LV_SDL_PARTIAL_MODE
    int32_t y;
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    uint8_t * fb_tmp = dsc->fb;
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    uint32_t px_map_stride = lv_area_get_width(area) * px_size;
    lv_coord_t fb_stride = lv_disp_get_hor_res(disp) * px_size;
    fb_tmp += area->y1 * fb_stride;
    fb_tmp += area->x1 * px_size;
    for(y = area->y1; y <= area->y2; y++) {
        lv_memcpy(fb_tmp, px_map, px_map_stride);
        px_map += px_map_stride;
        fb_tmp += fb_stride;
    }
#else
    LV_UNUSED(area);
    LV_UNUSED(px_map);
#endif


    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp)) {
        window_update(disp);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp);
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
        _lv_sdl_mouse_handler(&event);
        _lv_sdl_mousewheel_handler(&event);
        _lv_sdl_keyboard_handler(&event);

        if(event.type == SDL_WINDOWEVENT) {
            lv_disp_t * disp = _lv_sdl_get_disp_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);

            switch(event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(disp);
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    dsc->ignore_size_chg = 1;
                    lv_disp_set_res(disp, event.window.data1 / dsc->zoom, event.window.data2 / dsc->zoom);
                    dsc->ignore_size_chg = 0;
                    lv_refr_now(disp);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    clean_up(disp);
                    break;
                default:
                    break;
            }
        }
        if(event.type == SDL_QUIT) {
            SDL_Quit();
            lv_timer_del(event_handler_timer);
            inited = false;
#if LV_SDL_DIRECT_EXIT
            exit(0);
#endif
        }
    }
}

static void clean_up(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    SDL_DestroyTexture(dsc->texture);
    SDL_DestroyRenderer(dsc->renderer);
    SDL_DestroyWindow(dsc->window);

    lv_free(dsc);
}

static void window_create(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    dsc->zoom = 1;

    int flag = SDL_WINDOW_RESIZABLE;
#if LV_SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);
    dsc->window = SDL_CreateWindow("LVGL Simulator",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   hor_res * dsc->zoom, ver_res * dsc->zoom, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    dsc->renderer = SDL_CreateRenderer(dsc->window, -1, SDL_RENDERER_SOFTWARE);
    texture_resize(disp);
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    lv_memset(dsc->fb, 0xff, hor_res * ver_res * px_size);
    /*Some platforms (e.g. Emscripten) seem to require setting the size again */
    SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
    texture_resize(disp);
}

static void window_update(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    SDL_UpdateTexture(dsc->texture, NULL, dsc->fb, hor_res * px_size);

    SDL_RenderClear(dsc->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(dsc->renderer, dsc->texture, NULL, NULL);
    SDL_RenderPresent(dsc->renderer);
}

static void texture_resize(lv_disp_t * disp)
{
    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);
    uint32_t px_size = lv_color_format_get_size(lv_disp_get_color_format(disp));
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);

    dsc->fb = realloc(dsc->fb, hor_res * ver_res * px_size);
    memset(dsc->fb, 0x00, hor_res * ver_res * px_size);

#if LV_SDL_PARTIAL_MODE == 0
    lv_disp_set_draw_buffers(disp, dsc->fb, NULL, hor_res * ver_res * px_size, LV_DISP_RENDER_MODE_DIRECT);
#endif
    if(dsc->texture) SDL_DestroyTexture(dsc->texture);

#if LV_COLOR_DEPTH == 32
    SDL_PixelFormatEnum px_format =
        SDL_PIXELFORMAT_RGB888; /*same as SDL_PIXELFORMAT_RGB888, but it's not supported in older versions*/
#elif LV_COLOR_DEPTH == 24
    SDL_PixelFormatEnum px_format = SDL_PIXELFORMAT_BGR24;
#elif LV_COLOR_DEPTH == 16
    SDL_PixelFormatEnum px_format = SDL_PIXELFORMAT_RGB565;
#else
#error("Unsupported color format")
#endif

    dsc->texture = SDL_CreateTexture(dsc->renderer, px_format,
                                     SDL_TEXTUREACCESS_STATIC, hor_res, ver_res);
    SDL_SetTextureBlendMode(dsc->texture, SDL_BLENDMODE_BLEND);
}

static void res_chg_event_cb(lv_event_t * e)
{
    lv_disp_t * disp = lv_event_get_target(e);

    int32_t hor_res = lv_disp_get_hor_res(disp);
    int32_t ver_res = lv_disp_get_ver_res(disp);
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    if(dsc->ignore_size_chg == false) {
        SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
    }

    texture_resize(disp);
}

#if !LV_TICK_CUSTOM
static int tick_thread(void * ptr)
{
    LV_UNUSED(ptr);
    static uint32_t tick_prev = 0;

    while(1) {
        uint32_t tick_now = SDL_GetTicks();
        lv_tick_inc(tick_now - tick_prev);
        tick_prev = tick_now;
        SDL_Delay(5);
    }

    return 0;
}
#endif

static void release_disp_cb(lv_event_t * e)
{
    lv_disp_t * disp = (lv_disp_t *) lv_event_get_user_data(e);
    clean_up(disp);
}

#endif /*LV_USE_SDL*/

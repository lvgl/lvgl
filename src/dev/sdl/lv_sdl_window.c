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
    lv_color_t * fb;
    uint8_t zoom;
} lv_sdl_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p);
static void window_create(lv_disp_t * disp);
static void window_update(lv_disp_t * disp);
static void clean_up(lv_disp_t * disp);
static void texture_resize(lv_disp_t * disp);
static void sdl_event_handler(lv_timer_t * t);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);
void _lv_sdl_keyboard_handler(SDL_Event * event);
static void res_chg_event_cb(lv_event_t * e);

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
    static bool inited = false;
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
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
    lv_disp_set_driver_data(disp, dsc);
    window_create(disp);

    lv_disp_set_flush_cb(disp, flush_cb);
    lv_disp_set_draw_buffers(disp, dsc->fb, NULL,
                             lv_disp_get_hor_res(disp) * lv_disp_get_hor_res(disp), LV_DISP_RENDER_MODE_DIRECT);
    lv_disp_add_event(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    return disp;
}

void lv_sdl_window_set_zoom(lv_disp_t * disp, uint8_t zoom)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    dsc->zoom = zoom;
    texture_resize(disp);
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

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

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
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    lv_disp_set_res(disp, event.window.data1 / dsc->zoom, event.window.data2 / dsc->zoom);
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
            exit(0);
        }
    }
}

static void clean_up(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    SDL_DestroyTexture(dsc->texture);
    SDL_DestroyRenderer(dsc->renderer);
    SDL_DestroyWindow(dsc->window);
}

static void window_create(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    dsc->zoom = 1;

    int flag = SDL_WINDOW_RESIZABLE;
#if SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);
    dsc->window = SDL_CreateWindow("TFT Simulator",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   hor_res * dsc->zoom, ver_res * dsc->zoom, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    dsc->renderer = SDL_CreateRenderer(dsc->window, -1, SDL_RENDERER_SOFTWARE);
    texture_resize(disp);
    lv_memset(dsc->fb, 0xff, hor_res * ver_res * sizeof(lv_color_t));
}

static void window_update(lv_disp_t * disp)
{
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);
    lv_coord_t hor_res = lv_disp_get_hor_res(disp);

    SDL_UpdateTexture(dsc->texture, NULL, dsc->fb, hor_res * sizeof(lv_color_t));
    SDL_RenderClear(dsc->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(dsc->renderer, dsc->texture, NULL, NULL);
    SDL_RenderPresent(dsc->renderer);
}

static void texture_resize(lv_disp_t * disp)
{
    lv_coord_t hor_res = lv_disp_get_hor_res(disp);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);
    lv_sdl_window_t * dsc = lv_disp_get_driver_data(disp);

    dsc->fb = (lv_color_t *)realloc(dsc->fb, sizeof(lv_color_t) * hor_res * ver_res);
    lv_disp_set_draw_buffers(disp, dsc->fb, NULL, hor_res * ver_res * sizeof(lv_color_t), LV_DISP_RENDER_MODE_DIRECT);

    SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
    if(dsc->texture) SDL_DestroyTexture(dsc->texture);

#if LV_COLOR_DEPTH == 32
    SDL_PixelFormatEnum px_format = SDL_PIXELFORMAT_ARGB8888;
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
    texture_resize(disp);
}

#endif /*LV_USE_SDL*/

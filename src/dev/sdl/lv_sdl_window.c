/**
 * @file lv_sdl_window.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#include <stdbool.h>
#if LV_USE_SDL

#include LV_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_sdl_disp_priv_t {
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    lv_color_t * fb;
} _lv_sdl_disp_priv_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p);
static void window_create(lv_sdl_window_t * drv);
static void window_update(lv_sdl_window_t * drv);
static void monitor_sdl_clean_up(lv_sdl_window_t * dev);
static void sdl_event_handler(lv_timer_t * t);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);
void _lv_sdl_keyboard_handler(SDL_Event * event);

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

lv_sdl_window_t * lv_sdl_window_create(void)
{
    static bool inited = false;
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
        inited = true;
    }

    lv_sdl_window_t * cfg = lv_malloc(sizeof(lv_sdl_window_t));
    LV_ASSERT_MALLOC(cfg);
    if(cfg == NULL) return NULL;
    lv_memzero(cfg, sizeof(lv_sdl_window_t));

    cfg->_priv = lv_malloc(sizeof(_lv_sdl_disp_priv_t));
    LV_ASSERT_MALLOC(cfg->_priv);

    if(cfg->_priv == NULL) {
        lv_free(cfg);
        return NULL;
    }

    lv_memzero(cfg->_priv, sizeof(_lv_sdl_disp_priv_t));

    cfg->hor_res = 800;
    cfg->ver_res = 480;
    cfg->zoom = 1;


    return cfg;
}

lv_disp_t * lv_sdl_window_register(lv_sdl_window_t * cfg)
{
    lv_disp_t * disp = lv_disp_create();
    if(disp == NULL) return NULL;

    window_create(cfg);

    disp->hor_res = cfg->hor_res;
    disp->ver_res = cfg->ver_res;
    disp->buf1 = cfg->_priv->fb;
    disp->buf_act = disp->buf1;
    disp->draw_buf_size = cfg->hor_res * cfg->ver_res;
    disp->direct_mode = 1;
    disp->user_data = cfg;
    disp->flush_cb = flush_cb;

    return disp;
}


lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_sdl_window_t * cfg = disp->user_data;
        if(SDL_GetWindowID(cfg->_priv->window) == win_id) {
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
        window_update((lv_sdl_window_t *)disp->user_data);
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

        lv_disp_t * disp;

        lv_sdl_window_t * dev;
        if(event.type == SDL_WINDOWEVENT) {
            disp = _lv_sdl_get_disp_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            dev = disp->user_data;
            switch(event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(dev);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    monitor_sdl_clean_up(dev);
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

static void monitor_sdl_clean_up(lv_sdl_window_t * dev)
{
    SDL_DestroyTexture(dev->_priv->texture);
    SDL_DestroyRenderer(dev->_priv->renderer);
    SDL_DestroyWindow(dev->_priv->window);
}

static void window_create(lv_sdl_window_t * drv)
{
    int flag = 0;
#if SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

#if LV_COLOR_DEPTH == 32
    SDL_PixelFormatEnum px_format = SDL_PIXELFORMAT_ARGB8888;
#elif LV_COLOR_DEPTH == 16
    SDL_PixelFormatEnum px_format = SDL_PIXELFORMAT_RGB565;
#else
#error("Unsupported color format")
#endif


    drv->_priv->window = SDL_CreateWindow("TFT Simulator",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          drv->hor_res * drv->zoom, drv->ver_res * drv->zoom, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    drv->_priv->renderer = SDL_CreateRenderer(drv->_priv->window, -1, SDL_RENDERER_SOFTWARE);
    drv->_priv->texture = SDL_CreateTexture(drv->_priv->renderer, px_format,
                                            SDL_TEXTUREACCESS_STATIC, drv->hor_res, drv->ver_res);
    SDL_SetTextureBlendMode(drv->_priv->texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    drv->_priv->fb = (lv_color_t *)malloc(sizeof(lv_color_t) * drv->hor_res * drv->ver_res);
    lv_memset(drv->_priv->fb, 0xff, drv->hor_res * drv->ver_res * sizeof(lv_color_t));
}

static void window_update(lv_sdl_window_t * drv)
{
    SDL_UpdateTexture(drv->_priv->texture, NULL, drv->_priv->fb, drv->hor_res * sizeof(lv_color_t));
    SDL_RenderClear(drv->_priv->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(drv->_priv->renderer, drv->_priv->texture, NULL, NULL);
    SDL_RenderPresent(drv->_priv->renderer);
}

#endif /*LV_USE_SDL*/

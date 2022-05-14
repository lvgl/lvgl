/**
 * @file sdl.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_sdl_window.h"
#if LV_USE_DRV_SDL

#include LV_PLATFORM_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_drv_sdl_disp_priv_t {
    SDL_Window * window;
    SDL_Renderer * renderer;
    SDL_Texture * texture;
    lv_color_t * fb;
} _lv_drv_sdl_disp_priv_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t send_image(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, void * color_p);
static void window_create(lv_drv_sdl_window_t * drv);
static void window_update(lv_drv_sdl_window_t * drv);
static void monitor_sdl_clean_up(lv_drv_sdl_window_t * dev);
static void sdl_event_handler(lv_timer_t * t);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_disp_t * lv_drv_sdl_get_disp_from_win_id(uint32_t win_id);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);
void _lv_sdl_keyboard_handler(SDL_Event * event);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_timer_t * event_handler_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_drv_sdl_window_init(lv_drv_sdl_window_t * drv)
{
    static bool inited = false;
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
        inited = true;
    }

    lv_memset_00(drv, sizeof(lv_drv_sdl_window_t));
    drv->hor_res = 800;
    drv->ver_res = 480;
    drv->zoom = 1;
}

lv_disp_drv_t * lv_drv_sdl_window_create(lv_drv_sdl_window_t * drv)
{
    drv->_priv = lv_mem_alloc(sizeof(_lv_drv_sdl_disp_priv_t));
    LV_ASSERT_MALLOC(drv->_priv);

    lv_disp_drv_t * disp_drv = lv_mem_alloc(sizeof(lv_disp_drv_t));
    LV_ASSERT_MALLOC(disp_drv);

    lv_disp_draw_buf_t * draw_buf = lv_mem_alloc(sizeof(lv_disp_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);

    if(drv->_priv == NULL || disp_drv == NULL || draw_buf == NULL) {
        lv_mem_free(drv->_priv);
        lv_mem_free(disp_drv);
        lv_mem_free(draw_buf);
        return NULL;
    }

    lv_memset_00(drv->_priv, sizeof(_lv_drv_sdl_disp_priv_t));

    window_create(drv);

    lv_disp_draw_buf_init(draw_buf, drv->_priv->fb, NULL, drv->hor_res * drv->ver_res);
    disp_drv->draw_buf = draw_buf;
    disp_drv->direct_mode = 1;
    disp_drv->user_data = drv;

    drv->base.send_image_cb = send_image;

    return disp_drv;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static lv_res_t send_image(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, void * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp_drv)) {
        window_update((lv_drv_sdl_window_t *)drv);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);
    return LV_RES_OK;
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

        lv_drv_sdl_window_t * dev;
        if(event.type == SDL_WINDOWEVENT) {
            disp = lv_drv_sdl_get_disp_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            dev = disp->driver->user_data;
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

static void monitor_sdl_clean_up(lv_drv_sdl_window_t * dev)
{
    SDL_DestroyTexture(dev->_priv->texture);
    SDL_DestroyRenderer(dev->_priv->renderer);
    SDL_DestroyWindow(dev->_priv->window);
}

static void window_create(lv_drv_sdl_window_t * drv)
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
    lv_memset_ff(drv->_priv->fb, drv->hor_res * drv->ver_res * sizeof(lv_color_t));
}

static void window_update(lv_drv_sdl_window_t * drv)
{
    SDL_UpdateTexture(drv->_priv->texture, NULL, drv->_priv->fb, drv->hor_res * sizeof(lv_color_t));
    SDL_RenderClear(drv->_priv->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(drv->_priv->renderer, drv->_priv->texture, NULL, NULL);
    SDL_RenderPresent(drv->_priv->renderer);
}

lv_disp_t * lv_drv_sdl_get_disp_from_win_id(uint32_t win_id)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_drv_sdl_window_t * drv = disp->driver->user_data;
        if(SDL_GetWindowID(drv->_priv->window) == win_id) {
            return disp;
        }
        disp = lv_disp_get_next(disp);
    }
    return NULL;
}

#endif /*LV_DRV_SDL_DISP*/

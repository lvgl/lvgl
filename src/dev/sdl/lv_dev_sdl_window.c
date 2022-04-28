/**
 * @file sdl.h
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <lvgl/src/dev/sdl/lv_dev_sdl_mouse.h>
#include <lvgl/src/dev/sdl/lv_dev_sdl_window.h>
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
    lv_color_t * tft_fb;
} _lv_drv_sdl_disp_priv_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void window_create(lv_dev_sdl_window_dsc_t * dsc);
static void window_update(lv_dev_sdl_window_dsc_t * dsc);
static void monitor_sdl_clean_up(lv_dev_sdl_window_dsc_t * dsc);
static void sdl_event_handler(lv_timer_t * t);
lv_disp_t * lv_dev_sdl_get_from_win_id(uint32_t win_id);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
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

void lv_dev_sdl_window_init(lv_dev_sdl_window_dsc_t * dsc)
{
    static bool inited = false;
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
        inited = true;
    }

    lv_memset_00(dsc, sizeof(lv_dev_sdl_window_dsc_t));
    dsc->hor_res = 800;
    dsc->ver_res = 480;
    dsc->zoom = 1;
}

lv_disp_t * lv_dev_sdl_window_create(lv_dev_sdl_window_dsc_t * dsc)
{
    dsc->_priv = lv_mem_alloc(sizeof(_lv_drv_sdl_disp_priv_t));
    LV_ASSERT_MALLOC(dsc->_priv);

    lv_disp_drv_t * disp_drv = lv_mem_alloc(sizeof(lv_disp_drv_t));
    LV_ASSERT_MALLOC(disp_drv);

    lv_disp_draw_buf_t * draw_buf = lv_mem_alloc(sizeof(lv_disp_draw_buf_t));
    LV_ASSERT_MALLOC(draw_buf);

    if(dsc->_priv == NULL || disp_drv == NULL || draw_buf == NULL) {
        lv_mem_free(dsc->_priv);
        lv_mem_free(disp_drv);
        lv_mem_free(draw_buf);
        return NULL;
    }

    lv_memset_00(dsc->_priv, sizeof(_lv_drv_sdl_disp_priv_t));

    window_create(dsc);

    lv_disp_draw_buf_init(draw_buf, dsc->_priv->tft_fb, NULL, dsc->hor_res * dsc->ver_res);

    lv_disp_drv_init(disp_drv);
    disp_drv->draw_buf = draw_buf;
    disp_drv->hor_res = dsc->hor_res;
    disp_drv->ver_res = dsc->ver_res;
    disp_drv->flush_cb = flush_cb;
    disp_drv->direct_mode = 1;
    disp_drv->user_data = dsc;
    lv_disp_t * disp = lv_disp_drv_register(disp_drv);

    return disp;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    LV_UNUSED(area);
    LV_UNUSED(color_p);

    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_disp_flush_is_last(disp_drv)) {
        lv_dev_sdl_window_dsc_t * dsc = disp_drv->user_data;
        window_update(dsc);
    }

    /*IMPORTANT! It must be called to tell the system the flush is ready*/
    lv_disp_flush_ready(disp_drv);
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

        lv_dev_sdl_window_dsc_t * dsc;
        if(event.type == SDL_WINDOWEVENT) {
            disp = lv_dev_sdl_get_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            dsc = disp->driver->user_data;
            switch(event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(dsc);
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    monitor_sdl_clean_up(dsc);
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

static void monitor_sdl_clean_up(lv_dev_sdl_window_dsc_t * dsc)
{
    SDL_DestroyTexture(dsc->_priv->texture);
    SDL_DestroyRenderer(dsc->_priv->renderer);
    SDL_DestroyWindow(dsc->_priv->window);
}

static void window_create(lv_dev_sdl_window_dsc_t * dsc)
{
    int flag = 0;
#if SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    dsc->_priv->window = SDL_CreateWindow("TFT Simulator",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          dsc->hor_res * dsc->zoom, dsc->ver_res * dsc->zoom, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    dsc->_priv->renderer = SDL_CreateRenderer(dsc->_priv->window, -1, SDL_RENDERER_SOFTWARE);
    dsc->_priv->texture = SDL_CreateTexture(dsc->_priv->renderer,
                                            SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, dsc->hor_res, dsc->ver_res);
    SDL_SetTextureBlendMode(dsc->_priv->texture, SDL_BLENDMODE_BLEND);

    /*Initialize the frame buffer to gray (77 is an empirical value) */
    dsc->_priv->tft_fb = (lv_color_t *)malloc(sizeof(lv_color_t) * dsc->hor_res * dsc->ver_res);
    lv_memset_ff(dsc->_priv->tft_fb, dsc->hor_res * dsc->ver_res * sizeof(lv_color_t));
}

static void window_update(lv_dev_sdl_window_dsc_t * dsc)
{
    SDL_UpdateTexture(dsc->_priv->texture, NULL, dsc->_priv->tft_fb, dsc->hor_res * sizeof(lv_color_t));
    SDL_RenderClear(dsc->_priv->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(dsc->_priv->renderer, dsc->_priv->texture, NULL, NULL);
    SDL_RenderPresent(dsc->_priv->renderer);
}

lv_disp_t * lv_dev_sdl_get_from_win_id(uint32_t win_id)
{
    lv_disp_t * disp = lv_disp_get_next(NULL);
    while(disp) {
        lv_dev_sdl_window_dsc_t * dsc = disp->driver->user_data;
        if(SDL_GetWindowID(dsc->_priv->window) == win_id) {
            return disp;
        }
        disp = lv_disp_get_next(disp);
    }
    return NULL;
}

#endif /*LV_DRV_SDL_DISP*/

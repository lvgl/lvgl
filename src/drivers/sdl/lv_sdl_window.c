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
#include "../../core/lv_global.h"
#include "../../lv_init.h"

#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include LV_SDL_INCLUDE_PATH

#if LV_USE_DRAW_SDL
    #include <SDL2/SDL_image.h>
#endif

/*********************
 *      DEFINES
 *********************/

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
#endif
    uint8_t zoom;
    uint8_t ignore_size_chg;
} lv_sdl_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static void window_create(lv_display_t * disp);
static void window_update(lv_display_t * disp);
#if LV_USE_DRAW_SDL == 0
    static void texture_resize(lv_display_t * disp);
#endif
static void sdl_event_handler(lv_timer_t * t);
static void release_disp_cb(lv_event_t * e);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/
lv_display_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);
void _lv_sdl_keyboard_handler(SDL_Event * event);
static void res_chg_event_cb(lv_event_t * e);

static bool inited = false;

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_timer_t * event_handler_timer;

#define lv_deinit_in_progress  LV_GLOBAL_DEFAULT()->deinit_in_progress

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_sdl_window_create(int32_t hor_res, int32_t ver_res)
{
    if(!inited) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_StartTextInput();
        event_handler_timer = lv_timer_create(sdl_event_handler, 5, NULL);
        lv_tick_set_cb(SDL_GetTicks);

#if LV_USE_DRAW_SDL
        if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
            fprintf(stderr, "could not initialize sdl2_image: %s\n", IMG_GetError());
            return NULL;
        }
#endif

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
    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_display_set_driver_data(disp, dsc);
    window_create(disp);

    lv_display_set_flush_cb(disp, flush_cb);

#if LV_USE_DRAW_SDL == 0
    if(LV_SDL_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        dsc->buf1 = malloc(32 * 1024);
#if LV_SDL_BUF_COUNT == 2
        dsc->buf2 = malloc(32 * 1024);
#endif
        lv_display_set_buffers(disp, dsc->buf1, dsc->buf2,
                               32 * 1024, LV_DISPLAY_RENDER_MODE_PARTIAL);
    }
    /*LV_DISPLAY_RENDER_MODE_DIRECT or FULL */
    else {
        uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp),
                                                      lv_display_get_color_format(disp));
        lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * lv_display_get_vertical_resolution(disp),
                               LV_SDL_RENDER_MODE);
    }
#else /*/*LV_USE_DRAW_SDL == 1*/
    uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp),
                                                  lv_display_get_color_format(disp));
    /*It will render directly to default Texture, so the buffer is not used, so just set something*/
    static uint8_t dummy_buf[1];
    lv_display_set_buffers(disp, dummy_buf, NULL, stride * lv_display_get_vertical_resolution(disp),
                           LV_SDL_RENDER_MODE);
#endif /*LV_USE_DRAW_SDL == 0*/
    lv_display_add_event_cb(disp, res_chg_event_cb, LV_EVENT_RESOLUTION_CHANGED, NULL);

    return disp;
}

void lv_sdl_window_set_resizeable(lv_display_t * disp, bool value)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    SDL_SetWindowResizable(dsc->window, value);
}

void lv_sdl_window_set_zoom(lv_display_t * disp, uint8_t zoom)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = zoom;
    lv_display_send_event(disp, LV_EVENT_RESOLUTION_CHANGED, NULL);
    lv_refr_now(disp);
}

uint8_t lv_sdl_window_get_zoom(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    return dsc->zoom;
}

lv_display_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id)
{
    lv_display_t * disp = lv_display_get_next(NULL);
    if(win_id == UINT32_MAX) return disp;

    while(disp) {
        lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
        if(SDL_GetWindowID(dsc->window) == win_id) {
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

void * lv_sdl_window_get_renderer(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    return dsc->renderer;
}

void lv_sdl_quit()
{
    if(inited) {
        SDL_Quit();
        lv_timer_delete(event_handler_timer);
        event_handler_timer = NULL;
        inited = false;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
#if LV_USE_DRAW_SDL == 0
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(LV_SDL_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        int32_t y;
        uint8_t * fb_tmp = dsc->fb_act;
        uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
        uint32_t px_map_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), lv_display_get_color_format(disp));
        uint32_t data_size = lv_area_get_width(area) * px_size;
        int32_t fb_stride = lv_display_get_horizontal_resolution(disp) * px_size;
        fb_tmp += area->y1 * fb_stride;
        fb_tmp += area->x1 * px_size;
        for(y = area->y1; y <= area->y2; y++) {
            lv_memcpy(fb_tmp, px_map, data_size);
            px_map += px_map_stride;
            fb_tmp += fb_stride;
        }
    }
    /* TYPICALLY YOU DO NOT NEED THIS
     * If it was the last part to refresh update the texture of the window.*/
    if(lv_display_flush_is_last(disp)) {
        if(LV_SDL_RENDER_MODE != LV_DISPLAY_RENDER_MODE_PARTIAL) {
            dsc->fb_act = px_map;
        }
        window_update(disp);
    }
#else
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
        _lv_sdl_mouse_handler(&event);
#if LV_SDL_MOUSEWHEEL_MODE == LV_SDL_MOUSEWHEEL_MODE_ENCODER
        _lv_sdl_mousewheel_handler(&event);
#endif
        _lv_sdl_keyboard_handler(&event);

        if(event.type == SDL_WINDOWEVENT) {
            lv_display_t * disp = _lv_sdl_get_disp_from_win_id(event.window.windowID);
            if(disp == NULL) continue;
            lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);

            switch(event.window.event) {
#if SDL_VERSION_ATLEAST(2, 0, 5)
                case SDL_WINDOWEVENT_TAKE_FOCUS:
#endif
                case SDL_WINDOWEVENT_EXPOSED:
                    window_update(disp);
                    break;
                case SDL_WINDOWEVENT_RESIZED:
                    dsc->ignore_size_chg = 1;
                    lv_display_set_resolution(disp, event.window.data1 / dsc->zoom, event.window.data2 / dsc->zoom);
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

static void window_create(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = 1;

    int flag = SDL_WINDOW_RESIZABLE;
#if LV_SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    dsc->window = SDL_CreateWindow("LVGL Simulator",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   hor_res * dsc->zoom, ver_res * dsc->zoom, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    dsc->renderer = SDL_CreateRenderer(dsc->window, -1, SDL_RENDERER_SOFTWARE);
#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    lv_memset(dsc->fb1, 0xff, hor_res * ver_res * px_size);
#if LV_SDL_BUF_COUNT == 2
    lv_memset(dsc->fb2, 0xff, hor_res * ver_res * px_size);
#endif
#endif /*LV_USE_DRAW_SDL == 0*/
    /*Some platforms (e.g. Emscripten) seem to require setting the size again */
    SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);
#endif /*LV_USE_DRAW_SDL == 0*/
}

static void window_update(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
#if LV_USE_DRAW_SDL == 0
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(hor_res, lv_display_get_color_format(disp));
    SDL_UpdateTexture(dsc->texture, NULL, dsc->fb_act, stride);

    SDL_RenderClear(dsc->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(dsc->renderer, dsc->texture, NULL, NULL);
#endif
    SDL_RenderPresent(dsc->renderer);
}

#if LV_USE_DRAW_SDL == 0
static void texture_resize(lv_display_t * disp)
{
    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    uint32_t stride = lv_draw_buf_width_to_stride(hor_res, lv_display_get_color_format(disp));
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);

    dsc->fb1 = realloc(dsc->fb1, stride * ver_res);
    memset(dsc->fb1, 0x00, stride * ver_res);

    if(LV_SDL_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        dsc->fb_act = dsc->fb1;
    }
    else {
#if LV_SDL_BUF_COUNT == 2
        dsc->fb2 = realloc(dsc->fb2, stride * ver_res);
        memset(dsc->fb2, 0x00, stride * ver_res);
#endif
        lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * ver_res, LV_SDL_RENDER_MODE);
    }
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
    //    px_format = SDL_PIXELFORMAT_BGR24;

    dsc->texture = SDL_CreateTexture(dsc->renderer, px_format,
                                     SDL_TEXTUREACCESS_STATIC, hor_res, ver_res);
    SDL_SetTextureBlendMode(dsc->texture, SDL_BLENDMODE_BLEND);
}
#endif

static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);

    int32_t hor_res = lv_display_get_horizontal_resolution(disp);
    int32_t ver_res = lv_display_get_vertical_resolution(disp);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc->ignore_size_chg == false) {
        SDL_SetWindowSize(dsc->window, hor_res * dsc->zoom, ver_res * dsc->zoom);
    }

#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);
#endif
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
    SDL_DestroyRenderer(dsc->renderer);
    SDL_DestroyWindow(dsc->window);
#if LV_USE_DRAW_SDL == 0
    if(dsc->fb1) free(dsc->fb1);
    if(dsc->fb2) free(dsc->fb2);
    if(dsc->buf1) free(dsc->buf1);
    if(dsc->buf2) free(dsc->buf2);
#endif
    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

#endif /*LV_USE_SDL*/

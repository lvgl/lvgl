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
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"
#include "../../display/lv_display_private.h"
#include "../../lv_init.h"
#include "../../draw/lv_draw_buf.h"

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
} lv_sdl_window_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline int sdl_render_mode(void);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * color_p);
static void window_create(lv_display_t * disp);
static void window_update(lv_display_t * disp);
#if LV_USE_DRAW_SDL == 0
    static void texture_resize(lv_display_t * disp);
    static void * sdl_draw_buf_realloc_aligned(void * ptr, size_t new_size);
    static void sdl_draw_buf_free(void * ptr);
#endif
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
    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);
    lv_display_set_driver_data(disp, dsc);
    window_create(disp);

    lv_display_set_flush_cb(disp, flush_cb);

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
    /*LV_DISPLAY_RENDER_MODE_DIRECT or FULL */
    else {
        uint32_t stride = lv_draw_buf_width_to_stride(disp->hor_res,
                                                      lv_display_get_color_format(disp));
        lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * disp->ver_res,
                               LV_SDL_RENDER_MODE);
    }
#else /*LV_USE_DRAW_SDL == 1*/
    /*It will render directly to default Texture, so the buffer is not used, so just set something*/
    static lv_draw_buf_t draw_buf;
    static uint8_t dummy_buf; /*It won't be used as it will render to the SDL textures directly*/
    lv_draw_buf_init(&draw_buf, 4096, 4096, LV_COLOR_FORMAT_ARGB8888, 4096 * 4, &dummy_buf, 4096 * 4096 * 4);

    lv_display_set_draw_buffers(disp, &draw_buf, NULL);
    lv_display_set_render_mode(disp, LV_DISPLAY_RENDER_MODE_DIRECT);
#endif /*LV_USE_DRAW_SDL == 0*/
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

void * lv_sdl_window_get_renderer(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    return dsc->renderer;
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
                    window_update(disp);
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

static void window_create(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    dsc->zoom = 1.0;

    int flag = 0;
#if LV_SDL_FULLSCREEN
    flag |= SDL_WINDOW_FULLSCREEN;
#endif

    int32_t hor_res = (int32_t)((float)(disp->hor_res) * dsc->zoom);
    int32_t ver_res = (int32_t)((float)(disp->ver_res) * dsc->zoom);
    dsc->window = SDL_CreateWindow("LVGL Simulator",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   hor_res, ver_res, flag);       /*last param. SDL_WINDOW_BORDERLESS to hide borders*/

    dsc->renderer = SDL_CreateRenderer(dsc->window, -1,
                                       LV_SDL_ACCELERATED ? SDL_RENDERER_ACCELERATED : SDL_RENDERER_SOFTWARE);
#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(disp));
    lv_memset(dsc->fb1, 0xff, hor_res * ver_res * px_size);
    if(dsc->fb2) lv_memset(dsc->fb2, 0xff, hor_res * ver_res * px_size);

#endif /*LV_USE_DRAW_SDL == 0*/
    /*Some platforms (e.g. Emscripten) seem to require setting the size again */
    SDL_SetWindowSize(dsc->window, hor_res, ver_res);
#if LV_USE_DRAW_SDL == 0
    texture_resize(disp);
#endif /*LV_USE_DRAW_SDL == 0*/
}

static void window_update(lv_display_t * disp)
{
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
#if LV_USE_DRAW_SDL == 0
    int32_t hor_res = disp->hor_res;
    lv_color_format_t cf = lv_display_get_color_format(disp);
    if(cf == LV_COLOR_FORMAT_I1) {
        cf = LV_COLOR_FORMAT_ARGB8888;
    }
    uint32_t stride = lv_draw_buf_width_to_stride(hor_res, cf);
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
    lv_color_format_t cf = lv_display_get_color_format(disp);
    /*In some cases SDL stride might be different than LVGL render stride, like in I1 format.
    SDL still uses ARGB8888 as the color format, but LVGL renders in I1, thus causing a mismatch
    This ensures correct stride for SDL buffers in this case.*/
    if(cf == LV_COLOR_FORMAT_I1) {
        cf = LV_COLOR_FORMAT_ARGB8888;
    }
    uint32_t stride = lv_draw_buf_width_to_stride(disp->hor_res, cf);
    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);

    dsc->fb1 = sdl_draw_buf_realloc_aligned(dsc->fb1, stride * disp->ver_res);
    LV_ASSERT_MALLOC(dsc->fb1);
    lv_memzero(dsc->fb1, stride * disp->ver_res);

    if(sdl_render_mode() == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        dsc->fb_act = dsc->fb1;
    }
    else {
#if LV_SDL_BUF_COUNT == 2
        dsc->fb2 = sdl_draw_buf_realloc_aligned(dsc->fb2, stride * disp->ver_res);
        memset(dsc->fb2, 0x00, stride * disp->ver_res);
#endif
        lv_display_set_buffers(disp, dsc->fb1, dsc->fb2, stride * disp->ver_res, LV_SDL_RENDER_MODE);
    }
    if(dsc->texture) SDL_DestroyTexture(dsc->texture);

#if LV_COLOR_DEPTH == 32 || LV_COLOR_DEPTH == 1
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
                                     SDL_TEXTUREACCESS_STATIC, disp->hor_res, disp->ver_res);
    SDL_SetTextureBlendMode(dsc->texture, SDL_BLENDMODE_BLEND);
}

static void * sdl_draw_buf_realloc_aligned(void * ptr, size_t new_size)
{
    if(ptr) {
        sdl_draw_buf_free(ptr);
    }

    /* No need copy for drawing buffer */

#ifndef _WIN32
    /* Size must be multiple of align, See: https://en.cppreference.com/w/c/memory/aligned_alloc */

#define BUF_ALIGN (LV_DRAW_BUF_ALIGN < sizeof(void *) ? sizeof(void *) : LV_DRAW_BUF_ALIGN)
    return aligned_alloc(BUF_ALIGN, LV_ALIGN_UP(new_size, BUF_ALIGN));
#else
    return _aligned_malloc(LV_ALIGN_UP(new_size, LV_DRAW_BUF_ALIGN), LV_DRAW_BUF_ALIGN);
#endif /* _WIN32 */
}

static void sdl_draw_buf_free(void * ptr)
{
#ifndef _WIN32
    free(ptr);
#else
    _aligned_free(ptr);
#endif /* _WIN32 */
}
#endif

static void res_chg_event_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_current_target(e);

    lv_sdl_window_t * dsc = lv_display_get_driver_data(disp);
    if(dsc->ignore_size_chg == false) {
        SDL_SetWindowSize(dsc->window,
                          (int)((float)(disp->hor_res)*dsc->zoom), (int)((float)(disp->ver_res)*dsc->zoom));
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
    if(dsc->fb1) sdl_draw_buf_free(dsc->fb1);
    if(dsc->fb2) sdl_draw_buf_free(dsc->fb2);
    if(dsc->buf1) sdl_draw_buf_free(dsc->buf1);
    if(dsc->buf2) sdl_draw_buf_free(dsc->buf2);
#endif
    lv_free(dsc);
    lv_display_set_driver_data(disp, NULL);
}

#endif /*LV_USE_SDL*/

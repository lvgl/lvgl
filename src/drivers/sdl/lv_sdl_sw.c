/**
 * @file lv_sdl_sw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

/* for aligned_alloc */
#ifndef __USE_ISOC11
    #define _ISOC11_SOURCE
#endif

#include "lv_sdl_private.h"

#if LV_USE_SDL && !LV_SDL_USE_EGL && !LV_USE_DRAW_SDL

#ifndef _WIN32
    #include <stdlib.h>
#else
    #include <malloc.h>
#endif /* _WIN32 */

#include "../../display/lv_display_private.h"
#include "../../misc/lv_types.h"
#include "../../draw/sw/lv_draw_sw_utils.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    SDL_Texture * texture;
    SDL_Renderer * renderer;
    uint8_t * fb1;
    uint8_t * fb2;
    uint8_t * fb_act;
    uint8_t * buf1;
    uint8_t * buf2;
    uint8_t * rotated_buf;
    size_t rotated_buf_size;
} lv_sdl_sw_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * sdl_draw_buf_realloc_aligned(void * ptr, size_t new_size);
static void sdl_draw_buf_free(void * ptr);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static lv_result_t window_update(lv_display_t * disp);

static inline int sdl_render_mode(void)
{
    return LV_SDL_RENDER_MODE;
}

static lv_result_t init_display(lv_display_t * display);
static lv_result_t resize_display(lv_display_t * display);
static void deinit_display(lv_display_t * display);
static SDL_Renderer * get_renderer(lv_display_t * display);

const lv_sdl_backend_ops_t lv_sdl_backend_ops = {
    .init_display = init_display,
    .resize_display = resize_display,
    .deinit_display = deinit_display,
    .redraw = window_update,
    .get_renderer = get_renderer,
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t init_display(lv_display_t * display)
{
    lv_sdl_sw_display_data_t * ddata = lv_malloc_zeroed(sizeof(*ddata));
    if(!ddata) {
        LV_LOG_WARN("No memory for display data");
        return LV_RESULT_INVALID;
    }
    ddata->renderer = SDL_CreateRenderer(lv_sdl_window_get_window(display), -1,
                                         LV_SDL_ACCELERATED ? SDL_RENDERER_ACCELERATED : SDL_RENDERER_SOFTWARE);
    if(!ddata->renderer) {
        LV_LOG_ERROR("Failed to create SDL renderer '%s'", SDL_GetError());
        lv_free(ddata);
        return LV_RESULT_INVALID;
    }
    lv_sdl_backend_set_display_data(display, ddata);

    int32_t hor_res = lv_sdl_window_get_horizontal_resolution(display);
    int32_t ver_res = lv_sdl_window_get_vertical_resolution(display);

    resize_display(display);

    uint32_t px_size = lv_color_format_get_size(lv_display_get_color_format(display));
    lv_memset(ddata->fb1, 0xff, hor_res * ver_res * px_size);
    if(ddata->fb2) lv_memset(ddata->fb2, 0xff, hor_res * ver_res * px_size);

    lv_display_set_flush_cb(display, flush_cb);

    if(LV_SDL_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(lv_display_get_color_format(display)) * 4;
        uint32_t buffer_size_bytes = 32 * 1024 + palette_size;
        ddata->buf1 = sdl_draw_buf_realloc_aligned(NULL, buffer_size_bytes);
        if(LV_SDL_BUF_COUNT == 2) {
            ddata->buf2 = sdl_draw_buf_realloc_aligned(NULL, buffer_size_bytes);
        }
        lv_display_set_buffers(display, ddata->buf1, ddata->buf2, buffer_size_bytes, LV_SDL_RENDER_MODE);
    }
    else {
        /*LV_DISPLAY_RENDER_MODE_DIRECT or FULL */
        uint32_t stride = lv_draw_buf_width_to_stride(display->hor_res,
                                                      lv_display_get_color_format(display));
        lv_display_set_buffers(display, ddata->fb1, ddata->fb2, stride * display->ver_res,
                               LV_SDL_RENDER_MODE);
    }
    return LV_RESULT_OK;
}
static lv_result_t resize_display(lv_display_t * display)
{
    lv_color_format_t cf = lv_display_get_color_format(display);
    /*In some cases SDL stride might be different than LVGL render stride, like in I1 format.
    SDL still uses ARGB8888 as the color format, but LVGL renders in I1, thus causing a mismatch
    This ensures correct stride for SDL buffers in this case.*/
    if(cf == LV_COLOR_FORMAT_I1) {
        cf = LV_COLOR_FORMAT_ARGB8888;
    }
    uint32_t stride = lv_draw_buf_width_to_stride(display->hor_res, cf);
    lv_sdl_sw_display_data_t * ddata = lv_sdl_backend_get_display_data(display);

    ddata->fb1 = sdl_draw_buf_realloc_aligned(ddata->fb1, stride * display->ver_res);
    LV_ASSERT_MALLOC(ddata->fb1);
    lv_memzero(ddata->fb1, stride * display->ver_res);

    if(sdl_render_mode() == LV_DISPLAY_RENDER_MODE_PARTIAL) {
        ddata->fb_act = ddata->fb1;
    }
    else {
        if(LV_SDL_BUF_COUNT == 2) {
            ddata->fb2 = sdl_draw_buf_realloc_aligned(ddata->fb2, stride * display->ver_res);
            lv_memset(ddata->fb2, 0x00, stride * display->ver_res);
        }
        lv_display_set_buffers(display, ddata->fb1, ddata->fb2, stride * display->ver_res, LV_SDL_RENDER_MODE);
    }
    if(ddata->texture) SDL_DestroyTexture(ddata->texture);

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

    ddata->texture = SDL_CreateTexture(ddata->renderer, px_format,
                                       SDL_TEXTUREACCESS_STATIC, display->hor_res, display->ver_res);
    SDL_SetTextureBlendMode(ddata->texture, SDL_BLENDMODE_BLEND);
    return LV_RESULT_OK;
}

static void deinit_display(lv_display_t * display)
{

    lv_sdl_sw_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT_NULL(ddata);
    if(ddata->texture) {
        SDL_DestroyTexture(ddata->texture);
        ddata->texture = NULL;
    }
    if(ddata->renderer) {
        SDL_DestroyRenderer(ddata->renderer);
        ddata->renderer = NULL;
    }

    if(ddata->fb1) {
        sdl_draw_buf_free(ddata->fb1);
        ddata->fb1 = NULL;
    }
    if(ddata->fb2) {
        sdl_draw_buf_free(ddata->fb2);
        ddata->fb2 = NULL;
    }

    if(ddata->buf1) {
        sdl_draw_buf_free(ddata->buf1);
        ddata->buf1 = NULL;
    }
    if(ddata->buf2) {
        sdl_draw_buf_free(ddata->buf2);
        ddata->buf2 = NULL;
    }

    lv_free(ddata);
    lv_sdl_backend_set_display_data(display, NULL);
}

static SDL_Renderer * get_renderer(lv_display_t * display)
{
    lv_sdl_sw_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT_NULL(ddata);
    return ddata->renderer;
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

static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    lv_color_format_t cf = lv_display_get_color_format(display);
    lv_sdl_sw_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    uint32_t * argb_px_map = NULL;

    if(LV_SDL_RENDER_MODE == LV_DISPLAY_RENDER_MODE_PARTIAL) {
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
                lv_display_flush_ready(display);
                return;
            }
            /* skip the palette */
            px_map += LV_COLOR_INDEXED_PALETTE_SIZE(LV_COLOR_FORMAT_I1) * 4;
            const uint32_t i1_stride = lv_draw_buf_width_to_stride(width, LV_COLOR_FORMAT_I1);
            const uint32_t argb8888_stride = lv_draw_buf_width_to_stride(width, LV_COLOR_FORMAT_ARGB8888);
            lv_draw_sw_i1_to_argb8888(px_map, argb_px_map, width, height, i1_stride, argb8888_stride, 0xFF000000u, 0xFFFFFFFFu);
            px_map = (uint8_t *)argb_px_map;
        }

        lv_area_t rotated_area = *area;
        lv_display_rotate_area(display, &rotated_area);

        int32_t px_map_w = lv_area_get_width(area);
        int32_t px_map_h = lv_area_get_height(area);
        uint32_t px_map_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
        uint32_t px_size = lv_color_format_get_size(cf);

        int32_t fb_stride = lv_draw_buf_width_to_stride(display->hor_res, cf);
        uint8_t * fb_start = ddata->fb_act;
        fb_start += rotated_area.y1 * fb_stride + rotated_area.x1 * px_size;
        lv_display_rotation_t rotation = lv_display_get_rotation(display);

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

    if(lv_display_flush_is_last(display)) {
        if(sdl_render_mode() != LV_DISPLAY_RENDER_MODE_PARTIAL) {
            ddata->fb_act = px_map;
        }
        window_update(display);
    }
    free(argb_px_map);
    lv_display_flush_ready(display);
}

static lv_result_t window_update(lv_display_t * display)
{
    lv_color_format_t cf = lv_display_get_color_format(display);
    lv_sdl_sw_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    if(cf == LV_COLOR_FORMAT_I1) {
        cf = LV_COLOR_FORMAT_ARGB8888;
    }
    uint32_t stride = lv_draw_buf_width_to_stride(display->hor_res, cf);
    SDL_UpdateTexture(ddata->texture, NULL, ddata->fb_act, stride);

    SDL_RenderClear(ddata->renderer);

    /*Update the renderer with the texture containing the rendered image*/
    SDL_RenderCopy(ddata->renderer, ddata->texture, NULL, NULL);
    SDL_RenderPresent(ddata->renderer);
    return LV_RESULT_OK;
}

#endif /*LV_USE_SDL && !LV_SDL_USE_EGL && !LV_USE_DRAW_SDL*/

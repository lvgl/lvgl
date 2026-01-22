/**
 * @file lv_sdl_texture.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_sdl_private.h"

#if LV_USE_SDL && !LV_SDL_USE_EGL && LV_USE_DRAW_SDL

#include "../../draw/lv_draw_buf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    SDL_Renderer * renderer;
} lv_sdl_texture_display_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

static lv_result_t init_display(lv_display_t * display);
static lv_result_t resize(lv_display_t * display);
static void deinit_display(lv_display_t * display);
static SDL_Renderer * get_renderer(lv_display_t * display);
static lv_result_t redraw(lv_display_t * display);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_sdl_backend_ops_t lv_sdl_backend_ops = {
    .init_display = init_display,
    .resize_display = resize,
    .deinit_display = deinit_display,
    .get_renderer = get_renderer,
    .redraw = redraw,
};

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
    lv_sdl_texture_display_data_t * ddata = lv_malloc_zeroed(sizeof(*ddata));
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

    /*It will render directly to default Texture, so the buffer is not used, so just set something*/
    static lv_draw_buf_t draw_buf;
    static uint8_t dummy_buf; /*It won't be used as it will render to the SDL textures directly*/
    lv_draw_buf_init(&draw_buf, 4096, 4096, LV_COLOR_FORMAT_ARGB8888, 4096 * 4, &dummy_buf, 4096 * 4096 * 4);

    lv_display_set_draw_buffers(display, &draw_buf, NULL);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, flush_cb);
    return LV_RESULT_OK;
}

static lv_result_t resize(lv_display_t * display)
{
    LV_UNUSED(display);
    return LV_RESULT_OK;
}

static void deinit_display(lv_display_t * display)
{
    lv_sdl_texture_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT_NULL(ddata);

    if(ddata->renderer) {
        SDL_DestroyRenderer(ddata->renderer);
    }

    lv_free(ddata);
    lv_sdl_backend_set_display_data(display, NULL);
}

static void flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_sdl_texture_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    if(lv_display_flush_is_last(display)) {
        SDL_RenderPresent(ddata->renderer);
    }
    lv_display_flush_ready(display);
}

static SDL_Renderer * get_renderer(lv_display_t * display)
{
    lv_sdl_texture_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT_NULL(ddata);
    return ddata->renderer;
}
static lv_result_t redraw(lv_display_t * display)
{

    lv_sdl_texture_display_data_t * ddata = lv_sdl_backend_get_display_data(display);
    LV_ASSERT_NULL(ddata);
    SDL_RenderPresent(ddata->renderer);
    return LV_RESULT_OK;
}

#endif /*LV_USE_SDL && !LV_SDL_USE_EGL && LV_USE_DRAW_SDL*/

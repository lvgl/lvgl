/**
 * @file lv_sdl_private.h
 *
 */

#ifndef LV_SDL_PRIVATE_H
#define LV_SDL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_types.h"
#include "lv_sdl_window.h"

#if LV_USE_SDL

#include "../opengles/lv_opengles_egl_private.h"
#include "../opengles/lv_opengles_texture_private.h"

#include LV_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void * backend_data;
    SDL_Window * window;
    float zoom;
    uint8_t ignore_size_chg;
} lv_sdl_window_t;

void lv_sdl_backend_set_display_data(lv_display_t * display, void * backend_display_data);
void * lv_sdl_backend_get_display_data(lv_display_t * display);

int32_t lv_sdl_window_get_horizontal_resolution(lv_display_t * display);
int32_t lv_sdl_window_get_vertical_resolution(lv_display_t * display);

typedef lv_result_t (*lv_sdl_backend_init_display_t)(lv_display_t * disp);
typedef lv_result_t (*lv_sdl_backend_resize_display_t)(lv_display_t * disp);
typedef lv_result_t (*lv_sdl_backend_redraw_t)(lv_display_t * disp);
typedef SDL_Renderer * (*lv_sdl_backend_get_renderer_t)(lv_display_t * disp);
typedef void (*lv_sdl_backend_deinit_display_t)(lv_display_t * disp);

typedef struct {
    lv_sdl_backend_init_display_t init_display;
    lv_sdl_backend_resize_display_t resize_display;
    lv_sdl_backend_deinit_display_t deinit_display;
    lv_sdl_backend_redraw_t redraw;
    lv_sdl_backend_get_renderer_t get_renderer;
} lv_sdl_backend_ops_t;

extern const lv_sdl_backend_ops_t lv_sdl_backend_ops;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_sdl_keyboard_handler(SDL_Event * event);
void lv_sdl_mouse_handler(SDL_Event * event);
void lv_sdl_mousewheel_handler(SDL_Event * event);
lv_display_t * lv_sdl_get_disp_from_win_id(uint32_t win_id);


#if LV_SDL_USE_EGL
lv_result_t lv_sdl_egl_init(lv_display_t * disp);
lv_result_t lv_sdl_egl_resize(lv_display_t * disp);
void lv_sdl_egl_deinit(lv_display_t * disp);
#elif LV_USE_DRAW_SDL
lv_result_t lv_sdl_texture_init(lv_display_t * disp);
lv_result_t lv_sdl_texture_resize(lv_display_t * disp);
void lv_sdl_texture_deinit(lv_display_t * disp);
#else
lv_result_t lv_sdl_sw_init(lv_display_t * disp);
lv_result_t lv_sdl_sw_resize(lv_display_t * disp);
void lv_sdl_sw_deinit(lv_display_t * disp);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_PRIVATE_H */

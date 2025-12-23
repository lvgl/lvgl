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

#if LV_USE_EGL
    lv_opengles_egl_t * egl_ctx;
#if LV_USE_DRAW_OPENGLES
    lv_opengles_texture_t opengles_texture;
#endif
#endif
} lv_sdl_window_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_sdl_keyboard_handler(SDL_Event * event);
void lv_sdl_mouse_handler(SDL_Event * event);
void lv_sdl_mousewheel_handler(SDL_Event * event);
lv_display_t * lv_sdl_get_disp_from_win_id(uint32_t win_id);

#if LV_USE_EGL
lv_result_t lv_sdl_egl_init(lv_display_t * disp);
lv_result_t lv_sdl_egl_resize(lv_display_t * disp);
void lv_sdl_egl_deinit(lv_display_t * disp);
#endif

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_PRIVATE_H */

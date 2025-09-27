/**
 * @file lv_sdl_window.h
 *
 */

/**
 * Modified by NXP in 2025
 */

#ifndef LV_SDL_WINDOW_H
#define LV_SDL_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

#if LV_USE_SDL

/*********************
 *      DEFINES
 *********************/

/* Possible values of LV_SDL_MOUSEWHEEL_MODE */
#define LV_SDL_MOUSEWHEEL_MODE_ENCODER  0  /* The mousewheel emulates an encoder input device*/
#define LV_SDL_MOUSEWHEEL_MODE_CROWN    1  /* The mousewheel emulates a smart watch crown*/

/**********************
 *      TYPEDEFS
 **********************/

struct SDL_Window;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * lv_sdl_window_create(int32_t hor_res, int32_t ver_res);

void lv_sdl_window_set_resizeable(lv_display_t * disp, bool value);

void lv_sdl_window_set_size(lv_display_t * disp, int32_t hor_res, int32_t ver_res);

void lv_sdl_window_set_zoom(lv_display_t * disp, float zoom);

float lv_sdl_window_get_zoom(lv_display_t * disp);

void lv_sdl_window_set_title(lv_display_t * disp, const char * title);

void lv_sdl_window_set_icon(lv_display_t * disp, void * icon, int32_t width, int32_t height);

void * lv_sdl_window_get_renderer(lv_display_t * disp);

void lv_sdl_quit(void);

struct SDL_Window * lv_sdl_window_get_window(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/


#endif /* LV_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_WINDOW_H */

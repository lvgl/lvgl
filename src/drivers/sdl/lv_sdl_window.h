/**
 * @file lv_sdl_window.h
 *
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

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_display_t * lv_sdl_window_create(int32_t hor_res, int32_t ver_res);

void lv_sdl_window_set_resizeable(lv_display_t * disp, bool value);

void lv_sdl_window_set_zoom(lv_display_t * disp, uint8_t zoom);

uint8_t lv_sdl_window_get_zoom(lv_display_t * disp);

void lv_sdl_window_set_title(lv_display_t * disp, const char * title);

void * lv_sdl_window_get_renderer(lv_display_t * disp);

void lv_sdl_quit(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_WINDOW_H */

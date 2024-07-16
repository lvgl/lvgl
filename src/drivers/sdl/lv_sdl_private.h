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
#if LV_USE_SDL

#include LV_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_sdl_keyboard_handler(SDL_Event * event);
void _lv_sdl_mouse_handler(SDL_Event * event);
void _lv_sdl_mousewheel_handler(SDL_Event * event);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_PRIVATE_H */

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

void lv_sdl_keyboard_handler(SDL_Event * event);
void lv_sdl_mouse_handler(SDL_Event * event);
void lv_sdl_mousewheel_handler(SDL_Event * event);
lv_display_t * lv_sdl_get_disp_from_win_id(uint32_t win_id);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_PRIVATE_H */

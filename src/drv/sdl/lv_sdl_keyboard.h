/**
 * @file lv_sdl_keyboard.h
 *
 */

#ifndef LV_SDL_KEYBOARD_H
#define LV_SDL_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_sdl_window.h"
#if LV_USE_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_sdl_keyboard_priv_t;

typedef struct {
    struct _lv_sdl_keyboard_priv_t * _priv;
} lv_sdl_keyboard_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_dev_sdl_keyboard_init(lv_sdl_keyboard_t * dev);

lv_indev_t * lv_dev_sdl_keyboard_create(lv_sdl_keyboard_t * dev);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_KEYBOARD_H */

/**
 * @file Lv_sdl_mouse.h
 *
 */

#ifndef LV_SDL_MOUSE_H
#define LV_SDL_MOUSE_H

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
struct _lv_sdl_mouse_priv_t;

typedef struct {
    struct _lv_sdl_mouse_priv_t * _priv;
} lv_sdl_mouse_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_sdl_mouse_t * lv_sdl_mouse_create(void);

lv_indev_t * lv_sdl_mouse_register(lv_sdl_mouse_t * dev);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_MOUSE_H */

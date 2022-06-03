/**
 * @file lv_sdl_mousewheel.h
 *
 */

#ifndef LV_SDL_MOUSEWHEEL_H
#define LV_SDL_MOUSEWHEEL_H

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

struct _lv_sdl_mousewheel_priv_t;

typedef struct {
    struct _lv_sdl_mousewheel_priv_t * _priv;
} lv_sdl_mousewheel_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_sdl_mousewheel_init(lv_sdl_mousewheel_t * dev);

lv_indev_t * lv_sdl_mousewheel_create(lv_sdl_mousewheel_t * dev);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SDL*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DEV_SDL_MOUSEWHEEL_H */

/**
 * @file sdl_common.h
 *
 */

#ifndef LV_DRV_SDL_INDEV_H
#define LV_DRV_SDL_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_sdl_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_drv_sdl_mouse_priv_t;

typedef struct {
    struct _lv_drv_sdl_mouse_priv_t * _priv;
} lv_drv_sdl_mouse_dsc_t;

struct _lv_drv_sdl_mousewheel_priv_t;

typedef struct {
    struct _lv_drv_sdl_mousewheel_priv_t * _priv;
} lv_drv_sdl_mousewheel_dsc_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_drv_sdl_mouse_init(lv_drv_sdl_mouse_dsc_t * dsc);

lv_indev_t * lv_drv_sdl_mouse_create(lv_drv_sdl_mouse_dsc_t * dsc);

void lv_drv_sdl_mousewheel_init(lv_drv_sdl_mousewheel_dsc_t * dsc);

lv_indev_t * lv_drv_sdl_mousewheel_create(lv_drv_sdl_mousewheel_dsc_t * dsc);
/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_INDEV_H */

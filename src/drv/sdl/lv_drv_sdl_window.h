/**
 * @file lv_drv_sdl_disp.h
 *
 */

#ifndef LV_DRV_SDL_DISP_H
#define LV_DRV_SDL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_drv.h"
#if LV_USE_DRV_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_drv_sdl_disp_priv_t;
typedef struct {
    lv_drv_t base;
    lv_coord_t hor_res;
    lv_coord_t ver_res;
    uint8_t zoom;
    struct _lv_drv_sdl_disp_priv_t * _priv;
} lv_drv_sdl_window_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_drv_sdl_window_init(lv_drv_sdl_window_t * dev);

lv_disp_drv_t * lv_drv_sdl_window_create(lv_drv_sdl_window_t * drv);

lv_disp_t * lv_drv_sdl_get_disp_from_win_id(uint32_t win_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_DISP_H */

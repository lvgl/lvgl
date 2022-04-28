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

#include "../../../src/hal/lv_hal_disp.h"
#if LV_USE_DRV_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
struct _lv_drv_sdl_disp_priv_t;
typedef struct {
    lv_coord_t hor_res;
    lv_coord_t ver_res;
    uint32_t buf_size;
    uint8_t zoom;
    struct _lv_drv_sdl_disp_priv_t * _priv;
} lv_dev_sdl_window_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_dev_sdl_window_init(lv_dev_sdl_window_t * dev);

lv_disp_t * lv_dev_sdl_window_create(lv_dev_sdl_window_t * dev);

lv_disp_t * lv_dev_sdl_get_from_win_id(uint32_t win_id);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_DISP_H */

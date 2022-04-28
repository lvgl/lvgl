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
} lv_drv_sdl_disp_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_drv_sdl_disp_init(lv_drv_sdl_disp_dsc_t * dsc);

lv_disp_t * lv_drv_sdl_disp_create(lv_drv_sdl_disp_dsc_t * dsc);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_DISP_H */

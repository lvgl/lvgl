/**
 * @file lv_platform_sdl.h
 *
 */

#ifndef LV_PLATFORM_SDL_H
#define LV_PLATFORM_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../src/hal/lv_hal_disp.h"
#if LV_USE_PLATFORM_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    int placeholder;
} lv_platform_sdl_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_platform_sdl_disp_init(lv_platform_sdl_dsc_t * dsc);

void lv_platform_sdl_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_DRV_SDL_DISP_H */

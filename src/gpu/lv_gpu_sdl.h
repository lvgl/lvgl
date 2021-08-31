/**
 * @file lv_gpu_sdl.h
 *
 */

#ifndef LV_GPU_SDL_H
#define LV_GPU_SDL_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../hal/lv_hal_disp.h"
#include "../misc/lv_color.h"
#include "../misc/lv_area.h"
#include "../misc/lv_style.h"
#include "../font/lv_font.h"
#include "../draw/lv_draw_mask.h"

#include LV_GPU_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_disp_t *lv_sdl_display_init(SDL_Window *window);

void lv_sdl_display_deinit(lv_disp_t *disp);


/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GPU_SDL_H*/

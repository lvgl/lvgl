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

#if LV_USE_EXTERNAL_RENDERER == 0
#error "SDL GPU requires LV_USE_EXTERNAL_RENDERER 1. Enable it in lv_conf.h"
#endif

#include LV_GPU_SDL_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_gpu_sdl_init();

/**
 * @brief Free caches
 * 
 */
void lv_gpu_sdl_deinit();

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

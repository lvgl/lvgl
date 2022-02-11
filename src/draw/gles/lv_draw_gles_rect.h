/**
 * @file lv_draw_gles_rect.h
 *
 */

#ifndef LV_DRAW_GLES_RECT_H
#define LV_DRAW_GLES_RECT_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

//#include LV_GPU_SDL_INCLUDE_PATH

#include "../lv_draw.h"

//#include "lv_draw_sdl_texture_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
#if 0
typedef struct lv_draw_sdl_rect_header_t {
    lv_img_header_t base;
    SDL_Rect rect;
} lv_draw_sdl_rect_header_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

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

#endif /*LV_DRAW_SDL_RECT_H*/

/**
 * @file lv_draw_gles_priv.h
 *
 */

#ifndef LV_DRAW_GLES_PRIV_H
#define LV_DRAW_GLES_PRIV_H


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

#include <cglm/cglm.h>


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct lv_draw_gles_context_internals_t {
    GLubyte *gpu_texture_pixels;
    GLuint gpu_texture;

} lv_draw_gles_context_internals_t;

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

#endif /*LV_USE_GPU_SDL_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_PRIV_H*/

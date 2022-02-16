/**
 * @file lv_draw_gles_utils.h
 *
 */
#ifndef LV_DRAW_GLES_UTILS_H
#define LV_DRAW_GLES_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_GPU_SDL_GLES

#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

#include "lv_draw_gles_priv.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void _lv_draw_gles_utils_init();
void lv_draw_gles_utils_internals_init(lv_draw_gles_context_internals_t * internals);
GLuint lv_draw_gles_shader_program_create(const char *vertex_src, const char *fragment_src);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_SDL_GLES*/
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_UTILS_H*/

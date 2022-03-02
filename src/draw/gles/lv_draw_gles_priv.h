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
    GLuint framebuffer;
    mat4 projection;
    GLuint plain_rect_shader;
    GLint plain_rect_shader_pos_location;
    GLint plain_rect_shader_projection_location;
    GLint plain_rect_shader_model_location;
    GLint plain_rect_shader_color_location;
    GLuint rect_shader;
    GLint rect_shader_pos_location;
    GLint rect_shader_projection_location;
    GLint rect_shader_model_location;
    GLint rect_shader_color_location;
    GLint rect_shader_resolution_location;
    GLint rect_shader_radius_location;
    GLint rect_shader_coords_location;
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

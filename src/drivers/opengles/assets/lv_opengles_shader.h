/**
 * @file lv_opengles_shader.h
 *
 */

#ifndef LV_OPENGLES_SHADER_H
#define LV_OPENGLES_SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../opengl_shader/lv_opengl_shader_internal.h"

#if LV_USE_OPENGLES

/**********************
 * GLOBAL PROTOTYPES
 **********************/

char * lv_opengles_shader_get_vertex(lv_opengl_glsl_version version);
char * lv_opengles_shader_get_fragment(lv_opengl_glsl_version version);
void lv_opengles_shader_get_source(lv_opengl_shader_portions_t * portions, lv_opengl_glsl_version version);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_SHADER_H*/

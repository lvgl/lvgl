/**
 * @file lv_opengles_standard_shader.h
 *
 */

#ifndef LV_OPENGLES_STANDARD_SHADER_H
#define LV_OPENGLES_STANDARD_SHADER_H

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

char *lv_opengles_standard_shader_get_vertex(void);
char *lv_opengles_standard_shader_get_fragment(void);
void lv_opengles_standard_shader_get_src(lv_opengl_shader_portions_t *portions);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_STANDARD_SHADER_H*/

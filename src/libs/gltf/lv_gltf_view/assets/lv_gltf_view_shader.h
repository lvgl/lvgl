/**
 * @file lv_gltf_view_shader.h
 *
 */

#ifndef LV_VIEW_SHADER_H
#define LV_VIEW_SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_gl_shader/lv_gl_shader_internal.h"

#if LV_USE_GLTF


/*********************
 *      DEFINES
 *********************/

#define GLSL_VERSION_PREFIX "#version 300 es\n"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	lv_gl_shader_t* shader_list;
	size_t count;
}lv_gltf_view_shader;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

char* lv_gltf_view_shader_get_vertex(void);
char* lv_gltf_view_shader_get_fragment(void);
void lv_gltf_view_shader_get_src(lv_gltf_view_shader* shaders);
void lv_gltf_view_shader_get_env(lv_gltf_view_shader* shaders);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_VIEW_SHADER_H*/

/**
 * @file lv_gltf_view_shader.h
 *
 */

#ifndef LV_GLTF_VIEW_SHADER_H
#define LV_GLTF_VIEW_SHADER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"

#if LV_USE_GLTF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
	const lv_opengl_shader_t *shader_list;
	size_t count;
} lv_gltf_view_shader_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

char *lv_gltf_view_shader_get_vertex(void);
char *lv_gltf_view_shader_get_fragment(void);
void lv_gltf_view_shader_get_src(lv_gltf_view_shader_t *shaders);
void lv_gltf_view_shader_get_env(lv_gltf_view_shader_t *shaders);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GLTF_VIEW_SHADER_H*/

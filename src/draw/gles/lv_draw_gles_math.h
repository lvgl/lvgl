/**
 * @file lv_draw_gles_math.h
 *
 */

#ifndef LV_DRAW_GLES_MATH_H
#define LV_DRAW_GLES_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec4 mat4[4];

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_draw_gles_math_mat4_zero(mat4 m);
void lv_draw_gles_math_mat4_identity(mat4 m);
void lv_draw_gles_math_mat4_copy(mat4 m, mat4 res);
void lv_draw_gles_math_mat4_mul(mat4 m1, mat4 m2, mat4 res);
void lv_draw_gles_math_translate(mat4 m, vec3 v);
void lv_draw_gles_math_scale(mat4 m, vec3 v);
void lv_draw_gles_math_ortho(float left, float right, float bottom, float top, float near, float far, mat4 res);
/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif


#endif /*LV_DRAW_GLES_MATH_H*/
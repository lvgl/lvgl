/**
 * @file lv_draw_gles_shaders.h
 *
 */

#ifndef LV_DRAW_GLES_SHADER_H
#define LV_DRAW_GLES_SHADER_H

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

#define RECT_VERTEX_SHADER_SRC  \
    "attribute vec2 a_position;"  \
    "uniform mat4 u_projection;"\
    "uniform mat4 u_model;"\
    "void main()"\
    "{"\
    "   gl_Position = u_projection * u_model * vec4(a_position.x, a_position.y, 0.0, 1.0);"\
    "}"

#define PLAIN_RECT_VERTEX_SHADER_SRC  \
    "attribute vec2 a_position;"  \
    "uniform mat4 u_projection;"\
    "uniform mat4 u_model;"\
    "void main()"\
    "{"\
    "   gl_Position = u_projection * u_model * vec4(a_position.x, a_position.y, 0.0, 1.0);"\
    "}"

#define PLAIN_RECT_FRAGMENT_SHADER_SRC \
    "precision mediump float;" \
    "uniform vec4 u_color;" \
    "void main()" \
    "{" \
    "    gl_FragColor = u_color;" \
    "}"

#define CORNER_RECT_VERTEX_SHADER_SRC \
    "attribute vec2 a_position;" \
    "uniform mat4 u_projection;" \
    "uniform mat4 u_model;" \
    "void main()" \
    "{" \
    "   gl_Position = u_projection * u_model * vec4(a_position.x, a_position.y, 0.0, 1.0);" \
    "}"

#define CORNER_RECT_FRAGMENT_SHADER_SRC \
    "precision mediump float;" \
    "uniform vec4 u_color;" \
    "uniform vec2 u_corner;" \
    "uniform float u_radius;" \
    "void main()" \
    "{" \
    "    float dist = distance(gl_FragCoord.xy, u_corner);" \
    "    float c = smoothstep(u_radius, u_radius - 0.8, dist);" \
    "    vec4 mask_color = vec4(u_color.r , u_color.g, u_color.b, c);" \
    "    gl_FragColor = mask_color;" \
    "}"

#define SIMPLE_IMG_VERTEX_SHADER_SRC \
    "attribute vec2 a_position;" \
    "attribute vec2 a_uv;" \
    "varying vec2 v_uv;" \
    "uniform mat4 u_projection;" \
    "uniform mat4 u_model;" \
    "void main()" \
    "{" \
    "   gl_Position = u_projection * u_model * vec4(a_position.x, a_position.y, 0.0, 1.0);" \
    "   v_uv = a_uv;" \
    "}"

#define SIMPLE_IMG_FRAGMENT_SHADER_SRC \
    "precision mediump float;" \
    "varying vec2 v_uv;" \
    "uniform sampler2D s_texture;" \
    "uniform vec4 u_color;" \
    "void main()" \
    "{" \
    "    vec4 texture_color = texture2D(s_texture, v_uv);" \
    "    if(texture_color.a < 0.1) discard;" \
    "    vec4 mix_color =  mix(vec4(texture_color.rgb, 1.0), vec4(u_color.rgb,1.0), u_color.a);" \
    "    gl_FragColor = mix_color;" \
    "}"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GPU_GLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_GLES_SHADER_H*/
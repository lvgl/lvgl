/**
 * @file lv_opengles_private.h
 *
 */

#ifndef LV_OPENGLES_PRIVATE_H
#define LV_OPENGLES_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_OPENGLES

#include "../../misc/lv_area.h"
#include "../../misc/lv_color.h"

#if LV_USE_EGL
#include "glad/include/glad/gles2.h"
#include "glad/include/glad/egl.h"
#else
#include "glad/include/glad/gl.h"
#endif /*LV_USE_EGL*/

#if LV_USE_GLFW
#include <GLFW/glfw3.h>
#endif

/*********************
 *      DEFINES
 *********************/

/* In desktop GL (<Gl/gl.h>) these symbols are defined but for EGL
 * they are defined as extensions with the _EXT suffix */
#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif /*GL_BGRA*/

#ifndef GL_TEXTURE_MAX_LEVEL
#define GL_TEXTURE_MAX_LEVEL GL_TEXTURE_MAX_LEVEL_APPLE
#endif /*GL_TEXTURE_MAX_LEVEL*/

#ifndef GL_UNPACK_ROW_LENGTH
#define GL_UNPACK_ROW_LENGTH GL_UNPACK_ROW_LENGTH_EXT
#endif /*GL_UNPACK_ROW_LENGTH*/

#ifndef glGenVertexArrays
#define glGenVertexArrays glGenVertexArraysOES
#endif

#ifndef glBindVertexArray
#define glBindVertexArray glBindVertexArrayOES
#endif

#ifndef glDeleteVertexArrays
#define glDeleteVertexArrays glDeleteVertexArraysOES
#endif

#ifndef glTexStorage2D
#define glTexStorage2D glTexStorage2DEXT
#endif

#ifndef GL_RGBA32F
#define GL_RGBA32F 0x8814
#endif
#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS 0x821D
#endif

#ifndef GL_RGB8
#define GL_RGB8 0x8051
#endif

#ifndef GL_RGBA8
#define GL_RGBA8 0x8058
#endif

#ifndef LV_GL_PREFERRED_DEPTH
#ifdef GL_DEPTH_COMPONENT24
#define LV_GL_PREFERRED_DEPTH GL_DEPTH_COMPONENT24
#else
/*
 * This will not run correctly yet, it compiles fine but fails to render on RPi3B.  Work in progress.
 *
#ifdef GL_DEPTH_COMPONENT24_OES
#define LV_GL_PREFERRED_DEPTH GL_DEPTH_COMPONENT24_OES
#else
#define LV_GL_PREFERRED_DEPTH GL_DEPTH_COMPONENT16
#endif
*/
#define LV_GL_PREFERRED_DEPTH GL_DEPTH_COMPONENT16
#endif
#endif
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_opengles_render(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                        int32_t disp_w, int32_t disp_h, const lv_area_t * texture_clip_area,
                        bool h_flip, bool v_flip, lv_color_t fill_color, bool blend_opt, bool flipRB);


/**
 * Render a texture using alternate blending mode, with red and blue channels flipped in the shader.
 * @param texture        OpenGL texture ID
 * @param texture_area   the area in the window to render the texture in
 * @param opa            opacity to blend the texture with existing contents
 * @param disp_w         width of the window/framebuffer being rendered to
 * @param disp_h         height of the window/framebuffer being rendered to
 * @param h_flip         horizontal flip
 * @param v_flip         vertical flip
 */
void lv_opengles_render_texture_rbswap(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                                       int32_t disp_w, int32_t disp_h, const lv_area_t * texture_clip_area,
                                       bool h_flip, bool v_flip);

/**
 * Set the OpenGL viewport, with vertical co-ordinate conversion
 * @param x        x position of the viewport
 * @param y        y position of the viewport
 * @param w        width of the viewport
 * @param h        height of the viewport
 */
void lv_opengles_regular_viewport(int32_t x, int32_t y, int32_t w, int32_t h);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_PRIVATE_H*/

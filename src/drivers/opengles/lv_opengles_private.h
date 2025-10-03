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

#if LV_USE_EGL
#include "glad/include/glad/gles2.h"
#include "glad/include/glad/egl.h"
#else
/* For now, by default we add glew and glfw.
   In the future we need to consider adding a config for setting these includes*/
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif /*LV_USE_EGL*/

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

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_PRIVATE_H*/

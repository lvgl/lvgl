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

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
#include <GLES/gl.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <GLES/glext.h>
#include <GLES2/gl2ext.h>
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

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif /*GL_BGR*/

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

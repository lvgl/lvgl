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

#if LV_USE_OPENGLES_API == LV_OPENGLES_API_GLFW
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif LV_USE_OPENGLES_API == LV_OPENGLES_API_EGL
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#endif

/*********************
 *      DEFINES
 *********************/

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

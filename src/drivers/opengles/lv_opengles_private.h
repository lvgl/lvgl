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

#include "../../misc/lv_color.h"

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
#ifdef GL_BGR_EXT
#define GL_BGR GL_BGR_EXT
#endif
#endif /*GL_BGR*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline void lv_color_format_to_gl_color_format(lv_color_format_t cf, GLenum * internal_format, GLenum * format,
                                                      GLenum * type)
{
    switch(cf) {
        case LV_COLOR_FORMAT_L8:
            *internal_format = GL_R8;
            *format = GL_RED;
            *type = GL_UNSIGNED_BYTE;
            break;

        case LV_COLOR_FORMAT_RGB565:
            *internal_format = GL_RGB565;
            *format = GL_RGB;
            *type = GL_UNSIGNED_SHORT_5_6_5;
            break;

#ifdef GL_BGR
        case LV_COLOR_FORMAT_RGB888:
            *internal_format = GL_RGB;
            *format = GL_BGR;
            *type = GL_UNSIGNED_BYTE;
            break;
#endif

        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            *internal_format = GL_RGBA;
            *format = GL_BGRA;
            *type = GL_UNSIGNED_BYTE;
            break;

        default:
            LV_ASSERT_FORMAT_MSG(false, "Unsupported color format: %d", cf);
            break;
    }
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OPENGLES_PRIVATE_H*/

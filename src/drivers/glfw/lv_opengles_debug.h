/**
 * @file lv_opengles_debug.h
 *
 */

#ifndef LV_OPENGLES_DEBUG_H
#define LV_OPENGLES_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void GLClearError(void);

bool GLLogCall(const char * function, const char * file, int line);

#if LV_USE_OPENGLES_DEBUG
#define GL_CALL(x) GLClearError();\
    x;\
    GLLogCall(#x, __FILE__, __LINE__)
#else
#define GL_CALL(x) x
#endif

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DEBUG_H */

/**
 * @file lv_opengles_common.h
 *
 */

#ifndef LV_OPENGLES_COMMON_H
#define LV_OPENGLES_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#if LV_USE_OPENGLES

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if LV_USE_OPENGLES_DEBUG
#define ASSERT(x) if(!(x)) __builtin_trap();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

void GLClearError();

bool GLLogCall(const char * function, const char * file, int line);

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_COMMON_H */

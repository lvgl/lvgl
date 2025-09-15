/**
 * @file lv_opengles_debug.h
 *
 */

#ifndef LV_OPENGLES_DEBUG_H
#define LV_OPENGLES_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_OPENGLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

#if LV_USE_OPENGLES_DEBUG

void GLClearError(void);

void GLLogCall(const char * function, const char * file, int line);

#endif

/**********************
 *      MACROS
 **********************/

#if LV_USE_OPENGLES_DEBUG
#define GL_CALL(x) do {\
        GLClearError();\
        x;\
        GLLogCall(#x, __FILE__, __LINE__);\
    } while(0)
#else
#define GL_CALL(x) x
#endif

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DEBUG_H */

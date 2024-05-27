/**
 * @file lv_opengles_debug.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../display/lv_display.h"
#include "lv_opengles_debug.h"

#if LV_USE_OPENGLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char * function, const char * file, int line)
{
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
        LV_LOG_ERROR("[OpenGL Error] (%d) %s %s:%d\n", error, function, file, line);
        return false;
    }
    return true;
}

#endif /* LV_USE_OPENGLES */

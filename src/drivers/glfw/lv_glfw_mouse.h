/**
 * @file lv_glfw_mouse.h
 *
 */

#ifndef LV_GLFW_MOUSE_H
#define LV_GLFW_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_glfw_window.h"
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

lv_indev_t * lv_glfw_mouse_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_GLFW_MOUSE_H */

/**
 * @file lv_glfw_window_private.h
 *
 */

#ifndef LV_GLFW_WINDOW_PRIVATE_H
#define LV_GLFW_WINDOW_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_glfw_window.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_area.h"
#include "../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_glfw_texture_t {
    unsigned int texture_id;
    lv_area_t area;
    lv_display_t * disp;
};

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

#endif /*LV_GLFW_WINDOW_PRIVATE_H*/

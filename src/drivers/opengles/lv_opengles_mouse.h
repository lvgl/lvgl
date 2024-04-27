/**
 * @file Lv_opengles_mouse.h
 *
 */

#ifndef LV_OPENGLES_MOUSE_H
#define LV_OPENGLES_MOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_opengles_window.h"
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

lv_indev_t * lv_opengles_mouse_create(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OPENGLES*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_MOUSE_H */

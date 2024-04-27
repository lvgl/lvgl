/**
 * @file lv_opengles_window.h
 *
 */

#ifndef LV_OPENGLES_WINDOW_H
#define LV_OPENGLES_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"
#include "../../indev/lv_indev.h"

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

lv_display_t * lv_opengles_window_create(int32_t hor_res, int32_t ver_res);

void lv_opengles_quit();

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_WINDOW_H */

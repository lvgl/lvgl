/**
 * @file lv_draw_opengles.h
 *
 */

#ifndef LV_DRAW_OPENGLES_H
#define LV_DRAW_OPENGLES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_OPENGLES

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_opengles_init(void);
void lv_draw_opengles_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_OPENGLES*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_OPENGLES_H*/

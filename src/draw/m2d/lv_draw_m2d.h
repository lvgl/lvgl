/**
 * @file lv_draw_m2d.h
 *
 */

#ifndef LV_DRAW_M2D_H
#define LV_DRAW_M2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_M2D

/*********************
 *      DEFINES
 *********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the M2D renderer. Called internally.
 * It creates one M2D renderer
 */
void lv_draw_m2d_init(void);

/**
 * Deinitialize the M2D renderer
 */
void lv_draw_m2d_deinit(void);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_M2D*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_M2D_H*/

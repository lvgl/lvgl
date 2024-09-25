/**
 * @file lv_nanosvg.h
 *
 */

#ifndef LV_NANOSVG_H
#define LV_NANOSVG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_NANOSVG

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Register the SVG decoder functions in LVGL
 */
void lv_nanosvg_init(void);

void lv_nanosvg_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NANOSVG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_NANOSVG_H*/

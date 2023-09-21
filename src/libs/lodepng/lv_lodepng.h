/**
 * @file lv_lodepng.h
 *
 */

#ifndef LV_LODEPNG_H
#define LV_LODEPNG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_LODEPNG

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
 * Register the PNG decoder functions in LVGL
 */
void lv_lodepng_init(void);

void lv_lodepng_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LODEPNG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LODEPNG_H*/

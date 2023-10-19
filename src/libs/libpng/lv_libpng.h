/**
 * @file lv_libpng.h
 *
 */

#ifndef LV_LIBPNG_H
#define LV_LIBPNG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_LIBPNG

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
void lv_libpng_init(void);

void lv_libpng_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIBPNG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LIBPNG_H*/

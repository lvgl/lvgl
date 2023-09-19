/**
 * @file lv_jpeg_turbo.h
 *
 */

#ifndef LV_JPEG_TURBO_H
#define LV_JPEG_TURBO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_JPEG_TURBO

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
 * Register the JPEG decoder functions in LVGL
 */
void lv_jpeg_turbo_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_JPEG_TURBO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_JPEG_TURBO_H*/
/**
 * @file lv_libjpeg_turbo.h
 *
 */

#ifndef LV_LIBJPEG_TURBO_H
#define LV_LIBJPEG_TURBO_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_LIBJPEG_TURBO

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
 * Register the JPEG-Turbo decoder functions in LVGL
 */
void lv_libjpeg_turbo_init(void);

void lv_libjpeg_turbo_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIBJPEG_TURBO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LIBJPEG_TURBO_H*/

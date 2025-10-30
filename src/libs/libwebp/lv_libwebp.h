/**
 * @file lv_libwebp.h
 *
 */

#ifndef LV_LIBWEBP_H
#define LV_LIBWEBP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_LIBWEBP

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
 * Register the WEBP decoder functions in LVGL
 */
void lv_libwebp_init(void);

void lv_libwebp_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_LIBWEBP*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LIBWEBP_H*/

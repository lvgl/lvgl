/**
 * @file lv_etc2.h
 *
 */

#ifndef LV_ETC2_H
#define LV_ETC2_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_ETC2

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
 * Register the ETC2 decoder functions in LVGL
 */

void lv_etc2_init(void);

void lv_etc2_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_ETC2*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_ETC2_H*/

/**
 * @file lv_qoi.h
 *
 */

#ifndef LV_QOI_H
#define LV_QOI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"
#if LV_USE_QOI

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
 * Register the QOI decoder functions in LVGL
 */
void lv_qoi_init(void);

void lv_qoi_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QOI*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_QOI_H*/
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
#include "../../lv_conf_internal.h"
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
 * @brief Register the QOI decoder functions in LVGL.
 *
 * Sets up the QOI image decoder and registers it with the LVGL image decoding subsystem.
 */
void lv_qoi_init(void);

/**
 * @brief Deinitialize the QOI decoder and remove it from LVGL.
 *
 * Cleans up resources and unregisters the QOI image decoder from the LVGL image decoding subsystem.
 */
void lv_qoi_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_QOI*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_QOI_H*/
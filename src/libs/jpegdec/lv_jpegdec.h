/**
 * @file lv_jpegdec.h
 *
 */

#ifndef LV_JPEGDEC_H
#define LV_JPEGDEC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_JPEGDEC

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
void lv_jpegdec_init(void);

void lv_jpegdec_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_JPEGDEC*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_JPEGDEC_H*/

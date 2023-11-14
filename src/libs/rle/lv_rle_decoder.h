/**
 * @file lv_rle_decoder.h
 *
 */

#ifndef LV_RLE_DECODER_H
#define LV_RLE_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_RLE

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
 * Register the RLE decoder functions in LVGL
 */
void lv_rle_decoder_init(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_RLE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_RLE_DECODER_H*/

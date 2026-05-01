/**
 * @file lv_svg_decoder.h
 *
 */

#ifndef LV_SVG_DECODER_H
#define LV_SVG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_public_api.h"

#if LV_USE_SVG

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
 * Register the SVG decoder functions in LVGL
 */
void lv_svg_decoder_init(void);

void lv_svg_decoder_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SVG_DECODER_H*/

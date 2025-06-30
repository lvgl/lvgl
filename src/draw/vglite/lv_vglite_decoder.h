/**
 * @file lv_vglite_decoder.h
 *
 */

#ifndef LV_VGLITE_DECODER_H
#define LV_VGLITE_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_image_decoder.h"

#if LV_USE_DRAW_VGLITE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_vglite_decoder_init(void);
void lv_vglite_decoder_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VGLITE_DECODER_H*/

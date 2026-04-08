/**
 * @file lv_font_private.h
 *
 */

#ifndef LV_FONT_PRIVATE_H
#define LV_FONT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

int32_t lv_font_glyph_dsc_compare(const lv_font_glyph_dsc_t * lhs, const lv_font_glyph_dsc_t * rhs);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_PRIVATE_H*/


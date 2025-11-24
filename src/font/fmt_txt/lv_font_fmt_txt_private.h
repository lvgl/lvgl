/**
 * @file lv_font_fmt_txt_private.h
 *
 */

#ifndef LV_FONT_FMT_TXT_PRIVATE_H
#define LV_FONT_FMT_TXT_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_fmt_txt.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_FONT_COMPRESSED
typedef enum {
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATED,
    RLE_STATE_COUNTER,
} lv_font_fmt_rle_state_t;

typedef struct {
    uint32_t rdp;
    const uint8_t * in;
    uint8_t bpp;
    uint8_t prev_v;
    uint8_t count;
    lv_font_fmt_rle_state_t state;
} lv_font_fmt_rle_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FONT_FMT_TXT_PRIVATE_H*/

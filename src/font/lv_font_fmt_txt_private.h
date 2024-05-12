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

/** A simple mapping of kern values from pairs*/
struct lv_font_fmt_txt_kern_pair_t {
    /*To get a kern value of two code points:
       1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_fmt_txt_cmap_t
       2. for(i = 0; i < pair_cnt * 2; i += 2)
             if(glyph_ids[i] == glyph_id_left &&
                glyph_ids[i+1] == glyph_id_right)
                 return values[i / 2];
     */
    const void * glyph_ids;
    const int8_t * values;
    uint32_t pair_cnt   : 30;
    uint32_t glyph_ids_size : 2;    /*0: `glyph_ids` is stored as `uint8_t`; 1: as `uint16_t`*/
};

#if LV_USE_FONT_COMPRESSED
typedef enum {
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATE,
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

/**
 * @file lv_api_map_v9_0.h
 *
 */

#ifndef LV_API_MAP_V9_0_H
#define LV_API_MAP_V9_0_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/
#define lv_image_set_align               lv_image_set_inner_align
#define lv_image_get_align               lv_image_get_inner_align

#define _lv_text_get_next_line         lv_text_get_next_line
#define _lv_text_ins                   lv_text_ins
#define _lv_text_cut                   lv_text_cut
#define _lv_text_set_text_vfmt         lv_text_set_text_vfmt
#define _lv_text_encoded_letter_next_2 lv_text_encoded_letter_next_2
#define _lv_text_is_break_char         lv_text_is_break_char
#define _lv_text_is_a_word             lv_text_is_a_word
#define _lv_text_is_marker             lv_text_is_marker
#define _lv_text_encoded_size          lv_text_encoded_size
#define _lv_text_unicode_to_encoded    lv_text_unicode_to_encoded
#define _lv_text_encoded_conv_wc       lv_text_encoded_conv_wc
#define _lv_text_encoded_next          lv_text_encoded_next
#define _lv_text_encoded_prev          lv_text_encoded_prev
#define _lv_text_encoded_get_byte_id   lv_text_encoded_get_byte_id
#define _lv_text_encoded_get_char_id   lv_text_encoded_get_char_id
#define _lv_text_get_encoded_length    lv_text_get_encoded_length

#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 * DEPRECATED FUNCTIONS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_API_MAP_V9_0_H*/

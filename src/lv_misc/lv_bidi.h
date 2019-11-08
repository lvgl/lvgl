/**
 * @file lv_bifi.h
 *
 */

#ifndef LV_BIDI_H
#define LV_BIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
/* Special non printable  strong characters.
 * They can be inserted to texts to affect the run's direction*/
#define LV_BIDI_LRO  "\xE2\x80\xAD" /*U+202D*/
#define LV_BIDI_RLO  "\xE2\x80\xAE" /*U+202E*/

/**********************
 *      TYPEDEFS
 **********************/
enum
{
    /*The first 4 values are stored in `lv_obj_t` on 2 bits*/
    LV_BIDI_DIR_LTR      = 0x00,
    LV_BIDI_DIR_RTL      = 0x01,
    LV_BIDI_DIR_AUTO     = 0x02,
    LV_BIDI_DIR_INHERIT  = 0x03,

    LV_BIDI_DIR_NEUTRAL  = 0x20,
    LV_BIDI_DIR_WEAK     = 0x21,
};

typedef uint8_t lv_bidi_dir_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
#if LV_USE_BIDI

void lv_bidi_process(const char * str_in, char * str_out, lv_bidi_dir_t base_dir);
void lv_bidi_process_paragraph(const char * str_in, char * str_out, uint32_t len, lv_bidi_dir_t base_dir, uint16_t *pos_conv_out, uint16_t pos_conv_len);
uint32_t lv_bidi_get_next_paragraph(const char * txt);
lv_bidi_dir_t lv_bidi_detect_base_dir(const char * txt);
lv_bidi_dir_t lv_bidi_get_letter_dir(uint32_t letter);
bool lv_bidi_letter_is_weak(uint32_t letter);
bool lv_bidi_letter_is_rtl(uint32_t letter);
bool lv_bidi_letter_is_neutral(uint32_t letter);
uint16_t lv_bidi_get_logical_pos(const char * str_in, char **bidi_txt, uint32_t len, lv_bidi_dir_t base_dir, uint32_t visual_pos, bool *is_rtl);
uint16_t lv_bidi_get_visual_pos(const char * str_in, char **bidi_txt, uint16_t len, lv_bidi_dir_t base_dir, uint32_t logical_pos, bool *is_rtl);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_BIDI*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BIDI_H*/

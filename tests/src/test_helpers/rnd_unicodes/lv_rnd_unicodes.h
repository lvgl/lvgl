/**
* @file lv_rnd_unicodes.h
*
*/
#if LV_BUILD_TEST

#ifndef LV_RND_UNICODES_H
#define LV_RND_UNICODES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

extern const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE[];
extern const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE_LEN;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Generate random UTF-8 characters in the given ranges.
 * @param buf         buffer to store the generated characters
 * @param buf_len     length of the buffer
 * @param ranges      array of ranges to choose from
 * @param range_num   number of ranges
 * @param char_num    number of characters to generate
 * @return number of generated characters in bytes
 */
int lv_random_utf8_chars(uint8_t * buf, int buf_len, const uint32_t * ranges, uint32_t range_num, int char_num);

/*************************
 *    GLOBAL VARIABLES
 *************************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RND_UNICODES_H*/

#endif /*LV_BUILD_TEST*/

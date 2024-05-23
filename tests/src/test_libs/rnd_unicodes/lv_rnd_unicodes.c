/**
* @file lv_rnd_unicodes.c
*
*/
#if LV_BUILD_TEST

/*********************
 *      INCLUDES
 *********************/
#include "lv_rnd_unicodes.h"


#include "rnd_unicodes/src/library.h"

#define UNICODE_MAX_VALUE 0x10FFFF

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE[] = {
    0x4e00, 0x9fa5, // CJK Unified Ideographs
    'A', 'Z',
    'a', 'z',
    '0', '9',
};
const uint32_t LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE_LEN = sizeof(LV_RND_UNICODE_ALPHANUM_AND_CJK_TABLE) / sizeof(
                                                               unicode_t) / 2;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

uint32_t RANDOM_CALL_PROCESS(void)
{
    return lv_rand(0, UNICODE_MAX_VALUE);
}

int lv_random_utf8_chars(uint8_t * buf, int buf_len, const uint32_t * ranges, uint32_t range_num, int char_num)
{
    return random_utf8_chars(buf, buf_len, ranges, range_num, char_num);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_BUILD_TEST*/

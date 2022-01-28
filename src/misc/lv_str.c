/**
 * @file lv_str.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_str.h"
#include "lv_assert.h"
#include "lv_log.h"

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
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

char * lv_strncpy(char * dst, const char * src, size_t count)
{
    LV_ASSERT_VERIFY(dst);
    LV_ASSERT_VERIFY(src);
    LV_ASSERT_VERIFY(0 < count);

    strncpy(dst, src, count - 1);
    dst[count - 1] = '\0';

    return dst;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


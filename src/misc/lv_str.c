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
    LV_ASSERT_VERIFY(count > 0);

    size_t i;
    for(i = 0; i < count && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }

    dst[count] = '\0';

    return dst;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


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

    char *ret = strncpy(dst, src, count);
    
    /* If count is reached before the entire array src was copied, the
     * resulting character array is not null-terminated */
    if (count < strlen(src)) {
        dst[count] = '\0';
    }

    return ret;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


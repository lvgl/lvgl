/**
 * @file lv_string.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_STRING == LV_STDLIB_CLIB
#include "../lv_string.h"
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

LV_ATTRIBUTE_FAST_MEM void * lv_memcpy(void * dst, const void * src, size_t len)
{
    return memcpy(dst, src, len);
}

LV_ATTRIBUTE_FAST_MEM void lv_memset(void * dst, uint8_t v, size_t len)
{
    memset(dst, v, len);
}

size_t lv_strlen(const char * str)
{
    return strlen(str);
}

char * lv_strncpy(char * dst, const char * src, size_t dest_size)
{
    if(dest_size > 0) {
        dst[0] = '\0';
        strncat(dst, src, dest_size - 1);
    }

    return dst;
}

char * lv_strcpy(char * dst, const char * src)
{
    return strcpy(dst, src);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_BUILTIN_MEMCPY*/

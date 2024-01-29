/**
 * @file lv_string.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_STRING == LV_STDLIB_CLIB
#include "../lv_string.h"
#include "../lv_mem.h" /*Need lv_malloc*/
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

void * LV_ATTRIBUTE_FAST_MEM lv_memcpy(void * dst, const void * src, size_t len)
{
    return memcpy(dst, src, len);
}

void LV_ATTRIBUTE_FAST_MEM lv_memset(void * dst, uint8_t v, size_t len)
{
    memset(dst, v, len);
}

void * LV_ATTRIBUTE_FAST_MEM lv_memmove(void * dst, const void * src, size_t len)
{
    return memmove(dst, src, len);
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

int32_t lv_strcmp(const char * s1, const char * s2)
{
    return strcmp(s1, s2);
}

char * lv_strdup(const char * src)
{
    /*strdup uses malloc, so use the lv_malloc when LV_USE_STDLIB_MALLOC is not LV_STDLIB_CLIB */
    size_t len = lv_strlen(src) + 1;
    char * dst = lv_malloc(len);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len); /*do memcpy is faster than strncpy when length is known*/
    return dst;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_CLIB*/

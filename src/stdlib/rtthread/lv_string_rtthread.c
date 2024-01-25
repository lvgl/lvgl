/**
 * @file lv_string_rtthread.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_STRING == LV_STDLIB_RTTHREAD
#include "../lv_string.h"
#include "../lv_mem.h" /*Need lv_malloc*/
#include <rtthread.h>

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
    return rt_memcpy(dst, src, len);
}

void LV_ATTRIBUTE_FAST_MEM lv_memset(void * dst, uint8_t v, size_t len)
{
    rt_memset(dst, v, len);
}

void * LV_ATTRIBUTE_FAST_MEM lv_memmove(void * dst, const void * src, size_t len)
{
    return rt_memmove(dst, src, len);
}

size_t lv_strlen(const char * str)
{
    return rt_strlen(str);
}

char * lv_strncpy(char * dst, const char * src, size_t dest_size)
{
    return rt_strncpy(dst, src, dest_size);
}

char * lv_strcpy(char * dst, const char * src)
{
    return rt_strcpy(dst, src);
}

int32_t lv_strcmp(const char * s1, const char * s2)
{
    return rt_strcmp(s1, s2);
}

char * lv_strdup(const char * src)
{
    /*strdup uses rt_malloc, so use the lv_malloc when LV_USE_STDLIB_MALLOC is not LV_STDLIB_RTTHREAD */
#if LV_USE_STDLIB_MALLOC != LV_STDLIB_RTTHREAD
    size_t len = lv_strlen(src) + 1;
    char * dst = lv_malloc(len);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len); /*do memcpy is faster than strncpy when length is known*/
    return dst;
#else
    return rt_strdup(src);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_RTTHREAD*/

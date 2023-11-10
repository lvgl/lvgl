/**
 * @file lv_string_rtthread.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_STRING == LV_STDLIB_RTTHREAD
#include "../lv_string.h"
#include <rtthread.h>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    #include "../lv_mem.h"
#endif

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
    return rt_memcpy(dst, src, len);
}

LV_ATTRIBUTE_FAST_MEM void lv_memset(void * dst, uint8_t v, size_t len)
{
    rt_memset(dst, v, len);
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

char * lv_strdup(const char * src)
{
    /*strdup uses malloc, so use the built in malloc if it's enabled */
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
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

#endif /*LV_USE_STDLIB_STRING*/

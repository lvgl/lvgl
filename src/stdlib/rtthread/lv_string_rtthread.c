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

size_t lv_strnlen(const char * str, size_t max_len)
{
    size_t i = 0;
    while(i < max_len && str[i]) i++;

    return i;
}

int lv_memcmp(const void * p1, const void * p2, size_t len)
{
    return rt_memcmp(p1, p2, len);
}

size_t lv_strlcpy(char * dst, const char * src, size_t dst_size)
{
    size_t src_len = lv_strlen(src);
    if(dst_size > 0) {
        size_t copy_size = src_len < dst_size ? src_len : dst_size - 1;
        lv_memcpy(dst, src, copy_size);
        dst[copy_size] = '\0';
    }
    return src_len;
}

char * lv_strncpy(char * dst, const char * src, size_t dest_size)
{
    return rt_strncpy(dst, src, dest_size);
}

char * lv_strcpy(char * dst, const char * src)
{
    return rt_strcpy(dst, src);
}

int lv_strcmp(const char * s1, const char * s2)
{
    return rt_strcmp(s1, s2);
}

int lv_strncmp(const char * s1, const char * s2, size_t len)
{
    return rt_strncmp(s1, s2, len);
}

char * lv_strdup(const char * src)
{
    size_t len = lv_strlen(src) + 1;
    char * dst = lv_malloc(len);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len); /*memcpy is faster than strncpy when length is known*/
    return dst;
}

char * lv_strndup(const char * src, size_t max_len)
{
    size_t len = lv_strnlen(src, max_len);
    char * dst = lv_malloc(len + 1);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

char * lv_strcat(char * dst, const char * src)
{
    /*Since RT-thread does not have rt_strcat,
    the following code is used instead.*/
    lv_strcpy(dst + lv_strlen(dst), src);
    return dst;
}

char * lv_strncat(char * dst, const char * src, size_t src_len)
{
    char * tmp = dst;
    dst += lv_strlen(dst);
    while(src_len != 0 && *src != '\0') {
        src_len--;
        *dst++ = *src++;
    }
    *dst = '\0';
    return tmp;
}

char * lv_strchr(const char * s, int c)
{
    for(; ; s++) {
        if(*s == c) {
            return (char *)s;
        }

        if(*s == '\0') {
            break;
        }
    }

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_STDLIB_RTTHREAD*/

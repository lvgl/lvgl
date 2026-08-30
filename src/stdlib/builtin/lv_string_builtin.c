/**
 * @file lv_string_builtin.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl_public.h"

#if LV_USE_STDLIB_STRING == LV_STDLIB_BUILTIN

/*********************
 *      DEFINES
 *********************/
#ifdef LV_ARCH_64
    #define MEM_UNIT         uint64_t
    #define ALIGN_MASK       0x7
#else
    #define MEM_UNIT         uint32_t
    #define ALIGN_MASK       0x3
#endif

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
#if LV_USE_LOG && LV_LOG_TRACE_MEM
    #define LV_TRACE_MEM(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_MEM(...)
#endif

#define _COPY(d, s) *d = *s; d++; s++;
#define _SET(d, v) *d = v; d++;
#define _REPEAT8(expr) expr expr expr expr expr expr expr expr

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * LV_ATTRIBUTE_FAST_MEM lv_memcpy(void * dst, const void * src, size_t len)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    /*The destination is volatile on purpose: it stops the compiler recognizing the byte
     *loops below as memcpy and calling into it. See #7573.*/
    volatile uint8_t * d8 = dst;
    const uint8_t * s8 = src;

    /*Simplify for small memories*/
    if(len < 16) {
        while(len) {
            *d8 = *s8;
            d8++;
            s8++;
            len--;
        }
        return dst;
    }

    lv_uintptr_t d_align = (lv_uintptr_t)d8 & ALIGN_MASK;
    lv_uintptr_t s_align = (lv_uintptr_t)s8 & ALIGN_MASK;

    /*Byte copy for unaligned memories*/
    if(s_align != d_align) {
        while(len >= 32) {
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            _REPEAT8(_COPY(d8, s8));
            len -= 32;
        }
        while(len) {
            _COPY(d8, s8)
            len--;
        }
        return dst;
    }

    /*Make the memories aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            _COPY(d8, s8);
            d_align--;
            len--;
        }
    }

    /*The bulk of the work, in whole words. MEM_UNIT is as wide as the machine, so this is
     *8 bytes at a time on a 64 bit target instead of 4*/
    MEM_UNIT * du = (MEM_UNIT *)(uint8_t *)d8;
    const MEM_UNIT * su = (const MEM_UNIT *)s8;
    while(len >= 8 * sizeof(MEM_UNIT)) {
        _REPEAT8(_COPY(du, su))
        len -= 8 * sizeof(MEM_UNIT);
    }

    /*Whatever is left of a whole word. Without this every length that is a multiple of the
     *block size finished byte by byte*/
    while(len >= sizeof(MEM_UNIT)) {
        _COPY(du, su)
        len -= sizeof(MEM_UNIT);
    }

    d8 = (volatile uint8_t *)du;
    s8 = (const uint8_t *)su;
    while(len) {
        _COPY(d8, s8)
        len--;
    }

    return dst;
}

void LV_ATTRIBUTE_FAST_MEM lv_memset(void * dst, uint8_t v, size_t len)
{
    LV_ASSERT(dst != NULL);

    uint8_t * d8 = (uint8_t *)dst;
    lv_uintptr_t d_align = (lv_uintptr_t) d8 & ALIGN_MASK;

    /*Make the address aligned*/
    if(d_align) {
        d_align = ALIGN_MASK + 1 - d_align;
        while(d_align && len) {
            _SET(d8, v);
            len--;
            d_align--;
        }
    }

    MEM_UNIT vu = (MEM_UNIT)v;
    vu |= vu << 8;
    vu |= vu << 16;
#ifdef LV_ARCH_64
    vu |= vu << 32;
#endif

    MEM_UNIT * du = (MEM_UNIT *)d8;
    while(len >= 8 * sizeof(MEM_UNIT)) {
        _REPEAT8(_SET(du, vu));
        len -= 8 * sizeof(MEM_UNIT);
    }
    while(len >= sizeof(MEM_UNIT)) {
        _SET(du, vu);
        len -= sizeof(MEM_UNIT);
    }

    d8 = (uint8_t *)du;
    while(len) {
        _SET(d8, v);
        len--;
    }
}

void * LV_ATTRIBUTE_FAST_MEM lv_memmove(void * dst, const void * src, size_t len)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    if(dst < src || (char *)dst > ((char *)src + len)) {
        return lv_memcpy(dst, src, len);
    }

    if(dst > src) {
        char * tmp = (char *)dst + len - 1;
        char * s   = (char *)src + len - 1;

        while(len--) {
            *tmp-- = *s--;
        }
    }
    else {
        char * tmp = (char *)dst;
        char * s   = (char *)src;

        while(len--) {
            *tmp++ = *s++;
        }
    }

    return dst;
}

int lv_memcmp(const void * p1, const void * p2, size_t len)
{
    LV_ASSERT(p1 != NULL);
    LV_ASSERT(p2 != NULL);

    const char * s1 = (const char *) p1;
    const char * s2 = (const char *) p2;
    while(--len > 0 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/* See https://en.cppreference.com/w/c/string/byte/strlen for reference */
size_t lv_strlen(const char * str)
{
    LV_ASSERT(str != NULL);

    size_t i = 0;
    while(str[i]) i++;

    return i;
}

size_t lv_strnlen(const char * str, size_t max_len)
{
    LV_ASSERT(str != NULL);

    size_t i = 0;
    while(i < max_len && str[i]) i++;

    return i;
}

size_t lv_strlcpy(char * dst, const char * src, size_t dst_size)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    size_t i = 0;
    if(dst_size > 0) {
        for(; i < dst_size - 1 && src[i]; i++) {
            dst[i] = src[i];
        }
        dst[i] = '\0';
    }
    while(src[i]) i++;
    return i;
}

char * lv_strncpy(char * dst, const char * src, size_t dst_size)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    size_t i;
    for(i = 0; i < dst_size && src[i]; i++) {
        dst[i] = src[i];
    }
    for(; i < dst_size; i++) {
        dst[i] = '\0';
    }
    return dst;
}

char * lv_strcpy(char * dst, const char * src)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    char * tmp = dst;
    while((*dst++ = *src++) != '\0');
    return tmp;
}

int lv_strcmp(const char * s1, const char * s2)
{
    LV_ASSERT(s1 != NULL);
    LV_ASSERT(s2 != NULL);

    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int lv_strncmp(const char * s1, const char * s2, size_t len)
{
    LV_ASSERT(s1 != NULL);
    LV_ASSERT(s2 != NULL);

    if(len == 0) {
        return 0;
    }

    while(len > 0 && *s1 && (*s1 == *s2)) {
        if(--len == 0) {
            return 0;
        }
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char * lv_strdup(const char * src)
{
    LV_ASSERT(src != NULL);

    size_t len = lv_strlen(src) + 1;
    char * dst = lv_malloc(len);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len); /*memcpy is faster than strncpy when length is known*/
    return dst;
}

char * lv_strndup(const char * src, size_t max_len)
{
    LV_ASSERT(src != NULL);

    size_t len = lv_strnlen(src, max_len);
    char * dst = lv_malloc(len + 1);
    if(dst == NULL) return NULL;

    lv_memcpy(dst, src, len);
    dst[len] = '\0';
    return dst;
}

char * lv_strcat(char * dst, const char * src)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    lv_strcpy(dst + lv_strlen(dst), src);
    return dst;
}

char * lv_strncat(char * dst, const char * src, size_t src_len)
{
    LV_ASSERT(dst != NULL);
    LV_ASSERT(src != NULL);

    char * tmp = dst;
    while(*dst != '\0') {
        dst++;
    }
    while(src_len != 0 && *src != '\0') {
        src_len--;
        *dst++ = *src++;
    }
    *dst = '\0';
    return tmp;
}

char * lv_strchr(const char * s, int c)
{
    LV_ASSERT(s != NULL);

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

#endif /*LV_STDLIB_BUILTIN*/

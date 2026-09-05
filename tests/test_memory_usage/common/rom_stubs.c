/**
 * @file rom_stubs.c
 *
 * The Espressif toolchains expect these to come from the chip's ROM, so a bare-metal link
 * that does not pull in the ESP-IDF startup code cannot resolve them. The implementations
 * are the simplest that work; they only need to link, never to run, and they are identical
 * across commits so they do not disturb the measurement.
 */

#include <stddef.h>
#include <stdint.h>

void * memcpy(void * dest, const void * src, size_t len)
{
    uint8_t * d = dest;
    const uint8_t * s = src;
    while(len--) *d++ = *s++;
    return dest;
}

void * memmove(void * dest, const void * src, size_t len)
{
    uint8_t * d = dest;
    const uint8_t * s = src;
    if(d < s) {
        while(len--) *d++ = *s++;
    }
    else {
        d += len;
        s += len;
        while(len--) *--d = *--s;
    }
    return dest;
}

void * memset(void * dest, int c, size_t len)
{
    uint8_t * d = dest;
    while(len--) *d++ = (uint8_t)c;
    return dest;
}

size_t strlen(const char * str)
{
    const char * p = str;
    while(*p) p++;
    return (size_t)(p - str);
}

uint64_t __udivdi3(uint64_t num, uint64_t den)
{
    uint64_t quot = 0;
    uint64_t rem = 0;
    int32_t i;
    for(i = 63; i >= 0; i--) {
        rem = (rem << 1) | ((num >> i) & 1);
        if(rem >= den) {
            rem -= den;
            quot |= (uint64_t)1 << i;
        }
    }
    return quot;
}

uint64_t __umoddi3(uint64_t num, uint64_t den)
{
    return num - __udivdi3(num, den) * den;
}

int64_t __divdi3(int64_t num, int64_t den)
{
    int32_t negate = 0;
    if(num < 0) {
        num = -num;
        negate ^= 1;
    }
    if(den < 0) {
        den = -den;
        negate ^= 1;
    }
    uint64_t quot = __udivdi3((uint64_t)num, (uint64_t)den);
    return negate ? -(int64_t)quot : (int64_t)quot;
}

/**
 * @file lv_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>

#include "lv_utils.h"
#include "lv_math.h"

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

/**
 * Convert a number to string
 * @param num a number
 * @param buf pointer to a `char` buffer. The result will be stored here (max 10 elements)
 * @return same as `buf` (just for convenience)
 */
char * lv_utils_num_to_str(int32_t num, char * buf)
{
    if(num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }
    int8_t digitCount = 0;
    int8_t i          = 0;
    if(num < 0) {
        buf[digitCount++] = '-';
        num               = LV_MATH_ABS(num);
        ++i;
    }
    while(num) {
        char digit        = num % 10;
        buf[digitCount++] = digit + 48;
        num /= 10;
    }
    buf[digitCount] = '\0';
    digitCount--;
    while(digitCount > i) {
        char temp       = buf[i];
        buf[i]          = buf[digitCount];
        buf[digitCount] = temp;
        digitCount--;
        i++;
    }
    return buf;
}

/**
 * Convert a fixed point number to string
 * @param num a number
 * @param number of digits after decimal point
 * @param buf pointer to a `char` buffer. Buffer must be big enough for number.
 * @return same as `buf` (just for convenience)
 */
char * lv_utils_num_to_str_fixed(int32_t num, int decimals, char * buf)
{
    uint16_t dec_power = 1;
    for(int i=0; i<decimals; i++)
        dec_power *= 10;

    lv_utils_num_to_str(num / dec_power, buf);
    if(decimals > 0) {
        /*find end of string*/
        char * p = buf;
        while(*p != 0)
            p++;
        *p++ = '.';

        /*now work backwards from end*/
        num = num % dec_power;
        p += decimals;
        *p-- = '\x0';

        while(decimals > 0) {
            char digit = num % 10;
            *p-- = digit + 48;
            decimals--;
            num = num / 10;
        }
    }
    return buf;
}

/** Searches base[0] to base[n - 1] for an item that matches *key.
 *
 * @note The function cmp must return negative if its first
 *  argument (the search key) is less that its second (a table entry),
 *  zero if equal, and positive if greater.
 *
 *  @note Items in the array must be in ascending order.
 *
 * @param key    Pointer to item being searched for
 * @param base   Pointer to first element to search
 * @param n      Number of elements
 * @param size   Size of each element
 * @param cmp    Pointer to comparison function (see #lv_font_codeCompare as a comparison function
 * example)
 *
 * @return a pointer to a matching item, or NULL if none exists.
 */
void * lv_utils_bsearch(const void * key, const void * base, uint32_t n, uint32_t size,
                        int32_t (*cmp)(const void * pRef, const void * pElement))
{
    const char * middle;
    int32_t c;

    for(middle = base; n != 0;) {
        middle += (n / 2) * size;
        if((c = (*cmp)(key, middle)) > 0) {
            n    = (n / 2) - ((n & 1) == 0);
            base = (middle += size);
        }
        else if(c < 0) {
            n /= 2;
            middle = base;
        }
        else {
            return (char *)middle;
        }
    }
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

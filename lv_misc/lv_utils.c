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
    char * buf_ori = buf;
    if(num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    } else if(num < 0) {
        (*buf) = '-';
        buf++;
        num = LV_MATH_ABS(num);
    }
    uint32_t output = 0;
    int8_t i;

    for(i = 31; i >= 0; i--) {
        if((output & 0xF) >= 5)
            output += 3;
        if(((output & 0xF0) >> 4) >= 5)
            output += (3 << 4);
        if(((output & 0xF00) >> 8) >= 5)
            output += (3 << 8);
        if(((output & 0xF000) >> 12) >= 5)
            output += (3 << 12);
        if(((output & 0xF0000) >> 16) >= 5)
            output += (3 << 16);
        if(((output & 0xF00000) >> 20) >= 5)
            output += (3 << 20);
        if(((output & 0xF000000) >> 24) >= 5)
            output += (3 << 24);
        if(((output & 0xF0000000) >> 28) >= 5)
            output += (3 << 28);
        output = (output << 1) | ((num >> i) & 1);
    }

    uint8_t digit;
    bool leading_zero_ready = false;
    for(i = 28; i >= 0; i -= 4) {
        digit = ((output >> i) & 0xF) + '0';
        if(digit == '0' && leading_zero_ready == false) continue;

        leading_zero_ready = true;
        (*buf) = digit;
        buf++;
    }

    (*buf) = '\0';

    return buf_ori;
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
 * @param cmp    Pointer to comparison function (see #lv_font_codeCompare as a comparison function example)
 *
 * @return a pointer to a matching item, or NULL if none exists.
 */
void * lv_utils_bsearch(const void * key, const void * base, uint32_t n, uint32_t size, int32_t (* cmp)(const void * pRef, const void * pElement))
{
    const char * middle;
    int32_t c;

    for (middle = base; n != 0;) {
        middle += (n/2) * size;
        if ((c = (*cmp)(key, middle)) > 0) {
            n = (n/2) - ((n&1) == 0);
            base = (middle += size);
        } else if (c < 0) {
            n /= 2;
            middle = base;
        } else {
            return (char *) middle;
        }
    }
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/



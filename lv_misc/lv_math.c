/**
 * @file lv_math.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_math.h"
#include <stdbool.h>

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
char * lv_math_num_to_str(int32_t num, char * buf)
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

    for(i = 31; i >= 0; i--){
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

/**********************
 *   STATIC FUNCTIONS
 **********************/



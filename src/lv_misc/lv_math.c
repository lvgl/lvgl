/**
 * @file lv_math.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_math.h"
#include <stdbool.h>
#include <stdlib.h>
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
static int16_t sin0_90_table[] = {
    0,     572,   1144,  1715,  2286,  2856,  3425,  3993,  4560,  5126,  5690,  6252,  6813,  7371,  7927,  8481,
    9032,  9580,  10126, 10668, 11207, 11743, 12275, 12803, 13328, 13848, 14364, 14876, 15383, 15886, 16383, 16876,
    17364, 17846, 18323, 18794, 19260, 19720, 20173, 20621, 21062, 21497, 21925, 22347, 22762, 23170, 23571, 23964,
    24351, 24730, 25101, 25465, 25821, 26169, 26509, 26841, 27165, 27481, 27788, 28087, 28377, 28659, 28932, 29196,
    29451, 29697, 29934, 30162, 30381, 30591, 30791, 30982, 31163, 31335, 31498, 31650, 31794, 31927, 32051, 32165,
    32269, 32364, 32448, 32523, 32587, 32642, 32687, 32722, 32747, 32762, 32767};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
int16_t lv_trigo_sin(int16_t angle)
{
    int16_t ret = 0;
    angle       = angle % 360;

    if(angle < 0) angle = 360 + angle;

    if(angle < 90) {
        ret = sin0_90_table[angle];
    } else if(angle >= 90 && angle < 180) {
        angle = 180 - angle;
        ret   = sin0_90_table[angle];
    } else if(angle >= 180 && angle < 270) {
        angle = angle - 180;
        ret   = -sin0_90_table[angle];
    } else { /*angle >=270*/
        angle = 360 - angle;
        ret   = -sin0_90_table[angle];
    }

    return ret;
}

/**
 * Calculate a value of a Cubic Bezier function.
 * @param t time in range of [0..LV_BEZIER_VAL_MAX]
 * @param u0 start values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u1 control value 1 values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u2 control value 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param u3 end values in range of [0..LV_BEZIER_VAL_MAX]
 * @return the value calculated from the given parameters in range of [0..LV_BEZIER_VAL_MAX]
 */
int32_t lv_bezier3(uint32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3)
{
    uint32_t t_rem  = 1024 - t;
    uint32_t t_rem2 = (t_rem * t_rem) >> 10;
    uint32_t t_rem3 = (t_rem2 * t_rem) >> 10;
    uint32_t t2     = (t * t) >> 10;
    uint32_t t3     = (t2 * t) >> 10;

    uint32_t v1 = ((uint32_t)t_rem3 * u0) >> 10;
    uint32_t v2 = ((uint32_t)3 * t_rem2 * t * u1) >> 20;
    uint32_t v3 = ((uint32_t)3 * t_rem * t2 * u2) >> 20;
    uint32_t v4 = ((uint32_t)t3 * u3) >> 10;

    return v1 + v2 + v3 + v4;
}
#define BITSPERLONG 32

#define TOP2BITS(x) ((x & (3L << (BITSPERLONG-2))) >> (BITSPERLONG-2))

void lv_sqrt(uint32_t x, lv_sqrt_res_t * q)
{

    if(x == 0) {
        q->f = 0;
        q->i = 0;
        return;
    }

    /*Look up for x=1..64*/
    static const uint8_t ci[] = {1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
                                 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6,
                                 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7,
                                 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, };

    static const uint8_t cf[] = {0,   106, 187, 0,   60,  115, 165, 212, 0,   42,  81,  119, 155, 190, 223, 0,
                                 32,  62,  92,  121, 149, 177, 204, 230, 0,   25,  50,  75,  99,  122, 145, 168,
                                 191, 213, 235, 0,   21,  42,  63,  83,  103, 123, 143, 162, 181, 200, 219, 238,
                                 0,   18,  36,  54,  72,  89, 107,  124, 141, 158, 174, 191, 207, 224, 240, 0, };

    if(x <= 64) {
        x--;
        q->i = ci[x];
        q->f = cf[x];
        return;
    }

    /*
     * Source:
     * http://web.archive.org/web/20080303101624/http://c.snippets.org/snip_lister.php?fname=isqrt.c
     * https://stackoverflow.com/questions/1100090/looking-for-an-efficient-integer-square-root-algorithm-for-arm-thumb2
     */

    uint32_t a = 0L;                   /* accumulator      */
    uint32_t r = 0L;                   /* remainder        */
    uint32_t e = 0L;                   /* trial product    */

    uint32_t i;
    for (i = 0; i < BITSPERLONG / 2 + 8; i++) {
          r = (r << 2) + TOP2BITS(x); x <<= 2;
          a <<= 1;
          e = (a << 1) + 1;
          if (r >= e)  {
                r -= e;
                a++;
          }
    }

    q->f = a & 0xFF;
    q->i = a >> 8;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

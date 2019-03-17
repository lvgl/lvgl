/**
 * @file math_base.h
 *
 */

#ifndef LV_MATH_H
#define LV_MATH_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_MATH_MIN(a,b) ((a) < (b) ? (a) : (b))
#define LV_MATH_MAX(a,b) ((a) > (b) ? (a) : (b))
#define LV_MATH_ABS(x) ((x) > 0 ? (x) : (-(x)))

#define LV_TRIGO_SIN_MAX    32767
#define LV_TRIGO_SHIFT      15      /* >> LV_TRIGO_SHIFT to normalize*/

#define LV_BEZIER_VAL_MAX       1024    /*Max time in Bezier functions (not [0..1] to use integers) */
#define LV_BEZIER_VAL_SHIFT     10      /*log2(LV_BEZIER_VAL_MAX): used to normalize up scaled values*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Convert a number to string
 * @param num a number
 * @param buf pointer to a `char` buffer. The result will be stored here (max 10 elements)
 * @return same as `buf` (just for convenience)
 */
char * lv_math_num_to_str(int32_t num, char * buf);

/**
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
int16_t lv_trigo_sin(int16_t angle);

/**
 * Calculate a value of a Cubic Bezier function.
 * @param t time in range of [0..LV_BEZIER_VAL_MAX]
 * @param u0 start values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u1 control value 1 values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u2 control value 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param u3 end values in range of [0..LV_BEZIER_VAL_MAX]
 * @return the value calculated from the given parameters in range of [0..LV_BEZIER_VAL_MAX]
 */
int32_t lv_bezier3(uint32_t t, int32_t u0, int32_t u1, int32_t u2, int32_t u3);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

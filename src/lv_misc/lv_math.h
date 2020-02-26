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
#define LV_MATH_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LV_MATH_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LV_MATH_ABS(x) ((x) > 0 ? (x) : (-(x)))

#define LV_IS_SIGNED(t) (((t)(-1)) < ((t) 0))
#define LV_UMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_SMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0x7ULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_MAX_OF(t) ((unsigned long) (LV_IS_SIGNED(t) ? LV_SMAX_OF(t) : LV_UMAX_OF(t)))

#define LV_TRIGO_SIN_MAX 32767
#define LV_TRIGO_SHIFT 15 /**<  >> LV_TRIGO_SHIFT to normalize*/

#define LV_BEZIER_VAL_MAX 1024 /**< Max time in Bezier functions (not [0..1] to use integers) */
#define LV_BEZIER_VAL_SHIFT 10 /**< log2(LV_BEZIER_VAL_MAX): used to normalize up scaled values*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

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

/**
 * Calculate the atan2 of a vector.
 * @param x
 * @param y
 * @return the angle in degree calculated from the given parameters in range of [0..360]
 */
uint16_t lv_atan2(int x, int y);

/**
 * Calculate the integer square root of a number.
 * @param num
 * @return square root of 'num'
 */
uint32_t lv_sqrt(uint32_t num);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

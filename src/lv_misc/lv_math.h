/**
 * @file lv_math.h
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
#include "../lv_conf_internal.h"
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define LV_MATH_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LV_MATH_MIN3(a, b, c) (LV_MATH_MIN(LV_MATH_MIN(a,b), c))
#define LV_MATH_MIN4(a, b, c, d) (LV_MATH_MIN(LV_MATH_MIN(a,b), LV_MATH_MIN(c,d)))

#define LV_MATH_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LV_MATH_MAX3(a, b, c) (LV_MATH_MAX(LV_MATH_MAX(a,b), c))
#define LV_MATH_MAX4(a, b, c, d) (LV_MATH_MAX(LV_MATH_MAX(a,b), LV_MATH_MAX(c,d)))

#define LV_MATH_ABS(x) ((x) > 0 ? (x) : (-(x)))

#define LV_MATH_UDIV255(x) ((uint32_t)((uint32_t) (x) * 0x8081) >> 0x17)

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

typedef struct {
    uint16_t i;
    uint16_t f;
} lv_sqrt_res_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

//! @cond Doxygen_Suppress
/**
 * Return with sinus of an angle
 * @param angle
 * @return sinus of 'angle'. sin(-90) = -32767, sin(90) = 32767
 */
LV_ATTRIBUTE_FAST_MEM int16_t _lv_trigo_sin(int16_t angle);

//! @endcond

/**
 * Calculate a value of a Cubic Bezier function.
 * @param t time in range of [0..LV_BEZIER_VAL_MAX]
 * @param u0 start values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u1 control value 1 values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u2 control value 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param u3 end values in range of [0..LV_BEZIER_VAL_MAX]
 * @return the value calculated from the given parameters in range of [0..LV_BEZIER_VAL_MAX]
 */
uint32_t _lv_bezier3(uint32_t t, uint32_t u0, uint32_t u1, uint32_t u2, uint32_t u3);

/**
 * Calculate the atan2 of a vector.
 * @param x
 * @param y
 * @return the angle in degree calculated from the given parameters in range of [0..360]
 */
uint16_t _lv_atan2(int x, int y);

//! @cond Doxygen_Suppress

/**
 * Get the square root of a number
 * @param x integer which square root should be calculated
 * @param q store the result here. q->i: integer part, q->f: fractional part in 1/256 unit
 * @param mask: optional to skip some iterations if the magnitude of the root is known.
 * Set to 0x8000 by default.
 * If root < 16: mask = 0x80
 * If root < 256: mask = 0x800
 * Else: mask = 0x8000
 */
LV_ATTRIBUTE_FAST_MEM void _lv_sqrt(uint32_t x, lv_sqrt_res_t * q, uint32_t mask);

//! @endcond

/**
 * Calculate the integer exponents.
 * @param base
 * @param power
 * @return base raised to the power exponent
 */
int64_t _lv_pow(int64_t base, int8_t exp);

/**
 * Get the mapped of a number given an input and output range
 * @param x integer which mapped value should be calculated
 * @param min_in min input range
 * @param max_in max input range
 * @param min_out max output range
 * @param max_out max output range
 * @return the mapped number
 */
int32_t _lv_map(int32_t x, int32_t min_in, int32_t max_in, int32_t min, int32_t max);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

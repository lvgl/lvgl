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
#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TRIGO_SIN_MAX 32768
#define LV_TRIGO_SHIFT 15 /**<  >> LV_TRIGO_SHIFT to normalize*/

#define LV_BEZIER_VAL_SHIFT 10 /**< log2(LV_BEZIER_VAL_MAX): used to normalize up scaled values*/
#define LV_BEZIER_VAL_MAX (1L << LV_BEZIER_VAL_SHIFT) /**< Max time in Bezier functions (not [0..1] to use integers)*/
#define LV_BEZIER_VAL_FLOAT(f) ((int32_t)((f) * LV_BEZIER_VAL_MAX)) /**< Convert const float number cubic-bezier values to fix-point value*/

/** Align up value x to align, align must be a power of two */
#define LV_ALIGN_UP(x, align) (((x) + ((align) - 1)) & ~((align) - 1))

/** Round up value x to round, round can be any integer number */
#define LV_ROUND_UP(x, round) ((((x) + ((round) - 1)) / (round)) * (round))

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
int32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_trigo_sin(int16_t angle);

int32_t LV_ATTRIBUTE_FAST_MEM lv_trigo_cos(int16_t angle);

//! @endcond

/**
 * Calculate the y value of cubic-bezier(x1, y1, x2, y2) function as specified x.
 * @param x time in range of [0..LV_BEZIER_VAL_MAX]
 * @param x1 x of control point 1 in range of [0..LV_BEZIER_VAL_MAX]
 * @param y1 y of control point 1 in range of [0..LV_BEZIER_VAL_MAX]
 * @param x2 x of control point 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param y2 y of control point 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @return the value calculated
 */
int32_t lv_cubic_bezier(int32_t x, int32_t x1, int32_t y1, int32_t x2, int32_t y2);

/**
 * Calculate a value of a Cubic Bezier function.
 * @param t time in range of [0..LV_BEZIER_VAL_MAX]
 * @param u0 must be 0
 * @param u1 control value 1 values in range of [0..LV_BEZIER_VAL_MAX]
 * @param u2 control value 2 in range of [0..LV_BEZIER_VAL_MAX]
 * @param u3 must be LV_BEZIER_VAL_MAX
 * @return the value calculated from the given parameters in range of [0..LV_BEZIER_VAL_MAX]
 */
int32_t lv_bezier3(int32_t t, int32_t u0, uint32_t u1, int32_t u2, int32_t u3);


/**
 * Calculate the atan2 of a vector.
 * @param x
 * @param y
 * @return the angle in degree calculated from the given parameters in range of [0..360]
 */
uint16_t lv_atan2(int x, int y);

//! @cond Doxygen_Suppress

/**
 * Get the square root of a number
 * @param x integer which square root should be calculated
 * @param q store the result here. q->i: integer part, q->f: fractional part in 1/256 unit
 * @param mask optional to skip some iterations if the magnitude of the root is known.
 * Set to 0x8000 by default.
 * If root < 16: mask = 0x80
 * If root < 256: mask = 0x800
 * Else: mask = 0x8000
 */
void /* LV_ATTRIBUTE_FAST_MEM */ lv_sqrt(uint32_t x, lv_sqrt_res_t * q, uint32_t mask);

//! @endcond

/**
 * Alternative (fast, approximate) implementation for getting the square root of an integer.
 * @param x integer which square root should be calculated
 */
int32_t /* LV_ATTRIBUTE_FAST_MEM */ lv_sqrt32(uint32_t x);

/**
 * Calculate the square of an integer (input range is 0..32767).
 * @param x input
 * @return square
 */
static inline int32_t lv_sqr(int32_t x)
{
    return x * x;
}

/**
 * Calculate the integer exponents.
 * @param base
 * @param exp
 * @return base raised to the power exponent
 */
int64_t lv_pow(int64_t base, int8_t exp);

/**
 * Get the mapped of a number given an input and output range
 * @param x integer which mapped value should be calculated
 * @param min_in min input range
 * @param max_in max input range
 * @param min_out max output range
 * @param max_out max output range
 * @return the mapped number
 */
int32_t lv_map(int32_t x, int32_t min_in, int32_t max_in, int32_t min_out, int32_t max_out);

/**
 * Set the seed of the pseudo random number generator
 * @param seed a number to initialize the random generator
 */
void lv_rand_set_seed(uint32_t seed);

/**
 * Get a pseudo random number in the given range
 * @param min   the minimum value
 * @param max   the maximum value
 * @return return the random number. min <= return_value <= max
 */
uint32_t lv_rand(uint32_t min, uint32_t max);

/**********************
 *      MACROS
 **********************/
#define LV_MIN(a, b) ((a) < (b) ? (a) : (b))
#define LV_MIN3(a, b, c) (LV_MIN(LV_MIN(a,b), c))
#define LV_MIN4(a, b, c, d) (LV_MIN(LV_MIN(a,b), LV_MIN(c,d)))

#define LV_MAX(a, b) ((a) > (b) ? (a) : (b))
#define LV_MAX3(a, b, c) (LV_MAX(LV_MAX(a,b), c))
#define LV_MAX4(a, b, c, d) (LV_MAX(LV_MAX(a,b), LV_MAX(c,d)))

#define LV_CLAMP(min, val, max) (LV_MAX(min, (LV_MIN(val, max))))

#define LV_ABS(x) ((x) > 0 ? (x) : (-(x)))
#define LV_UDIV255(x) (((x) * 0x8081U) >> 0x17)

#define LV_IS_SIGNED(t) (((t)(-1)) < ((t)0))
#define LV_UMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0xFULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_SMAX_OF(t) (((0x1ULL << ((sizeof(t) * 8ULL) - 1ULL)) - 1ULL) | (0x7ULL << ((sizeof(t) * 8ULL) - 4ULL)))
#define LV_MAX_OF(t) ((unsigned long)(LV_IS_SIGNED(t) ? LV_SMAX_OF(t) : LV_UMAX_OF(t)))

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

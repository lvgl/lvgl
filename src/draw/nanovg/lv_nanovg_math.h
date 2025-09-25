/**
 * @file lv_nanovg_math.h
 *
 */

#ifndef LV_NANOVG_MATH_H
#define LV_NANOVG_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_NANOVG

#include <math.h>
#include <stdbool.h>
#include <float.h>

/*********************
 *      DEFINES
 *********************/

#define NVG_MATH_PI  3.14159265358979323846f
#define NVG_MATH_HALF_PI 1.57079632679489661923f
#define NVG_MATH_TWO_PI 6.28318530717958647692f
#define NVG_DEG_TO_RAD 0.017453292519943295769236907684886f
#define NVG_RAD_TO_DEG 57.295779513082320876798154814105f

#define NVG_MATH_TANF(x) tanf(x)
#define NVG_MATH_SINF(x) sinf(x)
#define NVG_MATH_COSF(x) cosf(x)
#define NVG_MATH_ASINF(x) asinf(x)
#define NVG_MATH_ACOSF(x) acosf(x)
#define NVG_MATH_FABSF(x) fabsf(x)
#define NVG_MATH_SQRTF(x) sqrtf(x)

#define NVG_MATH_RADIANS(deg) ((deg) * NVG_DEG_TO_RAD)
#define NVG_MATH_DEGREES(rad) ((rad) * NVG_RAD_TO_DEG)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline bool nvg_math_is_zero(float a)
{
    return (NVG_MATH_FABSF(a) < FLT_EPSILON);
}

static inline bool nvg_math_is_equal(float a, float b)
{
    return nvg_math_is_zero(a - b);
}

static inline float nvg_math_inv_sqrtf(float number)
{
    /* From https://en.wikipedia.org/wiki/Fast_inverse_square_root#Avoiding_undefined_behavior */
    union {
        float   f;
        int32_t i;
    } conv = { .f = number };
    conv.i  = 0x5f3759df - (conv.i >> 1);
    conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
    return conv.f;
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_NANOVG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NANOVG_MATH_H*/

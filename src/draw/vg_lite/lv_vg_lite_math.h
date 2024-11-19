/**
 * @file lv_vg_lite_math.h
 *
 */

#ifndef LV_VG_LITE_MATH_H
#define LV_VG_LITE_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VG_LITE

#include <math.h>
#include <stdbool.h>
#include <float.h>

/*********************
 *      DEFINES
 *********************/

#define MATH_PI  3.14159265358979323846f
#define MATH_HALF_PI 1.57079632679489661923f
#define MATH_TWO_PI 6.28318530717958647692f
#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

#define MATH_TANF(x) tanf(x)
#define MATH_SINF(x) sinf(x)
#define MATH_COSF(x) cosf(x)
#define MATH_ASINF(x) asinf(x)
#define MATH_ACOSF(x) acosf(x)
#define MATH_FABSF(x) fabsf(x)
#define MATH_SQRTF(x) sqrtf(x)

#define MATH_RADIANS(deg) ((deg) * DEG_TO_RAD)
#define MATH_DEGREES(rad) ((rad) * RAD_TO_DEG)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static inline bool math_zero(float a)
{
    return (MATH_FABSF(a) < FLT_EPSILON);
}

static inline bool math_equal(float a, float b)
{
    return math_zero(a - b);
}

float math_fast_inv_sqrtf(float number);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_VG_LITE_MATH_H*/

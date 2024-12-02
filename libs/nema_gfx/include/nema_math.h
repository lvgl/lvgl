/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon Single Member PC be liable for any claim, damages or other
 *  liability, whether in an action of contract, tort or otherwise, arising
 *  from, out of or in connection with the software or the use or other dealings
 *  in the software.
 ******************************************************************************/


#ifndef NEMA_MATH_H__
#define NEMA_MATH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NEMA_E          2.71828182845904523536f  /**< e */
#define NEMA_LOG2E      1.44269504088896340736f  /**< log2(e) */
#define NEMA_LOG10E     0.434294481903251827651f /**< log10(e) */
#define NEMA_LN2        0.693147180559945309417f /**< ln(2) */
#define NEMA_LN10       2.30258509299404568402f  /**< ln(10) */
#define NEMA_PI         3.14159265358979323846f  /**< pi */
#define NEMA_PI_2       1.57079632679489661923f  /**< pi/2 */
#define NEMA_PI_4       0.785398163397448309616f /**< pi/4 */
#define NEMA_1_PI       0.318309886183790671538f /**< 1/pi */
#define NEMA_2_PI       0.636619772367581343076f /**< 2/pi */
#define NEMA_2_SQRTPI   1.12837916709551257390f  /**< 2/sqrt(pi) */
#define NEMA_SQRT2      1.41421356237309504880f  /**< sqrt(2) */
#define NEMA_SQRT1_2    0.707106781186547524401f /**< 1/sqrt(2) */

/** \brief Fast sine approximation of a given angle
 *
 * \param angle_degrees Angle in degrees
 * \return Sine of the given angle
 *
 */
float nema_sin(float angle_degrees);


/** \brief Fast cosine approximation of a given angle
 *
 * \param angle_degrees Angle in degrees
 * \return Cosine of the given angle
 *
 */
float nema_cos(float angle_degrees);

/** \brief Fast tangent approximation of a given angle
 *
 * \param angle_degrees Angle in degrees
 * \return Tangent of the given angle
 *
 */
float nema_tan(float angle_degrees);


/** \brief Fast sine approximation of a given angle
 *
 * \param angle_radians Angle in radians
 * \return Sine of the given angle
 *
 */
float nema_sin_r(float angle_radians);


/** \brief Fast cosine approximation of a given angle
 *
 * \param angle_radians Angle in radians
 * \return Cosine of the given angle
 *
 */
float nema_cos_r(float angle_radians);

/** \brief Fast tangent approximation of a given angle
 *
 * \param angle_radians Angle in radians
 * \return Tangent of the given angle
 *
 */
float nema_tan_r(float angle_radians);

/** \brief Fast arc tangent approximation of a y/x
 *
 * \param y value
 * \param x value
 * \return Arc tangent of the given y/x in degrees
 *
 */
float nema_atan2(float y, float x);

/** \brief Fast arc tangent approximation of a y/x
 *
 * \param y value
 * \param x value
 * \return Arc tangent of the given y/x in radians
 *
 */
float nema_atan2_r(float y, float x);

/** \brief A rough approximation of x raised to the power of y. USE WITH CAUTION!
 *
 * \param x base value. Must be non negative.
 * \param y power value
 * \return the result of raising x to the power y
 *
 */
float nema_pow(float x, float y);

/** \brief A rough approximation of the square root of x. USE WITH CAUTION!
 *
 * \param x X value. Must be non negative
 * \param
 * \return The square root of x
 *
 */
float nema_sqrt(float x);


/** \brief A floating-point approximation of the inverse tangent of x
 *
 * \param x X value
 * \return Inverse tangent (angle) of x in degrees
 *
 */
float nema_atan(float x);

/** \brief Find the minimum of two values
 *
 * \param a First value
 * \param b Second value
 * \return The minimum of a and b
 *
 */
#define nema_min2(a,b) (((a)<(b))?( a):(b))

/** \brief Find the maximum of two values
 *
 * \param a First value
 * \param b Second value
 * \return The maximum of a and b
 *
 */
#define nema_max2(a,b) (((a)>(b))?( a):(b))

/** \brief Clamp value
 *
 * \param val Value to clamp
 * \param min Minimum value
 * \param max Minimum value
 * \return Clamped value
 *
 */
#define nema_clamp(val, min, max) nema_min2((max), nema_max2((min), (val)))

/** \brief Calculate the absolute value of int
 *
 * \param a Value
 * \return The absolute value of a
 *
 */
#define nema_abs(a)    (((a)< 0 )?(-(a)):(a))

/** \brief Calculate the absolute value of float
 *
 * \param a Value
 * \return The absolute value of a
 *
 */
#define nema_absf(a)    (((a)< 0.f )?(-(a)):(a))


/** \brief Compare two floats
 *
 * \param x First float
 * \param y Second float
 * \return 1 if x == y, 0 if x != y
 *
 */
#define nema_floats_equal(x, y) (nema_absf((x) - (y)) <= 0.00001f * nema_min2(nema_absf(x), nema_absf(y)))

/** \brief Checks if value x is zero
 *
 * \param x X value
 * \return 1 if x == 0, 0 if x != 0
 *
 */
#define nema_float_is_zero(x) (nema_absf(x) <= 0.00001f)

/** \brief Convert degrees to radians
 *
 * \param d Angle in degrees
 * \return Angle in radians
 *
 */
#define nema_deg_to_rad(d) (0.0174532925199f * (d))  //rad = deg * pi / 180

/** \brief Convert radians to degries
 *
 * \param r Angle in radians
 * \return Angle in degrees
 *
 */
#define nema_rad_to_deg(r) (57.295779513f    * (r))  //deg = rad * 180 / pi

/** \brief Convert integer to 16.16 fixed point
 *
 * \param a Value to be converted
 * \return 16.16 fixed point value
 *
 */
#define nema_i2fx(a)   ((a)*0x10000)

/** \brief Convert float to 16.16 fixed point
 *
 * \param a Value to be converted
 * \return 16.16 fixed point value
 *
 */
// #define nema_f2fx(a)   ((int)(((a)*((float)0x10000)+0.5f)))
int nema_f2fx(float f); //   ((int)(((a)*((float)0x10000)+0.5f)))

/** \brief Floor function
 *
 * \param a Value to be floored
 * \return floored value
 *
 */
#define nema_floor(f) ((int)(f) - ( (int)(f) > (f) ))

/** \brief Ceiling function
 *
 * \param a Value to be ceiled
 * \return ceiled value
 *
 */
#define nema_ceil(f)  ((int)(f) + ( (int)(f) < (f) ))

/** \brief Truncate function
 *
 * \param x Value to be truncated
 * \return truncated value
 *
 */
#define nema_truncf(x) (x < 0.0f ? nema_ceil(x) : nema_floor(x))

/** \brief Float Modulo function
 *
 * \param x Dividend
 * \param y Divisor
 * \return Remainder
 *
 */
#define nema_fmod(x, y) ( (x) - nema_truncf( ( (x) / (y) ) ) * (y) )

#ifdef __cplusplus
}
#endif

#endif

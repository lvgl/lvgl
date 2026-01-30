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


#ifndef NEMA_EASING_H__
#define NEMA_EASING_H__

#ifdef __cplusplus
extern "C" {
#endif

//Linear

// Modeled after the line y = x
/** \brief Linear easing, no acceleration
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_linear(float p);

//Quadratic

// Modeled after the parabola y = x^2
/** \brief Quadratic easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quad_in(float p);

// Modeled after the parabola y = -x^2 + 2x
/** \brief Quadratic easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quad_out(float p);

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
/** \brief Quadratic easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quad_in_out(float p);

//Cubic

// Modeled after the cubic y = x^3
/** \brief Cubic easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_cub_in(float p);

// Modeled after the cubic y = (x - 1)^3 + 1
/** \brief Cubic easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_cub_out(float p);

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
/** \brief Cubic easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_cub_in_out(float p);

//Quartic

// Modeled after the quartic x^4
/** \brief Quartic easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quar_in(float p);

// Modeled after the quartic y = 1 - (x - 1)^4
/** \brief Quartic easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quar_out(float p);

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
/** \brief Quartic easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quar_in_out(float p);

//Quintic

// Modeled after the quintic y = x^5
/** \brief Quintic easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quin_in(float p);

// Modeled after the quintic y = (x - 1)^5 + 1
/** \brief Quintic easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quin_out(float p);

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
/** \brief Quintic easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_quin_in_out(float p);

//Sin

// Modeled after quarter-cycle of sine wave
/** \brief Sinusoidal easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_sin_in(float p);

// Modeled after quarter-cycle of sine wave (different phase)
/** \brief Sinusoidal easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_sin_out(float p);

// Modeled after half sine wave
/** \brief Sinusoidal easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_sin_in_out(float p);

//Circular

// Modeled after shifted quadrant IV of unit circle
/** \brief Circular easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_circ_in(float p);

// Modeled after shifted quadrant II of unit circle
/** \brief Circular easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_circ_out(float p);

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
/** \brief Circular easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_circ_in_out(float p);

//Exponential

// Modeled after the exponential function y = 2^(10(x - 1))
/** \brief Exponential easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_exp_in(float p);

// Modeled after the exponential function y = -2^(-10x) + 1
/** \brief Exponential easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_exp_out(float p);

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
/** \brief Exponential easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_exp_in_out(float p);

//Elastic
// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))

/** \brief Elastic easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_elast_in(float p);

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
/** \brief Elastic easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_elast_out(float p);

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
/** \brief Elastic easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_elast_in_out(float p);

//Back

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
/** \brief Overshooting easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_back_in(float p);

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
/** \brief Overshooting easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_back_out(float p);

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
/** \brief Overshooting easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_back_in_out(float p);

//Bounce

/** \brief Bouncing easing in, accelerate from zero
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_bounce_out(float p);

/** \brief Bouncing easing out, decelerate to zero velocity
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_bounce_in(float p);

/** \brief Bouncing easing in and out, accelerate to halfway, then decelerate
 *
 * \param p Input value, typically within the [0, 1] range
 * \return Eased value
 *
 */
float nema_ez_bounce_in_out(float p);


/** \brief Convenience function to perform easing between two values given number of steps, current step and easing function
 *
 * \param A         Initial value within range [0, 1]
 * \param B         Finale value within range [0, 1]
 * \param steps     Total number of steps
 * \param cur_step  Current Step
 * \param ez_func   pointer to the desired easing function
 * \return Eased value
 *
 */


float nema_ez(float A, float B, float steps, float cur_step, float (*ez_func)(float p));

#ifdef __cplusplus
}
#endif

#endif

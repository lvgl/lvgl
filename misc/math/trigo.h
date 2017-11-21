/**
 * @file trig.h
 * Basic trigonometrical integer functions
 */

#ifndef TRIGO_H
#define TRIGO_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_TRIGO != 0

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/
#define TRIGO_SIN_MAX 32767

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
int16_t trigo_sin(int16_t angle);

/**********************
 *      MACROS
 **********************/

#endif /*USE_TRIGO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

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
#include "misc_conf.h"

/*********************
 *      DEFINES
 *********************/

#define LV_MATH_MIN(a,b) (a<b?a:b)
#define LV_MATH_MAX(a,b) (a>b?a:b)
#define LV_MATH_ABS(x) ((x)>0?(x):(-(x)))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

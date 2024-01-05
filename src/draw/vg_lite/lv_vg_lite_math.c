/**
 * @file lv_vg_lite_math.h
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_math.h"

#if LV_USE_DRAW_VG_LITE

#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

float math_fast_inv_sqrtf(float number)
{
    int32_t i;
    float x2, y;
    const float threehalfs = 1.5f;

    x2 = number * 0.5f;
    y = number;
    i = *(int32_t *)&y; /* evil floating point bit level hacking */
    i = 0x5f3759df - (i >> 1); /* what the fuck? */
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y)); /* 1st iteration */

    return y;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

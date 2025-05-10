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
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/

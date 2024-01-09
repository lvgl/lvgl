/**
 * @file lv_draw_sw_helium.h
 *
 */

#ifndef LV_DRAW_SW_HELIUM_H
#define LV_DRAW_SW_HELIUM_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../../lv_conf_internal.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && !defined(__ASSEMBLY__) && LV_USE_DRAW_ARM2D

#define __ARM_2D_IMPL__
#include "arm_2d.h"
#include "__arm_2d_impl.h"

#if defined(__IS_COMPILER_ARM_COMPILER_5__)
#pragma diag_suppress 174,177,188,68,513,144,1296
#elif defined(__IS_COMPILER_IAR__)
#pragma diag_suppress=Pa093
#elif defined(__IS_COMPILER_GCC__)
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
#endif
/*********************
 *      DEFINES
 *********************/

#define LV_DRAW_SW_RGB565_SWAP(__buf_ptr, __buf_size_px)                        \
    _lv_draw_sw_rgb565_swap_helium((__buf_ptr), (__buf_size_px))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
extern void arm_2d_helper_swap_rgb16(uint16_t * phwBuffer, uint32_t wCount);

/**********************
 *      MACROS
 **********************/

static inline lv_result_t _lv_draw_sw_rgb565_swap_helium(void * buf, uint32_t buf_size_px)
{
    arm_2d_helper_swap_rgb16((uint16_t *)buf, buf_size_px);
    return LV_RESULT_OK;
}

#endif /* LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM && !defined(__ASSEMBLY__) && LV_USE_DRAW_ARM2D */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_SW_HELIUM_H*/

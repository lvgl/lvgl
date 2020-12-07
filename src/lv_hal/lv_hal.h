/**
 * @file lv_hal.h
 *
 */

#ifndef LV_HAL_H
#define LV_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_hal_disp.h"
#include "lv_hal_indev.h"
#include "lv_hal_tick.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_HOR_RES_MAX
# define LV_HOR_RES_MAX LV_HOR_RES_DEF
#endif

#ifndef LV_VER_RES_MAX
# define LV_VER_RES_MAX LV_VER_RES_DEF
#endif

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

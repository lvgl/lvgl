/**
 * @file lv_layouts.h
 *
 */

#ifndef LV_LAYOUTS_H
#define LV_LAYOUTS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_FLEX
#include "flex/lv_flex.h"
#endif /* LV_USE_FLEX */
#if LV_USE_GRID
#include "grid/lv_grid.h"
#endif /* LV_USE_GRID */

/*********************
 *      DEFINES
 *********************/

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
} /*extern "C"*/
#endif

#endif /*LV_LAYOUTS_H*/

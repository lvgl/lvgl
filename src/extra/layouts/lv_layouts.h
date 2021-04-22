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
#include "flex/lv_flex.h"
#include "grid/lv_grid.h"

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
#if LV_USE_LOG && LV_LOG_TRACE_LAYOUT
#  define LV_TRACE_LAYOUT(...) LV_LOG_TRACE( __VA_ARGS__)
#else
#  define LV_TRACE_LAYOUT(...)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_LAYOUTS_H*/

/**
 * @file lv_profiler.h
 *
 */

#ifndef LV_PROFILER_H
#define LV_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lv_conf_internal.h"

#if LV_USE_PROFILER

#include LV_PROFILER_INCLUDE

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

#else

#define LV_PROFILER_BEGIN
#define LV_PROFILER_END
#define LV_PROFILER_BEGIN_TAG(tag) LV_UNUSED(tag)
#define LV_PROFILER_END_TAG(tag)   LV_UNUSED(tag)

#endif /*LV_USE_PROFILER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PROFILER_H*/

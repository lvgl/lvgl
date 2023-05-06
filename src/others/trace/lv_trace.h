/**
 * @file lv_trace.h
 *
 */

#ifndef LV_TRACE_H
#define LV_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_TRACE

#include LV_TRACE_INCLUDE

/*********************
 *      DEFINES
 *********************/

#ifndef LV_TRACE_BEGIN
#warning "LV_TRACE_BEGIN is not defined"
#define LV_TRACE_BEGIN
#endif

#ifndef LV_TRACE_END
#warning "LV_TRACE_END is not defined"
#define LV_TRACE_END
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

#else

#define LV_TRACE_BEGIN
#define LV_TRACE_END

#endif /*LV_USE_TRACE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TRACE_H*/

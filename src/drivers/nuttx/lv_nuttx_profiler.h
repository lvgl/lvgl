/**
 * @file lv_nuttx_profiler.h
 *
 */

#ifndef LV_NUTTX_PROFILER_H
#define LV_NUTTX_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_NUTTX && LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_nuttx_profiler_init(void);
void lv_nuttx_profiler_set_file(const char * file);
void lv_nuttx_profiler_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_NUTTX && LV_USE_PROFILER && LV_USE_PROFILER_BUILTIN*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NUTTX_PROFILER_H*/

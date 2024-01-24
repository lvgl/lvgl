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

#if LV_USE_NUTTX

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

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_NUTTX */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NUTTX_PROFILER_H*/

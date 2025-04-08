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

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_NUTTX_PROFILER_H*/

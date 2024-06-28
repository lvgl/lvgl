/**
 * @file lv_demo_benchmark.h
 *
 */

#ifndef LV_DEMO_BENCHMARK_H
#define LV_DEMO_BENCHMARK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_demos.h"

#if LV_USE_DEMO_BENCHMARK

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Run all benchmark scenes.
 *
 * On the summary end screen the values shall be interpreted according to the followings:
 * - CPU usage:
 *    - If `LV_SYSMON_GET_IDLE` is not modified it's measured based on the time spent in
 *      `lv_timer_handler`.
 *    - If an (RT)OS is used `LV_SYSMON_GET_IDLE` can be changed to a custom function
 *      which returns the idle percentage of idle task.
 *
 * - FPS: LVGL attempted to render this many times in a second. It's limited based on `LV_DEF_REFR_PERIOD`
 *
 * - Render time: LVGL spent this much time with rendering only. It's not aware of task yielding,
 *   but simply the time difference between the start and end of the rendering is measured
 *
 * - Flush time: It's the sum of
 *     - the time spent in the `flush_cb` and
 *     - the time spent with waiting for flush ready.
 */
void lv_demo_benchmark(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_BENCHMARK*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_BENCHMARK_H*/

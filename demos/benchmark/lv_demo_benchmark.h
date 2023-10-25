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

#if LV_USE_PERF_MONITOR == 0
#error "lv_demo_benchmark: LV_USE_PERF_MONITOR is required. Enable it in lv_conf.h (LV_USE_PERF_MONITOR 1)"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/** Run all test scenes in the LVGL benchmark with a given mode
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

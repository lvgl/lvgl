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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef void finished_cb_t(void);


/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_demo_benchmark(void);

void lv_demo_benchmark_run_scene(int_fast16_t scene_no);

void lv_demo_benchmark_set_finished_cb(finished_cb_t * finished_cb);

/**
 * Make the benchmark work at the highest frame rate
 * @param en true: highest frame rate; false: default frame rate
 */
void lv_demo_benchmark_set_max_speed(bool en);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_BENCHMARK_H*/

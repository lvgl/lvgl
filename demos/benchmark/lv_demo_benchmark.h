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
typedef enum {
    /**Render the scenes and show them on the display.
     * Measure rendering time but it might contain extra time when LVGL waits for the driver.
     * Run each scenes for a few seconds so the performance can be seen by eye too.
     * As only the rendering time is measured and converted to FPS, really high values (e.g. 1000 FPS)
     * are possible.*/
    LV_DEMO_BENCHMARK_MODE_RENDER_AND_DRIVER,

    /**Similar to RENDER_AND_DRIVER but instead of measuring the rendering time only measure the real FPS of the system.
     * E.g. even if a scene was rendered in 1 ms, but the screen is redrawn only in every 100 ms, the result will be 10 FPS.*/
    LV_DEMO_BENCHMARK_MODE_REAL,

    /**Temporarily display the `flush_cb` so the pure rendering time will be measured.
     * The display is not updated during the benchmark, only at the end when the summary table is shown.
     * Render a given number of frames from each scene adn calculate the FPS from them.*/
    LV_DEMO_BENCHMARK_MODE_RENDER_ONLY,
} lv_demo_benchmark_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_demo_benchmark(lv_demo_benchmark_mode_t mode);
void lv_demo_benchmark_run_scene(lv_demo_benchmark_mode_t mode, uint16_t scene_no);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_BENCHMARK_H*/

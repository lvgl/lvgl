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
#if LV_DEMO_BENCHMARK_ALIGNED_FONTS

LV_FONT_DECLARE(lv_font_benchmark_montserrat_12_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_14_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_16_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_18_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_20_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_24_aligned)
LV_FONT_DECLARE(lv_font_benchmark_montserrat_26_aligned)

#else

/** Make sure that the aligned fonts are not built when not used */
#define LV_FONT_BENCHMARK_MONTSERRAT_12_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_14_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_16_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_18_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_20_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_22_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_24_ALIGNED 0
#define LV_FONT_BENCHMARK_MONTSERRAT_26_ALIGNED 0

#endif /*LV_DEMO_BENCHMARK_ALIGNED_FONTS*/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    void (*create_cb)(void);
    uint32_t scene_time;
    uint32_t cpu_avg_usage;
    uint32_t fps_avg;
    uint32_t render_avg_time;
    uint32_t flush_avg_time;
    uint32_t measurement_cnt;
} lv_demo_benchmark_scene_dsc_t;

typedef struct {
    /*
     * List of scenes
     * The last scne in this array of scenes is terminated
     * by a sentinel scene that has `create_cb` == NULL
     * Must not be free'd
     */
    lv_demo_benchmark_scene_dsc_t * scenes;

    int32_t total_avg_fps;
    int32_t total_avg_cpu;
    int32_t total_avg_render_time;
    int32_t total_avg_flush_time;
    int32_t valid_scene_cnt; /* Number of scenes in `scenes` with a `measurement_cnt` greater than 0 */
} lv_demo_benchmark_summary_t;

typedef void (*lv_demo_benchmark_on_end_cb_t)(const lv_demo_benchmark_summary_t *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Run all benchmark scenes.
 *
 * On the summary end screen the values shall be interpreted according to the following:
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

/*
 * Register a function to call when the benchmark demo is over
 * @param cb    function to call when the demo is over
 */
void lv_demo_benchmark_set_end_cb(lv_demo_benchmark_on_end_cb_t cb);


/*
 * Display and log the summary
 * This function is called automatically if `lv_on_benchmark_end_cb` is not set
 * @param summary   summary of the benchmark results
 */
void lv_demo_benchmark_summary_display(const lv_demo_benchmark_summary_t * summary);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DEMO_BENCHMARK*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DEMO_BENCHMARK_H*/

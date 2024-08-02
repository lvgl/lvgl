/**
 * @file lv_sysmon_private.h
 *
 */

#ifndef LV_SYSMON_PRIVATE_H
#define LV_SYSMON_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_sysmon.h"

#if LV_USE_SYSMON

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_sysmon_backend_data_t {
    lv_subject_t subject;
    lv_timer_t * timer;
};

#if LV_USE_PERF_MONITOR
struct lv_sysmon_perf_info_t {
    struct {
        bool inited;
        uint32_t refr_start;
        uint32_t refr_interval_sum;
        uint32_t refr_elaps_sum;
        uint32_t refr_cnt;
        uint32_t render_start;
        uint32_t render_elaps_sum; /*Contains the flush time too*/
        uint32_t render_cnt;
        uint32_t flush_in_render_start;
        uint32_t flush_in_render_elaps_sum;
        uint32_t flush_not_in_render_start;
        uint32_t flush_not_in_render_elaps_sum;
        uint32_t last_report_timestamp;
        uint32_t render_in_progress : 1;
    } measured;

    struct {
        uint32_t fps;
        uint32_t cpu;
        uint32_t refr_avg_time;
        uint32_t render_avg_time;       /**< Pure rendering time without flush time*/
        uint32_t flush_avg_time;        /**< Pure flushing time without rendering time*/
        uint32_t cpu_avg_total;
        uint32_t fps_avg_total;
        uint32_t run_cnt;
    } calculated;

};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize built-in system monitor, such as performance and memory monitor.
 */
void lv_sysmon_builtin_init(void);

/**
 * DeInitialize built-in system monitor, such as performance and memory monitor.
 */
void lv_sysmon_builtin_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SYSMON */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYSMON_PRIVATE_H*/

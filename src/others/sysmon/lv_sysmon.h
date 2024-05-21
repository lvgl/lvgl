/**
 * @file lv_sysmon.h
 *
 */

#ifndef LV_SYSMON_H
#define LV_SYSMON_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_timer.h"
#include "../../others/observer/lv_observer.h"

#if LV_USE_SYSMON

#if LV_USE_LABEL == 0
#error "lv_sysmon: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#if LV_USE_OBSERVER == 0
#error "lv_observer: lv_observer is required. Enable it in lv_conf.h (LV_USE_OBSERVER  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_sysmon_backend_data_t {
    lv_subject_t subject;
    lv_timer_t * timer;
};

#if LV_USE_PERF_MONITOR
struct _lv_sysmon_perf_info_t {
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
 * Create a new system monitor label
 * @param disp      create the sys. mon. on this display's system layer
 * @return          the create label
 */
lv_obj_t * lv_sysmon_create(lv_display_t * disp);

#if LV_USE_PERF_MONITOR

/**
 * Show system performance monitor: CPU usage and FPS count
 * @param disp      target display, NULL: use the default displays
 */
void lv_sysmon_show_performance(lv_display_t * disp);

/**
 * Hide system performance monitor
 * @param disp      target display, NULL: use the default
 */
void lv_sysmon_hide_performance(lv_display_t * disp);

#endif /*LV_USE_PERF_MONITOR*/

#if LV_USE_MEM_MONITOR

/**
 * Show system memory monitor: used memory and the memory fragmentation
 * @param disp      target display, NULL: use the default displays
 */
void lv_sysmon_show_memory(lv_display_t * disp);

/**
 * Hide system memory monitor
 * @param disp      target display, NULL: use the default displays
 */
void lv_sysmon_hide_memory(lv_display_t * disp);

#endif /*LV_USE_MEM_MONITOR*/

/**
 * Initialize built-in system monitor, such as performance and memory monitor.
 */
void _lv_sysmon_builtin_init(void);

/**
 * DeInitialize built-in system monitor, such as performance and memory monitor.
 */
void _lv_sysmon_builtin_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SYSMON*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYSMON_H*/

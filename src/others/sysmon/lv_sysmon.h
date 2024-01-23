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

typedef struct {
    lv_subject_t subject;
    lv_timer_t * timer;
} lv_sysmon_backend_data_t;

#if LV_USE_PERF_MONITOR
typedef struct {
    struct {
        uint32_t refr_start;
        uint32_t refr_interval_sum;
        uint32_t refr_elaps_sum;
        uint32_t refr_cnt;
        uint32_t render_start;
        uint32_t render_elaps_sum; /*Contains the flush time too*/
        uint32_t render_cnt;
        uint32_t flush_start;
        uint32_t flush_elaps_sum;
        uint32_t flush_cnt;
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

} lv_sysmon_perf_info_t;
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a system monitor object.
 * @param parent pointer to an object, it will be the parent of the new system monitor
 * @return       pointer to the new system monitor object
 */
lv_obj_t * lv_sysmon_create(lv_obj_t * parent);

/**
 * Set the refresh period of the system monitor object
 * @param obj    pointer to a system monitor object
 * @param period the refresh period in milliseconds
 */
void lv_sysmon_set_refr_period(lv_obj_t * obj, uint32_t period);

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

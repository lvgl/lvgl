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

struct _lv_sysmon_backend_data_t {
    lv_subject_t subject;
    lv_timer_t * timer;
};

#if LV_USE_PERF_MONITOR
struct _lv_sysmon_perf_backend_data_t {
    /**
     * For perf label.
     */
    lv_subject_t subject;
    lv_timer_t * timer;
    lv_sysmon_perf_t * instance;

    /**
     * For perf monitor instances.
     */
    lv_ll_t instances_ll;
    uint32_t last_refr_start;
    uint32_t last_render_start;
    bool scrolling;
    bool inited;
};

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
        uint32_t perf_start;
        uint32_t render_in_progress : 1;
    } measured;

    struct {
        uint32_t duration;
        lv_value_precise_t fps;
        uint32_t cpu;
#if LV_SYSMON_PROC_IDLE_AVAILABLE
        uint32_t cpu_proc;                        /** The applications idle time percentage */
#endif
        lv_value_precise_t refr_avg_time;
        lv_value_precise_t render_avg_time;       /**< Pure rendering time without flush time*/
        lv_value_precise_t flush_avg_time;        /**< Pure flushing time without rendering time*/
        /**
         * Below fields are kept across start/stop/reset of the perf monitor, only valid for overall data.
         */
        lv_value_precise_t cpu_avg_total;
        lv_value_precise_t fps_avg_total;
        uint32_t run_cnt;
    } calculated;

};

typedef enum {
    LV_SYSMON_EVENT_TYPE_INVALID = 0,
    LV_SYSMON_EVENT_TYPE_REFR_BEGIN = 1,
    LV_SYSMON_EVENT_TYPE_REFR_END = 2,
    LV_SYSMON_EVENT_TYPE_RENDER_BEGIN = 3,
    LV_SYSMON_EVENT_TYPE_RENDER_END = 4,
    LV_SYSMON_EVENT_TYPE_SCROLL_BEGIN = 5,
    LV_SYSMON_EVENT_TYPE_SCROLL_END = 6,
} lv_sysmon_event_type_t;

typedef struct {
    lv_sysmon_event_type_t type : 8;
    uint64_t timestamp : 56;
} lv_sysmon_event_data_t;

typedef enum {
    LV_SYSMON_PERF_TYPE_NONE = 0,
    LV_SYSMON_PERF_TYPE_OVERALL = 1 << 0,
    LV_SYSMON_PERF_TYPE_SCROLLS = 1 << 1,
    LV_SYSMON_PERF_TYPE_EVENTS = 1 << 2,
    LV_SYSMON_PERF_TYPE_ALL = LV_SYSMON_PERF_TYPE_OVERALL | LV_SYSMON_PERF_TYPE_SCROLLS | LV_SYSMON_PERF_TYPE_EVENTS,
} lv_sysmon_perf_type_t;

struct _lv_sysmon_perf_data_t {
    lv_sysmon_perf_info_t overall;
    lv_circle_buf_t * scrolls;     /**< Circle buf of lv_sysmon_perf_info_t */
    lv_circle_buf_t * events;      /**< Circle buf of lv_sysmon_event_data_t */
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

#if LV_USE_PERF_MONITOR
/**
 * Create a performance monitor instance.
 * @param disp        the display to create the performance monitor instance on
 * @param tag         the tag of the performance monitor
 * @param max_events  the maximum number of events to store, 0 means no event data will be stored
 * @param max_scrolls the maximum number of scrolls to store, 0 means no scroll data will be stored
 * @return            pointer to the new performance monitor instance
 */
lv_sysmon_perf_t * lv_sysmon_perf_create(lv_display_t * disp, const char * tag, size_t max_events, size_t max_scrolls);

/**
 * Destroy a performance monitor instance.
 * @param perf pointer to the performance monitor instance
 */
void lv_sysmon_perf_destroy(lv_sysmon_perf_t * perf);

/**
 * Start a performance monitor instance, data will be cleared.
 * @param perf pointer to the performance monitor instance
 * @return     LV_RESULT_OK if the performance monitor instance is started successfully, LV_RESULT_INVALID if the performance monitor instance is failed to start
 */
lv_result_t lv_sysmon_perf_start(lv_sysmon_perf_t * perf);

/**
 * Reset the data of a performance monitor instance.
 * @param perf  pointer to the performance monitor instance
 * @param types the types of performance monitor instance to reset
 */
void lv_sysmon_perf_reset_data(lv_sysmon_perf_t * perf, lv_sysmon_perf_type_t types);

/**
 * Get the data of a performance monitor instance.
 * @param perf pointer to the performance monitor instance
 * @return     pointer to the data of performance
 */
const lv_sysmon_perf_data_t * lv_sysmon_perf_get_data(lv_sysmon_perf_t * perf);

/**
 * Stop a performance monitor instance, data will be reset on next start
 * @param perf pointer to the performance monitor instance
 * @return     pointer to the data of performance
 */
const lv_sysmon_perf_data_t * lv_sysmon_perf_stop(lv_sysmon_perf_t * perf);

/**
 * Generate a trace from a performance monitor instance, call lv_profiler to convert to trace, output to console or file.
 * @param perf pointer to the performance monitor instance
 */
void lv_sysmon_perf_generate_trace(lv_sysmon_perf_t * perf);

/**
 * Event handler for performance monitor.
 * @param disp pointer to the display
 * @param e pointer to the event
 */
void lv_sysmon_perf_event(lv_display_t * disp, lv_event_t * e);
#endif

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_SYSMON */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYSMON_PRIVATE_H*/

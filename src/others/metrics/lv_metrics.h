/**
 * @file lv_metrics.h
 *
 */

#ifndef LV_METRICS_H
#define LV_METRICS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"

#if LV_USE_METRICS

#if LV_USE_LABEL == 0
#error "lv_metrics: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_label_t label;
    lv_timer_t * timer;
} lv_metrics_t;

extern const lv_obj_class_t lv_metrics_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a metrics object.
 * @param parent pointer to an object, it will be the parent of the new metrics
 * @return       pointer to the new metrics object
 */
lv_obj_t * lv_metrics_create(lv_obj_t * parent);

/**
 * Check if a given flag or any of the flags are set on an object.
 * @param obj     pointer to a metrics object
 * @param period  metrics refresh period
 */
void lv_metrics_set_refr_period(lv_obj_t * obj, uint32_t period);

/**
 * Initialize built-in metricss, such as performance and memory metricss.
 */
void _lv_metrics_builtin_init(void);

/**********************
 *      MACROS
 **********************/

#else

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
#warning "lv_metrics: lv_metrics is required. Enable it in lv_conf.h (LV_USE_METRICS  1)"
#endif

#endif /*LV_USE_METRICS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_METRICS_H*/

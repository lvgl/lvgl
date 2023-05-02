/**
 * @file lv_stat.h
 *
 */

#ifndef LV_STAT_H
#define LV_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"

#if LV_USE_STAT

#if LV_USE_LABEL == 0
#error "lv_stat: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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
} lv_stat_t;

extern const lv_obj_class_t lv_stat_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a stat object.
 * @param parent pointer to an object, it will be the parent of the new stat
 * @return       pointer to the new stat object
 */
lv_obj_t * lv_stat_create(lv_obj_t * parent);

/**
 * Check if a given flag or any of the flags are set on an object.
 * @param obj     pointer to a stat object
 * @param period  stat refresh period
 */
void lv_stat_set_refr_period(lv_obj_t * obj, uint32_t period);

/**
 * Initialize built-in stats, such as performance and memory stats.
 */
void _lv_stat_builtin_init(void);

/**********************
 *      MACROS
 **********************/

#else

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
#warning "lv_stat: lv_stat is required. Enable it in lv_conf.h (LV_USE_STAT  1)"
#endif

#endif /*LV_USE_STAT*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_STAT_H*/

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

#include "../../../lvgl.h"

#if LV_USE_SYSMON

#if LV_USE_LABEL == 0
#error "lv_sysmon: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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
} lv_sysmon_t;

extern const lv_obj_class_t lv_sysmon_class;

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

/**********************
 *      MACROS
 **********************/

#else

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
#warning "lv_sysmon: lv_sysmon is required. Enable it in lv_conf.h (LV_USE_SYSMON  1)"
#endif

#endif /*LV_USE_SYSMON*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYSMON_H*/

/**
 * @file lv_monitor.h
 *
 */

#ifndef LV_MONITOR_H
#define LV_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"

#if LV_USE_MONITOR

#if LV_USE_LABEL == 0
#error "lv_monitor: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
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
} lv_monitor_t;

extern const lv_obj_class_t lv_monitor_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_monitor_create(void);

void lv_monitor_set_refr_time(lv_obj_t * obj, uint32_t time);

void lv_monitor_builtin_init(void);

/**********************
 *      MACROS
 **********************/

#else

#if LV_USE_PERF_MONITOR || LV_USE_MEM_MONITOR
#warning "lv_monitor: lv_monitor is required. Enable it in lv_conf.h (LV_USE_MONITOR  1)"
#endif

#endif /*LV_USE_MONITOR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MONITOR_H*/

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

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_SYSMON*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_SYSMON_H*/

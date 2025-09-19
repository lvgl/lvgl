/**
 * @file lv_os.h
 *
 */

#ifndef LV_OS_H
#define LV_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    OS OPTIONS
 *********************/

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_types.h"

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
 * Lock LVGL's general mutex.
 * LVGL is not thread safe, so a mutex is used to avoid executing multiple LVGL functions at the same time
 * from different threads. It shall be called when calling LVGL functions from threads
 * different than lv_timer_handler's thread. It doesn't need to be called in LVGL events because
 * they are called from lv_timer_handler().
 * It is called internally in lv_timer_handler().
 */
void lv_lock(void);

/**
 * Same as `lv_lock()` but can be called from an interrupt.
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_lock_isr(void);

/**
 * The pair of `lv_lock()` and `lv_lock_isr()`.
 * It unlocks LVGL general mutex.
 * It is called internally in lv_timer_handler().
 */
void lv_unlock(void);

/**
 * Sleeps the current thread by an amount of milliseconds.
 * @param ms     amount of milliseconds to sleep the current thread.
 */
void lv_sleep_ms(uint32_t ms);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OS_H*/

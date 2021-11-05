/**
 * @file lv_timer.h
 */

#ifndef LV_TIMER_H
#define LV_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stdbool.h>
#include "lv_mem.h"
#include "lv_ll.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_TIMER_HANDLER
#endif

#define LV_NO_TIMER_READY 0xFFFFFFFF

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_timer_t;

/**
 * Timers execute this type of functions.
 */
typedef void (*lv_timer_cb_t)(struct _lv_timer_t *);

/**
 * Descriptor of a lv_timer
 */
typedef struct _lv_timer_t {
    uint32_t period; /**< How often the timer should run*/
    uint32_t last_run; /**< Last time the timer ran*/
    lv_timer_cb_t timer_cb; /**< Timer function*/
    void * user_data; /**< Custom user data*/
    int32_t repeat_count; /**< 1: One time;  -1 : infinity;  n>0: residual times*/
    uint32_t paused : 1;
} lv_timer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the lv_timer module
 */
void _lv_timer_core_init(void);

//! @cond Doxygen_Suppress

/**
 * Call it periodically to handle lv_timers.
 * @return time till it needs to be run next (in ms)
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void);

//! @endcond

/**
 * Create an "empty" timer. It needs to initialized with at least
 * `lv_timer_set_cb` and `lv_timer_set_period`
 * @return pointer to the created timer
 */
lv_timer_t * lv_timer_create_basic(void);

/**
 * Create a new lv_timer
 * @param timer_xcb a callback to call periodically.
 *                 (the 'x' in the argument name indicates that it's not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param user_data custom parameter
 * @return pointer to the new timer
 */
lv_timer_t * lv_timer_create(lv_timer_cb_t timer_xcb, uint32_t period, void * user_data);

/**
 * Delete a lv_timer
 * @param timer pointer to an lv_timer
 */
void lv_timer_del(lv_timer_t * timer);

/**
 * Pause/resume a timer.
 * @param timer pointer to an lv_timer
 */
void lv_timer_pause(lv_timer_t * timer);

void lv_timer_resume(lv_timer_t * timer);

/**
 * Set the callback the timer (the function to call periodically)
 * @param timer pointer to a timer
 * @param timer_cb the function to call periodically
 */
void lv_timer_set_cb(lv_timer_t * timer, lv_timer_cb_t timer_cb);

/**
 * Set new period for a lv_timer
 * @param timer pointer to a lv_timer
 * @param period the new period
 */
void lv_timer_set_period(lv_timer_t * timer, uint32_t period);

/**
 * Make a lv_timer ready. It will not wait its period.
 * @param timer pointer to a lv_timer.
 */
void lv_timer_ready(lv_timer_t * timer);

/**
 * Set the number of times a timer will repeat.
 * @param timer pointer to a lv_timer.
 * @param repeat_count -1 : infinity;  0 : stop ;  n>0: residual times
 */
void lv_timer_set_repeat_count(lv_timer_t * timer, int32_t repeat_count);

/**
 * Reset a lv_timer.
 * It will be called the previously set period milliseconds later.
 * @param timer pointer to a lv_timer.
 */
void lv_timer_reset(lv_timer_t * timer);

/**
 * Enable or disable the whole lv_timer handling
 * @param en true: lv_timer handling is running, false: lv_timer handling is suspended
 */
void lv_timer_enable(bool en);

/**
 * Get idle percentage
 * @return the lv_timer idle in percentage
 */
uint8_t lv_timer_get_idle(void);

/**
 * Iterate through the timers
 * @param timer NULL to start iteration or the previous return value to get the next timer
 * @return the next timer or NULL if there is no more timer
 */
lv_timer_t * lv_timer_get_next(lv_timer_t * timer);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

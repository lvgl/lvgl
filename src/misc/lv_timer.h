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
#include "../tick/lv_tick.h"
#include "lv_types.h"
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

/**
 * Timers execute this type of functions.
 */
typedef void (*lv_timer_cb_t)(lv_timer_t *);

/**
 * Timer handler resume this type of function.
 */
typedef void (*lv_timer_handler_resume_cb_t)(void * data);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Call it periodically to handle lv_timers.
 * @return time till it needs to be run next (in ms)
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void);

/**
 * Call it in the super-loop of main() or threads. It will run lv_timer_handler()
 * with a given period in ms. You can use it with sleep or delay in OS environment.
 * This function is used to simplify the porting.
 * @param period the period for running lv_timer_handler()
 * @return the time after which it must be called again
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler_run_in_period(uint32_t period);

/**
 * Call it in the super-loop of main() or threads. It will automatically call lv_timer_handler() at the right time.
 * This function is used to simplify the porting.
 */
LV_ATTRIBUTE_TIMER_HANDLER void lv_timer_periodic_handler(void);

/**
 * Set the resume callback to the timer handler
 * @param cb the function to call when timer handler is resumed
 * @param data pointer to a resume data
 */
void lv_timer_handler_set_resume_cb(lv_timer_handler_resume_cb_t cb, void * data);

/**
 * Create an "empty" timer. It needs to be initialized with at least
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
void lv_timer_delete(lv_timer_t * timer);

/**
 * Pause a timer.
 * It is typically safe to call from an interrupt handler or a different thread.
 * @param timer pointer to an lv_timer
 */
void lv_timer_pause(lv_timer_t * timer);

/**
 * Resume a timer.
 * @param timer pointer to an lv_timer
 */
void lv_timer_resume(lv_timer_t * timer);

/**
 * Set the callback to the timer (the function to call periodically)
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
 * Set whether a lv_timer will be deleted automatically when it is called `repeat_count` times.
 * @param timer pointer to a lv_timer.
 * @param auto_delete true: auto delete; false: timer will be paused when it is called `repeat_count` times.
 */
void lv_timer_set_auto_delete(lv_timer_t * timer, bool auto_delete);

/**
 * Set custom parameter to the lv_timer.
 * @param timer pointer to a lv_timer.
 * @param user_data custom parameter
 */
void lv_timer_set_user_data(lv_timer_t * timer, void * user_data);

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
uint32_t lv_timer_get_idle(void);

/**
 * Get the time remaining until the next timer will run
 * @return the time remaining in ms
 */
uint32_t lv_timer_get_time_until_next(void);

/**
 * Iterate through the timers
 * @param timer NULL to start iteration or the previous return value to get the next timer
 * @return the next timer or NULL if there is no more timer
 */
lv_timer_t * lv_timer_get_next(lv_timer_t * timer);

/**
 * Get the user_data passed when the timer was created
 * @param timer pointer to the lv_timer
 * @return pointer to the user_data
 */
void * lv_timer_get_user_data(lv_timer_t * timer);

/**
 * Get the pause state of a timer
 * @param timer pointer to a lv_timer
 * @return true: timer is paused; false: timer is running
 */
bool lv_timer_get_paused(lv_timer_t * timer);

#if LV_USE_EXT_DATA
/**
 * @brief Attaches external user data and destructor callback to a timer object
 *
 * Associates custom user data with an LVGL timer and specifies a destructor function
 * that will be automatically invoked when the timer is deleted to properly clean up
 * the associated resources.
 *
 * @param timer      Pointer to the timer object
 * @param data       User-defined data pointer to associate with the timer
 * @param destructor Callback function for cleaning up ext_data when timer is deleted.
 *                   Receives ext_data as parameter. NULL means no cleanup required.
 */
void lv_timer_set_external_data(lv_timer_t * timer, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif

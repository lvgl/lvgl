/**
 * @file lv_tmr.h
 */

#ifndef LV_TMR_H
#define LV_TMR_H

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
#ifndef LV_ATTRIBUTE_TMR_HANDLER
#define LV_ATTRIBUTE_TMR_HANDLER
#endif

#define LV_NO_TMR_READY 0xFFFFFFFF
/**********************
 *      TYPEDEFS
 **********************/

struct _lv_tmr_t;

/**
 * Tasks execute this type type of functions.
 */
typedef void (*lv_tmr_cb_t)(struct _lv_tmr_t *);

/**
 * Descriptor of a lv_tmr
 */
typedef struct _lv_tmr_t {
    uint32_t period; /**< How often the tmr should run */
    uint32_t last_run; /**< Last time the tmr ran */
    lv_tmr_cb_t tmr_cb; /**< Task function */
    void * user_data; /**< Custom user data */
    int32_t repeat_count; /**< 1: One time;  -1 : infinity;  n>0: residual times */
    uint32_t paused :1;
} lv_tmr_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the lv_tmr module
 */
void _lv_tmr_core_init(void);

//! @cond Doxygen_Suppress

/**
 * Call it  periodically to handle lv_tmrs.
 * @return time till it needs to be run next (in ms)
 */
LV_ATTRIBUTE_TMR_HANDLER uint32_t lv_tmr_handler(void);

//! @endcond

/**
 * Create an "empty" tmr. It needs to initialized with at least
 * `lv_tmr_set_cb` and `lv_tmr_set_period`
 * @return pointer to the created tmr
 */
lv_tmr_t * lv_tmr_create_basic(void);

/**
 * Create a new lv_tmr
 * @param tmr_xcb a callback to call periodically.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param user_data custom parameter
 * @return pointer to the new timer
 */
lv_tmr_t * lv_tmr_create(lv_tmr_cb_t tmr_xcb, uint32_t period, void * user_data);

/**
 * Delete a lv_tmr
 * @param tmr pointer to an lv_tmr
 */
void lv_tmr_del(lv_tmr_t * tmr);

/**
 * Pause/resume a timer.
 * @param tmr pointer to an lv_tmr
 * @param pause true: pause the timer; false: resume
 */
void lv_tmr_pause(lv_tmr_t * tmr, bool pause);

/**
 * Set the callback the tmr (the function to call periodically)
 * @param tmr pointer to a tmr
 * @param tmr_cb the function to call periodically
 */
void lv_tmr_set_cb(lv_tmr_t * tmr, lv_tmr_cb_t tmr_cb);

/**
 * Make a lv_tmr ready. It will not wait its period.
 * @param tmr pointer to a lv_tmr.
 */
void lv_tmr_ready(lv_tmr_t * tmr);

/**
 * Set the number of times a tmr will repeat.
 * @param tmr pointer to a lv_tmr.
 * @param repeat_count -1 : infinity;  0 : stop ;  n>0: residual times
 */
void lv_tmr_set_repeat_count(lv_tmr_t * tmr, int32_t repeat_count);

/**
 * Reset a lv_tmr.
 * It will be called the previously set period milliseconds later.
 * @param tmr pointer to a lv_tmr.
 */
void lv_tmr_reset(lv_tmr_t * tmr);

/**
 * Enable or disable the whole lv_tmr handling
 * @param en: true: lv_tmr handling is running, false: lv_tmr handling is suspended
 */
void lv_tmr_enable(bool en);

/**
 * Get idle percentage
 * @return the lv_tmr idle in percentage
 */
uint8_t lv_tmr_get_idle(void);

/**
 * Iterate through the tmrs
 * @param tmr NULL to start iteration or the previous return value to get the next tmr
 * @return the next tmr or NULL if there is no more tmr
 */
lv_tmr_t * lv_tmr_get_next(lv_tmr_t * tmr);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

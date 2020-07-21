/**
 * @file lv_task.c
 * An 'lv_task'  is a void (*fp) (void* param) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to lv_tasks.
 */

#ifndef LV_TASK_H
#define LV_TASK_H

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
#ifndef LV_ATTRIBUTE_TASK_HANDLER
#define LV_ATTRIBUTE_TASK_HANDLER
#endif

#define LV_NO_TASK_READY 0xFFFFFFFF
/**********************
 *      TYPEDEFS
 **********************/

struct _lv_task_t;

/**
 * Tasks execute this type type of functions.
 */
typedef void (*lv_task_cb_t)(struct _lv_task_t *);

/**
 * Possible priorities for lv_tasks
 */
enum {
    LV_TASK_PRIO_OFF = 0,
    LV_TASK_PRIO_LOWEST,
    LV_TASK_PRIO_LOW,
    LV_TASK_PRIO_MID,
    LV_TASK_PRIO_HIGH,
    LV_TASK_PRIO_HIGHEST,
    _LV_TASK_PRIO_NUM,
};
typedef uint8_t lv_task_prio_t;

/**
 * Descriptor of a lv_task
 */
typedef struct _lv_task_t {
    uint32_t period; /**< How often the task should run */
    uint32_t last_run; /**< Last time the task ran */
    lv_task_cb_t task_cb; /**< Task function */

    void * user_data; /**< Custom user data */

    int32_t repeat_count; /**< 1: Task times;  -1 : infinity;  0 : stop ;  n>0: residual times */
    uint8_t prio : 3; /**< Task priority */
} lv_task_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the lv_task module
 */
void _lv_task_core_init(void);

//! @cond Doxygen_Suppress

/**
 * Call it  periodically to handle lv_tasks.
 * @return time till it needs to be run next (in ms)
 */
LV_ATTRIBUTE_TASK_HANDLER uint32_t lv_task_handler(void);

//! @endcond

/**
 * Create an "empty" task. It needs to initialized with at least
 * `lv_task_set_cb` and `lv_task_set_period`
 * @return pointer to the created task
 */
lv_task_t * lv_task_create_basic(void);

/**
 * Create a new lv_task
 * @param task_xcb a callback which is the task itself. It will be called periodically.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param prio priority of the task (LV_TASK_PRIO_OFF means the task is stopped)
 * @param user_data custom parameter
 * @return pointer to the new task
 */
lv_task_t * lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void * user_data);

/**
 * Delete a lv_task
 * @param task pointer to task_cb created by task
 */
void lv_task_del(lv_task_t * task);

/**
 * Set the callback the task (the function to call periodically)
 * @param task pointer to a task
 * @param task_cb the function to call periodically
 */
void lv_task_set_cb(lv_task_t * task, lv_task_cb_t task_cb);

/**
 * Set new priority for a lv_task
 * @param task pointer to a lv_task
 * @param prio the new priority
 */
void lv_task_set_prio(lv_task_t * task, lv_task_prio_t prio);

/**
 * Set new period for a lv_task
 * @param task pointer to a lv_task
 * @param period the new period
 */
void lv_task_set_period(lv_task_t * task, uint32_t period);

/**
 * Make a lv_task ready. It will not wait its period.
 * @param task pointer to a lv_task.
 */
void lv_task_ready(lv_task_t * task);

/**
 * Set the number of times a task will repeat.
 * @param task pointer to a lv_task.
 * @param repeat_count -1 : infinity;  0 : stop ;  n>0: residual times
 */
void lv_task_set_repeat_count(lv_task_t * task, int32_t repeat_count);

/**
 * Reset a lv_task.
 * It will be called the previously set period milliseconds later.
 * @param task pointer to a lv_task.
 */
void lv_task_reset(lv_task_t * task);

/**
 * Enable or disable the whole  lv_task handling
 * @param en: true: lv_task handling is running, false: lv_task handling is suspended
 */
void lv_task_enable(bool en);

/**
 * Get idle percentage
 * @return the lv_task idle in percentage
 */
uint8_t lv_task_get_idle(void);

/**
 * Iterate through the tasks
 * @param task NULL to start iteration or the previous return value to get the next task
 * @return the next task or NULL if there is no more task
 */
lv_task_t * lv_task_get_next(lv_task_t * task);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

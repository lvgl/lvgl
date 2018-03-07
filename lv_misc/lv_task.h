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
/**********************
 *      TYPEDEFS
 **********************/
/**
 * Possible priorities for lv_tasks
 */
typedef enum
{
    LV_TASK_PRIO_OFF = 0,
    LV_TASK_PRIO_LOWEST,
    LV_TASK_PRIO_LOW,
    LV_TASK_PRIO_MID,
    LV_TASK_PRIO_HIGH,
    LV_TASK_PRIO_HIGHEST,
    LV_TASK_PRIO_NUM,
}lv_task_prio_t;

/**
 * Descriptor of a lv_task
 */
typedef struct
{
    uint32_t period;
    uint32_t last_run;
    void (*task) (void*);
    void * param;
    uint8_t prio:3;
    uint8_t once:1;
}lv_task_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the lv_task module
 */
void lv_task_init(void);

/**
 * Call it  periodically to handle lv_tasks.
 */
LV_ATTRIBUTE_TASK_HANDLER void lv_task_handler(void);

/**
 * Create a new lv_task
 * @param task a function which is the task itself
 * @param period call period in ms unit
 * @param prio priority of the task (LV_TASK_PRIO_OFF means the task is stopped)
 * @param param free parameter
 * @return pointer to the new task
 */
lv_task_t* lv_task_create(void (*task) (void *), uint32_t period, lv_task_prio_t prio, void * param);

/**
 * Delete a lv_task
 * @param lv_task_p pointer to task created by lv_task_p
 */
void lv_task_del(lv_task_t* lv_task_p);

/**
 * Set new priority for a lv_task
 * @param lv_task_p pointer to a lv_task
 * @param prio the new priority
 */
void lv_task_set_prio(lv_task_t* lv_task_p, lv_task_prio_t prio);

/**
 * Set new period for a lv_task
 * @param lv_task_p pointer to a lv_task
 * @param period the new period
 */
void lv_task_set_period(lv_task_t* lv_task_p, uint32_t period);

/**
 * Make a lv_task ready. It will not wait its period.
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_ready(lv_task_t* lv_task_p);


/**
 * Delete the lv_task after one call
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_once(lv_task_t * lv_task_p);

/**
 * Reset a lv_task.
 * It will be called the previously set period milliseconds later.
 * @param lv_task_p pointer to a lv_task.
 */
void lv_task_reset(lv_task_t* lv_task_p);

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

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 

/**
 * @file ptask.h
 * A Periodic Tasks is a void (*fp) (void) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to ptasks. 
 */

#ifndef PTASK_H
#define PTASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#if USE_PTASK != 0
#include <stdint.h>
#include <stdbool.h>
#include "../mem/dyn_mem.h"
#include "../mem/linked_list.h"

/*********************
 *      DEFINES
 *********************/
#ifndef PTASK_IDLE_PERIOD
#define PTASK_IDLE_PERIOD 500
#endif


/**********************
 *      TYPEDEFS
 **********************/
/**
 * Possible priorities for ptasks
 */
typedef enum
{
    PTASK_PRIO_OFF = 0,
    PTASK_PRIO_LOWEST,
    PTASK_PRIO_LOW,
    PTASK_PRIO_MID,
    PTASK_PRIO_HIGH,
    PTASK_PRIO_HIGHEST,
    PTASK_PRIO_NUM,
}ptask_prio_t;

/**
 * Descriptor of a ptask
 */
typedef struct
{
    uint32_t period;
    uint32_t last_run;
    void (*task) (void*);
    void * param;
    uint8_t prio:3;
    uint8_t once:1;
}ptask_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the ptask module
 */
void ptask_init(void);

/**
 * Call it  periodically to handle ptasks.
 */
void ptask_handler(void);

/**
 * Create a new ptask
 * @param task a function which is the task itself
 * @param period call period in ms unit
 * @param prio priority of the task (PTASK_PRIO_OFF means the task is stopped)
 * @param param free parameter
 * @return pointer to the new task
 */
ptask_t* ptask_create(void (*task) (void *), uint32_t period, ptask_prio_t prio, void * param);

/**
 * Delete a ptask
 * @param ptask_p pointer to task created by ptask_p
 */
void ptask_del(ptask_t* ptask_p);

/**
 * Set new priority for a ptask
 * @param ptask_p pointer to a ptask
 * @param prio the new priority
 */
void ptask_set_prio(ptask_t* ptask_p, ptask_prio_t prio);

/**
 * Set new period for a ptask
 * @param ptask_p pointer to a ptask
 * @param period the new period
 */
void ptask_set_period(ptask_t* ptask_p, uint32_t period);

/**
 * Make a ptask ready. It will not wait its period.
 * @param ptask_p pointer to a ptask.
 */
void ptask_ready(ptask_t* ptask_p);


/**
 * Delete the ptask after one call
 * @param ptask_p pointer to a ptask.
 */
void ptask_once(ptask_t * ptask_p);

/**
 * Reset a ptask.
 * It will be called the previously set period milliseconds later.
 * @param ptask_p pointer to a ptask.
 */
void ptask_reset(ptask_t* ptask_p);

/**
 * Enable or disable ptask handling
 * @param en: true: ptask handling is running, false: ptask handling is suspended
 */
void ptask_en(bool en);

uint8_t ptask_get_idle(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif 

/**
 * @file ptask.c
 * A Periodic Tasks is a void (*fp) (void) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to ptasks. 
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_PTASK != 0


#include "lv_task.h"
#include "../lv_hal/lv_hal_tick.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool ptask_exec(ptask_t* ptask_p, ptask_prio_t prio_act);

/**********************
 *  STATIC VARIABLES
 **********************/
static ll_dsc_t ptask_ll;  /*Linked list to store the ptasks*/
static bool ptask_run = false;
static uint8_t idle_last = 0;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the ptask module
 */
void ptask_init(void)
{
    ll_init(&ptask_ll, sizeof(ptask_t));
    
    /*Initially enable the ptask handling*/
    ptask_en(true);
}

/**
 * Call it  periodically to handle ptasks.
 */
void ptask_handler(void)
{
	if(ptask_run == false) return;

	static uint32_t idle_tick = 0;
    static uint32_t used_tick = 0;
    uint32_t start_tick = MISC_SYSTICK_GET();

	if(idle_tick == 0) idle_tick = MISC_SYSTICK_GET();

    ptask_t* ptask_prio_a[PTASK_PRIO_NUM]; /*Lists for all prio.*/
    ptask_prio_t prio_act;
    bool prio_reset = false;  /*Used to go back to the highest priority*/
    ptask_t* ptask_next;

    /*Init. the lists*/
    for(prio_act = PTASK_PRIO_LOWEST; prio_act <= PTASK_PRIO_HIGHEST; prio_act++) {
        ptask_prio_a[prio_act] = ll_get_head(&ptask_ll);
    }

    /*Handle the ptasks on all priority*/
    for(prio_act = PTASK_PRIO_HIGHEST; prio_act > PTASK_PRIO_OFF; prio_act --) {
        /*Reset the prio. if necessary*/
        if(prio_reset != false) {
            prio_reset = false;
            prio_act = PTASK_PRIO_HIGHEST; /*Go again with highest prio */
        }

        /* Read all ptask on 'prio_act' but stop on 'prio_reset' */
        while(ptask_prio_a[prio_act] != NULL && prio_reset == false)  {
            /* Get the next task. (Invalid pointer if a ptask deletes itself)*/
            ptask_next = ll_get_next(&ptask_ll, ptask_prio_a[prio_act]);

            /*Execute the current ptask*/
            bool executed = ptask_exec(ptask_prio_a[prio_act], prio_act);
            if(executed != false) {     /*If the task is executed*/
                /* During the execution higher priority ptasks
                 * can be ready, so reset the priority if it is not highest*/
                if(prio_act != PTASK_PRIO_HIGHEST) {
                    prio_reset = true;
                }
            }

            ptask_prio_a[prio_act] = ptask_next; /*Load the next task*/
        }

        /*Reset higher priority lists on 'prio_reset' query*/
        if(prio_reset != false) {
            for(prio_act = prio_act + 1; prio_act <= PTASK_PRIO_HIGHEST; prio_act++) {
                ptask_prio_a[prio_act] = ll_get_head(&ptask_ll);
            }
        }
    }

    used_tick += MISC_SYSTICK_ELAPS(start_tick);
    if(MISC_SYSTICK_ELAPS(idle_tick) > PTASK_IDLE_PERIOD) {
        idle_last = (uint32_t)((uint32_t) used_tick * 100) / MISC_SYSTICK_ELAPS(idle_tick);  /*Calculate the busy time*/
        idle_last = idle_last > 100 ? 0 : 100 - idle_last;  /*Convert he busy time to idle time*/
        idle_tick = 0;
        used_tick = 0;
    }
}

/**
 * Create a new ptask
 * @param task a function which is the task itself
 * @param period call period in ms unit
 * @param prio priority of the task (PTASK_PRIO_OFF means the task is stopped)
 * @param param free parameter
 * @return pointer to the new task
 */
ptask_t* ptask_create(void (*task) (void *), uint32_t period, ptask_prio_t prio, void * param)
{
    ptask_t* new_ptask;
    
    new_ptask = ll_ins_head(&ptask_ll);
    dm_assert(new_ptask);
    
    new_ptask->period = period;
    new_ptask->task = task;
    new_ptask->prio = prio;
    new_ptask->param = param;
    new_ptask->once = 0;
    new_ptask->last_run = MISC_SYSTICK_GET();

    return new_ptask;
}

/**
 * Delete a ptask
 * @param ptask_p pointer to task created by ptask_p
 */
void ptask_del(ptask_t* ptask_p) 
{
    ll_rem(&ptask_ll, ptask_p);
    
    dm_free(ptask_p);
}

/**
 * Set new priority for a ptask
 * @param ptask_p pointer to a ptask
 * @param prio the new priority
 */
void ptask_set_prio(ptask_t* ptask_p, ptask_prio_t prio)
{
    ptask_p->prio = prio;
}

/**
 * Set new period for a ptask
 * @param ptask_p pointer to a ptask
 * @param period the new period
 */
void ptask_set_period(ptask_t* ptask_p, uint32_t period)
{
    ptask_p->period = period;
}

/**
 * Make a ptask ready. It will not wait its period.
 * @param ptask_p pointer to a ptask.
 */
void ptask_ready(ptask_t* ptask_p)
{
    ptask_p->last_run = MISC_SYSTICK_GET() - ptask_p->period - 1;
}

/**
 * Delete the ptask after one call
 * @param ptask_p pointer to a ptask.
 */
void ptask_once(ptask_t * ptask_p)
{
    ptask_p->once = 1;
}

/**
 * Reset a ptask. 
 * It will be called the previously set period milliseconds later.
 * @param ptask_p pointer to a ptask.
 */
void ptask_reset(ptask_t* ptask_p)
{
    ptask_p->last_run = MISC_SYSTICK_GET();
}

/**
 * Enable or disable ptask handling
 * @param en: true: ptask handling is running, false: ptask handling is suspended
 */
void ptask_en(bool en)
{
	ptask_run = en;
}

/**
 * Get idle percentage
 * @return the ptask idle in percentage
 */
uint8_t ptask_get_idle(void)
{
    return idle_last;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute task if its the priority is appropriate 
 * @param ptask_p pointer to ptask
 * @param prio_act the current priority
 * @return true: execute, false: not executed
 */
static bool ptask_exec (ptask_t* ptask_p, ptask_prio_t prio_act)
{
    bool exec = false;
    
    /*Execute ptask if its prio is 'prio_act'*/
    if(ptask_p->prio == prio_act) {
        /*Execute if at least 'period' time elapsed*/
        uint32_t elp = MISC_SYSTICK_ELAPS(ptask_p->last_run);
        if(elp >= ptask_p->period) {
            ptask_p->last_run = MISC_SYSTICK_GET();
            ptask_p->task(ptask_p->param);

            /*Delete if it was a one shot ptask*/
            if(ptask_p->once != 0) ptask_del(ptask_p);

            exec = true;
        }
    }
    
    return exec;
}


#endif

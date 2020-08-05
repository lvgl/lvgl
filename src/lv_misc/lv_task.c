/**
 * @file lv_task.c
 * An 'lv_task'  is a void (*fp) (void* param) type function which will be called periodically.
 * A priority (5 levels + disable) can be assigned to lv_tasks.
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_task.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_hal/lv_hal_tick.h"
#include "lv_gc.h"

#if defined(LV_GC_INCLUDE)
    #include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/
#define IDLE_MEAS_PERIOD 500 /*[ms]*/
#define DEF_PRIO LV_TASK_PRIO_MID
#define DEF_PERIOD 500

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_task_exec(lv_task_t * task);
static uint32_t lv_task_time_remaining(lv_task_t * task);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_task_run  = false;
static uint8_t idle_last = 0;
static bool task_deleted;
static bool task_list_changed;
static bool task_created;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the lv_task module
 */
void _lv_task_core_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_task_ll), sizeof(lv_task_t));

    task_list_changed = false;
    /*Initially enable the lv_task handling*/
    lv_task_enable(true);
}

/**
 * Call it  periodically to handle lv_tasks.
 * @return the time after which it must be called again
 */
LV_ATTRIBUTE_TASK_HANDLER uint32_t lv_task_handler(void)
{


    LV_LOG_TRACE("lv_task_handler started");

    /*Avoid concurrent running of the task handler*/
    static bool already_running = false;
    if(already_running) return 1;
    already_running = true;

    static uint32_t idle_period_start = 0;
    static uint32_t handler_start     = 0;
    static uint32_t busy_time         = 0;
    static uint32_t time_till_next;

    if(lv_task_run == false) {
        already_running = false; /*Release mutex*/
        return 1;
    }

    handler_start = lv_tick_get();

    /* Run all task from the highest to the lowest priority
     * If a lower priority task is executed check task again from the highest priority
     * but on the priority of executed tasks don't run tasks before the executed*/
    lv_task_t * task_interrupter = NULL;
    lv_task_t * next;
    bool end_flag;
    do {
        end_flag                 = true;
        task_deleted             = false;
        task_created             = false;
        LV_GC_ROOT(_lv_task_act) = _lv_ll_get_head(&LV_GC_ROOT(_lv_task_ll));
        while(LV_GC_ROOT(_lv_task_act)) {
            /* The task might be deleted if it runs only once ('once = 1')
             * So get next element until the current is surely valid*/
            next = _lv_ll_get_next(&LV_GC_ROOT(_lv_task_ll), LV_GC_ROOT(_lv_task_act));

            /*We reach priority of the turned off task. There is nothing more to do.*/
            if(((lv_task_t *)LV_GC_ROOT(_lv_task_act))->prio == LV_TASK_PRIO_OFF) {
                break;
            }

            /*Here is the interrupter task. Don't execute it again.*/
            if(LV_GC_ROOT(_lv_task_act) == task_interrupter) {
                task_interrupter = NULL; /*From this point only task after the interrupter comes, so
                                            the interrupter is not interesting anymore*/
                LV_GC_ROOT(_lv_task_act) = next;
                continue; /*Load the next task*/
            }

            /*Just try to run the tasks with highest priority.*/
            if(((lv_task_t *)LV_GC_ROOT(_lv_task_act))->prio == LV_TASK_PRIO_HIGHEST) {
                lv_task_exec(LV_GC_ROOT(_lv_task_act));
            }
            /*Tasks with higher priority than the interrupted shall be run in every case*/
            else if(task_interrupter) {
                if(((lv_task_t *)LV_GC_ROOT(_lv_task_act))->prio > task_interrupter->prio) {
                    if(lv_task_exec(LV_GC_ROOT(_lv_task_act))) {
                        if(!task_created && !task_deleted) {
                            /*Check all tasks again from the highest priority */
                            task_interrupter = LV_GC_ROOT(_lv_task_act);
                            end_flag = false;
                            break;
                        }
                    }
                }
            }
            /* It is no interrupter task or we already reached it earlier.
             * Just run the remaining tasks*/
            else {
                if(lv_task_exec(LV_GC_ROOT(_lv_task_act))) {
                    if(!task_created && !task_deleted) {
                        task_interrupter = LV_GC_ROOT(_lv_task_act); /*Check all tasks again from the highest priority */
                        end_flag         = false;
                        break;
                    }
                }
            }

            /*If a task was created or deleted then this or the next item might be corrupted*/
            if(task_created || task_deleted) {
                task_interrupter = NULL;
                break;
            }

            if(task_list_changed) {
                task_interrupter = NULL;
                end_flag = false;
                task_list_changed = false;
                break;
            }

            LV_GC_ROOT(_lv_task_act) = next; /*Load the next task*/
        }
    } while(!end_flag);

    busy_time += lv_tick_elaps(handler_start);
    uint32_t idle_period_time = lv_tick_elaps(idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {

        idle_last         = (uint32_t)((uint32_t)busy_time * 100) / IDLE_MEAS_PERIOD; /*Calculate the busy percentage*/
        idle_last         = idle_last > 100 ? 0 : 100 - idle_last;                    /*But we need idle time*/
        busy_time         = 0;
        idle_period_start = lv_tick_get();
    }

    time_till_next = LV_NO_TASK_READY;
    next = _lv_ll_get_head(&LV_GC_ROOT(_lv_task_ll));
    while(next) {
        if(next->prio != LV_TASK_PRIO_OFF) {
            uint32_t delay = lv_task_time_remaining(next);
            if(delay < time_till_next)
                time_till_next = delay;
        }

        next = _lv_ll_get_next(&LV_GC_ROOT(_lv_task_ll), next); /*Find the next task*/
    }

    already_running = false; /*Release the mutex*/

    LV_LOG_TRACE("lv_task_handler ready");
    return time_till_next;
}
/**
 * Create an "empty" task. It needs to initialized with at least
 * `lv_task_set_cb` and `lv_task_set_period`
 * @return pointer to the created task
 */
lv_task_t * lv_task_create_basic(void)
{
    lv_task_t * new_task = NULL;
    lv_task_t * tmp;

    /*Create task lists in order of priority from high to low*/
    tmp = _lv_ll_get_head(&LV_GC_ROOT(_lv_task_ll));

    /*It's the first task*/
    if(NULL == tmp) {
        new_task = _lv_ll_ins_head(&LV_GC_ROOT(_lv_task_ll));
        LV_ASSERT_MEM(new_task);
        if(new_task == NULL) return NULL;
    }
    /*Insert the new task to proper place according to its priority*/
    else {
        do {
            if(tmp->prio <= DEF_PRIO) {
                new_task = _lv_ll_ins_prev(&LV_GC_ROOT(_lv_task_ll), tmp);
                LV_ASSERT_MEM(new_task);
                if(new_task == NULL) return NULL;
                break;
            }
            tmp = _lv_ll_get_next(&LV_GC_ROOT(_lv_task_ll), tmp);
        } while(tmp != NULL);

        /*Only too high priority tasks were found. Add the task to the end*/
        if(tmp == NULL) {
            new_task = _lv_ll_ins_tail(&LV_GC_ROOT(_lv_task_ll));
            LV_ASSERT_MEM(new_task);
            if(new_task == NULL) return NULL;
        }
    }
    task_list_changed = true;

    new_task->period  = DEF_PERIOD;
    new_task->task_cb = NULL;
    new_task->prio    = DEF_PRIO;

    new_task->repeat_count = -1;
    new_task->last_run = lv_tick_get();

    new_task->user_data = NULL;

    task_created = true;

    return new_task;
}

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
lv_task_t * lv_task_create(lv_task_cb_t task_xcb, uint32_t period, lv_task_prio_t prio, void * user_data)
{
    lv_task_t * new_task = lv_task_create_basic();
    LV_ASSERT_MEM(new_task);
    if(new_task == NULL) return NULL;

    lv_task_set_cb(new_task, task_xcb);
    lv_task_set_period(new_task, period);
    lv_task_set_prio(new_task, prio);
    new_task->user_data = user_data;

    return new_task;
}

/**
 * Set the callback the task (the function to call periodically)
 * @param task pointer to a task
 * @param task_cb the function to call periodically
 */
void lv_task_set_cb(lv_task_t * task, lv_task_cb_t task_cb)
{
    task->task_cb = task_cb;
}

/**
 * Delete a lv_task
 * @param task pointer to task created by task
 */
void lv_task_del(lv_task_t * task)
{
    _lv_ll_remove(&LV_GC_ROOT(_lv_task_ll), task);
    task_list_changed = true;

    lv_mem_free(task);

    if(LV_GC_ROOT(_lv_task_act) == task) task_deleted = true; /*The active task was deleted*/
}

/**
 * Set new priority for a lv_task
 * @param task pointer to a lv_task
 * @param prio the new priority
 */
void lv_task_set_prio(lv_task_t * task, lv_task_prio_t prio)
{
    if(task->prio == prio) return;

    /*Find the tasks with new priority*/
    lv_task_t * i;
    _LV_LL_READ(LV_GC_ROOT(_lv_task_ll), i) {
        if(i->prio <= prio) {
            if(i != task) _lv_ll_move_before(&LV_GC_ROOT(_lv_task_ll), task, i);
            break;
        }
    }

    /*There was no such a low priority so far then add the node to the tail*/
    if(i == NULL) {
        _lv_ll_move_before(&LV_GC_ROOT(_lv_task_ll), task, NULL);
    }
    task_list_changed = true;

    task->prio = prio;
}

/**
 * Set new period for a lv_task
 * @param task pointer to a lv_task
 * @param period the new period
 */
void lv_task_set_period(lv_task_t * task, uint32_t period)
{
    task->period = period;
}

/**
 * Make a lv_task ready. It will not wait its period.
 * @param task pointer to a lv_task.
 */
void lv_task_ready(lv_task_t * task)
{
    task->last_run = lv_tick_get() - task->period - 1;
}

/**
 * Set the number of times a task will repeat.
 * @param task pointer to a lv_task.
 * @param repeat_count -1 : infinity;  0 : stop ;  n>0: residual times
 */
void lv_task_set_repeat_count(lv_task_t * task, int32_t repeat_count)
{
    task->repeat_count = repeat_count;
}

/**
 * Reset a lv_task.
 * It will be called the previously set period milliseconds later.
 * @param task pointer to a lv_task.
 */
void lv_task_reset(lv_task_t * task)
{
    task->last_run = lv_tick_get();
}

/**
 * Enable or disable the whole lv_task handling
 * @param en: true: lv_task handling is running, false: lv_task handling is suspended
 */
void lv_task_enable(bool en)
{
    lv_task_run = en;
}

/**
 * Get idle percentage
 * @return the lv_task idle in percentage
 */
uint8_t lv_task_get_idle(void)
{
    return idle_last;
}

/**
 * Iterate through the tasks
 * @param task NULL to start iteration or the previous return value to get the next task
 * @return the next task or NULL if there is no more task
 */
lv_task_t * lv_task_get_next(lv_task_t * task)
{
    if(task == NULL) return _lv_ll_get_head(&LV_GC_ROOT(_lv_task_ll));
    else return _lv_ll_get_next(&LV_GC_ROOT(_lv_task_ll), task);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute task if its the priority is appropriate
 * @param task pointer to lv_task
 * @return true: execute, false: not executed
 */
static bool lv_task_exec(lv_task_t * task)
{
    bool exec = false;

    if(lv_task_time_remaining(task) == 0) {
        task->last_run = lv_tick_get();
        task_deleted   = false;
        task_created   = false;
        if(task->task_cb) task->task_cb(task);

        /*Delete if it was a one shot lv_task*/
        if(task_deleted == false) { /*The task might be deleted by itself as well*/
            if(task->repeat_count > 0) {
                task->repeat_count--;
            }
            if(task->repeat_count == 0) {
                lv_task_del(task);
            }
        }
        exec = true;
    }

    return exec;
}

/**
 * Find out how much time remains before a task must be run.
 * @param task pointer to lv_task
 * @return the time remaining, or 0 if it needs to be run again
 */
static uint32_t lv_task_time_remaining(lv_task_t * task)
{
    /*Check if at least 'period' time elapsed*/
    uint32_t elp = lv_tick_elaps(task->last_run);
    if(elp >= task->period)
        return 0;
    return task->period - elp;
}

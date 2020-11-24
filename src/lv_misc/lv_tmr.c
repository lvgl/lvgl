/**
 * @file lv_tmr.c
 */

/*********************
 *      INCLUDES
 *********************/
#include <stddef.h>
#include "lv_tmr.h"
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
#define DEF_PERIOD 500

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_tmr_exec(lv_tmr_t * tmr);
static uint32_t lv_tmr_time_remaining(lv_tmr_t * tmr);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_tmr_run  = false;
static uint8_t idle_last = 0;
static bool tmr_deleted;
static bool tmr_created;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the lv_tmr module
 */
void _lv_tmr_core_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_tmr_ll), sizeof(lv_tmr_t));

    /*Initially enable the lv_tmr handling*/
    lv_tmr_enable(true);
}

/**
 * Call it  periodically to handle lv_tmrs.
 * @return the time after which it must be called again
 */
LV_ATTRIBUTE_TMR_HANDLER uint32_t lv_tmr_handler(void)
{
    LV_LOG_TRACE("lv_tmr_handler started");

    /*Avoid concurrent running of the tmr handler*/
    static bool already_running = false;
    if(already_running) return 1;
    already_running = true;

    static uint32_t idle_period_start = 0;
    static uint32_t handler_start     = 0;
    static uint32_t busy_time         = 0;
    static uint32_t time_till_next;

    if(lv_tmr_run == false) {
        already_running = false; /*Release mutex*/
        return 1;
    }

    handler_start = lv_tick_get();

    /* Run all tmr from the highest to the lowest priority
     * If a lower priority tmr is executed check tmr again from the highest priority
     * but on the priority of executed tmrs don't run tmrs before the executed*/
    lv_tmr_t * next;
    bool end_flag;
    do {
        end_flag                 = true;
        tmr_deleted             = false;
        tmr_created             = false;
        LV_GC_ROOT(_lv_tmr_act) = _lv_ll_get_head(&LV_GC_ROOT(_lv_tmr_ll));
        while(LV_GC_ROOT(_lv_tmr_act)) {
            /* The tmr might be deleted if it runs only once ('repeat_count = 1')
             * So get next element until the current is surely valid*/
            next = _lv_ll_get_next(&LV_GC_ROOT(_lv_tmr_ll), LV_GC_ROOT(_lv_tmr_act));

            if(lv_tmr_exec(LV_GC_ROOT(_lv_tmr_act))) {
                if(!tmr_created && !tmr_deleted) {
                    end_flag         = false;
                    break;
                }
            }

            /*If a tmr was created or deleted then this or the next item might be corrupted*/
            if(tmr_created || tmr_deleted) {
                break;
            }

            LV_GC_ROOT(_lv_tmr_act) = next; /*Load the next tmr*/
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

    time_till_next = LV_NO_TMR_READY;
    next = _lv_ll_get_head(&LV_GC_ROOT(_lv_tmr_ll));
    while(next) {
        if(next->repeat_count) {
            uint32_t delay = lv_tmr_time_remaining(next);
            if(delay < time_till_next)
                time_till_next = delay;
        }

        next = _lv_ll_get_next(&LV_GC_ROOT(_lv_tmr_ll), next); /*Find the next tmr*/
    }

    already_running = false; /*Release the mutex*/

    LV_LOG_TRACE("lv_tmr_handler ready");
    return time_till_next;
}
/**
 * Create an "empty" tmr. It needs to initialized with at least
 * `lv_tmr_set_cb` and `lv_tmr_set_period`
 * @return pointer to the created tmr
 */
lv_tmr_t * lv_tmr_create_basic(void)
{
    lv_tmr_t * new_tmr = NULL;

    new_tmr = _lv_ll_ins_head(&LV_GC_ROOT(_lv_tmr_ll));
    LV_ASSERT_MEM(new_tmr);
    if(new_tmr == NULL) return NULL;

    new_tmr->period  = DEF_PERIOD;
    new_tmr->tmr_cb = NULL;
    new_tmr->repeat_count = -1;
    new_tmr->paused = 0;
    new_tmr->last_run = lv_tick_get();
    new_tmr->user_data = NULL;

    tmr_created = true;

    return new_tmr;
}

/**
 * Create a new lv_tmr
 * @param tmr_xcb a callback which is the tmr itself. It will be called periodically.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param user_data custom parameter
 * @return pointer to the new tmr
 */
lv_tmr_t * lv_tmr_create(lv_tmr_cb_t tmr_xcb, uint32_t period, void * user_data)
{
    lv_tmr_t * new_tmr = lv_tmr_create_basic();
    LV_ASSERT_MEM(new_tmr);
    if(new_tmr == NULL) return NULL;

    lv_tmr_set_cb(new_tmr, tmr_xcb);
    lv_tmr_set_period(new_tmr, period);
    new_tmr->user_data = user_data;

    return new_tmr;
}

/**
 * Set the callback the tmr (the function to call periodically)
 * @param tmr pointer to a tmr
 * @param tmr_cb the function to call periodically
 */
void lv_tmr_set_cb(lv_tmr_t * tmr, lv_tmr_cb_t tmr_cb)
{
    tmr->tmr_cb = tmr_cb;
}

/**
 * Delete a lv_tmr
 * @param tmr pointer to tmr created by tmr
 */
void lv_tmr_del(lv_tmr_t * tmr)
{
    _lv_ll_remove(&LV_GC_ROOT(_lv_tmr_ll), tmr);
    tmr_deleted = true;

    lv_mem_free(tmr);
}

/**
 * Pause/resume a timer.
 * @param tmr pointer to an lv_tmr
 * @param pause true: pause the timer; false: resume
 */
void lv_tmr_pause(lv_tmr_t * tmr, bool pause)
{
    tmr->paused = pause;

}
/**
 * Set new period for a lv_tmr
 * @param tmr pointer to a lv_tmr
 * @param period the new period
 */
void lv_tmr_set_period(lv_tmr_t * tmr, uint32_t period)
{
    tmr->period = period;
}

/**
 * Make a lv_tmr ready. It will not wait its period.
 * @param tmr pointer to a lv_tmr.
 */
void lv_tmr_ready(lv_tmr_t * tmr)
{
    tmr->last_run = lv_tick_get() - tmr->period - 1;
}

/**
 * Set the number of times a tmr will repeat.
 * @param tmr pointer to a lv_tmr.
 * @param repeat_count -1 : infinity;  0 : stop ;  n >0: residual times
 */
void lv_tmr_set_repeat_count(lv_tmr_t * tmr, int32_t repeat_count)
{
    tmr->repeat_count = repeat_count;
}

/**
 * Reset a lv_tmr.
 * It will be called the previously set period milliseconds later.
 * @param tmr pointer to a lv_tmr.
 */
void lv_tmr_reset(lv_tmr_t * tmr)
{
    tmr->last_run = lv_tick_get();
}

/**
 * Enable or disable the whole lv_tmr handling
 * @param en: true: lv_tmr handling is running, false: lv_tmr handling is suspended
 */
void lv_tmr_enable(bool en)
{
    lv_tmr_run = en;
}

/**
 * Get idle percentage
 * @return the lv_tmr idle in percentage
 */
uint8_t lv_tmr_get_idle(void)
{
    return idle_last;
}

/**
 * Iterate through the tmrs
 * @param tmr NULL to start iteration or the previous return value to get the next tmr
 * @return the next tmr or NULL if there is no more tmr
 */
lv_tmr_t * lv_tmr_get_next(lv_tmr_t * tmr)
{
    if(tmr == NULL) return _lv_ll_get_head(&LV_GC_ROOT(_lv_tmr_ll));
    else return _lv_ll_get_next(&LV_GC_ROOT(_lv_tmr_ll), tmr);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute tmr if its the priority is appropriate
 * @param tmr pointer to lv_tmr
 * @return true: execute, false: not executed
 */
static bool lv_tmr_exec(lv_tmr_t * tmr)
{
    if(tmr->paused) return false;

    bool exec = false;
    if(lv_tmr_time_remaining(tmr) == 0) {
        tmr->last_run = lv_tick_get();
        tmr_deleted   = false;
        tmr_created   = false;
        if(tmr->tmr_cb) tmr->tmr_cb(tmr);
        LV_ASSERT_MEM_INTEGRITY();

        /*Delete if it was a one shot lv_tmr*/
        if(tmr_deleted == false) { /*The tmr might be deleted by itself as well*/
            if(tmr->repeat_count > 0) {
                tmr->repeat_count--;
            }
            if(tmr->repeat_count == 0) {
                lv_tmr_del(tmr);
            }
        }
        exec = true;
    }

    return exec;
}

/**
 * Find out how much time remains before a tmr must be run.
 * @param tmr pointer to lv_tmr
 * @return the time remaining, or 0 if it needs to be run again
 */
static uint32_t lv_tmr_time_remaining(lv_tmr_t * tmr)
{
    /*Check if at least 'period' time elapsed*/
    uint32_t elp = lv_tick_elaps(tmr->last_run);
    if(elp >= tmr->period)
        return 0;
    return tmr->period - elp;
}

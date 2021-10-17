/**
 * @file lv_timer.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_timer.h"
#include <stddef.h>
#include "../misc/lv_assert.h"
#include "../hal/lv_hal_tick.h"
#include "lv_gc.h"

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
static bool lv_timer_exec(lv_timer_t * timer);
static uint32_t lv_timer_time_remaining(lv_timer_t * timer);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool lv_timer_run = false;
static uint8_t idle_last = 0;
static bool timer_deleted;
static bool timer_created;

/**********************
 *      MACROS
 **********************/
#if LV_LOG_TRACE_TIMER
    #define TIMER_TRACE(...) LV_LOG_TRACE( __VA_ARGS__)
#else
    #define TIMER_TRACE(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the lv_timer module
 */
void _lv_timer_core_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_timer_ll), sizeof(lv_timer_t));

    /*Initially enable the lv_timer handling*/
    lv_timer_enable(true);
}

/**
 * Call it periodically to handle lv_timers.
 * @return the time after which it must be called again
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void)
{
    TIMER_TRACE("begin");

    /*Avoid concurrent running of the timer handler*/
    static bool already_running = false;
    if(already_running) {
        TIMER_TRACE("already running, concurrent calls are not allow, returning");
        return 1;
    }
    already_running = true;

    if(lv_timer_run == false) {
        already_running = false; /*Release mutex*/
        return 1;
    }

    static uint32_t idle_period_start = 0;
    static uint32_t busy_time         = 0;

    uint32_t handler_start = lv_tick_get();

    if(handler_start == 0) {
        static uint32_t run_cnt = 0;
        run_cnt++;
        if(run_cnt > 100) {
            run_cnt = 0;
            LV_LOG_WARN("It seems lv_tick_inc() is not called.");
        }
    }

    /*Run all timer from the list*/
    lv_timer_t * next;
    do {
        timer_deleted             = false;
        timer_created             = false;
        LV_GC_ROOT(_lv_timer_act) = _lv_ll_get_head(&LV_GC_ROOT(_lv_timer_ll));
        while(LV_GC_ROOT(_lv_timer_act)) {
            /*The timer might be deleted if it runs only once ('repeat_count = 1')
             *So get next element until the current is surely valid*/
            next = _lv_ll_get_next(&LV_GC_ROOT(_lv_timer_ll), LV_GC_ROOT(_lv_timer_act));

            if(lv_timer_exec(LV_GC_ROOT(_lv_timer_act))) {
                /*If a timer was created or deleted then this or the next item might be corrupted*/
                if(timer_created || timer_deleted) {
                    TIMER_TRACE("Start from the first timer again because a timer was created or deleted");
                    break;
                }
            }

            LV_GC_ROOT(_lv_timer_act) = next; /*Load the next timer*/
        }
    } while(LV_GC_ROOT(_lv_timer_act));

    uint32_t time_till_next = LV_NO_TIMER_READY;
    next = _lv_ll_get_head(&LV_GC_ROOT(_lv_timer_ll));
    while(next) {
        if(!next->paused) {
            uint32_t delay = lv_timer_time_remaining(next);
            if(delay < time_till_next)
                time_till_next = delay;
        }

        next = _lv_ll_get_next(&LV_GC_ROOT(_lv_timer_ll), next); /*Find the next timer*/
    }

    busy_time += lv_tick_elaps(handler_start);
    uint32_t idle_period_time = lv_tick_elaps(idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {
        idle_last         = (busy_time * 100) / idle_period_time;  /*Calculate the busy percentage*/
        idle_last         = idle_last > 100 ? 0 : 100 - idle_last; /*But we need idle time*/
        busy_time         = 0;
        idle_period_start = lv_tick_get();
    }

    already_running = false; /*Release the mutex*/

    TIMER_TRACE("finished (%d ms until the next timer call)", time_till_next);
    return time_till_next;
}

/**
 * Create an "empty" timer. It needs to initialized with at least
 * `lv_timer_set_cb` and `lv_timer_set_period`
 * @return pointer to the created timer
 */
lv_timer_t * lv_timer_create_basic(void)
{
    return lv_timer_create(NULL, DEF_PERIOD, NULL);
}

/**
 * Create a new lv_timer
 * @param timer_xcb a callback which is the timer itself. It will be called periodically.
 *                 (the 'x' in the argument name indicates that it's not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param period call period in ms unit
 * @param user_data custom parameter
 * @return pointer to the new timer
 */
lv_timer_t * lv_timer_create(lv_timer_cb_t timer_xcb, uint32_t period, void * user_data)
{
    lv_timer_t * new_timer = NULL;

    new_timer = _lv_ll_ins_head(&LV_GC_ROOT(_lv_timer_ll));
    LV_ASSERT_MALLOC(new_timer);
    if(new_timer == NULL) return NULL;

    new_timer->period = period;
    new_timer->timer_cb = timer_xcb;
    new_timer->repeat_count = -1;
    new_timer->paused = 0;
    new_timer->last_run = lv_tick_get();
    new_timer->user_data = user_data;

    timer_created = true;

    return new_timer;
}

/**
 * Set the callback the timer (the function to call periodically)
 * @param timer pointer to a timer
 * @param timer_cb the function to call periodically
 */
void lv_timer_set_cb(lv_timer_t * timer, lv_timer_cb_t timer_cb)
{
    timer->timer_cb = timer_cb;
}

/**
 * Delete a lv_timer
 * @param timer pointer to timer created by timer
 */
void lv_timer_del(lv_timer_t * timer)
{
    _lv_ll_remove(&LV_GC_ROOT(_lv_timer_ll), timer);
    timer_deleted = true;

    lv_mem_free(timer);
}

/**
 * Pause/resume a timer.
 * @param timer pointer to an lv_timer
 */
void lv_timer_pause(lv_timer_t * timer)
{
    timer->paused = true;
}

void lv_timer_resume(lv_timer_t * timer)
{
    timer->paused = false;
}

/**
 * Set new period for a lv_timer
 * @param timer pointer to a lv_timer
 * @param period the new period
 */
void lv_timer_set_period(lv_timer_t * timer, uint32_t period)
{
    timer->period = period;
}

/**
 * Make a lv_timer ready. It will not wait its period.
 * @param timer pointer to a lv_timer.
 */
void lv_timer_ready(lv_timer_t * timer)
{
    timer->last_run = lv_tick_get() - timer->period - 1;
}

/**
 * Set the number of times a timer will repeat.
 * @param timer pointer to a lv_timer.
 * @param repeat_count -1 : infinity;  0 : stop ;  n >0: residual times
 */
void lv_timer_set_repeat_count(lv_timer_t * timer, int32_t repeat_count)
{
    timer->repeat_count = repeat_count;
}

/**
 * Reset a lv_timer.
 * It will be called the previously set period milliseconds later.
 * @param timer pointer to a lv_timer.
 */
void lv_timer_reset(lv_timer_t * timer)
{
    timer->last_run = lv_tick_get();
}

/**
 * Enable or disable the whole lv_timer handling
 * @param en true: lv_timer handling is running, false: lv_timer handling is suspended
 */
void lv_timer_enable(bool en)
{
    lv_timer_run = en;
}

/**
 * Get idle percentage
 * @return the lv_timer idle in percentage
 */
uint8_t lv_timer_get_idle(void)
{
    return idle_last;
}

/**
 * Iterate through the timers
 * @param timer NULL to start iteration or the previous return value to get the next timer
 * @return the next timer or NULL if there is no more timer
 */
lv_timer_t * lv_timer_get_next(lv_timer_t * timer)
{
    if(timer == NULL) return _lv_ll_get_head(&LV_GC_ROOT(_lv_timer_ll));
    else return _lv_ll_get_next(&LV_GC_ROOT(_lv_timer_ll), timer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute timer if its priority is appropriate
 * @param timer pointer to lv_timer
 * @return true: execute, false: not executed
 */
static bool lv_timer_exec(lv_timer_t * timer)
{
    if(timer->paused) return false;

    bool exec = false;
    if(lv_timer_time_remaining(timer) == 0) {
        /* Decrement the repeat count before executing the timer_cb.
         * If any timer is deleted `if(timer->repeat_count == 0)` is not executed below
         * but at least the repeat count is zero and the timer can be deleted in the next round*/
        int32_t original_repeat_count = timer->repeat_count;
        if(timer->repeat_count > 0) timer->repeat_count--;
        timer->last_run = lv_tick_get();
        TIMER_TRACE("calling timer callback: %p", timer->timer_cb);
        if(timer->timer_cb && original_repeat_count != 0) timer->timer_cb(timer);
        TIMER_TRACE("timer callback %p finished", timer->timer_cb);
        LV_ASSERT_MEM_INTEGRITY();
        exec = true;
    }

    if(timer_deleted == false) { /*The timer might be deleted by itself as well*/
        if(timer->repeat_count == 0) { /*The repeat count is over, delete the timer*/
            TIMER_TRACE("deleting timer with %p callback because the repeat count is over", timer->timer_cb);
            lv_timer_del(timer);
        }
    }

    return exec;
}

/**
 * Find out how much time remains before a timer must be run.
 * @param timer pointer to lv_timer
 * @return the time remaining, or 0 if it needs to be run again
 */
static uint32_t lv_timer_time_remaining(lv_timer_t * timer)
{
    /*Check if at least 'period' time elapsed*/
    uint32_t elp = lv_tick_elaps(timer->last_run);
    if(elp >= timer->period)
        return 0;
    return timer->period - elp;
}

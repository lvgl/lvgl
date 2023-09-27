/**
 * @file lv_timer.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_timer.h"
#include "../core/lv_global.h"
#include "../tick/lv_tick.h"
#include "../stdlib/lv_mem.h"
#include "../stdlib/lv_sprintf.h"
#include "lv_assert.h"
#include "lv_ll.h"
#include "lv_profiler.h"

/*********************
 *      DEFINES
 *********************/

#define IDLE_MEAS_PERIOD 500 /*[ms]*/
#define DEF_PERIOD 500

#define state LV_GLOBAL_DEFAULT()->timer_state
#define timer_ll_p &(state.timer_ll)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_timer_exec(lv_timer_t * timer);
static uint32_t lv_timer_time_remaining(lv_timer_t * timer);
static void lv_timer_handler_resume(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#if LV_USE_LOG && LV_LOG_TRACE_TIMER
    #define LV_TRACE_TIMER(...) LV_LOG_TRACE(__VA_ARGS__)
#else
    #define LV_TRACE_TIMER(...)
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init the lv_timer module
 */
void _lv_timer_core_init(void)
{
    _lv_ll_init(timer_ll_p, sizeof(lv_timer_t));

    /*Initially enable the lv_timer handling*/
    lv_timer_enable(true);
}

/**
 * Call it periodically to handle lv_timers.
 * @return the time after which it must be called again
 */
LV_ATTRIBUTE_TIMER_HANDLER uint32_t lv_timer_handler(void)
{
    LV_TRACE_TIMER("begin");

    lv_timer_state_t * state_p = &state;
    /*Avoid concurrent running of the timer handler*/
    if(state_p->already_running) {
        LV_TRACE_TIMER("already running, concurrent calls are not allow, returning");
        return 1;
    }
    state_p->already_running = true;

    if(state_p->lv_timer_run == false) {
        state_p->already_running = false; /*Release mutex*/
        return 1;
    }

    LV_PROFILER_BEGIN;
    uint32_t handler_start = lv_tick_get();

    if(handler_start == 0) {
        state.run_cnt++;
        if(state.run_cnt > 100) {
            state.run_cnt = 0;
            LV_LOG_WARN("It seems lv_tick_inc() is not called.");
        }
    }

    /*Run all timer from the list*/
    lv_timer_t * next;
    lv_timer_t * timer_active;
    lv_ll_t * timer_head = timer_ll_p;
    do {
        state_p->timer_deleted             = false;
        state_p->timer_created             = false;

        timer_active = _lv_ll_get_head(timer_head);
        while(timer_active) {
            /*The timer might be deleted if it runs only once ('repeat_count = 1')
             *So get next element until the current is surely valid*/
            next = _lv_ll_get_next(timer_head, timer_active);

            if(lv_timer_exec(timer_active)) {
                /*If a timer was created or deleted then this or the next item might be corrupted*/
                if(state_p->timer_created || state_p->timer_deleted) {
                    LV_TRACE_TIMER("Start from the first timer again because a timer was created or deleted");
                    break;
                }
            }

            timer_active = next; /*Load the next timer*/
        }
    } while(timer_active);

    uint32_t time_until_next = LV_NO_TIMER_READY;
    next = _lv_ll_get_head(timer_head);
    while(next) {
        if(!next->paused) {
            uint32_t delay = lv_timer_time_remaining(next);
            if(delay < time_until_next)
                time_until_next = delay;
        }

        next = _lv_ll_get_next(timer_head, next); /*Find the next timer*/
    }

    state_p->busy_time += lv_tick_elaps(handler_start);
    uint32_t idle_period_time = lv_tick_elaps(state_p->idle_period_start);
    if(idle_period_time >= IDLE_MEAS_PERIOD) {
        state_p->idle_last         = (state_p->busy_time * 100) / idle_period_time;  /*Calculate the busy percentage*/
        state_p->idle_last         = state_p->idle_last > 100 ? 0 : 100 - state_p->idle_last; /*But we need idle time*/
        state_p->busy_time         = 0;
        state_p->idle_period_start = lv_tick_get();
    }

    state_p->timer_time_until_next = time_until_next;
    state_p->already_running = false; /*Release the mutex*/

    LV_TRACE_TIMER("finished (%" LV_PRIu32 " ms until the next timer call)", time_until_next);
    LV_PROFILER_END;
    return time_until_next;
}

LV_ATTRIBUTE_TIMER_HANDLER void lv_timer_periodic_handler(void)
{
    lv_timer_state_t * state_p = &state;
    if(lv_tick_elaps(state_p->periodic_last_tick) >= state_p->timer_time_until_next) {
        LV_TRACE_TIMER("calling lv_timer_handler()");
        lv_timer_handler();
        state_p->periodic_last_tick = lv_tick_get();
    }
}

/**
 * Create an "empty" timer. It needs to be initialized with at least
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

    new_timer = _lv_ll_ins_head(timer_ll_p);
    LV_ASSERT_MALLOC(new_timer);
    if(new_timer == NULL) return NULL;

    new_timer->period = period;
    new_timer->timer_cb = timer_xcb;
    new_timer->repeat_count = -1;
    new_timer->paused = 0;
    new_timer->last_run = lv_tick_get();
    new_timer->user_data = user_data;

    state.timer_created = true;

    lv_timer_handler_resume();

    return new_timer;
}

/**
 * Set the callback to the timer (the function to call periodically)
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
    _lv_ll_remove(timer_ll_p, timer);
    state.timer_deleted = true;

    lv_free(timer);
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
    lv_timer_handler_resume();
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
 * Set custom parameter to the lv_timer.
 * @param timer pointer to a lv_timer.
 * @param user_data custom parameter
 */
void lv_timer_set_user_data(lv_timer_t * timer, void * user_data)
{
    timer->user_data = user_data;
}

/**
 * Reset a lv_timer.
 * It will be called the previously set period milliseconds later.
 * @param timer pointer to a lv_timer.
 */
void lv_timer_reset(lv_timer_t * timer)
{
    timer->last_run = lv_tick_get();
    lv_timer_handler_resume();
}

/**
 * Enable or disable the whole lv_timer handling
 * @param en true: lv_timer handling is running, false: lv_timer handling is suspended
 */
void lv_timer_enable(bool en)
{
    state.lv_timer_run = en;
    if(en) lv_timer_handler_resume();
}

/**
 * Get idle percentage
 * @return the lv_timer idle in percentage
 */
uint8_t lv_timer_get_idle(void)
{
    return state.idle_last;
}

/**
 * Get idle period start tick
 * @return the lv_timer idle period start tick
 */
uint32_t lv_timer_get_time_until_next(void)
{
    return state.timer_time_until_next;
}

/**
 * Iterate through the timers
 * @param timer NULL to start iteration or the previous return value to get the next timer
 * @return the next timer or NULL if there is no more timer
 */
lv_timer_t * lv_timer_get_next(lv_timer_t * timer)
{
    if(timer == NULL) return _lv_ll_get_head(timer_ll_p);
    else return _lv_ll_get_next(timer_ll_p, timer);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Execute timer if its remaining time is zero
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
        LV_TRACE_TIMER("calling timer callback: %p", *((void **)&timer->timer_cb));

        if(timer->timer_cb && original_repeat_count != 0) timer->timer_cb(timer);

        if(!state.timer_deleted) {
            LV_TRACE_TIMER("timer callback %p finished", *((void **)&timer->timer_cb));
        }
        else {
            LV_TRACE_TIMER("timer callback finished");
        }

        LV_ASSERT_MEM_INTEGRITY();
        exec = true;
    }

    if(state.timer_deleted == false) { /*The timer might be deleted by itself as well*/
        if(timer->repeat_count == 0) { /*The repeat count is over, delete the timer*/
            LV_TRACE_TIMER("deleting timer with %p callback because the repeat count is over", *((void **)&timer->timer_cb));
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

/**
 * Call the ready lv_timer
 */
static void lv_timer_handler_resume(void)
{
    /*If there is a timer which is ready to run then resume the timer loop*/
    state.timer_time_until_next = 0;
}

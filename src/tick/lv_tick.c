/**
 * @file lv_tick.c
 * Provide access to the system tick with 1 millisecond resolution
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_tick.h"
#include <stddef.h>
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define state LV_GLOBAL_DEFAULT()->tick_state

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * You have to call this function periodically
 * @param tick_period the call period of this function in milliseconds
 */
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period)
{
    lv_tick_state_t * state_p = &state;

    state_p->sys_irq_flag = 0;
    state_p->sys_time += tick_period;
}

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t lv_tick_get(void)
{
    lv_tick_state_t * state_p = &state;

    if(state_p->tick_get_cb)
        return state_p->tick_get_cb();

    /*If `lv_tick_inc` is called from an interrupt while `sys_time` is read
     *the result might be corrupted.
     *This loop detects if `lv_tick_inc` was called while reading `sys_time`.
     *If `tick_irq_flag` was cleared in `lv_tick_inc` try to read again
     *until `tick_irq_flag` remains `1`.*/
    uint32_t result;
    do {
        state_p->sys_irq_flag = 1;
        result        = state_p->sys_time;
    } while(!state_p->sys_irq_flag); /*Continue until see a non interrupted cycle*/

    return result;
}

/**
 * Get the elapsed milliseconds since a previous time stamp
 * @param prev_tick a previous time stamp (return value of lv_tick_get() )
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t lv_tick_elaps(uint32_t prev_tick)
{
    uint32_t act_time = lv_tick_get();

    /*If there is no overflow in sys_time simple subtract*/
    if(act_time >= prev_tick) {
        prev_tick = act_time - prev_tick;
    }
    else {
        prev_tick = UINT32_MAX - prev_tick + 1;
        prev_tick += act_time;
    }

    return prev_tick;
}

void lv_tick_set_cb(lv_tick_get_cb_t cb)
{
    state.tick_get_cb = cb;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

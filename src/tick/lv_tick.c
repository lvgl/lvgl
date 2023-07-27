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

#if LV_TICK_CUSTOM == 1
    #include LV_TICK_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
#if !LV_TICK_CUSTOM
    /*Warning: sys_time is modified across threads by SDL backend, which prevents the ability to use multiple instances of this variable in SDL.*/
    #if !LV_USE_SDL
        #define sys_time lv_global_default()->tick_sys_time
    #endif
    #define irq_flag lv_global_default()->tick_irq_flag
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#if !LV_TICK_CUSTOM && LV_USE_SDL
    static uint32_t sys_time;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if !LV_TICK_CUSTOM
/**
 * You have to call this function periodically
 * @param tick_period the call period of this function in milliseconds
 */
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period)
{
    irq_flag = 0;
    sys_time += tick_period;
}
#endif

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t lv_tick_get(void)
{
#if LV_TICK_CUSTOM == 0

    /*If `lv_tick_inc` is called from an interrupt while `sys_time` is read
     *the result might be corrupted.
     *This loop detects if `lv_tick_inc` was called while reading `sys_time`.
     *If `irq_flag` was cleared in `lv_tick_inc` try to read again
     *until `irq_flag` remains `1`.*/
    uint32_t result;
    do {
        irq_flag = 1;
        result        = sys_time;
    } while(!irq_flag); /*Continue until see a non interrupted cycle*/

    return result;
#else
    return LV_TICK_CUSTOM_SYS_TIME_EXPR;
#endif
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * @file lv_tick.h
 * Provide access to the system tick with 1 millisecond resolution
 */

#ifndef LV_TICK_H
#define LV_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef uint32_t (*lv_tick_get_cb_t)(void);

typedef void (*lv_delay_cb_t)(uint32_t ms);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * You have to call this function periodically.
 * It is typically safe to call from an interrupt handler or a different thread.
 * @param tick_period   the call period of this function in milliseconds
 */
LV_ATTRIBUTE_TICK_INC void lv_tick_inc(uint32_t tick_period);

/**
 * Get the elapsed milliseconds since start up
 * @return          the elapsed milliseconds
 */
uint32_t lv_tick_get(void);

/**
 * Get the elapsed milliseconds since a previous time stamp
 * @param prev_tick     a previous time stamp (return value of lv_tick_get() )
 * @return              the elapsed milliseconds since 'prev_tick'
 */
uint32_t lv_tick_elaps(uint32_t prev_tick);

/**
 * Get the elapsed milliseconds between two time stamps
 * @param tick          a time stamp
 * @param prev_tick     a time stamp before `tick`
 * @return              the elapsed milliseconds between `prev_tick` and `tick`
 */
uint32_t lv_tick_diff(uint32_t tick, uint32_t prev_tick);

/**
 * Delay for the given milliseconds.
 * By default it's a blocking delay, but with `lv_delay_set_cb()`
 * a custom delay function can be set too
 * @param ms        the number of milliseconds to delay
 */
void lv_delay_ms(uint32_t ms);

/**
 * Set a callback for a blocking delay
 * @param cb        pointer to a callback
 */
void lv_delay_set_cb(lv_delay_cb_t cb);

/**
 * Set the custom callback for 'lv_tick_get'
 * @param cb        call this callback on 'lv_tick_get'
 */
void lv_tick_set_cb(lv_tick_get_cb_t cb);

/**
 * Get the custom callback for 'lv_tick_get'
 * @return      call this callback on 'lv_tick_get'
 */
lv_tick_get_cb_t lv_tick_get_cb(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TICK_H*/

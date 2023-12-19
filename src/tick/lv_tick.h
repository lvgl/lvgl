/**
 * @file lv_hal_tick.h
 * Provide access to the system tick with 1 millisecond resolution
 */

#ifndef LV_HAL_TICK_H
#define LV_HAL_TICK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
    uint32_t sys_time;
    volatile uint8_t sys_irq_flag;
    lv_tick_get_cb_t tick_get_cb;
    lv_delay_cb_t delay_cb;
} lv_tick_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * You have to call this function periodically
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
 * Delay for the given milliseconds.
 * By default it's a blocking delay, but with `lv_delay_set_cb()`
 * a custom delay function can be set too
 * @param ms        the number of milliseconds to delay
 */
void lv_delay_ms(uint32_t ms);

/**
 * Set the custom callback for 'lv_tick_get'
 * @param cb        call this callback on 'lv_tick_get'
 */
void lv_tick_set_cb(lv_tick_get_cb_t cb);

/**
 * Set a custom callback for 'lv_dalay_ms'
 * @param cb        call this callback in 'lv_dalay_ms'
 */
void lv_delay_set_cb(lv_delay_cb_t cb);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_HAL_TICK_H*/

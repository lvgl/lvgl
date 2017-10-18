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
#include <stdint.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Call this function every milliseconds
 */
void lv_tick_handler(void);

/**
 * Get the elapsed milliseconds since start up
 * @return the elapsed milliseconds
 */
uint32_t lv_tick_get(void);

/**
 * Get the elapsed milliseconds science a previous time stamp
 * @param prev_tick a previous time stamp from 'systick_get'
 * @return the elapsed milliseconds since 'prev_tick'
 */
uint32_t lv_tick_elaps(uint32_t prev_tick);

/**
 * Add a callback function to the systick interrupt
 * @param cb a function pointer
 * @return true: 'cb' added to the systick callbacks, false: 'cb' not added
 */
bool lv_tick_add_callback(void (*cb) (void));

/**
 * Remove a callback function from the tick callbacks
 * @param cb a function pointer (added with 'lv_hal_tick_add_callback')
 */
void lv_tick_rem_callback(void (*cb) (void));

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_HAL_TICK_H*/

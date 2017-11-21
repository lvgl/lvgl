/**
 * @file idle.h
 * Measure the CPU load by measuring how long can CPU count (increment the variable) 
 * if do nothing else in reference time.
 * After it in the main loop also counts. 
 * The ratio of the last count in given time and the reference count will give the idle ratio. 
 */

#ifndef IDLE_H
#define IDLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_IDLE != 0

#include <stdint.h>

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
 * Init the idle module. It will make a reference measurement.
 */
void idle_init(void);

/**
 * Return with the measured CPU idle in percentage
 * @return The CPU idle in percentage
 */
uint8_t idle_get(void);

/**
 * Use it instead of an empty while(1) loop to measure the idle time
 */
void idle_loop(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif

/**
 * @file lv_tick_private.h
 *
 */

#ifndef LV_TICK_PRIVATE_H
#define LV_TICK_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_tick.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    volatile uint32_t sys_time;
    volatile uint8_t sys_irq_flag;
    lv_tick_get_cb_t tick_get_cb;
    lv_delay_cb_t delay_cb;
} lv_tick_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TICK_PRIVATE_H*/

/**
 * @file lv_timer_private.h
 *
 */

#ifndef LV_TIMER_PRIVATE_H
#define LV_TIMER_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_timer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Descriptor of a lv_timer
 */
struct lv_timer_t {
    uint32_t period; /**< How often the timer should run*/
    uint32_t last_run; /**< Last time the timer ran*/
    lv_timer_cb_t timer_cb; /**< Timer function*/
    void * user_data; /**< Custom user data*/
    int32_t repeat_count; /**< 1: One time;  -1 : infinity;  n>0: residual times*/
    uint32_t paused : 1;
    uint32_t auto_delete : 1;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init the lv_timer module
 */
void lv_timer_core_init(void);

/**
 * Deinit the lv_timer module
 */
void lv_timer_core_deinit(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TIMER_PRIVATE_H*/

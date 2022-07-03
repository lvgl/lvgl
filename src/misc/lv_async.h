/**
 * @file lv_async.h
 *
 */

#ifndef LV_ASYNC_H
#define LV_ASYNC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_types.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Type for async callback.
 */
typedef void (*lv_async_cb_t)(void *);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Call an asynchronous function the next time lv_timer_handler() is run. This function is likely to return
 * **before** the call actually happens!
 * @param async_xcb a callback which is the task itself.
 *                 (the 'x' in the argument name indicates that it's not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param user_data custom parameter
 */
lv_res_t lv_async_call(lv_async_cb_t async_xcb, void * user_data);

/**
 * Cancel an asynchronous function call
 * @param async_xcb a callback which is the task itself.
 * @param user_data custom parameter
 */
lv_res_t lv_async_call_cancel(lv_async_cb_t async_xcb, void * user_data);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_ASYNC_H*/

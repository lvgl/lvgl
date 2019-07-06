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

#include "lv_task.h"
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

typedef struct _lv_async_info_t {
    lv_async_cb_t cb;
    void *user_data;
} lv_async_info_t;

struct _lv_obj_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Call an asynchronous function the next time lv_task_handler() is run. This function is likely to return
 * **before** the call actually happens!
 * @param task_xcb a callback which is the task itself.
 *                 (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *                  the `func_name(object, callback, ...)` convention)
 * @param user_data custom parameter
 */
lv_res_t lv_async_call(lv_async_cb_t async_xcb, void * user_data);

/**
 * Helper function for asynchronously deleting objects.
 * 
 * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
 * @param obj object to delete
 * @see lv_async_call
 */
void lv_obj_del_async(struct _lv_obj_t *obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/

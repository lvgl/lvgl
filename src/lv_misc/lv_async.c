/**
 * @file lv_async.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_async.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_async_info_t {
    lv_async_cb_t cb;
    void * user_data;
} lv_async_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_async_task_cb(lv_task_t * task);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_async_call(lv_async_cb_t async_xcb, void * user_data)
{
    /*Allocate an info structure */
    lv_async_info_t * info = lv_mem_alloc(sizeof(lv_async_info_t));

    if(info == NULL)
        return LV_RES_INV;

    /* Create a new task */
    /* Use highest priority so that it will run before a refresh */
    lv_task_t * task = lv_task_create(lv_async_task_cb, 0, LV_TASK_PRIO_HIGHEST, info);

    if(task == NULL) {
        lv_mem_free(info);
        return LV_RES_INV;
    }

    info->cb = async_xcb;
    info->user_data = user_data;

    lv_task_set_repeat_count(task, 1);
    return LV_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_async_task_cb(lv_task_t * task)
{
    lv_async_info_t * info = (lv_async_info_t *)task->user_data;

    info->cb(info->user_data);

    lv_mem_free(info);
}

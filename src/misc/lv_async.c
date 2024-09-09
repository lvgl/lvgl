/**
 * @file lv_async.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_async.h"
#include "lv_timer_private.h"
#include "../stdlib/lv_mem.h"

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

static void lv_async_timer_cb(lv_timer_t * timer);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_async_call(lv_async_cb_t async_xcb, void * user_data)
{
    /*Allocate an info structure*/
    lv_async_info_t * info = lv_malloc(sizeof(lv_async_info_t));

    if(info == NULL)
        return LV_RESULT_INVALID;

    /*Create a new timer*/
    lv_timer_t * timer = lv_timer_create(lv_async_timer_cb, 0, info);

    if(timer == NULL) {
        lv_free(info);
        return LV_RESULT_INVALID;
    }

    info->cb = async_xcb;
    info->user_data = user_data;

    lv_timer_set_repeat_count(timer, 1);
    return LV_RESULT_OK;
}

lv_result_t lv_async_call_cancel(lv_async_cb_t async_xcb, void * user_data)
{
    lv_timer_t * timer = lv_timer_get_next(NULL);
    lv_result_t res = LV_RESULT_INVALID;

    while(timer != NULL) {
        /*Find the next timer node*/
        lv_timer_t * timer_next = lv_timer_get_next(timer);

        /*Find async timer callback*/
        if(timer->timer_cb == lv_async_timer_cb) {
            lv_async_info_t * info = (lv_async_info_t *)timer->user_data;

            /*Match user function callback and user data*/
            if(info->cb == async_xcb && info->user_data == user_data) {
                lv_timer_delete(timer);
                lv_free(info);
                res = LV_RESULT_OK;
            }
        }

        timer = timer_next;
    }

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_async_timer_cb(lv_timer_t * timer)
{
    /*Save the info because an lv_async_call_cancel might delete it in the callback*/
    lv_async_info_t * info = (lv_async_info_t *)timer->user_data;
    lv_async_info_t info_save = *info;
    lv_timer_delete(timer);
    lv_free(info);

    info_save.cb(info_save.user_data);
}

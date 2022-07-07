/**
 * @file lv_async.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_async.h"
#include "lv_gc.h"
#include "lv_mem.h"
#include "lv_timer.h"
#include "lv_ll.h"

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

lv_res_t lv_async_call(lv_async_cb_t async_xcb, void * user_data)
{
    /*Allocate an info structure*/
    lv_async_info_t * info = lv_mem_alloc(sizeof(lv_async_info_t));

    if(info == NULL)
        return LV_RES_INV;

    /*Create a new timer*/
    lv_timer_t * timer = lv_timer_create(lv_async_timer_cb, 0, info);

    if(timer == NULL) {
        lv_mem_free(info);
        return LV_RES_INV;
    }

    info->cb = async_xcb;
    info->user_data = user_data;

    lv_timer_set_repeat_count(timer, 1);
    return LV_RES_OK;
}

lv_res_t lv_async_call_cancel(lv_async_cb_t async_xcb, void * user_data)
{
    lv_timer_t * timer = lv_timer_get_next(NULL);
    lv_res_t res = LV_RES_INV;

    while(timer != NULL) {
        /*Find the next timer node*/
        lv_timer_t * timer_next = lv_timer_get_next(timer);

        /*Find async timer callback*/
        if(timer->timer_cb == lv_async_timer_cb) {
            lv_async_info_t * info = (lv_async_info_t *)timer->user_data;

            /*Match user function callback and user data*/
            if(info->cb == async_xcb && info->user_data == user_data) {
                lv_timer_del(timer);
                lv_mem_free(info);
                res = LV_RES_OK;
            }
        }

        timer = timer_next;
    }

    return res;
}

void _lv_async_init(void)
{
    _lv_ll_init(&LV_GC_ROOT(_lv_async_ll), sizeof(lv_async_info_t));
}

void _lv_async_after_render(void)
{
    if(_lv_ll_is_empty(&LV_GC_ROOT(_lv_async_ll))) {
        return;
    }

    lv_async_info_t * info;
    _LV_LL_READ(&LV_GC_ROOT(_lv_async_ll), info) {
        info->cb(info->user_data);
    }
    _lv_ll_clear(&LV_GC_ROOT(_lv_async_ll));
}

lv_res_t lv_async_after_render_call(lv_async_cb_t async_xcb, void * user_data)
{
    lv_async_info_t * info = _lv_ll_ins_tail(&LV_GC_ROOT(_lv_async_ll));

    if(info == NULL) {
        return LV_RES_INV;
    }

    info->cb = async_xcb;
    info->user_data = user_data;
    return LV_RES_OK;
}

lv_res_t lv_async_after_render_call_cancel(lv_async_cb_t async_xcb, void * user_data)
{
    lv_res_t res = LV_RES_INV;
    lv_async_info_t * info = _lv_ll_get_head(&LV_GC_ROOT(_lv_async_ll));

    while(info != NULL) {
        lv_async_info_t * info_next = _lv_ll_get_next(&LV_GC_ROOT(_lv_async_ll), info);

        if(info->cb == async_xcb && info->user_data == user_data) {
            _lv_ll_remove(&LV_GC_ROOT(_lv_async_ll), info);
            lv_mem_free(info);
            res = LV_RES_OK;
        }

        info = info_next;
    }

    return res;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_async_timer_cb(lv_timer_t * timer)
{
    lv_async_info_t * info = (lv_async_info_t *)timer->user_data;

    info->cb(info->user_data);
    lv_mem_free(info);
}

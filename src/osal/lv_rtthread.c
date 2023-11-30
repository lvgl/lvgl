/**
 * @file lv_rtthread.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_RTTHREAD

#include "../misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

#define THREAD_TIMESLICE 20U

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * thread, lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
                           void * user_data)
{
    thread->thread = rt_thread_create("thread",
                                      callback,
                                      user_data,
                                      stack_size,
                                      prio,
                                      THREAD_TIMESLICE);
    rt_err_t ret = rt_thread_startup(thread->thread);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    rt_err_t ret = rt_thread_delete(thread->thread);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    mutex->mutex = rt_mutex_create("mutex", RT_IPC_FLAG_PRIO);
    if(mutex->mutex == RT_NULL) {
        LV_LOG_WARN("create mutex failed");
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    rt_err_t ret = rt_mutex_take(mutex->mutex, RT_WAITING_FOREVER);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    rt_err_t ret = rt_mutex_take(mutex->mutex, RT_WAITING_FOREVER);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    rt_err_t ret = rt_mutex_release(mutex->mutex);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    rt_err_t ret = rt_mutex_delete(mutex->mutex);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    sync->sem = rt_sem_create("sem", 0, RT_IPC_FLAG_PRIO);
    if(sync->sem == RT_NULL) {
        LV_LOG_WARN("create semaphore failed");
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    rt_err_t ret = rt_sem_take(sync->sem, RT_WAITING_FOREVER);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    rt_err_t ret = rt_sem_release(sync->sem);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    rt_err_t ret = rt_sem_delete(sync->sem);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OS == LV_OS_RTTHREAD*/

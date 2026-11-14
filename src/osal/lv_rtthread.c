/**
 * @file lv_rtthread.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os_private.h"

#if LV_USE_OS == LV_OS_RTTHREAD

/*********************
 *      DEFINES
 *********************/

#define THREAD_TIMESLICE 20U
#define THREAD_SYNC_EVENT (1U << 0)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static rt_err_t thread_sync_signal(lv_thread_sync_t * sync);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name, lv_thread_prio_t prio,
                           void (*callback)(void *), size_t stack_size,
                           void * user_data)
{
    thread->thread = rt_thread_create(name,
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
#ifdef RT_IPC_CMD_SET_VLIMIT
    sync->sem = rt_sem_create("sem", 0, RT_IPC_FLAG_PRIO);
    if(sync->sem == RT_NULL) {
        LV_LOG_WARN("create semaphore failed");
        return LV_RESULT_INVALID;
    }

    rt_err_t ret = rt_sem_control(sync->sem, RT_IPC_CMD_SET_VLIMIT, (void *)1);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        rt_sem_delete(sync->sem);
        sync->sem = RT_NULL;
        return LV_RESULT_INVALID;
    }
#else
    /* An event bit coalesces notifications without accessing semaphore internals. */
    sync->event = rt_event_create("sync", RT_IPC_FLAG_PRIO);
    if(sync->event == RT_NULL) {
        LV_LOG_WARN("create event failed");
        return LV_RESULT_INVALID;
    }
#endif
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
#ifdef RT_IPC_CMD_SET_VLIMIT
    rt_err_t ret = rt_sem_take(sync->sem, RT_WAITING_FOREVER);
#else
    rt_err_t ret = rt_event_recv(sync->event, THREAD_SYNC_EVENT,
                                 RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, RT_WAITING_FOREVER, RT_NULL);
#endif
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
    rt_err_t ret = thread_sync_signal(sync);
    if(ret && ret != -RT_EFULL) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
#ifdef RT_IPC_CMD_SET_VLIMIT
    rt_err_t ret = rt_sem_delete(sync->sem);
#else
    rt_err_t ret = rt_event_delete(sync->event);
#endif
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
#ifdef RT_IPC_CMD_SET_VLIMIT
        sync->sem = RT_NULL;
#else
        sync->event = RT_NULL;
#endif
        return LV_RESULT_OK;
    }
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    /* Both IPC send operations are ISR-safe. Avoid logging from interrupt context. */
    rt_err_t ret = thread_sync_signal(sync);
    return (ret == RT_EOK || ret == -RT_EFULL) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

uint32_t lv_os_get_idle_percent(void)
{
    return lv_timer_get_idle();
}

void lv_sleep_ms(uint32_t ms)
{
    rt_thread_mdelay(ms);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static rt_err_t thread_sync_signal(lv_thread_sync_t * sync)
{
    if(sync == NULL) {
        return -RT_ERROR;
    }

#ifdef RT_IPC_CMD_SET_VLIMIT
    if(sync->sem == RT_NULL) {
        return -RT_ERROR;
    }
    return rt_sem_release(sync->sem);
#else
    if(sync->event == RT_NULL) {
        return -RT_ERROR;
    }
    return rt_event_send(sync->event, THREAD_SYNC_EVENT);
#endif
}

#endif /*LV_USE_OS == LV_OS_RTTHREAD*/

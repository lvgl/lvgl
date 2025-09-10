/**
 * @file lv_mqx.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_MQX

#include "../misc/lv_log.h"
#include "../misc/lv_timer.h"
#include "../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

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

lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name, lv_thread_prio_t prio,
                           void (*callback)(void *), size_t stack_size,
                           void * user_data)
{
    TASK_TEMPLATE_STRUCT task_template;

    lv_memzero(&task_template, sizeof(task_template));

    task_template.TASK_ADDRESS = (TASK_FPTR)callback;
    task_template.TASK_STACKSIZE = stack_size;
    task_template.TASK_PRIORITY = _sched_get_min_priority(0) - prio;
    task_template.TASK_NAME = name;
    task_template.CREATION_PARAMETER = (uint32_t)user_data;

    *thread = _task_create(0, 0, (uint32_t)&task_template);
    if(*thread == MQX_NULL_TASK_ID) {
        LV_LOG_WARN("_task_create failed!");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    _mqx_uint ret = _task_destroy(*thread);
    if(ret != MQX_OK) {
        LV_LOG_WARN("_task_destroy failed!");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    if(MQX_OK != _mutex_init(mutex, NULL)) {
        LV_LOG_WARN("create mutex failed");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    _mqx_uint ret = _mutex_lock(mutex);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    _mqx_uint ret = _mutex_lock(mutex);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    _mqx_uint ret = _mutex_unlock(mutex);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    _mqx_uint ret = _mutex_destroy(mutex);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    if(MQX_OK != _lwsem_create(sync, 0)) {
        LV_LOG_WARN("create semaphore failed");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    _mqx_uint ret = _lwsem_wait(sync);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    _mqx_uint ret = _lwsem_post(sync);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    _mqx_uint ret = _lwsem_destroy(sync);
    if(ret != MQX_OK) {
        LV_LOG_WARN("Error: %x", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

uint32_t lv_os_get_idle_percent(void)
{
    return lv_timer_get_idle();
}

void lv_sleep_ms(uint32_t ms)
{
    _time_delay(ms);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OS == LV_OS_MQX*/

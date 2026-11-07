/**
 * @file lv_chibios.c
 * ChibiOS/RT OSAL, adapted from https://github.com/lvgl/lvgl/pull/8198.
 */
#include "lv_os_private.h"

#if LV_USE_OS == LV_OS_CHIBIOS

#if !CH_CFG_USE_MUTEXES || !CH_CFG_USE_MUTEXES_RECURSIVE
    #error "LVGL requires ChibiOS recursive mutexes"
#endif
#if !CH_CFG_USE_SEMAPHORES || !CH_CFG_USE_DYNAMIC || !CH_CFG_USE_HEAP || !CH_CFG_USE_WAITEXIT
    #error "LVGL requires ChibiOS semaphores, dynamic threads, heap and wait/exit"
#endif

static THD_FUNCTION(run_thread, argument)
{
    lv_thread_t * thread = argument;
    thread->callback(thread->user_data);
}

lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name,
                           lv_thread_prio_t prio, void (*callback)(void *),
                           size_t stack_size, void * user_data)
{
    if(callback == NULL || prio < LV_THREAD_PRIO_LOWEST || prio > LV_THREAD_PRIO_HIGHEST ||
       stack_size > SIZE_MAX - THD_WORKING_AREA_SIZE(0) - PORT_STACK_ALIGN) return LV_RESULT_INVALID;

    thread->callback = callback;
    thread->user_data = user_data;
    /* ChibiOS expects the whole working area, including its thread object
     * and saved context, rather than only the requested stack bytes. */
    thread->handle = chThdCreateFromHeap(NULL, THD_WORKING_AREA_SIZE(stack_size), name,
                                         NORMALPRIO + prio, run_thread, thread);
    return thread->handle != NULL ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    if(thread->handle == NULL || thread->handle == chThdGetSelfX()) return LV_RESULT_INVALID;
    /* Termination is cooperative: the caller must first arrange for the
     * worker callback to return, as with the pthread backend. */
    chThdTerminate(thread->handle);
    chThdWait(thread->handle);
    thread->handle = NULL;
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    /* Explicit initialization also works for uninitialized automatic objects. */
    chMtxObjectInit(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    chMtxLock(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    /* ChibiOS mutex ownership belongs to threads, never to an ISR. */
    LV_UNUSED(mutex);
    return LV_RESULT_INVALID;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    chMtxUnlock(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    /* Native mutexes have no dynamically allocated resources. */
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    /* Start empty. A binary semaphore retains one early signal and wakes
     * one waiter, matching LVGL's event semantics without a racy counter. */
    chBSemObjectInit(sync, true);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    return chBSemWait(sync) == MSG_OK ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    chBSemSignal(sync);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    /* Initialization is a thread-context operation, completed before use. */
    chSysLockFromISR();
    chBSemSignalI(sync);
    chSysUnlockFromISR();
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    chBSemReset(sync, true);
    return LV_RESULT_OK;
}

void lv_sleep_ms(uint32_t ms)
{
    if(ms == 0) chThdYield();
    else chThdSleepMilliseconds(ms);
}

#if LV_USE_SYSMON
#if !CH_DBG_STATISTICS || !CH_CFG_USE_TM || CH_CFG_NO_IDLE_THREAD
    #error "LVGL sysmon requires ChibiOS statistics, time measurement and the idle thread"
#endif

uint32_t lv_os_get_idle_percent(void)
{
    static bool initialized;
    static uint32_t previous_time;
    static rttime_t previous_idle;
    static uint32_t last_idle_percent;

    chSysLock();
    thread_t * idle = chSysGetIdleThreadX();
    const uint32_t now = chSysGetRealtimeCounterX();
    rttime_t total_idle = idle->stats.cumulative;
    if(chThdGetSelfX() == idle) total_idle += (uint32_t)(now - idle->stats.last);

    const uint32_t elapsed = now - previous_time;
    const rttime_t elapsed_idle = total_idle - previous_idle;
    if(initialized && elapsed != 0) {
        /* Use wall-clock cycles instead of summing the registry: adding or
         * removing a thread must not make the denominator underflow. */
        const uint64_t percent = (uint64_t)elapsed_idle * 100U / elapsed;
        last_idle_percent = percent > 100U ? 100U : (uint32_t)percent;
    }
    initialized = true;
    previous_time = now;
    previous_idle = total_idle;
    const uint32_t result = last_idle_percent;
    chSysUnlock();
    return result;
}
#endif /*LV_USE_SYSMON*/
#endif /*LV_USE_OS == LV_OS_CHIBIOS*/

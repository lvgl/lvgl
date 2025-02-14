/**
 * @file lv_os.h
 *
 */

#ifndef LV_OS_H
#define LV_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *    OS OPTIONS
 *********************/

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"

#if LV_USE_OS == LV_OS_NONE
#include "lv_os_none.h"
#elif LV_USE_OS == LV_OS_PTHREAD
#include "lv_pthread.h"
#elif LV_USE_OS == LV_OS_FREERTOS
#include "lv_freertos.h"
#elif LV_USE_OS == LV_OS_CMSIS_RTOS2
#include "lv_cmsis_rtos2.h"
#elif LV_USE_OS == LV_OS_RTTHREAD
#include "lv_rtthread.h"
#elif LV_USE_OS == LV_OS_WINDOWS
#include "lv_windows.h"
#elif LV_USE_OS == LV_OS_MQX
#include "lv_mqx.h"
#elif LV_USE_OS == LV_OS_SDL2
#include "lv_sdl2.h"
#elif LV_USE_OS == LV_OS_CUSTOM
#include LV_OS_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_THREAD_PRIO_LOWEST,
    LV_THREAD_PRIO_LOW,
    LV_THREAD_PRIO_MID,
    LV_THREAD_PRIO_HIGH,
    LV_THREAD_PRIO_HIGHEST,
} lv_thread_prio_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set it for `LV_SYSMON_GET_IDLE` to show the CPU usage
 * @return the idle percentage since the last call
 */
uint32_t lv_os_get_idle_percent(void);

#if LV_USE_OS != LV_OS_NONE

/*----------------------------------------
 * These functions needs to be implemented
 * for specific operating systems
 *---------------------------------------*/

/**
 * Create a new thread
 * @param thread        a variable in which the thread will be stored
 * @param name          the name of the thread
 * @param prio          priority of the thread
 * @param callback      function of the thread
 * @param stack_size    stack size in bytes
 * @param user_data     arbitrary data, will be available in the callback
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name,
                           lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
                           void * user_data);

/**
 * Delete a thread
 * @param thread        the thread to delete
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_delete(lv_thread_t * thread);

/**
 * Create a mutex
 * @param mutex         a variable in which the thread will be stored
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_mutex_init(lv_mutex_t * mutex);

/**
 * Lock a mutex
 * @param mutex         the mutex to lock
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_mutex_lock(lv_mutex_t * mutex);

/**
 * Lock a mutex from interrupt
 * @param mutex         the mutex to lock
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex);

/**
 * Unlock a mutex
 * @param mutex         the mutex to unlock
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_mutex_unlock(lv_mutex_t * mutex);

/**
 * Delete a mutex
 * @param mutex         the mutex to delete
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_mutex_delete(lv_mutex_t * mutex);

/**
 * Create a thread synchronization object
 * @param sync          a variable in which the sync will be stored
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync);

/**
 * Wait for a "signal" on a sync object
 * @param sync      a sync object
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync);

/**
 * Send a wake-up signal to a sync object
 * @param sync      a sync object
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync);

/**
 * Send a wake-up signal to a sync object from interrupt
 * @param sync      a sync object
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync);

/**
 * Delete a sync object
 * @param sync      a sync object to delete
 * @return          LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync);

/**
 * Lock LVGL's general mutex.
 * LVGL is not thread safe, so a mutex is used to avoid executing multiple LVGL functions at the same time
 * from different threads. It shall be called when calling LVGL functions from threads
 * different than lv_timer_handler's thread. It doesn't need to be called in LVGL events because
 * they are called from lv_timer_handler().
 * It is called internally in lv_timer_handler().
 */
void lv_lock(void);

/**
 * Same as `lv_lock()` but can be called from an interrupt.
 * @return              LV_RESULT_OK: success; LV_RESULT_INVALID: failure
 */
lv_result_t lv_lock_isr(void);

/**
 * The pair of `lv_lock()` and `lv_lock_isr()`.
 * It unlocks LVGL general mutex.
 * It is called internally in lv_timer_handler().
 */
void lv_unlock(void);

#else

/* Since compilation does not necessarily optimize cross-file empty functions well
 * (-O3 optimization alone is not enough unless LTO optimization is enabled),
 * In the absence of an operating system, use inline functions to help compile
 * optimizations and avoid the call overhead of the OS API to ensure no performance penalty.
 */

static inline lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name, lv_thread_prio_t prio,
                                         void (*callback)(void *), size_t stack_size, void * user_data)
{
    LV_UNUSED(thread);
    LV_UNUSED(name);
    LV_UNUSED(callback);
    LV_UNUSED(prio);
    LV_UNUSED(stack_size);
    LV_UNUSED(user_data);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    LV_UNUSED(thread);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

static inline lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

static inline void lv_lock(void)
{
    /*Do nothing*/
}

static inline lv_result_t lv_lock_isr(void)
{
    return LV_RESULT_OK;
}

static inline void lv_unlock(void)
{
    /*Do nothing*/
}

#endif /*LV_USE_OS != LV_OS_NONE*/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OS_H*/

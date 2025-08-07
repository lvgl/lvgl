/**
 * @file lv_chibios.h
 */

#ifndef LV_CHIBIOS_H
#define LV_CHIBIOS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lv_os.h"

#if LV_USE_OS == LV_OS_CHIBIOS

#include "ch.h"

typedef struct {
    void (*pvStartRoutine)(void *);
    void * pTaskArg;
    thread_t * pThreadHandle; /**< ChibiOS thread handle. */
} lv_thread_t;

typedef struct {
    bool is_initialized; /**< Set to true if this mutex is initialized. */
    mutex_t mtx;         /**< ChibiOS mutex object. */
} lv_mutex_t;

typedef struct {
    bool is_initialized; /**< Set to true once initialized. */
    /* Binary semaphore to wake threads (used in place of a task-notification or
       counting semaphore in FreeRTOS). */
    binary_semaphore_t bsem;
    mutex_t sync_mtx;         /**<  Mutex to protect the condition variable internal state. */
    uint32_t waiting_threads; /**< Number of threads currently waiting. */
    bool sync_signal;         /**< True when a signal has been issued. */
} lv_thread_sync_t;

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_OS == LV_OS_CHIBIOS */

#endif /* LV_CHIBIOS_H */
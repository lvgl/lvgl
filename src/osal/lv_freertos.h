/**
 * @file lv_freertos.h
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_FREERTOS_H
#define LV_FREERTOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_FREERTOS

#ifdef ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#else
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void (*pvStartRoutine)(void *);       /**< Application thread function. */
    void * pTaskArg;                      /**< Arguments for application thread function. */
    TaskHandle_t xTaskHandle;             /**< FreeRTOS task handle. */
} lv_thread_t;

typedef struct {
    BaseType_t xIsInitialized;            /**< Set to pdTRUE if this mutex is initialized, pdFALSE otherwise. */
    SemaphoreHandle_t xMutex;             /**< FreeRTOS mutex. */
} lv_mutex_t;

typedef struct {
    BaseType_t
    xIsInitialized;                       /**< Set to pdTRUE if this condition variable is initialized, pdFALSE otherwise. */
    BaseType_t xSyncSignal;               /**< Set to pdTRUE if the thread is signaled, pdFALSE otherwise. */
#if LV_USE_FREERTOS_TASK_NOTIFY
    TaskHandle_t xTaskToNotify;           /**< The task waiting to be signalled. NULL if nothing is waiting. */
#else
    SemaphoreHandle_t xCondWaitSemaphore; /**< Threads block on this semaphore in lv_thread_sync_wait. */
    uint32_t ulWaitingThreads;            /**< The number of threads currently waiting on this condition variable. */
    SemaphoreHandle_t xSyncMutex;         /**< Threads take this mutex before accessing the condition variable. */
#endif
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set it for `traceTASK_SWITCHED_IN()` as
 * `lv_freertos_task_switch_in(pxCurrentTCB->pcTaskName)`
 * to save the start time stamp of a task
 * @param name      the name of the which is switched in
 */
void lv_freertos_task_switch_in(const char * name);

/**
 * Set it for `traceTASK_SWITCHED_OUT()` as
 * `lv_freertos_task_switch_out()`
 * to save finish time stamp of a task
 */
void lv_freertos_task_switch_out(void);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_FREERTOS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FREERTOS_H*/

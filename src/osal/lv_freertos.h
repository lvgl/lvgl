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
#if LV_USE_OS == LV_OS_FREERTOS

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void (*pvStartRoutine)(void *);       /**< Application thread function. */
    void * xTaskArg;                      /**< Arguments for application thread function. */
    TaskHandle_t xTaskHandle;             /**< FreeRTOS task handle. */
} lv_thread_t;

typedef struct {
    BaseType_t xIsInitialized;            /**< Set to pdTRUE if this mutex is initialized, pdFALSE otherwise. */
    SemaphoreHandle_t xMutex;             /**< FreeRTOS mutex. */
} lv_mutex_t;

typedef struct {
    BaseType_t
    xIsInitialized;                       /**< Set to pdTRUE if this condition variable is initialized, pdFALSE otherwise. */
    SemaphoreHandle_t xCondWaitSemaphore; /**< Threads block on this semaphore in lv_thread_sync_wait. */
    uint32_t ulWaitingThreads;            /**< The number of threads currently waiting on this condition variable. */
    SemaphoreHandle_t xSyncMutex;         /**< Threads take this mutex before accessing the condition variable. */
    BaseType_t xSyncSignal;               /**< Set to pdTRUE if the thread is signaled, pdFALSE otherwise. */
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_FREERTOS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FREERTOS_H*/

/**
 * @file lv_freertos.h
 *
 */

/**
 * MIT License
 *
 * Copyright 2023 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
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
    void * (*pvStartRoutine)(void *);     /**< Application thread function. */
    void * xTaskArg;                      /**< Arguments for application thread function. */
    TaskHandle_t xTaskHandle;             /**< FreeRTOS task handle. */
    void * xReturn;                       /**< Return value of pvStartRoutine. */
} lv_thread_t;

typedef struct {
    BaseType_t xIsInitialized;            /**< Set to pdTRUE if this mutex is initialized, pdFALSE otherwise. */
    SemaphoreHandle_t xMutex;             /**< FreeRTOS mutex. */
} lv_mutex_t;

typedef struct {
    BaseType_t
    xIsInitialized;            /**< Set to pdTRUE if this condition variable is initialized, pdFALSE otherwise. */
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

#endif /*LV_USE_FREERTOS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FREERTOS_H*/

/**
 * @file lv_cmsis_rtos2.c
 *
 */

/*
 * Copyright (C) 2023 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_CMSIS_RTOS2

#include "../misc/lv_log.h"

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

lv_result_t lv_thread_init(lv_thread_t * thread, lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
                           void * user_data)
{
    static const osPriority_t prio_map[] = {
        [LV_THREAD_PRIO_LOWEST] = osPriorityLow,
        [LV_THREAD_PRIO_LOW] = osPriorityBelowNormal,
        [LV_THREAD_PRIO_MID] = osPriorityNormal,
        [LV_THREAD_PRIO_HIGH] = osPriorityHigh,
        [LV_THREAD_PRIO_HIGHEST] = osPriorityRealtime7,
    };

    osThreadAttr_t c_tThreadAttribute = {
        .stack_size = stack_size,
        .priority = prio_map[prio],
    };

    *thread = osThreadNew(callback, user_data, &c_tThreadAttribute);

    if(NULL == *thread) {
        LV_LOG_WARN("Error: Failed to create a cmsis-rtos2 thread.");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;

}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    osThreadDetach(*thread);
    osStatus_t status = osThreadTerminate(*thread);
    if(status == osOK) {
        return LV_RESULT_OK;
    }
    return LV_RESULT_INVALID;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    const osMutexAttr_t Thread_Mutex_attr = {
        "LVGLMutex",
        osMutexRecursive | osMutexPrioInherit | osMutexRobust,
    };

    *mutex = osMutexNew(&Thread_Mutex_attr);
    if(*mutex == NULL)  {
        LV_LOG_WARN("Error: failed to create cmsis-rtos mutex");
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;

}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    osStatus_t status = osMutexAcquire(*mutex, 0U);
    if(status != osOK)  {
        LV_LOG_WARN("Error: failed to lock cmsis-rtos2 mutex %d", (int)status);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    osStatus_t status = osMutexAcquire(*mutex, 0U);
    if(status != osOK)  {
        LV_LOG_WARN("Error: failed to lock cmsis-rtos2 mutex in an ISR %d", (int)status);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    osStatus_t status = osMutexRelease(*mutex);
    if(status != osOK)  {
        LV_LOG_WARN("Error: failed to release cmsis-rtos2 mutex %d", (int)status);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    osStatus_t status = osMutexDelete(*mutex);
    if(status != osOK)  {
        LV_LOG_WARN("Error: failed to delete cmsis-rtos2 mutex %d", (int)status);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    *sync = osEventFlagsNew(NULL);
    if(NULL == *sync) {
        LV_LOG_WARN("Error: failed to create a cmsis-rtos2 EventFlag");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    uint32_t ret = osEventFlagsWait(*sync, 0x01, osFlagsWaitAny, osWaitForever);
    if(ret & (1 << 31)) {
        LV_LOG_WARN("Error: failed to wait a cmsis-rtos2 EventFlag %d", ret);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    uint32_t ret = osEventFlagsSet(*sync, 0x01);
    if(ret & (1 << 31)) {
        LV_LOG_WARN("Error: failed to set a cmsis-rtos2 EventFlag %d", ret);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    osStatus_t status = osEventFlagsDelete(*sync);
    if(status != osOK)  {
        LV_LOG_WARN("Error: failed to delete a cmsis-rtos2 EventFlag %d", (int)status);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OS == LV_OS_CMSIS_RTOS2*/

/**
 * @file lv_mqx.h
 *
 */

#ifndef LV_MQX_H
#define LV_MQX_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_MQX

#include "mqx.h"
#include "mutex.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    _task_id tid;                         /**< MQX task id. */
} lv_thread_t;

typedef struct {
    MUTEX_STRUCT mutex;                   /**< MQX mutex. */
} lv_mutex_t;

typedef struct {
    LWSEM_STRUCT sem;                     /**< MQX semaphore. */
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_MQX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_MQX_H*/

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
#include "lv_os_private.h"

#if LV_USE_OS == LV_OS_MQX

#include "mqx.h"
#include "mutex.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef _task_id lv_thread_t;

typedef MUTEX_STRUCT lv_mutex_t;

typedef LWSEM_STRUCT lv_thread_sync_t;

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

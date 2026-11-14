/**
 * @file lv_rtthread.h
 *
 */

#ifndef LV_RTTHREAD_H
#define LV_RTTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_OS == LV_OS_RTTHREAD

#include <rtthread.h>
#include LV_STDBOOL_INCLUDE

#if !defined(RT_IPC_CMD_SET_VLIMIT) && !defined(RT_USING_EVENT)
#error "LV_OS_RTTHREAD requires RT_USING_EVENT when semaphore value limits are unavailable"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    rt_thread_t thread;
} lv_thread_t;

typedef struct {
    rt_mutex_t mutex;
} lv_mutex_t;

typedef struct {
#ifdef RT_IPC_CMD_SET_VLIMIT
    rt_sem_t sem;
#else
    rt_event_t event;
#endif
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_RTTHREAD*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_RTTHREAD_H*/

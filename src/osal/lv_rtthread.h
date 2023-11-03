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
#include <stdbool.h>

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
    rt_sem_t sem;
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

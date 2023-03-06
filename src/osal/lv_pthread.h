/**
 * @file lv_pthread.h
 *
 */

#ifndef LV_PTHREAD_H
#define LV_PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#if LV_USE_PTHREAD

#include <pthread.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    pthread_t thread;
    void (*callback)(void *);
    void * user_data;
} lv_thread_t;

typedef pthread_mutex_t lv_mutex_t;

typedef struct {
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_PTHREAD*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PTHREAD_H*/

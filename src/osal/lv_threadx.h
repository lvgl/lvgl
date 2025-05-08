#ifndef LV_THREADX_H
#define LV_THREADX_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_OS == LV_OS_THREADX

#include "tx_api.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    TX_THREAD thread;
} lv_thread_t;

typedef struct {
    TX_MUTEX mutex;
} lv_mutex_t;

typedef struct {
    TX_SEMAPHORE sem;
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_THREADX*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_THREADX_H*/

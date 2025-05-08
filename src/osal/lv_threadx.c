/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_THREADX

/*********************
 *      DEFINES
 *********************/

#define THREAD_TIMESLICE 20U

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
lv_thread_init(lv_thread_t * thread, lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
               void * user_data)
{
    uint8_t * data = malloc(stack_size);
    if(data == NULL) {
        return LV_RESULT_INVALID;
    }

    UINT result = tx_thread_create(&thread->thread,
                                   "lv_thread",
                                   (VOID (*)(ULONG))callback,
                                   (ULONG)user_data,
                                   data,
                                   stack_size,
                                   prio,
                                   prio,
                                   THREAD_TIMESLICE,
                                   TX_AUTO_START);

    if(result != TX_SUCCESS) {
        free(data);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    return (tx_thread_delete(&thread->thread) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    return (tx_mutex_create(&mutex->mutex, "mutex", 0) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    return (tx_mutex_get(&mutex->mutex, TX_WAIT_FOREVER) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    return (tx_mutex_get(&mutex->mutex, TX_WAIT_FOREVER) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    return (tx_mutex_put(&mutex->mutex) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    return (tx_mutex_delete(&mutex->mutex) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    return (tx_semaphore_create(&sync->sem, "sem", 0) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    return (tx_semaphore_get(&sync->sem, TX_WAIT_FOREVER) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    return (tx_semaphore_put(&sync->sem) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    return (tx_semaphore_delete(&sync->sem) == TX_SUCCESS) ? LV_RESULT_OK : LV_RESULT_INVALID;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OS == LV_OS_THREADX*/

/**
 * @file lv_pthread.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_PTHREAD

#include <errno.h>
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
static void * generic_callback(void * user_data);

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
    LV_UNUSED(prio);
    LV_UNUSED(stack_size);
    thread->callback = callback;
    thread->user_data = user_data;
    pthread_create(&thread->thread, NULL, generic_callback, thread);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    int ret = pthread_join(thread->thread, NULL);
    if(ret != 0) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    int ret = pthread_mutex_init(mutex, NULL);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    int ret = pthread_mutex_lock(mutex);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    int ret = pthread_mutex_lock(mutex);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    int ret = pthread_mutex_unlock(mutex);
    if(ret) {
        LV_LOG_WARN("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    pthread_mutex_destroy(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    pthread_mutex_init(&sync->mutex, 0);
    pthread_cond_init(&sync->cond, 0);
    sync->v = false;
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    pthread_mutex_lock(&sync->mutex);
    while(!sync->v) {
        pthread_cond_wait(&sync->cond, &sync->mutex);
    }
    sync->v = false;
    pthread_mutex_unlock(&sync->mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    pthread_mutex_lock(&sync->mutex);
    sync->v = true;
    pthread_cond_signal(&sync->cond);
    pthread_mutex_unlock(&sync->mutex);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    pthread_mutex_destroy(&sync->mutex);
    pthread_cond_destroy(&sync->cond);
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * generic_callback(void * user_data)
{
    lv_thread_t * thread = user_data;
    thread->callback(thread->user_data);
    return NULL;
}

#endif /*LV_USE_OS == LV_OS_PTHREAD*/

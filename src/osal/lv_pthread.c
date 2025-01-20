/**
 * @file lv_pthread.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_PTHREAD

#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
#include "../misc/lv_log.h"

#define LV_UPTIME_MONITOR_FILE "/proc/uptime"

static uint32_t original_uptime_s, original_idletime_s;
static int original_uptime_ms, original_idletime_ms;
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

static void lv_os_get_delta(uint32_t now_s, int now_ms, uint32_t original_s,
                            int original_ms, uint32_t * delta_s, int * delta_ms);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name,
                           lv_thread_prio_t prio, void (*callback)(void *),
                           size_t stack_size, void * user_data)
{
    LV_UNUSED(name);
    LV_UNUSED(prio);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);
    thread->callback = callback;
    thread->user_data = user_data;
    pthread_create(&thread->thread, &attr, generic_callback, thread);
    pthread_attr_destroy(&attr);
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
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int ret = pthread_mutex_init(mutex, &attr);
    pthread_mutexattr_destroy(&attr);

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

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

uint32_t lv_os_get_idle_percent(void)
{
    FILE *fp = fopen(LV_UPTIME_MONITOR_FILE, "r");

    if(!fp) {
        LV_LOG_WARN("Failed to open " LV_UPTIME_MONITOR_FILE);
        return UINT_MAX;
    }
    // UINT32_MAX seconds > 136 years
    uint32_t uptime_s, idletime_s;

    // Range is [0:100[
    int uptime_ms, idletime_ms;

    int err = fscanf(fp,
                     "%" PRIu32 ".%d"
                     " %" PRIu32 ".%d",
                     &uptime_s, &uptime_ms, &idletime_s, &idletime_ms);
    fclose(fp);

    if(original_uptime_s == 0) {
        original_uptime_s = uptime_s;
        original_uptime_ms = uptime_ms;
        original_idletime_s = idletime_s;
        original_idletime_ms = idletime_ms;
        return 0;
    }

    uint32_t delta_uptime_s, delta_idletime_s;
    int delta_uptime_ms, delta_idletime_ms;

    // Calculate the delta first to avoid overflowing
    lv_os_get_delta(uptime_s, uptime_ms, original_uptime_s,
                    original_uptime_ms, &delta_uptime_s, &delta_uptime_ms);

    lv_os_get_delta(idletime_s, idletime_ms, original_idletime_s,
                    original_idletime_ms, &delta_idletime_s,
                    &delta_idletime_ms);

    uint32_t total_ms = delta_uptime_ms + delta_idletime_ms;
    uint32_t total_s = delta_uptime_s + delta_idletime_s;

    if(total_ms >= 100) {
        total_s += 1;
        total_ms -= 100;
    }

    return ((delta_idletime_s * 100 + delta_idletime_ms) * 100) /
           (total_s * 100 + total_ms);
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

static void lv_os_get_delta(uint32_t now_s, int now_ms, uint32_t original_s,
                            int original_ms, uint32_t * delta_s, int * delta_ms)
{
    *delta_s = now_s - original_s;
    *delta_ms = now_ms - original_ms;

    if(*delta_ms < 0) {
        *delta_s -= 1;
        *delta_ms += 100;
    }
}

#endif /*LV_USE_OS == LV_OS_PTHREAD*/

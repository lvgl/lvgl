/**
 * @file lv_sdl2.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_SDL2

#include <errno.h>
#include "../misc/lv_log.h"

#ifndef __linux__
    #include "../misc/lv_timer.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int generic_callback(void * user_data);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * thread, const char * const name, lv_thread_prio_t prio,
                           void (*callback)(void *), size_t stack_size, void * user_data)
{
    LV_UNUSED(prio);
    thread->callback = callback;
    thread->user_data = user_data;
    thread->thread = SDL_CreateThreadWithStackSize(generic_callback, name, stack_size, thread);
    if(thread->thread == NULL) {
        LV_LOG_ERROR("Error: %s", SDL_GetError());
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    int ret;
    SDL_WaitThread(thread->thread, &ret);
    if(ret != 0) {
        LV_LOG_ERROR("Error: %d", ret);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    *mutex = SDL_CreateMutex();

    if(*mutex == NULL) {
        LV_LOG_ERROR("Error: %s", SDL_GetError());
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    int ret = SDL_LockMutex(*mutex);
    if(ret) {
        LV_LOG_ERROR("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    int ret = SDL_LockMutex(*mutex);
    if(ret) {
        LV_LOG_ERROR("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    int ret = SDL_UnlockMutex(*mutex);
    if(ret) {
        LV_LOG_ERROR("Error: %d", ret);
        return LV_RESULT_INVALID;
    }
    else {
        return LV_RESULT_OK;
    }
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    SDL_DestroyMutex(*mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    sync->mutex = SDL_CreateMutex();
    if(sync->mutex == NULL) {
        LV_LOG_ERROR("Error: %s", SDL_GetError());
        return LV_RESULT_INVALID;
    }
    sync->cond = SDL_CreateCond();
    if(sync->cond == NULL) {
        LV_LOG_ERROR("Error: %s", SDL_GetError());
        return LV_RESULT_INVALID;
    }
    sync->v = false;
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    SDL_LockMutex(sync->mutex);
    while(!sync->v) {
        SDL_CondWait(sync->cond, sync->mutex);
    }
    sync->v = false;
    SDL_UnlockMutex(sync->mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    SDL_LockMutex(sync->mutex);
    sync->v = true;
    SDL_CondSignal(sync->cond);
    SDL_UnlockMutex(sync->mutex);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    SDL_DestroyMutex(sync->mutex);
    SDL_DestroyCond(sync->cond);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

#ifndef __linux__
uint32_t lv_os_get_idle_percent(void)
{
    return lv_timer_get_idle();
}
#endif

void lv_sleep_ms(uint32_t ms)
{
    SDL_Delay(ms);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int generic_callback(void * user_data)
{
    lv_thread_t * thread = user_data;
    thread->callback(thread->user_data);
    return 0;
}

#endif /*LV_USE_OS == LV_OS_SDL2*/

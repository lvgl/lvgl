/**
 * @file lv_windows.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_os.h"

#if LV_USE_OS == LV_OS_WINDOWS

#include <process.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    void (*callback)(void *);
    void * user_data;
} lv_thread_init_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static unsigned __stdcall thread_start_routine(void * parameter);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(
    lv_thread_t * thread,
    lv_thread_prio_t prio,
    void (*callback)(void *),
    size_t stack_size,
    void * user_data)
{
    if(!thread) {
        return LV_RESULT_INVALID;
    }

    static const int prio_map[] = {
        [LV_THREAD_PRIO_LOWEST] = THREAD_PRIORITY_LOWEST,
        [LV_THREAD_PRIO_LOW] = THREAD_PRIORITY_BELOW_NORMAL,
        [LV_THREAD_PRIO_MID] = THREAD_PRIORITY_NORMAL,
        [LV_THREAD_PRIO_HIGH] = THREAD_PRIORITY_ABOVE_NORMAL,
        [LV_THREAD_PRIO_HIGHEST] = THREAD_PRIORITY_HIGHEST,
    };

    lv_thread_init_data_t * init_data =
        (lv_thread_init_data_t *)(malloc(
                                      sizeof(lv_thread_init_data_t)));
    if(!init_data) {
        return LV_RESULT_INVALID;
    }
    init_data->callback = callback;
    init_data->user_data = user_data;

    /*
    Reference: https://learn.microsoft.com/en-us/windows/win32/api
               /processthreadsapi/nf-processthreadsapi-createthread

    A thread in an executable that calls the C run-time library (CRT) should
    use the _beginthreadex and _endthreadex functions for thread management
    rather than CreateThread and ExitThread; this requires the use of the
    multithreaded version of the CRT. If a thread created using CreateThread
    calls the CRT, the CRT may terminate the process in low-memory conditions.
    */
    *thread = (HANDLE)(_beginthreadex(
                           NULL,
                           (unsigned)(stack_size),
                           thread_start_routine,
                           init_data,
                           0,
                           NULL));
    if(!*thread) {
        return LV_RESULT_INVALID;
    }

    /*
    Try to set the thread priority. (Not mandatory for creating a new thread.)
    */
    SetThreadPriority(*thread, prio_map[prio]);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    lv_result_t result = LV_RESULT_OK;

    if(!TerminateThread(thread, 0)) {
        result = LV_RESULT_INVALID;
    }

    CloseHandle(thread);

    return result;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    InitializeCriticalSection(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    EnterCriticalSection(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    EnterCriticalSection(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    LeaveCriticalSection(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    DeleteCriticalSection(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    if(!sync) {
        return LV_RESULT_INVALID;
    }

    InitializeCriticalSection(&sync->cs);
    InitializeConditionVariable(&sync->cv);
    sync->v = false;

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    if(!sync) {
        return LV_RESULT_INVALID;
    }

    EnterCriticalSection(&sync->cs);
    while(!sync->v) {
        SleepConditionVariableCS(&sync->cv, &sync->cs, INFINITE);
    }
    sync->v = false;
    LeaveCriticalSection(&sync->cs);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    if(!sync) {
        return LV_RESULT_INVALID;
    }

    EnterCriticalSection(&sync->cs);
    sync->v = true;
    WakeConditionVariable(&sync->cv);
    LeaveCriticalSection(&sync->cs);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    if(!sync) {
        return LV_RESULT_INVALID;
    }

    DeleteCriticalSection(&sync->cs);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    return LV_RESULT_INVALID;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static unsigned __stdcall thread_start_routine(void * parameter)
{
    lv_thread_init_data_t * init_data = (lv_thread_init_data_t *)(parameter);
    if(init_data) {
        init_data->callback(init_data->user_data);
        free(init_data);
    }

    return 0;
}

#endif /*LV_USE_OS == LV_OS_WINDOWS*/

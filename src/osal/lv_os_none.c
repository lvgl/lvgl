/**
 * @file lv_os_none.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os.h"

#if LV_USE_OS == LV_OS_NONE
#include "../misc/lv_types.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/

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

lv_result_t lv_thread_init(lv_thread_t * thread, lv_thread_prio_t prio, void (*callback)(void *), size_t stack_size,
                           void * user_data)
{
    LV_UNUSED(thread);
    LV_UNUSED(callback);
    LV_UNUSED(prio);
    LV_UNUSED(stack_size);
    LV_UNUSED(user_data);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_thread_delete(lv_thread_t * thread)
{
    LV_UNUSED(thread);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_mutex_init(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * mutex)
{
    LV_UNUSED(mutex);
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_wait(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * sync)
{
    LV_UNUSED(sync);
    LV_ASSERT(0);
    return LV_RESULT_INVALID;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OS == LV_OS_NONE*/

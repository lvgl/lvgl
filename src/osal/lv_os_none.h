/**
 * @file lv_os_none.h
 *
 */

#ifndef LV_OS_NONE_H
#define LV_OS_NONE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_OS == LV_OS_NONE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef int lv_mutex_t;
typedef int lv_thread_t;
typedef int lv_thread_sync_t;

/* Since the compiler cannot guarantee to optimize empty function calls well
 * (-O3 optimization alone is not enough unless LTO optimization is enabled),
 * macros are used to force the removal of OS API calls in the absence of OS to ensure no performance loss.
 */

#define lv_thread_init(thread, prio, callback, stack_size, user_data)   \
    do {                                                                \
        LV_UNUSED(thread);                                              \
        LV_UNUSED(prio);                                                \
        LV_UNUSED(callback);                                            \
        LV_UNUSED(stack_size);                                          \
        LV_UNUSED(user_data);                                           \
    } while (0)

#define lv_thread_delete(thread) LV_UNUSED(thread)

#define lv_mutex_init(mutex) LV_UNUSED(mutex)

#define lv_mutex_lock(mutex) LV_UNUSED(mutex)

#define lv_mutex_lock_isr(mutex) LV_UNUSED(mutex)

#define lv_mutex_unlock(mutex) LV_UNUSED(mutex)

#define lv_mutex_delete(mutex) LV_UNUSED(mutex)

#define lv_thread_sync_init(thread_sync) LV_UNUSED(thread_sync)

#define lv_thread_sync_wait(thread_sync) LV_UNUSED(thread_sync)

#define lv_thread_sync_signal(thread_sync) LV_UNUSED(thread_sync)

#define lv_thread_sync_signal_isr(thread_sync) LV_UNUSED(thread_sync)

#define lv_thread_sync_delete(thread_sync) LV_UNUSED(thread_sync)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_NONE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OS_NONE_H*/

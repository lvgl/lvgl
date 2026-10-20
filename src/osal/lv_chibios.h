/**
 * @file lv_chibios.h
 * ChibiOS/RT OSAL, adapted from https://github.com/lvgl/lvgl/pull/8198.
 */
#ifndef LV_CHIBIOS_H
#define LV_CHIBIOS_H

#include "../lvgl_public.h"

#if LV_USE_OS == LV_OS_CHIBIOS
#include "ch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*callback)(void *);
    void * user_data;
    thread_t * handle;
} lv_thread_t;

typedef mutex_t lv_mutex_t;
typedef binary_semaphore_t lv_thread_sync_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif
#endif /*LV_USE_OS == LV_OS_CHIBIOS*/
#endif /*LV_CHIBIOS_H*/

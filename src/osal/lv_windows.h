/**
 * @file lv_windows.h
 *
 */

#ifndef LV_WINDOWS_H
#define LV_WINDOWS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#if LV_USE_OS == LV_OS_WINDOWS

#include <Windows.h>
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef HANDLE lv_thread_t;

typedef CRITICAL_SECTION lv_mutex_t;

typedef struct {
    CRITICAL_SECTION cs;
    CONDITION_VARIABLE cv;
    bool v;
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_WINDOWS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_WINDOWS_H*/

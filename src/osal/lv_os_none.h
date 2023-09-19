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

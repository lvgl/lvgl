/**
 * @file lv_cmsis_rtos2.h
 *
 */

/*
 * Copyright (C) 2023 Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LV_CMSIS_RTOS2_H
#define LV_CMSIS_RTOS2_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_OS == LV_OS_CMSIS_RTOS2

#include "cmsis_os2.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef osThreadId_t lv_thread_t;

typedef osMutexId_t lv_mutex_t;

typedef osEventFlagsId_t lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_CMSIS_RTOS2*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OS_CMSIS_RTOS2*/

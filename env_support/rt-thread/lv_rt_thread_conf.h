/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-15     Meco Man     The first version
 */

#ifndef LV_RT_THREAD_CONF_H
#define LV_RT_THREAD_CONF_H

#ifdef __RTTHREAD__

#include <lvgl.h>

#define LV_RTTHREAD_INCLUDE <rtthread.h>
#include LV_RTTHREAD_INCLUDE

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/

#ifdef RT_USING_HEAP
    #define LV_USE_STDLIB_MALLOC    LV_STDLIB_RTTHREAD
#endif

#define LV_USE_STDLIB_STRING    LV_STDLIB_RTTHREAD

#if LV_USE_FLOAT == 0
    #define LV_USE_STDLIB_SPRINTF   LV_STDLIB_RTTHREAD
#endif

/*=================
 * OPERATING SYSTEM
 *=================*/

#define LV_USE_OS   LV_OS_RTTHREAD

/*-------------
 * Asserts
 *-----------*/

#define LV_ASSERT_HANDLER_INCLUDE LV_RTTHREAD_INCLUDE
#define LV_ASSERT_HANDLER RT_ASSERT(0);

/*=====================
 *  COMPILER SETTINGS
 *====================*/

#ifdef ARCH_CPU_BIG_ENDIAN
    #define LV_BIG_ENDIAN_SYSTEM 1
#else
    #define LV_BIG_ENDIAN_SYSTEM 0
#endif

#ifdef rt_align /* >= RT-Thread v5.0.0 */
    #define LV_ATTRIBUTE_MEM_ALIGN rt_align(RT_ALIGN_SIZE)
#else
    #define LV_ATTRIBUTE_MEM_ALIGN ALIGN(RT_ALIGN_SIZE)
#endif

/*==================
* EXAMPLES
*==================*/

#ifdef PKG_LVGL_USING_EXAMPLES
    #define LV_BUILD_EXAMPLES 1
#endif

/*--END OF LV_RT_THREAD_CONF_H--*/

#endif /*__RTTHREAD__*/

#endif /*LV_CONF_H*/

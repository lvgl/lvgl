/*
 * Copyright (c) 2020 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef __ZEPHYR__

#include <autoconf.h>

#define LV_MEM_CUSTOM 1

#define LV_MEMCPY_MEMSET_STD 1

#ifdef CONFIG_LVGL_MEM_POOL_HEAP_KERNEL

#define LV_MEM_CUSTOM_INCLUDE   "kernel.h"
#define LV_MEM_CUSTOM_ALLOC     k_malloc
#define LV_MEM_CUTOM_FREE       k_free

#elif defined(CONFIG_LVGL_MEM_POOL_HEAP_LIB_C)

#define LV_MEM_CUSTOM_INCLUDE   "stdlib.h"
#define LV_MEM_CUSTOM_ALLOC     malloc
#define LV_MEM_CUTOM_FREE       free

#else

#define LV_MEM_CUSTOM_INCLUDE   "lvgl_mem.h"
#define LV_MEM_CUSTOM_ALLOC     lvgl_malloc
#define LV_MEM_CUTOM_FREE       lvgl_free

#endif

#define LV_ENABLE_GC 0

#define LV_TICK_CUSTOM                  1
#define LV_TICK_CUSTOM_INCLUDE          "kernel.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR    (k_uptime_get_32())

#define LV_SPRINTF_CUSTOM 1

#if LV_SPRINTF_CUSTOM
#define LV_SPRINTF_INCLUDE  "stdio.h"
#define lv_snprintf         snprintf
#define lv_vsnprintf        vsnprintf
#endif

#endif

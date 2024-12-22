/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * This file ensures all defines of lv_conf.h have a default value.
 */

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/* Config options */
#define LV_OS_NONE          0
#define LV_OS_PTHREAD       1
#define LV_OS_FREERTOS      2
#define LV_OS_CMSIS_RTOS2   3
#define LV_OS_RTTHREAD      4
#define LV_OS_WINDOWS       5
#define LV_OS_MQX           6
#define LV_OS_SDL2          7
#define LV_OS_CUSTOM        255

#define LV_STDLIB_BUILTIN           0
#define LV_STDLIB_CLIB              1
#define LV_STDLIB_MICROPYTHON       2
#define LV_STDLIB_RTTHREAD          3
#define LV_STDLIB_CUSTOM            255

#define LV_DRAW_SW_ASM_NONE         0
#define LV_DRAW_SW_ASM_NEON         1
#define LV_DRAW_SW_ASM_HELIUM       2
#define LV_DRAW_SW_ASM_CUSTOM       255

#define LV_NEMA_HAL_CUSTOM          0
#define LV_NEMA_HAL_STM32           1

/** Handle special Kconfig options. */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #if defined(CONFIG_LV_CONF_SKIP) && !defined(LV_CONF_SKIP)
        #define LV_CONF_SKIP
    #endif
#endif

/* If "lv_conf.h" is available from here try to use it later. */
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/* If lv_conf.h is not skipped, include it. */
#if !defined(LV_CONF_SKIP) || defined(LV_CONF_PATH)
    #ifdef LV_CONF_PATH                           /* If there is a path defined for lv_conf.h, use it */
        #include LV_CONF_PATH                     /* Note: Make sure to define custom CONF_PATH as a string */
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /* Or simply include lv_conf.h is enabled. */
        #include "lv_conf.h"
    #else
        #include "../../lv_conf.h"                /* Else assume lv_conf.h is next to the lvgl folder. */
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define LV_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if  0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*====================
   COLOR SETTINGS
 *====================*/

/** Color depth: 1 (I1), 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#ifndef LV_COLOR_DEPTH
    #ifdef CONFIG_LV_COLOR_DEPTH
        #define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
    #else
        #define LV_COLOR_DEPTH 16
    #endif
#endif

/*=========================
   STDLIB WRAPPER SETTINGS
 *=========================*/

/** Possible values
 * - LV_STDLIB_BUILTIN:     LVGL's built in implementation
 * - LV_STDLIB_CLIB:        Standard C functions, like malloc, strlen, etc
 * - LV_STDLIB_MICROPYTHON: MicroPython implementation
 * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
 * - LV_STDLIB_CUSTOM:      Implement the functions externally
 */
#ifndef LV_USE_STDLIB_MALLOC
    #ifdef CONFIG_LV_USE_STDLIB_MALLOC
        #define LV_USE_STDLIB_MALLOC CONFIG_LV_USE_STDLIB_MALLOC
    #else
        #define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN
    #endif
#endif

/** Possible values
 * - LV_STDLIB_BUILTIN:     LVGL's built in implementation
 * - LV_STDLIB_CLIB:        Standard C functions, like malloc, strlen, etc
 * - LV_STDLIB_MICROPYTHON: MicroPython implementation
 * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
 * - LV_STDLIB_CUSTOM:      Implement the functions externally
 */
#ifndef LV_USE_STDLIB_STRING
    #ifdef CONFIG_LV_USE_STDLIB_STRING
        #define LV_USE_STDLIB_STRING CONFIG_LV_USE_STDLIB_STRING
    #else
        #define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN
    #endif
#endif

/** Possible values
 * - LV_STDLIB_BUILTIN:     LVGL's built in implementation
 * - LV_STDLIB_CLIB:        Standard C functions, like malloc, strlen, etc
 * - LV_STDLIB_MICROPYTHON: MicroPython implementation
 * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
 * - LV_STDLIB_CUSTOM:      Implement the functions externally
 */
#ifndef LV_USE_STDLIB_SPRINTF
    #ifdef CONFIG_LV_USE_STDLIB_SPRINTF
        #define LV_USE_STDLIB_SPRINTF CONFIG_LV_USE_STDLIB_SPRINTF
    #else
        #define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN
    #endif
#endif

#ifndef LV_STDINT_INCLUDE
    #ifdef CONFIG_LV_STDINT_INCLUDE
        #define LV_STDINT_INCLUDE CONFIG_LV_STDINT_INCLUDE
    #else
        #define LV_STDINT_INCLUDE       <stdint.h>
    #endif
#endif
#ifndef LV_STDDEF_INCLUDE
    #ifdef CONFIG_LV_STDDEF_INCLUDE
        #define LV_STDDEF_INCLUDE CONFIG_LV_STDDEF_INCLUDE
    #else
        #define LV_STDDEF_INCLUDE       <stddef.h>
    #endif
#endif
#ifndef LV_STDBOOL_INCLUDE
    #ifdef CONFIG_LV_STDBOOL_INCLUDE
        #define LV_STDBOOL_INCLUDE CONFIG_LV_STDBOOL_INCLUDE
    #else
        #define LV_STDBOOL_INCLUDE      <stdbool.h>
    #endif
#endif
#ifndef LV_INTTYPES_INCLUDE
    #ifdef CONFIG_LV_INTTYPES_INCLUDE
        #define LV_INTTYPES_INCLUDE CONFIG_LV_INTTYPES_INCLUDE
    #else
        #define LV_INTTYPES_INCLUDE     <inttypes.h>
    #endif
#endif
#ifndef LV_LIMITS_INCLUDE
    #ifdef CONFIG_LV_LIMITS_INCLUDE
        #define LV_LIMITS_INCLUDE CONFIG_LV_LIMITS_INCLUDE
    #else
        #define LV_LIMITS_INCLUDE       <limits.h>
    #endif
#endif
#ifndef LV_STDARG_INCLUDE
    #ifdef CONFIG_LV_STDARG_INCLUDE
        #define LV_STDARG_INCLUDE CONFIG_LV_STDARG_INCLUDE
    #else
        #define LV_STDARG_INCLUDE       <stdarg.h>
    #endif
#endif

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** Size of memory available for `lv_malloc()` in bytes (>= 2kB) */
    #ifndef LV_MEM_SIZE
        #ifdef CONFIG_LV_MEM_SIZE
            #define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
        #else
            #define LV_MEM_SIZE (64 * 1024U)          /**< [bytes] */
        #endif
    #endif

    /** Size of the memory expand for `lv_malloc()` in bytes */
    #ifndef LV_MEM_POOL_EXPAND_SIZE
        #ifdef CONFIG_LV_MEM_POOL_EXPAND_SIZE
            #define LV_MEM_POOL_EXPAND_SIZE CONFIG_LV_MEM_POOL_EXPAND_SIZE
        #else
            #define LV_MEM_POOL_EXPAND_SIZE 0
        #endif
    #endif

    /** Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too. */
    #ifndef LV_MEM_ADR
        #ifdef CONFIG_LV_MEM_ADR
            #define LV_MEM_ADR CONFIG_LV_MEM_ADR
        #else
            #define LV_MEM_ADR 0     /**< 0: unused*/
        #endif
    #endif
    /* Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc */
    #if LV_MEM_ADR == 0
        #ifndef LV_MEM_POOL_INCLUDE
            #ifdef CONFIG_LV_MEM_POOL_INCLUDE
                #define LV_MEM_POOL_INCLUDE CONFIG_LV_MEM_POOL_INCLUDE
            #else
                #undef LV_MEM_POOL_INCLUDE
            #endif
        #endif
        #ifndef LV_MEM_POOL_ALLOC
            #ifdef CONFIG_LV_MEM_POOL_ALLOC
                #define LV_MEM_POOL_ALLOC CONFIG_LV_MEM_POOL_ALLOC
            #else
                #undef LV_MEM_POOL_ALLOC
            #endif
        #endif
    #endif
#endif  /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

/*====================
   HAL SETTINGS
 *====================*/

/** Default display refresh, input device read and animation step period. */
#ifndef LV_DEF_REFR_PERIOD
    #ifdef CONFIG_LV_DEF_REFR_PERIOD
        #define LV_DEF_REFR_PERIOD CONFIG_LV_DEF_REFR_PERIOD
    #else
        #define LV_DEF_REFR_PERIOD  33      /**< [ms] */
    #endif
#endif

/** Default Dots Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 * (Not so important, you can adjust it to modify default sizes and spaces.) */
#ifndef LV_DPI_DEF
    #ifdef CONFIG_LV_DPI_DEF
        #define LV_DPI_DEF CONFIG_LV_DPI_DEF
    #else
        #define LV_DPI_DEF 130              /**< [px/inch] */
    #endif
#endif

/*=================
 * OPERATING SYSTEM
 *=================*/
/** Select operating system to use. Possible options:
 * - LV_OS_NONE
 * - LV_OS_PTHREAD
 * - LV_OS_FREERTOS
 * - LV_OS_CMSIS_RTOS2
 * - LV_OS_RTTHREAD
 * - LV_OS_WINDOWS
 * - LV_OS_MQX
 * - LV_OS_SDL2
 * - LV_OS_CUSTOM */
#ifndef LV_USE_OS
    #ifdef CONFIG_LV_USE_OS
        #define LV_USE_OS CONFIG_LV_USE_OS
    #else
        #define LV_USE_OS   LV_OS_NONE
    #endif
#endif

#if LV_USE_OS == LV_OS_CUSTOM
    #ifndef LV_OS_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_OS_CUSTOM_INCLUDE
            #define LV_OS_CUSTOM_INCLUDE CONFIG_LV_OS_CUSTOM_INCLUDE
        #else
            #define LV_OS_CUSTOM_INCLUDE <stdint.h>
        #endif
    #endif
#endif
#if LV_USE_OS == LV_OS_FREERTOS
	/*
	 * Unblocking an RTOS task with a direct notification is 45% faster and uses less RAM
	 * than unblocking a task using an intermediary object such as a binary semaphore.
	 * RTOS task notifications can only be used when there is only one task that can be the recipient of the event.
	 */
	#ifndef LV_USE_FREERTOS_TASK_NOTIFY
	    #ifdef LV_KCONFIG_PRESENT
	        #ifdef CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
	            #define LV_USE_FREERTOS_TASK_NOTIFY CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
	        #else
	            #define LV_USE_FREERTOS_TASK_NOTIFY 0
	        #endif
	    #else
	        #define LV_USE_FREERTOS_TASK_NOTIFY 1
	    #endif
	#endif
#endif

/*========================
 * RENDERING CONFIGURATION
 *========================*/

/** Align stride of all layers and images to this bytes */
#ifndef LV_DRAW_BUF_STRIDE_ALIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
            #define LV_DRAW_BUF_STRIDE_ALIGN CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
        #else
            #define LV_DRAW_BUF_STRIDE_ALIGN 0
        #endif
    #else
        #define LV_DRAW_BUF_STRIDE_ALIGN                1
    #endif
#endif

/** Align start address of draw_buf addresses to this bytes*/
#ifndef LV_DRAW_BUF_ALIGN
    #ifdef CONFIG_LV_DRAW_BUF_ALIGN
        #define LV_DRAW_BUF_ALIGN CONFIG_LV_DRAW_BUF_ALIGN
    #else
        #define LV_DRAW_BUF_ALIGN                       4
    #endif
#endif

/** Using matrix for transformations.
 * Requirements:
 * - `LV_USE_MATRIX = 1`.
 * - Rendering engine needs to support 3x3 matrix transformations. */
#ifndef LV_DRAW_TRANSFORM_USE_MATRIX
    #ifdef CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
        #define LV_DRAW_TRANSFORM_USE_MATRIX CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
    #else
        #define LV_DRAW_TRANSFORM_USE_MATRIX            0
    #endif
#endif

/* If a widget has `style_opa < 255` (not `bg_opa`, `text_opa` etc) or not NORMAL blend mode
 * it is buffered into a "simple" layer before rendering. The widget can be buffered in smaller chunks.
 * "Transformed layers" (if `transform_angle/zoom` are set) use larger buffers
 * and can't be drawn in chunks. */

/** The target buffer size for simple layer chunks. */
#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #ifdef CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #else
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /**< [bytes]*/
    #endif
#endif

/* Limit the max allocated memory for simple and transformed layers.
 * It should be at least `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` sized but if transformed layers are also used
 * it should be enough to store the largest widget too (width x height x 4 area).
 * Set it to 0 to have no limit. */
#ifndef LV_DRAW_LAYER_MAX_MEMORY
    #ifdef CONFIG_LV_DRAW_LAYER_MAX_MEMORY
        #define LV_DRAW_LAYER_MAX_MEMORY CONFIG_LV_DRAW_LAYER_MAX_MEMORY
    #else
        #define LV_DRAW_LAYER_MAX_MEMORY 0  /**< No limit by default [bytes]*/
    #endif
#endif

/** Stack size of drawing thread.
 * NOTE: If FreeType or ThorVG is enabled, it is recommended to set it to 32KB or more.
 */
#ifndef LV_DRAW_THREAD_STACK_SIZE
    #ifdef CONFIG_LV_DRAW_THREAD_STACK_SIZE
        #define LV_DRAW_THREAD_STACK_SIZE CONFIG_LV_DRAW_THREAD_STACK_SIZE
    #else
        #define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes]*/
    #endif
#endif

#ifndef LV_USE_DRAW_SW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DRAW_SW
            #define LV_USE_DRAW_SW CONFIG_LV_USE_DRAW_SW
        #else
            #define LV_USE_DRAW_SW 0
        #endif
    #else
        #define LV_USE_DRAW_SW 1
    #endif
#endif
#if LV_USE_DRAW_SW == 1
    /*
     * Selectively disable color format support in order to reduce code size.
     * NOTE: some features use certain color formats internally, e.g.
     * - gradients use RGB888
     * - bitmaps with transparency may use ARGB8888
     */
    #ifndef LV_DRAW_SW_SUPPORT_RGB565
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565
                #define LV_DRAW_SW_SUPPORT_RGB565 CONFIG_LV_DRAW_SW_SUPPORT_RGB565
            #else
                #define LV_DRAW_SW_SUPPORT_RGB565 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565       1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_RGB565A8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
                #define LV_DRAW_SW_SUPPORT_RGB565A8 CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
            #else
                #define LV_DRAW_SW_SUPPORT_RGB565A8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565A8     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_RGB888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB888
                #define LV_DRAW_SW_SUPPORT_RGB888 CONFIG_LV_DRAW_SW_SUPPORT_RGB888
            #else
                #define LV_DRAW_SW_SUPPORT_RGB888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_RGB888       1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_XRGB8888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
                #define LV_DRAW_SW_SUPPORT_XRGB8888 CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
            #else
                #define LV_DRAW_SW_SUPPORT_XRGB8888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_XRGB8888     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_ARGB8888
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
                #define LV_DRAW_SW_SUPPORT_ARGB8888 CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
            #else
                #define LV_DRAW_SW_SUPPORT_ARGB8888 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888     1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_L8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_L8
                #define LV_DRAW_SW_SUPPORT_L8 CONFIG_LV_DRAW_SW_SUPPORT_L8
            #else
                #define LV_DRAW_SW_SUPPORT_L8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_L8           1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_AL88
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_AL88
                #define LV_DRAW_SW_SUPPORT_AL88 CONFIG_LV_DRAW_SW_SUPPORT_AL88
            #else
                #define LV_DRAW_SW_SUPPORT_AL88 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_AL88         1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_A8
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_A8
                #define LV_DRAW_SW_SUPPORT_A8 CONFIG_LV_DRAW_SW_SUPPORT_A8
            #else
                #define LV_DRAW_SW_SUPPORT_A8 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_A8           1
        #endif
    #endif
    #ifndef LV_DRAW_SW_SUPPORT_I1
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_SUPPORT_I1
                #define LV_DRAW_SW_SUPPORT_I1 CONFIG_LV_DRAW_SW_SUPPORT_I1
            #else
                #define LV_DRAW_SW_SUPPORT_I1 0
            #endif
        #else
            #define LV_DRAW_SW_SUPPORT_I1           1
        #endif
    #endif

    /** Set number of draw units.
     *  - > 1 requires operating system to be enabled in `LV_USE_OS`.
     *  - > 1 means multiple threads will render the screen in parallel. */
    #ifndef LV_DRAW_SW_DRAW_UNIT_CNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
                #define LV_DRAW_SW_DRAW_UNIT_CNT CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
            #else
                #define LV_DRAW_SW_DRAW_UNIT_CNT 0
            #endif
        #else
            #define LV_DRAW_SW_DRAW_UNIT_CNT    1
        #endif
    #endif

    /** Use Arm-2D to accelerate software (sw) rendering. */
    #ifndef LV_USE_DRAW_ARM2D_SYNC
        #ifdef CONFIG_LV_USE_DRAW_ARM2D_SYNC
            #define LV_USE_DRAW_ARM2D_SYNC CONFIG_LV_USE_DRAW_ARM2D_SYNC
        #else
            #define LV_USE_DRAW_ARM2D_SYNC      0
        #endif
    #endif

    /** Enable native helium assembly to be compiled. */
    #ifndef LV_USE_NATIVE_HELIUM_ASM
        #ifdef CONFIG_LV_USE_NATIVE_HELIUM_ASM
            #define LV_USE_NATIVE_HELIUM_ASM CONFIG_LV_USE_NATIVE_HELIUM_ASM
        #else
            #define LV_USE_NATIVE_HELIUM_ASM    0
        #endif
    #endif

    /**
     * - 0: Use a simple renderer capable of drawing only simple rectangles with gradient, images, text, and straight lines only.
     * - 1: Use a complex renderer capable of drawing rounded corners, shadow, skew lines, and arcs too. */
    #ifndef LV_DRAW_SW_COMPLEX
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_DRAW_SW_COMPLEX
                #define LV_DRAW_SW_COMPLEX CONFIG_LV_DRAW_SW_COMPLEX
            #else
                #define LV_DRAW_SW_COMPLEX 0
            #endif
        #else
            #define LV_DRAW_SW_COMPLEX          1
        #endif
    #endif

    #if LV_DRAW_SW_COMPLEX == 1
        /** Allow buffering some shadow calculation.
         *  LV_DRAW_SW_SHADOW_CACHE_SIZE is the maximum shadow size to buffer, where shadow size is
         *  `shadow_width + radius`.  Caching has LV_DRAW_SW_SHADOW_CACHE_SIZE^2 RAM cost. */
        #ifndef LV_DRAW_SW_SHADOW_CACHE_SIZE
            #ifdef CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
                #define LV_DRAW_SW_SHADOW_CACHE_SIZE CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
            #else
                #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0
            #endif
        #endif

        /** Set number of maximally-cached circle data.
         *  The circumference of 1/4 circle are saved for anti-aliasing.
         *  `radius * 4` bytes are used per circle (the most often used radiuses are saved).
         *  - 0: disables caching */
        #ifndef LV_DRAW_SW_CIRCLE_CACHE_SIZE
            #ifdef CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
                #define LV_DRAW_SW_CIRCLE_CACHE_SIZE CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
            #else
                #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
            #endif
        #endif
    #endif

    #ifndef LV_USE_DRAW_SW_ASM
        #ifdef CONFIG_LV_USE_DRAW_SW_ASM
            #define LV_USE_DRAW_SW_ASM CONFIG_LV_USE_DRAW_SW_ASM
        #else
            #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
        #endif
    #endif

    #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
        #ifndef LV_DRAW_SW_ASM_CUSTOM_INCLUDE
            #ifdef CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
                #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
            #else
                #define  LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
            #endif
        #endif
    #endif

    /** Enable drawing complex gradients in software: linear at an angle, radial or conical */
    #ifndef LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #ifdef CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
            #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #else
            #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0
        #endif
    #endif
#endif

/*Use TSi's aka (Think Silicon) NemaGFX */
#ifndef LV_USE_NEMA_GFX
    #ifdef CONFIG_LV_USE_NEMA_GFX
        #define LV_USE_NEMA_GFX CONFIG_LV_USE_NEMA_GFX
    #else
        #define LV_USE_NEMA_GFX 0
    #endif
#endif

#if LV_USE_NEMA_GFX
    /** Select which NemaGFX HAL to use. Possible options:
     * - LV_NEMA_HAL_CUSTOM
     * - LV_NEMA_HAL_STM32 */
    #ifndef LV_USE_NEMA_HAL
        #ifdef CONFIG_LV_USE_NEMA_HAL
            #define LV_USE_NEMA_HAL CONFIG_LV_USE_NEMA_HAL
        #else
            #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
        #endif
    #endif
    #if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
        #ifndef LV_NEMA_STM32_HAL_INCLUDE
            #ifdef CONFIG_LV_NEMA_STM32_HAL_INCLUDE
                #define LV_NEMA_STM32_HAL_INCLUDE CONFIG_LV_NEMA_STM32_HAL_INCLUDE
            #else
                #define LV_NEMA_STM32_HAL_INCLUDE <stm32u5xx_hal.h>
            #endif
        #endif
    #endif

    /*Enable Vector Graphics Operations. Available only if NemaVG library is present*/
    #ifndef LV_USE_NEMA_VG
        #ifdef CONFIG_LV_USE_NEMA_VG
            #define LV_USE_NEMA_VG CONFIG_LV_USE_NEMA_VG
        #else
            #define LV_USE_NEMA_VG 0
        #endif
    #endif
    #if LV_USE_NEMA_VG
        /*Define application's resolution used for VG related buffer allocation */
        #ifndef LV_NEMA_GFX_MAX_RESX
            #ifdef CONFIG_LV_NEMA_GFX_MAX_RESX
                #define LV_NEMA_GFX_MAX_RESX CONFIG_LV_NEMA_GFX_MAX_RESX
            #else
                #define LV_NEMA_GFX_MAX_RESX 800
            #endif
        #endif
        #ifndef LV_NEMA_GFX_MAX_RESY
            #ifdef CONFIG_LV_NEMA_GFX_MAX_RESY
                #define LV_NEMA_GFX_MAX_RESY CONFIG_LV_NEMA_GFX_MAX_RESY
            #else
                #define LV_NEMA_GFX_MAX_RESY 600
            #endif
        #endif
    #endif
#endif

/** Use NXP's VG-Lite GPU on iMX RTxxx platforms. */
#ifndef LV_USE_DRAW_VGLITE
    #ifdef CONFIG_LV_USE_DRAW_VGLITE
        #define LV_USE_DRAW_VGLITE CONFIG_LV_USE_DRAW_VGLITE
    #else
        #define LV_USE_DRAW_VGLITE 0
    #endif
#endif

#if LV_USE_DRAW_VGLITE
    /** Enable blit quality degradation workaround recommended for screen's dimension > 352 pixels. */
    #ifndef LV_USE_VGLITE_BLIT_SPLIT
        #ifdef CONFIG_LV_USE_VGLITE_BLIT_SPLIT
            #define LV_USE_VGLITE_BLIT_SPLIT CONFIG_LV_USE_VGLITE_BLIT_SPLIT
        #else
            #define LV_USE_VGLITE_BLIT_SPLIT 0
        #endif
    #endif

    #if LV_USE_OS
        /** Use additional draw thread for VG-Lite processing. */
        #ifndef LV_USE_VGLITE_DRAW_THREAD
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_USE_VGLITE_DRAW_THREAD
                    #define LV_USE_VGLITE_DRAW_THREAD CONFIG_LV_USE_VGLITE_DRAW_THREAD
                #else
                    #define LV_USE_VGLITE_DRAW_THREAD 0
                #endif
            #else
                #define LV_USE_VGLITE_DRAW_THREAD 1
            #endif
        #endif

        #if LV_USE_VGLITE_DRAW_THREAD
            /** Enable VGLite draw async. Queue multiple tasks and flash them once to the GPU. */
            #ifndef LV_USE_VGLITE_DRAW_ASYNC
                #ifdef LV_KCONFIG_PRESENT
                    #ifdef CONFIG_LV_USE_VGLITE_DRAW_ASYNC
                        #define LV_USE_VGLITE_DRAW_ASYNC CONFIG_LV_USE_VGLITE_DRAW_ASYNC
                    #else
                        #define LV_USE_VGLITE_DRAW_ASYNC 0
                    #endif
                #else
                    #define LV_USE_VGLITE_DRAW_ASYNC 1
                #endif
            #endif
        #endif
    #endif

    /** Enable VGLite asserts. */
    #ifndef LV_USE_VGLITE_ASSERT
        #ifdef CONFIG_LV_USE_VGLITE_ASSERT
            #define LV_USE_VGLITE_ASSERT CONFIG_LV_USE_VGLITE_ASSERT
        #else
            #define LV_USE_VGLITE_ASSERT 0
        #endif
    #endif
#endif

/** Use NXP's PXP on iMX RTxxx platforms. */
#ifndef LV_USE_PXP
    #ifdef CONFIG_LV_USE_PXP
        #define LV_USE_PXP CONFIG_LV_USE_PXP
    #else
        #define LV_USE_PXP 0
    #endif
#endif

#if LV_USE_PXP
    /** Use PXP for drawing.*/
    #ifndef LV_USE_DRAW_PXP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_DRAW_PXP
                #define LV_USE_DRAW_PXP CONFIG_LV_USE_DRAW_PXP
            #else
                #define LV_USE_DRAW_PXP 0
            #endif
        #else
            #define LV_USE_DRAW_PXP 1
        #endif
    #endif

    /** Use PXP to rotate display.*/
    #ifndef LV_USE_ROTATE_PXP
        #ifdef CONFIG_LV_USE_ROTATE_PXP
            #define LV_USE_ROTATE_PXP CONFIG_LV_USE_ROTATE_PXP
        #else
            #define LV_USE_ROTATE_PXP 0
        #endif
    #endif

    #if LV_USE_DRAW_PXP && LV_USE_OS
        /** Use additional draw thread for PXP processing.*/
        #ifndef LV_USE_PXP_DRAW_THREAD
            #ifdef LV_KCONFIG_PRESENT
                #ifdef CONFIG_LV_USE_PXP_DRAW_THREAD
                    #define LV_USE_PXP_DRAW_THREAD CONFIG_LV_USE_PXP_DRAW_THREAD
                #else
                    #define LV_USE_PXP_DRAW_THREAD 0
                #endif
            #else
                #define LV_USE_PXP_DRAW_THREAD 1
            #endif
        #endif
    #endif

    /** Enable PXP asserts. */
    #ifndef LV_USE_PXP_ASSERT
        #ifdef CONFIG_LV_USE_PXP_ASSERT
            #define LV_USE_PXP_ASSERT CONFIG_LV_USE_PXP_ASSERT
        #else
            #define LV_USE_PXP_ASSERT 0
        #endif
    #endif
#endif

/** Use Renesas Dave2D on RA  platforms. */
#ifndef LV_USE_DRAW_DAVE2D
    #ifdef CONFIG_LV_USE_DRAW_DAVE2D
        #define LV_USE_DRAW_DAVE2D CONFIG_LV_USE_DRAW_DAVE2D
    #else
        #define LV_USE_DRAW_DAVE2D 0
    #endif
#endif

/** Draw using cached SDL textures*/
#ifndef LV_USE_DRAW_SDL
    #ifdef CONFIG_LV_USE_DRAW_SDL
        #define LV_USE_DRAW_SDL CONFIG_LV_USE_DRAW_SDL
    #else
        #define LV_USE_DRAW_SDL 0
    #endif
#endif

/** Use VG-Lite GPU. */
#ifndef LV_USE_DRAW_VG_LITE
    #ifdef CONFIG_LV_USE_DRAW_VG_LITE
        #define LV_USE_DRAW_VG_LITE CONFIG_LV_USE_DRAW_VG_LITE
    #else
        #define LV_USE_DRAW_VG_LITE 0
    #endif
#endif

#if LV_USE_DRAW_VG_LITE
    /** Enable VG-Lite custom external 'gpu_init()' function */
    #ifndef LV_VG_LITE_USE_GPU_INIT
        #ifdef CONFIG_LV_VG_LITE_USE_GPU_INIT
            #define LV_VG_LITE_USE_GPU_INIT CONFIG_LV_VG_LITE_USE_GPU_INIT
        #else
            #define LV_VG_LITE_USE_GPU_INIT 0
        #endif
    #endif

    /** Enable VG-Lite assert. */
    #ifndef LV_VG_LITE_USE_ASSERT
        #ifdef CONFIG_LV_VG_LITE_USE_ASSERT
            #define LV_VG_LITE_USE_ASSERT CONFIG_LV_VG_LITE_USE_ASSERT
        #else
            #define LV_VG_LITE_USE_ASSERT 0
        #endif
    #endif

    /** VG-Lite flush commit trigger threshold. GPU will try to batch these many draw tasks. */
    #ifndef LV_VG_LITE_FLUSH_MAX_COUNT
        #ifdef CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
            #define LV_VG_LITE_FLUSH_MAX_COUNT CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
        #else
            #define LV_VG_LITE_FLUSH_MAX_COUNT 8
        #endif
    #endif

    /** Enable border to simulate shadow.
     *  NOTE: which usually improves performance,
     *  but does not guarantee the same rendering quality as the software. */
    #ifndef LV_VG_LITE_USE_BOX_SHADOW
        #ifdef CONFIG_LV_VG_LITE_USE_BOX_SHADOW
            #define LV_VG_LITE_USE_BOX_SHADOW CONFIG_LV_VG_LITE_USE_BOX_SHADOW
        #else
            #define LV_VG_LITE_USE_BOX_SHADOW 0
        #endif
    #endif

    /** VG-Lite gradient maximum cache number.
     *  @note  The memory usage of a single gradient image is 4K bytes. */
    #ifndef LV_VG_LITE_GRAD_CACHE_CNT
        #ifdef CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
            #define LV_VG_LITE_GRAD_CACHE_CNT CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
        #else
            #define LV_VG_LITE_GRAD_CACHE_CNT 32
        #endif
    #endif

    /** VG-Lite stroke maximum cache number. */
    #ifndef LV_VG_LITE_STROKE_CACHE_CNT
        #ifdef CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
            #define LV_VG_LITE_STROKE_CACHE_CNT CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
        #else
            #define LV_VG_LITE_STROKE_CACHE_CNT 32
        #endif
    #endif
#endif

/** Accelerate blends, fills, etc. with STM32 DMA2D */
#ifndef LV_USE_DRAW_DMA2D
    #ifdef CONFIG_LV_USE_DRAW_DMA2D
        #define LV_USE_DRAW_DMA2D CONFIG_LV_USE_DRAW_DMA2D
    #else
        #define LV_USE_DRAW_DMA2D 0
    #endif
#endif

#if LV_USE_DRAW_DMA2D
    #ifndef LV_DRAW_DMA2D_HAL_INCLUDE
        #ifdef CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
            #define LV_DRAW_DMA2D_HAL_INCLUDE CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
        #else
            #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"
        #endif
    #endif

    /* if enabled, the user is required to call `lv_draw_dma2d_transfer_complete_interrupt_handler`
     * upon receiving the DMA2D global interrupt
     */
    #ifndef LV_USE_DRAW_DMA2D_INTERRUPT
        #ifdef CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
            #define LV_USE_DRAW_DMA2D_INTERRUPT CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
        #else
            #define LV_USE_DRAW_DMA2D_INTERRUPT 0
        #endif
    #endif
#endif

/** Draw using cached OpenGLES textures */
#ifndef LV_USE_DRAW_OPENGLES
    #ifdef CONFIG_LV_USE_DRAW_OPENGLES
        #define LV_USE_DRAW_OPENGLES CONFIG_LV_USE_DRAW_OPENGLES
    #else
        #define LV_USE_DRAW_OPENGLES 0
    #endif
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Logging
 *-----------*/

/** Enable log module */
#ifndef LV_USE_LOG
    #ifdef CONFIG_LV_USE_LOG
        #define LV_USE_LOG CONFIG_LV_USE_LOG
    #else
        #define LV_USE_LOG 0
    #endif
#endif
#if LV_USE_LOG
    /** Set value to one of the following levels of logging detail:
     *  - LV_LOG_LEVEL_TRACE    Log detailed information.
     *  - LV_LOG_LEVEL_INFO     Log important events.
     *  - LV_LOG_LEVEL_WARN     Log if something unwanted happened but didn't cause a problem.
     *  - LV_LOG_LEVEL_ERROR    Log only critical issues, when system may fail.
     *  - LV_LOG_LEVEL_USER     Log only custom log messages added by the user.
     *  - LV_LOG_LEVEL_NONE     Do not log anything. */
    #ifndef LV_LOG_LEVEL
        #ifdef CONFIG_LV_LOG_LEVEL
            #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
        #else
            #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
        #endif
    #endif

    /** - 1: Print log with 'printf';
     *  - 0: User needs to register a callback with `lv_log_register_print_cb()`. */
    #ifndef LV_LOG_PRINTF
        #ifdef CONFIG_LV_LOG_PRINTF
            #define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
        #else
            #define LV_LOG_PRINTF 0
        #endif
    #endif

    /** Set callback to print logs.
     *  E.g `my_print`. The prototype should be `void my_print(lv_log_level_t level, const char * buf)`.
     *  Can be overwritten by `lv_log_register_print_cb`. */
    //#define LV_LOG_PRINT_CB

    /** - 1: Enable printing timestamp;
     *  - 0: Disable printing timestamp. */
    #ifndef LV_LOG_USE_TIMESTAMP
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_USE_TIMESTAMP
                #define LV_LOG_USE_TIMESTAMP CONFIG_LV_LOG_USE_TIMESTAMP
            #else
                #define LV_LOG_USE_TIMESTAMP 0
            #endif
        #else
            #define LV_LOG_USE_TIMESTAMP 1
        #endif
    #endif

    /** - 1: Print file and line number of the log;
     *  - 0: Do not print file and line number of the log. */
    #ifndef LV_LOG_USE_FILE_LINE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_USE_FILE_LINE
                #define LV_LOG_USE_FILE_LINE CONFIG_LV_LOG_USE_FILE_LINE
            #else
                #define LV_LOG_USE_FILE_LINE 0
            #endif
        #else
            #define LV_LOG_USE_FILE_LINE 1
        #endif
    #endif

    /* Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs. */
    #ifndef LV_LOG_TRACE_MEM
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_MEM
                #define LV_LOG_TRACE_MEM CONFIG_LV_LOG_TRACE_MEM
            #else
                #define LV_LOG_TRACE_MEM 0
            #endif
        #else
            #define LV_LOG_TRACE_MEM        1   /**< Enable/disable trace logs in memory operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_TIMER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_TIMER
                #define LV_LOG_TRACE_TIMER CONFIG_LV_LOG_TRACE_TIMER
            #else
                #define LV_LOG_TRACE_TIMER 0
            #endif
        #else
            #define LV_LOG_TRACE_TIMER      1   /**< Enable/disable trace logs in timer operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_INDEV
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_INDEV
                #define LV_LOG_TRACE_INDEV CONFIG_LV_LOG_TRACE_INDEV
            #else
                #define LV_LOG_TRACE_INDEV 0
            #endif
        #else
            #define LV_LOG_TRACE_INDEV      1   /**< Enable/disable trace logs in input device operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_DISP_REFR
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_DISP_REFR
                #define LV_LOG_TRACE_DISP_REFR CONFIG_LV_LOG_TRACE_DISP_REFR
            #else
                #define LV_LOG_TRACE_DISP_REFR 0
            #endif
        #else
            #define LV_LOG_TRACE_DISP_REFR  1   /**< Enable/disable trace logs in display re-draw operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_EVENT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_EVENT
                #define LV_LOG_TRACE_EVENT CONFIG_LV_LOG_TRACE_EVENT
            #else
                #define LV_LOG_TRACE_EVENT 0
            #endif
        #else
            #define LV_LOG_TRACE_EVENT      1   /**< Enable/disable trace logs in event dispatch logic. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_OBJ_CREATE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_OBJ_CREATE
                #define LV_LOG_TRACE_OBJ_CREATE CONFIG_LV_LOG_TRACE_OBJ_CREATE
            #else
                #define LV_LOG_TRACE_OBJ_CREATE 0
            #endif
        #else
            #define LV_LOG_TRACE_OBJ_CREATE 1   /**< Enable/disable trace logs in object creation (core `obj` creation plus every widget). */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_LAYOUT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_LAYOUT
                #define LV_LOG_TRACE_LAYOUT CONFIG_LV_LOG_TRACE_LAYOUT
            #else
                #define LV_LOG_TRACE_LAYOUT 0
            #endif
        #else
            #define LV_LOG_TRACE_LAYOUT     1   /**< Enable/disable trace logs in flex- and grid-layout operations. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_ANIM
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_ANIM
                #define LV_LOG_TRACE_ANIM CONFIG_LV_LOG_TRACE_ANIM
            #else
                #define LV_LOG_TRACE_ANIM 0
            #endif
        #else
            #define LV_LOG_TRACE_ANIM       1   /**< Enable/disable trace logs in animation logic. */
        #endif
    #endif
    #ifndef LV_LOG_TRACE_CACHE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_CACHE
                #define LV_LOG_TRACE_CACHE CONFIG_LV_LOG_TRACE_CACHE
            #else
                #define LV_LOG_TRACE_CACHE 0
            #endif
        #else
            #define LV_LOG_TRACE_CACHE      1   /**< Enable/disable trace logs in cache operations. */
        #endif
    #endif
#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts
 *-----------*/

/* Enable assertion failures if an operation fails or invalid data is found.
 * If LV_USE_LOG is enabled, an error message will be printed on failure. */
#ifndef LV_USE_ASSERT_NULL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_NULL
            #define LV_USE_ASSERT_NULL CONFIG_LV_USE_ASSERT_NULL
        #else
            #define LV_USE_ASSERT_NULL 0
        #endif
    #else
        #define LV_USE_ASSERT_NULL          1   /**< Check if the parameter is NULL. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_MALLOC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_MALLOC
            #define LV_USE_ASSERT_MALLOC CONFIG_LV_USE_ASSERT_MALLOC
        #else
            #define LV_USE_ASSERT_MALLOC 0
        #endif
    #else
        #define LV_USE_ASSERT_MALLOC        1   /**< Checks is the memory is successfully allocated or no. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_STYLE
    #ifdef CONFIG_LV_USE_ASSERT_STYLE
        #define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
    #else
        #define LV_USE_ASSERT_STYLE         0   /**< Check if the styles are properly initialized. (Very fast, recommended) */
    #endif
#endif
#ifndef LV_USE_ASSERT_MEM_INTEGRITY
    #ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
        #define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
    #else
        #define LV_USE_ASSERT_MEM_INTEGRITY 0   /**< Check the integrity of `lv_mem` after critical operations. (Slow) */
    #endif
#endif
#ifndef LV_USE_ASSERT_OBJ
    #ifdef CONFIG_LV_USE_ASSERT_OBJ
        #define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
    #else
        #define LV_USE_ASSERT_OBJ           0   /**< Check the object's type and existence (e.g. not deleted). (Slow) */
    #endif
#endif

/** Add a custom handler when assert happens e.g. to restart MCU. */
#ifndef LV_ASSERT_HANDLER_INCLUDE
    #ifdef CONFIG_LV_ASSERT_HANDLER_INCLUDE
        #define LV_ASSERT_HANDLER_INCLUDE CONFIG_LV_ASSERT_HANDLER_INCLUDE
    #else
        #define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
    #endif
#endif
#ifndef LV_ASSERT_HANDLER
    #ifdef CONFIG_LV_ASSERT_HANDLER
        #define LV_ASSERT_HANDLER CONFIG_LV_ASSERT_HANDLER
    #else
        #define LV_ASSERT_HANDLER while(1);     /**< Halt by default */
    #endif
#endif

/*-------------
 * Debug
 *-----------*/

/** 1: Draw random colored rectangles over the redrawn areas. */
#ifndef LV_USE_REFR_DEBUG
    #ifdef CONFIG_LV_USE_REFR_DEBUG
        #define LV_USE_REFR_DEBUG CONFIG_LV_USE_REFR_DEBUG
    #else
        #define LV_USE_REFR_DEBUG 0
    #endif
#endif

/** 1: Draw a red overlay for ARGB layers and a green overlay for RGB layers*/
#ifndef LV_USE_LAYER_DEBUG
    #ifdef CONFIG_LV_USE_LAYER_DEBUG
        #define LV_USE_LAYER_DEBUG CONFIG_LV_USE_LAYER_DEBUG
    #else
        #define LV_USE_LAYER_DEBUG 0
    #endif
#endif

/** 1: Adds the following behaviors for debugging:
 *  - Draw overlays with different colors for each draw_unit's tasks.
 *  - Draw index number of draw unit on white background.
 *  - For layers, draws index number of draw unit on black background. */
#ifndef LV_USE_PARALLEL_DRAW_DEBUG
    #ifdef CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
        #define LV_USE_PARALLEL_DRAW_DEBUG CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
    #else
        #define LV_USE_PARALLEL_DRAW_DEBUG 0
    #endif
#endif

/*-------------
 * Others
 *-----------*/

#ifndef LV_ENABLE_GLOBAL_CUSTOM
    #ifdef CONFIG_LV_ENABLE_GLOBAL_CUSTOM
        #define LV_ENABLE_GLOBAL_CUSTOM CONFIG_LV_ENABLE_GLOBAL_CUSTOM
    #else
        #define LV_ENABLE_GLOBAL_CUSTOM 0
    #endif
#endif
#if LV_ENABLE_GLOBAL_CUSTOM
    /** Header to include for custom 'lv_global' function" */
    #ifndef LV_GLOBAL_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
            #define LV_GLOBAL_CUSTOM_INCLUDE CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
        #else
            #define LV_GLOBAL_CUSTOM_INCLUDE <stdint.h>
        #endif
    #endif
#endif

/** Default cache size in bytes.
 *  Used by image decoders such as `lv_lodepng` to keep the decoded image in memory.
 *  If size is not set to 0, the decoder will fail to decode when the cache is full.
 *  If size is 0, the cache function is not enabled and the decoded memory will be
 *  released immediately after use. */
#ifndef LV_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_CACHE_DEF_SIZE
        #define LV_CACHE_DEF_SIZE CONFIG_LV_CACHE_DEF_SIZE
    #else
        #define LV_CACHE_DEF_SIZE       0
    #endif
#endif

/** Default number of image header cache entries. The cache is used to store the headers of images
 *  The main logic is like `LV_CACHE_DEF_SIZE` but for image headers. */
#ifndef LV_IMAGE_HEADER_CACHE_DEF_CNT
    #ifdef CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
    #else
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT 0
    #endif
#endif

/** Number of stops allowed per gradient. Increase this to allow more stops.
 *  This adds (sizeof(lv_color_t) + 1) bytes per additional stop. */
#ifndef LV_GRADIENT_MAX_STOPS
    #ifdef CONFIG_LV_GRADIENT_MAX_STOPS
        #define LV_GRADIENT_MAX_STOPS CONFIG_LV_GRADIENT_MAX_STOPS
    #else
        #define LV_GRADIENT_MAX_STOPS   2
    #endif
#endif

/** Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 *  - 0:   round down,
 *  - 64:  round up from x.75,
 *  - 128: round up from half,
 *  - 192: round up from x.25,
 *  - 254: round up */
#ifndef LV_COLOR_MIX_ROUND_OFS
    #ifdef CONFIG_LV_COLOR_MIX_ROUND_OFS
        #define LV_COLOR_MIX_ROUND_OFS CONFIG_LV_COLOR_MIX_ROUND_OFS
    #else
        #define LV_COLOR_MIX_ROUND_OFS  0
    #endif
#endif

/** Add 2 x 32-bit variables to each `lv_obj_t` to speed up getting style properties */
#ifndef LV_OBJ_STYLE_CACHE
    #ifdef CONFIG_LV_OBJ_STYLE_CACHE
        #define LV_OBJ_STYLE_CACHE CONFIG_LV_OBJ_STYLE_CACHE
    #else
        #define LV_OBJ_STYLE_CACHE      0
    #endif
#endif

/** Add `id` field to `lv_obj_t` */
#ifndef LV_USE_OBJ_ID
    #ifdef CONFIG_LV_USE_OBJ_ID
        #define LV_USE_OBJ_ID CONFIG_LV_USE_OBJ_ID
    #else
        #define LV_USE_OBJ_ID           0
    #endif
#endif

/** Automatically assign an ID when obj is created */
#ifndef LV_OBJ_ID_AUTO_ASSIGN
    #ifdef CONFIG_LV_OBJ_ID_AUTO_ASSIGN
        #define LV_OBJ_ID_AUTO_ASSIGN CONFIG_LV_OBJ_ID_AUTO_ASSIGN
    #else
        #define LV_OBJ_ID_AUTO_ASSIGN   LV_USE_OBJ_ID
    #endif
#endif

/** Use builtin obj ID handler functions:
* - lv_obj_assign_id:       Called when a widget is created. Use a separate counter for each widget class as an ID.
* - lv_obj_id_compare:      Compare the ID to decide if it matches with a requested value.
* - lv_obj_stringify_id:    Return string-ified identifier, e.g. "button3".
* - lv_obj_free_id:         Does nothing, as there is no memory allocation for the ID.
* When disabled these functions needs to be implemented by the user.*/
#ifndef LV_USE_OBJ_ID_BUILTIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_ID_BUILTIN
            #define LV_USE_OBJ_ID_BUILTIN CONFIG_LV_USE_OBJ_ID_BUILTIN
        #else
            #define LV_USE_OBJ_ID_BUILTIN 0
        #endif
    #else
        #define LV_USE_OBJ_ID_BUILTIN   1
    #endif
#endif

/** Use obj property set/get API. */
#ifndef LV_USE_OBJ_PROPERTY
    #ifdef CONFIG_LV_USE_OBJ_PROPERTY
        #define LV_USE_OBJ_PROPERTY CONFIG_LV_USE_OBJ_PROPERTY
    #else
        #define LV_USE_OBJ_PROPERTY 0
    #endif
#endif

/** Enable property name support. */
#ifndef LV_USE_OBJ_PROPERTY_NAME
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_PROPERTY_NAME
            #define LV_USE_OBJ_PROPERTY_NAME CONFIG_LV_USE_OBJ_PROPERTY_NAME
        #else
            #define LV_USE_OBJ_PROPERTY_NAME 0
        #endif
    #else
        #define LV_USE_OBJ_PROPERTY_NAME 1
    #endif
#endif

/* Use VG-Lite Simulator.
 * - Requires: LV_USE_THORVG_INTERNAL or LV_USE_THORVG_EXTERNAL */
#ifndef LV_USE_VG_LITE_THORVG
    #ifdef CONFIG_LV_USE_VG_LITE_THORVG
        #define LV_USE_VG_LITE_THORVG CONFIG_LV_USE_VG_LITE_THORVG
    #else
        #define LV_USE_VG_LITE_THORVG  0
    #endif
#endif

#if LV_USE_VG_LITE_THORVG
    /** Enable LVGL's blend mode support */
    #ifndef LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
        #ifdef CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
            #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
        #else
            #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0
        #endif
    #endif

    /** Enable YUV color format support */
    #ifndef LV_VG_LITE_THORVG_YUV_SUPPORT
        #ifdef CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
            #define LV_VG_LITE_THORVG_YUV_SUPPORT CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
        #else
            #define LV_VG_LITE_THORVG_YUV_SUPPORT 0
        #endif
    #endif

    /** Enable Linear gradient extension support */
    #ifndef LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
        #ifdef CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
            #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
        #else
            #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0
        #endif
    #endif

    /** Enable alignment on 16 pixels */
    #ifndef LV_VG_LITE_THORVG_16PIXELS_ALIGN
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
                #define LV_VG_LITE_THORVG_16PIXELS_ALIGN CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
            #else
                #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0
            #endif
        #else
            #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1
        #endif
    #endif

    /** Buffer address alignment */
    #ifndef LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
        #ifdef CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
            #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
        #else
            #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64
        #endif
    #endif

    /** Enable multi-thread render */
    #ifndef LV_VG_LITE_THORVG_THREAD_RENDER
        #ifdef CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
            #define LV_VG_LITE_THORVG_THREAD_RENDER CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
        #else
            #define LV_VG_LITE_THORVG_THREAD_RENDER 0
        #endif
    #endif
#endif

/* Enable the multi-touch gesture recognition feature */
/* Gesture recognition requires the use of floats */
#ifndef LV_USE_GESTURE_RECOGNITION
    #ifdef CONFIG_LV_USE_GESTURE_RECOGNITION
        #define LV_USE_GESTURE_RECOGNITION CONFIG_LV_USE_GESTURE_RECOGNITION
    #else
        #define LV_USE_GESTURE_RECOGNITION 0
    #endif
#endif

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/** For big endian systems set to 1 */
#ifndef LV_BIG_ENDIAN_SYSTEM
    #ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
        #define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
    #else
        #define LV_BIG_ENDIAN_SYSTEM 0
    #endif
#endif

/** Define a custom attribute for `lv_tick_inc` function */
#ifndef LV_ATTRIBUTE_TICK_INC
    #ifdef CONFIG_LV_ATTRIBUTE_TICK_INC
        #define LV_ATTRIBUTE_TICK_INC CONFIG_LV_ATTRIBUTE_TICK_INC
    #else
        #define LV_ATTRIBUTE_TICK_INC
    #endif
#endif

/** Define a custom attribute for `lv_timer_handler` function */
#ifndef LV_ATTRIBUTE_TIMER_HANDLER
    #ifdef CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
        #define LV_ATTRIBUTE_TIMER_HANDLER CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
    #else
        #define LV_ATTRIBUTE_TIMER_HANDLER
    #endif
#endif

/** Define a custom attribute for `lv_display_flush_ready` function */
#ifndef LV_ATTRIBUTE_FLUSH_READY
    #ifdef CONFIG_LV_ATTRIBUTE_FLUSH_READY
        #define LV_ATTRIBUTE_FLUSH_READY CONFIG_LV_ATTRIBUTE_FLUSH_READY
    #else
        #define LV_ATTRIBUTE_FLUSH_READY
    #endif
#endif

/** Align VG_LITE buffers on this number of bytes.
 *  @note  vglite_src_buf_aligned() uses this value to validate alignment of passed buffer pointers. */
#ifndef LV_ATTRIBUTE_MEM_ALIGN_SIZE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
        #else
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 0
        #endif
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
    #endif
#endif

/** Will be added where memory needs to be aligned (with -Os data might not be aligned to boundary by default).
 *  E.g. __attribute__((aligned(4)))*/
#ifndef LV_ATTRIBUTE_MEM_ALIGN
    #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN
        #define LV_ATTRIBUTE_MEM_ALIGN CONFIG_LV_ATTRIBUTE_MEM_ALIGN
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN
    #endif
#endif

/** Attribute to mark large constant arrays, for example for font bitmaps */
#ifndef LV_ATTRIBUTE_LARGE_CONST
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_CONST
        #define LV_ATTRIBUTE_LARGE_CONST CONFIG_LV_ATTRIBUTE_LARGE_CONST
    #else
        #define LV_ATTRIBUTE_LARGE_CONST
    #endif
#endif

/** Compiler prefix for a large array declaration in RAM */
#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #else
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #endif
#endif

/** Place performance critical functions into a faster memory (e.g RAM) */
#ifndef LV_ATTRIBUTE_FAST_MEM
    #ifdef CONFIG_LV_ATTRIBUTE_FAST_MEM
        #define LV_ATTRIBUTE_FAST_MEM CONFIG_LV_ATTRIBUTE_FAST_MEM
    #else
        #define LV_ATTRIBUTE_FAST_MEM
    #endif
#endif

/** Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *  should also appear on LVGL binding API such as MicroPython. */
#ifndef LV_EXPORT_CONST_INT
    #ifdef CONFIG_LV_EXPORT_CONST_INT
        #define LV_EXPORT_CONST_INT CONFIG_LV_EXPORT_CONST_INT
    #else
        #define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning  /**< The default value just prevents GCC warning */
    #endif
#endif

/** Prefix all global extern data with this */
#ifndef LV_ATTRIBUTE_EXTERN_DATA
    #ifdef CONFIG_LV_ATTRIBUTE_EXTERN_DATA
        #define LV_ATTRIBUTE_EXTERN_DATA CONFIG_LV_ATTRIBUTE_EXTERN_DATA
    #else
        #define LV_ATTRIBUTE_EXTERN_DATA
    #endif
#endif

/** Use `float` as `lv_value_precise_t` */
#ifndef LV_USE_FLOAT
    #ifdef CONFIG_LV_USE_FLOAT
        #define LV_USE_FLOAT CONFIG_LV_USE_FLOAT
    #else
        #define LV_USE_FLOAT            0
    #endif
#endif

/** Enable matrix support
 *  - Requires `LV_USE_FLOAT = 1` */
#ifndef LV_USE_MATRIX
    #ifdef CONFIG_LV_USE_MATRIX
        #define LV_USE_MATRIX CONFIG_LV_USE_MATRIX
    #else
        #define LV_USE_MATRIX           0
    #endif
#endif

/** Include `lvgl_private.h` in `lvgl.h` to access internal data and functions by default */
#ifndef LV_USE_PRIVATE_API
    #ifdef CONFIG_LV_USE_PRIVATE_API
        #define LV_USE_PRIVATE_API CONFIG_LV_USE_PRIVATE_API
    #else
        #define LV_USE_PRIVATE_API		0
    #endif
#endif

/*==================
 *   FONT USAGE
 *===================*/

/* Montserrat fonts with ASCII range and some symbols using bpp = 4
 * https://fonts.google.com/specimen/Montserrat */
#ifndef LV_FONT_MONTSERRAT_8
    #ifdef CONFIG_LV_FONT_MONTSERRAT_8
        #define LV_FONT_MONTSERRAT_8 CONFIG_LV_FONT_MONTSERRAT_8
    #else
        #define LV_FONT_MONTSERRAT_8  0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_10
    #ifdef CONFIG_LV_FONT_MONTSERRAT_10
        #define LV_FONT_MONTSERRAT_10 CONFIG_LV_FONT_MONTSERRAT_10
    #else
        #define LV_FONT_MONTSERRAT_10 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_12
    #ifdef CONFIG_LV_FONT_MONTSERRAT_12
        #define LV_FONT_MONTSERRAT_12 CONFIG_LV_FONT_MONTSERRAT_12
    #else
        #define LV_FONT_MONTSERRAT_12 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_14
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FONT_MONTSERRAT_14
            #define LV_FONT_MONTSERRAT_14 CONFIG_LV_FONT_MONTSERRAT_14
        #else
            #define LV_FONT_MONTSERRAT_14 0
        #endif
    #else
        #define LV_FONT_MONTSERRAT_14 1
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_16
    #ifdef CONFIG_LV_FONT_MONTSERRAT_16
        #define LV_FONT_MONTSERRAT_16 CONFIG_LV_FONT_MONTSERRAT_16
    #else
        #define LV_FONT_MONTSERRAT_16 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_18
    #ifdef CONFIG_LV_FONT_MONTSERRAT_18
        #define LV_FONT_MONTSERRAT_18 CONFIG_LV_FONT_MONTSERRAT_18
    #else
        #define LV_FONT_MONTSERRAT_18 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_20
    #ifdef CONFIG_LV_FONT_MONTSERRAT_20
        #define LV_FONT_MONTSERRAT_20 CONFIG_LV_FONT_MONTSERRAT_20
    #else
        #define LV_FONT_MONTSERRAT_20 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_22
    #ifdef CONFIG_LV_FONT_MONTSERRAT_22
        #define LV_FONT_MONTSERRAT_22 CONFIG_LV_FONT_MONTSERRAT_22
    #else
        #define LV_FONT_MONTSERRAT_22 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_24
    #ifdef CONFIG_LV_FONT_MONTSERRAT_24
        #define LV_FONT_MONTSERRAT_24 CONFIG_LV_FONT_MONTSERRAT_24
    #else
        #define LV_FONT_MONTSERRAT_24 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_26
    #ifdef CONFIG_LV_FONT_MONTSERRAT_26
        #define LV_FONT_MONTSERRAT_26 CONFIG_LV_FONT_MONTSERRAT_26
    #else
        #define LV_FONT_MONTSERRAT_26 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_28
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28
        #define LV_FONT_MONTSERRAT_28 CONFIG_LV_FONT_MONTSERRAT_28
    #else
        #define LV_FONT_MONTSERRAT_28 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_30
    #ifdef CONFIG_LV_FONT_MONTSERRAT_30
        #define LV_FONT_MONTSERRAT_30 CONFIG_LV_FONT_MONTSERRAT_30
    #else
        #define LV_FONT_MONTSERRAT_30 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_32
    #ifdef CONFIG_LV_FONT_MONTSERRAT_32
        #define LV_FONT_MONTSERRAT_32 CONFIG_LV_FONT_MONTSERRAT_32
    #else
        #define LV_FONT_MONTSERRAT_32 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_34
    #ifdef CONFIG_LV_FONT_MONTSERRAT_34
        #define LV_FONT_MONTSERRAT_34 CONFIG_LV_FONT_MONTSERRAT_34
    #else
        #define LV_FONT_MONTSERRAT_34 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_36
    #ifdef CONFIG_LV_FONT_MONTSERRAT_36
        #define LV_FONT_MONTSERRAT_36 CONFIG_LV_FONT_MONTSERRAT_36
    #else
        #define LV_FONT_MONTSERRAT_36 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_38
    #ifdef CONFIG_LV_FONT_MONTSERRAT_38
        #define LV_FONT_MONTSERRAT_38 CONFIG_LV_FONT_MONTSERRAT_38
    #else
        #define LV_FONT_MONTSERRAT_38 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_40
    #ifdef CONFIG_LV_FONT_MONTSERRAT_40
        #define LV_FONT_MONTSERRAT_40 CONFIG_LV_FONT_MONTSERRAT_40
    #else
        #define LV_FONT_MONTSERRAT_40 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_42
    #ifdef CONFIG_LV_FONT_MONTSERRAT_42
        #define LV_FONT_MONTSERRAT_42 CONFIG_LV_FONT_MONTSERRAT_42
    #else
        #define LV_FONT_MONTSERRAT_42 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_44
    #ifdef CONFIG_LV_FONT_MONTSERRAT_44
        #define LV_FONT_MONTSERRAT_44 CONFIG_LV_FONT_MONTSERRAT_44
    #else
        #define LV_FONT_MONTSERRAT_44 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_46
    #ifdef CONFIG_LV_FONT_MONTSERRAT_46
        #define LV_FONT_MONTSERRAT_46 CONFIG_LV_FONT_MONTSERRAT_46
    #else
        #define LV_FONT_MONTSERRAT_46 0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_48
    #ifdef CONFIG_LV_FONT_MONTSERRAT_48
        #define LV_FONT_MONTSERRAT_48 CONFIG_LV_FONT_MONTSERRAT_48
    #else
        #define LV_FONT_MONTSERRAT_48 0
    #endif
#endif

/* Demonstrate special features */
#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
        #define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
    #else
        #define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /**< bpp = 3 */
    #endif
#endif
#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #else
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /**< Hebrew, Arabic, Persian letters and all their forms */
    #endif
#endif
#ifndef LV_FONT_SIMSUN_14_CJK
    #ifdef CONFIG_LV_FONT_SIMSUN_14_CJK
        #define LV_FONT_SIMSUN_14_CJK CONFIG_LV_FONT_SIMSUN_14_CJK
    #else
        #define LV_FONT_SIMSUN_14_CJK            0  /**< 1000 most common CJK radicals */
    #endif
#endif
#ifndef LV_FONT_SIMSUN_16_CJK
    #ifdef CONFIG_LV_FONT_SIMSUN_16_CJK
        #define LV_FONT_SIMSUN_16_CJK CONFIG_LV_FONT_SIMSUN_16_CJK
    #else
        #define LV_FONT_SIMSUN_16_CJK            0  /**< 1000 most common CJK radicals */
    #endif
#endif

/** Pixel perfect monospaced fonts */
#ifndef LV_FONT_UNSCII_8
    #ifdef CONFIG_LV_FONT_UNSCII_8
        #define LV_FONT_UNSCII_8 CONFIG_LV_FONT_UNSCII_8
    #else
        #define LV_FONT_UNSCII_8  0
    #endif
#endif
#ifndef LV_FONT_UNSCII_16
    #ifdef CONFIG_LV_FONT_UNSCII_16
        #define LV_FONT_UNSCII_16 CONFIG_LV_FONT_UNSCII_16
    #else
        #define LV_FONT_UNSCII_16 0
    #endif
#endif

/** Optionally declare custom fonts here.
 *
 *  You can use any of these fonts as the default font too and they will be available
 *  globally.  Example:
 *
 *  @code
 *  #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
 *  @endcode
 */
#ifndef LV_FONT_CUSTOM_DECLARE
    #ifdef CONFIG_LV_FONT_CUSTOM_DECLARE
        #define LV_FONT_CUSTOM_DECLARE CONFIG_LV_FONT_CUSTOM_DECLARE
    #else
        #define LV_FONT_CUSTOM_DECLARE
    #endif
#endif

/** Always set a default font */
#ifndef LV_FONT_DEFAULT
    #ifdef CONFIG_LV_FONT_DEFAULT
        #define LV_FONT_DEFAULT CONFIG_LV_FONT_DEFAULT
    #else
        #define LV_FONT_DEFAULT &lv_font_montserrat_14
    #endif
#endif

/** Enable handling large font and/or fonts with a lot of characters.
 *  The limit depends on the font size, font face and bpp.
 *  A compiler error will be triggered if a font needs it. */
#ifndef LV_FONT_FMT_TXT_LARGE
    #ifdef CONFIG_LV_FONT_FMT_TXT_LARGE
        #define LV_FONT_FMT_TXT_LARGE CONFIG_LV_FONT_FMT_TXT_LARGE
    #else
        #define LV_FONT_FMT_TXT_LARGE 0
    #endif
#endif

/** Enables/disables support for compressed fonts. */
#ifndef LV_USE_FONT_COMPRESSED
    #ifdef CONFIG_LV_USE_FONT_COMPRESSED
        #define LV_USE_FONT_COMPRESSED CONFIG_LV_USE_FONT_COMPRESSED
    #else
        #define LV_USE_FONT_COMPRESSED 0
    #endif
#endif

/** Enable drawing placeholders when glyph dsc is not found. */
#ifndef LV_USE_FONT_PLACEHOLDER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FONT_PLACEHOLDER
            #define LV_USE_FONT_PLACEHOLDER CONFIG_LV_USE_FONT_PLACEHOLDER
        #else
            #define LV_USE_FONT_PLACEHOLDER 0
        #endif
    #else
        #define LV_USE_FONT_PLACEHOLDER 1
    #endif
#endif

/*=================
 *  TEXT SETTINGS
 *=================*/

/**
 * Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding.
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#ifndef LV_TXT_ENC
    #ifdef CONFIG_LV_TXT_ENC
        #define LV_TXT_ENC CONFIG_LV_TXT_ENC
    #else
        #define LV_TXT_ENC LV_TXT_ENC_UTF8
    #endif
#endif

/** While rendering text strings, break (wrap) text on these chars. */
#ifndef LV_TXT_BREAK_CHARS
    #ifdef CONFIG_LV_TXT_BREAK_CHARS
        #define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS
    #else
        #define LV_TXT_BREAK_CHARS " ,.;:-_)]}"
    #endif
#endif

/** If a word is at least this long, will break wherever "prettiest".
 *  To disable, set to a value <= 0. */
#ifndef LV_TXT_LINE_BREAK_LONG_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
        #define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_LEN 0
    #endif
#endif

/** Minimum number of characters in a long word to put on a line before a break.
 *  Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
    #endif
#endif

/** Minimum number of characters in a long word to put on a line after a break.
 *  Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
    #endif
#endif

/** Support bidirectional text. Allows mixing Left-to-Right and Right-to-Left text.
 *  The direction will be processed according to the Unicode Bidirectional Algorithm:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics */
#ifndef LV_USE_BIDI
    #ifdef CONFIG_LV_USE_BIDI
        #define LV_USE_BIDI CONFIG_LV_USE_BIDI
    #else
        #define LV_USE_BIDI 0
    #endif
#endif
#if LV_USE_BIDI
    /*Set the default direction. Supported values:
    *`LV_BASE_DIR_LTR` Left-to-Right
    *`LV_BASE_DIR_RTL` Right-to-Left
    *`LV_BASE_DIR_AUTO` detect text base direction*/
    #ifndef LV_BIDI_BASE_DIR_DEF
        #ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
            #define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
        #else
            #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
        #endif
    #endif
#endif

/** Enable Arabic/Persian processing
 *  In these languages characters should be replaced with another form based on their position in the text */
#ifndef LV_USE_ARABIC_PERSIAN_CHARS
    #ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
        #define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
    #else
        #define LV_USE_ARABIC_PERSIAN_CHARS 0
    #endif
#endif

/*The control character to use for signaling text recoloring*/
#ifndef LV_TXT_COLOR_CMD
    #ifdef CONFIG_LV_TXT_COLOR_CMD
        #define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
    #else
        #define LV_TXT_COLOR_CMD "#"
    #endif
#endif

/*==================
 * WIDGETS
 *================*/
/* Documentation for widgets can be found here: https://docs.lvgl.io/latest/en/html/widgets/index.html . */

/** 1: Causes these widgets to be given default values at creation time.
 *  - lv_buttonmatrix_t:  Get default maps:  {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, else map not set.
 *  - lv_checkbox_t    :  String label set to "Check box", else set to empty string.
 *  - lv_dropdown_t    :  Options set to "Option 1", "Option 2", "Option 3", else no values are set.
 *  - lv_roller_t      :  Options set to "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", else no values are set.
 *  - lv_label_t       :  Text set to "Text", else empty string.
 * */
#ifndef LV_WIDGETS_HAS_DEFAULT_VALUE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
            #define LV_WIDGETS_HAS_DEFAULT_VALUE CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
        #else
            #define LV_WIDGETS_HAS_DEFAULT_VALUE 0
        #endif
    #else
        #define LV_WIDGETS_HAS_DEFAULT_VALUE  1
    #endif
#endif

#ifndef LV_USE_ANIMIMG
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ANIMIMG
            #define LV_USE_ANIMIMG CONFIG_LV_USE_ANIMIMG
        #else
            #define LV_USE_ANIMIMG 0
        #endif
    #else
        #define LV_USE_ANIMIMG    1
    #endif
#endif

#ifndef LV_USE_ARC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ARC
            #define LV_USE_ARC CONFIG_LV_USE_ARC
        #else
            #define LV_USE_ARC 0
        #endif
    #else
        #define LV_USE_ARC        1
    #endif
#endif

#ifndef LV_USE_BAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BAR
            #define LV_USE_BAR CONFIG_LV_USE_BAR
        #else
            #define LV_USE_BAR 0
        #endif
    #else
        #define LV_USE_BAR        1
    #endif
#endif

#ifndef LV_USE_BUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTON
            #define LV_USE_BUTTON CONFIG_LV_USE_BUTTON
        #else
            #define LV_USE_BUTTON 0
        #endif
    #else
        #define LV_USE_BUTTON        1
    #endif
#endif

#ifndef LV_USE_BUTTONMATRIX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTONMATRIX
            #define LV_USE_BUTTONMATRIX CONFIG_LV_USE_BUTTONMATRIX
        #else
            #define LV_USE_BUTTONMATRIX 0
        #endif
    #else
        #define LV_USE_BUTTONMATRIX  1
    #endif
#endif

#ifndef LV_USE_CALENDAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CALENDAR
            #define LV_USE_CALENDAR CONFIG_LV_USE_CALENDAR
        #else
            #define LV_USE_CALENDAR 0
        #endif
    #else
        #define LV_USE_CALENDAR   1
    #endif
#endif
#if LV_USE_CALENDAR
    #ifndef LV_CALENDAR_WEEK_STARTS_MONDAY
        #ifdef CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
            #define LV_CALENDAR_WEEK_STARTS_MONDAY CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
        #else
            #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
        #endif
    #endif
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #ifndef LV_CALENDAR_DEFAULT_DAY_NAMES
            #ifdef CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
                #define LV_CALENDAR_DEFAULT_DAY_NAMES CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
            #else
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
            #endif
        #endif
    #else
        #ifndef LV_CALENDAR_DEFAULT_DAY_NAMES
            #ifdef CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
                #define LV_CALENDAR_DEFAULT_DAY_NAMES CONFIG_LV_CALENDAR_DEFAULT_DAY_NAMES
            #else
                #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
            #endif
        #endif
    #endif

    #ifndef LV_CALENDAR_DEFAULT_MONTH_NAMES
        #ifdef CONFIG_LV_CALENDAR_DEFAULT_MONTH_NAMES
            #define LV_CALENDAR_DEFAULT_MONTH_NAMES CONFIG_LV_CALENDAR_DEFAULT_MONTH_NAMES
        #else
            #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_HEADER_ARROW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_CALENDAR_HEADER_ARROW
                #define LV_USE_CALENDAR_HEADER_ARROW CONFIG_LV_USE_CALENDAR_HEADER_ARROW
            #else
                #define LV_USE_CALENDAR_HEADER_ARROW 0
            #endif
        #else
            #define LV_USE_CALENDAR_HEADER_ARROW 1
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_HEADER_DROPDOWN
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
                #define LV_USE_CALENDAR_HEADER_DROPDOWN CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
            #else
                #define LV_USE_CALENDAR_HEADER_DROPDOWN 0
            #endif
        #else
            #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
        #endif
    #endif
    #ifndef LV_USE_CALENDAR_CHINESE
        #ifdef CONFIG_LV_USE_CALENDAR_CHINESE
            #define LV_USE_CALENDAR_CHINESE CONFIG_LV_USE_CALENDAR_CHINESE
        #else
            #define LV_USE_CALENDAR_CHINESE 0
        #endif
    #endif
#endif  /*LV_USE_CALENDAR*/

#ifndef LV_USE_CANVAS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CANVAS
            #define LV_USE_CANVAS CONFIG_LV_USE_CANVAS
        #else
            #define LV_USE_CANVAS 0
        #endif
    #else
        #define LV_USE_CANVAS     1
    #endif
#endif

#ifndef LV_USE_CHART
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHART
            #define LV_USE_CHART CONFIG_LV_USE_CHART
        #else
            #define LV_USE_CHART 0
        #endif
    #else
        #define LV_USE_CHART      1
    #endif
#endif

#ifndef LV_USE_CHECKBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHECKBOX
            #define LV_USE_CHECKBOX CONFIG_LV_USE_CHECKBOX
        #else
            #define LV_USE_CHECKBOX 0
        #endif
    #else
        #define LV_USE_CHECKBOX   1
    #endif
#endif

#ifndef LV_USE_DROPDOWN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DROPDOWN
            #define LV_USE_DROPDOWN CONFIG_LV_USE_DROPDOWN
        #else
            #define LV_USE_DROPDOWN 0
        #endif
    #else
        #define LV_USE_DROPDOWN   1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_IMAGE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGE
            #define LV_USE_IMAGE CONFIG_LV_USE_IMAGE
        #else
            #define LV_USE_IMAGE 0
        #endif
    #else
        #define LV_USE_IMAGE      1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_IMAGEBUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGEBUTTON
            #define LV_USE_IMAGEBUTTON CONFIG_LV_USE_IMAGEBUTTON
        #else
            #define LV_USE_IMAGEBUTTON 0
        #endif
    #else
        #define LV_USE_IMAGEBUTTON     1
    #endif
#endif

#ifndef LV_USE_KEYBOARD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_KEYBOARD
            #define LV_USE_KEYBOARD CONFIG_LV_USE_KEYBOARD
        #else
            #define LV_USE_KEYBOARD 0
        #endif
    #else
        #define LV_USE_KEYBOARD   1
    #endif
#endif

#ifndef LV_USE_LABEL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LABEL
            #define LV_USE_LABEL CONFIG_LV_USE_LABEL
        #else
            #define LV_USE_LABEL 0
        #endif
    #else
        #define LV_USE_LABEL      1
    #endif
#endif
#if LV_USE_LABEL
    #ifndef LV_LABEL_TEXT_SELECTION
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_TEXT_SELECTION
                #define LV_LABEL_TEXT_SELECTION CONFIG_LV_LABEL_TEXT_SELECTION
            #else
                #define LV_LABEL_TEXT_SELECTION 0
            #endif
        #else
            #define LV_LABEL_TEXT_SELECTION 1   /**< Enable selecting text of the label */
        #endif
    #endif
    #ifndef LV_LABEL_LONG_TXT_HINT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_LONG_TXT_HINT
                #define LV_LABEL_LONG_TXT_HINT CONFIG_LV_LABEL_LONG_TXT_HINT
            #else
                #define LV_LABEL_LONG_TXT_HINT 0
            #endif
        #else
            #define LV_LABEL_LONG_TXT_HINT 1    /**< Store some extra info in labels to speed up drawing of very long text */
        #endif
    #endif
    #ifndef LV_LABEL_WAIT_CHAR_COUNT
        #ifdef CONFIG_LV_LABEL_WAIT_CHAR_COUNT
            #define LV_LABEL_WAIT_CHAR_COUNT CONFIG_LV_LABEL_WAIT_CHAR_COUNT
        #else
            #define LV_LABEL_WAIT_CHAR_COUNT 3  /**< The count of wait chart */
        #endif
    #endif
#endif

#ifndef LV_USE_LED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LED
            #define LV_USE_LED CONFIG_LV_USE_LED
        #else
            #define LV_USE_LED 0
        #endif
    #else
        #define LV_USE_LED        1
    #endif
#endif

#ifndef LV_USE_LINE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LINE
            #define LV_USE_LINE CONFIG_LV_USE_LINE
        #else
            #define LV_USE_LINE 0
        #endif
    #else
        #define LV_USE_LINE       1
    #endif
#endif

#ifndef LV_USE_LIST
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LIST
            #define LV_USE_LIST CONFIG_LV_USE_LIST
        #else
            #define LV_USE_LIST 0
        #endif
    #else
        #define LV_USE_LIST       1
    #endif
#endif

#ifndef LV_USE_LOTTIE
    #ifdef CONFIG_LV_USE_LOTTIE
        #define LV_USE_LOTTIE CONFIG_LV_USE_LOTTIE
    #else
        #define LV_USE_LOTTIE     0  /**< Requires: lv_canvas, thorvg */
    #endif
#endif

#ifndef LV_USE_MENU
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MENU
            #define LV_USE_MENU CONFIG_LV_USE_MENU
        #else
            #define LV_USE_MENU 0
        #endif
    #else
        #define LV_USE_MENU       1
    #endif
#endif

#ifndef LV_USE_MSGBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MSGBOX
            #define LV_USE_MSGBOX CONFIG_LV_USE_MSGBOX
        #else
            #define LV_USE_MSGBOX 0
        #endif
    #else
        #define LV_USE_MSGBOX     1
    #endif
#endif

#ifndef LV_USE_ROLLER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ROLLER
            #define LV_USE_ROLLER CONFIG_LV_USE_ROLLER
        #else
            #define LV_USE_ROLLER 0
        #endif
    #else
        #define LV_USE_ROLLER     1   /**< Requires: lv_label */
    #endif
#endif

#ifndef LV_USE_SCALE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SCALE
            #define LV_USE_SCALE CONFIG_LV_USE_SCALE
        #else
            #define LV_USE_SCALE 0
        #endif
    #else
        #define LV_USE_SCALE      1
    #endif
#endif

#ifndef LV_USE_SLIDER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SLIDER
            #define LV_USE_SLIDER CONFIG_LV_USE_SLIDER
        #else
            #define LV_USE_SLIDER 0
        #endif
    #else
        #define LV_USE_SLIDER     1   /**< Requires: lv_bar */
    #endif
#endif

#ifndef LV_USE_SPAN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPAN
            #define LV_USE_SPAN CONFIG_LV_USE_SPAN
        #else
            #define LV_USE_SPAN 0
        #endif
    #else
        #define LV_USE_SPAN       1
    #endif
#endif
#if LV_USE_SPAN
    /** A line of text can contain this maximum number of span descriptors. */
    #ifndef LV_SPAN_SNIPPET_STACK_SIZE
        #ifdef CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
            #define LV_SPAN_SNIPPET_STACK_SIZE CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
        #else
            #define LV_SPAN_SNIPPET_STACK_SIZE 64
        #endif
    #endif
#endif

#ifndef LV_USE_SPINBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINBOX
            #define LV_USE_SPINBOX CONFIG_LV_USE_SPINBOX
        #else
            #define LV_USE_SPINBOX 0
        #endif
    #else
        #define LV_USE_SPINBOX    1
    #endif
#endif

#ifndef LV_USE_SPINNER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINNER
            #define LV_USE_SPINNER CONFIG_LV_USE_SPINNER
        #else
            #define LV_USE_SPINNER 0
        #endif
    #else
        #define LV_USE_SPINNER    1
    #endif
#endif

#ifndef LV_USE_SWITCH
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SWITCH
            #define LV_USE_SWITCH CONFIG_LV_USE_SWITCH
        #else
            #define LV_USE_SWITCH 0
        #endif
    #else
        #define LV_USE_SWITCH     1
    #endif
#endif

#ifndef LV_USE_TABLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABLE
            #define LV_USE_TABLE CONFIG_LV_USE_TABLE
        #else
            #define LV_USE_TABLE 0
        #endif
    #else
        #define LV_USE_TABLE      1
    #endif
#endif

#ifndef LV_USE_TABVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABVIEW
            #define LV_USE_TABVIEW CONFIG_LV_USE_TABVIEW
        #else
            #define LV_USE_TABVIEW 0
        #endif
    #else
        #define LV_USE_TABVIEW    1
    #endif
#endif

#ifndef LV_USE_TEXTAREA
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TEXTAREA
            #define LV_USE_TEXTAREA CONFIG_LV_USE_TEXTAREA
        #else
            #define LV_USE_TEXTAREA 0
        #endif
    #else
        #define LV_USE_TEXTAREA   1   /**< Requires: lv_label */
    #endif
#endif
#if LV_USE_TEXTAREA != 0
    #ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #else
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /**< [ms] */
        #endif
    #endif
#endif

#ifndef LV_USE_TILEVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TILEVIEW
            #define LV_USE_TILEVIEW CONFIG_LV_USE_TILEVIEW
        #else
            #define LV_USE_TILEVIEW 0
        #endif
    #else
        #define LV_USE_TILEVIEW   1
    #endif
#endif

#ifndef LV_USE_WIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_WIN
            #define LV_USE_WIN CONFIG_LV_USE_WIN
        #else
            #define LV_USE_WIN 0
        #endif
    #else
        #define LV_USE_WIN        1
    #endif
#endif

/*==================
 * THEMES
 *==================*/
/* Documentation for themes can be found here: https://docs.lvgl.io/master/overview/style.html#themes . */

/** A simple, impressive and very complete theme */
#ifndef LV_USE_THEME_DEFAULT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_DEFAULT
            #define LV_USE_THEME_DEFAULT CONFIG_LV_USE_THEME_DEFAULT
        #else
            #define LV_USE_THEME_DEFAULT 0
        #endif
    #else
        #define LV_USE_THEME_DEFAULT 1
    #endif
#endif
#if LV_USE_THEME_DEFAULT
    /** 0: Light mode; 1: Dark mode */
    #ifndef LV_THEME_DEFAULT_DARK
        #ifdef CONFIG_LV_THEME_DEFAULT_DARK
            #define LV_THEME_DEFAULT_DARK CONFIG_LV_THEME_DEFAULT_DARK
        #else
            #define LV_THEME_DEFAULT_DARK 0
        #endif
    #endif

    /** 1: Enable grow on press */
    #ifndef LV_THEME_DEFAULT_GROW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_THEME_DEFAULT_GROW
                #define LV_THEME_DEFAULT_GROW CONFIG_LV_THEME_DEFAULT_GROW
            #else
                #define LV_THEME_DEFAULT_GROW 0
            #endif
        #else
            #define LV_THEME_DEFAULT_GROW 1
        #endif
    #endif

    /** Default transition time in ms. */
    #ifndef LV_THEME_DEFAULT_TRANSITION_TIME
        #ifdef CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
            #define LV_THEME_DEFAULT_TRANSITION_TIME CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
        #else
            #define LV_THEME_DEFAULT_TRANSITION_TIME 80
        #endif
    #endif
#endif /*LV_USE_THEME_DEFAULT*/

/** A very simple theme that is a good starting point for a custom theme */
#ifndef LV_USE_THEME_SIMPLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_SIMPLE
            #define LV_USE_THEME_SIMPLE CONFIG_LV_USE_THEME_SIMPLE
        #else
            #define LV_USE_THEME_SIMPLE 0
        #endif
    #else
        #define LV_USE_THEME_SIMPLE 1
    #endif
#endif

/** A theme designed for monochrome displays */
#ifndef LV_USE_THEME_MONO
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_MONO
            #define LV_USE_THEME_MONO CONFIG_LV_USE_THEME_MONO
        #else
            #define LV_USE_THEME_MONO 0
        #endif
    #else
        #define LV_USE_THEME_MONO 1
    #endif
#endif

/*==================
 * LAYOUTS
 *==================*/
/* Documentation for layouts can be found here: https://docs.lvgl.io/master/layouts/index.html . */

/** A layout similar to Flexbox in CSS. */
#ifndef LV_USE_FLEX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FLEX
            #define LV_USE_FLEX CONFIG_LV_USE_FLEX
        #else
            #define LV_USE_FLEX 0
        #endif
    #else
        #define LV_USE_FLEX 1
    #endif
#endif

/** A layout similar to Grid in CSS. */
#ifndef LV_USE_GRID
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_GRID
            #define LV_USE_GRID CONFIG_LV_USE_GRID
        #else
            #define LV_USE_GRID 0
        #endif
    #else
        #define LV_USE_GRID 1
    #endif
#endif

/*====================
 * 3RD PARTS LIBRARIES
 *====================*/
/* Documentation for libraries can be found here: https://docs.lvgl.io/master/libs/index.html . */

/* File system interfaces for common APIs */

/** Setting a default driver letter allows skipping the driver prefix in filepaths. */
#ifndef LV_FS_DEFAULT_DRIVE_LETTER
    #ifdef CONFIG_LV_FS_DEFAULT_DRIVE_LETTER
        #define LV_FS_DEFAULT_DRIVE_LETTER CONFIG_LV_FS_DEFAULT_DRIVE_LETTER
    #else
        #define LV_FS_DEFAULT_DRIVE_LETTER '\0'
    #endif
#endif

/** API for fopen, fread, etc. */
#ifndef LV_USE_FS_STDIO
    #ifdef CONFIG_LV_USE_FS_STDIO
        #define LV_USE_FS_STDIO CONFIG_LV_USE_FS_STDIO
    #else
        #define LV_USE_FS_STDIO 0
    #endif
#endif
#if LV_USE_FS_STDIO
    #ifndef LV_FS_STDIO_LETTER
        #ifdef CONFIG_LV_FS_STDIO_LETTER
            #define LV_FS_STDIO_LETTER CONFIG_LV_FS_STDIO_LETTER
        #else
            #define LV_FS_STDIO_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_STDIO_PATH
        #ifdef CONFIG_LV_FS_STDIO_PATH
            #define LV_FS_STDIO_PATH CONFIG_LV_FS_STDIO_PATH
        #else
            #define LV_FS_STDIO_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_STDIO_CACHE_SIZE
        #ifdef CONFIG_LV_FS_STDIO_CACHE_SIZE
            #define LV_FS_STDIO_CACHE_SIZE CONFIG_LV_FS_STDIO_CACHE_SIZE
        #else
            #define LV_FS_STDIO_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API for open, read, etc. */
#ifndef LV_USE_FS_POSIX
    #ifdef CONFIG_LV_USE_FS_POSIX
        #define LV_USE_FS_POSIX CONFIG_LV_USE_FS_POSIX
    #else
        #define LV_USE_FS_POSIX 0
    #endif
#endif
#if LV_USE_FS_POSIX
    #ifndef LV_FS_POSIX_LETTER
        #ifdef CONFIG_LV_FS_POSIX_LETTER
            #define LV_FS_POSIX_LETTER CONFIG_LV_FS_POSIX_LETTER
        #else
            #define LV_FS_POSIX_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_POSIX_PATH
        #ifdef CONFIG_LV_FS_POSIX_PATH
            #define LV_FS_POSIX_PATH CONFIG_LV_FS_POSIX_PATH
        #else
            #define LV_FS_POSIX_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_POSIX_CACHE_SIZE
        #ifdef CONFIG_LV_FS_POSIX_CACHE_SIZE
            #define LV_FS_POSIX_CACHE_SIZE CONFIG_LV_FS_POSIX_CACHE_SIZE
        #else
            #define LV_FS_POSIX_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API for CreateFile, ReadFile, etc. */
#ifndef LV_USE_FS_WIN32
    #ifdef CONFIG_LV_USE_FS_WIN32
        #define LV_USE_FS_WIN32 CONFIG_LV_USE_FS_WIN32
    #else
        #define LV_USE_FS_WIN32 0
    #endif
#endif
#if LV_USE_FS_WIN32
    #ifndef LV_FS_WIN32_LETTER
        #ifdef CONFIG_LV_FS_WIN32_LETTER
            #define LV_FS_WIN32_LETTER CONFIG_LV_FS_WIN32_LETTER
        #else
            #define LV_FS_WIN32_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_WIN32_PATH
        #ifdef CONFIG_LV_FS_WIN32_PATH
            #define LV_FS_WIN32_PATH CONFIG_LV_FS_WIN32_PATH
        #else
            #define LV_FS_WIN32_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_WIN32_CACHE_SIZE
        #ifdef CONFIG_LV_FS_WIN32_CACHE_SIZE
            #define LV_FS_WIN32_CACHE_SIZE CONFIG_LV_FS_WIN32_CACHE_SIZE
        #else
            #define LV_FS_WIN32_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API for FATFS (needs to be added separately). Uses f_open, f_read, etc. */
#ifndef LV_USE_FS_FATFS
    #ifdef CONFIG_LV_USE_FS_FATFS
        #define LV_USE_FS_FATFS CONFIG_LV_USE_FS_FATFS
    #else
        #define LV_USE_FS_FATFS 0
    #endif
#endif
#if LV_USE_FS_FATFS
    #ifndef LV_FS_FATFS_LETTER
        #ifdef CONFIG_LV_FS_FATFS_LETTER
            #define LV_FS_FATFS_LETTER CONFIG_LV_FS_FATFS_LETTER
        #else
            #define LV_FS_FATFS_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_FATFS_PATH
        #ifdef CONFIG_LV_FS_FATFS_PATH
            #define LV_FS_FATFS_PATH CONFIG_LV_FS_FATFS_PATH
        #else
            #define LV_FS_FATFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
    #ifndef LV_FS_FATFS_CACHE_SIZE
        #ifdef CONFIG_LV_FS_FATFS_CACHE_SIZE
            #define LV_FS_FATFS_CACHE_SIZE CONFIG_LV_FS_FATFS_CACHE_SIZE
        #else
            #define LV_FS_FATFS_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
        #endif
    #endif
#endif

/** API for memory-mapped file access. */
#ifndef LV_USE_FS_MEMFS
    #ifdef CONFIG_LV_USE_FS_MEMFS
        #define LV_USE_FS_MEMFS CONFIG_LV_USE_FS_MEMFS
    #else
        #define LV_USE_FS_MEMFS 0
    #endif
#endif
#if LV_USE_FS_MEMFS
    #ifndef LV_FS_MEMFS_LETTER
        #ifdef CONFIG_LV_FS_MEMFS_LETTER
            #define LV_FS_MEMFS_LETTER CONFIG_LV_FS_MEMFS_LETTER
        #else
            #define LV_FS_MEMFS_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
#endif

/** API for LittleFs. */
#ifndef LV_USE_FS_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_LITTLEFS
        #define LV_USE_FS_LITTLEFS CONFIG_LV_USE_FS_LITTLEFS
    #else
        #define LV_USE_FS_LITTLEFS 0
    #endif
#endif
#if LV_USE_FS_LITTLEFS
    #ifndef LV_FS_LITTLEFS_LETTER
        #ifdef CONFIG_LV_FS_LITTLEFS_LETTER
            #define LV_FS_LITTLEFS_LETTER CONFIG_LV_FS_LITTLEFS_LETTER
        #else
            #define LV_FS_LITTLEFS_LETTER '\0'  /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_LITTLEFS_PATH
        #ifdef CONFIG_LV_FS_LITTLEFS_PATH
            #define LV_FS_LITTLEFS_PATH CONFIG_LV_FS_LITTLEFS_PATH
        #else
            #define LV_FS_LITTLEFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** API for Arduino LittleFs. */
#ifndef LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #else
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
    #endif
#endif
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifndef LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
            #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #else
            #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER '\0'     /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
            #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #else
            #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** API for Arduino Sd. */
#ifndef LV_USE_FS_ARDUINO_SD
    #ifdef CONFIG_LV_USE_FS_ARDUINO_SD
        #define LV_USE_FS_ARDUINO_SD CONFIG_LV_USE_FS_ARDUINO_SD
    #else
        #define LV_USE_FS_ARDUINO_SD 0
    #endif
#endif
#if LV_USE_FS_ARDUINO_SD
    #ifndef LV_FS_ARDUINO_SD_LETTER
        #ifdef CONFIG_LV_FS_ARDUINO_SD_LETTER
            #define LV_FS_ARDUINO_SD_LETTER CONFIG_LV_FS_ARDUINO_SD_LETTER
        #else
            #define LV_FS_ARDUINO_SD_LETTER '\0'          /**< Set an upper cased letter on which the drive will accessible (e.g. 'A') */
        #endif
    #endif
    #ifndef LV_FS_ARDUINO_SD_PATH
        #ifdef CONFIG_LV_FS_ARDUINO_SD_PATH
            #define LV_FS_ARDUINO_SD_PATH CONFIG_LV_FS_ARDUINO_SD_PATH
        #else
            #define LV_FS_ARDUINO_SD_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
        #endif
    #endif
#endif

/** LODEPNG decoder library */
#ifndef LV_USE_LODEPNG
    #ifdef CONFIG_LV_USE_LODEPNG
        #define LV_USE_LODEPNG CONFIG_LV_USE_LODEPNG
    #else
        #define LV_USE_LODEPNG 0
    #endif
#endif

/** PNG decoder(libpng) library */
#ifndef LV_USE_LIBPNG
    #ifdef CONFIG_LV_USE_LIBPNG
        #define LV_USE_LIBPNG CONFIG_LV_USE_LIBPNG
    #else
        #define LV_USE_LIBPNG 0
    #endif
#endif

/** BMP decoder library */
#ifndef LV_USE_BMP
    #ifdef CONFIG_LV_USE_BMP
        #define LV_USE_BMP CONFIG_LV_USE_BMP
    #else
        #define LV_USE_BMP 0
    #endif
#endif

/** JPG + split JPG decoder library.
 *  Split JPG is a custom format optimized for embedded systems. */
#ifndef LV_USE_TJPGD
    #ifdef CONFIG_LV_USE_TJPGD
        #define LV_USE_TJPGD CONFIG_LV_USE_TJPGD
    #else
        #define LV_USE_TJPGD 0
    #endif
#endif

/** libjpeg-turbo decoder library.
 *  - Supports complete JPEG specifications and high-performance JPEG decoding. */
#ifndef LV_USE_LIBJPEG_TURBO
    #ifdef CONFIG_LV_USE_LIBJPEG_TURBO
        #define LV_USE_LIBJPEG_TURBO CONFIG_LV_USE_LIBJPEG_TURBO
    #else
        #define LV_USE_LIBJPEG_TURBO 0
    #endif
#endif

/** GIF decoder library */
#ifndef LV_USE_GIF
    #ifdef CONFIG_LV_USE_GIF
        #define LV_USE_GIF CONFIG_LV_USE_GIF
    #else
        #define LV_USE_GIF 0
    #endif
#endif
#if LV_USE_GIF
    /** GIF decoder accelerate */
    #ifndef LV_GIF_CACHE_DECODE_DATA
        #ifdef CONFIG_LV_GIF_CACHE_DECODE_DATA
            #define LV_GIF_CACHE_DECODE_DATA CONFIG_LV_GIF_CACHE_DECODE_DATA
        #else
            #define LV_GIF_CACHE_DECODE_DATA 0
        #endif
    #endif
#endif


/** Decode bin images to RAM */
#ifndef LV_BIN_DECODER_RAM_LOAD
    #ifdef CONFIG_LV_BIN_DECODER_RAM_LOAD
        #define LV_BIN_DECODER_RAM_LOAD CONFIG_LV_BIN_DECODER_RAM_LOAD
    #else
        #define LV_BIN_DECODER_RAM_LOAD 0
    #endif
#endif

/** RLE decompress library */
#ifndef LV_USE_RLE
    #ifdef CONFIG_LV_USE_RLE
        #define LV_USE_RLE CONFIG_LV_USE_RLE
    #else
        #define LV_USE_RLE 0
    #endif
#endif

/** QR code library */
#ifndef LV_USE_QRCODE
    #ifdef CONFIG_LV_USE_QRCODE
        #define LV_USE_QRCODE CONFIG_LV_USE_QRCODE
    #else
        #define LV_USE_QRCODE 0
    #endif
#endif

/** Barcode code library */
#ifndef LV_USE_BARCODE
    #ifdef CONFIG_LV_USE_BARCODE
        #define LV_USE_BARCODE CONFIG_LV_USE_BARCODE
    #else
        #define LV_USE_BARCODE 0
    #endif
#endif

/** FreeType library */
#ifndef LV_USE_FREETYPE
    #ifdef CONFIG_LV_USE_FREETYPE
        #define LV_USE_FREETYPE CONFIG_LV_USE_FREETYPE
    #else
        #define LV_USE_FREETYPE 0
    #endif
#endif
#if LV_USE_FREETYPE
    /** Let FreeType use LVGL memory and file porting */
    #ifndef LV_FREETYPE_USE_LVGL_PORT
        #ifdef CONFIG_LV_FREETYPE_USE_LVGL_PORT
            #define LV_FREETYPE_USE_LVGL_PORT CONFIG_LV_FREETYPE_USE_LVGL_PORT
        #else
            #define LV_FREETYPE_USE_LVGL_PORT 0
        #endif
    #endif

    /** Cache count of glyphs in FreeType, i.e. number of glyphs that can be cached.
     *  The higher the value, the more memory will be used. */
    #ifndef LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
            #define LV_FREETYPE_CACHE_FT_GLYPH_CNT CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #else
            #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
        #endif
    #endif
#endif

/** Built-in TTF decoder */
#ifndef LV_USE_TINY_TTF
    #ifdef CONFIG_LV_USE_TINY_TTF
        #define LV_USE_TINY_TTF CONFIG_LV_USE_TINY_TTF
    #else
        #define LV_USE_TINY_TTF 0
    #endif
#endif
#if LV_USE_TINY_TTF
    /* Enable loading TTF data from files */
    #ifndef LV_TINY_TTF_FILE_SUPPORT
        #ifdef CONFIG_LV_TINY_TTF_FILE_SUPPORT
            #define LV_TINY_TTF_FILE_SUPPORT CONFIG_LV_TINY_TTF_FILE_SUPPORT
        #else
            #define LV_TINY_TTF_FILE_SUPPORT 0
        #endif
    #endif
    #ifndef LV_TINY_TTF_CACHE_GLYPH_CNT
        #ifdef CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
            #define LV_TINY_TTF_CACHE_GLYPH_CNT CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
        #else
            #define LV_TINY_TTF_CACHE_GLYPH_CNT 256
        #endif
    #endif
#endif

/** Rlottie library */
#ifndef LV_USE_RLOTTIE
    #ifdef CONFIG_LV_USE_RLOTTIE
        #define LV_USE_RLOTTIE CONFIG_LV_USE_RLOTTIE
    #else
        #define LV_USE_RLOTTIE 0
    #endif
#endif

/** Enable Vector Graphic APIs
 *  - Requires `LV_USE_MATRIX = 1` */
#ifndef LV_USE_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_VECTOR_GRAPHIC
        #define LV_USE_VECTOR_GRAPHIC CONFIG_LV_USE_VECTOR_GRAPHIC
    #else
        #define LV_USE_VECTOR_GRAPHIC  0
    #endif
#endif

/** Enable ThorVG (vector graphics library) from the src/libs folder */
#ifndef LV_USE_THORVG_INTERNAL
    #ifdef CONFIG_LV_USE_THORVG_INTERNAL
        #define LV_USE_THORVG_INTERNAL CONFIG_LV_USE_THORVG_INTERNAL
    #else
        #define LV_USE_THORVG_INTERNAL 0
    #endif
#endif

/** Enable ThorVG by assuming that its installed and linked to the project */
#ifndef LV_USE_THORVG_EXTERNAL
    #ifdef CONFIG_LV_USE_THORVG_EXTERNAL
        #define LV_USE_THORVG_EXTERNAL CONFIG_LV_USE_THORVG_EXTERNAL
    #else
        #define LV_USE_THORVG_EXTERNAL 0
    #endif
#endif

/** Use lvgl built-in LZ4 lib */
#ifndef LV_USE_LZ4_INTERNAL
    #ifdef CONFIG_LV_USE_LZ4_INTERNAL
        #define LV_USE_LZ4_INTERNAL CONFIG_LV_USE_LZ4_INTERNAL
    #else
        #define LV_USE_LZ4_INTERNAL  0
    #endif
#endif

/** Use external LZ4 library */
#ifndef LV_USE_LZ4_EXTERNAL
    #ifdef CONFIG_LV_USE_LZ4_EXTERNAL
        #define LV_USE_LZ4_EXTERNAL CONFIG_LV_USE_LZ4_EXTERNAL
    #else
        #define LV_USE_LZ4_EXTERNAL  0
    #endif
#endif

/*SVG library
 *  - Requires `LV_USE_VECTOR_GRAPHIC = 1` */
#ifndef LV_USE_SVG
    #ifdef CONFIG_LV_USE_SVG
        #define LV_USE_SVG CONFIG_LV_USE_SVG
    #else
        #define LV_USE_SVG 0
    #endif
#endif
#ifndef LV_USE_SVG_ANIMATION
    #ifdef CONFIG_LV_USE_SVG_ANIMATION
        #define LV_USE_SVG_ANIMATION CONFIG_LV_USE_SVG_ANIMATION
    #else
        #define LV_USE_SVG_ANIMATION 0
    #endif
#endif
#ifndef LV_USE_SVG_DEBUG
    #ifdef CONFIG_LV_USE_SVG_DEBUG
        #define LV_USE_SVG_DEBUG CONFIG_LV_USE_SVG_DEBUG
    #else
        #define LV_USE_SVG_DEBUG 0
    #endif
#endif

/** FFmpeg library for image decoding and playing videos.
 *  Supports all major image formats so do not enable other image decoder with it. */
#ifndef LV_USE_FFMPEG
    #ifdef CONFIG_LV_USE_FFMPEG
        #define LV_USE_FFMPEG CONFIG_LV_USE_FFMPEG
    #else
        #define LV_USE_FFMPEG 0
    #endif
#endif
#if LV_USE_FFMPEG
    /** Dump input information to stderr */
    #ifndef LV_FFMPEG_DUMP_FORMAT
        #ifdef CONFIG_LV_FFMPEG_DUMP_FORMAT
            #define LV_FFMPEG_DUMP_FORMAT CONFIG_LV_FFMPEG_DUMP_FORMAT
        #else
            #define LV_FFMPEG_DUMP_FORMAT 0
        #endif
    #endif
    /** Use lvgl file path in FFmpeg Player widget 
     *  You won't be able to open URLs after enabling this feature.
     *  Note that FFmpeg image decoder will always use lvgl file system. */
    #ifndef LV_FFMPEG_PLAYER_USE_LV_FS
        #ifdef CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
            #define LV_FFMPEG_PLAYER_USE_LV_FS CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
        #else
            #define LV_FFMPEG_PLAYER_USE_LV_FS 0
        #endif
    #endif
#endif

/*==================
 * OTHERS
 *==================*/
/* Documentation for several of the below items can be found here: https://docs.lvgl.io/master/others/index.html . */

/** 1: Enable API to take snapshot for object */
#ifndef LV_USE_SNAPSHOT
    #ifdef CONFIG_LV_USE_SNAPSHOT
        #define LV_USE_SNAPSHOT CONFIG_LV_USE_SNAPSHOT
    #else
        #define LV_USE_SNAPSHOT 0
    #endif
#endif

/** 1: Enable system monitor component */
#ifndef LV_USE_SYSMON
    #ifdef CONFIG_LV_USE_SYSMON
        #define LV_USE_SYSMON CONFIG_LV_USE_SYSMON
    #else
        #define LV_USE_SYSMON   0
    #endif
#endif
#if LV_USE_SYSMON
    /** Get the idle percentage. E.g. uint32_t my_get_idle(void); */
    #ifndef LV_SYSMON_GET_IDLE
        #ifdef CONFIG_LV_SYSMON_GET_IDLE
            #define LV_SYSMON_GET_IDLE CONFIG_LV_SYSMON_GET_IDLE
        #else
            #define LV_SYSMON_GET_IDLE lv_timer_get_idle
        #endif
    #endif

    /** 1: Show CPU usage and FPS count.
     *  - Requires `LV_USE_SYSMON = 1` */
    #ifndef LV_USE_PERF_MONITOR
        #ifdef CONFIG_LV_USE_PERF_MONITOR
            #define LV_USE_PERF_MONITOR CONFIG_LV_USE_PERF_MONITOR
        #else
            #define LV_USE_PERF_MONITOR 0
        #endif
    #endif
    #if LV_USE_PERF_MONITOR
        #ifndef LV_USE_PERF_MONITOR_POS
            #ifdef CONFIG_LV_USE_PERF_MONITOR_POS
                #define LV_USE_PERF_MONITOR_POS CONFIG_LV_USE_PERF_MONITOR_POS
            #else
                #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
            #endif
        #endif

        /** 0: Displays performance data on the screen; 1: Prints performance data using log. */
        #ifndef LV_USE_PERF_MONITOR_LOG_MODE
            #ifdef CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
                #define LV_USE_PERF_MONITOR_LOG_MODE CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
            #else
                #define LV_USE_PERF_MONITOR_LOG_MODE 0
            #endif
        #endif
    #endif

    /** 1: Show used memory and memory fragmentation.
     *     - Requires `LV_USE_STDLIB_MALLOC = LV_STDLIB_BUILTIN`
     *     - Requires `LV_USE_SYSMON = 1`*/
    #ifndef LV_USE_MEM_MONITOR
        #ifdef CONFIG_LV_USE_MEM_MONITOR
            #define LV_USE_MEM_MONITOR CONFIG_LV_USE_MEM_MONITOR
        #else
            #define LV_USE_MEM_MONITOR 0
        #endif
    #endif
    #if LV_USE_MEM_MONITOR
        #ifndef LV_USE_MEM_MONITOR_POS
            #ifdef CONFIG_LV_USE_MEM_MONITOR_POS
                #define LV_USE_MEM_MONITOR_POS CONFIG_LV_USE_MEM_MONITOR_POS
            #else
                #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
            #endif
        #endif
    #endif
#endif /*LV_USE_SYSMON*/

/** 1: Enable runtime performance profiler */
#ifndef LV_USE_PROFILER
    #ifdef CONFIG_LV_USE_PROFILER
        #define LV_USE_PROFILER CONFIG_LV_USE_PROFILER
    #else
        #define LV_USE_PROFILER 0
    #endif
#endif
#if LV_USE_PROFILER
    /** 1: Enable the built-in profiler */
    #ifndef LV_USE_PROFILER_BUILTIN
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_PROFILER_BUILTIN
                #define LV_USE_PROFILER_BUILTIN CONFIG_LV_USE_PROFILER_BUILTIN
            #else
                #define LV_USE_PROFILER_BUILTIN 0
            #endif
        #else
            #define LV_USE_PROFILER_BUILTIN 1
        #endif
    #endif
    #if LV_USE_PROFILER_BUILTIN
        /** Default profiler trace buffer size */
        #ifndef LV_PROFILER_BUILTIN_BUF_SIZE
            #ifdef CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
                #define LV_PROFILER_BUILTIN_BUF_SIZE CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
            #else
                #define LV_PROFILER_BUILTIN_BUF_SIZE (16 * 1024)     /**< [bytes] */
            #endif
        #endif
    #endif

    /** Header to include for profiler */
    #ifndef LV_PROFILER_INCLUDE
        #ifdef CONFIG_LV_PROFILER_INCLUDE
            #define LV_PROFILER_INCLUDE CONFIG_LV_PROFILER_INCLUDE
        #else
            #define LV_PROFILER_INCLUDE "lvgl/src/misc/lv_profiler_builtin.h"
        #endif
    #endif

    /** Profiler start point function */
    #ifndef LV_PROFILER_BEGIN
        #ifdef CONFIG_LV_PROFILER_BEGIN
            #define LV_PROFILER_BEGIN CONFIG_LV_PROFILER_BEGIN
        #else
            #define LV_PROFILER_BEGIN    LV_PROFILER_BUILTIN_BEGIN
        #endif
    #endif

    /** Profiler end point function */
    #ifndef LV_PROFILER_END
        #ifdef CONFIG_LV_PROFILER_END
            #define LV_PROFILER_END CONFIG_LV_PROFILER_END
        #else
            #define LV_PROFILER_END      LV_PROFILER_BUILTIN_END
        #endif
    #endif

    /** Profiler start point function with custom tag */
    #ifndef LV_PROFILER_BEGIN_TAG
        #ifdef CONFIG_LV_PROFILER_BEGIN_TAG
            #define LV_PROFILER_BEGIN_TAG CONFIG_LV_PROFILER_BEGIN_TAG
        #else
            #define LV_PROFILER_BEGIN_TAG LV_PROFILER_BUILTIN_BEGIN_TAG
        #endif
    #endif

    /** Profiler end point function with custom tag */
    #ifndef LV_PROFILER_END_TAG
        #ifdef CONFIG_LV_PROFILER_END_TAG
            #define LV_PROFILER_END_TAG CONFIG_LV_PROFILER_END_TAG
        #else
            #define LV_PROFILER_END_TAG   LV_PROFILER_BUILTIN_END_TAG
        #endif
    #endif

    /*Enable layout profiler*/
    #ifndef LV_PROFILER_LAYOUT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_LAYOUT
                #define LV_PROFILER_LAYOUT CONFIG_LV_PROFILER_LAYOUT
            #else
                #define LV_PROFILER_LAYOUT 0
            #endif
        #else
            #define LV_PROFILER_LAYOUT 1
        #endif
    #endif

    /*Enable disp refr profiler*/
    #ifndef LV_PROFILER_REFR
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_REFR
                #define LV_PROFILER_REFR CONFIG_LV_PROFILER_REFR
            #else
                #define LV_PROFILER_REFR 0
            #endif
        #else
            #define LV_PROFILER_REFR 1
        #endif
    #endif

    /*Enable draw profiler*/
    #ifndef LV_PROFILER_DRAW
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_DRAW
                #define LV_PROFILER_DRAW CONFIG_LV_PROFILER_DRAW
            #else
                #define LV_PROFILER_DRAW 0
            #endif
        #else
            #define LV_PROFILER_DRAW 1
        #endif
    #endif

    /*Enable indev profiler*/
    #ifndef LV_PROFILER_INDEV
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_INDEV
                #define LV_PROFILER_INDEV CONFIG_LV_PROFILER_INDEV
            #else
                #define LV_PROFILER_INDEV 0
            #endif
        #else
            #define LV_PROFILER_INDEV 1
        #endif
    #endif

    /*Enable decoder profiler*/
    #ifndef LV_PROFILER_DECODER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_DECODER
                #define LV_PROFILER_DECODER CONFIG_LV_PROFILER_DECODER
            #else
                #define LV_PROFILER_DECODER 0
            #endif
        #else
            #define LV_PROFILER_DECODER 1
        #endif
    #endif

    /*Enable font profiler*/
    #ifndef LV_PROFILER_FONT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_FONT
                #define LV_PROFILER_FONT CONFIG_LV_PROFILER_FONT
            #else
                #define LV_PROFILER_FONT 0
            #endif
        #else
            #define LV_PROFILER_FONT 1
        #endif
    #endif

    /*Enable fs profiler*/
    #ifndef LV_PROFILER_FS
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_FS
                #define LV_PROFILER_FS CONFIG_LV_PROFILER_FS
            #else
                #define LV_PROFILER_FS 0
            #endif
        #else
            #define LV_PROFILER_FS 1
        #endif
    #endif

    /*Enable style profiler*/
    #ifndef LV_PROFILER_STYLE
        #ifdef CONFIG_LV_PROFILER_STYLE
            #define LV_PROFILER_STYLE CONFIG_LV_PROFILER_STYLE
        #else
            #define LV_PROFILER_STYLE 0
        #endif
    #endif

    /*Enable timer profiler*/
    #ifndef LV_PROFILER_TIMER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_TIMER
                #define LV_PROFILER_TIMER CONFIG_LV_PROFILER_TIMER
            #else
                #define LV_PROFILER_TIMER 0
            #endif
        #else
            #define LV_PROFILER_TIMER 1
        #endif
    #endif

    /*Enable cache profiler*/
    #ifndef LV_PROFILER_CACHE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_CACHE
                #define LV_PROFILER_CACHE CONFIG_LV_PROFILER_CACHE
            #else
                #define LV_PROFILER_CACHE 0
            #endif
        #else
            #define LV_PROFILER_CACHE 1
        #endif
    #endif

    /*Enable event profiler*/
    #ifndef LV_PROFILER_EVENT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_PROFILER_EVENT
                #define LV_PROFILER_EVENT CONFIG_LV_PROFILER_EVENT
            #else
                #define LV_PROFILER_EVENT 0
            #endif
        #else
            #define LV_PROFILER_EVENT 1
        #endif
    #endif
#endif

/** 1: Enable Monkey test */
#ifndef LV_USE_MONKEY
    #ifdef CONFIG_LV_USE_MONKEY
        #define LV_USE_MONKEY CONFIG_LV_USE_MONKEY
    #else
        #define LV_USE_MONKEY 0
    #endif
#endif

/** 1: Enable grid navigation */
#ifndef LV_USE_GRIDNAV
    #ifdef CONFIG_LV_USE_GRIDNAV
        #define LV_USE_GRIDNAV CONFIG_LV_USE_GRIDNAV
    #else
        #define LV_USE_GRIDNAV 0
    #endif
#endif

/** 1: Enable `lv_obj` fragment logic */
#ifndef LV_USE_FRAGMENT
    #ifdef CONFIG_LV_USE_FRAGMENT
        #define LV_USE_FRAGMENT CONFIG_LV_USE_FRAGMENT
    #else
        #define LV_USE_FRAGMENT 0
    #endif
#endif

/** 1: Support using images as font in label or span widgets */
#ifndef LV_USE_IMGFONT
    #ifdef CONFIG_LV_USE_IMGFONT
        #define LV_USE_IMGFONT CONFIG_LV_USE_IMGFONT
    #else
        #define LV_USE_IMGFONT 0
    #endif
#endif

/** 1: Enable an observer pattern implementation */
#ifndef LV_USE_OBSERVER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBSERVER
            #define LV_USE_OBSERVER CONFIG_LV_USE_OBSERVER
        #else
            #define LV_USE_OBSERVER 0
        #endif
    #else
        #define LV_USE_OBSERVER 1
    #endif
#endif

/** 1: Enable Pinyin input method
 *  - Requires: lv_keyboard */
#ifndef LV_USE_IME_PINYIN
    #ifdef CONFIG_LV_USE_IME_PINYIN
        #define LV_USE_IME_PINYIN CONFIG_LV_USE_IME_PINYIN
    #else
        #define LV_USE_IME_PINYIN 0
    #endif
#endif
#if LV_USE_IME_PINYIN
    /** 1: Use default thesaurus.
     *  @note  If you do not use the default thesaurus, be sure to use `lv_ime_pinyin` after setting the thesaurus. */
    #ifndef LV_IME_PINYIN_USE_DEFAULT_DICT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
                #define LV_IME_PINYIN_USE_DEFAULT_DICT CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
            #else
                #define LV_IME_PINYIN_USE_DEFAULT_DICT 0
            #endif
        #else
            #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
        #endif
    #endif
    /** Set maximum number of candidate panels that can be displayed.
     *  @note  This needs to be adjusted according to size of screen. */
    #ifndef LV_IME_PINYIN_CAND_TEXT_NUM
        #ifdef CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
            #define LV_IME_PINYIN_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
        #else
            #define LV_IME_PINYIN_CAND_TEXT_NUM 6
        #endif
    #endif

    /** Use 9-key input (k9). */
    #ifndef LV_IME_PINYIN_USE_K9_MODE
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_IME_PINYIN_USE_K9_MODE
                #define LV_IME_PINYIN_USE_K9_MODE CONFIG_LV_IME_PINYIN_USE_K9_MODE
            #else
                #define LV_IME_PINYIN_USE_K9_MODE 0
            #endif
        #else
            #define LV_IME_PINYIN_USE_K9_MODE      1
        #endif
    #endif
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #ifndef LV_IME_PINYIN_K9_CAND_TEXT_NUM
            #ifdef CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
                #define LV_IME_PINYIN_K9_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
            #else
                #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
            #endif
        #endif
    #endif /*LV_IME_PINYIN_USE_K9_MODE*/
#endif

/** 1: Enable file explorer.
 *  - Requires: lv_table */
#ifndef LV_USE_FILE_EXPLORER
    #ifdef CONFIG_LV_USE_FILE_EXPLORER
        #define LV_USE_FILE_EXPLORER CONFIG_LV_USE_FILE_EXPLORER
    #else
        #define LV_USE_FILE_EXPLORER                     0
    #endif
#endif
#if LV_USE_FILE_EXPLORER
    /** Maximum length of path */
    #ifndef LV_FILE_EXPLORER_PATH_MAX_LEN
        #ifdef CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
            #define LV_FILE_EXPLORER_PATH_MAX_LEN CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
        #else
            #define LV_FILE_EXPLORER_PATH_MAX_LEN        (128)
        #endif
    #endif
    /** Quick access bar, 1:use, 0:do not use.
     *  - Requires: lv_list */
    #ifndef LV_FILE_EXPLORER_QUICK_ACCESS
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
                #define LV_FILE_EXPLORER_QUICK_ACCESS CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
            #else
                #define LV_FILE_EXPLORER_QUICK_ACCESS 0
            #endif
        #else
            #define LV_FILE_EXPLORER_QUICK_ACCESS        1
        #endif
    #endif
#endif

/** 1: Enable freetype font manager
 *  - Requires: LV_USE_FREETYPE */
#ifndef LV_USE_FONT_MANAGER
    #ifdef CONFIG_LV_USE_FONT_MANAGER
        #define LV_USE_FONT_MANAGER CONFIG_LV_USE_FONT_MANAGER
    #else
        #define LV_USE_FONT_MANAGER                     0
    #endif
#endif
#if LV_USE_FONT_MANAGER

/**Font manager name max length*/
#ifndef LV_FONT_MANAGER_NAME_MAX_LEN
    #ifdef CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
        #define LV_FONT_MANAGER_NAME_MAX_LEN CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
    #else
        #define LV_FONT_MANAGER_NAME_MAX_LEN            32
    #endif
#endif

#endif

/** Enable loading XML UIs runtime */
#ifndef LV_USE_XML
    #ifdef CONFIG_LV_USE_XML
        #define LV_USE_XML CONFIG_LV_USE_XML
    #else
        #define LV_USE_XML	0
    #endif
#endif

/*==================
 * DEVICES
 *==================*/

/** Use SDL to open window on PC and handle mouse and keyboard. */
#ifndef LV_USE_SDL
    #ifdef CONFIG_LV_USE_SDL
        #define LV_USE_SDL CONFIG_LV_USE_SDL
    #else
        #define LV_USE_SDL              0
    #endif
#endif
#if LV_USE_SDL
    #ifndef LV_SDL_INCLUDE_PATH
        #ifdef CONFIG_LV_SDL_INCLUDE_PATH
            #define LV_SDL_INCLUDE_PATH CONFIG_LV_SDL_INCLUDE_PATH
        #else
            #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
        #endif
    #endif
    #ifndef LV_SDL_RENDER_MODE
        #ifdef CONFIG_LV_SDL_RENDER_MODE
            #define LV_SDL_RENDER_MODE CONFIG_LV_SDL_RENDER_MODE
        #else
            #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT   /**< LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance */
        #endif
    #endif
    #ifndef LV_SDL_BUF_COUNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_BUF_COUNT
                #define LV_SDL_BUF_COUNT CONFIG_LV_SDL_BUF_COUNT
            #else
                #define LV_SDL_BUF_COUNT 0
            #endif
        #else
            #define LV_SDL_BUF_COUNT        1    /**< 1 or 2 */
        #endif
    #endif
    #ifndef LV_SDL_ACCELERATED
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_ACCELERATED
                #define LV_SDL_ACCELERATED CONFIG_LV_SDL_ACCELERATED
            #else
                #define LV_SDL_ACCELERATED 0
            #endif
        #else
            #define LV_SDL_ACCELERATED      1    /**< 1: Use hardware acceleration*/
        #endif
    #endif
    #ifndef LV_SDL_FULLSCREEN
        #ifdef CONFIG_LV_SDL_FULLSCREEN
            #define LV_SDL_FULLSCREEN CONFIG_LV_SDL_FULLSCREEN
        #else
            #define LV_SDL_FULLSCREEN       0    /**< 1: Make the window full screen by default */
        #endif
    #endif
    #ifndef LV_SDL_DIRECT_EXIT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_SDL_DIRECT_EXIT
                #define LV_SDL_DIRECT_EXIT CONFIG_LV_SDL_DIRECT_EXIT
            #else
                #define LV_SDL_DIRECT_EXIT 0
            #endif
        #else
            #define LV_SDL_DIRECT_EXIT      1    /**< 1: Exit the application when all SDL windows are closed */
        #endif
    #endif
    #ifndef LV_SDL_MOUSEWHEEL_MODE
        #ifdef CONFIG_LV_SDL_MOUSEWHEEL_MODE
            #define LV_SDL_MOUSEWHEEL_MODE CONFIG_LV_SDL_MOUSEWHEEL_MODE
        #else
            #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER  /*LV_SDL_MOUSEWHEEL_MODE_ENCODER/CROWN*/
        #endif
    #endif
#endif

/** Use X11 to open window on Linux desktop and handle mouse and keyboard */
#ifndef LV_USE_X11
    #ifdef CONFIG_LV_USE_X11
        #define LV_USE_X11 CONFIG_LV_USE_X11
    #else
        #define LV_USE_X11              0
    #endif
#endif
#if LV_USE_X11
    #ifndef LV_X11_DIRECT_EXIT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_DIRECT_EXIT
                #define LV_X11_DIRECT_EXIT CONFIG_LV_X11_DIRECT_EXIT
            #else
                #define LV_X11_DIRECT_EXIT 0
            #endif
        #else
            #define LV_X11_DIRECT_EXIT         1  /**< Exit application when all X11 windows have been closed */
        #endif
    #endif
    #ifndef LV_X11_DOUBLE_BUFFER
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_DOUBLE_BUFFER
                #define LV_X11_DOUBLE_BUFFER CONFIG_LV_X11_DOUBLE_BUFFER
            #else
                #define LV_X11_DOUBLE_BUFFER 0
            #endif
        #else
            #define LV_X11_DOUBLE_BUFFER       1  /**< Use double buffers for rendering */
        #endif
    #endif
    /* Select only 1 of the following render modes (LV_X11_RENDER_MODE_PARTIAL preferred!). */
    #ifndef LV_X11_RENDER_MODE_PARTIAL
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_X11_RENDER_MODE_PARTIAL
                #define LV_X11_RENDER_MODE_PARTIAL CONFIG_LV_X11_RENDER_MODE_PARTIAL
            #else
                #define LV_X11_RENDER_MODE_PARTIAL 0
            #endif
        #else
            #define LV_X11_RENDER_MODE_PARTIAL 1  /**< Partial render mode (preferred) */
        #endif
    #endif
    #ifndef LV_X11_RENDER_MODE_DIRECT
        #ifdef CONFIG_LV_X11_RENDER_MODE_DIRECT
            #define LV_X11_RENDER_MODE_DIRECT CONFIG_LV_X11_RENDER_MODE_DIRECT
        #else
            #define LV_X11_RENDER_MODE_DIRECT  0  /**< Direct render mode */
        #endif
    #endif
    #ifndef LV_X11_RENDER_MODE_FULL
        #ifdef CONFIG_LV_X11_RENDER_MODE_FULL
            #define LV_X11_RENDER_MODE_FULL CONFIG_LV_X11_RENDER_MODE_FULL
        #else
            #define LV_X11_RENDER_MODE_FULL    0  /**< Full render mode */
        #endif
    #endif
#endif

/** Use Wayland to open a window and handle input on Linux or BSD desktops */
#ifndef LV_USE_WAYLAND
    #ifdef CONFIG_LV_USE_WAYLAND
        #define LV_USE_WAYLAND CONFIG_LV_USE_WAYLAND
    #else
        #define LV_USE_WAYLAND          0
    #endif
#endif
#if LV_USE_WAYLAND
    #ifndef LV_WAYLAND_WINDOW_DECORATIONS
        #ifdef CONFIG_LV_WAYLAND_WINDOW_DECORATIONS
            #define LV_WAYLAND_WINDOW_DECORATIONS CONFIG_LV_WAYLAND_WINDOW_DECORATIONS
        #else
            #define LV_WAYLAND_WINDOW_DECORATIONS   0    /**< Draw client side window decorations only necessary on Mutter/GNOME */
        #endif
    #endif
    #ifndef LV_WAYLAND_WL_SHELL
        #ifdef CONFIG_LV_WAYLAND_WL_SHELL
            #define LV_WAYLAND_WL_SHELL CONFIG_LV_WAYLAND_WL_SHELL
        #else
            #define LV_WAYLAND_WL_SHELL             0    /**< Use the legacy wl_shell protocol instead of the default XDG shell */
        #endif
    #endif
#endif

/** Driver for /dev/fb */
#ifndef LV_USE_LINUX_FBDEV
    #ifdef CONFIG_LV_USE_LINUX_FBDEV
        #define LV_USE_LINUX_FBDEV CONFIG_LV_USE_LINUX_FBDEV
    #else
        #define LV_USE_LINUX_FBDEV      0
    #endif
#endif
#if LV_USE_LINUX_FBDEV
    #ifndef LV_LINUX_FBDEV_BSD
        #ifdef CONFIG_LV_LINUX_FBDEV_BSD
            #define LV_LINUX_FBDEV_BSD CONFIG_LV_LINUX_FBDEV_BSD
        #else
            #define LV_LINUX_FBDEV_BSD           0
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_RENDER_MODE
        #ifdef CONFIG_LV_LINUX_FBDEV_RENDER_MODE
            #define LV_LINUX_FBDEV_RENDER_MODE CONFIG_LV_LINUX_FBDEV_RENDER_MODE
        #else
            #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_BUFFER_COUNT
        #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
            #define LV_LINUX_FBDEV_BUFFER_COUNT CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
        #else
            #define LV_LINUX_FBDEV_BUFFER_COUNT  0
        #endif
    #endif
    #ifndef LV_LINUX_FBDEV_BUFFER_SIZE
        #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
            #define LV_LINUX_FBDEV_BUFFER_SIZE CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
        #else
            #define LV_LINUX_FBDEV_BUFFER_SIZE   60
        #endif
    #endif
#endif

/** Use Nuttx to open window and handle touchscreen */
#ifndef LV_USE_NUTTX
    #ifdef CONFIG_LV_USE_NUTTX
        #define LV_USE_NUTTX CONFIG_LV_USE_NUTTX
    #else
        #define LV_USE_NUTTX    0
    #endif
#endif

#if LV_USE_NUTTX
    #ifndef LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #ifdef CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
            #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #else
            #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0
        #endif
    #endif

    #ifndef LV_USE_NUTTX_LIBUV
        #ifdef CONFIG_LV_USE_NUTTX_LIBUV
            #define LV_USE_NUTTX_LIBUV CONFIG_LV_USE_NUTTX_LIBUV
        #else
            #define LV_USE_NUTTX_LIBUV    0
        #endif
    #endif

    /** Use Nuttx custom init API to open window and handle touchscreen */
    #ifndef LV_USE_NUTTX_CUSTOM_INIT
        #ifdef CONFIG_LV_USE_NUTTX_CUSTOM_INIT
            #define LV_USE_NUTTX_CUSTOM_INIT CONFIG_LV_USE_NUTTX_CUSTOM_INIT
        #else
            #define LV_USE_NUTTX_CUSTOM_INIT    0
        #endif
    #endif

    /** Driver for /dev/lcd */
    #ifndef LV_USE_NUTTX_LCD
        #ifdef CONFIG_LV_USE_NUTTX_LCD
            #define LV_USE_NUTTX_LCD CONFIG_LV_USE_NUTTX_LCD
        #else
            #define LV_USE_NUTTX_LCD      0
        #endif
    #endif
    #if LV_USE_NUTTX_LCD
        #ifndef LV_NUTTX_LCD_BUFFER_COUNT
            #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
                #define LV_NUTTX_LCD_BUFFER_COUNT CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
            #else
                #define LV_NUTTX_LCD_BUFFER_COUNT    0
            #endif
        #endif
        #ifndef LV_NUTTX_LCD_BUFFER_SIZE
            #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
                #define LV_NUTTX_LCD_BUFFER_SIZE CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
            #else
                #define LV_NUTTX_LCD_BUFFER_SIZE     60
            #endif
        #endif
    #endif

    /** Driver for /dev/input */
    #ifndef LV_USE_NUTTX_TOUCHSCREEN
        #ifdef CONFIG_LV_USE_NUTTX_TOUCHSCREEN
            #define LV_USE_NUTTX_TOUCHSCREEN CONFIG_LV_USE_NUTTX_TOUCHSCREEN
        #else
            #define LV_USE_NUTTX_TOUCHSCREEN    0
        #endif
    #endif

    /*Touchscreen cursor size in pixels(<=0: disable cursor)*/
    #ifndef LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #ifdef CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
            #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #else
            #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE    0
        #endif
    #endif
#endif

/** Driver for /dev/dri/card */
#ifndef LV_USE_LINUX_DRM
    #ifdef CONFIG_LV_USE_LINUX_DRM
        #define LV_USE_LINUX_DRM CONFIG_LV_USE_LINUX_DRM
    #else
        #define LV_USE_LINUX_DRM        0
    #endif
#endif

/** Interface for TFT_eSPI */
#ifndef LV_USE_TFT_ESPI
    #ifdef CONFIG_LV_USE_TFT_ESPI
        #define LV_USE_TFT_ESPI CONFIG_LV_USE_TFT_ESPI
    #else
        #define LV_USE_TFT_ESPI         0
    #endif
#endif

/** Driver for evdev input devices */
#ifndef LV_USE_EVDEV
    #ifdef CONFIG_LV_USE_EVDEV
        #define LV_USE_EVDEV CONFIG_LV_USE_EVDEV
    #else
        #define LV_USE_EVDEV    0
    #endif
#endif

/** Driver for libinput input devices */
#ifndef LV_USE_LIBINPUT
    #ifdef CONFIG_LV_USE_LIBINPUT
        #define LV_USE_LIBINPUT CONFIG_LV_USE_LIBINPUT
    #else
        #define LV_USE_LIBINPUT    0
    #endif
#endif

#if LV_USE_LIBINPUT
    #ifndef LV_LIBINPUT_BSD
        #ifdef CONFIG_LV_LIBINPUT_BSD
            #define LV_LIBINPUT_BSD CONFIG_LV_LIBINPUT_BSD
        #else
            #define LV_LIBINPUT_BSD    0
        #endif
    #endif

    /** Full keyboard support */
    #ifndef LV_LIBINPUT_XKB
        #ifdef CONFIG_LV_LIBINPUT_XKB
            #define LV_LIBINPUT_XKB CONFIG_LV_LIBINPUT_XKB
        #else
            #define LV_LIBINPUT_XKB             0
        #endif
    #endif
    #if LV_LIBINPUT_XKB
        /** "setxkbmap -query" can help find the right values for your keyboard */
        #ifndef LV_LIBINPUT_XKB_KEY_MAP
            #ifdef CONFIG_LV_LIBINPUT_XKB_KEY_MAP
                #define LV_LIBINPUT_XKB_KEY_MAP CONFIG_LV_LIBINPUT_XKB_KEY_MAP
            #else
                #define LV_LIBINPUT_XKB_KEY_MAP { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
            #endif
        #endif
    #endif
#endif

/* Drivers for LCD devices connected via SPI/parallel port */
#ifndef LV_USE_ST7735
    #ifdef CONFIG_LV_USE_ST7735
        #define LV_USE_ST7735 CONFIG_LV_USE_ST7735
    #else
        #define LV_USE_ST7735        0
    #endif
#endif
#ifndef LV_USE_ST7789
    #ifdef CONFIG_LV_USE_ST7789
        #define LV_USE_ST7789 CONFIG_LV_USE_ST7789
    #else
        #define LV_USE_ST7789        0
    #endif
#endif
#ifndef LV_USE_ST7796
    #ifdef CONFIG_LV_USE_ST7796
        #define LV_USE_ST7796 CONFIG_LV_USE_ST7796
    #else
        #define LV_USE_ST7796        0
    #endif
#endif
#ifndef LV_USE_ILI9341
    #ifdef CONFIG_LV_USE_ILI9341
        #define LV_USE_ILI9341 CONFIG_LV_USE_ILI9341
    #else
        #define LV_USE_ILI9341       0
    #endif
#endif

#ifndef LV_USE_GENERIC_MIPI
    #ifdef CONFIG_LV_USE_GENERIC_MIPI
        #define LV_USE_GENERIC_MIPI CONFIG_LV_USE_GENERIC_MIPI
    #else
        #define LV_USE_GENERIC_MIPI (LV_USE_ST7735 | LV_USE_ST7789 | LV_USE_ST7796 | LV_USE_ILI9341)
    #endif
#endif

/** Driver for Renesas GLCD */
#ifndef LV_USE_RENESAS_GLCDC
    #ifdef CONFIG_LV_USE_RENESAS_GLCDC
        #define LV_USE_RENESAS_GLCDC CONFIG_LV_USE_RENESAS_GLCDC
    #else
        #define LV_USE_RENESAS_GLCDC    0
    #endif
#endif

/** Driver for ST LTDC */
#ifndef LV_USE_ST_LTDC
    #ifdef CONFIG_LV_USE_ST_LTDC
        #define LV_USE_ST_LTDC CONFIG_LV_USE_ST_LTDC
    #else
        #define LV_USE_ST_LTDC    0
    #endif
#endif
#if LV_USE_ST_LTDC
    /* Only used for partial. */
    #ifndef LV_ST_LTDC_USE_DMA2D_FLUSH
        #ifdef CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
            #define LV_ST_LTDC_USE_DMA2D_FLUSH CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
        #else
            #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
        #endif
    #endif
#endif

/** LVGL Windows backend */
#ifndef LV_USE_WINDOWS
    #ifdef CONFIG_LV_USE_WINDOWS
        #define LV_USE_WINDOWS CONFIG_LV_USE_WINDOWS
    #else
        #define LV_USE_WINDOWS    0
    #endif
#endif

/** Use OpenGL to open window on PC and handle mouse and keyboard */
#ifndef LV_USE_OPENGLES
    #ifdef CONFIG_LV_USE_OPENGLES
        #define LV_USE_OPENGLES CONFIG_LV_USE_OPENGLES
    #else
        #define LV_USE_OPENGLES   0
    #endif
#endif
#if LV_USE_OPENGLES
    #ifndef LV_USE_OPENGLES_DEBUG
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_OPENGLES_DEBUG
                #define LV_USE_OPENGLES_DEBUG CONFIG_LV_USE_OPENGLES_DEBUG
            #else
                #define LV_USE_OPENGLES_DEBUG 0
            #endif
        #else
            #define LV_USE_OPENGLES_DEBUG        1    /**< Enable or disable debug for opengles */
        #endif
    #endif
#endif

/** QNX Screen display and input drivers */
#ifndef LV_USE_QNX
    #ifdef CONFIG_LV_USE_QNX
        #define LV_USE_QNX CONFIG_LV_USE_QNX
    #else
        #define LV_USE_QNX              0
    #endif
#endif
#if LV_USE_QNX
    #ifndef LV_QNX_BUF_COUNT
        #ifdef LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_QNX_BUF_COUNT
                #define LV_QNX_BUF_COUNT CONFIG_LV_QNX_BUF_COUNT
            #else
                #define LV_QNX_BUF_COUNT 0
            #endif
        #else
            #define LV_QNX_BUF_COUNT        1    /**< 1 or 2 */
        #endif
    #endif
#endif

/*==================
* EXAMPLES
*==================*/

/** Enable examples to be built with the library. */
#ifndef LV_BUILD_EXAMPLES
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_BUILD_EXAMPLES
            #define LV_BUILD_EXAMPLES CONFIG_LV_BUILD_EXAMPLES
        #else
            #define LV_BUILD_EXAMPLES 0
        #endif
    #else
        #define LV_BUILD_EXAMPLES 1
    #endif
#endif

/*===================
 * DEMO USAGE
 ====================*/

/** Show some widgets. This might be required to increase `LV_MEM_SIZE`. */
#ifndef LV_USE_DEMO_WIDGETS
    #ifdef CONFIG_LV_USE_DEMO_WIDGETS
        #define LV_USE_DEMO_WIDGETS CONFIG_LV_USE_DEMO_WIDGETS
    #else
        #define LV_USE_DEMO_WIDGETS 0
    #endif
#endif

/** Demonstrate usage of encoder and keyboard. */
#ifndef LV_USE_DEMO_KEYPAD_AND_ENCODER
    #ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
    #else
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #endif
#endif

/** Benchmark your system */
#ifndef LV_USE_DEMO_BENCHMARK
    #ifdef CONFIG_LV_USE_DEMO_BENCHMARK
        #define LV_USE_DEMO_BENCHMARK CONFIG_LV_USE_DEMO_BENCHMARK
    #else
        #define LV_USE_DEMO_BENCHMARK 0
    #endif
#endif

/** Render test for each primitive.
 *  - Requires at least 480x272 display. */
#ifndef LV_USE_DEMO_RENDER
    #ifdef CONFIG_LV_USE_DEMO_RENDER
        #define LV_USE_DEMO_RENDER CONFIG_LV_USE_DEMO_RENDER
    #else
        #define LV_USE_DEMO_RENDER 0
    #endif
#endif

/** Stress test for LVGL */
#ifndef LV_USE_DEMO_STRESS
    #ifdef CONFIG_LV_USE_DEMO_STRESS
        #define LV_USE_DEMO_STRESS CONFIG_LV_USE_DEMO_STRESS
    #else
        #define LV_USE_DEMO_STRESS 0
    #endif
#endif

/** Music player demo */
#ifndef LV_USE_DEMO_MUSIC
    #ifdef CONFIG_LV_USE_DEMO_MUSIC
        #define LV_USE_DEMO_MUSIC CONFIG_LV_USE_DEMO_MUSIC
    #else
        #define LV_USE_DEMO_MUSIC 0
    #endif
#endif
#if LV_USE_DEMO_MUSIC
    #ifndef LV_DEMO_MUSIC_SQUARE
        #ifdef CONFIG_LV_DEMO_MUSIC_SQUARE
            #define LV_DEMO_MUSIC_SQUARE CONFIG_LV_DEMO_MUSIC_SQUARE
        #else
            #define LV_DEMO_MUSIC_SQUARE    0
        #endif
    #endif
    #ifndef LV_DEMO_MUSIC_LANDSCAPE
        #ifdef CONFIG_LV_DEMO_MUSIC_LANDSCAPE
            #define LV_DEMO_MUSIC_LANDSCAPE CONFIG_LV_DEMO_MUSIC_LANDSCAPE
        #else
            #define LV_DEMO_MUSIC_LANDSCAPE 0
        #endif
    #endif
    #ifndef LV_DEMO_MUSIC_ROUND
        #ifdef CONFIG_LV_DEMO_MUSIC_ROUND
            #define LV_DEMO_MUSIC_ROUND CONFIG_LV_DEMO_MUSIC_ROUND
        #else
            #define LV_DEMO_MUSIC_ROUND     0
        #endif
    #endif
    #ifndef LV_DEMO_MUSIC_LARGE
        #ifdef CONFIG_LV_DEMO_MUSIC_LARGE
            #define LV_DEMO_MUSIC_LARGE CONFIG_LV_DEMO_MUSIC_LARGE
        #else
            #define LV_DEMO_MUSIC_LARGE     0
        #endif
    #endif
    #ifndef LV_DEMO_MUSIC_AUTO_PLAY
        #ifdef CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
            #define LV_DEMO_MUSIC_AUTO_PLAY CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
        #else
            #define LV_DEMO_MUSIC_AUTO_PLAY 0
        #endif
    #endif
#endif

/** Flex layout demo */
#ifndef LV_USE_DEMO_FLEX_LAYOUT
    #ifdef CONFIG_LV_USE_DEMO_FLEX_LAYOUT
        #define LV_USE_DEMO_FLEX_LAYOUT CONFIG_LV_USE_DEMO_FLEX_LAYOUT
    #else
        #define LV_USE_DEMO_FLEX_LAYOUT     0
    #endif
#endif

/** Smart-phone like multi-language demo */
#ifndef LV_USE_DEMO_MULTILANG
    #ifdef CONFIG_LV_USE_DEMO_MULTILANG
        #define LV_USE_DEMO_MULTILANG CONFIG_LV_USE_DEMO_MULTILANG
    #else
        #define LV_USE_DEMO_MULTILANG       0
    #endif
#endif

/** Widget transformation demo */
#ifndef LV_USE_DEMO_TRANSFORM
    #ifdef CONFIG_LV_USE_DEMO_TRANSFORM
        #define LV_USE_DEMO_TRANSFORM CONFIG_LV_USE_DEMO_TRANSFORM
    #else
        #define LV_USE_DEMO_TRANSFORM       0
    #endif
#endif

/** Demonstrate scroll settings */
#ifndef LV_USE_DEMO_SCROLL
    #ifdef CONFIG_LV_USE_DEMO_SCROLL
        #define LV_USE_DEMO_SCROLL CONFIG_LV_USE_DEMO_SCROLL
    #else
        #define LV_USE_DEMO_SCROLL          0
    #endif
#endif

/** Vector graphic demo */
#ifndef LV_USE_DEMO_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
        #define LV_USE_DEMO_VECTOR_GRAPHIC CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
    #else
        #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    #endif
#endif

/*E-bike demo with Lottie animations (if LV_USE_LOTTIE is enabled)*/
#ifndef LV_USE_DEMO_EBIKE
    #ifdef CONFIG_LV_USE_DEMO_EBIKE
        #define LV_USE_DEMO_EBIKE CONFIG_LV_USE_DEMO_EBIKE
    #else
        #define LV_USE_DEMO_EBIKE			0
    #endif
#endif
#if LV_USE_DEMO_EBIKE
	#ifndef LV_DEMO_EBIKE_PORTRAIT
	    #ifdef CONFIG_LV_DEMO_EBIKE_PORTRAIT
	        #define LV_DEMO_EBIKE_PORTRAIT CONFIG_LV_DEMO_EBIKE_PORTRAIT
	    #else
	        #define LV_DEMO_EBIKE_PORTRAIT  0    /*0: for 480x270..480x320, 1: for 480x800..720x1280*/
	    #endif
	#endif
#endif

/** High-resolution demo */
#ifndef LV_USE_DEMO_HIGH_RES
    #ifdef CONFIG_LV_USE_DEMO_HIGH_RES
        #define LV_USE_DEMO_HIGH_RES CONFIG_LV_USE_DEMO_HIGH_RES
    #else
        #define LV_USE_DEMO_HIGH_RES        0
    #endif
#endif



/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

/*Fix inconsistent name*/
#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef LV_KCONFIG_PRESENT

/* Set some defines if a dependency is disabled. */
#if LV_USE_LOG == 0
    #define LV_LOG_LEVEL            LV_LOG_LEVEL_NONE
    #define LV_LOG_TRACE_MEM        0
    #define LV_LOG_TRACE_TIMER      0
    #define LV_LOG_TRACE_INDEV      0
    #define LV_LOG_TRACE_DISP_REFR  0
    #define LV_LOG_TRACE_EVENT      0
    #define LV_LOG_TRACE_OBJ_CREATE 0
    #define LV_LOG_TRACE_LAYOUT     0
    #define LV_LOG_TRACE_ANIM       0
#endif  /*LV_USE_LOG*/

#if LV_USE_SYSMON == 0
    #define LV_USE_PERF_MONITOR 0
    #define LV_USE_MEM_MONITOR 0
#endif /*LV_USE_SYSMON*/

#ifndef LV_USE_LZ4
    #define LV_USE_LZ4  (LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL)
#endif

#ifndef LV_USE_THORVG
    #define LV_USE_THORVG  (LV_USE_THORVG_INTERNAL || LV_USE_THORVG_EXTERNAL)
#endif

#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #warning "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."
    #endif

    #if defined(LV_DRAW_THREAD_STACKSIZE) && !defined(LV_DRAW_THREAD_STACK_SIZE)
        #warning "LV_DRAW_THREAD_STACKSIZE was renamed to LV_DRAW_THREAD_STACK_SIZE. Please update lv_conf.h or run menuconfig again."
        #define LV_DRAW_THREAD_STACK_SIZE LV_DRAW_THREAD_STACKSIZE
    #endif
#endif

/*Allow only upper case letters and '/'  ('/' is a special case for backward compatibility)*/
#define LV_FS_IS_VALID_LETTER(l) ((l) == '/' || ((l) >= 'A' && (l) <= 'Z'))

/* If running without lv_conf.h, add typedefs with default value. */
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*defined(LV_CONF_SKIP)*/

#endif  /*LV_CONF_INTERNAL_H*/

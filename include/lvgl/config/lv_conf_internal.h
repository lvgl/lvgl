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

#define LV_DRAW_SW_ASM_NONE             0
#define LV_DRAW_SW_ASM_NEON             1
#define LV_DRAW_SW_ASM_HELIUM           2
#define LV_DRAW_SW_ASM_RISCV_V          3
#define LV_DRAW_SW_ASM_CUSTOM           255

#define LV_NEMA_LIB_NONE            0
#define LV_NEMA_LIB_M33_REVC        1
#define LV_NEMA_LIB_M33_NEMAPVG     2
#define LV_NEMA_LIB_M55             3
#define LV_NEMA_LIB_M7              4

#define LV_NEMA_HAL_CUSTOM          0
#define LV_NEMA_HAL_STM32           1

#define LV_NANOVG_BACKEND_GL2       1
#define LV_NANOVG_BACKEND_GL3       2
#define LV_NANOVG_BACKEND_GLES2     3
#define LV_NANOVG_BACKEND_GLES3     4

#define LV_CHECK_ARG_LOG_MODE_NONE    0
#define LV_CHECK_ARG_LOG_MODE_MINIMAL 1
#define LV_CHECK_ARG_LOG_MODE_VERBOSE 2

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
        #include "../../../../lv_conf.h"                /* Else assume lv_conf.h is next to the lvgl folder. */
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
#if 0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*============================================================================
 * MEMORY AND STANDARD LIBRARY
 *============================================================================*/

/** Malloc functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL's built in implementation
 *  - LV_STDLIB_CLIB: Standard C functions malloc/realloc/free
 *  - LV_STDLIB_MICROPYTHON: MicroPython functions malloc/realloc/free
 *  - LV_STDLIB_RTTHREAD: RTThread functions malloc/realloc/free
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#ifndef LV_USE_STDLIB_MALLOC
    #ifdef CONFIG_LV_USE_STDLIB_MALLOC
        #define LV_USE_STDLIB_MALLOC CONFIG_LV_USE_STDLIB_MALLOC
    #else
        #define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN
    #endif
#endif

/** String functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL's built in implementation
 *  - LV_STDLIB_CLIB: Standard C functions memcpy/memset/strlen/strcpy
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#ifndef LV_USE_STDLIB_STRING
    #ifdef CONFIG_LV_USE_STDLIB_STRING
        #define LV_USE_STDLIB_STRING CONFIG_LV_USE_STDLIB_STRING
    #else
        #define LV_USE_STDLIB_STRING LV_STDLIB_BUILTIN
    #endif
#endif

/** Sprintf functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL's built in implementation
 *  - LV_STDLIB_CLIB: Standard C functions vsnprintf
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#ifndef LV_USE_STDLIB_SPRINTF
    #ifdef CONFIG_LV_USE_STDLIB_SPRINTF
        #define LV_USE_STDLIB_SPRINTF CONFIG_LV_USE_STDLIB_SPRINTF
    #else
        #define LV_USE_STDLIB_SPRINTF LV_STDLIB_BUILTIN
    #endif
#endif

#if LV_USE_BUILTIN_MALLOC
/** Size of the memory used by `lv_malloc()` in bytes (Needs to be at least 2kB (2048)) */
#ifndef LV_MEM_SIZE
    #ifdef CONFIG_LV_MEM_SIZE
        #define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
    #else
        #define LV_MEM_SIZE 65536
    #endif
#endif

/** Address for the memory pool instead of allocating it as a normal array. 0: unused */
#ifndef LV_MEM_ADR
    #ifdef CONFIG_LV_MEM_ADR
        #define LV_MEM_ADR CONFIG_LV_MEM_ADR
    #else
        #define LV_MEM_ADR 0x0
    #endif
#endif

#endif /*LV_USE_BUILTIN_MALLOC*/

/** Header for integer types (stdint) */
#ifndef LV_STDINT_INCLUDE
    #ifdef CONFIG_LV_STDINT_INCLUDE
        #define LV_STDINT_INCLUDE CONFIG_LV_STDINT_INCLUDE
    #else
        #define LV_STDINT_INCLUDE "stdint.h"
    #endif
#endif

/** Header for standard definitions (stddef) */
#ifndef LV_STDDEF_INCLUDE
    #ifdef CONFIG_LV_STDDEF_INCLUDE
        #define LV_STDDEF_INCLUDE CONFIG_LV_STDDEF_INCLUDE
    #else
        #define LV_STDDEF_INCLUDE "stddef.h"
    #endif
#endif

/** Header for boolean types (stdbool) */
#ifndef LV_STDBOOL_INCLUDE
    #ifdef CONFIG_LV_STDBOOL_INCLUDE
        #define LV_STDBOOL_INCLUDE CONFIG_LV_STDBOOL_INCLUDE
    #else
        #define LV_STDBOOL_INCLUDE "stdbool.h"
    #endif
#endif

/** Header for fixed-width integer format macros (inttypes) */
#ifndef LV_INTTYPES_INCLUDE
    #ifdef CONFIG_LV_INTTYPES_INCLUDE
        #define LV_INTTYPES_INCLUDE CONFIG_LV_INTTYPES_INCLUDE
    #else
        #define LV_INTTYPES_INCLUDE "inttypes.h"
    #endif
#endif

/** Header for implementation limits (limits) */
#ifndef LV_LIMITS_INCLUDE
    #ifdef CONFIG_LV_LIMITS_INCLUDE
        #define LV_LIMITS_INCLUDE CONFIG_LV_LIMITS_INCLUDE
    #else
        #define LV_LIMITS_INCLUDE "limits.h"
    #endif
#endif

/** Header for variadic argument handling (stdarg) */
#ifndef LV_STDARG_INCLUDE
    #ifdef CONFIG_LV_STDARG_INCLUDE
        #define LV_STDARG_INCLUDE CONFIG_LV_STDARG_INCLUDE
    #else
        #define LV_STDARG_INCLUDE "stdarg.h"
    #endif
#endif



/*============================================================================
 * OPERATING SYSTEM (OS)
 *============================================================================*/

/** Default operating system to use
 *  Possible values:
 *  - LV_OS_NONE
 *  - LV_OS_PTHREAD
 *  - LV_OS_FREERTOS
 *  - LV_OS_CMSIS_RTOS2
 *  - LV_OS_RTTHREAD
 *  - LV_OS_WINDOWS
 *  - LV_OS_MQX
 *  - LV_OS_SDL2
 *  - LV_OS_CUSTOM
 */
#ifndef LV_USE_OS
    #ifdef CONFIG_LV_USE_OS
        #define LV_USE_OS CONFIG_LV_USE_OS
    #else
        #define LV_USE_OS LV_OS_NONE
    #endif
#endif

#if LV_USE_OS == LV_OS_CUSTOM
/** Custom OS include header */
#ifndef LV_OS_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_OS_CUSTOM_INCLUDE
        #define LV_OS_CUSTOM_INCLUDE CONFIG_LV_OS_CUSTOM_INCLUDE
    #else
        #define LV_OS_CUSTOM_INCLUDE ""
    #endif
#endif

#endif /*LV_USE_OS == LV_OS_CUSTOM*/

#if LV_USE_OS == LV_OS_FREERTOS
/** Unblocking an RTOS task with a direct notification is 45% faster and uses less RAM
 *  than unblocking a task using an intermediary object such as a binary semaphore.
 *  RTOS task notifications can only be used when there is only one task that can be the recipient of the event.
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

#endif /*LV_USE_OS == LV_OS_FREERTOS*/

#if LV_USE_OS == LV_OS_FREERTOS
/** Enable this to provide a custom implementation of lv_os_get_idle_percent.
 *  This is useful for multi-core systems where the default
 *  FreeRTOS implementation might not sufficiently track idle time across all cores.
 */
#ifndef LV_OS_IDLE_PERCENT_CUSTOM
    #ifdef CONFIG_LV_OS_IDLE_PERCENT_CUSTOM
        #define LV_OS_IDLE_PERCENT_CUSTOM CONFIG_LV_OS_IDLE_PERCENT_CUSTOM
    #else
        #define LV_OS_IDLE_PERCENT_CUSTOM 0
    #endif
#endif

#endif /*LV_USE_OS == LV_OS_FREERTOS*/


/*============================================================================
 * RENDERING CONFIGURATION
 *============================================================================*/

/** Color depth: 1 (I1), 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#ifndef LV_COLOR_DEPTH
    #ifdef CONFIG_LV_COLOR_DEPTH
        #define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
    #else
        #define LV_COLOR_DEPTH 16
    #endif
#endif

/** 0: no adjustment, get the integer part of the result (round down)
 *  64: round up from x.75
 *  128: round up from half
 *  192: round up from x.25
 *  254: round up
 */
#ifndef LV_COLOR_MIX_ROUND_OFS
    #ifdef CONFIG_LV_COLOR_MIX_ROUND_OFS
        #define LV_COLOR_MIX_ROUND_OFS CONFIG_LV_COLOR_MIX_ROUND_OFS
    #else
        #define LV_COLOR_MIX_ROUND_OFS 128
    #endif
#endif

/** Default display refresh, input device read and animation step period. */
#ifndef LV_DEF_REFR_PERIOD
    #ifdef CONFIG_LV_DEF_REFR_PERIOD
        #define LV_DEF_REFR_PERIOD CONFIG_LV_DEF_REFR_PERIOD
    #else
        #define LV_DEF_REFR_PERIOD 33
    #endif
#endif

/** Used to initialize default sizes such as widgets sizes and style paddings.
 *  (Not so important, you can adjust it to modify default sizes and spaces)
 */
#ifndef LV_DPI_DEF
    #ifdef CONFIG_LV_DPI_DEF
        #define LV_DPI_DEF CONFIG_LV_DPI_DEF
    #else
        #define LV_DPI_DEF 130
    #endif
#endif

/** Align the stride of all layers and images to this bytes. */
#ifndef LV_DRAW_BUF_STRIDE_ALIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
            #define LV_DRAW_BUF_STRIDE_ALIGN CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
        #else
            #define LV_DRAW_BUF_STRIDE_ALIGN 0
        #endif
    #else
        #define LV_DRAW_BUF_STRIDE_ALIGN 1
    #endif
#endif

/** Align the start address of draw_buf addresses to this bytes. */
#ifndef LV_DRAW_BUF_ALIGN
    #ifdef CONFIG_LV_DRAW_BUF_ALIGN
        #define LV_DRAW_BUF_ALIGN CONFIG_LV_DRAW_BUF_ALIGN
    #else
        #define LV_DRAW_BUF_ALIGN 4
    #endif
#endif

/** Enable matrix support */
#ifndef LV_USE_MATRIX
    #ifdef CONFIG_LV_USE_MATRIX
        #define LV_USE_MATRIX CONFIG_LV_USE_MATRIX
    #else
        #define LV_USE_MATRIX 0
    #endif
#endif

#if LV_USE_MATRIX
/** Requirements: The rendering engine needs to support 3x3 matrix transformations. */
#ifndef LV_DRAW_TRANSFORM_USE_MATRIX
    #ifdef CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
        #define LV_DRAW_TRANSFORM_USE_MATRIX CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
    #else
        #define LV_DRAW_TRANSFORM_USE_MATRIX 0
    #endif
#endif

#endif /*LV_USE_MATRIX*/

/** If a widget has `style_opa < 255` (not `bg_opa`, `text_opa` etc) or not NORMAL blend mode
 *  it is buffered into a "simple" layer before rendering. The widget can be buffered in smaller chunks.
 *  "Transformed layers" (if `transform_angle/zoom` are set) use larger buffers and can't be drawn in chunks.
 */
#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #ifdef CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #else
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE 24576
    #endif
#endif

/** Limit the max allocated memory for simple and transformed layers.
 *  It should be at least `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` sized but if transformed layers are also used
 *  it should be enough to store the largest widget too (width x height x 4 area).
 *  Set it to 0 to have no limit.
 */
#ifndef LV_DRAW_LAYER_MAX_MEMORY
    #ifdef CONFIG_LV_DRAW_LAYER_MAX_MEMORY
        #define LV_DRAW_LAYER_MAX_MEMORY CONFIG_LV_DRAW_LAYER_MAX_MEMORY
    #else
        #define LV_DRAW_LAYER_MAX_MEMORY 0
    #endif
#endif

#if LV_USE_OS != LV_OS_NONE
/** If FreeType or ThorVG is enabled, it is recommended to set it to 32KB or more. */
#ifndef LV_DRAW_THREAD_STACK_SIZE
    #ifdef CONFIG_LV_DRAW_THREAD_STACK_SIZE
        #define LV_DRAW_THREAD_STACK_SIZE CONFIG_LV_DRAW_THREAD_STACK_SIZE
    #else
        #define LV_DRAW_THREAD_STACK_SIZE 8192
    #endif
#endif

#endif /*LV_USE_OS != LV_OS_NONE*/

#if LV_USE_OS != LV_OS_NONE
/** Thread priority controls the relative importance of the drawing threads.
 *  Values correspond to lv_thread_prio_t enum in lv_os.h:
 *  0: LV_THREAD_PRIO_LOWEST
 *  1: LV_THREAD_PRIO_LOW
 *  2: LV_THREAD_PRIO_MID (default)
 *  3: LV_THREAD_PRIO_HIGH
 *  4: LV_THREAD_PRIO_HIGHEST
 *
 *  Higher priority can improve rendering performance but might cause
 *  starvation of lower priority tasks.
 */
#ifndef LV_DRAW_THREAD_PRIO
    #ifdef CONFIG_LV_DRAW_THREAD_PRIO
        #define LV_DRAW_THREAD_PRIO CONFIG_LV_DRAW_THREAD_PRIO
    #else
        #define LV_DRAW_THREAD_PRIO 3
    #endif
#endif

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable drawing support vector graphic APIs. */
#ifndef LV_USE_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_VECTOR_GRAPHIC
        #define LV_USE_VECTOR_GRAPHIC CONFIG_LV_USE_VECTOR_GRAPHIC
    #else
        #define LV_USE_VECTOR_GRAPHIC 0
    #endif
#endif

/** Enable API to take snapshot for object */
#ifndef LV_USE_SNAPSHOT
    #ifdef CONFIG_LV_USE_SNAPSHOT
        #define LV_USE_SNAPSHOT CONFIG_LV_USE_SNAPSHOT
    #else
        #define LV_USE_SNAPSHOT 0
    #endif
#endif

/** Required to draw anything on the screen. */
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

#if LV_USE_DRAW_SW
/** Enable support for RGB565 color format */
#ifndef LV_DRAW_SW_SUPPORT_RGB565
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565
            #define LV_DRAW_SW_SUPPORT_RGB565 CONFIG_LV_DRAW_SW_SUPPORT_RGB565
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_RGB565 1
    #endif
#endif

/** Enable support for RGB565 swapped color format */
#ifndef LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
            #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 1
    #endif
#endif

/** Enable support for RGB565A8 color format */
#ifndef LV_DRAW_SW_SUPPORT_RGB565A8
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
            #define LV_DRAW_SW_SUPPORT_RGB565A8 CONFIG_LV_DRAW_SW_SUPPORT_RGB565A8
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565A8 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_RGB565A8 1
    #endif
#endif

/** Enable support for RGB888 color format */
#ifndef LV_DRAW_SW_SUPPORT_RGB888
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB888
            #define LV_DRAW_SW_SUPPORT_RGB888 CONFIG_LV_DRAW_SW_SUPPORT_RGB888
        #else
            #define LV_DRAW_SW_SUPPORT_RGB888 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_RGB888 1
    #endif
#endif

/** Enable support for XRGB8888 color format */
#ifndef LV_DRAW_SW_SUPPORT_XRGB8888
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
            #define LV_DRAW_SW_SUPPORT_XRGB8888 CONFIG_LV_DRAW_SW_SUPPORT_XRGB8888
        #else
            #define LV_DRAW_SW_SUPPORT_XRGB8888 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_XRGB8888 1
    #endif
#endif

/** Enable support for ARGB8888 color format */
#ifndef LV_DRAW_SW_SUPPORT_ARGB8888
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
            #define LV_DRAW_SW_SUPPORT_ARGB8888 CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_ARGB8888 1
    #endif
#endif

/** Enable support for ARGB8888 premultiplied color format */
#ifndef LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
            #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1
    #endif
#endif

/** Enable support for L8 color format */
#ifndef LV_DRAW_SW_SUPPORT_L8
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_L8
            #define LV_DRAW_SW_SUPPORT_L8 CONFIG_LV_DRAW_SW_SUPPORT_L8
        #else
            #define LV_DRAW_SW_SUPPORT_L8 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_L8 1
    #endif
#endif

/** Enable support for AL88 color format */
#ifndef LV_DRAW_SW_SUPPORT_AL88
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_AL88
            #define LV_DRAW_SW_SUPPORT_AL88 CONFIG_LV_DRAW_SW_SUPPORT_AL88
        #else
            #define LV_DRAW_SW_SUPPORT_AL88 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_AL88 1
    #endif
#endif

/** Enable support for A8 color format */
#ifndef LV_DRAW_SW_SUPPORT_A8
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_A8
            #define LV_DRAW_SW_SUPPORT_A8 CONFIG_LV_DRAW_SW_SUPPORT_A8
        #else
            #define LV_DRAW_SW_SUPPORT_A8 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_A8 1
    #endif
#endif

/** Enable support for I1 color format */
#ifndef LV_DRAW_SW_SUPPORT_I1
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_I1
            #define LV_DRAW_SW_SUPPORT_I1 CONFIG_LV_DRAW_SW_SUPPORT_I1
        #else
            #define LV_DRAW_SW_SUPPORT_I1 0
        #endif
    #else
        #define LV_DRAW_SW_SUPPORT_I1 1
    #endif
#endif

#if LV_DRAW_SW_SUPPORT_I1
/** Luminance threshold for a pixel to be active */
#ifndef LV_DRAW_SW_I1_LUM_THRESHOLD
    #ifdef CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
        #define LV_DRAW_SW_I1_LUM_THRESHOLD CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
    #else
        #define LV_DRAW_SW_I1_LUM_THRESHOLD 127
    #endif
#endif

#endif /*LV_DRAW_SW_SUPPORT_I1*/

#if LV_USE_OS != LV_OS_NONE
/** Number of threads used to render a frame in parallel */
#ifndef LV_DRAW_SW_DRAW_UNIT_CNT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
            #define LV_DRAW_SW_DRAW_UNIT_CNT CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
        #else
            #define LV_DRAW_SW_DRAW_UNIT_CNT 0
        #endif
    #else
        #define LV_DRAW_SW_DRAW_UNIT_CNT 1
    #endif
#endif

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Must deploy arm-2d library to your project and add include PATH for "arm_2d.h". */
#ifndef LV_USE_DRAW_ARM2D_SYNC
    #ifdef CONFIG_LV_USE_DRAW_ARM2D_SYNC
        #define LV_USE_DRAW_ARM2D_SYNC CONFIG_LV_USE_DRAW_ARM2D_SYNC
    #else
        #define LV_USE_DRAW_ARM2D_SYNC 0
    #endif
#endif

/** Disabling this allows arm2d to work on its own (for testing only) */
#ifndef LV_USE_NATIVE_HELIUM_ASM
    #ifdef CONFIG_LV_USE_NATIVE_HELIUM_ASM
        #define LV_USE_NATIVE_HELIUM_ASM CONFIG_LV_USE_NATIVE_HELIUM_ASM
    #else
        #define LV_USE_NATIVE_HELIUM_ASM 0
    #endif
#endif

/** 0: use a simple renderer capable of drawing only simple rectangles with gradient, images, texts, and straight lines only,
 *  1: use a complex renderer capable of drawing rounded corners, shadow, skew lines, and arcs too.
 */
#ifndef LV_DRAW_SW_COMPLEX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_COMPLEX
            #define LV_DRAW_SW_COMPLEX CONFIG_LV_DRAW_SW_COMPLEX
        #else
            #define LV_DRAW_SW_COMPLEX 0
        #endif
    #else
        #define LV_DRAW_SW_COMPLEX 1
    #endif
#endif

/** Increase this to allow more stops.
 *  This adds (sizeof(lv_color_t) + 1) bytes per additional stop
 */
#ifndef LV_GRADIENT_MAX_STOPS
    #ifdef CONFIG_LV_GRADIENT_MAX_STOPS
        #define LV_GRADIENT_MAX_STOPS CONFIG_LV_GRADIENT_MAX_STOPS
    #else
        #define LV_GRADIENT_MAX_STOPS 2
    #endif
#endif

/** 0: do not enable complex gradients
 *  1: enable complex gradients (linear at an angle, radial or conical)
 */
#ifndef LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    #ifdef CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    #else
        #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 0
    #endif
#endif

#if LV_DRAW_SW_COMPLEX
/** LV_DRAW_SW_SHADOW_CACHE_SIZE is the max shadow size to buffer, where
 *  shadow size is `shadow_width + radius`.
 *  Caching has LV_DRAW_SW_SHADOW_CACHE_SIZE^2 RAM cost.
 */
#ifndef LV_DRAW_SW_SHADOW_CACHE_SIZE
    #ifdef CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
    #else
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0
    #endif
#endif

/** The circumference of 1/4 circle are saved for anti-aliasing
 *  radius * 4 bytes are used per circle (the most often used
 *  radiuses are saved).
 *  Set to 0 to disable caching.
 */
#ifndef LV_DRAW_SW_CIRCLE_CACHE_SIZE
    #ifdef CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
    #else
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
    #endif
#endif

#endif /*LV_DRAW_SW_COMPLEX*/

/** ASM mode to be used
 *  Possible values:
 *  - LV_DRAW_SW_ASM_NONE
 *  - LV_DRAW_SW_ASM_NEON
 *  - LV_DRAW_SW_ASM_HELIUM
 *  - LV_DRAW_SW_ASM_RISCV_V: RISC-V Vector
 *  - LV_DRAW_SW_ASM_CUSTOM
 */
#ifndef LV_USE_DRAW_SW_ASM
    #ifdef CONFIG_LV_USE_DRAW_SW_ASM
        #define LV_USE_DRAW_SW_ASM CONFIG_LV_USE_DRAW_SW_ASM
    #else
        #define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE
    #endif
#endif

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
/** Set the custom asm include file */
#ifndef LV_DRAW_SW_ASM_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
        #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
    #else
        #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
    #endif
#endif

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM*/
#endif /*LV_USE_DRAW_SW*/

/** Enable ThorVG (vector graphics library) from the src/libs folder.
 *  Requires LV_USE_VECTOR_GRAPHIC
 */
#ifndef LV_USE_THORVG_INTERNAL
    #ifdef CONFIG_LV_USE_THORVG_INTERNAL
        #define LV_USE_THORVG_INTERNAL CONFIG_LV_USE_THORVG_INTERNAL
    #else
        #define LV_USE_THORVG_INTERNAL 0
    #endif
#endif

/** Enable ThorVG by assuming that its installed and linked to the project
 *  Requires LV_USE_VECTOR_GRAPHIC
 */
#ifndef LV_USE_THORVG_EXTERNAL
    #ifdef CONFIG_LV_USE_THORVG_EXTERNAL
        #define LV_USE_THORVG_EXTERNAL CONFIG_LV_USE_THORVG_EXTERNAL
    #else
        #define LV_USE_THORVG_EXTERNAL 0
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

/** Enable VG-Lite assert */
#ifndef LV_VG_LITE_USE_ASSERT
    #ifdef CONFIG_LV_VG_LITE_USE_ASSERT
        #define LV_VG_LITE_USE_ASSERT CONFIG_LV_VG_LITE_USE_ASSERT
    #else
        #define LV_VG_LITE_USE_ASSERT 0
    #endif
#endif

/** GPU will try to batch these many draw tasks */
#ifndef LV_VG_LITE_FLUSH_MAX_COUNT
    #ifdef CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
        #define LV_VG_LITE_FLUSH_MAX_COUNT CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
    #else
        #define LV_VG_LITE_FLUSH_MAX_COUNT 8
    #endif
#endif

/** which usually improves performance,
 *  but does not guarantee the same rendering quality as the software.
 */
#ifndef LV_VG_LITE_USE_BOX_SHADOW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_VG_LITE_USE_BOX_SHADOW
            #define LV_VG_LITE_USE_BOX_SHADOW CONFIG_LV_VG_LITE_USE_BOX_SHADOW
        #else
            #define LV_VG_LITE_USE_BOX_SHADOW 0
        #endif
    #else
        #define LV_VG_LITE_USE_BOX_SHADOW 1
    #endif
#endif

/** The memory usage of a single gradient:
 *          linear: 4K bytes.
 *          radial: radius * 4K bytes.
 */
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

/** VG-Lite unaligned bitmap font maximum cache number. */
#ifndef LV_VG_LITE_BITMAP_FONT_CACHE_CNT
    #ifdef CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
        #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
    #else
        #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256
    #endif
#endif

/** Remove VLC_OP_CLOSE path instruction (Workaround for NXP) */
#ifndef LV_VG_LITE_DISABLE_VLC_OP_CLOSE
    #ifdef CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
        #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
    #else
        #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0
    #endif
#endif

/** Disable linear gradient extension for some older versions of drivers. */
#ifndef LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
    #ifdef CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
        #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
    #else
        #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0
    #endif
#endif

/** Disable blit rectangular offset to resolve certain hardware errors. */
#ifndef LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
    #ifdef CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
        #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
    #else
        #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0
    #endif
#endif

/** Maximum path dump print length (in points) */
#ifndef LV_VG_LITE_PATH_DUMP_MAX_LEN
    #ifdef CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
        #define LV_VG_LITE_PATH_DUMP_MAX_LEN CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
    #else
        #define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000
    #endif
#endif

/** Use LVGL's built-in vg_lite driver */
#ifndef LV_USE_VG_LITE_DRIVER
    #ifdef CONFIG_LV_USE_VG_LITE_DRIVER
        #define LV_USE_VG_LITE_DRIVER CONFIG_LV_USE_VG_LITE_DRIVER
    #else
        #define LV_USE_VG_LITE_DRIVER 0
    #endif
#endif

#if LV_USE_VG_LITE_DRIVER
/** VG-Lite GPU series */
#ifndef LV_VG_LITE_HAL_GPU_SERIES
    #ifdef CONFIG_LV_VG_LITE_HAL_GPU_SERIES
        #define LV_VG_LITE_HAL_GPU_SERIES CONFIG_LV_VG_LITE_HAL_GPU_SERIES
    #else
        #define LV_VG_LITE_HAL_GPU_SERIES "gc255"
    #endif
#endif

/** VG-Lite GPU revision */
#ifndef LV_VG_LITE_HAL_GPU_REVISION
    #ifdef CONFIG_LV_VG_LITE_HAL_GPU_REVISION
        #define LV_VG_LITE_HAL_GPU_REVISION CONFIG_LV_VG_LITE_HAL_GPU_REVISION
    #else
        #define LV_VG_LITE_HAL_GPU_REVISION 0x40
    #endif
#endif

/** VG-Lite GPU base address */
#ifndef LV_VG_LITE_HAL_GPU_BASE_ADDRESS
    #ifdef CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
        #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
    #else
        #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000
    #endif
#endif

#endif /*LV_USE_VG_LITE_DRIVER*/

/** Use thorvg to simulate VG-Lite hardware behavior, it's useful
 *  for debugging and testing on PC simulator. Enable LV_USE_THORVG,
 *  Either internal ThorVG or external ThorVG library is required.
 */
#ifndef LV_USE_VG_LITE_THORVG
    #ifdef CONFIG_LV_USE_VG_LITE_THORVG
        #define LV_USE_VG_LITE_THORVG CONFIG_LV_USE_VG_LITE_THORVG
    #else
        #define LV_USE_VG_LITE_THORVG 0
    #endif
#endif

#if LV_USE_VG_LITE_THORVG
/** Enable LVGL blend mode support */
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

/** Enable linear gradient extension support */
#ifndef LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
    #ifdef CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
        #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
    #else
        #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0
    #endif
#endif

/** Enable 16 pixels alignment */
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

#endif /*LV_USE_VG_LITE_THORVG*/
#endif /*LV_USE_DRAW_VG_LITE*/

/** Use TSi's aka (Think Silicon) NemaGFX */
#ifndef LV_USE_NEMA_GFX
    #ifdef CONFIG_LV_USE_NEMA_GFX
        #define LV_USE_NEMA_GFX CONFIG_LV_USE_NEMA_GFX
    #else
        #define LV_USE_NEMA_GFX 0
    #endif
#endif

#if LV_USE_NEMA_GFX
/** Cache handling of NemaGFX */
#ifndef LV_NEMA_USE_CACHE
    #ifdef CONFIG_LV_NEMA_USE_CACHE
        #define LV_NEMA_USE_CACHE CONFIG_LV_NEMA_USE_CACHE
    #else
        #define LV_NEMA_USE_CACHE 0
    #endif
#endif

#if LV_NEMA_USE_CACHE
/** NemaGFX Cache HAL include */
#ifndef LV_NEMA_CACHE_HAL_INCLUDE
    #ifdef CONFIG_LV_NEMA_CACHE_HAL_INCLUDE
        #define LV_NEMA_CACHE_HAL_INCLUDE CONFIG_LV_NEMA_CACHE_HAL_INCLUDE
    #else
        #define LV_NEMA_CACHE_HAL_INCLUDE "stm32u5xx_hal.h"
    #endif
#endif

#endif /*LV_NEMA_USE_CACHE*/

/** NemaGFX static library
 *  Possible values:
 *  - LV_NEMA_LIB_NONE
 *  - LV_NEMA_LIB_M33_REVC
 *  - LV_NEMA_LIB_M33_NEMAPVG
 *  - LV_NEMA_LIB_M55
 *  - LV_NEMA_LIB_M7
 */
#ifndef LV_USE_NEMA_LIB
    #ifdef CONFIG_LV_USE_NEMA_LIB
        #define LV_USE_NEMA_LIB CONFIG_LV_USE_NEMA_LIB
    #else
        #define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE
    #endif
#endif

/** NemaGFX HAL
 *  Possible values:
 *  - LV_NEMA_HAL_CUSTOM
 *  - LV_NEMA_HAL_STM32
 */
#ifndef LV_USE_NEMA_HAL
    #ifdef CONFIG_LV_USE_NEMA_HAL
        #define LV_USE_NEMA_HAL CONFIG_LV_USE_NEMA_HAL
    #else
        #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
    #endif
#endif

#if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
/** NemaGFX STM32 HAL include */
#ifndef LV_NEMA_STM32_HAL_INCLUDE
    #ifdef CONFIG_LV_NEMA_STM32_HAL_INCLUDE
        #define LV_NEMA_STM32_HAL_INCLUDE CONFIG_LV_NEMA_STM32_HAL_INCLUDE
    #else
        #define LV_NEMA_STM32_HAL_INCLUDE "stm32u5xx_hal.h"
    #endif
#endif

#endif /*LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32*/

#if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
/** Optional header to override NemaGFX/STM32 HAL-specific macros.
 *
 *  LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
 *    Attribute applied to the NemaGFX memory pool on STM32 targets.
 *    Used to place the pool in a specific RAM region accessible by
 *    the NemaGFX DMA.
 *    e.g. __attribute__((section(".nema_pool")))
 */
#ifndef LV_NEMA_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_NEMA_CUSTOM_INCLUDE
        #define LV_NEMA_CUSTOM_INCLUDE CONFIG_LV_NEMA_CUSTOM_INCLUDE
    #else
        #define LV_NEMA_CUSTOM_INCLUDE ""
    #endif
#endif

#endif /*LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32*/

/** Enable NemaVG operations */
#ifndef LV_USE_NEMA_VG
    #ifdef CONFIG_LV_USE_NEMA_VG
        #define LV_USE_NEMA_VG CONFIG_LV_USE_NEMA_VG
    #else
        #define LV_USE_NEMA_VG 0
    #endif
#endif

/** NemaVG max horizontal resolution */
#ifndef LV_NEMA_GFX_MAX_RESX
    #ifdef CONFIG_LV_NEMA_GFX_MAX_RESX
        #define LV_NEMA_GFX_MAX_RESX CONFIG_LV_NEMA_GFX_MAX_RESX
    #else
        #define LV_NEMA_GFX_MAX_RESX 800
    #endif
#endif

/** NemaVG max vertical resolution */
#ifndef LV_NEMA_GFX_MAX_RESY
    #ifdef CONFIG_LV_NEMA_GFX_MAX_RESY
        #define LV_NEMA_GFX_MAX_RESY CONFIG_LV_NEMA_GFX_MAX_RESY
    #else
        #define LV_NEMA_GFX_MAX_RESY 600
    #endif
#endif

#endif /*LV_USE_NEMA_GFX*/

/** Use PXP for drawing */
#ifndef LV_USE_DRAW_PXP
    #ifdef CONFIG_LV_USE_DRAW_PXP
        #define LV_USE_DRAW_PXP CONFIG_LV_USE_DRAW_PXP
    #else
        #define LV_USE_DRAW_PXP 0
    #endif
#endif

#if LV_USE_DRAW_PXP
/** Use PXP to rotate the display */
#ifndef LV_USE_ROTATE_PXP
    #ifdef CONFIG_LV_USE_ROTATE_PXP
        #define LV_USE_ROTATE_PXP CONFIG_LV_USE_ROTATE_PXP
    #else
        #define LV_USE_ROTATE_PXP 0
    #endif
#endif

#if LV_USE_OS != LV_OS_NONE
/** Use additional draw thread for PXP processing */
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

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable PXP asserts */
#ifndef LV_USE_PXP_ASSERT
    #ifdef CONFIG_LV_USE_PXP_ASSERT
        #define LV_USE_PXP_ASSERT CONFIG_LV_USE_PXP_ASSERT
    #else
        #define LV_USE_PXP_ASSERT 0
    #endif
#endif

#endif /*LV_USE_DRAW_PXP*/

/** Use G2D for drawing. */
#ifndef LV_USE_DRAW_G2D
    #ifdef CONFIG_LV_USE_DRAW_G2D
        #define LV_USE_DRAW_G2D CONFIG_LV_USE_DRAW_G2D
    #else
        #define LV_USE_DRAW_G2D 0
    #endif
#endif

#if LV_USE_DRAW_G2D
/** Includes the frame buffers and assets. */
#ifndef LV_G2D_HASH_TABLE_SIZE
    #ifdef CONFIG_LV_G2D_HASH_TABLE_SIZE
        #define LV_G2D_HASH_TABLE_SIZE CONFIG_LV_G2D_HASH_TABLE_SIZE
    #else
        #define LV_G2D_HASH_TABLE_SIZE 50
    #endif
#endif

#if LV_USE_OS != LV_OS_NONE
/** Use additional draw thread for G2D processing */
#ifndef LV_USE_G2D_DRAW_THREAD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_G2D_DRAW_THREAD
            #define LV_USE_G2D_DRAW_THREAD CONFIG_LV_USE_G2D_DRAW_THREAD
        #else
            #define LV_USE_G2D_DRAW_THREAD 0
        #endif
    #else
        #define LV_USE_G2D_DRAW_THREAD 1
    #endif
#endif

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable G2D asserts */
#ifndef LV_USE_G2D_ASSERT
    #ifdef CONFIG_LV_USE_G2D_ASSERT
        #define LV_USE_G2D_ASSERT CONFIG_LV_USE_G2D_ASSERT
    #else
        #define LV_USE_G2D_ASSERT 0
    #endif
#endif

#endif /*LV_USE_DRAW_G2D*/

/** Draw using espressif PPA accelerator */
#ifndef LV_USE_PPA
    #ifdef CONFIG_LV_USE_PPA
        #define LV_USE_PPA CONFIG_LV_USE_PPA
    #else
        #define LV_USE_PPA 0
    #endif
#endif

#if LV_USE_PPA
/** Use Espressif's PPA accelerator for Image draw */
#ifndef LV_USE_PPA_IMG
    #ifdef CONFIG_LV_USE_PPA_IMG
        #define LV_USE_PPA_IMG CONFIG_LV_USE_PPA_IMG
    #else
        #define LV_USE_PPA_IMG 0
    #endif
#endif

/** PPA burst length size in bytes. */
#ifndef LV_PPA_BURST_LENGTH
    #ifdef CONFIG_LV_PPA_BURST_LENGTH
        #define LV_PPA_BURST_LENGTH CONFIG_LV_PPA_BURST_LENGTH
    #else
        #define LV_PPA_BURST_LENGTH 128
    #endif
#endif

#endif /*LV_USE_PPA*/

/** Use Renesas Dave2D on RA  platforms. */
#ifndef LV_USE_DRAW_DAVE2D
    #ifdef CONFIG_LV_USE_DRAW_DAVE2D
        #define LV_USE_DRAW_DAVE2D CONFIG_LV_USE_DRAW_DAVE2D
    #else
        #define LV_USE_DRAW_DAVE2D 0
    #endif
#endif

/** Uses SDL renderer API */
#ifndef LV_USE_DRAW_SDL
    #ifdef CONFIG_LV_USE_DRAW_SDL
        #define LV_USE_DRAW_SDL CONFIG_LV_USE_DRAW_SDL
    #else
        #define LV_USE_DRAW_SDL 0
    #endif
#endif

/** Accelerate blends, fills, image decoding, etc. with STM32 DMA2D. */
#ifndef LV_USE_DRAW_DMA2D
    #ifdef CONFIG_LV_USE_DRAW_DMA2D
        #define LV_USE_DRAW_DMA2D CONFIG_LV_USE_DRAW_DMA2D
    #else
        #define LV_USE_DRAW_DMA2D 0
    #endif
#endif

#if LV_USE_DRAW_DMA2D
/** the header file for LVGL to include for DMA2D */
#ifndef LV_DRAW_DMA2D_HAL_INCLUDE
    #ifdef CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
        #define LV_DRAW_DMA2D_HAL_INCLUDE CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
    #else
        #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"
    #endif
#endif

/** if enabled, the user is required to call
 *  `lv_draw_dma2d_transfer_complete_interrupt_handler`
 *  upon receiving the DMA2D global interrupt
 */
#ifndef LV_USE_DRAW_DMA2D_INTERRUPT
    #ifdef CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
        #define LV_USE_DRAW_DMA2D_INTERRUPT CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
    #else
        #define LV_USE_DRAW_DMA2D_INTERRUPT 0
    #endif
#endif

#endif /*LV_USE_DRAW_DMA2D*/

/** Use EVE FT81X GPU. */
#ifndef LV_USE_DRAW_EVE
    #ifdef CONFIG_LV_USE_DRAW_EVE
        #define LV_USE_DRAW_EVE CONFIG_LV_USE_DRAW_EVE
    #else
        #define LV_USE_DRAW_EVE 0
    #endif
#endif

#if LV_USE_DRAW_EVE
/** EVE_GEN value */
#ifndef LV_DRAW_EVE_EVE_GENERATION
    #ifdef CONFIG_LV_DRAW_EVE_EVE_GENERATION
        #define LV_DRAW_EVE_EVE_GENERATION CONFIG_LV_DRAW_EVE_EVE_GENERATION
    #else
        #define LV_DRAW_EVE_EVE_GENERATION 4
    #endif
#endif

/** Max bytes to buffer for each SPI transmission or 0 to disable buffering */
#ifndef LV_DRAW_EVE_WRITE_BUFFER_SIZE
    #ifdef CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
        #define LV_DRAW_EVE_WRITE_BUFFER_SIZE CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
    #else
        #define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048
    #endif
#endif

#endif /*LV_USE_DRAW_EVE*/

/** Draw using cached OpenGLES textures. Requires LV_USE_OPENGLES */
#ifndef LV_USE_DRAW_OPENGLES
    #ifdef CONFIG_LV_USE_DRAW_OPENGLES
        #define LV_USE_DRAW_OPENGLES CONFIG_LV_USE_DRAW_OPENGLES
    #else
        #define LV_USE_DRAW_OPENGLES 0
    #endif
#endif

#if LV_USE_DRAW_OPENGLES
/** OpenGLES texture cache count */
#ifndef LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
    #ifdef CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
        #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
    #else
        #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64
    #endif
#endif

#endif /*LV_USE_DRAW_OPENGLES*/

/** Use NanoVG Renderer
 *  - Requires LV_USE_NANOVG, LV_USE_MATRIX.
 */
#ifndef LV_USE_DRAW_NANOVG
    #ifdef CONFIG_LV_USE_DRAW_NANOVG
        #define LV_USE_DRAW_NANOVG CONFIG_LV_USE_DRAW_NANOVG
    #else
        #define LV_USE_DRAW_NANOVG 0
    #endif
#endif

#if LV_USE_DRAW_NANOVG
/** Select which OpenGL implementation to use for NanoVG rendering.
 *  Possible values:
 *  - LV_NANOVG_BACKEND_GL2: OpenGL 2.0
 *  - LV_NANOVG_BACKEND_GL3: OpenGL 3.0+
 *  - LV_NANOVG_BACKEND_GLES2: OpenGL ES 2.0
 *  - LV_NANOVG_BACKEND_GLES3: OpenGL ES 3.0+
 */
#ifndef LV_NANOVG_BACKEND
    #ifdef CONFIG_LV_NANOVG_BACKEND
        #define LV_NANOVG_BACKEND CONFIG_LV_NANOVG_BACKEND
    #else
        #define LV_NANOVG_BACKEND LV_NANOVG_BACKEND_GLES2
    #endif
#endif

/** Draw image texture cache count */
#ifndef LV_NANOVG_IMAGE_CACHE_CNT
    #ifdef CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
        #define LV_NANOVG_IMAGE_CACHE_CNT CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
    #else
        #define LV_NANOVG_IMAGE_CACHE_CNT 128
    #endif
#endif

/** Draw letter cache count */
#ifndef LV_NANOVG_LETTER_CACHE_CNT
    #ifdef CONFIG_LV_NANOVG_LETTER_CACHE_CNT
        #define LV_NANOVG_LETTER_CACHE_CNT CONFIG_LV_NANOVG_LETTER_CACHE_CNT
    #else
        #define LV_NANOVG_LETTER_CACHE_CNT 512
    #endif
#endif

#endif /*LV_USE_DRAW_NANOVG*/


/*============================================================================
 * INPUT DEVICES
 *============================================================================*/

/** Enable grid navigation */
#ifndef LV_USE_GRIDNAV
    #ifdef CONFIG_LV_USE_GRIDNAV
        #define LV_USE_GRIDNAV CONFIG_LV_USE_GRIDNAV
    #else
        #define LV_USE_GRIDNAV 0
    #endif
#endif

/** Enable the multi-touch gesture recognition feature
 *  Gesture recognition requires the use of floats
 */
#ifndef LV_USE_GESTURE_RECOGNITION
    #ifdef CONFIG_LV_USE_GESTURE_RECOGNITION
        #define LV_USE_GESTURE_RECOGNITION CONFIG_LV_USE_GESTURE_RECOGNITION
    #else
        #define LV_USE_GESTURE_RECOGNITION 0
    #endif
#endif



/*============================================================================
 * CORE
 *============================================================================*/

/** Add 2 x 32 bit variables to each lv_obj_t to speed up getting style properties */
#ifndef LV_OBJ_STYLE_CACHE
    #ifdef CONFIG_LV_OBJ_STYLE_CACHE
        #define LV_OBJ_STYLE_CACHE CONFIG_LV_OBJ_STYLE_CACHE
    #else
        #define LV_OBJ_STYLE_CACHE 0
    #endif
#endif

/** Enable support widget names */
#ifndef LV_USE_OBJ_NAME
    #ifdef CONFIG_LV_USE_OBJ_NAME
        #define LV_USE_OBJ_NAME CONFIG_LV_USE_OBJ_NAME
    #else
        #define LV_USE_OBJ_NAME 0
    #endif
#endif

/** Add `id` field to `lv_obj_t` */
#ifndef LV_USE_OBJ_ID
    #ifdef CONFIG_LV_USE_OBJ_ID
        #define LV_USE_OBJ_ID CONFIG_LV_USE_OBJ_ID
    #else
        #define LV_USE_OBJ_ID 0
    #endif
#endif

#if LV_USE_OBJ_ID
/** Automatically assign an ID when obj is created */
#ifndef LV_OBJ_ID_AUTO_ASSIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_OBJ_ID_AUTO_ASSIGN
            #define LV_OBJ_ID_AUTO_ASSIGN CONFIG_LV_OBJ_ID_AUTO_ASSIGN
        #else
            #define LV_OBJ_ID_AUTO_ASSIGN 0
        #endif
    #else
        #define LV_OBJ_ID_AUTO_ASSIGN 1
    #endif
#endif

/** Use builtin obj ID handler functions:
 *  - lv_obj_assign_id:       Called when a widget is created. Use a separate counter for each widget class as an ID.
 *  - lv_obj_id_compare:      Compare the ID to decide if it matches with a requested value.
 *  - lv_obj_stringify_id:    Return string-ified identifier, e.g. "button3".
 *  - lv_obj_free_id:         Does nothing, as there is no memory allocation for the ID.
 *  When disabled these functions needs to be implemented by the user.
 */
#ifndef LV_USE_OBJ_ID_BUILTIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_ID_BUILTIN
            #define LV_USE_OBJ_ID_BUILTIN CONFIG_LV_USE_OBJ_ID_BUILTIN
        #else
            #define LV_USE_OBJ_ID_BUILTIN 0
        #endif
    #else
        #define LV_USE_OBJ_ID_BUILTIN 1
    #endif
#endif

#endif /*LV_USE_OBJ_ID*/

/** Use obj property set/get API. */
#ifndef LV_USE_OBJ_PROPERTY
    #ifdef CONFIG_LV_USE_OBJ_PROPERTY
        #define LV_USE_OBJ_PROPERTY CONFIG_LV_USE_OBJ_PROPERTY
    #else
        #define LV_USE_OBJ_PROPERTY 0
    #endif
#endif

#if LV_USE_OBJ_PROPERTY
/** Add a name table to every widget class, so the property can be accessed by name.
 *  Note, the const table will increase flash usage.
 */
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

#endif /*LV_USE_OBJ_PROPERTY*/

/** Enable this option to activate external data and destructor functionality,
 *  which assists in resource cleanup when objects are freed by either LVGL core
 *  or applications. Currently supported features include:
 *          - event
 *          - object
 *          - observer
 *          - anim
 *          - timer
 *          - group
 *          - display
 *          - indev (input device)
 *          - theme
 */
#ifndef LV_USE_EXT_DATA
    #ifdef CONFIG_LV_USE_EXT_DATA
        #define LV_USE_EXT_DATA CONFIG_LV_USE_EXT_DATA
    #else
        #define LV_USE_EXT_DATA 0
    #endif
#endif

/** Use `float` as `lv_value_precise_t` */
#ifndef LV_USE_FLOAT
    #ifdef CONFIG_LV_USE_FLOAT
        #define LV_USE_FLOAT CONFIG_LV_USE_FLOAT
    #else
        #define LV_USE_FLOAT 0
    #endif
#endif

/** Enable an observer pattern implementation */
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

/** Enable text translation support */
#ifndef LV_USE_TRANSLATION
    #ifdef CONFIG_LV_USE_TRANSLATION
        #define LV_USE_TRANSLATION CONFIG_LV_USE_TRANSLATION
    #else
        #define LV_USE_TRANSLATION 0
    #endif
#endif

/** Enable color filter style */
#ifndef LV_USE_COLOR_FILTER
    #ifdef CONFIG_LV_USE_COLOR_FILTER
        #define LV_USE_COLOR_FILTER CONFIG_LV_USE_COLOR_FILTER
    #else
        #define LV_USE_COLOR_FILTER 0
    #endif
#endif



/*============================================================================
 * LOGGING
 *============================================================================*/

/** Enable log module */
#ifndef LV_USE_LOG
    #ifdef CONFIG_LV_USE_LOG
        #define LV_USE_LOG CONFIG_LV_USE_LOG
    #else
        #define LV_USE_LOG 0
    #endif
#endif

#if LV_USE_LOG
/** Specify how important log should be added.
 *  Possible values:
 *  - LV_LOG_LEVEL_TRACE: A lot of logs to give detailed information
 *  - LV_LOG_LEVEL_INFO: Log important events
 *  - LV_LOG_LEVEL_WARN: Log if something unwanted happened but didn't cause a problem
 *  - LV_LOG_LEVEL_ERROR: Only critical issues, when the system may fail
 *  - LV_LOG_LEVEL_USER: Only logs added by the user
 *  - LV_LOG_LEVEL_NONE: Do not log anything
 */
#ifndef LV_LOG_LEVEL
    #ifdef CONFIG_LV_LOG_LEVEL
        #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
    #else
        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #endif
#endif

/** Use printf for log output.
 *  If not set the user needs to register a callback with `lv_log_register_print_cb`.
 */
#ifndef LV_LOG_PRINTF
    #ifdef CONFIG_LV_LOG_PRINTF
        #define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
    #else
        #define LV_LOG_PRINTF 0
    #endif
#endif

/** Enable print timestamp */
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

/** Enable print file and line number */
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

/** Enable/Disable LV_LOG_TRACE in mem module */
#ifndef LV_LOG_TRACE_MEM
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_MEM
            #define LV_LOG_TRACE_MEM CONFIG_LV_LOG_TRACE_MEM
        #else
            #define LV_LOG_TRACE_MEM 0
        #endif
    #else
        #define LV_LOG_TRACE_MEM 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in timer module */
#ifndef LV_LOG_TRACE_TIMER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_TIMER
            #define LV_LOG_TRACE_TIMER CONFIG_LV_LOG_TRACE_TIMER
        #else
            #define LV_LOG_TRACE_TIMER 0
        #endif
    #else
        #define LV_LOG_TRACE_TIMER 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in indev module */
#ifndef LV_LOG_TRACE_INDEV
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_INDEV
            #define LV_LOG_TRACE_INDEV CONFIG_LV_LOG_TRACE_INDEV
        #else
            #define LV_LOG_TRACE_INDEV 0
        #endif
    #else
        #define LV_LOG_TRACE_INDEV 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in disp refr module */
#ifndef LV_LOG_TRACE_DISP_REFR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_DISP_REFR
            #define LV_LOG_TRACE_DISP_REFR CONFIG_LV_LOG_TRACE_DISP_REFR
        #else
            #define LV_LOG_TRACE_DISP_REFR 0
        #endif
    #else
        #define LV_LOG_TRACE_DISP_REFR 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in event module */
#ifndef LV_LOG_TRACE_EVENT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_EVENT
            #define LV_LOG_TRACE_EVENT CONFIG_LV_LOG_TRACE_EVENT
        #else
            #define LV_LOG_TRACE_EVENT 0
        #endif
    #else
        #define LV_LOG_TRACE_EVENT 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in obj create module */
#ifndef LV_LOG_TRACE_OBJ_CREATE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_OBJ_CREATE
            #define LV_LOG_TRACE_OBJ_CREATE CONFIG_LV_LOG_TRACE_OBJ_CREATE
        #else
            #define LV_LOG_TRACE_OBJ_CREATE 0
        #endif
    #else
        #define LV_LOG_TRACE_OBJ_CREATE 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in layout module */
#ifndef LV_LOG_TRACE_LAYOUT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_LAYOUT
            #define LV_LOG_TRACE_LAYOUT CONFIG_LV_LOG_TRACE_LAYOUT
        #else
            #define LV_LOG_TRACE_LAYOUT 0
        #endif
    #else
        #define LV_LOG_TRACE_LAYOUT 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in anim module */
#ifndef LV_LOG_TRACE_ANIM
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_ANIM
            #define LV_LOG_TRACE_ANIM CONFIG_LV_LOG_TRACE_ANIM
        #else
            #define LV_LOG_TRACE_ANIM 0
        #endif
    #else
        #define LV_LOG_TRACE_ANIM 1
    #endif
#endif

/** Enable/Disable LV_LOG_TRACE in cache module */
#ifndef LV_LOG_TRACE_CACHE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_CACHE
            #define LV_LOG_TRACE_CACHE CONFIG_LV_LOG_TRACE_CACHE
        #else
            #define LV_LOG_TRACE_CACHE 0
        #endif
    #else
        #define LV_LOG_TRACE_CACHE 1
    #endif
#endif

#endif /*LV_USE_LOG*/


/*============================================================================
 * THEMES
 *============================================================================*/

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
/** Yes to set dark mode, No to set light mode */
#ifndef LV_THEME_DEFAULT_DARK
    #ifdef CONFIG_LV_THEME_DEFAULT_DARK
        #define LV_THEME_DEFAULT_DARK CONFIG_LV_THEME_DEFAULT_DARK
    #else
        #define LV_THEME_DEFAULT_DARK 0
    #endif
#endif

/** Enable grow on press */
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

/** Default transition time in [ms] */
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



/*============================================================================
 * LAYOUTS
 *============================================================================*/

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



/*============================================================================
 * IMAGE SETTINGS
 *============================================================================*/

/** When using only built-in image formats, caching provides little benefit.
 *  For complex image decoders (e.g. PNG or JPG), caching avoids repeatedly
 *  opening and decoding the same images, at the cost of additional RAM usage.
 */
#ifndef LV_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_CACHE_DEF_SIZE
        #define LV_CACHE_DEF_SIZE CONFIG_LV_CACHE_DEF_SIZE
    #else
        #define LV_CACHE_DEF_SIZE 0
    #endif
#endif

/** When using only built-in image formats, caching provides little benefit.
 *  For complex image decoders (e.g. PNG or JPG), caching avoids repeatedly
 *  reading image headers, at the cost of additional RAM usage.
 */
#ifndef LV_IMAGE_HEADER_CACHE_DEF_CNT
    #ifdef CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
    #else
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT 0
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

/** Use lvgl built-in LZ4 lib */
#ifndef LV_USE_LZ4_INTERNAL
    #ifdef CONFIG_LV_USE_LZ4_INTERNAL
        #define LV_USE_LZ4_INTERNAL CONFIG_LV_USE_LZ4_INTERNAL
    #else
        #define LV_USE_LZ4_INTERNAL 0
    #endif
#endif

/** Use external LZ4 library */
#ifndef LV_USE_LZ4_EXTERNAL
    #ifdef CONFIG_LV_USE_LZ4_EXTERNAL
        #define LV_USE_LZ4_EXTERNAL CONFIG_LV_USE_LZ4_EXTERNAL
    #else
        #define LV_USE_LZ4_EXTERNAL 0
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
 *  Split JPG is a custom format optimized for embedded systems.
 */
#ifndef LV_USE_TJPGD
    #ifdef CONFIG_LV_USE_TJPGD
        #define LV_USE_TJPGD CONFIG_LV_USE_TJPGD
    #else
        #define LV_USE_TJPGD 0
    #endif
#endif

/** libjpeg-turbo decoder library.
 *  - Supports complete JPEG specifications and high-performance JPEG decoding.
 */
#ifndef LV_USE_LIBJPEG_TURBO
    #ifdef CONFIG_LV_USE_LIBJPEG_TURBO
        #define LV_USE_LIBJPEG_TURBO CONFIG_LV_USE_LIBJPEG_TURBO
    #else
        #define LV_USE_LIBJPEG_TURBO 0
    #endif
#endif

/** WebP decoder library */
#ifndef LV_USE_LIBWEBP
    #ifdef CONFIG_LV_USE_LIBWEBP
        #define LV_USE_LIBWEBP CONFIG_LV_USE_LIBWEBP
    #else
        #define LV_USE_LIBWEBP 0
    #endif
#endif

#if LV_DRAW_HAS_VECTOR_SUPPORT
/** SVG library
 *  - Requires `LV_USE_VECTOR_GRAPHIC = 1`
 */
#ifndef LV_USE_SVG
    #ifdef CONFIG_LV_USE_SVG
        #define LV_USE_SVG CONFIG_LV_USE_SVG
    #else
        #define LV_USE_SVG 0
    #endif
#endif

#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

#if LV_USE_SVG
/** SVG animation */
#ifndef LV_USE_SVG_ANIMATION
    #ifdef CONFIG_LV_USE_SVG_ANIMATION
        #define LV_USE_SVG_ANIMATION CONFIG_LV_USE_SVG_ANIMATION
    #else
        #define LV_USE_SVG_ANIMATION 0
    #endif
#endif

/** Enable SVG debug logs */
#ifndef LV_USE_SVG_DEBUG
    #ifdef CONFIG_LV_USE_SVG_DEBUG
        #define LV_USE_SVG_DEBUG CONFIG_LV_USE_SVG_DEBUG
    #else
        #define LV_USE_SVG_DEBUG 0
    #endif
#endif

#endif /*LV_USE_SVG*/


/*============================================================================
 * TEXT & FONT SETTINGS
 *============================================================================*/

/** Select a character encoding for strings. Your IDE or editor should have the same character encoding.
 *  Possible values:
 *  - LV_TXT_ENC_UTF8
 *  - LV_TXT_ENC_ASCII
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
        #define LV_TXT_BREAK_CHARS " ,.;:-_)}"
    #endif
#endif

/** If a word is at least this long, will break wherever 'prettiest'.
 *  To disable, set to a value <= 0.
 */
#ifndef LV_TXT_LINE_BREAK_LONG_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
        #define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_LEN 0
    #endif
#endif

#if LV_TXT_LINE_BREAK_LONG_LEN > 0
/** Minimum number of characters in a long word to put on a line before a break. */
#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
    #endif
#endif

/** Minimum number of characters in a long word to put on a line after a break */
#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
    #endif
#endif

#endif /*LV_TXT_LINE_BREAK_LONG_LEN > 0*/

/** The control character to use for signaling text recoloring */
#ifndef LV_TXT_COLOR_CMD
    #ifdef CONFIG_LV_TXT_COLOR_CMD
        #define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
    #else
        #define LV_TXT_COLOR_CMD "#"
    #endif
#endif

/** Allows mixing Left-to-Right and Right-to-Left texts.
 *  The direction will be processed according to the Unicode Bidirectional Algorithm:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics
 */
#ifndef LV_USE_BIDI
    #ifdef CONFIG_LV_USE_BIDI
        #define LV_USE_BIDI CONFIG_LV_USE_BIDI
    #else
        #define LV_USE_BIDI 0
    #endif
#endif

#if LV_USE_BIDI
/** Set the default BIDI base direction
 *  Possible values:
 *  - LV_BASE_DIR_LTR: Left-to-Right
 *  - LV_BASE_DIR_RTL: Right-to-Left
 *  - LV_BASE_DIR_AUTO: Auto detect
 */
#ifndef LV_BIDI_BASE_DIR_DEF
    #ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
        #define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
    #else
        #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
    #endif
#endif

#endif /*LV_USE_BIDI*/

/** In these languages characters should be replaced with
 *  another form based on their position in the text.
 */
#ifndef LV_USE_ARABIC_PERSIAN_CHARS
    #ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
        #define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
    #else
        #define LV_USE_ARABIC_PERSIAN_CHARS 0
    #endif
#endif

/** Montserrat fonts with ASCII range and some symbols using bpp = 4
 *  https://fonts.google.com/specimen/Montserrat
 */
#ifndef LV_FONT_MONTSERRAT_8
    #ifdef CONFIG_LV_FONT_MONTSERRAT_8
        #define LV_FONT_MONTSERRAT_8 CONFIG_LV_FONT_MONTSERRAT_8
    #else
        #define LV_FONT_MONTSERRAT_8 0
    #endif
#endif

/** Enable Montserrat 10 */
#ifndef LV_FONT_MONTSERRAT_10
    #ifdef CONFIG_LV_FONT_MONTSERRAT_10
        #define LV_FONT_MONTSERRAT_10 CONFIG_LV_FONT_MONTSERRAT_10
    #else
        #define LV_FONT_MONTSERRAT_10 0
    #endif
#endif

/** Enable Montserrat 12 */
#ifndef LV_FONT_MONTSERRAT_12
    #ifdef CONFIG_LV_FONT_MONTSERRAT_12
        #define LV_FONT_MONTSERRAT_12 CONFIG_LV_FONT_MONTSERRAT_12
    #else
        #define LV_FONT_MONTSERRAT_12 0
    #endif
#endif

/** Enable Montserrat 14 */
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

/** Enable Montserrat 16 */
#ifndef LV_FONT_MONTSERRAT_16
    #ifdef CONFIG_LV_FONT_MONTSERRAT_16
        #define LV_FONT_MONTSERRAT_16 CONFIG_LV_FONT_MONTSERRAT_16
    #else
        #define LV_FONT_MONTSERRAT_16 0
    #endif
#endif

/** Enable Montserrat 18 */
#ifndef LV_FONT_MONTSERRAT_18
    #ifdef CONFIG_LV_FONT_MONTSERRAT_18
        #define LV_FONT_MONTSERRAT_18 CONFIG_LV_FONT_MONTSERRAT_18
    #else
        #define LV_FONT_MONTSERRAT_18 0
    #endif
#endif

/** Enable Montserrat 20 */
#ifndef LV_FONT_MONTSERRAT_20
    #ifdef CONFIG_LV_FONT_MONTSERRAT_20
        #define LV_FONT_MONTSERRAT_20 CONFIG_LV_FONT_MONTSERRAT_20
    #else
        #define LV_FONT_MONTSERRAT_20 0
    #endif
#endif

/** Enable Montserrat 22 */
#ifndef LV_FONT_MONTSERRAT_22
    #ifdef CONFIG_LV_FONT_MONTSERRAT_22
        #define LV_FONT_MONTSERRAT_22 CONFIG_LV_FONT_MONTSERRAT_22
    #else
        #define LV_FONT_MONTSERRAT_22 0
    #endif
#endif

/** Enable Montserrat 24 */
#ifndef LV_FONT_MONTSERRAT_24
    #ifdef CONFIG_LV_FONT_MONTSERRAT_24
        #define LV_FONT_MONTSERRAT_24 CONFIG_LV_FONT_MONTSERRAT_24
    #else
        #define LV_FONT_MONTSERRAT_24 0
    #endif
#endif

/** Enable Montserrat 26 */
#ifndef LV_FONT_MONTSERRAT_26
    #ifdef CONFIG_LV_FONT_MONTSERRAT_26
        #define LV_FONT_MONTSERRAT_26 CONFIG_LV_FONT_MONTSERRAT_26
    #else
        #define LV_FONT_MONTSERRAT_26 0
    #endif
#endif

/** Enable Montserrat 28 */
#ifndef LV_FONT_MONTSERRAT_28
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28
        #define LV_FONT_MONTSERRAT_28 CONFIG_LV_FONT_MONTSERRAT_28
    #else
        #define LV_FONT_MONTSERRAT_28 0
    #endif
#endif

/** Enable Montserrat 30 */
#ifndef LV_FONT_MONTSERRAT_30
    #ifdef CONFIG_LV_FONT_MONTSERRAT_30
        #define LV_FONT_MONTSERRAT_30 CONFIG_LV_FONT_MONTSERRAT_30
    #else
        #define LV_FONT_MONTSERRAT_30 0
    #endif
#endif

/** Enable Montserrat 32 */
#ifndef LV_FONT_MONTSERRAT_32
    #ifdef CONFIG_LV_FONT_MONTSERRAT_32
        #define LV_FONT_MONTSERRAT_32 CONFIG_LV_FONT_MONTSERRAT_32
    #else
        #define LV_FONT_MONTSERRAT_32 0
    #endif
#endif

/** Enable Montserrat 34 */
#ifndef LV_FONT_MONTSERRAT_34
    #ifdef CONFIG_LV_FONT_MONTSERRAT_34
        #define LV_FONT_MONTSERRAT_34 CONFIG_LV_FONT_MONTSERRAT_34
    #else
        #define LV_FONT_MONTSERRAT_34 0
    #endif
#endif

/** Enable Montserrat 36 */
#ifndef LV_FONT_MONTSERRAT_36
    #ifdef CONFIG_LV_FONT_MONTSERRAT_36
        #define LV_FONT_MONTSERRAT_36 CONFIG_LV_FONT_MONTSERRAT_36
    #else
        #define LV_FONT_MONTSERRAT_36 0
    #endif
#endif

/** Enable Montserrat 38 */
#ifndef LV_FONT_MONTSERRAT_38
    #ifdef CONFIG_LV_FONT_MONTSERRAT_38
        #define LV_FONT_MONTSERRAT_38 CONFIG_LV_FONT_MONTSERRAT_38
    #else
        #define LV_FONT_MONTSERRAT_38 0
    #endif
#endif

/** Enable Montserrat 40 */
#ifndef LV_FONT_MONTSERRAT_40
    #ifdef CONFIG_LV_FONT_MONTSERRAT_40
        #define LV_FONT_MONTSERRAT_40 CONFIG_LV_FONT_MONTSERRAT_40
    #else
        #define LV_FONT_MONTSERRAT_40 0
    #endif
#endif

/** Enable Montserrat 42 */
#ifndef LV_FONT_MONTSERRAT_42
    #ifdef CONFIG_LV_FONT_MONTSERRAT_42
        #define LV_FONT_MONTSERRAT_42 CONFIG_LV_FONT_MONTSERRAT_42
    #else
        #define LV_FONT_MONTSERRAT_42 0
    #endif
#endif

/** Enable Montserrat 44 */
#ifndef LV_FONT_MONTSERRAT_44
    #ifdef CONFIG_LV_FONT_MONTSERRAT_44
        #define LV_FONT_MONTSERRAT_44 CONFIG_LV_FONT_MONTSERRAT_44
    #else
        #define LV_FONT_MONTSERRAT_44 0
    #endif
#endif

/** Enable Montserrat 46 */
#ifndef LV_FONT_MONTSERRAT_46
    #ifdef CONFIG_LV_FONT_MONTSERRAT_46
        #define LV_FONT_MONTSERRAT_46 CONFIG_LV_FONT_MONTSERRAT_46
    #else
        #define LV_FONT_MONTSERRAT_46 0
    #endif
#endif

/** Enable Montserrat 48 */
#ifndef LV_FONT_MONTSERRAT_48
    #ifdef CONFIG_LV_FONT_MONTSERRAT_48
        #define LV_FONT_MONTSERRAT_48 CONFIG_LV_FONT_MONTSERRAT_48
    #else
        #define LV_FONT_MONTSERRAT_48 0
    #endif
#endif

/** Enable Montserrat 28 compressed */
#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
        #define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
    #else
        #define LV_FONT_MONTSERRAT_28_COMPRESSED 0
    #endif
#endif

/** Enable Dejavu 16 Persian, Hebrew, Arabic letters */
#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #else
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
    #endif
#endif

/** Enable SourceHanSansSC 14 CJK */
#ifndef LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0
    #endif
#endif

/** Enable SourceHanSansSC 16 CJK */
#ifndef LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0
    #endif
#endif

/** Pixel perfect monospaced fonts */
#ifndef LV_FONT_UNSCII_8
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FONT_UNSCII_8
            #define LV_FONT_UNSCII_8 CONFIG_LV_FONT_UNSCII_8
        #else
            #define LV_FONT_UNSCII_8 0
        #endif
    #else
        #define LV_FONT_UNSCII_8 1
    #endif
#endif

/** Enable UNSCII 16 (Perfect monospace font) */
#ifndef LV_FONT_UNSCII_16
    #ifdef CONFIG_LV_FONT_UNSCII_16
        #define LV_FONT_UNSCII_16 CONFIG_LV_FONT_UNSCII_16
    #else
        #define LV_FONT_UNSCII_16 0
    #endif
#endif

/** Optional header to override font-related macros.
 *
 *  Typical use is to declare extra fonts through LV_FONT_CUSTOM_DECLARE:
 *      #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(my_font_24)
 *
 *  It can also be used to override the default font to a custom one:
 *      #define LV_FONT_DEFAULT         &my_font_24
 */
#ifndef LV_FONT_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_FONT_CUSTOM_INCLUDE
        #define LV_FONT_CUSTOM_INCLUDE CONFIG_LV_FONT_CUSTOM_INCLUDE
    #else
        #define LV_FONT_CUSTOM_INCLUDE ""
    #endif
#endif

/** Enable this to set LV_FONT_DEFAULT to a font you define yourself
 *  rather than one of the built-in fonts below. When enabled, the
 *  built-in default-font selection is hidden, so you MUST define
 *  LV_FONT_DEFAULT yourself, either:
 *    - from the build system, e.g. -DLV_FONT_DEFAULT='&my_font_24', or
 *    - in the header given by LV_FONT_CUSTOM_INCLUDE, e.g.
 *          #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(my_font_24)
 *          #define LV_FONT_DEFAULT         &my_font_24
 *
 *  If you do not, LV_FONT_DEFAULT falls back to a builtin font,
 *  which is no longer compiled in, causing a link error.
 */
#ifndef LV_USE_CUSTOM_FONT_DEFAULT
    #ifdef CONFIG_LV_USE_CUSTOM_FONT_DEFAULT
        #define LV_USE_CUSTOM_FONT_DEFAULT CONFIG_LV_USE_CUSTOM_FONT_DEFAULT
    #else
        #define LV_USE_CUSTOM_FONT_DEFAULT 0
    #endif
#endif

#if !LV_USE_CUSTOM_FONT_DEFAULT
/** Select theme default text font
 *  Possible values:
 *  - LV_FONT_DEFAULT_MONTSERRAT_8
 *  - LV_FONT_DEFAULT_MONTSERRAT_10
 *  - LV_FONT_DEFAULT_MONTSERRAT_12
 *  - LV_FONT_DEFAULT_MONTSERRAT_14
 *  - LV_FONT_DEFAULT_MONTSERRAT_16
 *  - LV_FONT_DEFAULT_MONTSERRAT_18
 *  - LV_FONT_DEFAULT_MONTSERRAT_20
 *  - LV_FONT_DEFAULT_MONTSERRAT_22
 *  - LV_FONT_DEFAULT_MONTSERRAT_24
 *  - LV_FONT_DEFAULT_MONTSERRAT_26
 *  - LV_FONT_DEFAULT_MONTSERRAT_28
 *  - LV_FONT_DEFAULT_MONTSERRAT_30
 *  - LV_FONT_DEFAULT_MONTSERRAT_32
 *  - LV_FONT_DEFAULT_MONTSERRAT_34
 *  - LV_FONT_DEFAULT_MONTSERRAT_36
 *  - LV_FONT_DEFAULT_MONTSERRAT_38
 *  - LV_FONT_DEFAULT_MONTSERRAT_40
 *  - LV_FONT_DEFAULT_MONTSERRAT_42
 *  - LV_FONT_DEFAULT_MONTSERRAT_44
 *  - LV_FONT_DEFAULT_MONTSERRAT_46
 *  - LV_FONT_DEFAULT_MONTSERRAT_48
 *  - LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED
 *  - LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW: Dejavu 16 Persian, Hebrew, Arabic letters
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK
 *  - LV_FONT_DEFAULT_UNSCII_8: UNSCII 8 (Perfect monospace font)
 *  - LV_FONT_DEFAULT_UNSCII_16: UNSCII 16 (Perfect monospace font)
 */
#ifndef LV_FONT_DEFAULT
    #ifdef CONFIG_LV_FONT_DEFAULT
        #define LV_FONT_DEFAULT CONFIG_LV_FONT_DEFAULT
    #else
        #define LV_FONT_DEFAULT LV_FONT_DEFAULT_MONTSERRAT_14
    #endif
#endif

#endif /*!LV_USE_CUSTOM_FONT_DEFAULT*/

/** The limit depends on the font size, font face and format
 *  but with > 10,000 characters if you see issues probably you
 *  need to enable it.
 */
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

/** Enable Font manager */
#ifndef LV_USE_FONT_MANAGER
    #ifdef CONFIG_LV_USE_FONT_MANAGER
        #define LV_USE_FONT_MANAGER CONFIG_LV_USE_FONT_MANAGER
    #else
        #define LV_USE_FONT_MANAGER 0
    #endif
#endif

#if LV_USE_FONT_MANAGER
/** Font manager name max length */
#ifndef LV_FONT_MANAGER_NAME_MAX_LEN
    #ifdef CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
        #define LV_FONT_MANAGER_NAME_MAX_LEN CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
    #else
        #define LV_FONT_MANAGER_NAME_MAX_LEN 32
    #endif
#endif

#endif /*LV_USE_FONT_MANAGER*/

/** Support using images as font in label or span widgets */
#ifndef LV_USE_IMGFONT
    #ifdef CONFIG_LV_USE_IMGFONT
        #define LV_USE_IMGFONT CONFIG_LV_USE_IMGFONT
    #else
        #define LV_USE_IMGFONT 0
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
/** When enabled, FreeType will use LVGL's memory allocator and file system
 *  abstraction instead of the platform defaults.
 */
#ifndef LV_FREETYPE_USE_LVGL_PORT
    #ifdef CONFIG_LV_FREETYPE_USE_LVGL_PORT
        #define LV_FREETYPE_USE_LVGL_PORT CONFIG_LV_FREETYPE_USE_LVGL_PORT
    #else
        #define LV_FREETYPE_USE_LVGL_PORT 0
    #endif
#endif

/** The maximum number of Glyph in count */
#ifndef LV_FREETYPE_CACHE_FT_GLYPH_CNT
    #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #define LV_FREETYPE_CACHE_FT_GLYPH_CNT CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
    #else
        #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
    #endif
#endif

#if LV_USE_OS == LV_OS_NONE
/** Enable L1 glyph metrics cache (lock-free, single-thread only) */
#ifndef LV_FREETYPE_CACHE_FT_GLYPH_L1
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_L1
            #define LV_FREETYPE_CACHE_FT_GLYPH_L1 CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_L1
        #else
            #define LV_FREETYPE_CACHE_FT_GLYPH_L1 0
        #endif
    #else
        #define LV_FREETYPE_CACHE_FT_GLYPH_L1 1
    #endif
#endif

#endif /*LV_USE_OS == LV_OS_NONE*/
#endif /*LV_USE_FREETYPE*/

/** Built-in TTF decoder */
#ifndef LV_USE_TINY_TTF
    #ifdef CONFIG_LV_USE_TINY_TTF
        #define LV_USE_TINY_TTF CONFIG_LV_USE_TINY_TTF
    #else
        #define LV_USE_TINY_TTF 0
    #endif
#endif

#if LV_USE_TINY_TTF
/** Enable loading Tiny TTF data from files */
#ifndef LV_TINY_TTF_FILE_SUPPORT
    #ifdef CONFIG_LV_TINY_TTF_FILE_SUPPORT
        #define LV_TINY_TTF_FILE_SUPPORT CONFIG_LV_TINY_TTF_FILE_SUPPORT
    #else
        #define LV_TINY_TTF_FILE_SUPPORT 0
    #endif
#endif

/** Tiny ttf cache entries count */
#ifndef LV_TINY_TTF_CACHE_GLYPH_CNT
    #ifdef CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
        #define LV_TINY_TTF_CACHE_GLYPH_CNT CONFIG_LV_TINY_TTF_CACHE_GLYPH_CNT
    #else
        #define LV_TINY_TTF_CACHE_GLYPH_CNT 128
    #endif
#endif

/** Tiny ttf kerning cache entries count */
#ifndef LV_TINY_TTF_CACHE_KERNING_CNT
    #ifdef CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
        #define LV_TINY_TTF_CACHE_KERNING_CNT CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
    #else
        #define LV_TINY_TTF_CACHE_KERNING_CNT 256
    #endif
#endif

#endif /*LV_USE_TINY_TTF*/


/*============================================================================
 * WIDGETS
 *============================================================================*/

/** Causes these widgets to be given default values at creation time.
 *  - lv_buttonmatrix_t:  Get default maps:  {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, else map not set.
 *  - lv_checkbox_t    :  String label set to "Check box", else set to empty string.
 *  - lv_dropdown_t    :  Options set to "Option 1", "Option 2", "Option 3", else no values are set.
 *  - lv_roller_t      :  Options set to "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", else no values are set.
 *  - lv_label_t       :  Text set to "Text", else empty string.
 *  - lv_arclabel_t   :  Text set to "Arced Text", else empty string.
 */
#ifndef LV_WIDGETS_HAS_DEFAULT_VALUE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
            #define LV_WIDGETS_HAS_DEFAULT_VALUE CONFIG_LV_WIDGETS_HAS_DEFAULT_VALUE
        #else
            #define LV_WIDGETS_HAS_DEFAULT_VALUE 0
        #endif
    #else
        #define LV_WIDGETS_HAS_DEFAULT_VALUE 1
    #endif
#endif

#if LV_DRAW_HAS_3D_SUPPORT
/** 3D Texture */
#ifndef LV_USE_3DTEXTURE
    #ifdef CONFIG_LV_USE_3DTEXTURE
        #define LV_USE_3DTEXTURE CONFIG_LV_USE_3DTEXTURE
    #else
        #define LV_USE_3DTEXTURE 0
    #endif
#endif

#endif /*LV_DRAW_HAS_3D_SUPPORT*/

/** Anim image */
#ifndef LV_USE_ANIMIMG
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ANIMIMG
            #define LV_USE_ANIMIMG CONFIG_LV_USE_ANIMIMG
        #else
            #define LV_USE_ANIMIMG 0
        #endif
    #else
        #define LV_USE_ANIMIMG 1
    #endif
#endif

/** Arc */
#ifndef LV_USE_ARC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ARC
            #define LV_USE_ARC CONFIG_LV_USE_ARC
        #else
            #define LV_USE_ARC 0
        #endif
    #else
        #define LV_USE_ARC 1
    #endif
#endif

/** Arc Label */
#ifndef LV_USE_ARCLABEL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ARCLABEL
            #define LV_USE_ARCLABEL CONFIG_LV_USE_ARCLABEL
        #else
            #define LV_USE_ARCLABEL 0
        #endif
    #else
        #define LV_USE_ARCLABEL 1
    #endif
#endif

/** Bar */
#ifndef LV_USE_BAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BAR
            #define LV_USE_BAR CONFIG_LV_USE_BAR
        #else
            #define LV_USE_BAR 0
        #endif
    #else
        #define LV_USE_BAR 1
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

/** Button */
#ifndef LV_USE_BUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTON
            #define LV_USE_BUTTON CONFIG_LV_USE_BUTTON
        #else
            #define LV_USE_BUTTON 0
        #endif
    #else
        #define LV_USE_BUTTON 1
    #endif
#endif

/** Button matrix */
#ifndef LV_USE_BUTTONMATRIX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BUTTONMATRIX
            #define LV_USE_BUTTONMATRIX CONFIG_LV_USE_BUTTONMATRIX
        #else
            #define LV_USE_BUTTONMATRIX 0
        #endif
    #else
        #define LV_USE_BUTTONMATRIX 1
    #endif
#endif

/** Calendar */
#ifndef LV_USE_CALENDAR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CALENDAR
            #define LV_USE_CALENDAR CONFIG_LV_USE_CALENDAR
        #else
            #define LV_USE_CALENDAR 0
        #endif
    #else
        #define LV_USE_CALENDAR 1
    #endif
#endif

#if LV_USE_CALENDAR
/** Calendar week starts monday */
#ifndef LV_CALENDAR_WEEK_STARTS_MONDAY
    #ifdef CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_WEEK_STARTS_MONDAY CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
    #else
        #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #endif
#endif

/** Shortened string for Monday */
#ifndef LV_MONDAY_STR
    #ifdef CONFIG_LV_MONDAY_STR
        #define LV_MONDAY_STR CONFIG_LV_MONDAY_STR
    #else
        #define LV_MONDAY_STR "Mo"
    #endif
#endif

/** Shortened string for Tuesday */
#ifndef LV_TUESDAY_STR
    #ifdef CONFIG_LV_TUESDAY_STR
        #define LV_TUESDAY_STR CONFIG_LV_TUESDAY_STR
    #else
        #define LV_TUESDAY_STR "Tu"
    #endif
#endif

/** Shortened string for Wednesday */
#ifndef LV_WEDNESDAY_STR
    #ifdef CONFIG_LV_WEDNESDAY_STR
        #define LV_WEDNESDAY_STR CONFIG_LV_WEDNESDAY_STR
    #else
        #define LV_WEDNESDAY_STR "We"
    #endif
#endif

/** Shortened string for Thursday */
#ifndef LV_THURSDAY_STR
    #ifdef CONFIG_LV_THURSDAY_STR
        #define LV_THURSDAY_STR CONFIG_LV_THURSDAY_STR
    #else
        #define LV_THURSDAY_STR "Th"
    #endif
#endif

/** Shortened string for Friday */
#ifndef LV_FRIDAY_STR
    #ifdef CONFIG_LV_FRIDAY_STR
        #define LV_FRIDAY_STR CONFIG_LV_FRIDAY_STR
    #else
        #define LV_FRIDAY_STR "Fr"
    #endif
#endif

/** Shortened string for Saturday */
#ifndef LV_SATURDAY_STR
    #ifdef CONFIG_LV_SATURDAY_STR
        #define LV_SATURDAY_STR CONFIG_LV_SATURDAY_STR
    #else
        #define LV_SATURDAY_STR "Sa"
    #endif
#endif

/** Shortened string for Sunday */
#ifndef LV_SUNDAY_STR
    #ifdef CONFIG_LV_SUNDAY_STR
        #define LV_SUNDAY_STR CONFIG_LV_SUNDAY_STR
    #else
        #define LV_SUNDAY_STR "Su"
    #endif
#endif

/** String for January */
#ifndef LV_JANUARY_STR
    #ifdef CONFIG_LV_JANUARY_STR
        #define LV_JANUARY_STR CONFIG_LV_JANUARY_STR
    #else
        #define LV_JANUARY_STR "January"
    #endif
#endif

/** String for February */
#ifndef LV_FEBRUARY_STR
    #ifdef CONFIG_LV_FEBRUARY_STR
        #define LV_FEBRUARY_STR CONFIG_LV_FEBRUARY_STR
    #else
        #define LV_FEBRUARY_STR "February"
    #endif
#endif

/** String for March */
#ifndef LV_MARCH_STR
    #ifdef CONFIG_LV_MARCH_STR
        #define LV_MARCH_STR CONFIG_LV_MARCH_STR
    #else
        #define LV_MARCH_STR "March"
    #endif
#endif

/** String for April */
#ifndef LV_APRIL_STR
    #ifdef CONFIG_LV_APRIL_STR
        #define LV_APRIL_STR CONFIG_LV_APRIL_STR
    #else
        #define LV_APRIL_STR "April"
    #endif
#endif

/** String for May */
#ifndef LV_MAY_STR
    #ifdef CONFIG_LV_MAY_STR
        #define LV_MAY_STR CONFIG_LV_MAY_STR
    #else
        #define LV_MAY_STR "May"
    #endif
#endif

/** String for June */
#ifndef LV_JUNE_STR
    #ifdef CONFIG_LV_JUNE_STR
        #define LV_JUNE_STR CONFIG_LV_JUNE_STR
    #else
        #define LV_JUNE_STR "June"
    #endif
#endif

/** String for July */
#ifndef LV_JULY_STR
    #ifdef CONFIG_LV_JULY_STR
        #define LV_JULY_STR CONFIG_LV_JULY_STR
    #else
        #define LV_JULY_STR "July"
    #endif
#endif

/** String for August */
#ifndef LV_AUGUST_STR
    #ifdef CONFIG_LV_AUGUST_STR
        #define LV_AUGUST_STR CONFIG_LV_AUGUST_STR
    #else
        #define LV_AUGUST_STR "August"
    #endif
#endif

/** String for September */
#ifndef LV_SEPTEMBER_STR
    #ifdef CONFIG_LV_SEPTEMBER_STR
        #define LV_SEPTEMBER_STR CONFIG_LV_SEPTEMBER_STR
    #else
        #define LV_SEPTEMBER_STR "September"
    #endif
#endif

/** String for October */
#ifndef LV_OCTOBER_STR
    #ifdef CONFIG_LV_OCTOBER_STR
        #define LV_OCTOBER_STR CONFIG_LV_OCTOBER_STR
    #else
        #define LV_OCTOBER_STR "October"
    #endif
#endif

/** String for November */
#ifndef LV_NOVEMBER_STR
    #ifdef CONFIG_LV_NOVEMBER_STR
        #define LV_NOVEMBER_STR CONFIG_LV_NOVEMBER_STR
    #else
        #define LV_NOVEMBER_STR "November"
    #endif
#endif

/** String for December */
#ifndef LV_DECEMBER_STR
    #ifdef CONFIG_LV_DECEMBER_STR
        #define LV_DECEMBER_STR CONFIG_LV_DECEMBER_STR
    #else
        #define LV_DECEMBER_STR "December"
    #endif
#endif

/** Use calendar header arrow */
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

/** Use calendar header dropdown */
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

/** Use chinese calendar */
#ifndef LV_USE_CALENDAR_CHINESE
    #ifdef CONFIG_LV_USE_CALENDAR_CHINESE
        #define LV_USE_CALENDAR_CHINESE CONFIG_LV_USE_CALENDAR_CHINESE
    #else
        #define LV_USE_CALENDAR_CHINESE 0
    #endif
#endif

#endif /*LV_USE_CALENDAR*/

/** Canvas */
#ifndef LV_USE_CANVAS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CANVAS
            #define LV_USE_CANVAS CONFIG_LV_USE_CANVAS
        #else
            #define LV_USE_CANVAS 0
        #endif
    #else
        #define LV_USE_CANVAS 1
    #endif
#endif

/** Chart */
#ifndef LV_USE_CHART
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHART
            #define LV_USE_CHART CONFIG_LV_USE_CHART
        #else
            #define LV_USE_CHART 0
        #endif
    #else
        #define LV_USE_CHART 1
    #endif
#endif

/** Check Box */
#ifndef LV_USE_CHECKBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHECKBOX
            #define LV_USE_CHECKBOX CONFIG_LV_USE_CHECKBOX
        #else
            #define LV_USE_CHECKBOX 0
        #endif
    #else
        #define LV_USE_CHECKBOX 1
    #endif
#endif

/** Drop down list */
#ifndef LV_USE_DROPDOWN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DROPDOWN
            #define LV_USE_DROPDOWN CONFIG_LV_USE_DROPDOWN
        #else
            #define LV_USE_DROPDOWN 0
        #endif
    #else
        #define LV_USE_DROPDOWN 1
    #endif
#endif

/** FFmpeg library for image decoding and playing videos.
 *  Supports all major image formats so do not enable other image decoder with it.
 */
#ifndef LV_USE_FFMPEG
    #ifdef CONFIG_LV_USE_FFMPEG
        #define LV_USE_FFMPEG CONFIG_LV_USE_FFMPEG
    #else
        #define LV_USE_FFMPEG 0
    #endif
#endif

#if LV_USE_FFMPEG
/** Dump format */
#ifndef LV_FFMPEG_DUMP_FORMAT
    #ifdef CONFIG_LV_FFMPEG_DUMP_FORMAT
        #define LV_FFMPEG_DUMP_FORMAT CONFIG_LV_FFMPEG_DUMP_FORMAT
    #else
        #define LV_FFMPEG_DUMP_FORMAT 0
    #endif
#endif

/** You won't be able to open URLs after enabling this feature.
 *  Note that FFmpeg image decoder will always use lvgl file system.
 */
#ifndef LV_FFMPEG_PLAYER_USE_LV_FS
    #ifdef CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
        #define LV_FFMPEG_PLAYER_USE_LV_FS CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
    #else
        #define LV_FFMPEG_PLAYER_USE_LV_FS 0
    #endif
#endif

#endif /*LV_USE_FFMPEG*/

/** GIF decoder library */
#ifndef LV_USE_GIF
    #ifdef CONFIG_LV_USE_GIF
        #define LV_USE_GIF CONFIG_LV_USE_GIF
    #else
        #define LV_USE_GIF 0
    #endif
#endif

#if LV_USE_GIF
/** Use extra 16KB RAM to cache decoded data to accelerate */
#ifndef LV_GIF_CACHE_DECODE_DATA
    #ifdef CONFIG_LV_GIF_CACHE_DECODE_DATA
        #define LV_GIF_CACHE_DECODE_DATA CONFIG_LV_GIF_CACHE_DECODE_DATA
    #else
        #define LV_GIF_CACHE_DECODE_DATA 0
    #endif
#endif

#endif /*LV_USE_GIF*/

#if LV_DRAW_HAS_3D_SUPPORT
/** Requires `LV_USE_3DTEXTURE = 1` */
#ifndef LV_USE_GLTF
    #ifdef CONFIG_LV_USE_GLTF
        #define LV_USE_GLTF CONFIG_LV_USE_GLTF
    #else
        #define LV_USE_GLTF 0
    #endif
#endif

#endif /*LV_DRAW_HAS_3D_SUPPORT*/

/** GStreamer library */
#ifndef LV_USE_GSTREAMER
    #ifdef CONFIG_LV_USE_GSTREAMER
        #define LV_USE_GSTREAMER CONFIG_LV_USE_GSTREAMER
    #else
        #define LV_USE_GSTREAMER 0
    #endif
#endif

/** Image */
#ifndef LV_USE_IMAGE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGE
            #define LV_USE_IMAGE CONFIG_LV_USE_IMAGE
        #else
            #define LV_USE_IMAGE 0
        #endif
    #else
        #define LV_USE_IMAGE 1
    #endif
#endif

/** ImageButton */
#ifndef LV_USE_IMAGEBUTTON
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMAGEBUTTON
            #define LV_USE_IMAGEBUTTON CONFIG_LV_USE_IMAGEBUTTON
        #else
            #define LV_USE_IMAGEBUTTON 0
        #endif
    #else
        #define LV_USE_IMAGEBUTTON 1
    #endif
#endif

/** Enable Pinyin input method */
#ifndef LV_USE_IME_PINYIN
    #ifdef CONFIG_LV_USE_IME_PINYIN
        #define LV_USE_IME_PINYIN CONFIG_LV_USE_IME_PINYIN
    #else
        #define LV_USE_IME_PINYIN 0
    #endif
#endif

#if LV_USE_IME_PINYIN
/** Enable Pinyin input method 9 key input mode */
#ifndef LV_IME_PINYIN_USE_K9_MODE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_IME_PINYIN_USE_K9_MODE
            #define LV_IME_PINYIN_USE_K9_MODE CONFIG_LV_IME_PINYIN_USE_K9_MODE
        #else
            #define LV_IME_PINYIN_USE_K9_MODE 0
        #endif
    #else
        #define LV_IME_PINYIN_USE_K9_MODE 1
    #endif
#endif

#if LV_IME_PINYIN_USE_K9_MODE
/** Maximum number of candidate panels for 9-key input mode */
#ifndef LV_IME_PINYIN_K9_CAND_TEXT_NUM
    #ifdef CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
    #else
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif
#endif

#endif /*LV_IME_PINYIN_USE_K9_MODE*/

/** If you do not use the default thesaurus, be sure to use lv_ime_pinyin after setting the thesaurus */
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

/** Set the maximum number of candidate panels that can be displayed.
 *  This needs to be adjusted according to the size of the screen.
 */
#ifndef LV_IME_PINYIN_CAND_TEXT_NUM
    #ifdef CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
        #define LV_IME_PINYIN_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
    #else
        #define LV_IME_PINYIN_CAND_TEXT_NUM 6
    #endif
#endif

#endif /*LV_USE_IME_PINYIN*/

/** Keyboard */
#ifndef LV_USE_KEYBOARD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_KEYBOARD
            #define LV_USE_KEYBOARD CONFIG_LV_USE_KEYBOARD
        #else
            #define LV_USE_KEYBOARD 0
        #endif
    #else
        #define LV_USE_KEYBOARD 1
    #endif
#endif

/** Label */
#ifndef LV_USE_LABEL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LABEL
            #define LV_USE_LABEL CONFIG_LV_USE_LABEL
        #else
            #define LV_USE_LABEL 0
        #endif
    #else
        #define LV_USE_LABEL 1
    #endif
#endif

#if LV_USE_LABEL
/** Enable selecting text of the label */
#ifndef LV_LABEL_TEXT_SELECTION
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LABEL_TEXT_SELECTION
            #define LV_LABEL_TEXT_SELECTION CONFIG_LV_LABEL_TEXT_SELECTION
        #else
            #define LV_LABEL_TEXT_SELECTION 0
        #endif
    #else
        #define LV_LABEL_TEXT_SELECTION 1
    #endif
#endif

/** Store extra some info in labels (12 bytes) to speed up drawing of very long texts */
#ifndef LV_LABEL_LONG_TXT_HINT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LABEL_LONG_TXT_HINT
            #define LV_LABEL_LONG_TXT_HINT CONFIG_LV_LABEL_LONG_TXT_HINT
        #else
            #define LV_LABEL_LONG_TXT_HINT 0
        #endif
    #else
        #define LV_LABEL_LONG_TXT_HINT 1
    #endif
#endif

/** The count of wait chart */
#ifndef LV_LABEL_WAIT_CHAR_COUNT
    #ifdef CONFIG_LV_LABEL_WAIT_CHAR_COUNT
        #define LV_LABEL_WAIT_CHAR_COUNT CONFIG_LV_LABEL_WAIT_CHAR_COUNT
    #else
        #define LV_LABEL_WAIT_CHAR_COUNT 3
    #endif
#endif

#endif /*LV_USE_LABEL*/

/** LED */
#ifndef LV_USE_LED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LED
            #define LV_USE_LED CONFIG_LV_USE_LED
        #else
            #define LV_USE_LED 0
        #endif
    #else
        #define LV_USE_LED 1
    #endif
#endif

/** Line */
#ifndef LV_USE_LINE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LINE
            #define LV_USE_LINE CONFIG_LV_USE_LINE
        #else
            #define LV_USE_LINE 0
        #endif
    #else
        #define LV_USE_LINE 1
    #endif
#endif

/** List */
#ifndef LV_USE_LIST
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LIST
            #define LV_USE_LIST CONFIG_LV_USE_LIST
        #else
            #define LV_USE_LIST 0
        #endif
    #else
        #define LV_USE_LIST 1
    #endif
#endif

#if LV_DRAW_HAS_VECTOR_SUPPORT
/** Enable Lottie animations */
#ifndef LV_USE_LOTTIE
    #ifdef CONFIG_LV_USE_LOTTIE
        #define LV_USE_LOTTIE CONFIG_LV_USE_LOTTIE
    #else
        #define LV_USE_LOTTIE 0
    #endif
#endif

#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

/** Menu */
#ifndef LV_USE_MENU
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MENU
            #define LV_USE_MENU CONFIG_LV_USE_MENU
        #else
            #define LV_USE_MENU 0
        #endif
    #else
        #define LV_USE_MENU 1
    #endif
#endif

/** Msgbox */
#ifndef LV_USE_MSGBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MSGBOX
            #define LV_USE_MSGBOX CONFIG_LV_USE_MSGBOX
        #else
            #define LV_USE_MSGBOX 0
        #endif
    #else
        #define LV_USE_MSGBOX 1
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

/** Rlottie library */
#ifndef LV_USE_RLOTTIE
    #ifdef CONFIG_LV_USE_RLOTTIE
        #define LV_USE_RLOTTIE CONFIG_LV_USE_RLOTTIE
    #else
        #define LV_USE_RLOTTIE 0
    #endif
#endif

/** Roller */
#ifndef LV_USE_ROLLER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ROLLER
            #define LV_USE_ROLLER CONFIG_LV_USE_ROLLER
        #else
            #define LV_USE_ROLLER 0
        #endif
    #else
        #define LV_USE_ROLLER 1
    #endif
#endif

/** Scale */
#ifndef LV_USE_SCALE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SCALE
            #define LV_USE_SCALE CONFIG_LV_USE_SCALE
        #else
            #define LV_USE_SCALE 0
        #endif
    #else
        #define LV_USE_SCALE 1
    #endif
#endif

/** Slider */
#ifndef LV_USE_SLIDER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SLIDER
            #define LV_USE_SLIDER CONFIG_LV_USE_SLIDER
        #else
            #define LV_USE_SLIDER 0
        #endif
    #else
        #define LV_USE_SLIDER 1
    #endif
#endif

/** Span */
#ifndef LV_USE_SPAN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPAN
            #define LV_USE_SPAN CONFIG_LV_USE_SPAN
        #else
            #define LV_USE_SPAN 0
        #endif
    #else
        #define LV_USE_SPAN 1
    #endif
#endif

#if LV_USE_SPAN
/** Maximum number of span descriptor */
#ifndef LV_SPAN_SNIPPET_STACK_SIZE
    #ifdef CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
        #define LV_SPAN_SNIPPET_STACK_SIZE CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
    #else
        #define LV_SPAN_SNIPPET_STACK_SIZE 64
    #endif
#endif

#endif /*LV_USE_SPAN*/

/** Spinbox */
#ifndef LV_USE_SPINBOX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINBOX
            #define LV_USE_SPINBOX CONFIG_LV_USE_SPINBOX
        #else
            #define LV_USE_SPINBOX 0
        #endif
    #else
        #define LV_USE_SPINBOX 1
    #endif
#endif

/** Spinner */
#ifndef LV_USE_SPINNER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINNER
            #define LV_USE_SPINNER CONFIG_LV_USE_SPINNER
        #else
            #define LV_USE_SPINNER 0
        #endif
    #else
        #define LV_USE_SPINNER 1
    #endif
#endif

/** Switch */
#ifndef LV_USE_SWITCH
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SWITCH
            #define LV_USE_SWITCH CONFIG_LV_USE_SWITCH
        #else
            #define LV_USE_SWITCH 0
        #endif
    #else
        #define LV_USE_SWITCH 1
    #endif
#endif

/** Text area */
#ifndef LV_USE_TEXTAREA
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TEXTAREA
            #define LV_USE_TEXTAREA CONFIG_LV_USE_TEXTAREA
        #else
            #define LV_USE_TEXTAREA 0
        #endif
    #else
        #define LV_USE_TEXTAREA 1
    #endif
#endif

#if LV_USE_TEXTAREA
/** Text area def. pwd show time [ms] */
#ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #else
        #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500
    #endif
#endif

#endif /*LV_USE_TEXTAREA*/

/** Table */
#ifndef LV_USE_TABLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABLE
            #define LV_USE_TABLE CONFIG_LV_USE_TABLE
        #else
            #define LV_USE_TABLE 0
        #endif
    #else
        #define LV_USE_TABLE 1
    #endif
#endif

/** Tabview */
#ifndef LV_USE_TABVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABVIEW
            #define LV_USE_TABVIEW CONFIG_LV_USE_TABVIEW
        #else
            #define LV_USE_TABVIEW 0
        #endif
    #else
        #define LV_USE_TABVIEW 1
    #endif
#endif

/** Tileview */
#ifndef LV_USE_TILEVIEW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TILEVIEW
            #define LV_USE_TILEVIEW CONFIG_LV_USE_TILEVIEW
        #else
            #define LV_USE_TILEVIEW 0
        #endif
    #else
        #define LV_USE_TILEVIEW 1
    #endif
#endif

/** Win */
#ifndef LV_USE_WIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_WIN
            #define LV_USE_WIN CONFIG_LV_USE_WIN
        #else
            #define LV_USE_WIN 0
        #endif
    #else
        #define LV_USE_WIN 1
    #endif
#endif



/*============================================================================
 * DRIVERS
 *============================================================================*/

/** Driver for /dev/dri/card */
#ifndef LV_USE_LINUX_DRM
    #ifdef CONFIG_LV_USE_LINUX_DRM
        #define LV_USE_LINUX_DRM CONFIG_LV_USE_LINUX_DRM
    #else
        #define LV_USE_LINUX_DRM 0
    #endif
#endif

#if LV_USE_LINUX_DRM
/** Hardware-accelerated rendering via OpenGL ES 2.0 and EGL.
 *  Requires OpenGL ES 2.0 on the target hardware and linking with
 *  -lEGL. Compatible with LVGL 3D/glTF rendering.
 *  Automatically enables GBM buffer support.
 */
#ifndef LV_USE_LINUX_DRM_EGL
    #ifdef CONFIG_LV_USE_LINUX_DRM_EGL
        #define LV_USE_LINUX_DRM_EGL CONFIG_LV_USE_LINUX_DRM_EGL
    #else
        #define LV_USE_LINUX_DRM_EGL 0
    #endif
#endif

/** Use MESA GBM for Linux DRM DMA buffers */
#ifndef LV_USE_LINUX_DRM_GBM_BUFFERS
    #ifdef CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS
        #define LV_USE_LINUX_DRM_GBM_BUFFERS CONFIG_LV_USE_LINUX_DRM_GBM_BUFFERS
    #else
        #define LV_USE_LINUX_DRM_GBM_BUFFERS 0
    #endif
#endif

#endif /*LV_USE_LINUX_DRM*/

/** Driver for /dev/fb */
#ifndef LV_USE_LINUX_FBDEV
    #ifdef CONFIG_LV_USE_LINUX_FBDEV
        #define LV_USE_LINUX_FBDEV CONFIG_LV_USE_LINUX_FBDEV
    #else
        #define LV_USE_LINUX_FBDEV 0
    #endif
#endif

#if LV_USE_LINUX_FBDEV
/** Use BSD flavored framebuffer device */
#ifndef LV_LINUX_FBDEV_BSD
    #ifdef CONFIG_LV_LINUX_FBDEV_BSD
        #define LV_LINUX_FBDEV_BSD CONFIG_LV_LINUX_FBDEV_BSD
    #else
        #define LV_LINUX_FBDEV_BSD 0
    #endif
#endif

/** Framebuffer device render mode
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full mode
 */
#ifndef LV_LINUX_FBDEV_RENDER_MODE
    #ifdef CONFIG_LV_LINUX_FBDEV_RENDER_MODE
        #define LV_LINUX_FBDEV_RENDER_MODE CONFIG_LV_LINUX_FBDEV_RENDER_MODE
    #else
        #define LV_LINUX_FBDEV_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
    #endif
#endif

#ifndef LV_LINUX_FBDEV_BUFFER_COUNT
    #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
        #define LV_LINUX_FBDEV_BUFFER_COUNT CONFIG_LV_LINUX_FBDEV_BUFFER_COUNT
    #else
        #define LV_LINUX_FBDEV_BUFFER_COUNT 0
    #endif
#endif

#if LV_LINUX_FBDEV_CUSTOM_BUFFER
/** Custom partial buffer size (in number of rows) */
#ifndef LV_LINUX_FBDEV_BUFFER_SIZE
    #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
        #define LV_LINUX_FBDEV_BUFFER_SIZE CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
    #else
        #define LV_LINUX_FBDEV_BUFFER_SIZE 60
    #endif
#endif

#endif /*LV_LINUX_FBDEV_CUSTOM_BUFFER*/

/** Framebuffer device supports mmap */
#ifndef LV_LINUX_FBDEV_MMAP
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LINUX_FBDEV_MMAP
            #define LV_LINUX_FBDEV_MMAP CONFIG_LV_LINUX_FBDEV_MMAP
        #else
            #define LV_LINUX_FBDEV_MMAP 0
        #endif
    #else
        #define LV_LINUX_FBDEV_MMAP 1
    #endif
#endif

#endif /*LV_USE_LINUX_FBDEV*/

/** Use ft81x EVE driver */
#ifndef LV_USE_FT81X
    #ifdef CONFIG_LV_USE_FT81X
        #define LV_USE_FT81X CONFIG_LV_USE_FT81X
    #else
        #define LV_USE_FT81X 0
    #endif
#endif

/** Interface for Lovyan_GFX */
#ifndef LV_USE_LOVYAN_GFX
    #ifdef CONFIG_LV_USE_LOVYAN_GFX
        #define LV_USE_LOVYAN_GFX CONFIG_LV_USE_LOVYAN_GFX
    #else
        #define LV_USE_LOVYAN_GFX 0
    #endif
#endif

#if LV_USE_LOVYAN_GFX
/** Header for LovyanGFX user configuration */
#ifndef LV_LGFX_USER_INCLUDE
    #ifdef CONFIG_LV_LGFX_USER_INCLUDE
        #define LV_LGFX_USER_INCLUDE CONFIG_LV_LGFX_USER_INCLUDE
    #else
        #define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"
    #endif
#endif

#endif /*LV_USE_LOVYAN_GFX*/

/** Generic MIPI driver for LCD devices connected via SPI or parallel
 *  port. Use this if your specific LCD controller is not listed among
 *  the dedicated drivers below.
 */
#ifndef LV_USE_GENERIC_MIPI
    #ifdef CONFIG_LV_USE_GENERIC_MIPI
        #define LV_USE_GENERIC_MIPI CONFIG_LV_USE_GENERIC_MIPI
    #else
        #define LV_USE_GENERIC_MIPI 0
    #endif
#endif

/** Drivers for LCD devices connected via SPI/parallel port */
#ifndef LV_USE_ST7735
    #ifdef CONFIG_LV_USE_ST7735
        #define LV_USE_ST7735 CONFIG_LV_USE_ST7735
    #else
        #define LV_USE_ST7735 0
    #endif
#endif

/** Use ST7789 LCD driver */
#ifndef LV_USE_ST7789
    #ifdef CONFIG_LV_USE_ST7789
        #define LV_USE_ST7789 CONFIG_LV_USE_ST7789
    #else
        #define LV_USE_ST7789 0
    #endif
#endif

/** Use ST7796 LCD driver */
#ifndef LV_USE_ST7796
    #ifdef CONFIG_LV_USE_ST7796
        #define LV_USE_ST7796 CONFIG_LV_USE_ST7796
    #else
        #define LV_USE_ST7796 0
    #endif
#endif

/** Use ILI9341 LCD driver */
#ifndef LV_USE_ILI9341
    #ifdef CONFIG_LV_USE_ILI9341
        #define LV_USE_ILI9341 CONFIG_LV_USE_ILI9341
    #else
        #define LV_USE_ILI9341 0
    #endif
#endif

/** Use NV3007 LCD driver */
#ifndef LV_USE_NV3007
    #ifdef CONFIG_LV_USE_NV3007
        #define LV_USE_NV3007 CONFIG_LV_USE_NV3007
    #else
        #define LV_USE_NV3007 0
    #endif
#endif

/** Driver for NXP ELCDIF */
#ifndef LV_USE_NXP_ELCDIF
    #ifdef CONFIG_LV_USE_NXP_ELCDIF
        #define LV_USE_NXP_ELCDIF CONFIG_LV_USE_NXP_ELCDIF
    #else
        #define LV_USE_NXP_ELCDIF 0
    #endif
#endif

/** Driver for Renesas GLCD */
#ifndef LV_USE_RENESAS_GLCDC
    #ifdef CONFIG_LV_USE_RENESAS_GLCDC
        #define LV_USE_RENESAS_GLCDC CONFIG_LV_USE_RENESAS_GLCDC
    #else
        #define LV_USE_RENESAS_GLCDC 0
    #endif
#endif

/** Driver for ST LTDC */
#ifndef LV_USE_ST_LTDC
    #ifdef CONFIG_LV_USE_ST_LTDC
        #define LV_USE_ST_LTDC CONFIG_LV_USE_ST_LTDC
    #else
        #define LV_USE_ST_LTDC 0
    #endif
#endif

#if !LV_USE_DRAW_DMA2D
#if LV_USE_ST_LTDC
/** Only used for created partial mode LTDC displays */
#ifndef LV_ST_LTDC_USE_DMA2D_FLUSH
    #ifdef CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
        #define LV_ST_LTDC_USE_DMA2D_FLUSH CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
    #else
        #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
    #endif
#endif

#endif /*LV_USE_ST_LTDC*/
#endif /*!LV_USE_DRAW_DMA2D*/

/** Interface for TFT_eSPI */
#ifndef LV_USE_TFT_ESPI
    #ifdef CONFIG_LV_USE_TFT_ESPI
        #define LV_USE_TFT_ESPI CONFIG_LV_USE_TFT_ESPI
    #else
        #define LV_USE_TFT_ESPI 0
    #endif
#endif

/** Driver for evdev input devices */
#ifndef LV_USE_EVDEV
    #ifdef CONFIG_LV_USE_EVDEV
        #define LV_USE_EVDEV CONFIG_LV_USE_EVDEV
    #else
        #define LV_USE_EVDEV 0
    #endif
#endif

/** Driver for libinput input devices */
#ifndef LV_USE_LIBINPUT
    #ifdef CONFIG_LV_USE_LIBINPUT
        #define LV_USE_LIBINPUT CONFIG_LV_USE_LIBINPUT
    #else
        #define LV_USE_LIBINPUT 0
    #endif
#endif

#if LV_USE_LIBINPUT
/** Use the BSD variant of the libinput input driver */
#ifndef LV_LIBINPUT_BSD
    #ifdef CONFIG_LV_LIBINPUT_BSD
        #define LV_LIBINPUT_BSD CONFIG_LV_LIBINPUT_BSD
    #else
        #define LV_LIBINPUT_BSD 0
    #endif
#endif

/** Enable full keyboard support via XKB */
#ifndef LV_LIBINPUT_XKB
    #ifdef CONFIG_LV_LIBINPUT_XKB
        #define LV_LIBINPUT_XKB CONFIG_LV_LIBINPUT_XKB
    #else
        #define LV_LIBINPUT_XKB 0
    #endif
#endif

#if LV_LIBINPUT_XKB
/** XKB rules */
#ifndef LV_LIBINPUT_XKB_RULES
    #ifdef CONFIG_LV_LIBINPUT_XKB_RULES
        #define LV_LIBINPUT_XKB_RULES CONFIG_LV_LIBINPUT_XKB_RULES
    #else
        #define LV_LIBINPUT_XKB_RULES ""
    #endif
#endif

/** XKB model */
#ifndef LV_LIBINPUT_XKB_MODEL
    #ifdef CONFIG_LV_LIBINPUT_XKB_MODEL
        #define LV_LIBINPUT_XKB_MODEL CONFIG_LV_LIBINPUT_XKB_MODEL
    #else
        #define LV_LIBINPUT_XKB_MODEL "pc101"
    #endif
#endif

/** XKB layout */
#ifndef LV_LIBINPUT_XKB_LAYOUT
    #ifdef CONFIG_LV_LIBINPUT_XKB_LAYOUT
        #define LV_LIBINPUT_XKB_LAYOUT CONFIG_LV_LIBINPUT_XKB_LAYOUT
    #else
        #define LV_LIBINPUT_XKB_LAYOUT "us"
    #endif
#endif

/** XKB variant */
#ifndef LV_LIBINPUT_XKB_VARIANT
    #ifdef CONFIG_LV_LIBINPUT_XKB_VARIANT
        #define LV_LIBINPUT_XKB_VARIANT CONFIG_LV_LIBINPUT_XKB_VARIANT
    #else
        #define LV_LIBINPUT_XKB_VARIANT ""
    #endif
#endif

/** If enabled, NULL is passed for XKB options, letting the system
 *  default apply (e.g. from XKB_DEFAULT_OPTIONS env var).
 *  If disabled, the string from LV_LIBINPUT_XKB_OPTIONS is used instead.
 */
#ifndef LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
            #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT CONFIG_LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
        #else
            #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT 0
        #endif
    #else
        #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT 1
    #endif
#endif

#if !LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
/** XKB options */
#ifndef LV_LIBINPUT_XKB_OPTIONS
    #ifdef CONFIG_LV_LIBINPUT_XKB_OPTIONS
        #define LV_LIBINPUT_XKB_OPTIONS CONFIG_LV_LIBINPUT_XKB_OPTIONS
    #else
        #define LV_LIBINPUT_XKB_OPTIONS ""
    #endif
#endif

#endif /*!LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT*/
#endif /*LV_LIBINPUT_XKB*/
#endif /*LV_USE_LIBINPUT*/

/** Use Nuttx to open window and handle touchscreen */
#ifndef LV_USE_NUTTX
    #ifdef CONFIG_LV_USE_NUTTX
        #define LV_USE_NUTTX CONFIG_LV_USE_NUTTX
    #else
        #define LV_USE_NUTTX 0
    #endif
#endif

#if LV_USE_NUTTX
/** Use independent image heap */
#ifndef LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
    #ifdef CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
    #else
        #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0
    #endif
#endif

#if LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
/** Use independent image heap for default draw buffer */
#ifndef LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    #ifdef CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
        #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    #else
        #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP 0
    #endif
#endif

#endif /*LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP*/

/** Use uv loop to replace default timer loop and other fb/indev timers */
#ifndef LV_USE_NUTTX_LIBUV
    #ifdef CONFIG_LV_USE_NUTTX_LIBUV
        #define LV_USE_NUTTX_LIBUV CONFIG_LV_USE_NUTTX_LIBUV
    #else
        #define LV_USE_NUTTX_LIBUV 0
    #endif
#endif

/** Use Custom Nuttx init API to open window and handle touchscreen */
#ifndef LV_USE_NUTTX_CUSTOM_INIT
    #ifdef CONFIG_LV_USE_NUTTX_CUSTOM_INIT
        #define LV_USE_NUTTX_CUSTOM_INIT CONFIG_LV_USE_NUTTX_CUSTOM_INIT
    #else
        #define LV_USE_NUTTX_CUSTOM_INIT 0
    #endif
#endif

/** Use NuttX LCD device */
#ifndef LV_USE_NUTTX_LCD
    #ifdef CONFIG_LV_USE_NUTTX_LCD
        #define LV_USE_NUTTX_LCD CONFIG_LV_USE_NUTTX_LCD
    #else
        #define LV_USE_NUTTX_LCD 0
    #endif
#endif

#ifndef LV_NUTTX_LCD_BUFFER_COUNT
    #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
        #define LV_NUTTX_LCD_BUFFER_COUNT CONFIG_LV_NUTTX_LCD_BUFFER_COUNT
    #else
        #define LV_NUTTX_LCD_BUFFER_COUNT 0
    #endif
#endif

#if LV_NUTTX_LCD_CUSTOM_BUFFER
#if LV_USE_NUTTX_LCD
/** Custom partial buffer size (in number of rows) */
#ifndef LV_NUTTX_LCD_BUFFER_SIZE
    #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
        #define LV_NUTTX_LCD_BUFFER_SIZE CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
    #else
        #define LV_NUTTX_LCD_BUFFER_SIZE 60
    #endif
#endif

#endif /*LV_USE_NUTTX_LCD*/
#endif /*LV_NUTTX_LCD_CUSTOM_BUFFER*/

/** Use NuttX touchscreen driver */
#ifndef LV_USE_NUTTX_TOUCHSCREEN
    #ifdef CONFIG_LV_USE_NUTTX_TOUCHSCREEN
        #define LV_USE_NUTTX_TOUCHSCREEN CONFIG_LV_USE_NUTTX_TOUCHSCREEN
    #else
        #define LV_USE_NUTTX_TOUCHSCREEN 0
    #endif
#endif

#if LV_USE_NUTTX_TOUCHSCREEN
/** Set to 0 to disable cursor, or set to a value greater than 0 to set the cursor size in pixels. */
#ifndef LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
    #ifdef CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
    #else
        #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE 0
    #endif
#endif

#endif /*LV_USE_NUTTX_TOUCHSCREEN*/

/** Use NuttX mouse driver */
#ifndef LV_USE_NUTTX_MOUSE
    #ifdef CONFIG_LV_USE_NUTTX_MOUSE
        #define LV_USE_NUTTX_MOUSE CONFIG_LV_USE_NUTTX_MOUSE
    #else
        #define LV_USE_NUTTX_MOUSE 0
    #endif
#endif

#if LV_USE_NUTTX_MOUSE
/** Set the step size of the mouse movement in pixels. */
#ifndef LV_USE_NUTTX_MOUSE_MOVE_STEP
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
            #define LV_USE_NUTTX_MOUSE_MOVE_STEP CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
        #else
            #define LV_USE_NUTTX_MOUSE_MOVE_STEP 0
        #endif
    #else
        #define LV_USE_NUTTX_MOUSE_MOVE_STEP 1
    #endif
#endif

#endif /*LV_USE_NUTTX_MOUSE*/

#if LV_USE_PROFILER_BUILTIN
/** Use NuttX trace file */
#ifndef LV_USE_NUTTX_TRACE_FILE
    #ifdef CONFIG_LV_USE_NUTTX_TRACE_FILE
        #define LV_USE_NUTTX_TRACE_FILE CONFIG_LV_USE_NUTTX_TRACE_FILE
    #else
        #define LV_USE_NUTTX_TRACE_FILE 0
    #endif
#endif

#endif /*LV_USE_PROFILER_BUILTIN*/

#if LV_USE_NUTTX_TRACE_FILE
/** NuttX trace file path */
#ifndef LV_NUTTX_TRACE_FILE_PATH
    #ifdef CONFIG_LV_NUTTX_TRACE_FILE_PATH
        #define LV_NUTTX_TRACE_FILE_PATH CONFIG_LV_NUTTX_TRACE_FILE_PATH
    #else
        #define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"
    #endif
#endif

#endif /*LV_USE_NUTTX_TRACE_FILE*/
#endif /*LV_USE_NUTTX*/

/** Use a generic OpenGL driver that can be used to embed in other applications or used with GLFW/EGL
 *  - Requires LV_USE_MATRIX.
 */
#ifndef LV_USE_OPENGLES
    #ifdef CONFIG_LV_USE_OPENGLES
        #define LV_USE_OPENGLES CONFIG_LV_USE_OPENGLES
    #else
        #define LV_USE_OPENGLES 0
    #endif
#endif

#if LV_USE_OPENGLES
/** Enable debug mode for OpenGL */
#ifndef LV_USE_OPENGLES_DEBUG
    #ifdef CONFIG_LV_USE_OPENGLES_DEBUG
        #define LV_USE_OPENGLES_DEBUG CONFIG_LV_USE_OPENGLES_DEBUG
    #else
        #define LV_USE_OPENGLES_DEBUG 0
    #endif
#endif

#endif /*LV_USE_OPENGLES*/

#if !LV_USE_EGL
/** Use GLFW to open window on PC and handle mouse and keyboard. */
#ifndef LV_USE_GLFW
    #ifdef CONFIG_LV_USE_GLFW
        #define LV_USE_GLFW CONFIG_LV_USE_GLFW
    #else
        #define LV_USE_GLFW 0
    #endif
#endif

#endif /*!LV_USE_EGL*/

/** QNX Screen display and input drivers */
#ifndef LV_USE_QNX
    #ifdef CONFIG_LV_USE_QNX
        #define LV_USE_QNX CONFIG_LV_USE_QNX
    #else
        #define LV_USE_QNX 0
    #endif
#endif

#if LV_USE_QNX
/** QNX Buffer count */
#ifndef LV_QNX_BUF_COUNT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_QNX_BUF_COUNT
            #define LV_QNX_BUF_COUNT CONFIG_LV_QNX_BUF_COUNT
        #else
            #define LV_QNX_BUF_COUNT 0
        #endif
    #else
        #define LV_QNX_BUF_COUNT 1
    #endif
#endif

#endif /*LV_USE_QNX*/

/** Use SDL to open window on PC and handle mouse and keyboard. */
#ifndef LV_USE_SDL
    #ifdef CONFIG_LV_USE_SDL
        #define LV_USE_SDL CONFIG_LV_USE_SDL
    #else
        #define LV_USE_SDL 0
    #endif
#endif

#if LV_USE_SDL
/** SDL include path */
#ifndef LV_SDL_INCLUDE_PATH
    #ifdef CONFIG_LV_SDL_INCLUDE_PATH
        #define LV_SDL_INCLUDE_PATH CONFIG_LV_SDL_INCLUDE_PATH
    #else
        #define LV_SDL_INCLUDE_PATH "SDL2/SDL.h"
    #endif
#endif

/** LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Use the buffer(s) to render the screen is smaller parts
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Only the changed areas will be updated with 2 screen sized buffers
 *  - LV_DISPLAY_RENDER_MODE_FULL: Always redraw the whole screen even if only one pixel has been changed with 2 screen sized buffers
 */
#ifndef LV_SDL_RENDER_MODE
    #ifdef CONFIG_LV_SDL_RENDER_MODE
        #define LV_SDL_RENDER_MODE CONFIG_LV_SDL_RENDER_MODE
    #else
        #define LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT
    #endif
#endif

#ifndef LV_SDL_BUF_COUNT
    #ifdef CONFIG_LV_SDL_BUF_COUNT
        #define LV_SDL_BUF_COUNT CONFIG_LV_SDL_BUF_COUNT
    #else
        #define LV_SDL_BUF_COUNT 0
    #endif
#endif

/** Use hardware acceleration */
#ifndef LV_SDL_ACCELERATED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_SDL_ACCELERATED
            #define LV_SDL_ACCELERATED CONFIG_LV_SDL_ACCELERATED
        #else
            #define LV_SDL_ACCELERATED 0
        #endif
    #else
        #define LV_SDL_ACCELERATED 1
    #endif
#endif

/** SDL fullscreen */
#ifndef LV_SDL_FULLSCREEN
    #ifdef CONFIG_LV_SDL_FULLSCREEN
        #define LV_SDL_FULLSCREEN CONFIG_LV_SDL_FULLSCREEN
    #else
        #define LV_SDL_FULLSCREEN 0
    #endif
#endif

/** Exit the application when all SDL windows are closed */
#ifndef LV_SDL_DIRECT_EXIT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_SDL_DIRECT_EXIT
            #define LV_SDL_DIRECT_EXIT CONFIG_LV_SDL_DIRECT_EXIT
        #else
            #define LV_SDL_DIRECT_EXIT 0
        #endif
    #else
        #define LV_SDL_DIRECT_EXIT 1
    #endif
#endif

/** SDL mousewheel mode
 *  Possible values:
 *  - LV_SDL_MOUSEWHEEL_MODE_ENCODER: The mousewheel emulates an encoder input device
 *  - LV_SDL_MOUSEWHEEL_MODE_CROWN: The mousewheel emulates a smart watch crown
 */
#ifndef LV_SDL_MOUSEWHEEL_MODE
    #ifdef CONFIG_LV_SDL_MOUSEWHEEL_MODE
        #define LV_SDL_MOUSEWHEEL_MODE CONFIG_LV_SDL_MOUSEWHEEL_MODE
    #else
        #define LV_SDL_MOUSEWHEEL_MODE LV_SDL_MOUSEWHEEL_MODE_ENCODER
    #endif
#endif

#endif /*LV_USE_SDL*/

/** LVGL UEFI backend */
#ifndef LV_USE_UEFI
    #ifdef CONFIG_LV_USE_UEFI
        #define LV_USE_UEFI CONFIG_LV_USE_UEFI
    #else
        #define LV_USE_UEFI 0
    #endif
#endif

#if LV_USE_UEFI
/** Header that hides the actual framework (EDK2, gnu-efi, ...) */
#ifndef LV_USE_UEFI_INCLUDE
    #ifdef CONFIG_LV_USE_UEFI_INCLUDE
        #define LV_USE_UEFI_INCLUDE CONFIG_LV_USE_UEFI_INCLUDE
    #else
        #define LV_USE_UEFI_INCLUDE "myefi.h"
    #endif
#endif

/** Use the memory services from the boot services table */
#ifndef LV_UEFI_USE_MEMORY_SERVICES
    #ifdef CONFIG_LV_UEFI_USE_MEMORY_SERVICES
        #define LV_UEFI_USE_MEMORY_SERVICES CONFIG_LV_UEFI_USE_MEMORY_SERVICES
    #else
        #define LV_UEFI_USE_MEMORY_SERVICES 0
    #endif
#endif

#endif /*LV_USE_UEFI*/

/** Use Wayland to open a window and handle input on Linux or BSD desktops */
#ifndef LV_USE_WAYLAND
    #ifdef CONFIG_LV_USE_WAYLAND
        #define LV_USE_WAYLAND CONFIG_LV_USE_WAYLAND
    #else
        #define LV_USE_WAYLAND 0
    #endif
#endif

#if LV_USE_WAYLAND
/** Deinitialize LVGL and quit the application when the last wayland window closes */
#ifndef LV_WAYLAND_DIRECT_EXIT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WAYLAND_DIRECT_EXIT
            #define LV_WAYLAND_DIRECT_EXIT CONFIG_LV_WAYLAND_DIRECT_EXIT
        #else
            #define LV_WAYLAND_DIRECT_EXIT 0
        #endif
    #else
        #define LV_WAYLAND_DIRECT_EXIT 1
    #endif
#endif

#endif /*LV_USE_WAYLAND*/

#if LV_USE_OS == LV_OS_WINDOWS
/** LVGL Windows backend */
#ifndef LV_USE_WINDOWS
    #ifdef CONFIG_LV_USE_WINDOWS
        #define LV_USE_WINDOWS CONFIG_LV_USE_WINDOWS
    #else
        #define LV_USE_WINDOWS 0
    #endif
#endif

#endif /*LV_USE_OS == LV_OS_WINDOWS*/

/** Use X11 to open window on Linux desktop and handle mouse and keyboard */
#ifndef LV_USE_X11
    #ifdef CONFIG_LV_USE_X11
        #define LV_USE_X11 CONFIG_LV_USE_X11
    #else
        #define LV_USE_X11 0
    #endif
#endif

#if LV_USE_X11
/** Use double buffers for lvgl rendering */
#ifndef LV_X11_DOUBLE_BUFFER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_X11_DOUBLE_BUFFER
            #define LV_X11_DOUBLE_BUFFER CONFIG_LV_X11_DOUBLE_BUFFER
        #else
            #define LV_X11_DOUBLE_BUFFER 0
        #endif
    #else
        #define LV_X11_DOUBLE_BUFFER 1
    #endif
#endif

/** Exit the application when all X11 windows have been closed */
#ifndef LV_X11_DIRECT_EXIT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_X11_DIRECT_EXIT
            #define LV_X11_DIRECT_EXIT CONFIG_LV_X11_DIRECT_EXIT
        #else
            #define LV_X11_DIRECT_EXIT 0
        #endif
    #else
        #define LV_X11_DIRECT_EXIT 1
    #endif
#endif

#endif /*LV_USE_X11*/

/** Use the buffer(s) to render the screen is smaller parts. This way the buffers can be smaller then the display to save RAM.
 *  Appr. 1/10 screen size buffer(s) are used.
 */
#ifndef LV_X11_RENDER_MODE_PARTIAL
    #ifdef CONFIG_LV_X11_RENDER_MODE_PARTIAL
        #define LV_X11_RENDER_MODE_PARTIAL CONFIG_LV_X11_RENDER_MODE_PARTIAL
    #else
        #define LV_X11_RENDER_MODE_PARTIAL 0
    #endif
#endif

/** The buffer(s) has to be screen sized and LVGL will render into the correct location of the buffer. This way the buffer always contain the whole image. Only the changed ares will be updated.
 *  With 2 buffers the buffers' content are kept in sync automatically and in flush_cb only address change is required.
 */
#ifndef LV_X11_RENDER_MODE_DIRECT
    #ifdef CONFIG_LV_X11_RENDER_MODE_DIRECT
        #define LV_X11_RENDER_MODE_DIRECT CONFIG_LV_X11_RENDER_MODE_DIRECT
    #else
        #define LV_X11_RENDER_MODE_DIRECT 0
    #endif
#endif

/** Always redraw the whole screen even if only one pixel has been changed.
 *  With 2 buffers in flush_cb only and address change is required.
 */
#ifndef LV_X11_RENDER_MODE_FULL
    #ifdef CONFIG_LV_X11_RENDER_MODE_FULL
        #define LV_X11_RENDER_MODE_FULL CONFIG_LV_X11_RENDER_MODE_FULL
    #else
        #define LV_X11_RENDER_MODE_FULL 0
    #endif
#endif

/** Enable NanoVG (vector graphics library) */
#ifndef LV_USE_NANOVG
    #ifdef CONFIG_LV_USE_NANOVG
        #define LV_USE_NANOVG CONFIG_LV_USE_NANOVG
    #else
        #define LV_USE_NANOVG 0
    #endif
#endif



/*============================================================================
 * FILE SYSTEM
 *============================================================================*/

/** Setting a default drive letter allows skipping the driver
 *  prefix in filepaths.
 */
#ifndef LV_FS_DEFAULT_DRIVER_LETTER
    #ifdef CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
        #define LV_FS_DEFAULT_DRIVER_LETTER CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
    #else
        #define LV_FS_DEFAULT_DRIVER_LETTER 0
    #endif
#endif

/** File system on top of stdio */
#ifndef LV_USE_FS_STDIO
    #ifdef CONFIG_LV_USE_FS_STDIO
        #define LV_USE_FS_STDIO CONFIG_LV_USE_FS_STDIO
    #else
        #define LV_USE_FS_STDIO 0
    #endif
#endif

#if LV_USE_FS_STDIO
/** Driver-identifier letter for stdio (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_STDIO_LETTER
    #ifdef CONFIG_LV_FS_STDIO_LETTER
        #define LV_FS_STDIO_LETTER CONFIG_LV_FS_STDIO_LETTER
    #else
        #define LV_FS_STDIO_LETTER 0
    #endif
#endif

/** Working directory for stdio */
#ifndef LV_FS_STDIO_PATH
    #ifdef CONFIG_LV_FS_STDIO_PATH
        #define LV_FS_STDIO_PATH CONFIG_LV_FS_STDIO_PATH
    #else
        #define LV_FS_STDIO_PATH ""
    #endif
#endif

/** Read cache size in bytes for stdio (0 = disabled) */
#ifndef LV_FS_STDIO_CACHE_SIZE
    #ifdef CONFIG_LV_FS_STDIO_CACHE_SIZE
        #define LV_FS_STDIO_CACHE_SIZE CONFIG_LV_FS_STDIO_CACHE_SIZE
    #else
        #define LV_FS_STDIO_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_STDIO*/

/** File system on top of POSIX */
#ifndef LV_USE_FS_POSIX
    #ifdef CONFIG_LV_USE_FS_POSIX
        #define LV_USE_FS_POSIX CONFIG_LV_USE_FS_POSIX
    #else
        #define LV_USE_FS_POSIX 0
    #endif
#endif

#if LV_USE_FS_POSIX
/** Driver-identifier letter for POSIX (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_POSIX_LETTER
    #ifdef CONFIG_LV_FS_POSIX_LETTER
        #define LV_FS_POSIX_LETTER CONFIG_LV_FS_POSIX_LETTER
    #else
        #define LV_FS_POSIX_LETTER 0
    #endif
#endif

/** Working directory for POSIX */
#ifndef LV_FS_POSIX_PATH
    #ifdef CONFIG_LV_FS_POSIX_PATH
        #define LV_FS_POSIX_PATH CONFIG_LV_FS_POSIX_PATH
    #else
        #define LV_FS_POSIX_PATH ""
    #endif
#endif

/** Read cache size in bytes for POSIX (0 = disabled) */
#ifndef LV_FS_POSIX_CACHE_SIZE
    #ifdef CONFIG_LV_FS_POSIX_CACHE_SIZE
        #define LV_FS_POSIX_CACHE_SIZE CONFIG_LV_FS_POSIX_CACHE_SIZE
    #else
        #define LV_FS_POSIX_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_POSIX*/

/** File system on top of Win32 */
#ifndef LV_USE_FS_WIN32
    #ifdef CONFIG_LV_USE_FS_WIN32
        #define LV_USE_FS_WIN32 CONFIG_LV_USE_FS_WIN32
    #else
        #define LV_USE_FS_WIN32 0
    #endif
#endif

#if LV_USE_FS_WIN32
/** Driver-identifier letter for Win32 (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_WIN32_LETTER
    #ifdef CONFIG_LV_FS_WIN32_LETTER
        #define LV_FS_WIN32_LETTER CONFIG_LV_FS_WIN32_LETTER
    #else
        #define LV_FS_WIN32_LETTER 0
    #endif
#endif

/** Working directory for Win32 */
#ifndef LV_FS_WIN32_PATH
    #ifdef CONFIG_LV_FS_WIN32_PATH
        #define LV_FS_WIN32_PATH CONFIG_LV_FS_WIN32_PATH
    #else
        #define LV_FS_WIN32_PATH ""
    #endif
#endif

/** Read cache size in bytes for Win32 (0 = disabled) */
#ifndef LV_FS_WIN32_CACHE_SIZE
    #ifdef CONFIG_LV_FS_WIN32_CACHE_SIZE
        #define LV_FS_WIN32_CACHE_SIZE CONFIG_LV_FS_WIN32_CACHE_SIZE
    #else
        #define LV_FS_WIN32_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_WIN32*/

/** File system on top of FatFS */
#ifndef LV_USE_FS_FATFS
    #ifdef CONFIG_LV_USE_FS_FATFS
        #define LV_USE_FS_FATFS CONFIG_LV_USE_FS_FATFS
    #else
        #define LV_USE_FS_FATFS 0
    #endif
#endif

#if LV_USE_FS_FATFS
/** Driver-identifier letter for FatFS (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_FATFS_LETTER
    #ifdef CONFIG_LV_FS_FATFS_LETTER
        #define LV_FS_FATFS_LETTER CONFIG_LV_FS_FATFS_LETTER
    #else
        #define LV_FS_FATFS_LETTER 0
    #endif
#endif

/** Working directory for FatFS */
#ifndef LV_FS_FATFS_PATH
    #ifdef CONFIG_LV_FS_FATFS_PATH
        #define LV_FS_FATFS_PATH CONFIG_LV_FS_FATFS_PATH
    #else
        #define LV_FS_FATFS_PATH ""
    #endif
#endif

/** Read cache size in bytes for FatFS (0 = disabled) */
#ifndef LV_FS_FATFS_CACHE_SIZE
    #ifdef CONFIG_LV_FS_FATFS_CACHE_SIZE
        #define LV_FS_FATFS_CACHE_SIZE CONFIG_LV_FS_FATFS_CACHE_SIZE
    #else
        #define LV_FS_FATFS_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_FATFS*/

/** File system on top of littlefs */
#ifndef LV_USE_FS_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_LITTLEFS
        #define LV_USE_FS_LITTLEFS CONFIG_LV_USE_FS_LITTLEFS
    #else
        #define LV_USE_FS_LITTLEFS 0
    #endif
#endif

#if LV_USE_FS_LITTLEFS
/** Driver-identifier letter for littlefs (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_LITTLEFS_LETTER
    #ifdef CONFIG_LV_FS_LITTLEFS_LETTER
        #define LV_FS_LITTLEFS_LETTER CONFIG_LV_FS_LITTLEFS_LETTER
    #else
        #define LV_FS_LITTLEFS_LETTER 0
    #endif
#endif

/** Working directory for littlefs */
#ifndef LV_FS_LITTLEFS_PATH
    #ifdef CONFIG_LV_FS_LITTLEFS_PATH
        #define LV_FS_LITTLEFS_PATH CONFIG_LV_FS_LITTLEFS_PATH
    #else
        #define LV_FS_LITTLEFS_PATH ""
    #endif
#endif

/** Read cache size in bytes for littlefs (0 = disabled) */
#ifndef LV_FS_LITTLEFS_CACHE_SIZE
    #ifdef CONFIG_LV_FS_LITTLEFS_CACHE_SIZE
        #define LV_FS_LITTLEFS_CACHE_SIZE CONFIG_LV_FS_LITTLEFS_CACHE_SIZE
    #else
        #define LV_FS_LITTLEFS_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_LITTLEFS*/

/** File system on top of Arduino ESP littlefs */
#ifndef LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #else
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
    #endif
#endif

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
/** Driver-identifier letter for Arduino ESP littlefs (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
    #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
    #else
        #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER 0
    #endif
#endif

/** Working directory for Arduino ESP littlefs */
#ifndef LV_FS_ARDUINO_ESP_LITTLEFS_PATH
    #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
    #else
        #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""
    #endif
#endif

/** Read cache size in bytes for Arduino ESP littlefs (0 = disabled) */
#ifndef LV_FS_ARDUINO_ESP_LITTLEFS_CACHE_SIZE
    #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_CACHE_SIZE
        #define LV_FS_ARDUINO_ESP_LITTLEFS_CACHE_SIZE CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_CACHE_SIZE
    #else
        #define LV_FS_ARDUINO_ESP_LITTLEFS_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_ARDUINO_ESP_LITTLEFS*/

/** File system on top of Arduino SD */
#ifndef LV_USE_FS_ARDUINO_SD
    #ifdef CONFIG_LV_USE_FS_ARDUINO_SD
        #define LV_USE_FS_ARDUINO_SD CONFIG_LV_USE_FS_ARDUINO_SD
    #else
        #define LV_USE_FS_ARDUINO_SD 0
    #endif
#endif

#if LV_USE_FS_ARDUINO_SD
/** Driver-identifier letter for Arduino SD (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_ARDUINO_SD_LETTER
    #ifdef CONFIG_LV_FS_ARDUINO_SD_LETTER
        #define LV_FS_ARDUINO_SD_LETTER CONFIG_LV_FS_ARDUINO_SD_LETTER
    #else
        #define LV_FS_ARDUINO_SD_LETTER 0
    #endif
#endif

/** Working directory for Arduino SD */
#ifndef LV_FS_ARDUINO_SD_PATH
    #ifdef CONFIG_LV_FS_ARDUINO_SD_PATH
        #define LV_FS_ARDUINO_SD_PATH CONFIG_LV_FS_ARDUINO_SD_PATH
    #else
        #define LV_FS_ARDUINO_SD_PATH ""
    #endif
#endif

/** Read cache size in bytes for Arduino SD (0 = disabled) */
#ifndef LV_FS_ARDUINO_SD_CACHE_SIZE
    #ifdef CONFIG_LV_FS_ARDUINO_SD_CACHE_SIZE
        #define LV_FS_ARDUINO_SD_CACHE_SIZE CONFIG_LV_FS_ARDUINO_SD_CACHE_SIZE
    #else
        #define LV_FS_ARDUINO_SD_CACHE_SIZE 0
    #endif
#endif

#endif /*LV_USE_FS_ARDUINO_SD*/

/** File system on top of UEFI */
#ifndef LV_USE_FS_UEFI
    #ifdef CONFIG_LV_USE_FS_UEFI
        #define LV_USE_FS_UEFI CONFIG_LV_USE_FS_UEFI
    #else
        #define LV_USE_FS_UEFI 0
    #endif
#endif

#if LV_USE_FS_UEFI
/** Driver-identifier letter for UEFI (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_UEFI_LETTER
    #ifdef CONFIG_LV_FS_UEFI_LETTER
        #define LV_FS_UEFI_LETTER CONFIG_LV_FS_UEFI_LETTER
    #else
        #define LV_FS_UEFI_LETTER 0
    #endif
#endif

#endif /*LV_USE_FS_UEFI*/

/** File system on top of FrogFS */
#ifndef LV_USE_FS_FROGFS
    #ifdef CONFIG_LV_USE_FS_FROGFS
        #define LV_USE_FS_FROGFS CONFIG_LV_USE_FS_FROGFS
    #else
        #define LV_USE_FS_FROGFS 0
    #endif
#endif

#if LV_USE_FS_FROGFS
/** Driver-identifier letter for FrogFS (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_FROGFS_LETTER
    #ifdef CONFIG_LV_FS_FROGFS_LETTER
        #define LV_FS_FROGFS_LETTER CONFIG_LV_FS_FROGFS_LETTER
    #else
        #define LV_FS_FROGFS_LETTER 0
    #endif
#endif

#endif /*LV_USE_FS_FROGFS*/

/** API for memory-mapped file access. */
#ifndef LV_USE_FS_MEMFS
    #ifdef CONFIG_LV_USE_FS_MEMFS
        #define LV_USE_FS_MEMFS CONFIG_LV_USE_FS_MEMFS
    #else
        #define LV_USE_FS_MEMFS 0
    #endif
#endif

#if LV_USE_FS_MEMFS
/** Driver-identifier letter for memfs (e.g. 65 for 'A', 0 = disabled) */
#ifndef LV_FS_MEMFS_LETTER
    #ifdef CONFIG_LV_FS_MEMFS_LETTER
        #define LV_FS_MEMFS_LETTER CONFIG_LV_FS_MEMFS_LETTER
    #else
        #define LV_FS_MEMFS_LETTER 0
    #endif
#endif

#endif /*LV_USE_FS_MEMFS*/


/*============================================================================
 * DEBUGGING
 *============================================================================*/

/** Enable system monitor component */
#ifndef LV_USE_SYSMON
    #ifdef CONFIG_LV_USE_SYSMON
        #define LV_USE_SYSMON CONFIG_LV_USE_SYSMON
    #else
        #define LV_USE_SYSMON 0
    #endif
#endif

#if LV_USE_SYSMON
/** Enable process idle measurement */
#ifndef LV_SYSMON_PROC_IDLE_AVAILABLE
    #ifdef CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
        #define LV_SYSMON_PROC_IDLE_AVAILABLE CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
    #else
        #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
    #endif
#endif

/** Show CPU usage and FPS count */
#ifndef LV_USE_PERF_MONITOR
    #ifdef CONFIG_LV_USE_PERF_MONITOR
        #define LV_USE_PERF_MONITOR CONFIG_LV_USE_PERF_MONITOR
    #else
        #define LV_USE_PERF_MONITOR 0
    #endif
#endif

#if LV_USE_PERF_MONITOR
/** Performance monitor position
 *  Possible values:
 *  - LV_ALIGN_TOP_LEFT
 *  - LV_ALIGN_TOP_MID: Top middle
 *  - LV_ALIGN_TOP_RIGHT
 *  - LV_ALIGN_BOTTOM_LEFT
 *  - LV_ALIGN_BOTTOM_MID: Bottom middle
 *  - LV_ALIGN_BOTTOM_RIGHT
 *  - LV_ALIGN_LEFT_MID: Left middle
 *  - LV_ALIGN_RIGHT_MID: Right middle
 *  - LV_ALIGN_CENTER
 */
#ifndef LV_USE_PERF_MONITOR_POS
    #ifdef CONFIG_LV_USE_PERF_MONITOR_POS
        #define LV_USE_PERF_MONITOR_POS CONFIG_LV_USE_PERF_MONITOR_POS
    #else
        #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
    #endif
#endif

/** Prints performance data using log */
#ifndef LV_USE_PERF_MONITOR_LOG_MODE
    #ifdef CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
        #define LV_USE_PERF_MONITOR_LOG_MODE CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
    #else
        #define LV_USE_PERF_MONITOR_LOG_MODE 0
    #endif
#endif

#endif /*LV_USE_PERF_MONITOR*/

#if LV_USE_BUILTIN_MALLOC
/** Show the used memory and the memory fragmentation */
#ifndef LV_USE_MEM_MONITOR
    #ifdef CONFIG_LV_USE_MEM_MONITOR
        #define LV_USE_MEM_MONITOR CONFIG_LV_USE_MEM_MONITOR
    #else
        #define LV_USE_MEM_MONITOR 0
    #endif
#endif

#endif /*LV_USE_BUILTIN_MALLOC*/

#if LV_USE_MEM_MONITOR
/** Memory monitor position
 *  Possible values:
 *  - LV_ALIGN_TOP_LEFT
 *  - LV_ALIGN_TOP_MID: Top middle
 *  - LV_ALIGN_TOP_RIGHT
 *  - LV_ALIGN_BOTTOM_LEFT
 *  - LV_ALIGN_BOTTOM_MID: Bottom middle
 *  - LV_ALIGN_BOTTOM_RIGHT
 *  - LV_ALIGN_LEFT_MID: Left middle
 *  - LV_ALIGN_RIGHT_MID: Right middle
 *  - LV_ALIGN_CENTER
 */
#ifndef LV_USE_MEM_MONITOR_POS
    #ifdef CONFIG_LV_USE_MEM_MONITOR_POS
        #define LV_USE_MEM_MONITOR_POS CONFIG_LV_USE_MEM_MONITOR_POS
    #else
        #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
    #endif
#endif

#endif /*LV_USE_MEM_MONITOR*/

/** Optional header to override system monitor idle measurement macros.
 *
 *  LV_SYSMON_GET_IDLE
 *    Expression returning the CPU idle percentage (0-100) since last call.
 *    e.g. my_rtos_get_idle_percent()
 *
 *  LV_SYSMON_GET_PROC_IDLE
 *    Expression returning per-process idle percentage, if supported by
 *    the RTOS. Requires LV_SYSMON_PROC_IDLE_AVAILABLE to be enabled
 *    e.g. my_rtos_get_proc_idle_percent()
 */
#ifndef LV_SYSMON_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_SYSMON_CUSTOM_INCLUDE
        #define LV_SYSMON_CUSTOM_INCLUDE CONFIG_LV_SYSMON_CUSTOM_INCLUDE
    #else
        #define LV_SYSMON_CUSTOM_INCLUDE ""
    #endif
#endif

#endif /*LV_USE_SYSMON*/

/** Enable runtime performance profiler */
#ifndef LV_USE_PROFILER
    #ifdef CONFIG_LV_USE_PROFILER
        #define LV_USE_PROFILER CONFIG_LV_USE_PROFILER
    #else
        #define LV_USE_PROFILER 0
    #endif
#endif

/** Enable the built-in profiler */
#ifndef LV_USE_PROFILER_BUILTIN
    #ifdef CONFIG_LV_USE_PROFILER_BUILTIN
        #define LV_USE_PROFILER_BUILTIN CONFIG_LV_USE_PROFILER_BUILTIN
    #else
        #define LV_USE_PROFILER_BUILTIN 0
    #endif
#endif

#if LV_USE_PROFILER_BUILTIN
/** Default profiler trace buffer size in bytes */
#ifndef LV_PROFILER_BUILTIN_BUF_SIZE
    #ifdef CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
        #define LV_PROFILER_BUILTIN_BUF_SIZE CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
    #else
        #define LV_PROFILER_BUILTIN_BUF_SIZE 16384
    #endif
#endif

/** Enable built-in profiler by default */
#ifndef LV_PROFILER_BUILTIN_DEFAULT_ENABLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
            #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
        #else
            #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 0
        #endif
    #else
        #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1
    #endif
#endif

/** Enable POSIX profiler port */
#ifndef LV_USE_PROFILER_BUILTIN_POSIX
    #ifdef CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
        #define LV_USE_PROFILER_BUILTIN_POSIX CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
    #else
        #define LV_USE_PROFILER_BUILTIN_POSIX 0
    #endif
#endif

#endif /*LV_USE_PROFILER_BUILTIN*/

#if LV_USE_PROFILER
/** Path to a header that defines the following profiler hook macros:
 *
 *  LV_PROFILER_BEGIN
 *    Called at the start of a profiled scope.
 *    e.g. #define LV_PROFILER_BEGIN  my_profiler_begin()
 *
 *  LV_PROFILER_END
 *    Called at the end of a profiled scope.
 *    e.g. #define LV_PROFILER_END    my_profiler_end()
 *
 *  LV_PROFILER_BEGIN_TAG(tag)
 *    Called at the start of a profiled scope with a string tag.
 *    e.g. #define LV_PROFILER_BEGIN_TAG(tag)  my_profiler_begin_tag(tag)
 *
 *  LV_PROFILER_END_TAG(tag)
 *    Called at the end of a profiled scope with a string tag.
 *    e.g. #define LV_PROFILER_END_TAG(tag)    my_profiler_end_tag(tag)
 *
 *  The default header provides these via the built-in profiler.
 *  Set this to your own header to use a custom profiler backend.
 */
#ifndef LV_PROFILER_INCLUDE
    #ifdef CONFIG_LV_PROFILER_INCLUDE
        #define LV_PROFILER_INCLUDE CONFIG_LV_PROFILER_INCLUDE
    #else
        #define LV_PROFILER_INCLUDE "lvgl/debugging/profiler/lv_profiler_builtin.h"
    #endif
#endif

/** Enable layout profiler */
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

/** Enable disp refr profiler */
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

/** Enable draw profiler */
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

/** Enable indev profiler */
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

/** Enable decoder profiler */
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

/** Enable font profiler */
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

/** Enable fs profiler */
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

/** Enable style profiler */
#ifndef LV_PROFILER_STYLE
    #ifdef CONFIG_LV_PROFILER_STYLE
        #define LV_PROFILER_STYLE CONFIG_LV_PROFILER_STYLE
    #else
        #define LV_PROFILER_STYLE 0
    #endif
#endif

/** Enable timer profiler */
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

/** Enable cache profiler */
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

/** Enable event profiler */
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

#endif /*LV_USE_PROFILER*/

/** Enable emulated input devices, time emulation, and screenshot compares. */
#ifndef LV_USE_TEST
    #ifdef CONFIG_LV_USE_TEST
        #define LV_USE_TEST CONFIG_LV_USE_TEST
    #else
        #define LV_USE_TEST 0
    #endif
#endif

#if LV_USE_TEST
/** Enable `lv_test_screenshot_compare`.
 *  Requires lodepng and a few MB of extra RAM.
 */
#ifndef LV_USE_TEST_SCREENSHOT_COMPARE
    #ifdef CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
        #define LV_USE_TEST_SCREENSHOT_COMPARE CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
    #else
        #define LV_USE_TEST_SCREENSHOT_COMPARE 0
    #endif
#endif

#if LV_USE_TEST_SCREENSHOT_COMPARE
/** Create a reference image by default if it does not exist */
#ifndef LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
            #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
        #else
            #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 0
        #endif
    #else
        #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1
    #endif
#endif

#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/
#endif /*LV_USE_TEST*/

/** Enable Monkey test */
#ifndef LV_USE_MONKEY
    #ifdef CONFIG_LV_USE_MONKEY
        #define LV_USE_MONKEY CONFIG_LV_USE_MONKEY
    #else
        #define LV_USE_MONKEY 0
    #endif
#endif

/** Draw random colored rectangles over the redrawn areas. */
#ifndef LV_USE_REFR_DEBUG
    #ifdef CONFIG_LV_USE_REFR_DEBUG
        #define LV_USE_REFR_DEBUG CONFIG_LV_USE_REFR_DEBUG
    #else
        #define LV_USE_REFR_DEBUG 0
    #endif
#endif

/** Draw a red overlay for ARGB layers and a green overlay for RGB layers */
#ifndef LV_USE_LAYER_DEBUG
    #ifdef CONFIG_LV_USE_LAYER_DEBUG
        #define LV_USE_LAYER_DEBUG CONFIG_LV_USE_LAYER_DEBUG
    #else
        #define LV_USE_LAYER_DEBUG 0
    #endif
#endif

/** Also add the index number of the draw unit on white background.
 *  For layers add the index number of the draw unit on black background.
 */
#ifndef LV_USE_PARALLEL_DRAW_DEBUG
    #ifdef CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
        #define LV_USE_PARALLEL_DRAW_DEBUG CONFIG_LV_USE_PARALLEL_DRAW_DEBUG
    #else
        #define LV_USE_PARALLEL_DRAW_DEBUG 0
    #endif
#endif



/*============================================================================
 * OTHERS
 *============================================================================*/

/** Enable `lv_obj` fragment logic */
#ifndef LV_USE_FRAGMENT
    #ifdef CONFIG_LV_USE_FRAGMENT
        #define LV_USE_FRAGMENT CONFIG_LV_USE_FRAGMENT
    #else
        #define LV_USE_FRAGMENT 0
    #endif
#endif

/** Enable file explorer.
 *  - Requires: lv_table
 */
#ifndef LV_USE_FILE_EXPLORER
    #ifdef CONFIG_LV_USE_FILE_EXPLORER
        #define LV_USE_FILE_EXPLORER CONFIG_LV_USE_FILE_EXPLORER
    #else
        #define LV_USE_FILE_EXPLORER 0
    #endif
#endif

#if LV_USE_FILE_EXPLORER
/** Maximum length of path */
#ifndef LV_FILE_EXPLORER_PATH_MAX_LEN
    #ifdef CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
        #define LV_FILE_EXPLORER_PATH_MAX_LEN CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
    #else
        #define LV_FILE_EXPLORER_PATH_MAX_LEN 128
    #endif
#endif

/** This can save some memory, but not much.
 *  After the quick access bar is created, it can be hidden
 *  by clicking the button at the top left corner of
 *  the browsing area, which is very useful for small screen devices.
 */
#ifndef LV_FILE_EXPLORER_QUICK_ACCESS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
            #define LV_FILE_EXPLORER_QUICK_ACCESS CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
        #else
            #define LV_FILE_EXPLORER_QUICK_ACCESS 0
        #endif
    #else
        #define LV_FILE_EXPLORER_QUICK_ACCESS 1
    #endif
#endif

#endif /*LV_USE_FILE_EXPLORER*/


/*============================================================================
 * BUILD
 *============================================================================*/

/** Include `lvgl_private.h` in `lvgl.h` to access internal data and functions by default */
#ifndef LV_USE_PRIVATE_API
    #ifdef CONFIG_LV_USE_PRIVATE_API
        #define LV_USE_PRIVATE_API CONFIG_LV_USE_PRIVATE_API
    #else
        #define LV_USE_PRIVATE_API 0
    #endif
#endif

#if LV_USE_PRIVATE_API
/** By default, LVGL stores all runtime state in a single static
 *  'lv_global_t' variable, sufficient for most single-display
 *  single-threaded applications.
 *
 *  Enable this if you need multiple independent LVGL instances in
 *  the same process, for example:
 *    - Multiple physical displays with separate render loops
 *    - Per-thread LVGL contexts on an RTOS (FreeRTOS, Zephyr, etc.)
 *    - Hot-swappable or sandboxed UI instances
 *
 *  When enabled, every LVGL internal access to global state goes
 *  through LV_GLOBAL_CUSTOM() instead of a static variable. You
 *  must implement 'lv_global_t *lv_global_default(void)' to return
 *  the correct instance for the current context (e.g. by thread-local
 *  storage or a guarded pointer switch).
 *
 *  Returning NULL from that function is safe; LVGL will treat the
 *  library as uninitialized for that context.
 */
#ifndef LV_ENABLE_GLOBAL_CUSTOM
    #ifdef CONFIG_LV_ENABLE_GLOBAL_CUSTOM
        #define LV_ENABLE_GLOBAL_CUSTOM CONFIG_LV_ENABLE_GLOBAL_CUSTOM
    #else
        #define LV_ENABLE_GLOBAL_CUSTOM 0
    #endif
#endif

#if LV_ENABLE_GLOBAL_CUSTOM
/** Path to the header that declares:
 *    lv_global_t *lv_global_default(void);
 *
 *  This header is included wherever LVGL needs to resolve the
 *  LV_GLOBAL_DEFAULT() macro. Keep it lightweight, it is included
 *  in performance-critical paths.
 */
#ifndef LV_GLOBAL_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
        #define LV_GLOBAL_CUSTOM_INCLUDE CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
    #else
        #define LV_GLOBAL_CUSTOM_INCLUDE "lv_global.h"
    #endif
#endif

#endif /*LV_ENABLE_GLOBAL_CUSTOM*/
#endif /*LV_USE_PRIVATE_API*/

/** Check if the parameter is NULL. (Very fast, recommended) */
#ifndef LV_USE_ASSERT_NULL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_NULL
            #define LV_USE_ASSERT_NULL CONFIG_LV_USE_ASSERT_NULL
        #else
            #define LV_USE_ASSERT_NULL 0
        #endif
    #else
        #define LV_USE_ASSERT_NULL 1
    #endif
#endif

/** Checks if the memory is successfully allocated or no. (Very fast, recommended) */
#ifndef LV_USE_ASSERT_MALLOC
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_MALLOC
            #define LV_USE_ASSERT_MALLOC CONFIG_LV_USE_ASSERT_MALLOC
        #else
            #define LV_USE_ASSERT_MALLOC 0
        #endif
    #else
        #define LV_USE_ASSERT_MALLOC 1
    #endif
#endif

/** Check if the styles are properly initialized. (Very fast, recommended) */
#ifndef LV_USE_ASSERT_STYLE
    #ifdef CONFIG_LV_USE_ASSERT_STYLE
        #define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
    #else
        #define LV_USE_ASSERT_STYLE 0
    #endif
#endif

/** Check the integrity of `lv_mem` after critical operations. (Slow) */
#ifndef LV_USE_ASSERT_MEM_INTEGRITY
    #ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
        #define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
    #else
        #define LV_USE_ASSERT_MEM_INTEGRITY 0
    #endif
#endif

/** Check NULL, the object's type and existence (e.g. not deleted). (Slow) */
#ifndef LV_USE_ASSERT_OBJ
    #ifdef CONFIG_LV_USE_ASSERT_OBJ
        #define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
    #else
        #define LV_USE_ASSERT_OBJ 0
    #endif
#endif

/** Optional header to override assert-related macros.
 *
 *  LV_ASSERT_HANDLER
 *    Statement to execute when an assertion fails.
 *    e.g. #define LV_ASSERT_HANDLER my_assert_failed(__FILE__, __LINE__);
 */
#ifndef LV_ASSERT_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_ASSERT_CUSTOM_INCLUDE
        #define LV_ASSERT_CUSTOM_INCLUDE CONFIG_LV_ASSERT_CUSTOM_INCLUDE
    #else
        #define LV_ASSERT_CUSTOM_INCLUDE ""
    #endif
#endif

/** When enabled, LV_CHECK_ARG checks validate function arguments
 *  at runtime. Failed checks log a warning and execute the specified
 *  action. When disabled, all LV_CHECK_ARG checks compile to nothing.
 *  Disabling this is not recommended unless extreme care is taken and only
 *  in very resource constrained environments where it can be absolutely
 *  ensured that invariants are never violated.
 */
#ifndef LV_USE_CHECK_ARG
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHECK_ARG
            #define LV_USE_CHECK_ARG CONFIG_LV_USE_CHECK_ARG
        #else
            #define LV_USE_CHECK_ARG 0
        #endif
    #else
        #define LV_USE_CHECK_ARG 1
    #endif
#endif

#if LV_USE_CHECK_ARG
/** When enabled, LV_ASSERT_HANDLER is also invoked when an
 *  LV_CHECK_ARG check fails, before the action is executed.
 */
#ifndef LV_CHECK_ARG_ASSERT_ON_FAIL
    #ifdef CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL
        #define LV_CHECK_ARG_ASSERT_ON_FAIL CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL
    #else
        #define LV_CHECK_ARG_ASSERT_ON_FAIL 0
    #endif
#endif

/** Controls what is logged when an LV_CHECK_ARG check fails.
 *  MINIMAL and VERBOSE modes require LV_USE_LOG to be enabled;
 *  selecting either when LV_USE_LOG is disabled will cause a
 *  compile-time error.
 *  Possible values:
 *  - LV_CHECK_ARG_LOG_MODE_NONE: no log output on failure
 *  - LV_CHECK_ARG_LOG_MODE_MINIMAL: log 'Check failed' only (file/line from LV_LOG_WARN)
 *  - LV_CHECK_ARG_LOG_MODE_VERBOSE: log 'Check failed: <cond>' plus caller-supplied message
 */
#ifndef LV_CHECK_ARG_LOG_MODE
    #ifdef CONFIG_LV_CHECK_ARG_LOG_MODE
        #define LV_CHECK_ARG_LOG_MODE CONFIG_LV_CHECK_ARG_LOG_MODE
    #else
        #define LV_CHECK_ARG_LOG_MODE LV_CHECK_ARG_LOG_MODE_NONE
    #endif
#endif

/** When enabled, LV_CHECK_OBJ verifies that the object has the
 *  expected class (lv_obj_has_class). When disabled, the class
 *  check is skipped even if a class argument is supplied.
 */
#ifndef LV_USE_CHECK_OBJ_CLASSTYPE
    #ifdef CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
        #define LV_USE_CHECK_OBJ_CLASSTYPE CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
    #else
        #define LV_USE_CHECK_OBJ_CLASSTYPE 0
    #endif
#endif

/** When enabled, LV_CHECK_OBJ also verifies that the object is
 *  still part of the widget tree (lv_obj_is_valid). When disabled,
 *  the validity check is skipped even if the associated argument
 *  is supplied.
 */
#ifndef LV_USE_CHECK_OBJ_VALIDITY
    #ifdef CONFIG_LV_USE_CHECK_OBJ_VALIDITY
        #define LV_USE_CHECK_OBJ_VALIDITY CONFIG_LV_USE_CHECK_OBJ_VALIDITY
    #else
        #define LV_USE_CHECK_OBJ_VALIDITY 0
    #endif
#endif

#endif /*LV_USE_CHECK_ARG*/


/*============================================================================
 * COMPILER SETTINGS
 *============================================================================*/

/** For big endian systems set to 1 */
#ifndef LV_BIG_ENDIAN_SYSTEM
    #ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
        #define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
    #else
        #define LV_BIG_ENDIAN_SYSTEM 0
    #endif
#endif

/** Optional header to override LV_ATTRIBUTE_* macros for your platform.
 *  Leave empty to use defaults (all expand to nothing).
 *
 *  Create a header file and set this to its path, e.g. "my_lv_attr.h".
 *  The following macros can be defined in it:
 *
 *  LV_ATTRIBUTE_MEM_ALIGN
 *    Where memory needs to be aligned (e.g. with -Os, data may not be
 *    aligned to the required boundary by default).
 *    e.g. __attribute__((aligned(4)))
 *
 *  LV_ATTRIBUTE_TICK_INC
 *    Applied to lv_tick_inc(). Often called from a timer ISR, so fast
 *    memory placement can reduce latency.
 *    e.g. __attribute__((section(".itcm"))) or __ramfunc (IAR)
 *
 *  LV_ATTRIBUTE_TIMER_HANDLER
 *    Applied to lv_timer_handler().
 *    e.g. __attribute__((section(".itcm")))
 *
 *  LV_ATTRIBUTE_FLUSH_READY
 *    Applied to lv_display_flush_ready(). Often called from a DMA or
 *    display ISR.
 *    e.g. __attribute__((section(".itcm")))
 *
 *  LV_ATTRIBUTE_SYNC_READY
 *    Applied to lv_display_sync_ready(). Similar to FLUSH_READY, useful
 *    when called from an ISR context.
 *    e.g. __attribute__((section(".itcm")))
 *
 *  LV_ATTRIBUTE_LARGE_CONST
 *    Applied to large read-only arrays such as font bitmaps. Use to place
 *    them in a specific flash region or external memory.
 *    e.g. __attribute__((section(".ext_flash")))
 *
 *  LV_ATTRIBUTE_LARGE_RAM_ARRAY
 *    Applied to large arrays allocated in RAM.
 *    e.g. __attribute__((section(".ext_ram")))
 *
 *  LV_ATTRIBUTE_FAST_MEM
 *    Applied to performance-critical functions to place them in faster
 *    memory such as ITCM or SRAM.
 *    e.g. __attribute__((section(".itcm"))) or __ramfunc (IAR)
 *
 *  LV_ATTRIBUTE_EXTERN_DATA
 *    Prefix for all global extern data declarations. Useful for placing
 *    shared data in a named section or applying export attributes.
 *    e.g. __attribute__((section(".shared_mem")))
 *
 *  LV_EXPORT_CONST_INT(int_value)
 *    Exports an integer constant to a language binding API such as
 *    MicroPython. Called for all constants in the form LV_<CONST>.
 *    Leave undefined if not using a binding.
 *    e.g. mp_rom_map_elem_t entry = {MP_ROM_QSTR(MP_QSTR_##int_value), MP_ROM_INT(int_value)}
 */
#ifndef LV_ATTRIBUTE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_ATTRIBUTE_CUSTOM_INCLUDE
        #define LV_ATTRIBUTE_CUSTOM_INCLUDE CONFIG_LV_ATTRIBUTE_CUSTOM_INCLUDE
    #else
        #define LV_ATTRIBUTE_CUSTOM_INCLUDE ""
    #endif
#endif



/*============================================================================
 * EXAMPLES
 *============================================================================*/

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



/*============================================================================
 * DEMOS
 *============================================================================*/

/** Build the demos */
#ifndef LV_BUILD_DEMOS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_BUILD_DEMOS
            #define LV_BUILD_DEMOS CONFIG_LV_BUILD_DEMOS
        #else
            #define LV_BUILD_DEMOS 0
        #endif
    #else
        #define LV_BUILD_DEMOS 1
    #endif
#endif

#if LV_BUILD_DEMOS
/** Benchmark your system */
#ifndef LV_USE_DEMO_BENCHMARK
    #ifdef CONFIG_LV_USE_DEMO_BENCHMARK
        #define LV_USE_DEMO_BENCHMARK CONFIG_LV_USE_DEMO_BENCHMARK
    #else
        #define LV_USE_DEMO_BENCHMARK 0
    #endif
#endif

#if LV_USE_DEMO_BENCHMARK
/** Use static aligned fonts */
#ifndef LV_DEMO_BENCHMARK_ALIGNED_FONTS
    #ifdef CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
    #else
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0
    #endif
#endif

#endif /*LV_USE_DEMO_BENCHMARK*/

#if LV_USE_GLTF
/** glTF demo */
#ifndef LV_USE_DEMO_GLTF
    #ifdef CONFIG_LV_USE_DEMO_GLTF
        #define LV_USE_DEMO_GLTF CONFIG_LV_USE_DEMO_GLTF
    #else
        #define LV_USE_DEMO_GLTF 0
    #endif
#endif

#endif /*LV_USE_GLTF*/

/** Demonstrate the usage of encoder and keyboard */
#ifndef LV_USE_DEMO_KEYPAD_AND_ENCODER
    #ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
    #else
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
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
/** Enable Square */
#ifndef LV_DEMO_MUSIC_SQUARE
    #ifdef CONFIG_LV_DEMO_MUSIC_SQUARE
        #define LV_DEMO_MUSIC_SQUARE CONFIG_LV_DEMO_MUSIC_SQUARE
    #else
        #define LV_DEMO_MUSIC_SQUARE 0
    #endif
#endif

/** Enable Landscape */
#ifndef LV_DEMO_MUSIC_LANDSCAPE
    #ifdef CONFIG_LV_DEMO_MUSIC_LANDSCAPE
        #define LV_DEMO_MUSIC_LANDSCAPE CONFIG_LV_DEMO_MUSIC_LANDSCAPE
    #else
        #define LV_DEMO_MUSIC_LANDSCAPE 0
    #endif
#endif

/** Enable Round */
#ifndef LV_DEMO_MUSIC_ROUND
    #ifdef CONFIG_LV_DEMO_MUSIC_ROUND
        #define LV_DEMO_MUSIC_ROUND CONFIG_LV_DEMO_MUSIC_ROUND
    #else
        #define LV_DEMO_MUSIC_ROUND 0
    #endif
#endif

/** Enable Large */
#ifndef LV_DEMO_MUSIC_LARGE
    #ifdef CONFIG_LV_DEMO_MUSIC_LARGE
        #define LV_DEMO_MUSIC_LARGE CONFIG_LV_DEMO_MUSIC_LARGE
    #else
        #define LV_DEMO_MUSIC_LARGE 0
    #endif
#endif

/** Enable Auto play */
#ifndef LV_DEMO_MUSIC_AUTO_PLAY
    #ifdef CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
        #define LV_DEMO_MUSIC_AUTO_PLAY CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
    #else
        #define LV_DEMO_MUSIC_AUTO_PLAY 0
    #endif
#endif

#endif /*LV_USE_DEMO_MUSIC*/

/** Render test for each primitives. Requires at least 480x272 display */
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

#if LV_DRAW_HAS_VECTOR_SUPPORT
/** vector graphic demo */
#ifndef LV_USE_DEMO_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
        #define LV_USE_DEMO_VECTOR_GRAPHIC CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
    #else
        #define LV_USE_DEMO_VECTOR_GRAPHIC 0
    #endif
#endif

#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

/** Show-case LVGL widgets */
#ifndef LV_USE_DEMO_WIDGETS
    #ifdef CONFIG_LV_USE_DEMO_WIDGETS
        #define LV_USE_DEMO_WIDGETS CONFIG_LV_USE_DEMO_WIDGETS
    #else
        #define LV_USE_DEMO_WIDGETS 0
    #endif
#endif

/** Flex layout previewer */
#ifndef LV_USE_DEMO_FLEX_LAYOUT
    #ifdef CONFIG_LV_USE_DEMO_FLEX_LAYOUT
        #define LV_USE_DEMO_FLEX_LAYOUT CONFIG_LV_USE_DEMO_FLEX_LAYOUT
    #else
        #define LV_USE_DEMO_FLEX_LAYOUT 0
    #endif
#endif

/** multi-language demo */
#ifndef LV_USE_DEMO_MULTILANG
    #ifdef CONFIG_LV_USE_DEMO_MULTILANG
        #define LV_USE_DEMO_MULTILANG CONFIG_LV_USE_DEMO_MULTILANG
    #else
        #define LV_USE_DEMO_MULTILANG 0
    #endif
#endif

/** Smartwatch demo */
#ifndef LV_USE_DEMO_SMARTWATCH
    #ifdef CONFIG_LV_USE_DEMO_SMARTWATCH
        #define LV_USE_DEMO_SMARTWATCH CONFIG_LV_USE_DEMO_SMARTWATCH
    #else
        #define LV_USE_DEMO_SMARTWATCH 0
    #endif
#endif

/** Ebike demo */
#ifndef LV_USE_DEMO_EBIKE
    #ifdef CONFIG_LV_USE_DEMO_EBIKE
        #define LV_USE_DEMO_EBIKE CONFIG_LV_USE_DEMO_EBIKE
    #else
        #define LV_USE_DEMO_EBIKE 0
    #endif
#endif

#if LV_USE_DEMO_EBIKE
/** Ebike portrait layout */
#ifndef LV_DEMO_EBIKE_PORTRAIT
    #ifdef CONFIG_LV_DEMO_EBIKE_PORTRAIT
        #define LV_DEMO_EBIKE_PORTRAIT CONFIG_LV_DEMO_EBIKE_PORTRAIT
    #else
        #define LV_DEMO_EBIKE_PORTRAIT 0
    #endif
#endif

#endif /*LV_USE_DEMO_EBIKE*/

/** High resolution demo */
#ifndef LV_USE_DEMO_HIGH_RES
    #ifdef CONFIG_LV_USE_DEMO_HIGH_RES
        #define LV_USE_DEMO_HIGH_RES CONFIG_LV_USE_DEMO_HIGH_RES
    #else
        #define LV_USE_DEMO_HIGH_RES 0
    #endif
#endif

#endif /*LV_BUILD_DEMOS*/




/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

#ifndef LV_ATTRIBUTE_TIMER_HANDLER
#define LV_ATTRIBUTE_TIMER_HANDLER
#endif

#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

#ifndef LV_ATTRIBUTE_SYNC_READY
#define LV_ATTRIBUTE_SYNC_READY
#endif

#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY
#endif

#ifndef LV_ATTRIBUTE_FAST_MEM
#define LV_ATTRIBUTE_FAST_MEM
#endif

#ifndef LV_ATTRIBUTE_EXTERN_DATA
#define LV_ATTRIBUTE_EXTERN_DATA
#endif

#ifndef LV_EXPORT_CONST_INT
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#endif

/*Fix inconsistent name*/
#define LV_USE_ANIMIMAGE LV_USE_ANIMIMG

#ifndef __ASSEMBLY__
LV_EXPORT_CONST_INT(LV_DPI_DEF);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_STRIDE_ALIGN);
LV_EXPORT_CONST_INT(LV_DRAW_BUF_ALIGN);
#endif

#undef LV_KCONFIG_PRESENT

/* Disable VGLite drivers if VGLite drawing is disabled */
#ifndef LV_USE_VG_LITE_DRIVER
    #define LV_USE_VG_LITE_DRIVER 0
#endif

#ifndef LV_USE_VG_LITE_THORVG
    #define LV_USE_VG_LITE_THORVG 0
#endif

#ifndef LV_NEMA_USE_CACHE
    #define LV_NEMA_USE_CACHE 0
#endif

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

#if LV_USE_WAYLAND
    /* Backend is selected explicitly (Kconfig choice or lv_conf.h).  EGL/G2D are
     * resolved first; SHM is the fallback only when neither is selected, so the
     * three remain mutually exclusive. */
    #ifndef LV_WAYLAND_USE_EGL
        #ifdef CONFIG_LV_WAYLAND_USE_EGL
            #define LV_WAYLAND_USE_EGL CONFIG_LV_WAYLAND_USE_EGL
        #else
            #define LV_WAYLAND_USE_EGL 0
        #endif
    #endif
    #ifndef LV_WAYLAND_USE_G2D
        #ifdef CONFIG_LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_G2D CONFIG_LV_WAYLAND_USE_G2D
        #else
            #define LV_WAYLAND_USE_G2D 0
        #endif
    #endif
    #ifndef LV_WAYLAND_USE_SHM
        #ifdef CONFIG_LV_WAYLAND_USE_SHM
            #define LV_WAYLAND_USE_SHM CONFIG_LV_WAYLAND_USE_SHM
        #elif LV_WAYLAND_USE_EGL || LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_SHM 0
        #else
            #define LV_WAYLAND_USE_SHM 1
        #endif
    #endif
#else
    #define LV_WAYLAND_USE_SHM 0
    #define LV_WAYLAND_USE_EGL 0
    #define LV_WAYLAND_USE_G2D 0
#endif

#if LV_USE_LINUX_DRM
    #if LV_USE_OPENGLES
        #define LV_LINUX_DRM_USE_EGL 1
    #else
        #define LV_LINUX_DRM_USE_EGL 0
    #endif /* LV_USE_OPENGLES */
#else
    #define LV_LINUX_DRM_USE_EGL 0
#endif /*LV_USE_LINUX_DRM*/

#if LV_USE_SYSMON == 0
    #define LV_USE_PERF_MONITOR 0
    #define LV_USE_MEM_MONITOR 0
    #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
#endif /*LV_USE_SYSMON*/

#if LV_USE_PERF_MONITOR == 0
    #define LV_USE_PERF_MONITOR_LOG_MODE 0
#endif /*LV_USE_PERF_MONITOR*/

#if LV_BUILD_DEMOS == 0
    #define LV_USE_DEMO_WIDGETS 0
    #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #define LV_USE_DEMO_BENCHMARK 0
    #define LV_USE_DEMO_RENDER 0
    #define LV_USE_DEMO_STRESS 0
    #define LV_USE_DEMO_MUSIC 0
    #define LV_USE_DEMO_VECTOR_GRAPHIC  0
    #define LV_USE_DEMO_FLEX_LAYOUT     0
    #define LV_USE_DEMO_MULTILANG       0
    #define LV_USE_DEMO_EBIKE           0
    #define LV_USE_DEMO_HIGH_RES        0
    #define LV_USE_DEMO_SMARTWATCH      0
    #define LV_USE_DEMO_GLTF            0
#endif /* LV_BUILD_DEMOS */

#ifndef LV_USE_LZ4
    #if (LV_USE_LZ4_INTERNAL || LV_USE_LZ4_EXTERNAL)
        #define LV_USE_LZ4 1
    #else
        #define LV_USE_LZ4 0
    #endif
#endif

#ifndef LV_USE_THORVG
    #if (LV_USE_THORVG_INTERNAL || LV_USE_THORVG_EXTERNAL)
        #define LV_USE_THORVG 1
    #else
        #define LV_USE_THORVG 0
    #endif
#endif

#if LV_USE_SDL && LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
    #define LV_SDL_USE_EGL 1
#else
    #define LV_SDL_USE_EGL 0
#endif

#ifndef LV_USE_EGL
    #if LV_LINUX_DRM_USE_EGL || LV_WAYLAND_USE_EGL || LV_SDL_USE_EGL
        #define LV_USE_EGL 1
    #else
        #define LV_USE_EGL 0
    #endif
#endif /* LV_USE_EGL */


#if LV_USE_OS
    #if (LV_USE_FREETYPE || LV_USE_THORVG) && LV_DRAW_THREAD_STACK_SIZE < (32 * 1024)
        #error "Increase LV_DRAW_THREAD_STACK_SIZE to at least 32KB for FreeType or ThorVG."
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

#ifndef LV_CHECK_ARG_LOG_MODE
    #define LV_CHECK_ARG_LOG_MODE   0
#endif

#endif  /*LV_CONF_INTERNAL_H*/

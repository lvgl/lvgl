/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * This file ensures all defines of lv_conf.h have a default value.
 */

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/* Config options */
/* Built-in font selectors for LV_FONT_DEFAULT */
#define LV_FONT_DEFAULT_MONTSERRAT_8                &lv_font_montserrat_8
#define LV_FONT_DEFAULT_MONTSERRAT_10               &lv_font_montserrat_10
#define LV_FONT_DEFAULT_MONTSERRAT_12               &lv_font_montserrat_12
#define LV_FONT_DEFAULT_MONTSERRAT_14               &lv_font_montserrat_14
#define LV_FONT_DEFAULT_MONTSERRAT_16               &lv_font_montserrat_16
#define LV_FONT_DEFAULT_MONTSERRAT_18               &lv_font_montserrat_18
#define LV_FONT_DEFAULT_MONTSERRAT_20               &lv_font_montserrat_20
#define LV_FONT_DEFAULT_MONTSERRAT_22               &lv_font_montserrat_22
#define LV_FONT_DEFAULT_MONTSERRAT_24               &lv_font_montserrat_24
#define LV_FONT_DEFAULT_MONTSERRAT_26               &lv_font_montserrat_26
#define LV_FONT_DEFAULT_MONTSERRAT_28               &lv_font_montserrat_28
#define LV_FONT_DEFAULT_MONTSERRAT_30               &lv_font_montserrat_30
#define LV_FONT_DEFAULT_MONTSERRAT_32               &lv_font_montserrat_32
#define LV_FONT_DEFAULT_MONTSERRAT_34               &lv_font_montserrat_34
#define LV_FONT_DEFAULT_MONTSERRAT_36               &lv_font_montserrat_36
#define LV_FONT_DEFAULT_MONTSERRAT_38               &lv_font_montserrat_38
#define LV_FONT_DEFAULT_MONTSERRAT_40               &lv_font_montserrat_40
#define LV_FONT_DEFAULT_MONTSERRAT_42               &lv_font_montserrat_42
#define LV_FONT_DEFAULT_MONTSERRAT_44               &lv_font_montserrat_44
#define LV_FONT_DEFAULT_MONTSERRAT_46               &lv_font_montserrat_46
#define LV_FONT_DEFAULT_MONTSERRAT_48               &lv_font_montserrat_48
#define LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED    &lv_font_montserrat_28_compressed
#define LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW    &lv_font_dejavu_16_persian_hebrew
#define LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK   &lv_font_source_han_sans_sc_14_cjk
#define LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK   &lv_font_source_han_sans_sc_16_cjk
#define LV_FONT_DEFAULT_UNSCII_8                    &lv_font_unscii_8
#define LV_FONT_DEFAULT_UNSCII_16                   &lv_font_unscii_16

/* Named constants */
#define LV_STDLIB_BUILTIN       0
#define LV_STDLIB_CLIB          1
#define LV_STDLIB_MICROPYTHON   2
#define LV_STDLIB_RTTHREAD      3
#define LV_STDLIB_CUSTOM        255
#define LV_LOG_LEVEL_NUM        5

/* Default operating system to use */
#define LV_OS_NONE          0
#define LV_OS_PTHREAD       1
#define LV_OS_FREERTOS      2
#define LV_OS_CMSIS_RTOS2   3
#define LV_OS_RTTHREAD      4
#define LV_OS_WINDOWS       5
#define LV_OS_MQX           6
#define LV_OS_SDL2          7
#define LV_OS_CUSTOM        255

/* Asm mode in sw draw */
#define LV_DRAW_SW_ASM_NONE      0
#define LV_DRAW_SW_ASM_NEON      1
#define LV_DRAW_SW_ASM_HELIUM    2
#define LV_DRAW_SW_ASM_RISCV_V   3
#define LV_DRAW_SW_ASM_CUSTOM    255

/* VG-Lite GPU (series and revision) */
#define LV_VG_LITE_GPU_GC255_0X40A           0
#define LV_VG_LITE_GPU_GC355_0X0_1215        1
#define LV_VG_LITE_GPU_GC355_0X0_1216        2
#define LV_VG_LITE_GPU_GC555_0X423           3
#define LV_VG_LITE_GPU_GC555_0X423_ECO       4
#define LV_VG_LITE_GPU_GCNANOULTRAV_0X1003   5

/* NemaGFX static library */
#define LV_NEMA_LIB_NONE          0
#define LV_NEMA_LIB_M33_REVC      1
#define LV_NEMA_LIB_M33_NEMAPVG   2
#define LV_NEMA_LIB_M55           3
#define LV_NEMA_LIB_M7            4

/* NemaGFX HAL */
#define LV_NEMA_HAL_CUSTOM   0
#define LV_NEMA_HAL_STM32    1

/* NanoVG OpenGL backend */
#define LV_NANOVG_BACKEND_GL2     1
#define LV_NANOVG_BACKEND_GL3     2
#define LV_NANOVG_BACKEND_GLES2   3
#define LV_NANOVG_BACKEND_GLES3   4

/* Default log verbosity */
#define LV_LOG_LEVEL_TRACE   0
#define LV_LOG_LEVEL_INFO    1
#define LV_LOG_LEVEL_WARN    2
#define LV_LOG_LEVEL_ERROR   3
#define LV_LOG_LEVEL_USER    4
#define LV_LOG_LEVEL_NONE    5

/* Rendering backend */
#define LV_LINUX_DRM_BACKEND_FBDEV   0
#define LV_LINUX_DRM_BACKEND_GBM     1
#define LV_LINUX_DRM_BACKEND_EGL     2

/* Rendering backend */
#define LV_SDL_BACKEND_SW        0
#define LV_SDL_BACKEND_TEXTURE   1
#define LV_SDL_BACKEND_EGL       2

/* SDL mousewheel mode */
#define LV_SDL_MOUSEWHEEL_MODE_ENCODER   0
#define LV_SDL_MOUSEWHEEL_MODE_CROWN     1

/* Rendering backend */
#define LV_WAYLAND_BACKEND_SHM   0
#define LV_WAYLAND_BACKEND_EGL   1
#define LV_WAYLAND_BACKEND_G2D   2

/* Log behavior on LV_CHECK_ARG failure */
#define LV_CHECK_ARG_LOG_MODE_NONE      0
#define LV_CHECK_ARG_LOG_MODE_MINIMAL   1
#define LV_CHECK_ARG_LOG_MODE_VERBOSE   2

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

/* 
 * Detect if the user is using the new calendar day/month configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_MONDAY_STR
#define LV_CALENDAR_DISABLE_DEFAULT_DAY_NAMES 1
#else
#define LV_CALENDAR_DISABLE_DEFAULT_DAY_NAMES 0
#endif

/* 
 * Detect if the user is using the new calendar day/month configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_JANUARY_STR
#define LV_CALENDAR_DISABLE_DEFAULT_MONTH_NAMES 1
#else
#define LV_CALENDAR_DISABLE_DEFAULT_MONTH_NAMES 0
#endif

/* 
 * Detect if the user is using the xkb keymap configuration
 * in order to avoid warnings for users that have migrated.
 */
#ifdef LV_LIBINPUT_XKB_RULES
#define LV_LIBINPUT_XKB_DISABLE_KEY_MAP 1
#else
#define LV_LIBINPUT_XKB_DISABLE_KEY_MAP 0
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/

/*============================================================================
 * MEMORY AND STANDARD LIBRARY
 *============================================================================*/

#ifndef LV_USE_STDLIB_MALLOC
    #ifdef CONFIG_LV_USE_STDLIB_MALLOC
        #define LV_USE_STDLIB_MALLOC CONFIG_LV_USE_STDLIB_MALLOC
    #else
        #define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN
    #endif
#endif

#ifndef LV_USE_STDLIB_STRING
    #ifdef CONFIG_LV_USE_STDLIB_STRING
        #define LV_USE_STDLIB_STRING CONFIG_LV_USE_STDLIB_STRING
    #else
        #define LV_USE_STDLIB_STRING LV_STDLIB_BUILTIN
    #endif
#endif

#ifndef LV_USE_STDLIB_SPRINTF
    #ifdef CONFIG_LV_USE_STDLIB_SPRINTF
        #define LV_USE_STDLIB_SPRINTF CONFIG_LV_USE_STDLIB_SPRINTF
    #else
        #define LV_USE_STDLIB_SPRINTF LV_STDLIB_BUILTIN
    #endif
#endif

#ifndef LV_MEM_SIZE
    #ifdef CONFIG_LV_MEM_SIZE
        #define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
    #else
        #define LV_MEM_SIZE 65536
    #endif
#endif

#ifndef LV_MEM_ADR
    #ifdef CONFIG_LV_MEM_ADR
        #define LV_MEM_ADR CONFIG_LV_MEM_ADR
    #else
        #define LV_MEM_ADR 0x0
    #endif
#endif

#ifndef LV_STDINT_INCLUDE
    #ifdef CONFIG_LV_STDINT_INCLUDE
        #define LV_STDINT_INCLUDE CONFIG_LV_STDINT_INCLUDE
    #else
        #define LV_STDINT_INCLUDE "stdint.h"
    #endif
#endif

#ifndef LV_STDDEF_INCLUDE
    #ifdef CONFIG_LV_STDDEF_INCLUDE
        #define LV_STDDEF_INCLUDE CONFIG_LV_STDDEF_INCLUDE
    #else
        #define LV_STDDEF_INCLUDE "stddef.h"
    #endif
#endif

#ifndef LV_STDBOOL_INCLUDE
    #ifdef CONFIG_LV_STDBOOL_INCLUDE
        #define LV_STDBOOL_INCLUDE CONFIG_LV_STDBOOL_INCLUDE
    #else
        #define LV_STDBOOL_INCLUDE "stdbool.h"
    #endif
#endif

#ifndef LV_INTTYPES_INCLUDE
    #ifdef CONFIG_LV_INTTYPES_INCLUDE
        #define LV_INTTYPES_INCLUDE CONFIG_LV_INTTYPES_INCLUDE
    #else
        #define LV_INTTYPES_INCLUDE "inttypes.h"
    #endif
#endif

#ifndef LV_LIMITS_INCLUDE
    #ifdef CONFIG_LV_LIMITS_INCLUDE
        #define LV_LIMITS_INCLUDE CONFIG_LV_LIMITS_INCLUDE
    #else
        #define LV_LIMITS_INCLUDE "limits.h"
    #endif
#endif

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

#ifndef LV_USE_OS
    #ifdef CONFIG_LV_USE_OS
        #define LV_USE_OS CONFIG_LV_USE_OS
    #else
        #define LV_USE_OS LV_OS_NONE
    #endif
#endif

#ifndef LV_OS_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_OS_CUSTOM_INCLUDE
        #define LV_OS_CUSTOM_INCLUDE CONFIG_LV_OS_CUSTOM_INCLUDE
    #else
        #define LV_OS_CUSTOM_INCLUDE ""
    #endif
#endif

#ifndef LV_USE_FREERTOS_TASK_NOTIFY
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
            #define LV_USE_FREERTOS_TASK_NOTIFY CONFIG_LV_USE_FREERTOS_TASK_NOTIFY
        #else
            #define LV_USE_FREERTOS_TASK_NOTIFY 0
        #endif
    #else
          #define LV_USE_FREERTOS_TASK_NOTIFY LV_USE_OS == LV_OS_FREERTOS
    #endif
#endif

#ifndef LV_OS_IDLE_PERCENT_CUSTOM
    #ifdef CONFIG_LV_OS_IDLE_PERCENT_CUSTOM
        #define LV_OS_IDLE_PERCENT_CUSTOM CONFIG_LV_OS_IDLE_PERCENT_CUSTOM
    #else
        #define LV_OS_IDLE_PERCENT_CUSTOM 0
    #endif
#endif



/*============================================================================
 * RENDERING CONFIGURATION
 *============================================================================*/

#ifndef LV_COLOR_DEPTH
    #ifdef CONFIG_LV_COLOR_DEPTH
        #define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
    #else
        #define LV_COLOR_DEPTH 16
    #endif
#endif

#ifndef LV_COLOR_MIX_ROUND_OFS
    #ifdef CONFIG_LV_COLOR_MIX_ROUND_OFS
        #define LV_COLOR_MIX_ROUND_OFS CONFIG_LV_COLOR_MIX_ROUND_OFS
    #else
        #define LV_COLOR_MIX_ROUND_OFS 0
    #endif
#endif

#ifndef LV_DEF_REFR_PERIOD
    #ifdef CONFIG_LV_DEF_REFR_PERIOD
        #define LV_DEF_REFR_PERIOD CONFIG_LV_DEF_REFR_PERIOD
    #else
        #define LV_DEF_REFR_PERIOD 33
    #endif
#endif

#ifndef LV_DPI_DEF
    #ifdef CONFIG_LV_DPI_DEF
        #define LV_DPI_DEF CONFIG_LV_DPI_DEF
    #else
        #define LV_DPI_DEF 130
    #endif
#endif

#ifndef LV_DRAW_BUF_STRIDE_ALIGN
    #ifdef CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
        #define LV_DRAW_BUF_STRIDE_ALIGN CONFIG_LV_DRAW_BUF_STRIDE_ALIGN
    #else
        #define LV_DRAW_BUF_STRIDE_ALIGN 1
    #endif
#endif

#ifndef LV_DRAW_BUF_ALIGN
    #ifdef CONFIG_LV_DRAW_BUF_ALIGN
        #define LV_DRAW_BUF_ALIGN CONFIG_LV_DRAW_BUF_ALIGN
    #else
        #define LV_DRAW_BUF_ALIGN 4
    #endif
#endif

#ifndef LV_USE_MATRIX
    #ifdef CONFIG_LV_USE_MATRIX
        #define LV_USE_MATRIX CONFIG_LV_USE_MATRIX
    #else
        #define LV_USE_MATRIX 0
    #endif
#endif

#ifndef LV_DRAW_TRANSFORM_USE_MATRIX
    #ifdef CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
        #define LV_DRAW_TRANSFORM_USE_MATRIX CONFIG_LV_DRAW_TRANSFORM_USE_MATRIX
    #else
        #define LV_DRAW_TRANSFORM_USE_MATRIX 0
    #endif
#endif

#ifndef LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #ifdef CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE CONFIG_LV_DRAW_LAYER_SIMPLE_BUF_SIZE
    #else
        #define LV_DRAW_LAYER_SIMPLE_BUF_SIZE 24576
    #endif
#endif

#ifndef LV_DRAW_LAYER_MAX_MEMORY
    #ifdef CONFIG_LV_DRAW_LAYER_MAX_MEMORY
        #define LV_DRAW_LAYER_MAX_MEMORY CONFIG_LV_DRAW_LAYER_MAX_MEMORY
    #else
        #define LV_DRAW_LAYER_MAX_MEMORY 0
    #endif
#endif

#ifndef LV_DRAW_THREAD_STACK_SIZE
    #ifdef CONFIG_LV_DRAW_THREAD_STACK_SIZE
        #define LV_DRAW_THREAD_STACK_SIZE CONFIG_LV_DRAW_THREAD_STACK_SIZE
    #else
        #define LV_DRAW_THREAD_STACK_SIZE 8192
    #endif
#endif

#ifndef LV_DRAW_THREAD_PRIO
    #ifdef CONFIG_LV_DRAW_THREAD_PRIO
        #define LV_DRAW_THREAD_PRIO CONFIG_LV_DRAW_THREAD_PRIO
    #else
        #define LV_DRAW_THREAD_PRIO 3
    #endif
#endif

#ifndef LV_USE_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_VECTOR_GRAPHIC
        #define LV_USE_VECTOR_GRAPHIC CONFIG_LV_USE_VECTOR_GRAPHIC
    #else
        #define LV_USE_VECTOR_GRAPHIC 0
    #endif
#endif

#ifndef LV_USE_SNAPSHOT
    #ifdef CONFIG_LV_USE_SNAPSHOT
        #define LV_USE_SNAPSHOT CONFIG_LV_USE_SNAPSHOT
    #else
        #define LV_USE_SNAPSHOT 0
    #endif
#endif

#ifndef LV_USE_THORVG
    #ifdef CONFIG_LV_USE_THORVG
        #define LV_USE_THORVG CONFIG_LV_USE_THORVG
    #else
        #define LV_USE_THORVG 0
    #endif
#endif

#ifndef LV_USE_THORVG_INTERNAL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THORVG_INTERNAL
            #define LV_USE_THORVG_INTERNAL CONFIG_LV_USE_THORVG_INTERNAL
        #else
            #define LV_USE_THORVG_INTERNAL 0
        #endif
    #else
          #define LV_USE_THORVG_INTERNAL LV_USE_THORVG
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

#ifndef LV_DRAW_SW_SUPPORT_RGB565
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565
            #define LV_DRAW_SW_SUPPORT_RGB565 CONFIG_LV_DRAW_SW_SUPPORT_RGB565
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565 0
        #endif
    #else
          #define LV_DRAW_SW_SUPPORT_RGB565 LV_USE_DRAW_SW
    #endif
#endif

#ifndef LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
            #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED CONFIG_LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
        #else
            #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 0
        #endif
    #else
          #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_RGB565A8 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_RGB888 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_XRGB8888 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_ARGB8888 LV_USE_DRAW_SW
    #endif
#endif

#ifndef LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
            #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED CONFIG_LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
        #else
            #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 0
        #endif
    #else
          #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_L8 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_AL88 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_A8 LV_USE_DRAW_SW
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
          #define LV_DRAW_SW_SUPPORT_I1 LV_USE_DRAW_SW
    #endif
#endif

#ifndef LV_DRAW_SW_I1_LUM_THRESHOLD
    #ifdef CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
        #define LV_DRAW_SW_I1_LUM_THRESHOLD CONFIG_LV_DRAW_SW_I1_LUM_THRESHOLD
    #else
        #define LV_DRAW_SW_I1_LUM_THRESHOLD 127
    #endif
#endif

#ifndef LV_DRAW_SW_DRAW_UNIT_CNT
    #ifdef CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
        #define LV_DRAW_SW_DRAW_UNIT_CNT CONFIG_LV_DRAW_SW_DRAW_UNIT_CNT
    #else
        #define LV_DRAW_SW_DRAW_UNIT_CNT 1
    #endif
#endif

#ifndef LV_USE_DRAW_ARM2D_SYNC
    #ifdef CONFIG_LV_USE_DRAW_ARM2D_SYNC
        #define LV_USE_DRAW_ARM2D_SYNC CONFIG_LV_USE_DRAW_ARM2D_SYNC
    #else
        #define LV_USE_DRAW_ARM2D_SYNC 0
    #endif
#endif

#ifndef LV_USE_NATIVE_HELIUM_ASM
    #ifdef CONFIG_LV_USE_NATIVE_HELIUM_ASM
        #define LV_USE_NATIVE_HELIUM_ASM CONFIG_LV_USE_NATIVE_HELIUM_ASM
    #else
        #define LV_USE_NATIVE_HELIUM_ASM 0
    #endif
#endif

#ifndef LV_DRAW_SW_COMPLEX
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_SW_COMPLEX
            #define LV_DRAW_SW_COMPLEX CONFIG_LV_DRAW_SW_COMPLEX
        #else
            #define LV_DRAW_SW_COMPLEX 0
        #endif
    #else
          #define LV_DRAW_SW_COMPLEX LV_USE_DRAW_SW
    #endif
#endif

#ifndef LV_GRADIENT_MAX_STOPS
    #ifdef CONFIG_LV_GRADIENT_MAX_STOPS
        #define LV_GRADIENT_MAX_STOPS CONFIG_LV_GRADIENT_MAX_STOPS
    #else
        #define LV_GRADIENT_MAX_STOPS 2
    #endif
#endif

#ifndef LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    #ifdef CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS CONFIG_LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    #else
        #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 0
    #endif
#endif

#ifndef LV_DRAW_SW_SHADOW_CACHE_SIZE
    #ifdef CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE CONFIG_LV_DRAW_SW_SHADOW_CACHE_SIZE
    #else
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0
    #endif
#endif

#ifndef LV_DRAW_SW_CIRCLE_CACHE_SIZE
    #ifdef CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE CONFIG_LV_DRAW_SW_CIRCLE_CACHE_SIZE
    #else
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
    #endif
#endif

#ifndef LV_USE_DRAW_SW_ASM
    #ifdef CONFIG_LV_USE_DRAW_SW_ASM
        #define LV_USE_DRAW_SW_ASM CONFIG_LV_USE_DRAW_SW_ASM
    #else
        #define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE
    #endif
#endif

#ifndef LV_DRAW_SW_ASM_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
        #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE CONFIG_LV_DRAW_SW_ASM_CUSTOM_INCLUDE
    #else
        #define LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
    #endif
#endif

#ifndef LV_USE_DRAW_VG_LITE
    #ifdef CONFIG_LV_USE_DRAW_VG_LITE
        #define LV_USE_DRAW_VG_LITE CONFIG_LV_USE_DRAW_VG_LITE
    #else
        #define LV_USE_DRAW_VG_LITE 0
    #endif
#endif

#ifndef LV_VG_LITE_USE_GPU_INIT
    #ifdef CONFIG_LV_VG_LITE_USE_GPU_INIT
        #define LV_VG_LITE_USE_GPU_INIT CONFIG_LV_VG_LITE_USE_GPU_INIT
    #else
        #define LV_VG_LITE_USE_GPU_INIT 0
    #endif
#endif

#ifndef LV_VG_LITE_USE_ASSERT
    #ifdef CONFIG_LV_VG_LITE_USE_ASSERT
        #define LV_VG_LITE_USE_ASSERT CONFIG_LV_VG_LITE_USE_ASSERT
    #else
        #define LV_VG_LITE_USE_ASSERT 0
    #endif
#endif

#ifndef LV_VG_LITE_FLUSH_MAX_COUNT
    #ifdef CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
        #define LV_VG_LITE_FLUSH_MAX_COUNT CONFIG_LV_VG_LITE_FLUSH_MAX_COUNT
    #else
        #define LV_VG_LITE_FLUSH_MAX_COUNT 8
    #endif
#endif

#ifndef LV_VG_LITE_USE_BOX_SHADOW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_VG_LITE_USE_BOX_SHADOW
            #define LV_VG_LITE_USE_BOX_SHADOW CONFIG_LV_VG_LITE_USE_BOX_SHADOW
        #else
            #define LV_VG_LITE_USE_BOX_SHADOW 0
        #endif
    #else
          #define LV_VG_LITE_USE_BOX_SHADOW LV_USE_DRAW_VG_LITE
    #endif
#endif

#ifndef LV_VG_LITE_GRAD_CACHE_CNT
    #ifdef CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
        #define LV_VG_LITE_GRAD_CACHE_CNT CONFIG_LV_VG_LITE_GRAD_CACHE_CNT
    #else
        #define LV_VG_LITE_GRAD_CACHE_CNT 32
    #endif
#endif

#ifndef LV_VG_LITE_STROKE_CACHE_CNT
    #ifdef CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
        #define LV_VG_LITE_STROKE_CACHE_CNT CONFIG_LV_VG_LITE_STROKE_CACHE_CNT
    #else
        #define LV_VG_LITE_STROKE_CACHE_CNT 32
    #endif
#endif

#ifndef LV_VG_LITE_BITMAP_FONT_CACHE_CNT
    #ifdef CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
        #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT CONFIG_LV_VG_LITE_BITMAP_FONT_CACHE_CNT
    #else
        #define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256
    #endif
#endif

#ifndef LV_VG_LITE_DISABLE_VLC_OP_CLOSE
    #ifdef CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
        #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE CONFIG_LV_VG_LITE_DISABLE_VLC_OP_CLOSE
    #else
        #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0
    #endif
#endif

#ifndef LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
    #ifdef CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
        #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT CONFIG_LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT
    #else
        #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0
    #endif
#endif

#ifndef LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
    #ifdef CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
        #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET CONFIG_LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET
    #else
        #define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0
    #endif
#endif

#ifndef LV_VG_LITE_PATH_DUMP_MAX_LEN
    #ifdef CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
        #define LV_VG_LITE_PATH_DUMP_MAX_LEN CONFIG_LV_VG_LITE_PATH_DUMP_MAX_LEN
    #else
        #define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000
    #endif
#endif

#ifndef LV_USE_VG_LITE_DRIVER
    #ifdef CONFIG_LV_USE_VG_LITE_DRIVER
        #define LV_USE_VG_LITE_DRIVER CONFIG_LV_USE_VG_LITE_DRIVER
    #else
        #define LV_USE_VG_LITE_DRIVER 0
    #endif
#endif

#ifndef LV_VG_LITE_GPU
    #ifdef CONFIG_LV_VG_LITE_GPU
        #define LV_VG_LITE_GPU CONFIG_LV_VG_LITE_GPU
    #else
        #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC255_0X40A
    #endif
#endif

#ifndef LV_VG_LITE_HAL_GPU_BASE_ADDRESS
    #ifdef CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
        #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS CONFIG_LV_VG_LITE_HAL_GPU_BASE_ADDRESS
    #else
        #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000
    #endif
#endif

#ifndef LV_USE_VG_LITE_THORVG
    #ifdef CONFIG_LV_USE_VG_LITE_THORVG
        #define LV_USE_VG_LITE_THORVG CONFIG_LV_USE_VG_LITE_THORVG
    #else
        #define LV_USE_VG_LITE_THORVG 0
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
    #ifdef CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
        #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT CONFIG_LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
    #else
        #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_YUV_SUPPORT
    #ifdef CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
        #define LV_VG_LITE_THORVG_YUV_SUPPORT CONFIG_LV_VG_LITE_THORVG_YUV_SUPPORT
    #else
        #define LV_VG_LITE_THORVG_YUV_SUPPORT 0
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
    #ifdef CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
        #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT CONFIG_LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT
    #else
        #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_16PIXELS_ALIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
            #define LV_VG_LITE_THORVG_16PIXELS_ALIGN CONFIG_LV_VG_LITE_THORVG_16PIXELS_ALIGN
        #else
            #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0
        #endif
    #else
          #define LV_VG_LITE_THORVG_16PIXELS_ALIGN LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
    #ifdef CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
        #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN CONFIG_LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
    #else
        #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64
    #endif
#endif

#ifndef LV_VG_LITE_THORVG_THREAD_RENDER
    #ifdef CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
        #define LV_VG_LITE_THORVG_THREAD_RENDER CONFIG_LV_VG_LITE_THORVG_THREAD_RENDER
    #else
        #define LV_VG_LITE_THORVG_THREAD_RENDER 0
    #endif
#endif

#ifndef LV_USE_DRAW_DAVE2D
    #ifdef CONFIG_LV_USE_DRAW_DAVE2D
        #define LV_USE_DRAW_DAVE2D CONFIG_LV_USE_DRAW_DAVE2D
    #else
        #define LV_USE_DRAW_DAVE2D 0
    #endif
#endif

#ifndef LV_USE_NEMA_GFX
    #ifdef CONFIG_LV_USE_NEMA_GFX
        #define LV_USE_NEMA_GFX CONFIG_LV_USE_NEMA_GFX
    #else
        #define LV_USE_NEMA_GFX 0
    #endif
#endif

#ifndef LV_NEMA_USE_CACHE
    #ifdef CONFIG_LV_NEMA_USE_CACHE
        #define LV_NEMA_USE_CACHE CONFIG_LV_NEMA_USE_CACHE
    #else
        #define LV_NEMA_USE_CACHE 0
    #endif
#endif

#ifndef LV_NEMA_CACHE_HAL_INCLUDE
    #ifdef CONFIG_LV_NEMA_CACHE_HAL_INCLUDE
        #define LV_NEMA_CACHE_HAL_INCLUDE CONFIG_LV_NEMA_CACHE_HAL_INCLUDE
    #else
        #define LV_NEMA_CACHE_HAL_INCLUDE "stm32u5xx_hal.h"
    #endif
#endif

#ifndef LV_USE_NEMA_LIB
    #ifdef CONFIG_LV_USE_NEMA_LIB
        #define LV_USE_NEMA_LIB CONFIG_LV_USE_NEMA_LIB
    #else
        #define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE
    #endif
#endif

#ifndef LV_USE_NEMA_HAL
    #ifdef CONFIG_LV_USE_NEMA_HAL
        #define LV_USE_NEMA_HAL CONFIG_LV_USE_NEMA_HAL
    #else
        #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
    #endif
#endif

#ifndef LV_NEMA_STM32_HAL_INCLUDE
    #ifdef CONFIG_LV_NEMA_STM32_HAL_INCLUDE
        #define LV_NEMA_STM32_HAL_INCLUDE CONFIG_LV_NEMA_STM32_HAL_INCLUDE
    #else
        #define LV_NEMA_STM32_HAL_INCLUDE "stm32u5xx_hal.h"
    #endif
#endif

#ifndef LV_NEMA_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_NEMA_USE_CUSTOM_INCLUDE
        #define LV_NEMA_USE_CUSTOM_INCLUDE CONFIG_LV_NEMA_USE_CUSTOM_INCLUDE
    #else
        #define LV_NEMA_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_NEMA_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_NEMA_CUSTOM_INCLUDE
        #define LV_NEMA_CUSTOM_INCLUDE CONFIG_LV_NEMA_CUSTOM_INCLUDE
    #else
        #define LV_NEMA_CUSTOM_INCLUDE ""
    #endif
#endif

#ifndef LV_USE_NEMA_VG
    #ifdef CONFIG_LV_USE_NEMA_VG
        #define LV_USE_NEMA_VG CONFIG_LV_USE_NEMA_VG
    #else
        #define LV_USE_NEMA_VG 0
    #endif
#endif

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

#ifndef LV_USE_DRAW_PXP
    #ifdef CONFIG_LV_USE_DRAW_PXP
        #define LV_USE_DRAW_PXP CONFIG_LV_USE_DRAW_PXP
    #else
        #define LV_USE_DRAW_PXP 0
    #endif
#endif

#ifndef LV_USE_ROTATE_PXP
    #ifdef CONFIG_LV_USE_ROTATE_PXP
        #define LV_USE_ROTATE_PXP CONFIG_LV_USE_ROTATE_PXP
    #else
        #define LV_USE_ROTATE_PXP 0
    #endif
#endif

#ifndef LV_USE_PXP_DRAW_THREAD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_PXP_DRAW_THREAD
            #define LV_USE_PXP_DRAW_THREAD CONFIG_LV_USE_PXP_DRAW_THREAD
        #else
            #define LV_USE_PXP_DRAW_THREAD 0
        #endif
    #else
          #define LV_USE_PXP_DRAW_THREAD !(LV_USE_OS == LV_OS_NONE) && LV_USE_DRAW_PXP
    #endif
#endif

#ifndef LV_USE_PXP_ASSERT
    #ifdef CONFIG_LV_USE_PXP_ASSERT
        #define LV_USE_PXP_ASSERT CONFIG_LV_USE_PXP_ASSERT
    #else
        #define LV_USE_PXP_ASSERT 0
    #endif
#endif

#ifndef LV_USE_PPA
    #ifdef CONFIG_LV_USE_PPA
        #define LV_USE_PPA CONFIG_LV_USE_PPA
    #else
        #define LV_USE_PPA 0
    #endif
#endif

#ifndef LV_USE_PPA_IMG
    #ifdef CONFIG_LV_USE_PPA_IMG
        #define LV_USE_PPA_IMG CONFIG_LV_USE_PPA_IMG
    #else
        #define LV_USE_PPA_IMG 0
    #endif
#endif

#ifndef LV_PPA_BURST_LENGTH
    #ifdef CONFIG_LV_PPA_BURST_LENGTH
        #define LV_PPA_BURST_LENGTH CONFIG_LV_PPA_BURST_LENGTH
    #else
        #define LV_PPA_BURST_LENGTH 128
    #endif
#endif

#ifndef LV_USE_DRAW_DMA2D
    #ifdef CONFIG_LV_USE_DRAW_DMA2D
        #define LV_USE_DRAW_DMA2D CONFIG_LV_USE_DRAW_DMA2D
    #else
        #define LV_USE_DRAW_DMA2D 0
    #endif
#endif

#ifndef LV_DRAW_DMA2D_HAL_INCLUDE
    #ifdef CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
        #define LV_DRAW_DMA2D_HAL_INCLUDE CONFIG_LV_DRAW_DMA2D_HAL_INCLUDE
    #else
        #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"
    #endif
#endif

#ifndef LV_USE_DRAW_DMA2D_INTERRUPT
    #ifdef CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
        #define LV_USE_DRAW_DMA2D_INTERRUPT CONFIG_LV_USE_DRAW_DMA2D_INTERRUPT
    #else
        #define LV_USE_DRAW_DMA2D_INTERRUPT 0
    #endif
#endif

#ifndef LV_USE_DRAW_EVE
    #ifdef CONFIG_LV_USE_DRAW_EVE
        #define LV_USE_DRAW_EVE CONFIG_LV_USE_DRAW_EVE
    #else
        #define LV_USE_DRAW_EVE 0
    #endif
#endif

#ifndef LV_DRAW_EVE_EVE_GENERATION
    #ifdef CONFIG_LV_DRAW_EVE_EVE_GENERATION
        #define LV_DRAW_EVE_EVE_GENERATION CONFIG_LV_DRAW_EVE_EVE_GENERATION
    #else
        #define LV_DRAW_EVE_EVE_GENERATION 4
    #endif
#endif

#ifndef LV_DRAW_EVE_WRITE_BUFFER_SIZE
    #ifdef CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
        #define LV_DRAW_EVE_WRITE_BUFFER_SIZE CONFIG_LV_DRAW_EVE_WRITE_BUFFER_SIZE
    #else
        #define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048
    #endif
#endif

#ifndef LV_USE_DRAW_G2D
    #ifdef CONFIG_LV_USE_DRAW_G2D
        #define LV_USE_DRAW_G2D CONFIG_LV_USE_DRAW_G2D
    #else
        #define LV_USE_DRAW_G2D 0
    #endif
#endif

#ifndef LV_G2D_HASH_TABLE_SIZE
    #ifdef CONFIG_LV_G2D_HASH_TABLE_SIZE
        #define LV_G2D_HASH_TABLE_SIZE CONFIG_LV_G2D_HASH_TABLE_SIZE
    #else
        #define LV_G2D_HASH_TABLE_SIZE 50
    #endif
#endif

#ifndef LV_USE_G2D_DRAW_THREAD
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_G2D_DRAW_THREAD
            #define LV_USE_G2D_DRAW_THREAD CONFIG_LV_USE_G2D_DRAW_THREAD
        #else
            #define LV_USE_G2D_DRAW_THREAD 0
        #endif
    #else
          #define LV_USE_G2D_DRAW_THREAD !(LV_USE_OS == LV_OS_NONE) && LV_USE_DRAW_G2D
    #endif
#endif

#ifndef LV_USE_G2D_ASSERT
    #ifdef CONFIG_LV_USE_G2D_ASSERT
        #define LV_USE_G2D_ASSERT CONFIG_LV_USE_G2D_ASSERT
    #else
        #define LV_USE_G2D_ASSERT 0
    #endif
#endif

#ifndef LV_USE_DRAW_NANOVG
    #ifdef CONFIG_LV_USE_DRAW_NANOVG
        #define LV_USE_DRAW_NANOVG CONFIG_LV_USE_DRAW_NANOVG
    #else
        #define LV_USE_DRAW_NANOVG 0
    #endif
#endif

#ifndef LV_NANOVG_BACKEND
    #ifdef CONFIG_LV_NANOVG_BACKEND
        #define LV_NANOVG_BACKEND CONFIG_LV_NANOVG_BACKEND
    #else
        #define LV_NANOVG_BACKEND LV_NANOVG_BACKEND_GLES2
    #endif
#endif

#ifndef LV_NANOVG_IMAGE_CACHE_CNT
    #ifdef CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
        #define LV_NANOVG_IMAGE_CACHE_CNT CONFIG_LV_NANOVG_IMAGE_CACHE_CNT
    #else
        #define LV_NANOVG_IMAGE_CACHE_CNT 128
    #endif
#endif

#ifndef LV_NANOVG_LETTER_CACHE_CNT
    #ifdef CONFIG_LV_NANOVG_LETTER_CACHE_CNT
        #define LV_NANOVG_LETTER_CACHE_CNT CONFIG_LV_NANOVG_LETTER_CACHE_CNT
    #else
        #define LV_NANOVG_LETTER_CACHE_CNT 512
    #endif
#endif

#ifndef LV_USE_DRAW_OPENGLES
    #ifdef CONFIG_LV_USE_DRAW_OPENGLES
        #define LV_USE_DRAW_OPENGLES CONFIG_LV_USE_DRAW_OPENGLES
    #else
        #define LV_USE_DRAW_OPENGLES 0
    #endif
#endif

#ifndef LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
    #ifdef CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
        #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT CONFIG_LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT
    #else
        #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64
    #endif
#endif

#ifndef LV_USE_DRAW_SDL
    #ifdef CONFIG_LV_USE_DRAW_SDL
        #define LV_USE_DRAW_SDL CONFIG_LV_USE_DRAW_SDL
    #else
        #define LV_USE_DRAW_SDL 0
    #endif
#endif



/*============================================================================
 * INPUT DEVICES
 *============================================================================*/

#ifndef LV_USE_GRIDNAV
    #ifdef CONFIG_LV_USE_GRIDNAV
        #define LV_USE_GRIDNAV CONFIG_LV_USE_GRIDNAV
    #else
        #define LV_USE_GRIDNAV 0
    #endif
#endif

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

#ifndef LV_OBJ_STYLE_CACHE
    #ifdef CONFIG_LV_OBJ_STYLE_CACHE
        #define LV_OBJ_STYLE_CACHE CONFIG_LV_OBJ_STYLE_CACHE
    #else
        #define LV_OBJ_STYLE_CACHE 0
    #endif
#endif

#ifndef LV_USE_OBJ_NAME
    #ifdef CONFIG_LV_USE_OBJ_NAME
        #define LV_USE_OBJ_NAME CONFIG_LV_USE_OBJ_NAME
    #else
        #define LV_USE_OBJ_NAME 0
    #endif
#endif

#ifndef LV_USE_OBJ_ID
    #ifdef CONFIG_LV_USE_OBJ_ID
        #define LV_USE_OBJ_ID CONFIG_LV_USE_OBJ_ID
    #else
        #define LV_USE_OBJ_ID 0
    #endif
#endif

#ifndef LV_OBJ_ID_AUTO_ASSIGN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_OBJ_ID_AUTO_ASSIGN
            #define LV_OBJ_ID_AUTO_ASSIGN CONFIG_LV_OBJ_ID_AUTO_ASSIGN
        #else
            #define LV_OBJ_ID_AUTO_ASSIGN 0
        #endif
    #else
          #define LV_OBJ_ID_AUTO_ASSIGN LV_USE_OBJ_ID
    #endif
#endif

#ifndef LV_USE_OBJ_ID_BUILTIN
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_ID_BUILTIN
            #define LV_USE_OBJ_ID_BUILTIN CONFIG_LV_USE_OBJ_ID_BUILTIN
        #else
            #define LV_USE_OBJ_ID_BUILTIN 0
        #endif
    #else
          #define LV_USE_OBJ_ID_BUILTIN LV_USE_OBJ_ID
    #endif
#endif

#ifndef LV_USE_OBJ_PROPERTY
    #ifdef CONFIG_LV_USE_OBJ_PROPERTY
        #define LV_USE_OBJ_PROPERTY CONFIG_LV_USE_OBJ_PROPERTY
    #else
        #define LV_USE_OBJ_PROPERTY 0
    #endif
#endif

#ifndef LV_USE_OBJ_PROPERTY_NAME
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_OBJ_PROPERTY_NAME
            #define LV_USE_OBJ_PROPERTY_NAME CONFIG_LV_USE_OBJ_PROPERTY_NAME
        #else
            #define LV_USE_OBJ_PROPERTY_NAME 0
        #endif
    #else
          #define LV_USE_OBJ_PROPERTY_NAME LV_USE_OBJ_PROPERTY
    #endif
#endif

#ifndef LV_USE_EXT_DATA
    #ifdef CONFIG_LV_USE_EXT_DATA
        #define LV_USE_EXT_DATA CONFIG_LV_USE_EXT_DATA
    #else
        #define LV_USE_EXT_DATA 0
    #endif
#endif

#ifndef LV_USE_FLOAT
    #ifdef CONFIG_LV_USE_FLOAT
        #define LV_USE_FLOAT CONFIG_LV_USE_FLOAT
    #else
        #define LV_USE_FLOAT 0
    #endif
#endif

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

#ifndef LV_USE_TRANSLATION
    #ifdef CONFIG_LV_USE_TRANSLATION
        #define LV_USE_TRANSLATION CONFIG_LV_USE_TRANSLATION
    #else
        #define LV_USE_TRANSLATION 0
    #endif
#endif

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

#ifndef LV_USE_LOG
    #ifdef CONFIG_LV_USE_LOG
        #define LV_USE_LOG CONFIG_LV_USE_LOG
    #else
        #define LV_USE_LOG 0
    #endif
#endif

#ifndef LV_LOG_LEVEL
    #ifdef CONFIG_LV_LOG_LEVEL
        #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
    #else
        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #endif
#endif

#ifndef LV_LOG_PRINTF
    #ifdef CONFIG_LV_LOG_PRINTF
        #define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
    #else
        #define LV_LOG_PRINTF 0
    #endif
#endif

#ifndef LV_LOG_USE_TIMESTAMP
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_USE_TIMESTAMP
            #define LV_LOG_USE_TIMESTAMP CONFIG_LV_LOG_USE_TIMESTAMP
        #else
            #define LV_LOG_USE_TIMESTAMP 0
        #endif
    #else
          #define LV_LOG_USE_TIMESTAMP LV_USE_LOG
    #endif
#endif

#ifndef LV_LOG_USE_FILE_LINE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_USE_FILE_LINE
            #define LV_LOG_USE_FILE_LINE CONFIG_LV_LOG_USE_FILE_LINE
        #else
            #define LV_LOG_USE_FILE_LINE 0
        #endif
    #else
          #define LV_LOG_USE_FILE_LINE LV_USE_LOG
    #endif
#endif

#ifndef LV_LOG_TRACE_MEM
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LOG_TRACE_MEM
            #define LV_LOG_TRACE_MEM CONFIG_LV_LOG_TRACE_MEM
        #else
            #define LV_LOG_TRACE_MEM 0
        #endif
    #else
          #define LV_LOG_TRACE_MEM LV_USE_LOG
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
          #define LV_LOG_TRACE_TIMER LV_USE_LOG
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
          #define LV_LOG_TRACE_INDEV LV_USE_LOG
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
          #define LV_LOG_TRACE_DISP_REFR LV_USE_LOG
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
          #define LV_LOG_TRACE_EVENT LV_USE_LOG
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
          #define LV_LOG_TRACE_OBJ_CREATE LV_USE_LOG
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
          #define LV_LOG_TRACE_LAYOUT LV_USE_LOG
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
          #define LV_LOG_TRACE_ANIM LV_USE_LOG
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
          #define LV_LOG_TRACE_CACHE LV_USE_LOG
    #endif
#endif



/*============================================================================
 * THEMES
 *============================================================================*/

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

#ifndef LV_THEME_DEFAULT_DARK
    #ifdef CONFIG_LV_THEME_DEFAULT_DARK
        #define LV_THEME_DEFAULT_DARK CONFIG_LV_THEME_DEFAULT_DARK
    #else
        #define LV_THEME_DEFAULT_DARK 0
    #endif
#endif

#ifndef LV_THEME_DEFAULT_GROW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_THEME_DEFAULT_GROW
            #define LV_THEME_DEFAULT_GROW CONFIG_LV_THEME_DEFAULT_GROW
        #else
            #define LV_THEME_DEFAULT_GROW 0
        #endif
    #else
          #define LV_THEME_DEFAULT_GROW LV_USE_THEME_DEFAULT
    #endif
#endif

#ifndef LV_THEME_DEFAULT_TRANSITION_TIME
    #ifdef CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
        #define LV_THEME_DEFAULT_TRANSITION_TIME CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
    #else
        #define LV_THEME_DEFAULT_TRANSITION_TIME 80
    #endif
#endif

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

#ifndef LV_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_CACHE_DEF_SIZE
        #define LV_CACHE_DEF_SIZE CONFIG_LV_CACHE_DEF_SIZE
    #else
        #define LV_CACHE_DEF_SIZE 0
    #endif
#endif

#ifndef LV_IMAGE_HEADER_CACHE_DEF_CNT
    #ifdef CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT CONFIG_LV_IMAGE_HEADER_CACHE_DEF_CNT
    #else
        #define LV_IMAGE_HEADER_CACHE_DEF_CNT 0
    #endif
#endif

#ifndef LV_USE_RLE
    #ifdef CONFIG_LV_USE_RLE
        #define LV_USE_RLE CONFIG_LV_USE_RLE
    #else
        #define LV_USE_RLE 0
    #endif
#endif

#ifndef LV_USE_LZ4
    #ifdef CONFIG_LV_USE_LZ4
        #define LV_USE_LZ4 CONFIG_LV_USE_LZ4
    #else
        #define LV_USE_LZ4 0
    #endif
#endif

#ifndef LV_USE_LZ4_INTERNAL
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LZ4_INTERNAL
            #define LV_USE_LZ4_INTERNAL CONFIG_LV_USE_LZ4_INTERNAL
        #else
            #define LV_USE_LZ4_INTERNAL 0
        #endif
    #else
          #define LV_USE_LZ4_INTERNAL LV_USE_LZ4
    #endif
#endif

#ifndef LV_BIN_DECODER_RAM_LOAD
    #ifdef CONFIG_LV_BIN_DECODER_RAM_LOAD
        #define LV_BIN_DECODER_RAM_LOAD CONFIG_LV_BIN_DECODER_RAM_LOAD
    #else
        #define LV_BIN_DECODER_RAM_LOAD 0
    #endif
#endif

#ifndef LV_USE_LODEPNG
    #ifdef CONFIG_LV_USE_LODEPNG
        #define LV_USE_LODEPNG CONFIG_LV_USE_LODEPNG
    #else
        #define LV_USE_LODEPNG 0
    #endif
#endif

#ifndef LV_USE_LIBPNG
    #ifdef CONFIG_LV_USE_LIBPNG
        #define LV_USE_LIBPNG CONFIG_LV_USE_LIBPNG
    #else
        #define LV_USE_LIBPNG 0
    #endif
#endif

#ifndef LV_USE_BMP
    #ifdef CONFIG_LV_USE_BMP
        #define LV_USE_BMP CONFIG_LV_USE_BMP
    #else
        #define LV_USE_BMP 0
    #endif
#endif

#ifndef LV_USE_TJPGD
    #ifdef CONFIG_LV_USE_TJPGD
        #define LV_USE_TJPGD CONFIG_LV_USE_TJPGD
    #else
        #define LV_USE_TJPGD 0
    #endif
#endif

#ifndef LV_USE_LIBJPEG_TURBO
    #ifdef CONFIG_LV_USE_LIBJPEG_TURBO
        #define LV_USE_LIBJPEG_TURBO CONFIG_LV_USE_LIBJPEG_TURBO
    #else
        #define LV_USE_LIBJPEG_TURBO 0
    #endif
#endif

#ifndef LV_USE_LIBWEBP
    #ifdef CONFIG_LV_USE_LIBWEBP
        #define LV_USE_LIBWEBP CONFIG_LV_USE_LIBWEBP
    #else
        #define LV_USE_LIBWEBP 0
    #endif
#endif

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



/*============================================================================
 * TEXT & FONT SETTINGS
 *============================================================================*/

#ifndef LV_TXT_ENC
    #ifdef CONFIG_LV_TXT_ENC
        #define LV_TXT_ENC CONFIG_LV_TXT_ENC
    #else
        #define LV_TXT_ENC LV_TXT_ENC_UTF8
    #endif
#endif

#ifndef LV_TXT_BREAK_CHARS
    #ifdef CONFIG_LV_TXT_BREAK_CHARS
        #define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS
    #else
        #define LV_TXT_BREAK_CHARS " ,.;:-_)]}"
    #endif
#endif

#ifndef LV_TXT_LINE_BREAK_LONG_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
        #define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_LEN 0
    #endif
#endif

#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
    #endif
#endif

#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
    #endif
#endif

#ifndef LV_TXT_COLOR_CMD
    #ifdef CONFIG_LV_TXT_COLOR_CMD
        #define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
    #else
        #define LV_TXT_COLOR_CMD "#"
    #endif
#endif

#ifndef LV_USE_BIDI
    #ifdef CONFIG_LV_USE_BIDI
        #define LV_USE_BIDI CONFIG_LV_USE_BIDI
    #else
        #define LV_USE_BIDI 0
    #endif
#endif

#ifndef LV_BIDI_BASE_DIR_DEF
    #ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
        #define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
    #else
        #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
    #endif
#endif

#ifndef LV_USE_ARABIC_PERSIAN_CHARS
    #ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
        #define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
    #else
        #define LV_USE_ARABIC_PERSIAN_CHARS 0
    #endif
#endif

#ifndef LV_FONT_MONTSERRAT_8
    #ifdef CONFIG_LV_FONT_MONTSERRAT_8
        #define LV_FONT_MONTSERRAT_8 CONFIG_LV_FONT_MONTSERRAT_8
    #else
        #define LV_FONT_MONTSERRAT_8 0
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

#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
        #define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
    #else
        #define LV_FONT_MONTSERRAT_28_COMPRESSED 0
    #endif
#endif

#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #else
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0
    #endif
#endif

#ifndef LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_14_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0
    #endif
#endif

#ifndef LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #ifdef CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK CONFIG_LV_FONT_SOURCE_HAN_SANS_SC_16_CJK
    #else
        #define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0
    #endif
#endif

#ifndef LV_FONT_UNSCII_8
    #ifdef CONFIG_LV_FONT_UNSCII_8
        #define LV_FONT_UNSCII_8 CONFIG_LV_FONT_UNSCII_8
    #else
        #define LV_FONT_UNSCII_8 0
    #endif
#endif

#ifndef LV_FONT_UNSCII_16
    #ifdef CONFIG_LV_FONT_UNSCII_16
        #define LV_FONT_UNSCII_16 CONFIG_LV_FONT_UNSCII_16
    #else
        #define LV_FONT_UNSCII_16 0
    #endif
#endif

#ifndef LV_FONT_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_FONT_USE_CUSTOM_INCLUDE
        #define LV_FONT_USE_CUSTOM_INCLUDE CONFIG_LV_FONT_USE_CUSTOM_INCLUDE
    #else
        #define LV_FONT_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_FONT_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_FONT_CUSTOM_INCLUDE
        #define LV_FONT_CUSTOM_INCLUDE CONFIG_LV_FONT_CUSTOM_INCLUDE
    #else
        #define LV_FONT_CUSTOM_INCLUDE ""
    #endif
#endif

#ifndef LV_USE_CUSTOM_FONT_DEFAULT
    #ifdef CONFIG_LV_USE_CUSTOM_FONT_DEFAULT
        #define LV_USE_CUSTOM_FONT_DEFAULT CONFIG_LV_USE_CUSTOM_FONT_DEFAULT
    #else
        #define LV_USE_CUSTOM_FONT_DEFAULT 0
    #endif
#endif

#ifndef LV_FONT_DEFAULT
    #ifdef CONFIG_LV_FONT_DEFAULT
        #define LV_FONT_DEFAULT CONFIG_LV_FONT_DEFAULT
    #else
        #define LV_FONT_DEFAULT LV_FONT_DEFAULT_MONTSERRAT_14
    #endif
#endif

#ifndef LV_FONT_FMT_TXT_LARGE
    #ifdef CONFIG_LV_FONT_FMT_TXT_LARGE
        #define LV_FONT_FMT_TXT_LARGE CONFIG_LV_FONT_FMT_TXT_LARGE
    #else
        #define LV_FONT_FMT_TXT_LARGE 0
    #endif
#endif

#ifndef LV_USE_FONT_COMPRESSED
    #ifdef CONFIG_LV_USE_FONT_COMPRESSED
        #define LV_USE_FONT_COMPRESSED CONFIG_LV_USE_FONT_COMPRESSED
    #else
        #define LV_USE_FONT_COMPRESSED 0
    #endif
#endif

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

#ifndef LV_USE_FONT_MANAGER
    #ifdef CONFIG_LV_USE_FONT_MANAGER
        #define LV_USE_FONT_MANAGER CONFIG_LV_USE_FONT_MANAGER
    #else
        #define LV_USE_FONT_MANAGER 0
    #endif
#endif

#ifndef LV_FONT_MANAGER_NAME_MAX_LEN
    #ifdef CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
        #define LV_FONT_MANAGER_NAME_MAX_LEN CONFIG_LV_FONT_MANAGER_NAME_MAX_LEN
    #else
        #define LV_FONT_MANAGER_NAME_MAX_LEN 32
    #endif
#endif

#ifndef LV_USE_IMGFONT
    #ifdef CONFIG_LV_USE_IMGFONT
        #define LV_USE_IMGFONT CONFIG_LV_USE_IMGFONT
    #else
        #define LV_USE_IMGFONT 0
    #endif
#endif

#ifndef LV_USE_FREETYPE
    #ifdef CONFIG_LV_USE_FREETYPE
        #define LV_USE_FREETYPE CONFIG_LV_USE_FREETYPE
    #else
        #define LV_USE_FREETYPE 0
    #endif
#endif

#ifndef LV_FREETYPE_USE_LVGL_PORT
    #ifdef CONFIG_LV_FREETYPE_USE_LVGL_PORT
        #define LV_FREETYPE_USE_LVGL_PORT CONFIG_LV_FREETYPE_USE_LVGL_PORT
    #else
        #define LV_FREETYPE_USE_LVGL_PORT 0
    #endif
#endif

#ifndef LV_FREETYPE_CACHE_FT_GLYPH_CNT
    #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
        #define LV_FREETYPE_CACHE_FT_GLYPH_CNT CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_CNT
    #else
        #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
    #endif
#endif

#ifndef LV_FREETYPE_CACHE_FT_GLYPH_L1
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_L1
            #define LV_FREETYPE_CACHE_FT_GLYPH_L1 CONFIG_LV_FREETYPE_CACHE_FT_GLYPH_L1
        #else
            #define LV_FREETYPE_CACHE_FT_GLYPH_L1 0
        #endif
    #else
          #define LV_FREETYPE_CACHE_FT_GLYPH_L1 LV_USE_OS == LV_OS_NONE && LV_USE_FREETYPE
    #endif
#endif

#ifndef LV_USE_TINY_TTF
    #ifdef CONFIG_LV_USE_TINY_TTF
        #define LV_USE_TINY_TTF CONFIG_LV_USE_TINY_TTF
    #else
        #define LV_USE_TINY_TTF 0
    #endif
#endif

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
        #define LV_TINY_TTF_CACHE_GLYPH_CNT 128
    #endif
#endif

#ifndef LV_TINY_TTF_CACHE_KERNING_CNT
    #ifdef CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
        #define LV_TINY_TTF_CACHE_KERNING_CNT CONFIG_LV_TINY_TTF_CACHE_KERNING_CNT
    #else
        #define LV_TINY_TTF_CACHE_KERNING_CNT 256
    #endif
#endif



/*============================================================================
 * WIDGETS
 *============================================================================*/

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

#ifndef LV_USE_3DTEXTURE
    #ifdef CONFIG_LV_USE_3DTEXTURE
        #define LV_USE_3DTEXTURE CONFIG_LV_USE_3DTEXTURE
    #else
        #define LV_USE_3DTEXTURE 0
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
        #define LV_USE_ANIMIMG 1
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
        #define LV_USE_ARC 1
    #endif
#endif

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

#ifndef LV_USE_BARCODE
    #ifdef CONFIG_LV_USE_BARCODE
        #define LV_USE_BARCODE CONFIG_LV_USE_BARCODE
    #else
        #define LV_USE_BARCODE 0
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
        #define LV_USE_BUTTON 1
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
        #define LV_USE_BUTTONMATRIX 1
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
        #define LV_USE_CALENDAR 1
    #endif
#endif

#ifndef LV_CALENDAR_WEEK_STARTS_MONDAY
    #ifdef CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_WEEK_STARTS_MONDAY CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
    #else
        #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #endif
#endif

#ifndef LV_MONDAY_STR
    #ifdef CONFIG_LV_MONDAY_STR
        #define LV_MONDAY_STR CONFIG_LV_MONDAY_STR
    #else
        #define LV_MONDAY_STR "Mo"
    #endif
#endif

#ifndef LV_TUESDAY_STR
    #ifdef CONFIG_LV_TUESDAY_STR
        #define LV_TUESDAY_STR CONFIG_LV_TUESDAY_STR
    #else
        #define LV_TUESDAY_STR "Tu"
    #endif
#endif

#ifndef LV_WEDNESDAY_STR
    #ifdef CONFIG_LV_WEDNESDAY_STR
        #define LV_WEDNESDAY_STR CONFIG_LV_WEDNESDAY_STR
    #else
        #define LV_WEDNESDAY_STR "We"
    #endif
#endif

#ifndef LV_THURSDAY_STR
    #ifdef CONFIG_LV_THURSDAY_STR
        #define LV_THURSDAY_STR CONFIG_LV_THURSDAY_STR
    #else
        #define LV_THURSDAY_STR "Th"
    #endif
#endif

#ifndef LV_FRIDAY_STR
    #ifdef CONFIG_LV_FRIDAY_STR
        #define LV_FRIDAY_STR CONFIG_LV_FRIDAY_STR
    #else
        #define LV_FRIDAY_STR "Fr"
    #endif
#endif

#ifndef LV_SATURDAY_STR
    #ifdef CONFIG_LV_SATURDAY_STR
        #define LV_SATURDAY_STR CONFIG_LV_SATURDAY_STR
    #else
        #define LV_SATURDAY_STR "Sa"
    #endif
#endif

#ifndef LV_SUNDAY_STR
    #ifdef CONFIG_LV_SUNDAY_STR
        #define LV_SUNDAY_STR CONFIG_LV_SUNDAY_STR
    #else
        #define LV_SUNDAY_STR "Su"
    #endif
#endif

#ifndef LV_JANUARY_STR
    #ifdef CONFIG_LV_JANUARY_STR
        #define LV_JANUARY_STR CONFIG_LV_JANUARY_STR
    #else
        #define LV_JANUARY_STR "January"
    #endif
#endif

#ifndef LV_FEBRUARY_STR
    #ifdef CONFIG_LV_FEBRUARY_STR
        #define LV_FEBRUARY_STR CONFIG_LV_FEBRUARY_STR
    #else
        #define LV_FEBRUARY_STR "February"
    #endif
#endif

#ifndef LV_MARCH_STR
    #ifdef CONFIG_LV_MARCH_STR
        #define LV_MARCH_STR CONFIG_LV_MARCH_STR
    #else
        #define LV_MARCH_STR "March"
    #endif
#endif

#ifndef LV_APRIL_STR
    #ifdef CONFIG_LV_APRIL_STR
        #define LV_APRIL_STR CONFIG_LV_APRIL_STR
    #else
        #define LV_APRIL_STR "April"
    #endif
#endif

#ifndef LV_MAY_STR
    #ifdef CONFIG_LV_MAY_STR
        #define LV_MAY_STR CONFIG_LV_MAY_STR
    #else
        #define LV_MAY_STR "May"
    #endif
#endif

#ifndef LV_JUNE_STR
    #ifdef CONFIG_LV_JUNE_STR
        #define LV_JUNE_STR CONFIG_LV_JUNE_STR
    #else
        #define LV_JUNE_STR "June"
    #endif
#endif

#ifndef LV_JULY_STR
    #ifdef CONFIG_LV_JULY_STR
        #define LV_JULY_STR CONFIG_LV_JULY_STR
    #else
        #define LV_JULY_STR "July"
    #endif
#endif

#ifndef LV_AUGUST_STR
    #ifdef CONFIG_LV_AUGUST_STR
        #define LV_AUGUST_STR CONFIG_LV_AUGUST_STR
    #else
        #define LV_AUGUST_STR "August"
    #endif
#endif

#ifndef LV_SEPTEMBER_STR
    #ifdef CONFIG_LV_SEPTEMBER_STR
        #define LV_SEPTEMBER_STR CONFIG_LV_SEPTEMBER_STR
    #else
        #define LV_SEPTEMBER_STR "September"
    #endif
#endif

#ifndef LV_OCTOBER_STR
    #ifdef CONFIG_LV_OCTOBER_STR
        #define LV_OCTOBER_STR CONFIG_LV_OCTOBER_STR
    #else
        #define LV_OCTOBER_STR "October"
    #endif
#endif

#ifndef LV_NOVEMBER_STR
    #ifdef CONFIG_LV_NOVEMBER_STR
        #define LV_NOVEMBER_STR CONFIG_LV_NOVEMBER_STR
    #else
        #define LV_NOVEMBER_STR "November"
    #endif
#endif

#ifndef LV_DECEMBER_STR
    #ifdef CONFIG_LV_DECEMBER_STR
        #define LV_DECEMBER_STR CONFIG_LV_DECEMBER_STR
    #else
        #define LV_DECEMBER_STR "December"
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
          #define LV_USE_CALENDAR_HEADER_ARROW LV_USE_CALENDAR
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
          #define LV_USE_CALENDAR_HEADER_DROPDOWN LV_USE_CALENDAR
    #endif
#endif

#ifndef LV_USE_CALENDAR_CHINESE
    #ifdef CONFIG_LV_USE_CALENDAR_CHINESE
        #define LV_USE_CALENDAR_CHINESE CONFIG_LV_USE_CALENDAR_CHINESE
    #else
        #define LV_USE_CALENDAR_CHINESE 0
    #endif
#endif

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

#ifndef LV_USE_FFMPEG
    #ifdef CONFIG_LV_USE_FFMPEG
        #define LV_USE_FFMPEG CONFIG_LV_USE_FFMPEG
    #else
        #define LV_USE_FFMPEG 0
    #endif
#endif

#ifndef LV_FFMPEG_DUMP_FORMAT
    #ifdef CONFIG_LV_FFMPEG_DUMP_FORMAT
        #define LV_FFMPEG_DUMP_FORMAT CONFIG_LV_FFMPEG_DUMP_FORMAT
    #else
        #define LV_FFMPEG_DUMP_FORMAT 0
    #endif
#endif

#ifndef LV_FFMPEG_PLAYER_USE_LV_FS
    #ifdef CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
        #define LV_FFMPEG_PLAYER_USE_LV_FS CONFIG_LV_FFMPEG_PLAYER_USE_LV_FS
    #else
        #define LV_FFMPEG_PLAYER_USE_LV_FS 0
    #endif
#endif

#ifndef LV_USE_GIF
    #ifdef CONFIG_LV_USE_GIF
        #define LV_USE_GIF CONFIG_LV_USE_GIF
    #else
        #define LV_USE_GIF 0
    #endif
#endif

#ifndef LV_GIF_CACHE_DECODE_DATA
    #ifdef CONFIG_LV_GIF_CACHE_DECODE_DATA
        #define LV_GIF_CACHE_DECODE_DATA CONFIG_LV_GIF_CACHE_DECODE_DATA
    #else
        #define LV_GIF_CACHE_DECODE_DATA 0
    #endif
#endif

#ifndef LV_GIF_MAX_WIDTH
    #ifdef CONFIG_LV_GIF_MAX_WIDTH
        #define LV_GIF_MAX_WIDTH CONFIG_LV_GIF_MAX_WIDTH
    #else
        #define LV_GIF_MAX_WIDTH 480
    #endif
#endif

#ifndef LV_GIF_MAX_HEIGHT
    #ifdef CONFIG_LV_GIF_MAX_HEIGHT
        #define LV_GIF_MAX_HEIGHT CONFIG_LV_GIF_MAX_HEIGHT
    #else
        #define LV_GIF_MAX_HEIGHT 32768
    #endif
#endif

#ifndef LV_USE_GLTF
    #ifdef CONFIG_LV_USE_GLTF
        #define LV_USE_GLTF CONFIG_LV_USE_GLTF
    #else
        #define LV_USE_GLTF 0
    #endif
#endif

#ifndef LV_USE_GSTREAMER
    #ifdef CONFIG_LV_USE_GSTREAMER
        #define LV_USE_GSTREAMER CONFIG_LV_USE_GSTREAMER
    #else
        #define LV_USE_GSTREAMER 0
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
        #define LV_USE_IMAGE 1
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
        #define LV_USE_IMAGEBUTTON 1
    #endif
#endif

#ifndef LV_USE_IME_PINYIN
    #ifdef CONFIG_LV_USE_IME_PINYIN
        #define LV_USE_IME_PINYIN CONFIG_LV_USE_IME_PINYIN
    #else
        #define LV_USE_IME_PINYIN 0
    #endif
#endif

#ifndef LV_IME_PINYIN_USE_K9_MODE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_IME_PINYIN_USE_K9_MODE
            #define LV_IME_PINYIN_USE_K9_MODE CONFIG_LV_IME_PINYIN_USE_K9_MODE
        #else
            #define LV_IME_PINYIN_USE_K9_MODE 0
        #endif
    #else
          #define LV_IME_PINYIN_USE_K9_MODE LV_USE_IME_PINYIN
    #endif
#endif

#ifndef LV_IME_PINYIN_K9_CAND_TEXT_NUM
    #ifdef CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_K9_CAND_TEXT_NUM
    #else
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif
#endif

#ifndef LV_IME_PINYIN_USE_DEFAULT_DICT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
            #define LV_IME_PINYIN_USE_DEFAULT_DICT CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
        #else
            #define LV_IME_PINYIN_USE_DEFAULT_DICT 0
        #endif
    #else
          #define LV_IME_PINYIN_USE_DEFAULT_DICT LV_USE_IME_PINYIN
    #endif
#endif

#ifndef LV_IME_PINYIN_CAND_TEXT_NUM
    #ifdef CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
        #define LV_IME_PINYIN_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
    #else
        #define LV_IME_PINYIN_CAND_TEXT_NUM 6
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
        #define LV_USE_KEYBOARD 1
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
        #define LV_USE_LABEL 1
    #endif
#endif

#ifndef LV_LABEL_TEXT_SELECTION
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LABEL_TEXT_SELECTION
            #define LV_LABEL_TEXT_SELECTION CONFIG_LV_LABEL_TEXT_SELECTION
        #else
            #define LV_LABEL_TEXT_SELECTION 0
        #endif
    #else
          #define LV_LABEL_TEXT_SELECTION LV_USE_LABEL
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
          #define LV_LABEL_LONG_TXT_HINT LV_USE_LABEL
    #endif
#endif

#ifndef LV_LABEL_WAIT_CHAR_COUNT
    #ifdef CONFIG_LV_LABEL_WAIT_CHAR_COUNT
        #define LV_LABEL_WAIT_CHAR_COUNT CONFIG_LV_LABEL_WAIT_CHAR_COUNT
    #else
        #define LV_LABEL_WAIT_CHAR_COUNT 3
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
        #define LV_USE_LED 1
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
        #define LV_USE_LINE 1
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
        #define LV_USE_LIST 1
    #endif
#endif

#ifndef LV_USE_LOTTIE
    #ifdef CONFIG_LV_USE_LOTTIE
        #define LV_USE_LOTTIE CONFIG_LV_USE_LOTTIE
    #else
        #define LV_USE_LOTTIE 0
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
        #define LV_USE_MENU 1
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
        #define LV_USE_MSGBOX 1
    #endif
#endif

#ifndef LV_USE_QRCODE
    #ifdef CONFIG_LV_USE_QRCODE
        #define LV_USE_QRCODE CONFIG_LV_USE_QRCODE
    #else
        #define LV_USE_QRCODE 0
    #endif
#endif

#ifndef LV_USE_RLOTTIE
    #ifdef CONFIG_LV_USE_RLOTTIE
        #define LV_USE_RLOTTIE CONFIG_LV_USE_RLOTTIE
    #else
        #define LV_USE_RLOTTIE 0
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
        #define LV_USE_ROLLER 1
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
        #define LV_USE_SCALE 1
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
        #define LV_USE_SLIDER 1
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
        #define LV_USE_SPAN 1
    #endif
#endif

#ifndef LV_SPAN_SNIPPET_STACK_SIZE
    #ifdef CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
        #define LV_SPAN_SNIPPET_STACK_SIZE CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
    #else
        #define LV_SPAN_SNIPPET_STACK_SIZE 64
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
        #define LV_USE_SPINBOX 1
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
        #define LV_USE_SPINNER 1
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
        #define LV_USE_SWITCH 1
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
        #define LV_USE_TEXTAREA 1
    #endif
#endif

#ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
    #else
        #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500
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
        #define LV_USE_TABLE 1
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
        #define LV_USE_TABVIEW 1
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
        #define LV_USE_TILEVIEW 1
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
        #define LV_USE_WIN 1
    #endif
#endif



/*============================================================================
 * DRIVERS
 *============================================================================*/

#ifndef LV_USE_LINUX_DRM
    #ifdef CONFIG_LV_USE_LINUX_DRM
        #define LV_USE_LINUX_DRM CONFIG_LV_USE_LINUX_DRM
    #else
        #define LV_USE_LINUX_DRM 0
    #endif
#endif

#ifndef LV_LINUX_DRM_AUTO_BACKEND
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LINUX_DRM_AUTO_BACKEND
            #define LV_LINUX_DRM_AUTO_BACKEND CONFIG_LV_LINUX_DRM_AUTO_BACKEND
        #else
            #define LV_LINUX_DRM_AUTO_BACKEND 0
        #endif
    #else
          #define LV_LINUX_DRM_AUTO_BACKEND LV_USE_LINUX_DRM
    #endif
#endif

#ifndef LV_LINUX_DRM_BACKEND
    #ifdef CONFIG_LV_LINUX_DRM_BACKEND
        #define LV_LINUX_DRM_BACKEND CONFIG_LV_LINUX_DRM_BACKEND
    #else
        #define LV_LINUX_DRM_BACKEND LV_LINUX_DRM_BACKEND_FBDEV
    #endif
#endif

#ifndef LV_USE_LINUX_FBDEV
    #ifdef CONFIG_LV_USE_LINUX_FBDEV
        #define LV_USE_LINUX_FBDEV CONFIG_LV_USE_LINUX_FBDEV
    #else
        #define LV_USE_LINUX_FBDEV 0
    #endif
#endif

#ifndef LV_LINUX_FBDEV_BSD
    #ifdef CONFIG_LV_LINUX_FBDEV_BSD
        #define LV_LINUX_FBDEV_BSD CONFIG_LV_LINUX_FBDEV_BSD
    #else
        #define LV_LINUX_FBDEV_BSD 0
    #endif
#endif

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

#ifndef LV_LINUX_FBDEV_BUFFER_SIZE
    #ifdef CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
        #define LV_LINUX_FBDEV_BUFFER_SIZE CONFIG_LV_LINUX_FBDEV_BUFFER_SIZE
    #else
        #define LV_LINUX_FBDEV_BUFFER_SIZE 60
    #endif
#endif

#ifndef LV_LINUX_FBDEV_MMAP
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LINUX_FBDEV_MMAP
            #define LV_LINUX_FBDEV_MMAP CONFIG_LV_LINUX_FBDEV_MMAP
        #else
            #define LV_LINUX_FBDEV_MMAP 0
        #endif
    #else
          #define LV_LINUX_FBDEV_MMAP LV_USE_LINUX_FBDEV
    #endif
#endif

#ifndef LV_USE_FT81X
    #ifdef CONFIG_LV_USE_FT81X
        #define LV_USE_FT81X CONFIG_LV_USE_FT81X
    #else
        #define LV_USE_FT81X 0
    #endif
#endif

#ifndef LV_USE_LOVYAN_GFX
    #ifdef CONFIG_LV_USE_LOVYAN_GFX
        #define LV_USE_LOVYAN_GFX CONFIG_LV_USE_LOVYAN_GFX
    #else
        #define LV_USE_LOVYAN_GFX 0
    #endif
#endif

#ifndef LV_LGFX_USER_INCLUDE
    #ifdef CONFIG_LV_LGFX_USER_INCLUDE
        #define LV_LGFX_USER_INCLUDE CONFIG_LV_LGFX_USER_INCLUDE
    #else
        #define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"
    #endif
#endif

#ifndef LV_USE_GENERIC_MIPI
    #ifdef CONFIG_LV_USE_GENERIC_MIPI
        #define LV_USE_GENERIC_MIPI CONFIG_LV_USE_GENERIC_MIPI
    #else
        #define LV_USE_GENERIC_MIPI 0
    #endif
#endif

#ifndef LV_USE_ST7735
    #ifdef CONFIG_LV_USE_ST7735
        #define LV_USE_ST7735 CONFIG_LV_USE_ST7735
    #else
        #define LV_USE_ST7735 0
    #endif
#endif

#ifndef LV_USE_ST7789
    #ifdef CONFIG_LV_USE_ST7789
        #define LV_USE_ST7789 CONFIG_LV_USE_ST7789
    #else
        #define LV_USE_ST7789 0
    #endif
#endif

#ifndef LV_USE_ST7796
    #ifdef CONFIG_LV_USE_ST7796
        #define LV_USE_ST7796 CONFIG_LV_USE_ST7796
    #else
        #define LV_USE_ST7796 0
    #endif
#endif

#ifndef LV_USE_ILI9341
    #ifdef CONFIG_LV_USE_ILI9341
        #define LV_USE_ILI9341 CONFIG_LV_USE_ILI9341
    #else
        #define LV_USE_ILI9341 0
    #endif
#endif

#ifndef LV_USE_NV3007
    #ifdef CONFIG_LV_USE_NV3007
        #define LV_USE_NV3007 CONFIG_LV_USE_NV3007
    #else
        #define LV_USE_NV3007 0
    #endif
#endif

#ifndef LV_USE_NXP_ELCDIF
    #ifdef CONFIG_LV_USE_NXP_ELCDIF
        #define LV_USE_NXP_ELCDIF CONFIG_LV_USE_NXP_ELCDIF
    #else
        #define LV_USE_NXP_ELCDIF 0
    #endif
#endif

#ifndef LV_USE_RENESAS_GLCDC
    #ifdef CONFIG_LV_USE_RENESAS_GLCDC
        #define LV_USE_RENESAS_GLCDC CONFIG_LV_USE_RENESAS_GLCDC
    #else
        #define LV_USE_RENESAS_GLCDC 0
    #endif
#endif

#ifndef LV_USE_ST_LTDC
    #ifdef CONFIG_LV_USE_ST_LTDC
        #define LV_USE_ST_LTDC CONFIG_LV_USE_ST_LTDC
    #else
        #define LV_USE_ST_LTDC 0
    #endif
#endif

#ifndef LV_ST_LTDC_USE_DMA2D_FLUSH
    #ifdef CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
        #define LV_ST_LTDC_USE_DMA2D_FLUSH CONFIG_LV_ST_LTDC_USE_DMA2D_FLUSH
    #else
        #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
    #endif
#endif

#ifndef LV_USE_TFT_ESPI
    #ifdef CONFIG_LV_USE_TFT_ESPI
        #define LV_USE_TFT_ESPI CONFIG_LV_USE_TFT_ESPI
    #else
        #define LV_USE_TFT_ESPI 0
    #endif
#endif

#ifndef LV_USE_EVDEV
    #ifdef CONFIG_LV_USE_EVDEV
        #define LV_USE_EVDEV CONFIG_LV_USE_EVDEV
    #else
        #define LV_USE_EVDEV 0
    #endif
#endif

#ifndef LV_USE_LIBINPUT
    #ifdef CONFIG_LV_USE_LIBINPUT
        #define LV_USE_LIBINPUT CONFIG_LV_USE_LIBINPUT
    #else
        #define LV_USE_LIBINPUT 0
    #endif
#endif

#ifndef LV_LIBINPUT_BSD
    #ifdef CONFIG_LV_LIBINPUT_BSD
        #define LV_LIBINPUT_BSD CONFIG_LV_LIBINPUT_BSD
    #else
        #define LV_LIBINPUT_BSD 0
    #endif
#endif

#ifndef LV_LIBINPUT_XKB
    #ifdef CONFIG_LV_LIBINPUT_XKB
        #define LV_LIBINPUT_XKB CONFIG_LV_LIBINPUT_XKB
    #else
        #define LV_LIBINPUT_XKB 0
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_RULES
    #ifdef CONFIG_LV_LIBINPUT_XKB_RULES
        #define LV_LIBINPUT_XKB_RULES CONFIG_LV_LIBINPUT_XKB_RULES
    #else
        #define LV_LIBINPUT_XKB_RULES ""
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_MODEL
    #ifdef CONFIG_LV_LIBINPUT_XKB_MODEL
        #define LV_LIBINPUT_XKB_MODEL CONFIG_LV_LIBINPUT_XKB_MODEL
    #else
        #define LV_LIBINPUT_XKB_MODEL "pc101"
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_LAYOUT
    #ifdef CONFIG_LV_LIBINPUT_XKB_LAYOUT
        #define LV_LIBINPUT_XKB_LAYOUT CONFIG_LV_LIBINPUT_XKB_LAYOUT
    #else
        #define LV_LIBINPUT_XKB_LAYOUT "us"
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_VARIANT
    #ifdef CONFIG_LV_LIBINPUT_XKB_VARIANT
        #define LV_LIBINPUT_XKB_VARIANT CONFIG_LV_LIBINPUT_XKB_VARIANT
    #else
        #define LV_LIBINPUT_XKB_VARIANT ""
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
            #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT CONFIG_LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
        #else
            #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT 0
        #endif
    #else
          #define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT LV_LIBINPUT_XKB && LV_USE_LIBINPUT
    #endif
#endif

#ifndef LV_LIBINPUT_XKB_OPTIONS
    #ifdef CONFIG_LV_LIBINPUT_XKB_OPTIONS
        #define LV_LIBINPUT_XKB_OPTIONS CONFIG_LV_LIBINPUT_XKB_OPTIONS
    #else
        #define LV_LIBINPUT_XKB_OPTIONS ""
    #endif
#endif

#ifndef LV_USE_NUTTX
    #ifdef CONFIG_LV_USE_NUTTX
        #define LV_USE_NUTTX CONFIG_LV_USE_NUTTX
    #else
        #define LV_USE_NUTTX 0
    #endif
#endif

#ifndef LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
    #ifdef CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
        #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP CONFIG_LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
    #else
        #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0
    #endif
#endif

#ifndef LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    #ifdef CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
        #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP CONFIG_LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP
    #else
        #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP 0
    #endif
#endif

#ifndef LV_USE_NUTTX_LIBUV
    #ifdef CONFIG_LV_USE_NUTTX_LIBUV
        #define LV_USE_NUTTX_LIBUV CONFIG_LV_USE_NUTTX_LIBUV
    #else
        #define LV_USE_NUTTX_LIBUV 0
    #endif
#endif

#ifndef LV_USE_NUTTX_CUSTOM_INIT
    #ifdef CONFIG_LV_USE_NUTTX_CUSTOM_INIT
        #define LV_USE_NUTTX_CUSTOM_INIT CONFIG_LV_USE_NUTTX_CUSTOM_INIT
    #else
        #define LV_USE_NUTTX_CUSTOM_INIT 0
    #endif
#endif

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

#ifndef LV_NUTTX_LCD_BUFFER_SIZE
    #ifdef CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
        #define LV_NUTTX_LCD_BUFFER_SIZE CONFIG_LV_NUTTX_LCD_BUFFER_SIZE
    #else
        #define LV_NUTTX_LCD_BUFFER_SIZE 60
    #endif
#endif

#ifndef LV_USE_NUTTX_TOUCHSCREEN
    #ifdef CONFIG_LV_USE_NUTTX_TOUCHSCREEN
        #define LV_USE_NUTTX_TOUCHSCREEN CONFIG_LV_USE_NUTTX_TOUCHSCREEN
    #else
        #define LV_USE_NUTTX_TOUCHSCREEN 0
    #endif
#endif

#ifndef LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
    #ifdef CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
        #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE CONFIG_LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE
    #else
        #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE 0
    #endif
#endif

#ifndef LV_USE_NUTTX_MOUSE
    #ifdef CONFIG_LV_USE_NUTTX_MOUSE
        #define LV_USE_NUTTX_MOUSE CONFIG_LV_USE_NUTTX_MOUSE
    #else
        #define LV_USE_NUTTX_MOUSE 0
    #endif
#endif

#ifndef LV_USE_NUTTX_MOUSE_MOVE_STEP
    #ifdef CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
        #define LV_USE_NUTTX_MOUSE_MOVE_STEP CONFIG_LV_USE_NUTTX_MOUSE_MOVE_STEP
    #else
        #define LV_USE_NUTTX_MOUSE_MOVE_STEP 1
    #endif
#endif

#ifndef LV_USE_NUTTX_TRACE_FILE
    #ifdef CONFIG_LV_USE_NUTTX_TRACE_FILE
        #define LV_USE_NUTTX_TRACE_FILE CONFIG_LV_USE_NUTTX_TRACE_FILE
    #else
        #define LV_USE_NUTTX_TRACE_FILE 0
    #endif
#endif

#ifndef LV_NUTTX_TRACE_FILE_PATH
    #ifdef CONFIG_LV_NUTTX_TRACE_FILE_PATH
        #define LV_NUTTX_TRACE_FILE_PATH CONFIG_LV_NUTTX_TRACE_FILE_PATH
    #else
        #define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"
    #endif
#endif

#ifndef LV_USE_OPENGLES
    #ifdef CONFIG_LV_USE_OPENGLES
        #define LV_USE_OPENGLES CONFIG_LV_USE_OPENGLES
    #else
        #define LV_USE_OPENGLES 0
    #endif
#endif

#ifndef LV_USE_OPENGLES_DEBUG
    #ifdef CONFIG_LV_USE_OPENGLES_DEBUG
        #define LV_USE_OPENGLES_DEBUG CONFIG_LV_USE_OPENGLES_DEBUG
    #else
        #define LV_USE_OPENGLES_DEBUG 0
    #endif
#endif

#ifndef LV_USE_GLFW
    #ifdef CONFIG_LV_USE_GLFW
        #define LV_USE_GLFW CONFIG_LV_USE_GLFW
    #else
        #define LV_USE_GLFW 0
    #endif
#endif

#ifndef LV_USE_QNX
    #ifdef CONFIG_LV_USE_QNX
        #define LV_USE_QNX CONFIG_LV_USE_QNX
    #else
        #define LV_USE_QNX 0
    #endif
#endif

#ifndef LV_QNX_BUF_COUNT
    #ifdef CONFIG_LV_QNX_BUF_COUNT
        #define LV_QNX_BUF_COUNT CONFIG_LV_QNX_BUF_COUNT
    #else
        #define LV_QNX_BUF_COUNT 1
    #endif
#endif

#ifndef LV_USE_SDL
    #ifdef CONFIG_LV_USE_SDL
        #define LV_USE_SDL CONFIG_LV_USE_SDL
    #else
        #define LV_USE_SDL 0
    #endif
#endif

#ifndef LV_SDL_INCLUDE_PATH
    #ifdef CONFIG_LV_SDL_INCLUDE_PATH
        #define LV_SDL_INCLUDE_PATH CONFIG_LV_SDL_INCLUDE_PATH
    #else
        #define LV_SDL_INCLUDE_PATH "SDL2/SDL.h"
    #endif
#endif

#ifndef LV_SDL_AUTO_BACKEND
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_SDL_AUTO_BACKEND
            #define LV_SDL_AUTO_BACKEND CONFIG_LV_SDL_AUTO_BACKEND
        #else
            #define LV_SDL_AUTO_BACKEND 0
        #endif
    #else
          #define LV_SDL_AUTO_BACKEND LV_USE_SDL
    #endif
#endif

#ifndef LV_SDL_BACKEND
    #ifdef CONFIG_LV_SDL_BACKEND
        #define LV_SDL_BACKEND CONFIG_LV_SDL_BACKEND
    #else
        #define LV_SDL_BACKEND LV_SDL_BACKEND_SW
    #endif
#endif

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
        #define LV_SDL_BUF_COUNT 1
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
          #define LV_SDL_ACCELERATED LV_USE_SDL
    #endif
#endif

#ifndef LV_SDL_FULLSCREEN
    #ifdef CONFIG_LV_SDL_FULLSCREEN
        #define LV_SDL_FULLSCREEN CONFIG_LV_SDL_FULLSCREEN
    #else
        #define LV_SDL_FULLSCREEN 0
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
          #define LV_SDL_DIRECT_EXIT LV_USE_SDL
    #endif
#endif

#ifndef LV_SDL_MOUSEWHEEL_MODE
    #ifdef CONFIG_LV_SDL_MOUSEWHEEL_MODE
        #define LV_SDL_MOUSEWHEEL_MODE CONFIG_LV_SDL_MOUSEWHEEL_MODE
    #else
        #define LV_SDL_MOUSEWHEEL_MODE LV_SDL_MOUSEWHEEL_MODE_ENCODER
    #endif
#endif

#ifndef LV_USE_UEFI
    #ifdef CONFIG_LV_USE_UEFI
        #define LV_USE_UEFI CONFIG_LV_USE_UEFI
    #else
        #define LV_USE_UEFI 0
    #endif
#endif

#ifndef LV_USE_UEFI_INCLUDE
    #ifdef CONFIG_LV_USE_UEFI_INCLUDE
        #define LV_USE_UEFI_INCLUDE CONFIG_LV_USE_UEFI_INCLUDE
    #else
        #define LV_USE_UEFI_INCLUDE "myefi.h"
    #endif
#endif

#ifndef LV_UEFI_USE_MEMORY_SERVICES
    #ifdef CONFIG_LV_UEFI_USE_MEMORY_SERVICES
        #define LV_UEFI_USE_MEMORY_SERVICES CONFIG_LV_UEFI_USE_MEMORY_SERVICES
    #else
        #define LV_UEFI_USE_MEMORY_SERVICES 0
    #endif
#endif

#ifndef LV_USE_WAYLAND
    #ifdef CONFIG_LV_USE_WAYLAND
        #define LV_USE_WAYLAND CONFIG_LV_USE_WAYLAND
    #else
        #define LV_USE_WAYLAND 0
    #endif
#endif

#ifndef LV_WAYLAND_DIRECT_EXIT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WAYLAND_DIRECT_EXIT
            #define LV_WAYLAND_DIRECT_EXIT CONFIG_LV_WAYLAND_DIRECT_EXIT
        #else
            #define LV_WAYLAND_DIRECT_EXIT 0
        #endif
    #else
          #define LV_WAYLAND_DIRECT_EXIT LV_USE_WAYLAND
    #endif
#endif

#ifndef LV_WAYLAND_AUTO_BACKEND
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_WAYLAND_AUTO_BACKEND
            #define LV_WAYLAND_AUTO_BACKEND CONFIG_LV_WAYLAND_AUTO_BACKEND
        #else
            #define LV_WAYLAND_AUTO_BACKEND 0
        #endif
    #else
          #define LV_WAYLAND_AUTO_BACKEND LV_USE_WAYLAND
    #endif
#endif

#ifndef LV_WAYLAND_BACKEND
    #ifdef CONFIG_LV_WAYLAND_BACKEND
        #define LV_WAYLAND_BACKEND CONFIG_LV_WAYLAND_BACKEND
    #else
        #define LV_WAYLAND_BACKEND LV_WAYLAND_BACKEND_SHM
    #endif
#endif

#ifndef LV_USE_WINDOWS
    #ifdef CONFIG_LV_USE_WINDOWS
        #define LV_USE_WINDOWS CONFIG_LV_USE_WINDOWS
    #else
        #define LV_USE_WINDOWS 0
    #endif
#endif

#ifndef LV_USE_X11
    #ifdef CONFIG_LV_USE_X11
        #define LV_USE_X11 CONFIG_LV_USE_X11
    #else
        #define LV_USE_X11 0
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
          #define LV_X11_DOUBLE_BUFFER LV_USE_X11
    #endif
#endif

#ifndef LV_X11_DIRECT_EXIT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_X11_DIRECT_EXIT
            #define LV_X11_DIRECT_EXIT CONFIG_LV_X11_DIRECT_EXIT
        #else
            #define LV_X11_DIRECT_EXIT 0
        #endif
    #else
          #define LV_X11_DIRECT_EXIT LV_USE_X11
    #endif
#endif

#ifndef LV_X11_RENDER_MODE
    #ifdef CONFIG_LV_X11_RENDER_MODE
        #define LV_X11_RENDER_MODE CONFIG_LV_X11_RENDER_MODE
    #else
        #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
    #endif
#endif

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

#ifndef LV_FS_DEFAULT_DRIVER_LETTER
    #ifdef CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
        #define LV_FS_DEFAULT_DRIVER_LETTER CONFIG_LV_FS_DEFAULT_DRIVER_LETTER
    #else
        #define LV_FS_DEFAULT_DRIVER_LETTER 0
    #endif
#endif

#ifndef LV_USE_FS_STDIO
    #ifdef CONFIG_LV_USE_FS_STDIO
        #define LV_USE_FS_STDIO CONFIG_LV_USE_FS_STDIO
    #else
        #define LV_USE_FS_STDIO 0
    #endif
#endif

#ifndef LV_FS_STDIO_LETTER
    #ifdef CONFIG_LV_FS_STDIO_LETTER
        #define LV_FS_STDIO_LETTER CONFIG_LV_FS_STDIO_LETTER
    #else
        #define LV_FS_STDIO_LETTER 0
    #endif
#endif

#ifndef LV_FS_STDIO_PATH
    #ifdef CONFIG_LV_FS_STDIO_PATH
        #define LV_FS_STDIO_PATH CONFIG_LV_FS_STDIO_PATH
    #else
        #define LV_FS_STDIO_PATH ""
    #endif
#endif

#ifndef LV_FS_STDIO_CACHE_SIZE
    #ifdef CONFIG_LV_FS_STDIO_CACHE_SIZE
        #define LV_FS_STDIO_CACHE_SIZE CONFIG_LV_FS_STDIO_CACHE_SIZE
    #else
        #define LV_FS_STDIO_CACHE_SIZE 0
    #endif
#endif

#ifndef LV_USE_FS_POSIX
    #ifdef CONFIG_LV_USE_FS_POSIX
        #define LV_USE_FS_POSIX CONFIG_LV_USE_FS_POSIX
    #else
        #define LV_USE_FS_POSIX 0
    #endif
#endif

#ifndef LV_FS_POSIX_LETTER
    #ifdef CONFIG_LV_FS_POSIX_LETTER
        #define LV_FS_POSIX_LETTER CONFIG_LV_FS_POSIX_LETTER
    #else
        #define LV_FS_POSIX_LETTER 0
    #endif
#endif

#ifndef LV_FS_POSIX_PATH
    #ifdef CONFIG_LV_FS_POSIX_PATH
        #define LV_FS_POSIX_PATH CONFIG_LV_FS_POSIX_PATH
    #else
        #define LV_FS_POSIX_PATH ""
    #endif
#endif

#ifndef LV_FS_POSIX_CACHE_SIZE
    #ifdef CONFIG_LV_FS_POSIX_CACHE_SIZE
        #define LV_FS_POSIX_CACHE_SIZE CONFIG_LV_FS_POSIX_CACHE_SIZE
    #else
        #define LV_FS_POSIX_CACHE_SIZE 0
    #endif
#endif

#ifndef LV_USE_FS_WIN32
    #ifdef CONFIG_LV_USE_FS_WIN32
        #define LV_USE_FS_WIN32 CONFIG_LV_USE_FS_WIN32
    #else
        #define LV_USE_FS_WIN32 0
    #endif
#endif

#ifndef LV_FS_WIN32_LETTER
    #ifdef CONFIG_LV_FS_WIN32_LETTER
        #define LV_FS_WIN32_LETTER CONFIG_LV_FS_WIN32_LETTER
    #else
        #define LV_FS_WIN32_LETTER 0
    #endif
#endif

#ifndef LV_FS_WIN32_PATH
    #ifdef CONFIG_LV_FS_WIN32_PATH
        #define LV_FS_WIN32_PATH CONFIG_LV_FS_WIN32_PATH
    #else
        #define LV_FS_WIN32_PATH ""
    #endif
#endif

#ifndef LV_FS_WIN32_CACHE_SIZE
    #ifdef CONFIG_LV_FS_WIN32_CACHE_SIZE
        #define LV_FS_WIN32_CACHE_SIZE CONFIG_LV_FS_WIN32_CACHE_SIZE
    #else
        #define LV_FS_WIN32_CACHE_SIZE 0
    #endif
#endif

#ifndef LV_USE_FS_FATFS
    #ifdef CONFIG_LV_USE_FS_FATFS
        #define LV_USE_FS_FATFS CONFIG_LV_USE_FS_FATFS
    #else
        #define LV_USE_FS_FATFS 0
    #endif
#endif

#ifndef LV_FS_FATFS_LETTER
    #ifdef CONFIG_LV_FS_FATFS_LETTER
        #define LV_FS_FATFS_LETTER CONFIG_LV_FS_FATFS_LETTER
    #else
        #define LV_FS_FATFS_LETTER 0
    #endif
#endif

#ifndef LV_FS_FATFS_PATH
    #ifdef CONFIG_LV_FS_FATFS_PATH
        #define LV_FS_FATFS_PATH CONFIG_LV_FS_FATFS_PATH
    #else
        #define LV_FS_FATFS_PATH ""
    #endif
#endif

#ifndef LV_FS_FATFS_CACHE_SIZE
    #ifdef CONFIG_LV_FS_FATFS_CACHE_SIZE
        #define LV_FS_FATFS_CACHE_SIZE CONFIG_LV_FS_FATFS_CACHE_SIZE
    #else
        #define LV_FS_FATFS_CACHE_SIZE 0
    #endif
#endif

#ifndef LV_USE_FS_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_LITTLEFS
        #define LV_USE_FS_LITTLEFS CONFIG_LV_USE_FS_LITTLEFS
    #else
        #define LV_USE_FS_LITTLEFS 0
    #endif
#endif

#ifndef LV_FS_LITTLEFS_LETTER
    #ifdef CONFIG_LV_FS_LITTLEFS_LETTER
        #define LV_FS_LITTLEFS_LETTER CONFIG_LV_FS_LITTLEFS_LETTER
    #else
        #define LV_FS_LITTLEFS_LETTER 0
    #endif
#endif

#ifndef LV_FS_LITTLEFS_PATH
    #ifdef CONFIG_LV_FS_LITTLEFS_PATH
        #define LV_FS_LITTLEFS_PATH CONFIG_LV_FS_LITTLEFS_PATH
    #else
        #define LV_FS_LITTLEFS_PATH ""
    #endif
#endif

#ifndef LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #ifdef CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS CONFIG_LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #else
        #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0
    #endif
#endif

#ifndef LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
    #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
        #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_LETTER
    #else
        #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER 0
    #endif
#endif

#ifndef LV_FS_ARDUINO_ESP_LITTLEFS_PATH
    #ifdef CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
        #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH CONFIG_LV_FS_ARDUINO_ESP_LITTLEFS_PATH
    #else
        #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""
    #endif
#endif

#ifndef LV_USE_FS_ARDUINO_SD
    #ifdef CONFIG_LV_USE_FS_ARDUINO_SD
        #define LV_USE_FS_ARDUINO_SD CONFIG_LV_USE_FS_ARDUINO_SD
    #else
        #define LV_USE_FS_ARDUINO_SD 0
    #endif
#endif

#ifndef LV_FS_ARDUINO_SD_LETTER
    #ifdef CONFIG_LV_FS_ARDUINO_SD_LETTER
        #define LV_FS_ARDUINO_SD_LETTER CONFIG_LV_FS_ARDUINO_SD_LETTER
    #else
        #define LV_FS_ARDUINO_SD_LETTER 0
    #endif
#endif

#ifndef LV_FS_ARDUINO_SD_PATH
    #ifdef CONFIG_LV_FS_ARDUINO_SD_PATH
        #define LV_FS_ARDUINO_SD_PATH CONFIG_LV_FS_ARDUINO_SD_PATH
    #else
        #define LV_FS_ARDUINO_SD_PATH ""
    #endif
#endif

#ifndef LV_USE_FS_UEFI
    #ifdef CONFIG_LV_USE_FS_UEFI
        #define LV_USE_FS_UEFI CONFIG_LV_USE_FS_UEFI
    #else
        #define LV_USE_FS_UEFI 0
    #endif
#endif

#ifndef LV_FS_UEFI_LETTER
    #ifdef CONFIG_LV_FS_UEFI_LETTER
        #define LV_FS_UEFI_LETTER CONFIG_LV_FS_UEFI_LETTER
    #else
        #define LV_FS_UEFI_LETTER 0
    #endif
#endif

#ifndef LV_USE_FS_FROGFS
    #ifdef CONFIG_LV_USE_FS_FROGFS
        #define LV_USE_FS_FROGFS CONFIG_LV_USE_FS_FROGFS
    #else
        #define LV_USE_FS_FROGFS 0
    #endif
#endif

#ifndef LV_FS_FROGFS_LETTER
    #ifdef CONFIG_LV_FS_FROGFS_LETTER
        #define LV_FS_FROGFS_LETTER CONFIG_LV_FS_FROGFS_LETTER
    #else
        #define LV_FS_FROGFS_LETTER 0
    #endif
#endif

#ifndef LV_USE_FS_MEMFS
    #ifdef CONFIG_LV_USE_FS_MEMFS
        #define LV_USE_FS_MEMFS CONFIG_LV_USE_FS_MEMFS
    #else
        #define LV_USE_FS_MEMFS 0
    #endif
#endif

#ifndef LV_FS_MEMFS_LETTER
    #ifdef CONFIG_LV_FS_MEMFS_LETTER
        #define LV_FS_MEMFS_LETTER CONFIG_LV_FS_MEMFS_LETTER
    #else
        #define LV_FS_MEMFS_LETTER 0
    #endif
#endif



/*============================================================================
 * DEBUGGING
 *============================================================================*/

#ifndef LV_USE_SYSMON
    #ifdef CONFIG_LV_USE_SYSMON
        #define LV_USE_SYSMON CONFIG_LV_USE_SYSMON
    #else
        #define LV_USE_SYSMON 0
    #endif
#endif

#ifndef LV_SYSMON_PROC_IDLE_AVAILABLE
    #ifdef CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
        #define LV_SYSMON_PROC_IDLE_AVAILABLE CONFIG_LV_SYSMON_PROC_IDLE_AVAILABLE
    #else
        #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
    #endif
#endif

#ifndef LV_USE_PERF_MONITOR
    #ifdef CONFIG_LV_USE_PERF_MONITOR
        #define LV_USE_PERF_MONITOR CONFIG_LV_USE_PERF_MONITOR
    #else
        #define LV_USE_PERF_MONITOR 0
    #endif
#endif

#ifndef LV_USE_PERF_MONITOR_POS
    #ifdef CONFIG_LV_USE_PERF_MONITOR_POS
        #define LV_USE_PERF_MONITOR_POS CONFIG_LV_USE_PERF_MONITOR_POS
    #else
        #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
    #endif
#endif

#ifndef LV_USE_PERF_MONITOR_LOG_MODE
    #ifdef CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
        #define LV_USE_PERF_MONITOR_LOG_MODE CONFIG_LV_USE_PERF_MONITOR_LOG_MODE
    #else
        #define LV_USE_PERF_MONITOR_LOG_MODE 0
    #endif
#endif

#ifndef LV_USE_MEM_MONITOR
    #ifdef CONFIG_LV_USE_MEM_MONITOR
        #define LV_USE_MEM_MONITOR CONFIG_LV_USE_MEM_MONITOR
    #else
        #define LV_USE_MEM_MONITOR 0
    #endif
#endif

#ifndef LV_USE_MEM_MONITOR_POS
    #ifdef CONFIG_LV_USE_MEM_MONITOR_POS
        #define LV_USE_MEM_MONITOR_POS CONFIG_LV_USE_MEM_MONITOR_POS
    #else
        #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
    #endif
#endif

#ifndef LV_SYSMON_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_SYSMON_USE_CUSTOM_INCLUDE
        #define LV_SYSMON_USE_CUSTOM_INCLUDE CONFIG_LV_SYSMON_USE_CUSTOM_INCLUDE
    #else
        #define LV_SYSMON_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_SYSMON_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_SYSMON_CUSTOM_INCLUDE
        #define LV_SYSMON_CUSTOM_INCLUDE CONFIG_LV_SYSMON_CUSTOM_INCLUDE
    #else
        #define LV_SYSMON_CUSTOM_INCLUDE ""
    #endif
#endif

#ifndef LV_USE_PROFILER
    #ifdef CONFIG_LV_USE_PROFILER
        #define LV_USE_PROFILER CONFIG_LV_USE_PROFILER
    #else
        #define LV_USE_PROFILER 0
    #endif
#endif

#ifndef LV_USE_PROFILER_BUILTIN
    #ifdef CONFIG_LV_USE_PROFILER_BUILTIN
        #define LV_USE_PROFILER_BUILTIN CONFIG_LV_USE_PROFILER_BUILTIN
    #else
        #define LV_USE_PROFILER_BUILTIN 0
    #endif
#endif

#ifndef LV_PROFILER_BUILTIN_BUF_SIZE
    #ifdef CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
        #define LV_PROFILER_BUILTIN_BUF_SIZE CONFIG_LV_PROFILER_BUILTIN_BUF_SIZE
    #else
        #define LV_PROFILER_BUILTIN_BUF_SIZE 16384
    #endif
#endif

#ifndef LV_PROFILER_BUILTIN_DEFAULT_ENABLE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
            #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE CONFIG_LV_PROFILER_BUILTIN_DEFAULT_ENABLE
        #else
            #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 0
        #endif
    #else
          #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE LV_USE_PROFILER_BUILTIN
    #endif
#endif

#ifndef LV_USE_PROFILER_BUILTIN_POSIX
    #ifdef CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
        #define LV_USE_PROFILER_BUILTIN_POSIX CONFIG_LV_USE_PROFILER_BUILTIN_POSIX
    #else
        #define LV_USE_PROFILER_BUILTIN_POSIX 0
    #endif
#endif

#ifndef LV_PROFILER_INCLUDE
    #ifdef CONFIG_LV_PROFILER_INCLUDE
        #define LV_PROFILER_INCLUDE CONFIG_LV_PROFILER_INCLUDE
    #else
        #define LV_PROFILER_INCLUDE "lvgl/debugging/profiler/lv_profiler_builtin.h"
    #endif
#endif

#ifndef LV_PROFILER_LAYOUT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_LAYOUT
            #define LV_PROFILER_LAYOUT CONFIG_LV_PROFILER_LAYOUT
        #else
            #define LV_PROFILER_LAYOUT 0
        #endif
    #else
          #define LV_PROFILER_LAYOUT LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_REFR
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_REFR
            #define LV_PROFILER_REFR CONFIG_LV_PROFILER_REFR
        #else
            #define LV_PROFILER_REFR 0
        #endif
    #else
          #define LV_PROFILER_REFR LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_DRAW
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_DRAW
            #define LV_PROFILER_DRAW CONFIG_LV_PROFILER_DRAW
        #else
            #define LV_PROFILER_DRAW 0
        #endif
    #else
          #define LV_PROFILER_DRAW LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_INDEV
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_INDEV
            #define LV_PROFILER_INDEV CONFIG_LV_PROFILER_INDEV
        #else
            #define LV_PROFILER_INDEV 0
        #endif
    #else
          #define LV_PROFILER_INDEV LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_DECODER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_DECODER
            #define LV_PROFILER_DECODER CONFIG_LV_PROFILER_DECODER
        #else
            #define LV_PROFILER_DECODER 0
        #endif
    #else
          #define LV_PROFILER_DECODER LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_FONT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_FONT
            #define LV_PROFILER_FONT CONFIG_LV_PROFILER_FONT
        #else
            #define LV_PROFILER_FONT 0
        #endif
    #else
          #define LV_PROFILER_FONT LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_FS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_FS
            #define LV_PROFILER_FS CONFIG_LV_PROFILER_FS
        #else
            #define LV_PROFILER_FS 0
        #endif
    #else
          #define LV_PROFILER_FS LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_STYLE
    #ifdef CONFIG_LV_PROFILER_STYLE
        #define LV_PROFILER_STYLE CONFIG_LV_PROFILER_STYLE
    #else
        #define LV_PROFILER_STYLE 0
    #endif
#endif

#ifndef LV_PROFILER_TIMER
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_TIMER
            #define LV_PROFILER_TIMER CONFIG_LV_PROFILER_TIMER
        #else
            #define LV_PROFILER_TIMER 0
        #endif
    #else
          #define LV_PROFILER_TIMER LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_CACHE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_CACHE
            #define LV_PROFILER_CACHE CONFIG_LV_PROFILER_CACHE
        #else
            #define LV_PROFILER_CACHE 0
        #endif
    #else
          #define LV_PROFILER_CACHE LV_USE_PROFILER
    #endif
#endif

#ifndef LV_PROFILER_EVENT
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_PROFILER_EVENT
            #define LV_PROFILER_EVENT CONFIG_LV_PROFILER_EVENT
        #else
            #define LV_PROFILER_EVENT 0
        #endif
    #else
          #define LV_PROFILER_EVENT LV_USE_PROFILER
    #endif
#endif

#ifndef LV_USE_TEST
    #ifdef CONFIG_LV_USE_TEST
        #define LV_USE_TEST CONFIG_LV_USE_TEST
    #else
        #define LV_USE_TEST 0
    #endif
#endif

#ifndef LV_USE_TEST_SCREENSHOT_COMPARE
    #ifdef CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
        #define LV_USE_TEST_SCREENSHOT_COMPARE CONFIG_LV_USE_TEST_SCREENSHOT_COMPARE
    #else
        #define LV_USE_TEST_SCREENSHOT_COMPARE 0
    #endif
#endif

#ifndef LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
            #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE CONFIG_LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE
        #else
            #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 0
        #endif
    #else
          #define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE LV_USE_TEST_SCREENSHOT_COMPARE && LV_USE_TEST
    #endif
#endif

#ifndef LV_USE_MONKEY
    #ifdef CONFIG_LV_USE_MONKEY
        #define LV_USE_MONKEY CONFIG_LV_USE_MONKEY
    #else
        #define LV_USE_MONKEY 0
    #endif
#endif

#ifndef LV_USE_REFR_DEBUG
    #ifdef CONFIG_LV_USE_REFR_DEBUG
        #define LV_USE_REFR_DEBUG CONFIG_LV_USE_REFR_DEBUG
    #else
        #define LV_USE_REFR_DEBUG 0
    #endif
#endif

#ifndef LV_USE_LAYER_DEBUG
    #ifdef CONFIG_LV_USE_LAYER_DEBUG
        #define LV_USE_LAYER_DEBUG CONFIG_LV_USE_LAYER_DEBUG
    #else
        #define LV_USE_LAYER_DEBUG 0
    #endif
#endif

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

#ifndef LV_USE_FRAGMENT
    #ifdef CONFIG_LV_USE_FRAGMENT
        #define LV_USE_FRAGMENT CONFIG_LV_USE_FRAGMENT
    #else
        #define LV_USE_FRAGMENT 0
    #endif
#endif

#ifndef LV_USE_FILE_EXPLORER
    #ifdef CONFIG_LV_USE_FILE_EXPLORER
        #define LV_USE_FILE_EXPLORER CONFIG_LV_USE_FILE_EXPLORER
    #else
        #define LV_USE_FILE_EXPLORER 0
    #endif
#endif

#ifndef LV_FILE_EXPLORER_PATH_MAX_LEN
    #ifdef CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
        #define LV_FILE_EXPLORER_PATH_MAX_LEN CONFIG_LV_FILE_EXPLORER_PATH_MAX_LEN
    #else
        #define LV_FILE_EXPLORER_PATH_MAX_LEN 128
    #endif
#endif

#ifndef LV_FILE_EXPLORER_QUICK_ACCESS
    #ifdef LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
            #define LV_FILE_EXPLORER_QUICK_ACCESS CONFIG_LV_FILE_EXPLORER_QUICK_ACCESS
        #else
            #define LV_FILE_EXPLORER_QUICK_ACCESS 0
        #endif
    #else
          #define LV_FILE_EXPLORER_QUICK_ACCESS LV_USE_FILE_EXPLORER
    #endif
#endif



/*============================================================================
 * BUILD
 *============================================================================*/

#ifndef LV_USE_PRIVATE_API
    #ifdef CONFIG_LV_USE_PRIVATE_API
        #define LV_USE_PRIVATE_API CONFIG_LV_USE_PRIVATE_API
    #else
        #define LV_USE_PRIVATE_API 0
    #endif
#endif

#ifndef LV_ENABLE_GLOBAL_CUSTOM
    #ifdef CONFIG_LV_ENABLE_GLOBAL_CUSTOM
        #define LV_ENABLE_GLOBAL_CUSTOM CONFIG_LV_ENABLE_GLOBAL_CUSTOM
    #else
        #define LV_ENABLE_GLOBAL_CUSTOM 0
    #endif
#endif

#ifndef LV_GLOBAL_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_GLOBAL_USE_CUSTOM_INCLUDE
        #define LV_GLOBAL_USE_CUSTOM_INCLUDE CONFIG_LV_GLOBAL_USE_CUSTOM_INCLUDE
    #else
        #define LV_GLOBAL_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_GLOBAL_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
        #define LV_GLOBAL_CUSTOM_INCLUDE CONFIG_LV_GLOBAL_CUSTOM_INCLUDE
    #else
        #define LV_GLOBAL_CUSTOM_INCLUDE "lv_global.h"
    #endif
#endif

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

#ifndef LV_USE_ASSERT_STYLE
    #ifdef CONFIG_LV_USE_ASSERT_STYLE
        #define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
    #else
        #define LV_USE_ASSERT_STYLE 0
    #endif
#endif

#ifndef LV_USE_ASSERT_MEM_INTEGRITY
    #ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
        #define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
    #else
        #define LV_USE_ASSERT_MEM_INTEGRITY 0
    #endif
#endif

#ifndef LV_USE_ASSERT_OBJ
    #ifdef CONFIG_LV_USE_ASSERT_OBJ
        #define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
    #else
        #define LV_USE_ASSERT_OBJ 0
    #endif
#endif

#ifndef LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING
    #ifdef CONFIG_LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING
        #define LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING CONFIG_LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING
    #else
        #define LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING 0
    #endif
#endif

#ifndef LV_ASSERT_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_ASSERT_USE_CUSTOM_INCLUDE
        #define LV_ASSERT_USE_CUSTOM_INCLUDE CONFIG_LV_ASSERT_USE_CUSTOM_INCLUDE
    #else
        #define LV_ASSERT_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_ASSERT_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_ASSERT_CUSTOM_INCLUDE
        #define LV_ASSERT_CUSTOM_INCLUDE CONFIG_LV_ASSERT_CUSTOM_INCLUDE
    #else
        #define LV_ASSERT_CUSTOM_INCLUDE ""
    #endif
#endif

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

#ifndef LV_CHECK_ARG_ASSERT_ON_FAIL
    #ifdef CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL
        #define LV_CHECK_ARG_ASSERT_ON_FAIL CONFIG_LV_CHECK_ARG_ASSERT_ON_FAIL
    #else
        #define LV_CHECK_ARG_ASSERT_ON_FAIL 0
    #endif
#endif

#ifndef LV_CHECK_ARG_LOG_MODE
    #ifdef CONFIG_LV_CHECK_ARG_LOG_MODE
        #define LV_CHECK_ARG_LOG_MODE CONFIG_LV_CHECK_ARG_LOG_MODE
    #else
        #define LV_CHECK_ARG_LOG_MODE LV_CHECK_ARG_LOG_MODE_NONE
    #endif
#endif

#ifndef LV_USE_CHECK_OBJ_CLASSTYPE
    #ifdef CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
        #define LV_USE_CHECK_OBJ_CLASSTYPE CONFIG_LV_USE_CHECK_OBJ_CLASSTYPE
    #else
        #define LV_USE_CHECK_OBJ_CLASSTYPE 0
    #endif
#endif

#ifndef LV_USE_CHECK_OBJ_VALIDITY
    #ifdef CONFIG_LV_USE_CHECK_OBJ_VALIDITY
        #define LV_USE_CHECK_OBJ_VALIDITY CONFIG_LV_USE_CHECK_OBJ_VALIDITY
    #else
        #define LV_USE_CHECK_OBJ_VALIDITY 0
    #endif
#endif



/*============================================================================
 * COMPILER SETTINGS
 *============================================================================*/

#ifndef LV_BIG_ENDIAN_SYSTEM
    #ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
        #define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
    #else
        #define LV_BIG_ENDIAN_SYSTEM 0
    #endif
#endif

#ifndef LV_ATTRIBUTE_USE_CUSTOM_INCLUDE
    #ifdef CONFIG_LV_ATTRIBUTE_USE_CUSTOM_INCLUDE
        #define LV_ATTRIBUTE_USE_CUSTOM_INCLUDE CONFIG_LV_ATTRIBUTE_USE_CUSTOM_INCLUDE
    #else
        #define LV_ATTRIBUTE_USE_CUSTOM_INCLUDE 0
    #endif
#endif

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

#ifndef LV_USE_DEMO_BENCHMARK
    #ifdef CONFIG_LV_USE_DEMO_BENCHMARK
        #define LV_USE_DEMO_BENCHMARK CONFIG_LV_USE_DEMO_BENCHMARK
    #else
        #define LV_USE_DEMO_BENCHMARK 0
    #endif
#endif

#ifndef LV_DEMO_BENCHMARK_ALIGNED_FONTS
    #ifdef CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS CONFIG_LV_DEMO_BENCHMARK_ALIGNED_FONTS
    #else
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0
    #endif
#endif

#ifndef LV_USE_DEMO_GLTF
    #ifdef CONFIG_LV_USE_DEMO_GLTF
        #define LV_USE_DEMO_GLTF CONFIG_LV_USE_DEMO_GLTF
    #else
        #define LV_USE_DEMO_GLTF 0
    #endif
#endif

#ifndef LV_USE_DEMO_KEYPAD_AND_ENCODER
    #ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
    #else
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #endif
#endif

#ifndef LV_USE_DEMO_MUSIC
    #ifdef CONFIG_LV_USE_DEMO_MUSIC
        #define LV_USE_DEMO_MUSIC CONFIG_LV_USE_DEMO_MUSIC
    #else
        #define LV_USE_DEMO_MUSIC 0
    #endif
#endif

#ifndef LV_DEMO_MUSIC_SQUARE
    #ifdef CONFIG_LV_DEMO_MUSIC_SQUARE
        #define LV_DEMO_MUSIC_SQUARE CONFIG_LV_DEMO_MUSIC_SQUARE
    #else
        #define LV_DEMO_MUSIC_SQUARE 0
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
        #define LV_DEMO_MUSIC_ROUND 0
    #endif
#endif

#ifndef LV_DEMO_MUSIC_LARGE
    #ifdef CONFIG_LV_DEMO_MUSIC_LARGE
        #define LV_DEMO_MUSIC_LARGE CONFIG_LV_DEMO_MUSIC_LARGE
    #else
        #define LV_DEMO_MUSIC_LARGE 0
    #endif
#endif

#ifndef LV_DEMO_MUSIC_AUTO_PLAY
    #ifdef CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
        #define LV_DEMO_MUSIC_AUTO_PLAY CONFIG_LV_DEMO_MUSIC_AUTO_PLAY
    #else
        #define LV_DEMO_MUSIC_AUTO_PLAY 0
    #endif
#endif

#ifndef LV_USE_DEMO_RENDER
    #ifdef CONFIG_LV_USE_DEMO_RENDER
        #define LV_USE_DEMO_RENDER CONFIG_LV_USE_DEMO_RENDER
    #else
        #define LV_USE_DEMO_RENDER 0
    #endif
#endif

#ifndef LV_USE_DEMO_STRESS
    #ifdef CONFIG_LV_USE_DEMO_STRESS
        #define LV_USE_DEMO_STRESS CONFIG_LV_USE_DEMO_STRESS
    #else
        #define LV_USE_DEMO_STRESS 0
    #endif
#endif

#ifndef LV_USE_DEMO_VECTOR_GRAPHIC
    #ifdef CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
        #define LV_USE_DEMO_VECTOR_GRAPHIC CONFIG_LV_USE_DEMO_VECTOR_GRAPHIC
    #else
        #define LV_USE_DEMO_VECTOR_GRAPHIC 0
    #endif
#endif

#ifndef LV_USE_DEMO_WIDGETS
    #ifdef CONFIG_LV_USE_DEMO_WIDGETS
        #define LV_USE_DEMO_WIDGETS CONFIG_LV_USE_DEMO_WIDGETS
    #else
        #define LV_USE_DEMO_WIDGETS 0
    #endif
#endif

#ifndef LV_USE_DEMO_FLEX_LAYOUT
    #ifdef CONFIG_LV_USE_DEMO_FLEX_LAYOUT
        #define LV_USE_DEMO_FLEX_LAYOUT CONFIG_LV_USE_DEMO_FLEX_LAYOUT
    #else
        #define LV_USE_DEMO_FLEX_LAYOUT 0
    #endif
#endif

#ifndef LV_USE_DEMO_MULTILANG
    #ifdef CONFIG_LV_USE_DEMO_MULTILANG
        #define LV_USE_DEMO_MULTILANG CONFIG_LV_USE_DEMO_MULTILANG
    #else
        #define LV_USE_DEMO_MULTILANG 0
    #endif
#endif

#ifndef LV_USE_DEMO_SMARTWATCH
    #ifdef CONFIG_LV_USE_DEMO_SMARTWATCH
        #define LV_USE_DEMO_SMARTWATCH CONFIG_LV_USE_DEMO_SMARTWATCH
    #else
        #define LV_USE_DEMO_SMARTWATCH 0
    #endif
#endif

#ifndef LV_USE_DEMO_EBIKE
    #ifdef CONFIG_LV_USE_DEMO_EBIKE
        #define LV_USE_DEMO_EBIKE CONFIG_LV_USE_DEMO_EBIKE
    #else
        #define LV_USE_DEMO_EBIKE 0
    #endif
#endif

#ifndef LV_DEMO_EBIKE_PORTRAIT
    #ifdef CONFIG_LV_DEMO_EBIKE_PORTRAIT
        #define LV_DEMO_EBIKE_PORTRAIT CONFIG_LV_DEMO_EBIKE_PORTRAIT
    #else
        #define LV_DEMO_EBIKE_PORTRAIT 0
    #endif
#endif

#ifndef LV_USE_DEMO_HIGH_RES
    #ifdef CONFIG_LV_USE_DEMO_HIGH_RES
        #define LV_USE_DEMO_HIGH_RES CONFIG_LV_USE_DEMO_HIGH_RES
    #else
        #define LV_USE_DEMO_HIGH_RES 0
    #endif
#endif




/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

/*----------------------------------
 * Start of compatibility block
 -----------------------------------*/

/*  
 *  TODO: Remove this for v10.
 */

/*
 *  Before the user selected either LV_USE_LZ4_INTERNAL or LV_USE_LZ4_EXTERNAL
 *  For v9.6 LV_USE_LZ4_EXTERNAL doesn't exist anymore, instead the user 
 *  enables LV_USE_LZ4 and disables LV_USE_LZ4_INTERNAL
 *  To support users using LV_USE_LZ4_EXTERNAL from before v9.6 we 
 *  we enable LV_USE_LZ4 for them
 */
#if defined(LV_USE_LZ4_EXTERNAL) && LV_USE_LZ4_EXTERNAL
#if !LV_USE_LZ4
    #warning LV_USE_LZ4_EXTERNAL is deprecated and will be removed in a future release. Enable LV_USE_LZ4 and disable LV_USE_LZ4_INTERNAL to continue using an external version of LZ4
    #undef LV_USE_LZ4
    #define LV_USE_LZ4 1
#endif /*!LV_USE_LZ4*/
#endif /*defined(LV_USE_LZ4_EXTERNAL) && LV_USE_LZ4_EXTERNAL*/

/*  
 *  Before the user selected either LV_USE_THORVG_INTERNAL or LV_USE_THORVG_EXTERNAL
 *  For v9.6 LV_USE_THORVG_EXTERNAL doesn't exist anymore, instead the user 
 *  enables LV_USE_THORVG and disables LV_USE_THORVG_INTERNAL
 *  To support users using LV_USE_THORVG_EXTERNAL from before v9.6 we 
 *  we enable LV_USE_THORVG for them
 */
#if defined(LV_USE_THORVG_EXTERNAL) && LV_USE_THORVG_EXTERNAL
#if !LV_USE_THORVG
    #warning LV_USE_THORVG_EXTERNAL is deprecated and will be removed in a future release. Enable LV_USE_THORVG and disable LV_USE_THORVG_INTERNAL to continue using an external version of THORVG
    #undef LV_USE_THORVG
    #define LV_USE_THORVG 1
#endif /*!LV_USE_THORVG*/
#endif /*defined(LV_USE_THORVG_EXTERNAL) && LV_USE_THORVG_EXTERNAL*/

/*  
 *  Backward compatibility. Before the user selected either 
 *  LV_X11_RENDER_MODE_PARTIAL or LV_X11_RENDER_MODE_DIRECT or
 *  LV_X11_RENDER_MODE_FULL. For v9.6, this becomes a single choice:
 *  LV_X11_RENDER_MODE which maps to a LV_DISPLAY_RENDER_MODE value.
 */
#if defined(LV_X11_RENDER_MODE_PARTIAL) && LV_X11_RENDER_MODE_PARTIAL
    #warning LV_X11_RENDER_MODE_PARTIAL is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_PARTIAL instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
#endif /*defined(LV_X11_RENDER_MODE_PARTIAL) && LV_X11_RENDER_MODE_PARTIAL*/

#if defined(LV_X11_RENDER_MODE_DIRECT) && LV_X11_RENDER_MODE_DIRECT
    #warning LV_X11_RENDER_MODE_DIRECT is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_DIRECT instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT
#endif /*defined(LV_X11_RENDER_MODE_DIRECT) && LV_X11_RENDER_MODE_DIRECT*/

#if defined(LV_X11_RENDER_MODE_FULL) && LV_X11_RENDER_MODE_FULL
    #warning LV_X11_RENDER_MODE_FULL is deprecated and will be removed in a future release. Set LV_X11_RENDER_MODE to LV_DISPLAY_RENDER_MODE_FULL instead.
    #undef LV_X11_RENDER_MODE
    #define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_FULL
#endif /*defined(LV_X11_RENDER_MODE_FULL) && LV_X11_RENDER_MODE_FULL*/

/*
 *  Before, the VG-Lite GPU was chosen with LV_VG_LITE_HAL_GPU_SERIES (a bare
 *  token such as gc255) and LV_VG_LITE_HAL_GPU_REVISION (a hex revision), which
 *  were pasted into the options include path.  For v9.x these are replaced by the
 *  LV_VG_LITE_GPU choice.  Map the old hex revisions (each unique to one series)
 *  to it; anything else falls back to the GC255 default.
 */
#if defined(LV_VG_LITE_HAL_GPU_REVISION)
    /* Only remap when LV_VG_LITE_GPU is still at its default, i.e. the user has not
    * migrated to it yet*/
    #if LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC255_0X40A
        #warning LV_VG_LITE_HAL_GPU_SERIES/LV_VG_LITE_HAL_GPU_REVISION are deprecated and will be removed in a future release. Select your GPU with LV_VG_LITE_GPU instead.
        #undef LV_VG_LITE_GPU
        #if LV_VG_LITE_HAL_GPU_REVISION == 0x423
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC555_0X423
        #elif LV_VG_LITE_HAL_GPU_REVISION == 0x1003
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GCNANOULTRAV_0X1003
        #else
            #define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC255_0X40A
        #endif
    #endif /*LV_VG_LITE_GPU == LV_VG_LITE_GPU_GC255_0X40A*/
#endif /*defined(LV_VG_LITE_HAL_GPU_REVISION)*/

/*
 *  Legacy backend inference for the Linux DRM and SDL drivers.  Historically the
 *  EGL backend was turned on automatically from LV_USE_OPENGLES; for v9.x each
 *  driver gets an explicit LV_<DRIVER>_BACKEND choice instead.  While the
 *  deprecated LV_<DRIVER>_AUTO_BACKEND is set (its default) we reproduce the old
 *  inference here and pre-define the per-driver flag, so the Derived-capability
 *  ladder below (which is #ifndef-guarded) leaves it untouched.  Runs before that
 *  ladder.  The #warning only fires when inference actually turns EGL on.
 */
#if LV_USE_LINUX_DRM && LV_LINUX_DRM_AUTO_BACKEND
    #ifndef LV_LINUX_DRM_USE_EGL
        #if LV_USE_OPENGLES
            #warning LV_LINUX_DRM_AUTO_BACKEND is deprecated and will be removed in a future release. Set it to 0 and select a backend with LV_LINUX_DRM_BACKEND.
            #define LV_LINUX_DRM_USE_EGL 1
        #else
            #define LV_LINUX_DRM_USE_EGL 0
        #endif
    #endif
    #ifndef LV_USE_LINUX_DRM_GBM_BUFFERS
        #define LV_USE_LINUX_DRM_GBM_BUFFERS LV_LINUX_DRM_USE_EGL
    #endif
#endif /*LV_USE_LINUX_DRM && LV_LINUX_DRM_AUTO_BACKEND*/

#if LV_USE_SDL && LV_SDL_AUTO_BACKEND
    #ifndef LV_SDL_USE_EGL
        #if LV_USE_OPENGLES && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
            #warning LV_SDL_AUTO_BACKEND is deprecated and will be removed in a future release. Set it to 0 and select a backend with LV_SDL_BACKEND.
            #define LV_SDL_USE_EGL 1
        #else
            #define LV_SDL_USE_EGL 0
        #endif
    #endif
#endif /*LV_USE_SDL && LV_SDL_AUTO_BACKEND*/

/* Wayland never inferred its backend from LV_USE_OPENGLES; the legacy interface
 * was setting LV_WAYLAND_USE_* directly.  While LV_WAYLAND_AUTO_BACKEND is set we
 * honor any such define, default to SHM, and keep the three mutually exclusive.
 * The #warning fires only if a legacy LV_WAYLAND_USE_* was set by hand. */
#if LV_USE_WAYLAND && LV_WAYLAND_AUTO_BACKEND
    #if defined(LV_WAYLAND_USE_EGL) || defined(LV_WAYLAND_USE_G2D) || defined(LV_WAYLAND_USE_SHM)
        #warning Setting LV_WAYLAND_USE_* directly is deprecated and will be removed in a future release. Set LV_WAYLAND_AUTO_BACKEND to 0 and select a backend with LV_WAYLAND_BACKEND.
    #endif
    #ifndef LV_WAYLAND_USE_EGL
        #define LV_WAYLAND_USE_EGL 0
    #endif
    #ifndef LV_WAYLAND_USE_G2D
        #define LV_WAYLAND_USE_G2D 0
    #endif
    #ifndef LV_WAYLAND_USE_SHM
        #if LV_WAYLAND_USE_EGL || LV_WAYLAND_USE_G2D
            #define LV_WAYLAND_USE_SHM 0
        #else
            #define LV_WAYLAND_USE_SHM 1
        #endif
    #endif
#endif /*LV_USE_WAYLAND && LV_WAYLAND_AUTO_BACKEND*/

#if defined(LV_ASSERT_HANDLER_INCLUDE) && !LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING
#warning "LV_ASSERT_HANDLER_INCLUDE is deprecated and will be removed in a future release. Use LV_ASSERT_CUSTOM_INCLUDE and define LV_ASSERT_HANDLER inside. To suppress this warning, remove LV_ASSERT_HANDLER_INCLUDE or enable LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING."
#include LV_ASSERT_HANDLER_INCLUDE
#endif


/*----------------------------------
 * End of compatibility block
 -----------------------------------*/

/* Derived capability flags (set via Kconfig `select`). */
#ifndef LV_OS_USE_PTHREAD
    #if (LV_USE_OS == LV_OS_PTHREAD)
        #define LV_OS_USE_PTHREAD 1
    #else
        #define LV_OS_USE_PTHREAD 0
    #endif
#endif

#ifndef LV_OS_USE_FREERTOS
    #if (LV_USE_OS == LV_OS_FREERTOS)
        #define LV_OS_USE_FREERTOS 1
    #else
        #define LV_OS_USE_FREERTOS 0
    #endif
#endif

#ifndef LV_OS_USE_CMSIS_RTOS2
    #if (LV_USE_OS == LV_OS_CMSIS_RTOS2)
        #define LV_OS_USE_CMSIS_RTOS2 1
    #else
        #define LV_OS_USE_CMSIS_RTOS2 0
    #endif
#endif

#ifndef LV_OS_USE_RTTHREAD
    #if (LV_USE_OS == LV_OS_RTTHREAD)
        #define LV_OS_USE_RTTHREAD 1
    #else
        #define LV_OS_USE_RTTHREAD 0
    #endif
#endif

#ifndef LV_OS_USE_WINDOWS
    #if (LV_USE_OS == LV_OS_WINDOWS)
        #define LV_OS_USE_WINDOWS 1
    #else
        #define LV_OS_USE_WINDOWS 0
    #endif
#endif

#ifndef LV_OS_USE_MQX
    #if (LV_USE_OS == LV_OS_MQX)
        #define LV_OS_USE_MQX 1
    #else
        #define LV_OS_USE_MQX 0
    #endif
#endif

#ifndef LV_OS_USE_SDL2
    #if (LV_USE_OS == LV_OS_SDL2)
        #define LV_OS_USE_SDL2 1
    #else
        #define LV_OS_USE_SDL2 0
    #endif
#endif

#ifndef LV_OS_USE_CUSTOM_INCLUDE
    #if (LV_USE_OS == LV_OS_CUSTOM)
        #define LV_OS_USE_CUSTOM_INCLUDE 1
    #else
        #define LV_OS_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_DRAW_HAS_VECTOR_SUPPORT
    #if ((LV_USE_THORVG && LV_USE_DRAW_SW) || LV_USE_DRAW_VG_LITE || (LV_USE_NEMA_VG && LV_USE_NEMA_GFX) || LV_USE_DRAW_NANOVG)
        #define LV_DRAW_HAS_VECTOR_SUPPORT 1
    #else
        #define LV_DRAW_HAS_VECTOR_SUPPORT 0
    #endif
#endif

#ifndef LV_DRAW_HAS_3D_SUPPORT
    #if (LV_USE_DRAW_NANOVG || LV_USE_DRAW_OPENGLES)
        #define LV_DRAW_HAS_3D_SUPPORT 1
    #else
        #define LV_DRAW_HAS_3D_SUPPORT 0
    #endif
#endif

#ifndef LV_DRAW_SW_ASM_USE_CUSTOM_INCLUDE
    #if ((LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM) && (LV_USE_DRAW_SW))
        #define LV_DRAW_SW_ASM_USE_CUSTOM_INCLUDE 1
    #else
        #define LV_DRAW_SW_ASM_USE_CUSTOM_INCLUDE 0
    #endif
#endif

#ifndef LV_LINUX_DRM_USE_EGL
    #if ((LV_LINUX_DRM_BACKEND == LV_LINUX_DRM_BACKEND_EGL) && (LV_USE_LINUX_DRM))
        #define LV_LINUX_DRM_USE_EGL 1
    #else
        #define LV_LINUX_DRM_USE_EGL 0
    #endif
#endif

#ifndef LV_USE_LINUX_DRM_GBM_BUFFERS
    #if ((LV_LINUX_DRM_BACKEND == LV_LINUX_DRM_BACKEND_GBM || LV_LINUX_DRM_BACKEND == LV_LINUX_DRM_BACKEND_EGL) && (LV_USE_LINUX_DRM))
        #define LV_USE_LINUX_DRM_GBM_BUFFERS 1
    #else
        #define LV_USE_LINUX_DRM_GBM_BUFFERS 0
    #endif
#endif

#ifndef LV_SDL_USE_EGL
    #if ((LV_SDL_BACKEND == LV_SDL_BACKEND_EGL && (LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)) && (LV_USE_SDL))
        #define LV_SDL_USE_EGL 1
    #else
        #define LV_SDL_USE_EGL 0
    #endif
#endif

#ifndef LV_WAYLAND_USE_SHM
    #if ((LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_SHM) && (LV_USE_WAYLAND))
        #define LV_WAYLAND_USE_SHM 1
    #else
        #define LV_WAYLAND_USE_SHM 0
    #endif
#endif

#ifndef LV_WAYLAND_USE_EGL
    #if ((LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_EGL) && (LV_USE_WAYLAND))
        #define LV_WAYLAND_USE_EGL 1
    #else
        #define LV_WAYLAND_USE_EGL 0
    #endif
#endif

#ifndef LV_WAYLAND_USE_G2D
    #if ((LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_G2D) && (LV_USE_WAYLAND))
        #define LV_WAYLAND_USE_G2D 1
    #else
        #define LV_WAYLAND_USE_G2D 0
    #endif
#endif

#ifndef LV_USE_EGL
    #if ((LV_LINUX_DRM_USE_EGL && LV_USE_LINUX_DRM) || (LV_SDL_USE_EGL && LV_USE_SDL) || (LV_WAYLAND_USE_EGL && LV_USE_WAYLAND))
        #define LV_USE_EGL 1
    #else
        #define LV_USE_EGL 0
    #endif
#endif

/* Optional user headers (LV_*_USE_CUSTOM_INCLUDE) overriding config macros. */
#if LV_OS_USE_CUSTOM_INCLUDE
    #include LV_OS_CUSTOM_INCLUDE
#endif

#if LV_DRAW_SW_ASM_USE_CUSTOM_INCLUDE
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

#if LV_NEMA_USE_CUSTOM_INCLUDE
    #include LV_NEMA_CUSTOM_INCLUDE
#endif

#if LV_FONT_USE_CUSTOM_INCLUDE
    #include LV_FONT_CUSTOM_INCLUDE
#endif

#if LV_SYSMON_USE_CUSTOM_INCLUDE
    #include LV_SYSMON_CUSTOM_INCLUDE
#endif

#if LV_ASSERT_USE_CUSTOM_INCLUDE
    #include LV_ASSERT_CUSTOM_INCLUDE
#endif

#if LV_ATTRIBUTE_USE_CUSTOM_INCLUDE
    #include LV_ATTRIBUTE_CUSTOM_INCLUDE
#endif

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

/* Kconfig enforces `depends on` / `select`; these checks catch a
 * hand-written lv_conf.h that violates them. */
#if LV_OS_IDLE_PERCENT_CUSTOM && !(LV_USE_OS == LV_OS_FREERTOS)
    #error "LV_OS_IDLE_PERCENT_CUSTOM requires LV_USE_OS == LV_OS_FREERTOS (Kconfig depends on)"
#endif

#if (LV_USE_VECTOR_GRAPHIC || LV_USE_DRAW_VG_LITE || LV_USE_DRAW_NANOVG || LV_USE_OPENGLES) && !LV_USE_MATRIX
    #error "LV_USE_MATRIX must be enabled: Kconfig selects it from LV_USE_VECTOR_GRAPHIC || LV_USE_DRAW_VG_LITE || LV_USE_DRAW_NANOVG || LV_USE_OPENGLES"
#endif

#if LV_DRAW_TRANSFORM_USE_MATRIX && !(LV_USE_MATRIX)
    #error "LV_DRAW_TRANSFORM_USE_MATRIX requires LV_USE_MATRIX (Kconfig depends on)"
#endif

#if ((LV_USE_SVG && LV_DRAW_HAS_VECTOR_SUPPORT) || (LV_USE_LOTTIE && LV_DRAW_HAS_VECTOR_SUPPORT && LV_USE_THORVG) || (LV_USE_DEMO_VECTOR_GRAPHIC && LV_DRAW_HAS_VECTOR_SUPPORT && LV_BUILD_DEMOS)) && !LV_USE_VECTOR_GRAPHIC
    #error "LV_USE_VECTOR_GRAPHIC must be enabled: Kconfig selects it from (LV_USE_SVG && LV_DRAW_HAS_VECTOR_SUPPORT) || (LV_USE_LOTTIE && LV_DRAW_HAS_VECTOR_SUPPORT && LV_USE_THORVG) || (LV_USE_DEMO_VECTOR_GRAPHIC && LV_DRAW_HAS_VECTOR_SUPPORT && LV_BUILD_DEMOS)"
#endif

#if (LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE) && !LV_USE_THORVG
    #error "LV_USE_THORVG must be enabled: Kconfig selects it from LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE"
#endif

#if LV_USE_DRAW_ARM2D_SYNC && !(LV_USE_DRAW_SW)
    #error "LV_USE_DRAW_ARM2D_SYNC requires LV_USE_DRAW_SW (Kconfig depends on)"
#endif

#if LV_USE_NATIVE_HELIUM_ASM && !(LV_USE_DRAW_SW)
    #error "LV_USE_NATIVE_HELIUM_ASM requires LV_USE_DRAW_SW (Kconfig depends on)"
#endif

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS && !(LV_USE_DRAW_SW)
    #error "LV_USE_DRAW_SW_COMPLEX_GRADIENTS requires LV_USE_DRAW_SW (Kconfig depends on)"
#endif

#if LV_VG_LITE_USE_GPU_INIT && !(LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_USE_GPU_INIT requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_USE_ASSERT && !(LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_USE_ASSERT requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_DISABLE_VLC_OP_CLOSE && !(LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_DISABLE_VLC_OP_CLOSE requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT && !(LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET && !(LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_USE_VG_LITE_DRIVER && !(LV_USE_DRAW_VG_LITE)
    #error "LV_USE_VG_LITE_DRIVER requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_USE_VG_LITE_THORVG && !(LV_USE_DRAW_VG_LITE)
    #error "LV_USE_VG_LITE_THORVG requires LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT && !(LV_USE_VG_LITE_THORVG && LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT requires LV_USE_VG_LITE_THORVG && LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_THORVG_YUV_SUPPORT && !(LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_THORVG_YUV_SUPPORT requires LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT && !(LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT requires LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_VG_LITE_THORVG_THREAD_RENDER && !(LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE)
    #error "LV_VG_LITE_THORVG_THREAD_RENDER requires LV_USE_VG_LITE_THORVG && LV_USE_DRAW_VG_LITE (Kconfig depends on)"
#endif

#if LV_NEMA_USE_CACHE && !(LV_USE_NEMA_GFX)
    #error "LV_NEMA_USE_CACHE requires LV_USE_NEMA_GFX (Kconfig depends on)"
#endif

#if LV_NEMA_USE_CUSTOM_INCLUDE && !(LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32 && LV_USE_NEMA_GFX)
    #error "LV_NEMA_USE_CUSTOM_INCLUDE requires LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32 && LV_USE_NEMA_GFX (Kconfig depends on)"
#endif

#if LV_USE_NEMA_VG && !(LV_USE_NEMA_GFX)
    #error "LV_USE_NEMA_VG requires LV_USE_NEMA_GFX (Kconfig depends on)"
#endif

#if LV_USE_ROTATE_PXP && !(LV_USE_DRAW_PXP)
    #error "LV_USE_ROTATE_PXP requires LV_USE_DRAW_PXP (Kconfig depends on)"
#endif

#if LV_USE_PXP_ASSERT && !(LV_USE_DRAW_PXP)
    #error "LV_USE_PXP_ASSERT requires LV_USE_DRAW_PXP (Kconfig depends on)"
#endif

#if LV_USE_PPA_IMG && !(LV_USE_PPA)
    #error "LV_USE_PPA_IMG requires LV_USE_PPA (Kconfig depends on)"
#endif

#if LV_USE_DRAW_DMA2D_INTERRUPT && !(LV_USE_DRAW_DMA2D)
    #error "LV_USE_DRAW_DMA2D_INTERRUPT requires LV_USE_DRAW_DMA2D (Kconfig depends on)"
#endif

#if (LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_G2D) && !LV_USE_DRAW_G2D
    #error "LV_USE_DRAW_G2D must be enabled: Kconfig selects it from LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_G2D"
#endif

#if LV_USE_G2D_ASSERT && !(LV_USE_DRAW_G2D && LV_USE_DRAW_G2D)
    #error "LV_USE_G2D_ASSERT requires LV_USE_DRAW_G2D && LV_USE_DRAW_G2D (Kconfig depends on)"
#endif

#if (LV_SDL_BACKEND == LV_SDL_BACKEND_TEXTURE) && !LV_USE_DRAW_SDL
    #error "LV_USE_DRAW_SDL must be enabled: Kconfig selects it from LV_SDL_BACKEND == LV_SDL_BACKEND_TEXTURE"
#endif

#if (LV_USE_MATRIX || LV_USE_GESTURE_RECOGNITION) && !LV_USE_FLOAT
    #error "LV_USE_FLOAT must be enabled: Kconfig selects it from LV_USE_MATRIX || LV_USE_GESTURE_RECOGNITION"
#endif

#if LV_LOG_PRINTF && !(LV_USE_LOG)
    #error "LV_LOG_PRINTF requires LV_USE_LOG (Kconfig depends on)"
#endif

#if LV_THEME_DEFAULT_DARK && !(LV_USE_THEME_DEFAULT)
    #error "LV_THEME_DEFAULT_DARK requires LV_USE_THEME_DEFAULT (Kconfig depends on)"
#endif

#if (LV_USE_DEMO_RENDER && LV_BUILD_DEMOS) && !LV_USE_GRID
    #error "LV_USE_GRID must be enabled: Kconfig selects it from LV_USE_DEMO_RENDER && LV_BUILD_DEMOS"
#endif

#if (LV_USE_TEST_SCREENSHOT_COMPARE && LV_USE_TEST) && !LV_USE_LODEPNG
    #error "LV_USE_LODEPNG must be enabled: Kconfig selects it from LV_USE_TEST_SCREENSHOT_COMPARE && LV_USE_TEST"
#endif

#if LV_USE_SVG && !(LV_DRAW_HAS_VECTOR_SUPPORT)
    #error "LV_USE_SVG requires LV_DRAW_HAS_VECTOR_SUPPORT (Kconfig depends on)"
#endif

#if LV_USE_SVG_ANIMATION && !(LV_USE_SVG)
    #error "LV_USE_SVG_ANIMATION requires LV_USE_SVG (Kconfig depends on)"
#endif

#if LV_USE_SVG_DEBUG && !(LV_USE_SVG)
    #error "LV_USE_SVG_DEBUG requires LV_USE_SVG (Kconfig depends on)"
#endif

#if LV_FREETYPE_USE_LVGL_PORT && !(LV_USE_FREETYPE)
    #error "LV_FREETYPE_USE_LVGL_PORT requires LV_USE_FREETYPE (Kconfig depends on)"
#endif

#if LV_TINY_TTF_FILE_SUPPORT && !(LV_USE_TINY_TTF)
    #error "LV_TINY_TTF_FILE_SUPPORT requires LV_USE_TINY_TTF (Kconfig depends on)"
#endif

#if (LV_USE_GLTF && LV_DRAW_HAS_3D_SUPPORT) && !LV_USE_3DTEXTURE
    #error "LV_USE_3DTEXTURE must be enabled: Kconfig selects it from LV_USE_GLTF && LV_DRAW_HAS_3D_SUPPORT"
#endif

#if LV_USE_3DTEXTURE && !(LV_DRAW_HAS_3D_SUPPORT)
    #error "LV_USE_3DTEXTURE requires LV_DRAW_HAS_3D_SUPPORT (Kconfig depends on)"
#endif

#if (LV_USE_SLIDER) && !LV_USE_BAR
    #error "LV_USE_BAR must be enabled: Kconfig selects it from LV_USE_SLIDER"
#endif

#if LV_CALENDAR_WEEK_STARTS_MONDAY && !(LV_USE_CALENDAR)
    #error "LV_CALENDAR_WEEK_STARTS_MONDAY requires LV_USE_CALENDAR (Kconfig depends on)"
#endif

#if LV_USE_CALENDAR_CHINESE && !(LV_USE_CALENDAR)
    #error "LV_USE_CALENDAR_CHINESE requires LV_USE_CALENDAR (Kconfig depends on)"
#endif

#if (LV_USE_BARCODE || LV_USE_QRCODE) && !LV_USE_CANVAS
    #error "LV_USE_CANVAS must be enabled: Kconfig selects it from LV_USE_BARCODE || LV_USE_QRCODE"
#endif

#if LV_FFMPEG_DUMP_FORMAT && !(LV_USE_FFMPEG)
    #error "LV_FFMPEG_DUMP_FORMAT requires LV_USE_FFMPEG (Kconfig depends on)"
#endif

#if LV_FFMPEG_PLAYER_USE_LV_FS && !(LV_USE_FFMPEG)
    #error "LV_FFMPEG_PLAYER_USE_LV_FS requires LV_USE_FFMPEG (Kconfig depends on)"
#endif

#if LV_GIF_CACHE_DECODE_DATA && !(LV_USE_GIF)
    #error "LV_GIF_CACHE_DECODE_DATA requires LV_USE_GIF (Kconfig depends on)"
#endif

#if LV_USE_GLTF && !(LV_DRAW_HAS_3D_SUPPORT)
    #error "LV_USE_GLTF requires LV_DRAW_HAS_3D_SUPPORT (Kconfig depends on)"
#endif

#if (LV_USE_CANVAS || LV_USE_GSTREAMER) && !LV_USE_IMAGE
    #error "LV_USE_IMAGE must be enabled: Kconfig selects it from LV_USE_CANVAS || LV_USE_GSTREAMER"
#endif

#if (LV_USE_IME_PINYIN) && !LV_USE_KEYBOARD
    #error "LV_USE_KEYBOARD must be enabled: Kconfig selects it from LV_USE_IME_PINYIN"
#endif

#if (LV_USE_DROPDOWN || LV_USE_IMAGE || LV_USE_ROLLER || LV_USE_TEXTAREA) && !LV_USE_LABEL
    #error "LV_USE_LABEL must be enabled: Kconfig selects it from LV_USE_DROPDOWN || LV_USE_IMAGE || LV_USE_ROLLER || LV_USE_TEXTAREA"
#endif

#if LV_USE_LOTTIE && !(LV_DRAW_HAS_VECTOR_SUPPORT && LV_USE_THORVG)
    #error "LV_USE_LOTTIE requires LV_DRAW_HAS_VECTOR_SUPPORT && LV_USE_THORVG (Kconfig depends on)"
#endif

#if (LV_USE_FILE_EXPLORER) && !LV_USE_TABLE
    #error "LV_USE_TABLE must be enabled: Kconfig selects it from LV_USE_FILE_EXPLORER"
#endif

#if LV_LINUX_FBDEV_BSD && !(LV_USE_LINUX_FBDEV)
    #error "LV_LINUX_FBDEV_BSD requires LV_USE_LINUX_FBDEV (Kconfig depends on)"
#endif

#if (LV_USE_ST7735 || LV_USE_ST7789 || LV_USE_ST7796 || LV_USE_ILI9341 || LV_USE_NV3007) && !LV_USE_GENERIC_MIPI
    #error "LV_USE_GENERIC_MIPI must be enabled: Kconfig selects it from LV_USE_ST7735 || LV_USE_ST7789 || LV_USE_ST7796 || LV_USE_ILI9341 || LV_USE_NV3007"
#endif

#if LV_ST_LTDC_USE_DMA2D_FLUSH && !(!LV_USE_DRAW_DMA2D && LV_USE_ST_LTDC)
    #error "LV_ST_LTDC_USE_DMA2D_FLUSH requires !LV_USE_DRAW_DMA2D && LV_USE_ST_LTDC (Kconfig depends on)"
#endif

#if LV_LIBINPUT_BSD && !(LV_USE_LIBINPUT)
    #error "LV_LIBINPUT_BSD requires LV_USE_LIBINPUT (Kconfig depends on)"
#endif

#if LV_LIBINPUT_XKB && !(LV_USE_LIBINPUT)
    #error "LV_LIBINPUT_XKB requires LV_USE_LIBINPUT (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP && !(LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP && LV_USE_NUTTX)
    #error "LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP requires LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP && LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_LIBUV && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_LIBUV requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_CUSTOM_INIT && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_CUSTOM_INIT requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_LCD && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_LCD requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_TOUCHSCREEN && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_TOUCHSCREEN requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_MOUSE && !(LV_USE_NUTTX)
    #error "LV_USE_NUTTX_MOUSE requires LV_USE_NUTTX (Kconfig depends on)"
#endif

#if LV_USE_NUTTX_TRACE_FILE && !(LV_USE_PROFILER_BUILTIN && LV_USE_NUTTX)
    #error "LV_USE_NUTTX_TRACE_FILE requires LV_USE_PROFILER_BUILTIN && LV_USE_NUTTX (Kconfig depends on)"
#endif

#if (LV_USE_DRAW_OPENGLES || LV_LINUX_DRM_BACKEND == LV_LINUX_DRM_BACKEND_EGL || (LV_USE_GLFW && !LV_USE_EGL) || LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_EGL) && !LV_USE_OPENGLES
    #error "LV_USE_OPENGLES must be enabled: Kconfig selects it from LV_USE_DRAW_OPENGLES || LV_LINUX_DRM_BACKEND == LV_LINUX_DRM_BACKEND_EGL || (LV_USE_GLFW && !LV_USE_EGL) || LV_WAYLAND_BACKEND == LV_WAYLAND_BACKEND_EGL"
#endif

#if LV_USE_OPENGLES_DEBUG && !(LV_USE_OPENGLES)
    #error "LV_USE_OPENGLES_DEBUG requires LV_USE_OPENGLES (Kconfig depends on)"
#endif

#if LV_USE_GLFW && !(!LV_USE_EGL)
    #error "LV_USE_GLFW requires !LV_USE_EGL (Kconfig depends on)"
#endif

#if LV_SDL_FULLSCREEN && !(LV_USE_SDL)
    #error "LV_SDL_FULLSCREEN requires LV_USE_SDL (Kconfig depends on)"
#endif

#if LV_UEFI_USE_MEMORY_SERVICES && !(LV_USE_UEFI)
    #error "LV_UEFI_USE_MEMORY_SERVICES requires LV_USE_UEFI (Kconfig depends on)"
#endif

#if LV_USE_WINDOWS && !(LV_USE_OS == LV_OS_WINDOWS)
    #error "LV_USE_WINDOWS requires LV_USE_OS == LV_OS_WINDOWS (Kconfig depends on)"
#endif

#if (LV_USE_DRAW_NANOVG) && !LV_USE_NANOVG
    #error "LV_USE_NANOVG must be enabled: Kconfig selects it from LV_USE_DRAW_NANOVG"
#endif

#if LV_SYSMON_PROC_IDLE_AVAILABLE && !(LV_USE_SYSMON)
    #error "LV_SYSMON_PROC_IDLE_AVAILABLE requires LV_USE_SYSMON (Kconfig depends on)"
#endif

#if LV_USE_PERF_MONITOR && !(LV_USE_SYSMON)
    #error "LV_USE_PERF_MONITOR requires LV_USE_SYSMON (Kconfig depends on)"
#endif

#if LV_USE_PERF_MONITOR_LOG_MODE && !(LV_USE_PERF_MONITOR && LV_USE_SYSMON)
    #error "LV_USE_PERF_MONITOR_LOG_MODE requires LV_USE_PERF_MONITOR && LV_USE_SYSMON (Kconfig depends on)"
#endif

#if LV_USE_MEM_MONITOR && !(LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_USE_SYSMON)
    #error "LV_USE_MEM_MONITOR requires LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_USE_SYSMON (Kconfig depends on)"
#endif

#if LV_SYSMON_USE_CUSTOM_INCLUDE && !(LV_USE_SYSMON)
    #error "LV_SYSMON_USE_CUSTOM_INCLUDE requires LV_USE_SYSMON (Kconfig depends on)"
#endif

#if (LV_USE_PROFILER_BUILTIN) && !LV_USE_PROFILER
    #error "LV_USE_PROFILER must be enabled: Kconfig selects it from LV_USE_PROFILER_BUILTIN"
#endif

#if LV_USE_PROFILER_BUILTIN_POSIX && !(LV_USE_PROFILER_BUILTIN)
    #error "LV_USE_PROFILER_BUILTIN_POSIX requires LV_USE_PROFILER_BUILTIN (Kconfig depends on)"
#endif

#if LV_PROFILER_STYLE && !(LV_USE_PROFILER)
    #error "LV_PROFILER_STYLE requires LV_USE_PROFILER (Kconfig depends on)"
#endif

#if LV_USE_TEST_SCREENSHOT_COMPARE && !(LV_USE_TEST)
    #error "LV_USE_TEST_SCREENSHOT_COMPARE requires LV_USE_TEST (Kconfig depends on)"
#endif

#if LV_GLOBAL_USE_CUSTOM_INCLUDE && !(LV_ENABLE_GLOBAL_CUSTOM)
    #error "LV_GLOBAL_USE_CUSTOM_INCLUDE requires LV_ENABLE_GLOBAL_CUSTOM (Kconfig depends on)"
#endif

#if LV_CHECK_ARG_ASSERT_ON_FAIL && !(LV_USE_CHECK_ARG)
    #error "LV_CHECK_ARG_ASSERT_ON_FAIL requires LV_USE_CHECK_ARG (Kconfig depends on)"
#endif

#if LV_USE_CHECK_OBJ_CLASSTYPE && !(LV_USE_CHECK_ARG)
    #error "LV_USE_CHECK_OBJ_CLASSTYPE requires LV_USE_CHECK_ARG (Kconfig depends on)"
#endif

#if LV_USE_CHECK_OBJ_VALIDITY && !(LV_USE_CHECK_ARG)
    #error "LV_USE_CHECK_OBJ_VALIDITY requires LV_USE_CHECK_ARG (Kconfig depends on)"
#endif

#if LV_USE_DEMO_BENCHMARK && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_BENCHMARK requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_BENCHMARK_ALIGNED_FONTS && !(LV_USE_DEMO_BENCHMARK && LV_BUILD_DEMOS)
    #error "LV_DEMO_BENCHMARK_ALIGNED_FONTS requires LV_USE_DEMO_BENCHMARK && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_GLTF && !(LV_USE_GLTF && LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_GLTF requires LV_USE_GLTF && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_KEYPAD_AND_ENCODER && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_KEYPAD_AND_ENCODER requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_MUSIC && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_MUSIC requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_MUSIC_SQUARE && !(LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS)
    #error "LV_DEMO_MUSIC_SQUARE requires LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_MUSIC_LANDSCAPE && !(LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS)
    #error "LV_DEMO_MUSIC_LANDSCAPE requires LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_MUSIC_ROUND && !(LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS)
    #error "LV_DEMO_MUSIC_ROUND requires LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_MUSIC_LARGE && !(LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS)
    #error "LV_DEMO_MUSIC_LARGE requires LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_MUSIC_AUTO_PLAY && !(LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS)
    #error "LV_DEMO_MUSIC_AUTO_PLAY requires LV_USE_DEMO_MUSIC && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_RENDER && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_RENDER requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_STRESS && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_STRESS requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_VECTOR_GRAPHIC && !(LV_DRAW_HAS_VECTOR_SUPPORT && LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_VECTOR_GRAPHIC requires LV_DRAW_HAS_VECTOR_SUPPORT && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if (LV_USE_DEMO_BENCHMARK && LV_BUILD_DEMOS) && !LV_USE_DEMO_WIDGETS
    #error "LV_USE_DEMO_WIDGETS must be enabled: Kconfig selects it from LV_USE_DEMO_BENCHMARK && LV_BUILD_DEMOS"
#endif

#if LV_USE_DEMO_WIDGETS && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_WIDGETS requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_FLEX_LAYOUT && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_FLEX_LAYOUT requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_MULTILANG && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_MULTILANG requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_SMARTWATCH && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_SMARTWATCH requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_EBIKE && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_EBIKE requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_DEMO_EBIKE_PORTRAIT && !(LV_USE_DEMO_EBIKE && LV_BUILD_DEMOS)
    #error "LV_DEMO_EBIKE_PORTRAIT requires LV_USE_DEMO_EBIKE && LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#if LV_USE_DEMO_HIGH_RES && !(LV_BUILD_DEMOS)
    #error "LV_USE_DEMO_HIGH_RES requires LV_BUILD_DEMOS (Kconfig depends on)"
#endif

#endif  /*LV_CONF_INTERNAL_H*/

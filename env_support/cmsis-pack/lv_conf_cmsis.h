/**
 * @file lv_conf_cmsis.h
 * Configuration file for v9.4.0
 */

/* clang-format off */
#if 1 /* Set this to "1" to enable content */

#ifndef LV_CONF_CMSIS_H
#define LV_CONF_CMSIS_H

#if defined(_RTE_)
#include "RTE_Components.h"
#endif

/* If you need to include anything here, do it inside the `__ASSEMBLY__` guard */
#if  0 && defined(__ASSEMBLY__)
#include "my_include.h"
#endif

/*====================
   COLOR SETTINGS
 *====================*/

/** Color depth: 1 (I1), 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#define LV_COLOR_DEPTH 16

/** Swap the high and low bytes for RGB565 */
#define LV_COLOR_16_SWAP 0

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
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN

/** Possible values
 * - LV_STDLIB_BUILTIN:     LVGL's built in implementation
 * - LV_STDLIB_CLIB:        Standard C functions, like malloc, strlen, etc
 * - LV_STDLIB_MICROPYTHON: MicroPython implementation
 * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
 * - LV_STDLIB_CUSTOM:      Implement the functions externally
 */
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN

/** Possible values
 * - LV_STDLIB_BUILTIN:     LVGL's built in implementation
 * - LV_STDLIB_CLIB:        Standard C functions, like malloc, strlen, etc
 * - LV_STDLIB_MICROPYTHON: MicroPython implementation
 * - LV_STDLIB_RTTHREAD:    RT-Thread implementation
 * - LV_STDLIB_CUSTOM:      Implement the functions externally
 */
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

#define LV_STDINT_INCLUDE       <stdint.h>
#define LV_STDDEF_INCLUDE       <stddef.h>
#define LV_STDBOOL_INCLUDE      <stdbool.h>
#define LV_INTTYPES_INCLUDE     <inttypes.h>
#define LV_LIMITS_INCLUDE       <limits.h>
#define LV_STDARG_INCLUDE       <stdarg.h>

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
    /** Size of memory available for `lv_malloc()` in bytes (>= 2kB) */
    #define LV_MEM_SIZE (128 * 1024U)          /**< [bytes] */

    /** Size of the memory expand for `lv_malloc()` in bytes */
    #define LV_MEM_POOL_EXPAND_SIZE 0

    /** Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too. */
    #define LV_MEM_ADR 0     /**< 0: unused*/
    /* Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc */
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE
        #undef LV_MEM_POOL_ALLOC
    #endif
#endif  /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

/*====================
   HAL SETTINGS
 *====================*/

/** Default display refresh, input device read and animation step period. */
#define LV_DEF_REFR_PERIOD  33      /**< [ms] */

/** Default Dots Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 * (Not so important, you can adjust it to modify default sizes and spaces.) */
#define LV_DPI_DEF 130              /**< [px/inch] */

/*customize tick-get */
#if defined(__PERF_COUNTER__) && __PERF_COUNTER__
    #define LV_GLOBAL_INIT(__GLOBAL_PTR)                                    \
            do {                                                            \
                lv_global_init((lv_global_t *)(__GLOBAL_PTR));              \
                extern uint32_t perfc_tick_get(void);                       \
                (__GLOBAL_PTR)->tick_state.tick_get_cb = perfc_tick_get;    \
            } while(0)
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

#if LV_USE_OS == LV_OS_CUSTOM
    #define LV_OS_CUSTOM_INCLUDE <stdint.h>
#endif
#if LV_USE_OS == LV_OS_FREERTOS
    /*
     * Unblocking an RTOS task with a direct notification is 45% faster and uses less RAM
     * than unblocking a task using an intermediary object such as a binary semaphore.
     * RTOS task notifications can only be used when there is only one task that can be the recipient of the event.
     */
    #define LV_USE_FREERTOS_TASK_NOTIFY 1
#endif

/*========================
 * RENDERING CONFIGURATION
 *========================*/

/** Align stride of all layers and images to this bytes */
#define LV_DRAW_BUF_STRIDE_ALIGN                4

/** Align start address of draw_buf addresses to this bytes*/
#define LV_DRAW_BUF_ALIGN                       4

/** Using matrix for transformations.
 * Requirements:
 * - `LV_USE_MATRIX = 1`.
 * - Rendering engine needs to support 3x3 matrix transformations. */
#define LV_DRAW_TRANSFORM_USE_MATRIX            0

/* If a widget has `style_opa < 255` (not `bg_opa`, `text_opa` etc) or not NORMAL blend mode
 * it is buffered into a "simple" layer before rendering. The widget can be buffered in smaller chunks.
 * "Transformed layers" (if `transform_angle/zoom` are set) use larger buffers
 * and can't be drawn in chunks. */

/** The target buffer size for simple layer chunks. */
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE    (24 * 1024)    /**< [bytes]*/

/* Limit the max allocated memory for simple and transformed layers.
 * It should be at least `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` sized but if transformed layers are also used
 * it should be enough to store the largest widget too (width x height x 4 area).
 * Set it to 0 to have no limit. */
#define LV_DRAW_LAYER_MAX_MEMORY 0  /**< No limit by default [bytes]*/

/** Stack size of drawing thread.
 * NOTE: If FreeType or ThorVG is enabled, it is recommended to set it to 32KB or more.
 */
#define LV_DRAW_THREAD_STACK_SIZE    (8 * 1024)         /**< [bytes]*/

/** Thread priority of the drawing task.
 *  Higher values mean higher priority.
 *  Can use values from lv_thread_prio_t enum in lv_os.h: LV_THREAD_PRIO_LOWEST,
 *  LV_THREAD_PRIO_LOW, LV_THREAD_PRIO_MID, LV_THREAD_PRIO_HIGH, LV_THREAD_PRIO_HIGHEST
 *  Make sure the priority value aligns with the OS-specific priority levels.
 *  On systems with limited priority levels (e.g., FreeRTOS), a higher value can improve
 *  rendering performance but might cause other tasks to starve. */
#define LV_DRAW_THREAD_PRIO LV_THREAD_PRIO_HIGH

#define LV_USE_DRAW_SW 1
#if LV_USE_DRAW_SW == 1
    /*
     * Selectively disable color format support in order to reduce code size.
     * NOTE: some features use certain color formats internally, e.g.
     * - gradients use RGB888
     * - bitmaps with transparency may use ARGB8888
     */
    #define LV_DRAW_SW_SUPPORT_RGB565       1
    #define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED       1
    #define LV_DRAW_SW_SUPPORT_RGB565A8     1
    #define LV_DRAW_SW_SUPPORT_RGB888       1
    #define LV_DRAW_SW_SUPPORT_XRGB8888     1
    #define LV_DRAW_SW_SUPPORT_ARGB8888     1
    #define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1
    #define LV_DRAW_SW_SUPPORT_L8           1
    #define LV_DRAW_SW_SUPPORT_AL88         1
    #define LV_DRAW_SW_SUPPORT_A8           1
    #define LV_DRAW_SW_SUPPORT_I1           1

    /* The threshold of the luminance to consider a pixel as
     * active in indexed color format */
    #define LV_DRAW_SW_I1_LUM_THRESHOLD 127

    /** Set number of draw units.
     *  - > 1 requires operating system to be enabled in `LV_USE_OS`.
     *  - > 1 means multiple threads will render the screen in parallel. */
    #define LV_DRAW_SW_DRAW_UNIT_CNT    1

    /** Enable native helium assembly to be compiled. */
    #define LV_USE_NATIVE_HELIUM_ASM    0

    /**
     * - 0: Use a simple renderer capable of drawing only simple rectangles with gradient, images, text, and straight lines only.
     * - 1: Use a complex renderer capable of drawing rounded corners, shadow, skew lines, and arcs too. */
    #define LV_DRAW_SW_COMPLEX          1

    #if LV_DRAW_SW_COMPLEX == 1
        /** Allow buffering some shadow calculation.
         *  LV_DRAW_SW_SHADOW_CACHE_SIZE is the maximum shadow size to buffer, where shadow size is
         *  `shadow_width + radius`.  Caching has LV_DRAW_SW_SHADOW_CACHE_SIZE^2 RAM cost. */
        #define LV_DRAW_SW_SHADOW_CACHE_SIZE 0

        /** Set number of maximally-cached circle data.
         *  The circumference of 1/4 circle are saved for anti-aliasing.
         *  `radius * 4` bytes are used per circle (the most often used radiuses are saved).
         *  - 0: disables caching */
        #define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4
    #endif

    #if !defined(LV_USE_DRAW_SW_ASM) && defined(RTE_Acceleration_Arm_2D)
        /*turn-on helium acceleration when Arm-2D and the Helium-powered device are detected */
        #if defined(__ARM_FEATURE_MVE) && __ARM_FEATURE_MVE
            #define LV_USE_DRAW_SW_ASM      LV_DRAW_SW_ASM_HELIUM
            #define LV_USE_DRAW_ARM2D_SYNC  1
        #endif
    #endif

    #ifndef LV_USE_DRAW_SW_ASM
        #define  LV_USE_DRAW_SW_ASM     LV_DRAW_SW_ASM_NONE
    #endif

    #if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
        #define  LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""
    #endif

    /** Enable drawing complex gradients in software: linear at an angle, radial or conical */
    #define LV_USE_DRAW_SW_COMPLEX_GRADIENTS    0

#endif

/*Use TSi's aka (Think Silicon) NemaGFX */
#if LV_USE_NEMA_GFX
    /** Select which NemaGFX HAL to use. Possible options:
     * - LV_NEMA_HAL_CUSTOM
     * - LV_NEMA_HAL_STM32 */
    #define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM
    #if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
        #define LV_NEMA_STM32_HAL_INCLUDE <stm32u5xx_hal.h>
    #endif

    /*Enable Vector Graphics Operations. Available only if NemaVG library is present*/
    #define LV_USE_NEMA_VG 0
    #if LV_USE_NEMA_VG
        /*Define application's resolution used for VG related buffer allocation */
        #define LV_NEMA_GFX_MAX_RESX 800
        #define LV_NEMA_GFX_MAX_RESY 600
    #endif
#endif


/** Use NXP's PXP on iMX RTxxx platforms. */
#if LV_USE_PXP
    /** Use PXP for drawing.*/
    #define LV_USE_DRAW_PXP 1

    /** Use PXP to rotate display.*/
    #define LV_USE_ROTATE_PXP 0

    #if LV_USE_DRAW_PXP && LV_USE_OS
        /** Use additional draw thread for PXP processing.*/
        #define LV_USE_PXP_DRAW_THREAD 1
    #endif

    /** Enable PXP asserts. */
    #define LV_USE_PXP_ASSERT 0
#endif

/** Use NXP's G2D on MPU platforms. */
#if LV_USE_G2D
    /** Use G2D for drawing. **/
    #define LV_USE_DRAW_G2D 1

    /** Use G2D to rotate display. **/
    #define LV_USE_ROTATE_G2D 0

    /** Maximum number of buffers that can be stored for G2D draw unit.
     *  Includes the frame buffers and assets. */
    #define LV_G2D_HASH_TABLE_SIZE 50

    #if LV_USE_DRAW_G2D && LV_USE_OS
        /** Use additional draw thread for G2D processing.*/
        #define LV_USE_G2D_DRAW_THREAD 1
    #endif

    /** Enable G2D asserts. */
    #define LV_USE_G2D_ASSERT 0
#endif

/** Use VG-Lite GPU. */
#if LV_USE_DRAW_VG_LITE
    /** Enable VG-Lite custom external 'gpu_init()' function */
    #define LV_VG_LITE_USE_GPU_INIT 0

    /** Enable VG-Lite assert. */
    #define LV_VG_LITE_USE_ASSERT 0

    /** VG-Lite flush commit trigger threshold. GPU will try to batch these many draw tasks. */
    #define LV_VG_LITE_FLUSH_MAX_COUNT 8

    /** Enable border to simulate shadow.
     *  NOTE: which usually improves performance,
     *  but does not guarantee the same rendering quality as the software. */
    #define LV_VG_LITE_USE_BOX_SHADOW 1

    /** VG-Lite gradient maximum cache number.
     *  @note  The memory usage of a single gradient image is 4K bytes. */
    #define LV_VG_LITE_GRAD_CACHE_CNT 32

    /** VG-Lite stroke maximum cache number. */
    #define LV_VG_LITE_STROKE_CACHE_CNT 32

    /** Remove VLC_OP_CLOSE path instruction (Workaround for NXP) **/
    #define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0

    /** Disable linear gradient extension for some older versions of drivers. */
    #define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0
#endif

/** Accelerate blends, fills, etc. with STM32 DMA2D */
#if LV_USE_DRAW_DMA2D
    #define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"

    /* if enabled, the user is required to call `lv_draw_dma2d_transfer_complete_interrupt_handler`
     * upon receiving the DMA2D global interrupt
     */
    #define LV_USE_DRAW_DMA2D_INTERRUPT 0
#endif

/** Draw using cached OpenGLES textures. Requires LV_USE_OPENGLES */
#define LV_USE_DRAW_OPENGLES 0

#if LV_USE_DRAW_OPENGLES
    #define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64
#endif

/** Draw using espressif PPA accelerator */
#if LV_USE_PPA
    #define LV_USE_PPA_IMG 0
#endif

/* Use EVE FT81X GPU. */
#if LV_USE_DRAW_EVE
    /* EVE_GEN value: 2, 3, or 4 */
    #define LV_DRAW_EVE_EVE_GENERATION 4

    /* The maximum number of bytes to buffer before a single SPI transmission.
     * Set it to 0 to disable write buffering.
     */
    #define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Logging
 *-----------*/

/** Enable log module */
#define LV_USE_LOG 0
#if LV_USE_LOG
    /** Set value to one of the following levels of logging detail:
     *  - LV_LOG_LEVEL_TRACE    Log detailed information.
     *  - LV_LOG_LEVEL_INFO     Log important events.
     *  - LV_LOG_LEVEL_WARN     Log if something unwanted happened but didn't cause a problem.
     *  - LV_LOG_LEVEL_ERROR    Log only critical issues, when system may fail.
     *  - LV_LOG_LEVEL_USER     Log only custom log messages added by the user.
     *  - LV_LOG_LEVEL_NONE     Do not log anything. */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_USER

    /** - 1: Print log with 'printf';
     *  - 0: User needs to register a callback with `lv_log_register_print_cb()`. */
    #define LV_LOG_PRINTF 1

    /** Set callback to print logs.
     *  E.g `my_print`. The prototype should be `void my_print(lv_log_level_t level, const char * buf)`.
     *  Can be overwritten by `lv_log_register_print_cb`. */
    //#define LV_LOG_PRINT_CB

    /** - 1: Enable printing timestamp;
     *  - 0: Disable printing timestamp. */
    #define LV_LOG_USE_TIMESTAMP 1

    /** - 1: Print file and line number of the log;
     *  - 0: Do not print file and line number of the log. */
    #define LV_LOG_USE_FILE_LINE 1

    /* Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs. */
    #define LV_LOG_TRACE_MEM        1   /**< Enable/disable trace logs in memory operations. */
    #define LV_LOG_TRACE_TIMER      1   /**< Enable/disable trace logs in timer operations. */
    #define LV_LOG_TRACE_INDEV      1   /**< Enable/disable trace logs in input device operations. */
    #define LV_LOG_TRACE_DISP_REFR  1   /**< Enable/disable trace logs in display re-draw operations. */
    #define LV_LOG_TRACE_EVENT      1   /**< Enable/disable trace logs in event dispatch logic. */
    #define LV_LOG_TRACE_OBJ_CREATE 1   /**< Enable/disable trace logs in object creation (core `obj` creation plus every widget). */
    #define LV_LOG_TRACE_LAYOUT     1   /**< Enable/disable trace logs in flex- and grid-layout operations. */
    #define LV_LOG_TRACE_ANIM       1   /**< Enable/disable trace logs in animation logic. */
    #define LV_LOG_TRACE_CACHE      1   /**< Enable/disable trace logs in cache operations. */
#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts
 *-----------*/

/* Enable assertion failures if an operation fails or invalid data is found.
 * If LV_USE_LOG is enabled, an error message will be printed on failure. */
#define LV_USE_ASSERT_NULL          1   /**< Check if the parameter is NULL. (Very fast, recommended) */
#define LV_USE_ASSERT_MALLOC        1   /**< Checks is the memory is successfully allocated or no. (Very fast, recommended) */
#define LV_USE_ASSERT_STYLE         0   /**< Check if the styles are properly initialized. (Very fast, recommended) */
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /**< Check the integrity of `lv_mem` after critical operations. (Slow) */
#define LV_USE_ASSERT_OBJ           0   /**< Check the object's type and existence (e.g. not deleted). (Slow) */

/** Add a custom handler when assert happens e.g. to restart MCU. */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);     /**< Halt by default */

/*-------------
 * Debug
 *-----------*/

/** 1: Draw random colored rectangles over the redrawn areas. */
#define LV_USE_REFR_DEBUG 0

/** 1: Draw a red overlay for ARGB layers and a green overlay for RGB layers*/
#define LV_USE_LAYER_DEBUG 0

/** 1: Adds the following behaviors for debugging:
 *  - Draw overlays with different colors for each draw_unit's tasks.
 *  - Draw index number of draw unit on white background.
 *  - For layers, draws index number of draw unit on black background. */
#define LV_USE_PARALLEL_DRAW_DEBUG 0

/*-------------
 * Others
 *-----------*/

#define LV_ENABLE_GLOBAL_CUSTOM 0
#if LV_ENABLE_GLOBAL_CUSTOM
    /** Header to include for custom 'lv_global' function" */
    #define LV_GLOBAL_CUSTOM_INCLUDE <stdint.h>
#endif

/** Default cache size in bytes.
 *  Used by image decoders such as `lv_lodepng` to keep the decoded image in memory.
 *  If size is not set to 0, the decoder will fail to decode when the cache is full.
 *  If size is 0, the cache function is not enabled and the decoded memory will be
 *  released immediately after use. */
#define LV_CACHE_DEF_SIZE       0

/** Default number of image header cache entries. The cache is used to store the headers of images
 *  The main logic is like `LV_CACHE_DEF_SIZE` but for image headers. */
#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0

/** Number of stops allowed per gradient. Increase this to allow more stops.
 *  This adds (sizeof(lv_color_t) + 1) bytes per additional stop. */
#define LV_GRADIENT_MAX_STOPS   2

/** Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 *  - 0:   round down,
 *  - 64:  round up from x.75,
 *  - 128: round up from half,
 *  - 192: round up from x.25,
 *  - 254: round up */
#define LV_COLOR_MIX_ROUND_OFS  0

/** Add 2 x 32-bit variables to each `lv_obj_t` to speed up getting style properties */
#define LV_OBJ_STYLE_CACHE      0

/** Add `id` field to `lv_obj_t` */
#define LV_USE_OBJ_ID           0

/**  Enable support widget names*/
#define LV_USE_OBJ_NAME         0

/** Automatically assign an ID when obj is created */
#define LV_OBJ_ID_AUTO_ASSIGN   LV_USE_OBJ_ID

/** Use builtin obj ID handler functions:
* - lv_obj_assign_id:       Called when a widget is created. Use a separate counter for each widget class as an ID.
* - lv_obj_id_compare:      Compare the ID to decide if it matches with a requested value.
* - lv_obj_stringify_id:    Return string-ified identifier, e.g. "button3".
* - lv_obj_free_id:         Does nothing, as there is no memory allocation for the ID.
* When disabled these functions needs to be implemented by the user.*/
#define LV_USE_OBJ_ID_BUILTIN   1

/** Use obj property set/get API. */
#define LV_USE_OBJ_PROPERTY 0

/** Enable property name support. */
#define LV_USE_OBJ_PROPERTY_NAME 1

/* Use VG-Lite Simulator.
 * - Requires: LV_USE_THORVG_INTERNAL or LV_USE_THORVG_EXTERNAL */
#define LV_USE_VG_LITE_THORVG  0

#if LV_USE_VG_LITE_THORVG
    /** Enable LVGL's blend mode support */
    #define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

    /** Enable YUV color format support */
    #define LV_VG_LITE_THORVG_YUV_SUPPORT 0

    /** Enable Linear gradient extension support */
    #define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

    /** Enable alignment on 16 pixels */
    #define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1

    /** Buffer address alignment */
    #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64

    /** Enable multi-thread render */
    #define LV_VG_LITE_THORVG_THREAD_RENDER 0
#endif

/* Enable usage of the LVGL's vg_lite spec driver */
#if LV_USE_VG_LITE_DRIVER

    /* Used to pick the correct GPU series folder valid options are gc255, gc355 and gc555*/
    #define LV_VG_LITE_HAL_GPU_SERIES gc255

    /* Used to pick the correct GPU revision header it depends on the vendor */
    #define LV_VG_LITE_HAL_GPU_REVISION 0x40

    /* Base memory address of the GPU IP it depends on SoC, default value is for NXP based devices */
    #define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000

#endif

/* Enable the multi-touch gesture recognition feature */
/* Gesture recognition requires the use of floats */
#define LV_USE_GESTURE_RECOGNITION 0

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/** For big endian systems set to 1 */
#define LV_BIG_ENDIAN_SYSTEM 0

/** Define a custom attribute for `lv_tick_inc` function */
#define LV_ATTRIBUTE_TICK_INC

/** Define a custom attribute for `lv_timer_handler` function */
#define LV_ATTRIBUTE_TIMER_HANDLER

/** Define a custom attribute for `lv_display_flush_ready` function */
#define LV_ATTRIBUTE_FLUSH_READY

/** Align VG_LITE buffers on this number of bytes.
 *  @note  vglite_src_buf_aligned() uses this value to validate alignment of passed buffer pointers. */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

/** Will be added where memory needs to be aligned (with -Os data might not be aligned to boundary by default).
 *  E.g. __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN  __attribute__((aligned(4)))

/** Attribute to mark large constant arrays, for example for font bitmaps */
#define LV_ATTRIBUTE_LARGE_CONST

/** Compiler prefix for a large array declaration in RAM */
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/** Place performance critical functions into a faster memory (e.g RAM) */
#define LV_ATTRIBUTE_FAST_MEM

/** Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *  should also appear on LVGL binding API such as MicroPython. */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning  /**< The default value just prevents GCC warning */

/** Prefix all global extern data with this */
#define LV_ATTRIBUTE_EXTERN_DATA

/** Use `float` as `lv_value_precise_t` */
#define LV_USE_FLOAT            0

/** Enable matrix support
 *  - Requires `LV_USE_FLOAT = 1` */
#define LV_USE_MATRIX           0

/** Include `lvgl_private.h` in `lvgl.h` to access internal data and functions by default */
#ifndef LV_USE_PRIVATE_API
    #define LV_USE_PRIVATE_API  0
#endif

/*==================
 *   FONT USAGE
 *===================*/

/* Montserrat fonts with ASCII range and some symbols using bpp = 4
 * https://fonts.google.com/specimen/Montserrat */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 1
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_28_COMPRESSED    0  /**< bpp = 3 */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW    0  /**< Hebrew, Arabic, Persian letters and all their forms */
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK   0  /**< 1338 most common CJK radicals */
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK   0  /**< 1338 most common CJK radicals */

/** Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/** Optionally declare custom fonts here.
 *
 *  You can use any of these fonts as the default font too and they will be available
 *  globally.  Example:
 *
 *  @code
 *  #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)
 *  @endcode
 */
#define LV_FONT_CUSTOM_DECLARE

/** Always set a default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/** Enable handling large font and/or fonts with a lot of characters.
 *  The limit depends on the font size, font face and bpp.
 *  A compiler error will be triggered if a font needs it. */
#define LV_FONT_FMT_TXT_LARGE 0

/** Enables/disables support for compressed fonts. */
#define LV_USE_FONT_COMPRESSED 0

/** Enable drawing placeholders when glyph dsc is not found. */
#define LV_USE_FONT_PLACEHOLDER 1

/*=================
 *  TEXT SETTINGS
 *=================*/

/**
 * Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding.
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/** While rendering text strings, break (wrap) text on these chars. */
#define LV_TXT_BREAK_CHARS " ,.;:-_)]}"

/** If a word is at least this long, will break wherever "prettiest".
 *  To disable, set to a value <= 0. */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/** Minimum number of characters in a long word to put on a line before a break.
 *  Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/** Minimum number of characters in a long word to put on a line after a break.
 *  Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/** Support bidirectional text. Allows mixing Left-to-Right and Right-to-Left text.
 *  The direction will be processed according to the Unicode Bidirectional Algorithm:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics */
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    /*Set the default direction. Supported values:
    *`LV_BASE_DIR_LTR` Left-to-Right
    *`LV_BASE_DIR_RTL` Right-to-Left
    *`LV_BASE_DIR_AUTO` detect text base direction*/
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/** Enable Arabic/Persian processing
 *  In these languages characters should be replaced with another form based on their position in the text */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*The control character to use for signaling text recoloring*/
#define LV_TXT_COLOR_CMD "#"

/*==================
 * WIDGETS
 *================*/
/* Documentation for widgets can be found here: https://docs.lvgl.io/master/details/widgets/index.html . */

/** 1: Causes these widgets to be given default values at creation time.
 *  - lv_buttonmatrix_t:  Get default maps:  {"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5", ""}, else map not set.
 *  - lv_checkbox_t    :  String label set to "Check box", else set to empty string.
 *  - lv_dropdown_t    :  Options set to "Option 1", "Option 2", "Option 3", else no values are set.
 *  - lv_roller_t      :  Options set to "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", else no values are set.
 *  - lv_label_t       :  Text set to "Text", else empty string.
 *  - lv_arclabel_t   :  Text set to "Arced Text", else empty string.
 * */
#define LV_WIDGETS_HAS_DEFAULT_VALUE  1

#define LV_USE_ANIMIMG    1

#define LV_USE_ARC        1

#define LV_USE_ARCLABEL  1

#define LV_USE_BAR        1

#define LV_USE_BUTTON        1

#define LV_USE_BUTTONMATRIX  1

#define LV_USE_CALENDAR   1
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif

    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW 1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
    #define LV_USE_CALENDAR_CHINESE 0
#endif  /*LV_USE_CALENDAR*/

#define LV_USE_CANVAS     1

#define LV_USE_CHART      1

#define LV_USE_CHECKBOX   1

#define LV_USE_DROPDOWN   1   /**< Requires: lv_label */

#define LV_USE_IMAGE      1   /**< Requires: lv_label */

#define LV_USE_IMAGEBUTTON     1

#define LV_USE_KEYBOARD   1

#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1   /**< Enable selecting text of the label */
    #define LV_LABEL_LONG_TXT_HINT 1    /**< Store some extra info in labels to speed up drawing of very long text */
    #define LV_LABEL_WAIT_CHAR_COUNT 3  /**< The count of wait chart */
#endif

#define LV_USE_LED        1

#define LV_USE_LINE       1

#define LV_USE_LIST       1

#define LV_USE_LOTTIE     0  /**< Requires: lv_canvas, thorvg */

#define LV_USE_MENU       1

#define LV_USE_MSGBOX     1

#define LV_USE_ROLLER     1   /**< Requires: lv_label */

#define LV_USE_SCALE      1

#define LV_USE_SLIDER     1   /**< Requires: lv_bar */

#define LV_USE_SPAN       1
#if LV_USE_SPAN
    /** A line of text can contain this maximum number of span descriptors. */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

#define LV_USE_SPINBOX    1

#define LV_USE_SPINNER    1

#define LV_USE_SWITCH     1

#define LV_USE_TABLE      1

#define LV_USE_TABVIEW    1

#define LV_USE_TEXTAREA   1   /**< Requires: lv_label */
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /**< [ms] */
#endif

#define LV_USE_TILEVIEW   1

#define LV_USE_WIN        1

#define LV_USE_3DTEXTURE  0

/*==================
 * THEMES
 *==================*/
/* Documentation for themes can be found here: https://docs.lvgl.io/master/details/common-widget-features/styles/styles.html#themes . */

/** A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT
    /** 0: Light mode; 1: Dark mode */
    #define LV_THEME_DEFAULT_DARK 0

    /** 1: Enable grow on press */
    #define LV_THEME_DEFAULT_GROW 1

    /** Default transition time in ms. */
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif /*LV_USE_THEME_DEFAULT*/

/** A very simple theme that is a good starting point for a custom theme */
#define LV_USE_THEME_SIMPLE 1

/** A theme designed for monochrome displays */
#define LV_USE_THEME_MONO 1

/*==================
 * LAYOUTS
 *==================*/
/* Documentation for layouts can be found here: https://docs.lvgl.io/master/details/common-widget-features/layouts/index.html . */

/** A layout similar to Flexbox in CSS. */
#define LV_USE_FLEX 1

/** A layout similar to Grid in CSS. */
#define LV_USE_GRID 1

/*====================
 * 3RD PARTS LIBRARIES
 *====================*/
/* Documentation for libraries can be found here: https://docs.lvgl.io/master/details/libs/index.html . */

/* File system interfaces for common APIs */

/** Setting a default driver letter allows skipping the driver prefix in filepaths.
 *  Documentation about how to use the below driver-identifier letters can be found at
 *  https://docs.lvgl.io/master/details/main-modules/fs.html#lv-fs-identifier-letters . */
#define LV_FS_DEFAULT_DRIVER_LETTER '\0'

/** API for fopen, fread, etc. */
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_STDIO_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
    #define LV_FS_STDIO_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
#endif

/** API for open, read, etc. */
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_POSIX_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
    #define LV_FS_POSIX_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
#endif

/** API for CreateFile, ReadFile, etc. */
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_WIN32_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
    #define LV_FS_WIN32_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
#endif

/** API for FATFS (needs to be added separately). Uses f_open, f_read, etc. */
#if LV_USE_FS_FATFS
    #define LV_FS_FATFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_FATFS_PATH ""         /**< Set the working directory. File/directory paths will be appended to it. */
    #define LV_FS_FATFS_CACHE_SIZE 0    /**< >0 to cache this number of bytes in lv_fs_read() */
#endif

/** API for memory-mapped file access. */
#if LV_USE_FS_MEMFS
    #define LV_FS_MEMFS_LETTER '\0'     /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
#endif

/** API for LittleFs. */
#if LV_USE_FS_LITTLEFS
    #define LV_FS_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
#endif

/** API for Arduino LittleFs. */
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
#endif

/** API for Arduino Sd. */
#if LV_USE_FS_ARDUINO_SD
    #define LV_FS_ARDUINO_SD_LETTER '\0'  /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
    #define LV_FS_ARDUINO_SD_PATH ""      /**< Set the working directory. File/directory paths will be appended to it. */
#endif

/** API for UEFI */
#define LV_USE_FS_UEFI 0
#if LV_USE_FS_UEFI
    #define LV_FS_UEFI_LETTER '\0'      /**< Set an upper-case driver-identifier letter for this driver (e.g. 'A'). */
#endif

#if LV_USE_FS_FROGFS
    #define LV_FS_FROGFS_LETTER '\0'
#endif

/** GIF decoder library */
#if LV_USE_GIF
    /** GIF decoder accelerate */
    #define LV_GIF_CACHE_DECODE_DATA 0
#endif

/** Decode bin images to RAM */
#define LV_BIN_DECODER_RAM_LOAD 0

/** FreeType library */
#if LV_USE_FREETYPE
    /** Let FreeType use LVGL memory and file porting */
    #define LV_FREETYPE_USE_LVGL_PORT 0

    /** Cache count of glyphs in FreeType, i.e. number of glyphs that can be cached.
     *  The higher the value, the more memory will be used. */
    #define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256
#endif

/** Built-in TTF decoder */
#if LV_USE_TINY_TTF
    /* Enable loading TTF data from files */
    #define LV_TINY_TTF_FILE_SUPPORT 0
    #define LV_TINY_TTF_CACHE_GLYPH_CNT 128
    #define LV_TINY_TTF_CACHE_KERNING_CNT 256
#endif

/** Enable Vector Graphic APIs
 *  - Requires `LV_USE_MATRIX = 1` */
/*Enable Vector Graphic APIs*/
#ifndef LV_USE_VECTOR_GRAPHIC
#   define LV_USE_VECTOR_GRAPHIC  0

/* Enable ThorVG (vector graphics library) from the src/libs folder */
#   define LV_USE_THORVG_INTERNAL 0

/* Enable ThorVG by assuming that its installed and linked to the project */
#   define LV_USE_THORVG_EXTERNAL 0
#endif

/** Use lvgl built-in LZ4 lib */
#define LV_USE_LZ4_INTERNAL  0

/** Use external LZ4 library */
#define LV_USE_LZ4_EXTERNAL  0

/*SVG library
 *  - Requires `LV_USE_VECTOR_GRAPHIC = 1` */
#define LV_USE_SVG_ANIMATION 0
#define LV_USE_SVG_DEBUG 0

/** FFmpeg library for image decoding and playing videos.
 *  Supports all major image formats so do not enable other image decoder with it. */
#if LV_USE_FFMPEG
    /** Dump input information to stderr */
    #define LV_FFMPEG_DUMP_FORMAT 0
    /** Use lvgl file path in FFmpeg Player widget
     *  You won't be able to open URLs after enabling this feature.
     *  Note that FFmpeg image decoder will always use lvgl file system. */
    #define LV_FFMPEG_PLAYER_USE_LV_FS 0
#endif

/*==================
 * OTHERS
 *==================*/
/* Documentation for several of the below items can be found here: https://docs.lvgl.io/master/details/auxiliary-modules/index.html . */

/** 1: Enable system monitor component */
#define LV_USE_SYSMON   0
#if LV_USE_SYSMON
    /** Get the idle percentage. E.g. uint32_t my_get_idle(void); */
    #define LV_SYSMON_GET_IDLE lv_os_get_idle_percent
    /** 1: Enable usage of lv_os_get_proc_idle_percent.*/
    #define LV_SYSMON_PROC_IDLE_AVAILABLE 0
    #if LV_SYSMON_PROC_IDLE_AVAILABLE
        /** Get the applications idle percentage.
         * - Requires `LV_USE_OS == LV_OS_PTHREAD` */
        #define LV_SYSMON_GET_PROC_IDLE lv_os_get_proc_idle_percent
    #endif 

    /** 1: Show CPU usage and FPS count.
     *  - Requires `LV_USE_SYSMON = 1` */
    #define LV_USE_PERF_MONITOR 0
    #if LV_USE_PERF_MONITOR
        #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT

        /** 0: Displays performance data on the screen; 1: Prints performance data using log. */
        #define LV_USE_PERF_MONITOR_LOG_MODE 0
    #endif

    /** 1: Show used memory and memory fragmentation.
     *     - Requires `LV_USE_STDLIB_MALLOC = LV_STDLIB_BUILTIN`
     *     - Requires `LV_USE_SYSMON = 1`*/
    #define LV_USE_MEM_MONITOR 0
    #if LV_USE_MEM_MONITOR
        #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
    #endif
#endif /*LV_USE_SYSMON*/

/** 1: Enable runtime performance profiler */
#define LV_USE_PROFILER 0
#if LV_USE_PROFILER
    /** 1: Enable the built-in profiler */
    #define LV_USE_PROFILER_BUILTIN 1
    #if LV_USE_PROFILER_BUILTIN
        /** Default profiler trace buffer size */
        #define LV_PROFILER_BUILTIN_BUF_SIZE (16 * 1024)     /**< [bytes] */
        #define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1
        #define LV_USE_PROFILER_BUILTIN_POSIX 0 /**< Enable POSIX profiler port */
    #endif

    /** Header to include for profiler */
    #define LV_PROFILER_INCLUDE "src/misc/lv_profiler_builtin.h"

    /** Profiler start point function */
    #define LV_PROFILER_BEGIN    LV_PROFILER_BUILTIN_BEGIN

    /** Profiler end point function */
    #define LV_PROFILER_END      LV_PROFILER_BUILTIN_END

    /** Profiler start point function with custom tag */
    #define LV_PROFILER_BEGIN_TAG LV_PROFILER_BUILTIN_BEGIN_TAG

    /** Profiler end point function with custom tag */
    #define LV_PROFILER_END_TAG   LV_PROFILER_BUILTIN_END_TAG

    /*Enable layout profiler*/
    #define LV_PROFILER_LAYOUT 1

    /*Enable disp refr profiler*/
    #define LV_PROFILER_REFR 1

    /*Enable draw profiler*/
    #define LV_PROFILER_DRAW 1

    /*Enable indev profiler*/
    #define LV_PROFILER_INDEV 1

    /*Enable decoder profiler*/
    #define LV_PROFILER_DECODER 1

    /*Enable font profiler*/
    #define LV_PROFILER_FONT 1

    /*Enable fs profiler*/
    #define LV_PROFILER_FS 1

    /*Enable style profiler*/
    #define LV_PROFILER_STYLE 0

    /*Enable timer profiler*/
    #define LV_PROFILER_TIMER 1

    /*Enable cache profiler*/
    #define LV_PROFILER_CACHE 1

    /*Enable event profiler*/
    #define LV_PROFILER_EVENT 1
#endif

/** 1: Enable an observer pattern implementation */
#define LV_USE_OBSERVER 1

/** 1: Enable Pinyin input method
 *  - Requires: lv_keyboard */
#if LV_USE_IME_PINYIN
    /** 1: Use default thesaurus.
     *  @note  If you do not use the default thesaurus, be sure to use `lv_ime_pinyin` after setting the thesaurus. */
    #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
    /** Set maximum number of candidate panels that can be displayed.
     *  @note  This needs to be adjusted according to size of screen. */
    #define LV_IME_PINYIN_CAND_TEXT_NUM 6

    /** Use 9-key input (k9). */
    #define LV_IME_PINYIN_USE_K9_MODE      1
    #if LV_IME_PINYIN_USE_K9_MODE == 1
        #define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3
    #endif /*LV_IME_PINYIN_USE_K9_MODE*/
#endif

/** 1: Enable file explorer.
 *  - Requires: lv_table */
#if LV_USE_FILE_EXPLORER
    /** Maximum length of path */
    #define LV_FILE_EXPLORER_PATH_MAX_LEN        (128)
    /** Quick access bar, 1:use, 0:do not use.
     *  - Requires: lv_list */
    #define LV_FILE_EXPLORER_QUICK_ACCESS        1
#endif

/** 1: Enable Font manager */
#if LV_USE_FONT_MANAGER

/**Font manager name max length*/
#define LV_FONT_MANAGER_NAME_MAX_LEN            32

#endif

/** Enable emulated input devices, time emulation, and screenshot compares. */
#define LV_USE_TEST 0
#if LV_USE_TEST

/** Enable `lv_test_screenshot_compare`.
 * Requires libpng and a few MB of extra RAM. */
#define LV_USE_TEST_SCREENSHOT_COMPARE 0
#endif /*LV_USE_TEST*/

/*1: Enable color filter style*/
#define LV_USE_COLOR_FILTER     0

/*==================
 * DEVICES
 *==================*/

/** Use SDL to open window on PC and handle mouse and keyboard. */
#define LV_USE_SDL              0
#if LV_USE_SDL
    #define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>
    #define LV_SDL_RENDER_MODE      LV_DISPLAY_RENDER_MODE_DIRECT   /**< LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance */
    #define LV_SDL_BUF_COUNT        1    /**< 1 or 2 */
    #define LV_SDL_ACCELERATED      1    /**< 1: Use hardware acceleration*/
    #define LV_SDL_FULLSCREEN       0    /**< 1: Make the window full screen by default */
    #define LV_SDL_DIRECT_EXIT      1    /**< 1: Exit the application when all SDL windows are closed */
    #define LV_SDL_MOUSEWHEEL_MODE  LV_SDL_MOUSEWHEEL_MODE_ENCODER  /*LV_SDL_MOUSEWHEEL_MODE_ENCODER/CROWN*/
#endif

/** Use X11 to open window on Linux desktop and handle mouse and keyboard */
#define LV_USE_X11              0
#if LV_USE_X11
    #define LV_X11_DIRECT_EXIT         1  /**< Exit application when all X11 windows have been closed */
    #define LV_X11_DOUBLE_BUFFER       1  /**< Use double buffers for rendering */
    /* Select only 1 of the following render modes (LV_X11_RENDER_MODE_PARTIAL preferred!). */
    #define LV_X11_RENDER_MODE_PARTIAL 1  /**< Partial render mode (preferred) */
    #define LV_X11_RENDER_MODE_DIRECT  0  /**< Direct render mode */
    #define LV_X11_RENDER_MODE_FULL    0  /**< Full render mode */
#endif

/** Use Wayland to open a window and handle input on Linux or BSD desktops */
#define LV_USE_WAYLAND          0
#if LV_USE_WAYLAND
    #define LV_WAYLAND_BUF_COUNT            1    /**< Use 1 for single buffer with partial render mode or 2 for double buffer with full render mode*/
    #define LV_WAYLAND_USE_DMABUF           0    /**< Use DMA buffers for frame buffers. Requires LV_USE_DRAW_G2D */
    #define LV_WAYLAND_RENDER_MODE          LV_DISPLAY_RENDER_MODE_PARTIAL   /**< DMABUF supports LV_DISPLAY_RENDER_MODE_FULL and LV_DISPLAY_RENDER_MODE_DIRECT*/
                                                                             /**< When LV_WAYLAND_USE_DMABUF is disabled, only LV_DISPLAY_RENDER_MODE_PARTIAL is supported*/
    #define LV_WAYLAND_WINDOW_DECORATIONS   0    /**< Draw client side window decorations only necessary on Mutter/GNOME. Not supported using DMABUF*/
#endif

/** Driver for /dev/fb */
#if LV_USE_LINUX_FBDEV
    #define LV_LINUX_FBDEV_BSD           0
    #define LV_LINUX_FBDEV_RENDER_MODE   LV_DISPLAY_RENDER_MODE_PARTIAL
    #define LV_LINUX_FBDEV_BUFFER_COUNT  0
    #define LV_LINUX_FBDEV_BUFFER_SIZE   60
    #define LV_LINUX_FBDEV_MMAP          1
#endif

/** Use Nuttx to open window and handle touchscreen */
#define LV_USE_NUTTX    0

#if LV_USE_NUTTX
    #define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0

    /** Use independent image heap for default draw buffer */
    #define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP    0

    #define LV_USE_NUTTX_LIBUV    0

    /** Use Nuttx custom init API to open window and handle touchscreen */
    #define LV_USE_NUTTX_CUSTOM_INIT    0

    /** Driver for /dev/lcd */
    #define LV_USE_NUTTX_LCD      0
    #if LV_USE_NUTTX_LCD
        #define LV_NUTTX_LCD_BUFFER_COUNT    0
        #define LV_NUTTX_LCD_BUFFER_SIZE     60
    #endif

    /** Driver for /dev/input */
    #define LV_USE_NUTTX_TOUCHSCREEN    0

    /** Touchscreen cursor size in pixels(<=0: disable cursor) */
    #define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE    0

    /** Driver for /dev/mouse */
    #define LV_USE_NUTTX_MOUSE    0

    /** Mouse movement step (pixels) */
    #define LV_USE_NUTTX_MOUSE_MOVE_STEP    1

    /*NuttX trace file and its path*/
    #define LV_USE_NUTTX_TRACE_FILE 0
    #if LV_USE_NUTTX_TRACE_FILE
        #define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"
    #endif

#endif

/** Driver for /dev/dri/card */
#if LV_USE_LINUX_DRM

    /* Use the MESA GBM library to allocate DMA buffers that can be
     * shared across sub-systems and libraries using the Linux DMA-BUF API.
     * The GBM library aims to provide a platform independent memory management system
     * it supports the major GPU vendors - This option requires linking with libgbm */
    #define LV_USE_LINUX_DRM_GBM_BUFFERS 0

    #define LV_LINUX_DRM_USE_EGL     0
#endif

/** Interface for Lovyan_GFX */
#if LV_USE_LOVYAN_GFX
    #define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"

#endif /*LV_USE_LOVYAN_GFX*/

/** Driver for evdev input devices */
#define LV_USE_EVDEV    0

/** Driver for libinput input devices */
#define LV_USE_LIBINPUT    0

#if LV_USE_LIBINPUT
    #define LV_LIBINPUT_BSD    0

    /** Full keyboard support */
    #define LV_LIBINPUT_XKB             0
    #if LV_LIBINPUT_XKB
        /** "setxkbmap -query" can help find the right values for your keyboard */
        #define LV_LIBINPUT_XKB_KEY_MAP { .rules = NULL, .model = "pc101", .layout = "us", .variant = NULL, .options = NULL }
    #endif
#endif

/* Drivers for LCD devices connected via SPI/parallel port */
#if (LV_USE_ST7735 | LV_USE_ST7789 | LV_USE_ST7796 | LV_USE_ILI9341 | LV_USE_NV3007)
    #define LV_USE_GENERIC_MIPI 1
#else
    #define LV_USE_GENERIC_MIPI 0
#endif

/** Driver for ST LTDC */
#if LV_USE_ST_LTDC
    /* Only used for partial. */
    #define LV_ST_LTDC_USE_DMA2D_FLUSH 0
#endif

/** LVGL UEFI backend */
#define LV_USE_UEFI 0
#if LV_USE_UEFI
    #define LV_USE_UEFI_INCLUDE "myefi.h"   /**< Header that hides the actual framework (EDK2, gnu-efi, ...) */
    #define LV_UEFI_USE_MEMORY_SERVICES 0   /**< Use the memory functions from the boot services table */
#endif

/** Use a generic OpenGL driver that can be used to embed in other applications or used with GLFW/EGL */
#define LV_USE_OPENGLES   0
#if LV_USE_OPENGLES
    #define LV_USE_OPENGLES_DEBUG        1    /**< Enable or disable debug for opengles */
#endif

/** Use GLFW to open window on PC and handle mouse and keyboard. Requires*/
#define LV_USE_GLFW   0


/** QNX Screen display and input drivers */
#define LV_USE_QNX              0
#if LV_USE_QNX
    #define LV_QNX_BUF_COUNT        1    /**< 1 or 2 */
#endif

/*=====================
* BUILD OPTIONS
*======================*/

/** Enable examples to be built with the library. */
#define LV_BUILD_EXAMPLES 1

/** Build the demos */
#define LV_BUILD_DEMOS 1

/*===================
 * DEMO USAGE
 ====================*/

#if LV_BUILD_DEMOS

    #if LV_USE_DEMO_BENCHMARK
        /** Use fonts where bitmaps are aligned 16 byte and has Nx16 byte stride */
        #define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0
    #endif

    /** Music player demo */
    #if LV_USE_DEMO_MUSIC
        #define LV_DEMO_MUSIC_SQUARE    0
        #define LV_DEMO_MUSIC_LANDSCAPE 0
        #define LV_DEMO_MUSIC_ROUND     0
        #define LV_DEMO_MUSIC_LARGE     0
        #define LV_DEMO_MUSIC_AUTO_PLAY 0
    #endif

#endif /* LV_BUILD_DEMOS */

/*--END OF LV_CONF_CMSIS_H--*/

#endif /*LV_CONF_CMSIS_H*/

#endif /*End of "Content enable"*/

/**
 * @file lv_conf.h
 * Configuration file for v9.6.0
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to `lvgl` folder
 * 2. or to any other place and
 *    - define `LV_CONF_INCLUDE_SIMPLE`;
 *    - add the path as an include path.
 */

/* clang-format off */
#if 0 /* Set this to "1" to enable content */

#ifndef LV_CONF_H
#define LV_CONF_H

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
 *  - LV_STDLIB_RTTHREAD: RT-Thread functions malloc/realloc/free
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN

/** String functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL's built in implementation
 *  - LV_STDLIB_CLIB: Standard C functions memcpy/memset/strlen/strcpy
 *  - LV_STDLIB_RTTHREAD: RT-Thread functions rt_memcpy/rt_memset/rt_strlen/rt_strcpy
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#define LV_USE_STDLIB_STRING LV_STDLIB_BUILTIN

/** Sprintf functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL's built in implementation
 *  - LV_STDLIB_CLIB: Standard C function vsnprintf
 *  - LV_STDLIB_RTTHREAD: RT-Thread function rt_vsnprintf
 *  - LV_STDLIB_CUSTOM: Implement the functions externally
 */
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_BUILTIN

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
/** Memory size in bytes (Needs to be at least 2kB (2048)) */
#define LV_MEM_SIZE 65536

/** Address for the memory pool instead of allocating it as a normal array. 0: unused */
#define LV_MEM_ADR 0x0

#endif /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

/** Header for integer types (stdint) */
#define LV_STDINT_INCLUDE "stdint.h"

/** Header for standard definitions (stddef) */
#define LV_STDDEF_INCLUDE "stddef.h"

/** Header for boolean types (stdbool) */
#define LV_STDBOOL_INCLUDE "stdbool.h"

/** Header for fixed-width integer format macros (inttypes) */
#define LV_INTTYPES_INCLUDE "inttypes.h"

/** Header for implementation limits (limits) */
#define LV_LIMITS_INCLUDE "limits.h"

/** Header for variadic argument handling (stdarg) */
#define LV_STDARG_INCLUDE "stdarg.h"



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
#define LV_USE_OS LV_OS_NONE

#if LV_USE_OS == LV_OS_CUSTOM
/** Custom OS include header */
#define LV_OS_CUSTOM_INCLUDE ""

#endif /*LV_USE_OS == LV_OS_CUSTOM*/

#if LV_USE_OS == LV_OS_FREERTOS
/** Unblocking an RTOS task with a direct notification is 45% faster and uses less RAM
 *  than unblocking a task using an intermediary object such as a binary semaphore.
 *  RTOS task notifications can only be used when there is only one task that can be the recipient of the event.
 */
#define LV_USE_FREERTOS_TASK_NOTIFY 1

/** Enable this to provide a custom implementation of lv_os_get_idle_percent.
 *  This is useful for multi-core systems where the default
 *  FreeRTOS implementation might not sufficiently track idle time across all cores.
 */
#define LV_OS_IDLE_PERCENT_CUSTOM 0

#endif /*LV_USE_OS == LV_OS_FREERTOS*/


/*============================================================================
 * RENDERING CONFIGURATION
 *============================================================================*/

/** Color depth: 1 (I1), 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#define LV_COLOR_DEPTH 16

/** 0: no adjustment, get the integer part of the result (round down)
 *  64: round up from x.75
 *  128: round up from half
 *  192: round up from x.25
 *  254: round up
 */
#define LV_COLOR_MIX_ROUND_OFS 0

/** Default display refresh, input device read and animation step period. */
#define LV_DEF_REFR_PERIOD 33

/** Used to initialize default sizes such as widgets sizes and style paddings.
 *  (Not so important, you can adjust it to modify default sizes and spaces)
 */
#define LV_DPI_DEF 130

/** Align the stride of all layers and images to this many bytes. */
#define LV_DRAW_BUF_STRIDE_ALIGN 1

/** Align the start address of draw_buf addresses to this many bytes. */
#define LV_DRAW_BUF_ALIGN 4

/** Enable matrix support
 *
 *  Enable: LV_USE_FLOAT
 */
#define LV_USE_MATRIX 0

#if LV_USE_MATRIX
/** Requirements: The rendering engine needs to support 3x3 matrix transformations. */
#define LV_DRAW_TRANSFORM_USE_MATRIX 0

#endif /*LV_USE_MATRIX*/

/** If a widget has `style_opa < 255` (not `bg_opa`, `text_opa` etc) or not NORMAL blend mode
 *  it is buffered into a "simple" layer before rendering. The widget can be buffered in smaller chunks.
 *  "Transformed layers" (if `transform_angle/zoom` are set) use larger buffers and can't be drawn in chunks.
 */
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE 24576

/** Limit the max allocated memory for simple and transformed layers.
 *  It should be at least `LV_DRAW_LAYER_SIMPLE_BUF_SIZE` sized but if transformed layers are also used
 *  it should be enough to store the largest widget too (width x height x 4 area).
 *  Set it to 0 to have no limit.
 */
#define LV_DRAW_LAYER_MAX_MEMORY 0

#if LV_USE_OS != LV_OS_NONE
/** If FreeType or ThorVG is enabled, it is recommended to set it to 32KB or more. */
#define LV_DRAW_THREAD_STACK_SIZE 8192

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
#define LV_DRAW_THREAD_PRIO 3

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable drawing support vector graphic APIs.
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_VECTOR_GRAPHIC 0

/** Enable API to take snapshot for object */
#define LV_USE_SNAPSHOT 0

/** ThorVG library for vector graphics support */
#define LV_USE_THORVG 0

#if LV_USE_THORVG
/** Internal ThorVG library bundled with LVGL */
#define LV_USE_THORVG_INTERNAL 1

#endif /*LV_USE_THORVG*/

/** Required to draw anything on the screen. */
#define LV_USE_DRAW_SW 1

#if LV_USE_DRAW_SW
/** Enable support for RGB565 color format */
#define LV_DRAW_SW_SUPPORT_RGB565 1

/** Enable support for RGB565 swapped color format */
#define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 1

/** Enable support for RGB565A8 color format */
#define LV_DRAW_SW_SUPPORT_RGB565A8 1

/** Enable support for RGB888 color format */
#define LV_DRAW_SW_SUPPORT_RGB888 1

/** Enable support for XRGB8888 color format */
#define LV_DRAW_SW_SUPPORT_XRGB8888 1

/** Enable support for ARGB8888 color format */
#define LV_DRAW_SW_SUPPORT_ARGB8888 1

/** Enable support for ARGB8888 premultiplied color format */
#define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1

/** Enable support for L8 color format */
#define LV_DRAW_SW_SUPPORT_L8 1

/** Enable support for AL88 color format */
#define LV_DRAW_SW_SUPPORT_AL88 1

/** Enable support for A8 color format */
#define LV_DRAW_SW_SUPPORT_A8 1

/** Enable support for I1 color format */
#define LV_DRAW_SW_SUPPORT_I1 1

#if LV_DRAW_SW_SUPPORT_I1
/** Luminance threshold for a pixel to be active */
#define LV_DRAW_SW_I1_LUM_THRESHOLD 127

#endif /*LV_DRAW_SW_SUPPORT_I1*/

#if LV_USE_OS != LV_OS_NONE
/** Number of threads used to render a frame in parallel */
#define LV_DRAW_SW_DRAW_UNIT_CNT 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Must deploy arm-2d library to your project and add include PATH for "arm_2d.h". */
#define LV_USE_DRAW_ARM2D_SYNC 0

/** Disabling this allows arm2d to work on its own (for testing only) */
#define LV_USE_NATIVE_HELIUM_ASM 0

/** 0: use a simple renderer capable of drawing only simple rectangles with gradient, images, texts, and straight lines only,
 *  1: use a complex renderer capable of drawing rounded corners, shadow, skew lines, and arcs too.
 */
#define LV_DRAW_SW_COMPLEX 1

/** Increase this to allow more stops.
 *  This adds (sizeof(lv_color_t) + 1) bytes per additional stop
 */
#define LV_GRADIENT_MAX_STOPS 2

/** 0: do not enable complex gradients
 *  1: enable complex gradients (linear at an angle, radial or conical)
 */
#define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 0

#if LV_DRAW_SW_COMPLEX
/** LV_DRAW_SW_SHADOW_CACHE_SIZE is the max shadow size to buffer, where
 *  shadow size is `shadow_width + radius`.
 *  Caching has LV_DRAW_SW_SHADOW_CACHE_SIZE^2 RAM cost.
 */
#define LV_DRAW_SW_SHADOW_CACHE_SIZE 0

/** The circumference of 1/4 circle are saved for anti-aliasing
 *  radius * 4 bytes are used per circle (the most often used
 *  radiuses are saved).
 *  Set to 0 to disable caching.
 */
#define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4

#endif /*LV_DRAW_SW_COMPLEX*/

/** ASM mode to be used
 *  Possible values:
 *  - LV_DRAW_SW_ASM_NONE
 *  - LV_DRAW_SW_ASM_NEON
 *  - LV_DRAW_SW_ASM_HELIUM
 *  - LV_DRAW_SW_ASM_RISCV_V: RISC-V Vector
 *  - LV_DRAW_SW_ASM_CUSTOM
 */
#define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
/** Set the custom asm include file */
#define LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM*/
#endif /*LV_USE_DRAW_SW*/

/** Use VG-Lite GPU.
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_DRAW_VG_LITE 0

#if LV_USE_DRAW_VG_LITE
/** Enable VG-Lite custom external 'gpu_init()' function */
#define LV_VG_LITE_USE_GPU_INIT 0

/** Enable VG-Lite assert */
#define LV_VG_LITE_USE_ASSERT 0

/** GPU will try to batch these many draw tasks */
#define LV_VG_LITE_FLUSH_MAX_COUNT 8

/** which usually improves performance,
 *  but does not guarantee the same rendering quality as the software.
 */
#define LV_VG_LITE_USE_BOX_SHADOW 1

/** The memory usage of a single gradient:
 *          linear: 4K bytes.
 *          radial: radius * 4K bytes.
 */
#define LV_VG_LITE_GRAD_CACHE_CNT 32

/** VG-Lite stroke maximum cache number */
#define LV_VG_LITE_STROKE_CACHE_CNT 32

/** VG-Lite unaligned bitmap font maximum cache number */
#define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256

/** Remove VLC_OP_CLOSE path instruction (Workaround for NXP) */
#define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0

/** Disable linear gradient extension for some older versions of drivers */
#define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0

/** Disable blit rectangular offset to resolve certain hardware errors */
#define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0

/** Maximum path dump print length (in points) */
#define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000

/** Use LVGL's built-in vg_lite driver */
#define LV_USE_VG_LITE_DRIVER 0

#if LV_USE_VG_LITE_DRIVER
/** VG-Lite GPU (series and revision)
 *  Possible values:
 *  - LV_VG_LITE_GPU_GC255_0X40A: GC255 (revision 0x40A)
 *  - LV_VG_LITE_GPU_GC355_0X0_1215: GC355 (revision 0x0_1215)
 *  - LV_VG_LITE_GPU_GC355_0X0_1216: GC355 (revision 0x0_1216)
 *  - LV_VG_LITE_GPU_GC555_0X423: GC555 (revision 0x423)
 *  - LV_VG_LITE_GPU_GC555_0X423_ECO: GC555 (revision 0x423 ECO)
 *  - LV_VG_LITE_GPU_GCNANOULTRAV_0X1003: GCNanoUltraV (revision 0x1003)
 */
#define LV_VG_LITE_GPU LV_VG_LITE_GPU_GC255_0X40A

/** VG-Lite GPU base address */
#define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000

#endif /*LV_USE_VG_LITE_DRIVER*/

/** Use thorvg to simulate VG-Lite hardware behavior, it's useful
 *  for debugging and testing on PC simulator. Enable LV_USE_THORVG,
 *  Either internal ThorVG or external ThorVG library is required.
 *
 *  Enable: LV_USE_THORVG
 */
#define LV_USE_VG_LITE_THORVG 0

#if LV_USE_VG_LITE_THORVG
/** Enable LVGL blend mode support */
#define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

/** Enable YUV color format support */
#define LV_VG_LITE_THORVG_YUV_SUPPORT 0

/** Enable linear gradient extension support */
#define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

/** Enable 16 pixels alignment */
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1

/** Buffer address alignment */
#define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64

/** Enable multi-thread render */
#define LV_VG_LITE_THORVG_THREAD_RENDER 0

#endif /*LV_USE_VG_LITE_THORVG*/
#endif /*LV_USE_DRAW_VG_LITE*/

/** Use Renesas Dave2D on RA  platforms. */
#define LV_USE_DRAW_DAVE2D 0

/** Use TSi's aka (Think Silicon) NemaGFX */
#define LV_USE_NEMA_GFX 0

#if LV_USE_NEMA_GFX
/** Cache handling of NemaGFX */
#define LV_NEMA_USE_CACHE 0

#if LV_NEMA_USE_CACHE
/** NemaGFX Cache HAL include */
#define LV_NEMA_CACHE_HAL_INCLUDE "stm32u5xx_hal.h"

#endif /*LV_NEMA_USE_CACHE*/

/** NemaGFX static library
 *  Possible values:
 *  - LV_NEMA_LIB_NONE
 *  - LV_NEMA_LIB_M33_REVC
 *  - LV_NEMA_LIB_M33_NEMAPVG
 *  - LV_NEMA_LIB_M55
 *  - LV_NEMA_LIB_M7
 */
#define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE

/** NemaGFX HAL
 *  Possible values:
 *  - LV_NEMA_HAL_CUSTOM
 *  - LV_NEMA_HAL_STM32
 */
#define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM

#if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
/** NemaGFX STM32 HAL include */
#define LV_NEMA_STM32_HAL_INCLUDE "stm32u5xx_hal.h"

/** Include the header given by LV_NEMA_CUSTOM_INCLUDE to override
 *  NemaGFX/STM32 HAL-specific macros such as
 *  LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM.
 */
#define LV_NEMA_USE_CUSTOM_INCLUDE 0

#endif /*LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32*/

#if LV_NEMA_USE_CUSTOM_INCLUDE
/** Optional header to override NemaGFX/STM32 HAL-specific macros.
 *
 *  LV_NEMA_STM32_HAL_ATTRIBUTE_POOL_MEM
 *    Attribute applied to the NemaGFX memory pool on STM32 targets.
 *    Used to place the pool in a specific RAM region accessible by
 *    the NemaGFX DMA.
 *    e.g. __attribute__((section(".nema_pool")))
 */
#define LV_NEMA_CUSTOM_INCLUDE ""

#endif /*LV_NEMA_USE_CUSTOM_INCLUDE*/

/** Enable NemaVG operations */
#define LV_USE_NEMA_VG 0

/** NemaVG max horizontal resolution */
#define LV_NEMA_GFX_MAX_RESX 800

/** NemaVG max vertical resolution */
#define LV_NEMA_GFX_MAX_RESY 600

#endif /*LV_USE_NEMA_GFX*/

/** Use PXP for drawing */
#define LV_USE_DRAW_PXP 0

#if LV_USE_DRAW_PXP
/** Use PXP to rotate the display */
#define LV_USE_ROTATE_PXP 0

#if LV_USE_OS != LV_OS_NONE
/** Use additional draw thread for PXP processing */
#define LV_USE_PXP_DRAW_THREAD 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable PXP asserts */
#define LV_USE_PXP_ASSERT 0

#endif /*LV_USE_DRAW_PXP*/

/** Draw using Espressif PPA accelerator */
#define LV_USE_PPA 0

#if LV_USE_PPA
/** Use Espressif's PPA accelerator for Image draw */
#define LV_USE_PPA_IMG 0

/** PPA burst length size in bytes. */
#define LV_PPA_BURST_LENGTH 128

#endif /*LV_USE_PPA*/

/** Accelerate blends, fills, image decoding, etc. with STM32 DMA2D. */
#define LV_USE_DRAW_DMA2D 0

#if LV_USE_DRAW_DMA2D
/** the header file for LVGL to include for DMA2D */
#define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"

/** if enabled, the user is required to call
 *  `lv_draw_dma2d_transfer_complete_interrupt_handler`
 *  upon receiving the DMA2D global interrupt
 */
#define LV_USE_DRAW_DMA2D_INTERRUPT 0

#endif /*LV_USE_DRAW_DMA2D*/

/** Use EVE FT81X GPU. */
#define LV_USE_DRAW_EVE 0

#if LV_USE_DRAW_EVE
/** EVE_GEN value */
#define LV_DRAW_EVE_EVE_GENERATION 4

/** Max bytes to buffer for each SPI transmission or 0 to disable buffering */
#define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048

#endif /*LV_USE_DRAW_EVE*/

/** Use G2D for drawing */
#define LV_USE_DRAW_G2D 0

#if LV_USE_DRAW_G2D
/** Includes the frame buffers and assets. */
#define LV_G2D_HASH_TABLE_SIZE 50

#if LV_USE_OS != LV_OS_NONE
/** Use additional draw thread for G2D processing */
#define LV_USE_G2D_DRAW_THREAD 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Enable G2D asserts */
#define LV_USE_G2D_ASSERT 0

#endif /*LV_USE_DRAW_G2D*/

/** Use NanoVG Renderer
 *
 *  Enable: LV_USE_NANOVG, LV_USE_MATRIX
 */
#define LV_USE_DRAW_NANOVG 0

#if LV_USE_DRAW_NANOVG
/** Select which OpenGL implementation to use for NanoVG rendering.
 *  Possible values:
 *  - LV_NANOVG_BACKEND_GL2: OpenGL 2.0
 *  - LV_NANOVG_BACKEND_GL3: OpenGL 3.0+
 *  - LV_NANOVG_BACKEND_GLES2: OpenGL ES 2.0
 *  - LV_NANOVG_BACKEND_GLES3: OpenGL ES 3.0+
 */
#define LV_NANOVG_BACKEND LV_NANOVG_BACKEND_GLES2

/** Draw image texture cache count */
#define LV_NANOVG_IMAGE_CACHE_CNT 128

/** Draw letter cache count */
#define LV_NANOVG_LETTER_CACHE_CNT 512

#endif /*LV_USE_DRAW_NANOVG*/

/** Draw using cached OpenGLES textures.
 *
 *  Enable: LV_USE_OPENGLES
 */
#define LV_USE_DRAW_OPENGLES 0

#if LV_USE_DRAW_OPENGLES
/** OpenGLES texture cache count */
#define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64

#endif /*LV_USE_DRAW_OPENGLES*/

/** Uses SDL renderer API */
#define LV_USE_DRAW_SDL 0



/*============================================================================
 * INPUT DEVICES
 *============================================================================*/

/** Enable grid navigation */
#define LV_USE_GRIDNAV 0

/** Enable the multi-touch gesture recognition feature
 *  Gesture recognition requires the use of floats
 *
 *  Enable: LV_USE_FLOAT
 */
#define LV_USE_GESTURE_RECOGNITION 0



/*============================================================================
 * CORE
 *============================================================================*/

/** Add 2 x 32 bit variables to each lv_obj_t to speed up getting style properties */
#define LV_OBJ_STYLE_CACHE 0

/** Enable support for widget names */
#define LV_USE_OBJ_NAME 0

/** Add `id` field to `lv_obj_t` */
#define LV_USE_OBJ_ID 0

#if LV_USE_OBJ_ID
/** Automatically assign an ID when obj is created */
#define LV_OBJ_ID_AUTO_ASSIGN 1

/** Use builtin obj ID handler functions:
 *  - lv_obj_assign_id:       Called when a widget is created. Use a separate counter for each widget class as an ID.
 *  - lv_obj_id_compare:      Compare the ID to decide if it matches with a requested value.
 *  - lv_obj_stringify_id:    Return string-ified identifier, e.g. "button3".
 *  - lv_obj_free_id:         Does nothing, as there is no memory allocation for the ID.
 *  When disabled these functions needs to be implemented by the user.
 */
#define LV_USE_OBJ_ID_BUILTIN 1

#endif /*LV_USE_OBJ_ID*/

/** Use obj property set/get API. */
#define LV_USE_OBJ_PROPERTY 0

#if LV_USE_OBJ_PROPERTY
/** Add a name table to every widget class, so the property can be accessed by name.
 *  Note, the const table will increase flash usage.
 */
#define LV_USE_OBJ_PROPERTY_NAME 1

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
#define LV_USE_EXT_DATA 0

/** Use `float` as `lv_value_precise_t` */
#define LV_USE_FLOAT 0

/** Enable an observer pattern implementation */
#define LV_USE_OBSERVER 1

/** Enable text translation support */
#define LV_USE_TRANSLATION 0

/** Enable color filter style */
#define LV_USE_COLOR_FILTER 0



/*============================================================================
 * LOGGING
 *============================================================================*/

/** Enable log module */
#define LV_USE_LOG 0

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
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/** Use printf for log output.
 *  If not set the user needs to register a callback with `lv_log_register_print_cb`.
 */
#define LV_LOG_PRINTF 0

/** Enable print timestamp */
#define LV_LOG_USE_TIMESTAMP 1

/** Enable print file and line number */
#define LV_LOG_USE_FILE_LINE 1

/** Enable/Disable LV_LOG_TRACE in mem module */
#define LV_LOG_TRACE_MEM 1

/** Enable/Disable LV_LOG_TRACE in timer module */
#define LV_LOG_TRACE_TIMER 1

/** Enable/Disable LV_LOG_TRACE in indev module */
#define LV_LOG_TRACE_INDEV 1

/** Enable/Disable LV_LOG_TRACE in disp refr module */
#define LV_LOG_TRACE_DISP_REFR 1

/** Enable/Disable LV_LOG_TRACE in event module */
#define LV_LOG_TRACE_EVENT 1

/** Enable/Disable LV_LOG_TRACE in obj create module */
#define LV_LOG_TRACE_OBJ_CREATE 1

/** Enable/Disable LV_LOG_TRACE in layout module */
#define LV_LOG_TRACE_LAYOUT 1

/** Enable/Disable LV_LOG_TRACE in anim module */
#define LV_LOG_TRACE_ANIM 1

/** Enable/Disable LV_LOG_TRACE in cache module */
#define LV_LOG_TRACE_CACHE 1

#endif /*LV_USE_LOG*/


/*============================================================================
 * THEMES
 *============================================================================*/

/** A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT 1

#if LV_USE_THEME_DEFAULT
/** Yes to set dark mode, No to set light mode */
#define LV_THEME_DEFAULT_DARK 0

/** Enable grow on press */
#define LV_THEME_DEFAULT_GROW 1

/** Default transition time in [ms] */
#define LV_THEME_DEFAULT_TRANSITION_TIME 80

#endif /*LV_USE_THEME_DEFAULT*/

/** A very simple theme that is a good starting point for a custom theme */
#define LV_USE_THEME_SIMPLE 1

/** A theme designed for monochrome displays */
#define LV_USE_THEME_MONO 1



/*============================================================================
 * LAYOUTS
 *============================================================================*/

/** A layout similar to Flexbox in CSS. */
#define LV_USE_FLEX 1

/** A layout similar to Grid in CSS. */
#define LV_USE_GRID 1



/*============================================================================
 * IMAGE SETTINGS
 *============================================================================*/

/** When using only built-in image formats, caching provides little benefit.
 *  For complex image decoders (e.g. PNG or JPG), caching avoids repeatedly
 *  opening and decoding the same images, at the cost of additional RAM usage.
 */
#define LV_CACHE_DEF_SIZE 0

/** When using only built-in image formats, caching provides little benefit.
 *  For complex image decoders (e.g. PNG or JPG), caching avoids repeatedly
 *  reading image headers, at the cost of additional RAM usage.
 */
#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0

/** RLE decompress library */
#define LV_USE_RLE 0

/** Enable LZ4 compress/decompress lib */
#define LV_USE_LZ4 0

#if LV_USE_LZ4
/** Use lvgl built-in LZ4 lib */
#define LV_USE_LZ4_INTERNAL 1

#endif /*LV_USE_LZ4*/

/** Decode bin images to RAM */
#define LV_BIN_DECODER_RAM_LOAD 0

/** LODEPNG decoder library */
#define LV_USE_LODEPNG 0

/** PNG decoder(libpng) library */
#define LV_USE_LIBPNG 0

/** BMP decoder library */
#define LV_USE_BMP 0

/** JPG + split JPG decoder library.
 *  Split JPG is a custom format optimized for embedded systems.
 */
#define LV_USE_TJPGD 0

/** libjpeg-turbo decoder library.
 *  - Supports complete JPEG specifications and high-performance JPEG decoding.
 */
#define LV_USE_LIBJPEG_TURBO 0

/** WebP decoder library */
#define LV_USE_LIBWEBP 0

#if LV_DRAW_HAS_VECTOR_SUPPORT
/** SVG library
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_SVG 0

#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

#if LV_USE_SVG
/** SVG animation */
#define LV_USE_SVG_ANIMATION 0

/** Enable SVG debug logs */
#define LV_USE_SVG_DEBUG 0

#endif /*LV_USE_SVG*/


/*============================================================================
 * TEXT & FONT SETTINGS
 *============================================================================*/

/** Select a character encoding for strings. Your IDE or editor should have the same character encoding.
 *  Possible values:
 *  - LV_TXT_ENC_UTF8
 *  - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/** While rendering text strings, break (wrap) text on these chars. */
#define LV_TXT_BREAK_CHARS " ,.;:-_)]}"

/** If a word is at least this long, will break wherever 'prettiest'.
 *  To disable, set to a value <= 0.
 */
#define LV_TXT_LINE_BREAK_LONG_LEN 0

#if LV_TXT_LINE_BREAK_LONG_LEN > 0
/** Minimum number of characters in a long word to put on a line before a break. */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/** Minimum number of characters in a long word to put on a line after a break */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

#endif /*LV_TXT_LINE_BREAK_LONG_LEN > 0*/

/** The control character to use for signaling text recoloring */
#define LV_TXT_COLOR_CMD "#"

/** Allows mixing Left-to-Right and Right-to-Left texts.
 *  The direction will be processed according to the Unicode Bidirectional Algorithm:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics
 */
#define LV_USE_BIDI 0

#if LV_USE_BIDI
/** Set the default BIDI base direction
 *  Possible values:
 *  - LV_BASE_DIR_LTR: Left-to-Right
 *  - LV_BASE_DIR_RTL: Right-to-Left
 *  - LV_BASE_DIR_AUTO: Auto detect
 */
#define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO

#endif /*LV_USE_BIDI*/

/** In these languages characters should be replaced with
 *  another form based on their position in the text.
 */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/** Montserrat fonts with ASCII range and some symbols using bpp = 4
 *  https://fonts.google.com/specimen/Montserrat
 */
#define LV_FONT_MONTSERRAT_8 0

/** Enable Montserrat 10 */
#define LV_FONT_MONTSERRAT_10 0

/** Enable Montserrat 12 */
#define LV_FONT_MONTSERRAT_12 0

/** Enable Montserrat 14 */
#define LV_FONT_MONTSERRAT_14 1

/** Enable Montserrat 16 */
#define LV_FONT_MONTSERRAT_16 0

/** Enable Montserrat 18 */
#define LV_FONT_MONTSERRAT_18 0

/** Enable Montserrat 20 */
#define LV_FONT_MONTSERRAT_20 0

/** Enable Montserrat 22 */
#define LV_FONT_MONTSERRAT_22 0

/** Enable Montserrat 24 */
#define LV_FONT_MONTSERRAT_24 0

/** Enable Montserrat 26 */
#define LV_FONT_MONTSERRAT_26 0

/** Enable Montserrat 28 */
#define LV_FONT_MONTSERRAT_28 0

/** Enable Montserrat 30 */
#define LV_FONT_MONTSERRAT_30 0

/** Enable Montserrat 32 */
#define LV_FONT_MONTSERRAT_32 0

/** Enable Montserrat 34 */
#define LV_FONT_MONTSERRAT_34 0

/** Enable Montserrat 36 */
#define LV_FONT_MONTSERRAT_36 0

/** Enable Montserrat 38 */
#define LV_FONT_MONTSERRAT_38 0

/** Enable Montserrat 40 */
#define LV_FONT_MONTSERRAT_40 0

/** Enable Montserrat 42 */
#define LV_FONT_MONTSERRAT_42 0

/** Enable Montserrat 44 */
#define LV_FONT_MONTSERRAT_44 0

/** Enable Montserrat 46 */
#define LV_FONT_MONTSERRAT_46 0

/** Enable Montserrat 48 */
#define LV_FONT_MONTSERRAT_48 0

/** Enable Montserrat 28 compressed */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/** Enable Dejavu 16 Persian, Hebrew, Arabic letters */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0

/** Enable SourceHanSansSC 14 CJK */
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0

/** Enable SourceHanSansSC 16 CJK */
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0

/** Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8 0

/** Enable UNSCII 16 (Perfect monospace font) */
#define LV_FONT_UNSCII_16 0

/** Include the header given by LV_FONT_CUSTOM_INCLUDE to declare custom
 *  fonts (LV_FONT_CUSTOM_DECLARE) or override font-related macros.
 */
#define LV_FONT_USE_CUSTOM_INCLUDE 0

#if LV_FONT_USE_CUSTOM_INCLUDE
/** Optional header to override font-related macros.
 *
 *  Typical use is to declare extra fonts through LV_FONT_CUSTOM_DECLARE:
 *      #define LV_FONT_CUSTOM_DECLARE  LV_FONT_DECLARE(my_font_24)
 *
 *  It can also be used to override the default font to a custom one:
 *      #define LV_FONT_DEFAULT         &my_font_24
 */
#define LV_FONT_CUSTOM_INCLUDE ""

#endif /*LV_FONT_USE_CUSTOM_INCLUDE*/

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
#define LV_USE_CUSTOM_FONT_DEFAULT 0

#if !LV_USE_CUSTOM_FONT_DEFAULT
/** Select theme default text font
 *  Possible values:
 *  - LV_FONT_DEFAULT_MONTSERRAT_8 (enable: LV_FONT_MONTSERRAT_8)
 *  - LV_FONT_DEFAULT_MONTSERRAT_10 (enable: LV_FONT_MONTSERRAT_10)
 *  - LV_FONT_DEFAULT_MONTSERRAT_12 (enable: LV_FONT_MONTSERRAT_12)
 *  - LV_FONT_DEFAULT_MONTSERRAT_14 (enable: LV_FONT_MONTSERRAT_14)
 *  - LV_FONT_DEFAULT_MONTSERRAT_16 (enable: LV_FONT_MONTSERRAT_16)
 *  - LV_FONT_DEFAULT_MONTSERRAT_18 (enable: LV_FONT_MONTSERRAT_18)
 *  - LV_FONT_DEFAULT_MONTSERRAT_20 (enable: LV_FONT_MONTSERRAT_20)
 *  - LV_FONT_DEFAULT_MONTSERRAT_22 (enable: LV_FONT_MONTSERRAT_22)
 *  - LV_FONT_DEFAULT_MONTSERRAT_24 (enable: LV_FONT_MONTSERRAT_24)
 *  - LV_FONT_DEFAULT_MONTSERRAT_26 (enable: LV_FONT_MONTSERRAT_26)
 *  - LV_FONT_DEFAULT_MONTSERRAT_28 (enable: LV_FONT_MONTSERRAT_28)
 *  - LV_FONT_DEFAULT_MONTSERRAT_30 (enable: LV_FONT_MONTSERRAT_30)
 *  - LV_FONT_DEFAULT_MONTSERRAT_32 (enable: LV_FONT_MONTSERRAT_32)
 *  - LV_FONT_DEFAULT_MONTSERRAT_34 (enable: LV_FONT_MONTSERRAT_34)
 *  - LV_FONT_DEFAULT_MONTSERRAT_36 (enable: LV_FONT_MONTSERRAT_36)
 *  - LV_FONT_DEFAULT_MONTSERRAT_38 (enable: LV_FONT_MONTSERRAT_38)
 *  - LV_FONT_DEFAULT_MONTSERRAT_40 (enable: LV_FONT_MONTSERRAT_40)
 *  - LV_FONT_DEFAULT_MONTSERRAT_42 (enable: LV_FONT_MONTSERRAT_42)
 *  - LV_FONT_DEFAULT_MONTSERRAT_44 (enable: LV_FONT_MONTSERRAT_44)
 *  - LV_FONT_DEFAULT_MONTSERRAT_46 (enable: LV_FONT_MONTSERRAT_46)
 *  - LV_FONT_DEFAULT_MONTSERRAT_48 (enable: LV_FONT_MONTSERRAT_48)
 *  - LV_FONT_DEFAULT_MONTSERRAT_28_COMPRESSED (enable: LV_FONT_MONTSERRAT_28_COMPRESSED)
 *  - LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW: Dejavu 16 Persian, Hebrew, Arabic letters (enable: LV_FONT_DEJAVU_16_PERSIAN_HEBREW)
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK (enable: LV_FONT_SOURCE_HAN_SANS_SC_14_CJK)
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK (enable: LV_FONT_SOURCE_HAN_SANS_SC_16_CJK)
 *  - LV_FONT_DEFAULT_UNSCII_8: UNSCII 8 (Perfect monospace font) (enable: LV_FONT_UNSCII_8)
 *  - LV_FONT_DEFAULT_UNSCII_16: UNSCII 16 (Perfect monospace font) (enable: LV_FONT_UNSCII_16)
 */
#define LV_FONT_DEFAULT LV_FONT_DEFAULT_MONTSERRAT_14

#endif /*!LV_USE_CUSTOM_FONT_DEFAULT*/

/** The limit depends on the font size, font face and format
 *  but with > 10,000 characters if you see issues probably you
 *  need to enable it.
 */
#define LV_FONT_FMT_TXT_LARGE 0

/** Enables/disables support for compressed fonts. */
#define LV_USE_FONT_COMPRESSED 0

/** Enable drawing placeholders when glyph dsc is not found. */
#define LV_USE_FONT_PLACEHOLDER 1

/** Enable Font manager */
#define LV_USE_FONT_MANAGER 0

#if LV_USE_FONT_MANAGER
/** Font manager name max length */
#define LV_FONT_MANAGER_NAME_MAX_LEN 32

#endif /*LV_USE_FONT_MANAGER*/

/** Support using images as font in label or span widgets */
#define LV_USE_IMGFONT 0

/** FreeType library */
#define LV_USE_FREETYPE 0

#if LV_USE_FREETYPE
/** When enabled, FreeType will use LVGL's memory allocator and file system
 *  abstraction instead of the platform defaults.
 */
#define LV_FREETYPE_USE_LVGL_PORT 0

/** The maximum number of Glyph in count */
#define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256

#if LV_USE_OS == LV_OS_NONE
/** Enable L1 glyph metrics cache (lock-free, single-thread only) */
#define LV_FREETYPE_CACHE_FT_GLYPH_L1 1

#endif /*LV_USE_OS == LV_OS_NONE*/
#endif /*LV_USE_FREETYPE*/

/** Built-in TTF decoder */
#define LV_USE_TINY_TTF 0

#if LV_USE_TINY_TTF
/** Enable loading Tiny TTF data from files */
#define LV_TINY_TTF_FILE_SUPPORT 0

/** Tiny ttf cache entries count */
#define LV_TINY_TTF_CACHE_GLYPH_CNT 128

/** Tiny ttf kerning cache entries count */
#define LV_TINY_TTF_CACHE_KERNING_CNT 256

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
#define LV_WIDGETS_HAS_DEFAULT_VALUE 1

#if LV_DRAW_HAS_3D_SUPPORT
/** 3D Texture */
#define LV_USE_3DTEXTURE 0

#endif /*LV_DRAW_HAS_3D_SUPPORT*/

/** Anim image */
#define LV_USE_ANIMIMG 1

/** Arc */
#define LV_USE_ARC 1

/** Arc Label */
#define LV_USE_ARCLABEL 1

/** Bar */
#define LV_USE_BAR 1

/** Barcode code library
 *
 *  Enable: LV_USE_CANVAS
 */
#define LV_USE_BARCODE 0

/** Button */
#define LV_USE_BUTTON 1

/** Button matrix */
#define LV_USE_BUTTONMATRIX 1

/** Calendar */
#define LV_USE_CALENDAR 1

#if LV_USE_CALENDAR
/** Calendar week starts monday */
#define LV_CALENDAR_WEEK_STARTS_MONDAY 0

/** Shortened string for Monday */
#define LV_MONDAY_STR "Mo"

/** Shortened string for Tuesday */
#define LV_TUESDAY_STR "Tu"

/** Shortened string for Wednesday */
#define LV_WEDNESDAY_STR "We"

/** Shortened string for Thursday */
#define LV_THURSDAY_STR "Th"

/** Shortened string for Friday */
#define LV_FRIDAY_STR "Fr"

/** Shortened string for Saturday */
#define LV_SATURDAY_STR "Sa"

/** Shortened string for Sunday */
#define LV_SUNDAY_STR "Su"

/** String for January */
#define LV_JANUARY_STR "January"

/** String for February */
#define LV_FEBRUARY_STR "February"

/** String for March */
#define LV_MARCH_STR "March"

/** String for April */
#define LV_APRIL_STR "April"

/** String for May */
#define LV_MAY_STR "May"

/** String for June */
#define LV_JUNE_STR "June"

/** String for July */
#define LV_JULY_STR "July"

/** String for August */
#define LV_AUGUST_STR "August"

/** String for September */
#define LV_SEPTEMBER_STR "September"

/** String for October */
#define LV_OCTOBER_STR "October"

/** String for November */
#define LV_NOVEMBER_STR "November"

/** String for December */
#define LV_DECEMBER_STR "December"

/** Use calendar header arrow */
#define LV_USE_CALENDAR_HEADER_ARROW 1

/** Use calendar header dropdown */
#define LV_USE_CALENDAR_HEADER_DROPDOWN 1

/** Use chinese calendar */
#define LV_USE_CALENDAR_CHINESE 0

#endif /*LV_USE_CALENDAR*/

/** Canvas
 *
 *  Enable: LV_USE_IMAGE
 */
#define LV_USE_CANVAS 1

/** Chart */
#define LV_USE_CHART 1

/** Check Box */
#define LV_USE_CHECKBOX 1

/** Drop down list
 *
 *  Enable: LV_USE_LABEL
 */
#define LV_USE_DROPDOWN 1

/** FFmpeg library for image decoding and playing videos.
 *  Supports all major image formats so do not enable other image decoder with it.
 */
#define LV_USE_FFMPEG 0

#if LV_USE_FFMPEG
/** Dump format */
#define LV_FFMPEG_DUMP_FORMAT 0

/** You won't be able to open URLs after enabling this feature.
 *  Note that FFmpeg image decoder will always use lvgl file system.
 */
#define LV_FFMPEG_PLAYER_USE_LV_FS 0

#endif /*LV_USE_FFMPEG*/

/** GIF decoder library */
#define LV_USE_GIF 0

#if LV_USE_GIF
/** Use extra 16KB RAM to cache decoded data to accelerate */
#define LV_GIF_CACHE_DECODE_DATA 0

/** Increasing this value will consume more memory.
 *  GIFs wider than this are rejected
 */
#define LV_GIF_MAX_WIDTH 480

/** This value has no impact on memory usage
 *  GIFs taller than this are rejected
 */
#define LV_GIF_MAX_HEIGHT 32768

#endif /*LV_USE_GIF*/

#if LV_DRAW_HAS_3D_SUPPORT
/** Enables parsing and rendering of 3D models (.gltf/.glb).
 *
 *  Enable: LV_USE_3DTEXTURE
 */
#define LV_USE_GLTF 0

#endif /*LV_DRAW_HAS_3D_SUPPORT*/

/** GStreamer library
 *
 *  Enable: LV_USE_IMAGE
 */
#define LV_USE_GSTREAMER 0

/** Image
 *
 *  Enable: LV_USE_LABEL
 */
#define LV_USE_IMAGE 1

/** ImageButton */
#define LV_USE_IMAGEBUTTON 1

/** Enable Pinyin input method
 *
 *  Enable: LV_USE_KEYBOARD
 */
#define LV_USE_IME_PINYIN 0

#if LV_USE_IME_PINYIN
/** Enable Pinyin input method 9 key input mode */
#define LV_IME_PINYIN_USE_K9_MODE 1

#if LV_IME_PINYIN_USE_K9_MODE
/** Maximum number of candidate panels for 9-key input mode */
#define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3

#endif /*LV_IME_PINYIN_USE_K9_MODE*/

/** If you do not use the default thesaurus, be sure to use lv_ime_pinyin after setting the thesaurus */
#define LV_IME_PINYIN_USE_DEFAULT_DICT 1

/** Set the maximum number of candidate panels that can be displayed.
 *  This needs to be adjusted according to the size of the screen.
 */
#define LV_IME_PINYIN_CAND_TEXT_NUM 6

#endif /*LV_USE_IME_PINYIN*/

/** Keyboard */
#define LV_USE_KEYBOARD 1

/** Label */
#define LV_USE_LABEL 1

#if LV_USE_LABEL
/** Enable selecting text of the label */
#define LV_LABEL_TEXT_SELECTION 1

/** Store extra some info in labels (12 bytes) to speed up drawing of very long texts */
#define LV_LABEL_LONG_TXT_HINT 1

/** The count of wait chart */
#define LV_LABEL_WAIT_CHAR_COUNT 3

#endif /*LV_USE_LABEL*/

/** LED */
#define LV_USE_LED 1

/** Line */
#define LV_USE_LINE 1

/** List */
#define LV_USE_LIST 1

#if LV_DRAW_HAS_VECTOR_SUPPORT
#if LV_USE_THORVG
/** Enable Lottie animations
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_LOTTIE 0

#endif /*LV_USE_THORVG*/
#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

/** Menu */
#define LV_USE_MENU 1

/** Msgbox */
#define LV_USE_MSGBOX 1

/** QR code library
 *
 *  Enable: LV_USE_CANVAS
 */
#define LV_USE_QRCODE 0

/** Rlottie library (Deprecated: use LV_USE_LOTTIE instead) */
#define LV_USE_RLOTTIE 0

/** Roller
 *
 *  Enable: LV_USE_LABEL
 */
#define LV_USE_ROLLER 1

/** Scale */
#define LV_USE_SCALE 1

/** Slider
 *
 *  Enable: LV_USE_BAR
 */
#define LV_USE_SLIDER 1

/** Span */
#define LV_USE_SPAN 1

#if LV_USE_SPAN
/** Maximum number of span descriptor */
#define LV_SPAN_SNIPPET_STACK_SIZE 64

#endif /*LV_USE_SPAN*/

/** Spinbox */
#define LV_USE_SPINBOX 1

/** Spinner */
#define LV_USE_SPINNER 1

/** Switch */
#define LV_USE_SWITCH 1

/** Text area
 *
 *  Enable: LV_USE_LABEL
 */
#define LV_USE_TEXTAREA 1

#if LV_USE_TEXTAREA
/** Text area def. pwd show time [ms] */
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500

#endif /*LV_USE_TEXTAREA*/

/** Table */
#define LV_USE_TABLE 1

/** Tabview */
#define LV_USE_TABVIEW 1

/** Tileview */
#define LV_USE_TILEVIEW 1

/** Win */
#define LV_USE_WIN 1



/*============================================================================
 * DRIVERS
 *============================================================================*/

/** Driver for /dev/dri/card */
#define LV_USE_LINUX_DRM 0

#if LV_USE_LINUX_DRM
/** Legacy behavior, kept for backwards compatibility and slated for removal.
 *  While set, the backend is inferred from LV_USE_OPENGLES (EGL when OpenGLES
 *  is enabled).  Disable this and pick a backend explicitly via the Linux DRM
 *  "Rendering backend" choice.
 */
#define LV_LINUX_DRM_AUTO_BACKEND 1

/** Select how the Linux DRM driver presents rendered frames.
 *  Possible values:
 *  - LV_LINUX_DRM_BACKEND_FBDEV: Dumb buffers (no GPU)
 *  - LV_LINUX_DRM_BACKEND_GBM: GBM DMA buffers
 *  - LV_LINUX_DRM_BACKEND_EGL: EGL (OpenGL ES, hardware-accelerated) (enable: LV_USE_OPENGLES)
 */
#define LV_LINUX_DRM_BACKEND LV_LINUX_DRM_BACKEND_FBDEV

#endif /*LV_USE_LINUX_DRM*/

/** Driver for /dev/fb */
#define LV_USE_LINUX_FBDEV 0

#if LV_USE_LINUX_FBDEV
/** Use BSD flavored framebuffer device */
#define LV_LINUX_FBDEV_BSD 0

/** Framebuffer device render mode
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full mode
 */
#define LV_LINUX_FBDEV_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL

#define LV_LINUX_FBDEV_BUFFER_COUNT 0

#if LV_LINUX_FBDEV_BUFFER_COUNT == 0
/** Custom partial buffer size (in number of rows) */
#define LV_LINUX_FBDEV_BUFFER_SIZE 60

#endif /*LV_LINUX_FBDEV_BUFFER_COUNT == 0*/

/** Framebuffer device supports mmap */
#define LV_LINUX_FBDEV_MMAP 1

#endif /*LV_USE_LINUX_FBDEV*/

/** Use ft81x EVE driver */
#define LV_USE_FT81X 0

/** Interface for Lovyan_GFX */
#define LV_USE_LOVYAN_GFX 0

#if LV_USE_LOVYAN_GFX
/** Header for LovyanGFX user configuration */
#define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"

#endif /*LV_USE_LOVYAN_GFX*/

/** Generic MIPI driver for LCD devices connected via SPI or parallel
 *  port. Use this if your specific LCD controller is not listed among
 *  the dedicated drivers below.
 */
#define LV_USE_GENERIC_MIPI 0

/** Drivers for LCD devices connected via SPI/parallel port
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7735 0

/** Use ST7789 LCD driver
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7789 0

/** Use ST7796 LCD driver
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7796 0

/** Use ILI9341 LCD driver
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ILI9341 0

/** Use NV3007 LCD driver
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_NV3007 0

/** Driver for NXP ELCDIF */
#define LV_USE_NXP_ELCDIF 0

/** Driver for Renesas GLCD */
#define LV_USE_RENESAS_GLCDC 0

/** Driver for ST LTDC */
#define LV_USE_ST_LTDC 0

#if !LV_USE_DRAW_DMA2D
#if LV_USE_ST_LTDC
/** Only used for created partial mode LTDC displays */
#define LV_ST_LTDC_USE_DMA2D_FLUSH 0

#endif /*LV_USE_ST_LTDC*/
#endif /*!LV_USE_DRAW_DMA2D*/

/** Interface for TFT_eSPI */
#define LV_USE_TFT_ESPI 0

/** Driver for evdev input devices */
#define LV_USE_EVDEV 0

/** Driver for libinput input devices */
#define LV_USE_LIBINPUT 0

#if LV_USE_LIBINPUT
/** Use the BSD variant of the libinput input driver */
#define LV_LIBINPUT_BSD 0

/** Enable full keyboard support via XKB */
#define LV_LIBINPUT_XKB 0

#if LV_LIBINPUT_XKB
/** XKB rules */
#define LV_LIBINPUT_XKB_RULES ""

/** XKB model */
#define LV_LIBINPUT_XKB_MODEL "pc101"

/** XKB layout */
#define LV_LIBINPUT_XKB_LAYOUT "us"

/** XKB variant */
#define LV_LIBINPUT_XKB_VARIANT ""

/** If enabled, NULL is passed for XKB options, letting the system
 *  default apply (e.g. from XKB_DEFAULT_OPTIONS env var).
 *  If disabled, the string from LV_LIBINPUT_XKB_OPTIONS is used instead.
 */
#define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT 1

#if !LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
/** XKB options */
#define LV_LIBINPUT_XKB_OPTIONS ""

#endif /*!LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT*/
#endif /*LV_LIBINPUT_XKB*/
#endif /*LV_USE_LIBINPUT*/

/** Use Nuttx to open window and handle touchscreen */
#define LV_USE_NUTTX 0

#if LV_USE_NUTTX
/** Use independent image heap */
#define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0

#if LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
/** Use independent image heap for default draw buffer */
#define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP 0

#endif /*LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP*/

/** Use uv loop to replace default timer loop and other fb/indev timers */
#define LV_USE_NUTTX_LIBUV 0

/** Use Custom Nuttx init API to open window and handle touchscreen */
#define LV_USE_NUTTX_CUSTOM_INIT 0

/** Use NuttX LCD device */
#define LV_USE_NUTTX_LCD 0

#define LV_NUTTX_LCD_BUFFER_COUNT 0

#if LV_NUTTX_LCD_BUFFER_COUNT == 0
#if LV_USE_NUTTX_LCD
/** Custom partial buffer size (in number of rows) */
#define LV_NUTTX_LCD_BUFFER_SIZE 60

#endif /*LV_USE_NUTTX_LCD*/
#endif /*LV_NUTTX_LCD_BUFFER_COUNT == 0*/

/** Use NuttX touchscreen driver */
#define LV_USE_NUTTX_TOUCHSCREEN 0

#if LV_USE_NUTTX_TOUCHSCREEN
/** Set to 0 to disable cursor, or set to a value greater than 0 to set the cursor size in pixels. */
#define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE 0

#endif /*LV_USE_NUTTX_TOUCHSCREEN*/

/** Use NuttX mouse driver */
#define LV_USE_NUTTX_MOUSE 0

#if LV_USE_NUTTX_MOUSE
/** Set the step size of the mouse movement in pixels. */
#define LV_USE_NUTTX_MOUSE_MOVE_STEP 1

#endif /*LV_USE_NUTTX_MOUSE*/

#if LV_USE_PROFILER_BUILTIN
/** Use NuttX trace file */
#define LV_USE_NUTTX_TRACE_FILE 0

#endif /*LV_USE_PROFILER_BUILTIN*/

#if LV_USE_NUTTX_TRACE_FILE
/** NuttX trace file path */
#define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"

#endif /*LV_USE_NUTTX_TRACE_FILE*/
#endif /*LV_USE_NUTTX*/

/** Use a generic OpenGL driver that can be used to embed in other applications or used with GLFW/EGL
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_OPENGLES 0

#if LV_USE_OPENGLES
/** Enable debug mode for OpenGL */
#define LV_USE_OPENGLES_DEBUG 0

#endif /*LV_USE_OPENGLES*/

#if !LV_USE_EGL
/** Use GLFW to open window on PC and handle mouse and keyboard.
 *
 *  Enable: LV_USE_OPENGLES
 */
#define LV_USE_GLFW 0

#endif /*!LV_USE_EGL*/

/** QNX Screen display and input drivers */
#define LV_USE_QNX 0

#if LV_USE_QNX
/** QNX Buffer count */
#define LV_QNX_BUF_COUNT 1

#endif /*LV_USE_QNX*/

/** Use SDL to open window on PC and handle mouse and keyboard. */
#define LV_USE_SDL 0

#if LV_USE_SDL
/** SDL include path */
#define LV_SDL_INCLUDE_PATH "SDL2/SDL.h"

/** Legacy behavior, kept for backwards compatibility and slated for removal.
 *  While set, EGL is inferred the legacy way (OpenGLES enabled with an OpenGL
 *  draw unit).  Disable this and pick a backend explicitly via the SDL
 *  "Rendering backend" choice.
 */
#define LV_SDL_AUTO_BACKEND 1

/** Select how the SDL driver presents rendered frames.
 *  Possible values:
 *  - LV_SDL_BACKEND_SW: Software (SDL surface)
 *  - LV_SDL_BACKEND_TEXTURE: Cached SDL textures (enable: LV_USE_DRAW_SDL)
 *  - LV_SDL_BACKEND_EGL: EGL (OpenGL ES, hardware-accelerated) (requires LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
 */
#define LV_SDL_BACKEND LV_SDL_BACKEND_SW

/** LV_DISPLAY_RENDER_MODE_DIRECT is recommended for best performance
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Use the buffer(s) to render the screen is smaller parts
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Only the changed areas will be updated with 2 screen sized buffers
 *  - LV_DISPLAY_RENDER_MODE_FULL: Always redraw the whole screen even if only one pixel has been changed with 2 screen sized buffers
 */
#define LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT

/** Number of screen-sized buffers (1 or 2) */
#define LV_SDL_BUF_COUNT 1

/** Use hardware acceleration */
#define LV_SDL_ACCELERATED 1

/** SDL fullscreen */
#define LV_SDL_FULLSCREEN 0

/** Exit the application when all SDL windows are closed */
#define LV_SDL_DIRECT_EXIT 1

/** SDL mousewheel mode
 *  Possible values:
 *  - LV_SDL_MOUSEWHEEL_MODE_ENCODER: The mousewheel emulates an encoder input device
 *  - LV_SDL_MOUSEWHEEL_MODE_CROWN: The mousewheel emulates a smart watch crown
 */
#define LV_SDL_MOUSEWHEEL_MODE LV_SDL_MOUSEWHEEL_MODE_ENCODER

#endif /*LV_USE_SDL*/

/** LVGL UEFI backend */
#define LV_USE_UEFI 0

#if LV_USE_UEFI
/** Header that hides the actual framework (EDK2, gnu-efi, ...) */
#define LV_USE_UEFI_INCLUDE "myefi.h"

/** Use the memory services from the boot services table */
#define LV_UEFI_USE_MEMORY_SERVICES 0

#endif /*LV_USE_UEFI*/

/** Use Wayland to open a window and handle input on Linux or BSD desktops */
#define LV_USE_WAYLAND 0

#if LV_USE_WAYLAND
/** Deinitialize LVGL and quit the application when the last wayland window closes */
#define LV_WAYLAND_DIRECT_EXIT 1

/** Legacy behavior, kept for backwards compatibility and slated for removal.
 *  While set, the backend defaults to SHM and any LV_WAYLAND_USE_* set directly
 *  in lv_conf.h is honored.  Disable this and pick a backend explicitly via the
 *  Wayland "Rendering backend" choice.
 */
#define LV_WAYLAND_AUTO_BACKEND 1

/** Select the rendering backend used by the Wayland driver.
 *  Possible values:
 *  - LV_WAYLAND_BACKEND_SHM: SHM (Shared Memory)
 *  - LV_WAYLAND_BACKEND_EGL: EGL (OpenGL ES, hardware-accelerated) (enable: LV_USE_OPENGLES)
 *  - LV_WAYLAND_BACKEND_G2D: G2D (NXP i.MX hardware accelerator) (enable: LV_USE_DRAW_G2D)
 */
#define LV_WAYLAND_BACKEND LV_WAYLAND_BACKEND_SHM

#endif /*LV_USE_WAYLAND*/

#if LV_USE_OS == LV_OS_WINDOWS
/** LVGL Windows backend */
#define LV_USE_WINDOWS 0

#endif /*LV_USE_OS == LV_OS_WINDOWS*/

/** Use X11 to open window on Linux desktop and handle mouse and keyboard */
#define LV_USE_X11 0

#if LV_USE_X11
/** Use double buffers for lvgl rendering */
#define LV_X11_DOUBLE_BUFFER 1

/** Exit the application when all X11 windows have been closed */
#define LV_X11_DIRECT_EXIT 1

/** X11 device render mode
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial render mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct render mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full render mode
 */
#define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL

#endif /*LV_USE_X11*/

/** Enable NanoVG (vector graphics library) */
#define LV_USE_NANOVG 0



/*============================================================================
 * FILE SYSTEM
 *============================================================================*/

/** Setting a default drive letter allows skipping the driver
 *  prefix in filepaths.
 */
#define LV_FS_DEFAULT_DRIVER_LETTER 0

/** File system on top of stdio */
#define LV_USE_FS_STDIO 0

#if LV_USE_FS_STDIO
/** Driver-identifier letter for stdio (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_STDIO_LETTER 0

/** Working directory for stdio */
#define LV_FS_STDIO_PATH ""

/** Read cache size in bytes for stdio (0 = disabled) */
#define LV_FS_STDIO_CACHE_SIZE 0

#endif /*LV_USE_FS_STDIO*/

/** File system on top of POSIX */
#define LV_USE_FS_POSIX 0

#if LV_USE_FS_POSIX
/** Driver-identifier letter for POSIX (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_POSIX_LETTER 0

/** Working directory for POSIX */
#define LV_FS_POSIX_PATH ""

/** Read cache size in bytes for POSIX (0 = disabled) */
#define LV_FS_POSIX_CACHE_SIZE 0

#endif /*LV_USE_FS_POSIX*/

/** File system on top of Win32 */
#define LV_USE_FS_WIN32 0

#if LV_USE_FS_WIN32
/** Driver-identifier letter for Win32 (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_WIN32_LETTER 0

/** Working directory for Win32 */
#define LV_FS_WIN32_PATH ""

/** Read cache size in bytes for Win32 (0 = disabled) */
#define LV_FS_WIN32_CACHE_SIZE 0

#endif /*LV_USE_FS_WIN32*/

/** File system on top of FatFS */
#define LV_USE_FS_FATFS 0

#if LV_USE_FS_FATFS
/** Driver-identifier letter for FatFS (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_FATFS_LETTER 0

/** Working directory for FatFS */
#define LV_FS_FATFS_PATH ""

/** Read cache size in bytes for FatFS (0 = disabled) */
#define LV_FS_FATFS_CACHE_SIZE 0

#endif /*LV_USE_FS_FATFS*/

/** File system on top of littlefs */
#define LV_USE_FS_LITTLEFS 0

#if LV_USE_FS_LITTLEFS
/** Driver-identifier letter for littlefs (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_LITTLEFS_LETTER 0

/** Working directory for littlefs */
#define LV_FS_LITTLEFS_PATH ""

#endif /*LV_USE_FS_LITTLEFS*/

/** File system on top of Arduino ESP littlefs */
#define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
/** Driver-identifier letter for Arduino ESP littlefs (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER 0

/** Working directory for Arduino ESP littlefs */
#define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""

#endif /*LV_USE_FS_ARDUINO_ESP_LITTLEFS*/

/** File system on top of Arduino SD */
#define LV_USE_FS_ARDUINO_SD 0

#if LV_USE_FS_ARDUINO_SD
/** Driver-identifier letter for Arduino SD (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_ARDUINO_SD_LETTER 0

/** Working directory for Arduino SD */
#define LV_FS_ARDUINO_SD_PATH ""

#endif /*LV_USE_FS_ARDUINO_SD*/

/** File system on top of UEFI */
#define LV_USE_FS_UEFI 0

#if LV_USE_FS_UEFI
/** Driver-identifier letter for UEFI (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_UEFI_LETTER 0

#endif /*LV_USE_FS_UEFI*/

/** File system on top of FrogFS */
#define LV_USE_FS_FROGFS 0

#if LV_USE_FS_FROGFS
/** Driver-identifier letter for FrogFS (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_FROGFS_LETTER 0

#endif /*LV_USE_FS_FROGFS*/

/** API for memory-mapped file access. */
#define LV_USE_FS_MEMFS 0

#if LV_USE_FS_MEMFS
/** Driver-identifier letter for memfs (e.g. 65 for 'A', 0 = disabled) */
#define LV_FS_MEMFS_LETTER 0

#endif /*LV_USE_FS_MEMFS*/


/*============================================================================
 * DEBUGGING
 *============================================================================*/

/** Enable system monitor component */
#define LV_USE_SYSMON 0

#if LV_USE_SYSMON
/** Enable process idle measurement */
#define LV_SYSMON_PROC_IDLE_AVAILABLE 0

/** Show CPU usage and FPS count */
#define LV_USE_PERF_MONITOR 0

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
#define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT

/** Prints performance data using log */
#define LV_USE_PERF_MONITOR_LOG_MODE 0

#endif /*LV_USE_PERF_MONITOR*/

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
/** Show the used memory and the memory fragmentation */
#define LV_USE_MEM_MONITOR 0

#endif /*LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN*/

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
#define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT

#endif /*LV_USE_MEM_MONITOR*/

/** Include the header given by LV_SYSMON_CUSTOM_INCLUDE to override system
 *  monitor macros such as LV_SYSMON_GET_IDLE.
 */
#define LV_SYSMON_USE_CUSTOM_INCLUDE 0

#if LV_SYSMON_USE_CUSTOM_INCLUDE
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
#define LV_SYSMON_CUSTOM_INCLUDE ""

#endif /*LV_SYSMON_USE_CUSTOM_INCLUDE*/
#endif /*LV_USE_SYSMON*/

/** Enable runtime performance profiler */
#define LV_USE_PROFILER 0

/** Enable the built-in profiler
 *
 *  Enable: LV_USE_PROFILER
 */
#define LV_USE_PROFILER_BUILTIN 0

#if LV_USE_PROFILER_BUILTIN
/** Default profiler trace buffer size in bytes */
#define LV_PROFILER_BUILTIN_BUF_SIZE 16384

/** Enable built-in profiler by default */
#define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1

/** Enable POSIX profiler port */
#define LV_USE_PROFILER_BUILTIN_POSIX 0

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
#define LV_PROFILER_INCLUDE "lvgl/debugging/profiler/lv_profiler_builtin.h"

/** Enable layout profiler */
#define LV_PROFILER_LAYOUT 1

/** Enable disp refr profiler */
#define LV_PROFILER_REFR 1

/** Enable draw profiler */
#define LV_PROFILER_DRAW 1

/** Enable indev profiler */
#define LV_PROFILER_INDEV 1

/** Enable decoder profiler */
#define LV_PROFILER_DECODER 1

/** Enable font profiler */
#define LV_PROFILER_FONT 1

/** Enable fs profiler */
#define LV_PROFILER_FS 1

/** Enable style profiler */
#define LV_PROFILER_STYLE 0

/** Enable timer profiler */
#define LV_PROFILER_TIMER 1

/** Enable cache profiler */
#define LV_PROFILER_CACHE 1

/** Enable event profiler */
#define LV_PROFILER_EVENT 1

#endif /*LV_USE_PROFILER*/

/** Enable emulated input devices, time emulation, and screenshot compares */
#define LV_USE_TEST 0

#if LV_USE_TEST
/** Enable `lv_test_screenshot_compare`.
 *  Requires a few MB of extra RAM.
 *
 *  Enable: LV_USE_LODEPNG
 */
#define LV_USE_TEST_SCREENSHOT_COMPARE 0

#if LV_USE_TEST_SCREENSHOT_COMPARE
/** Create a reference image by default if it does not exist */
#define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1

#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/
#endif /*LV_USE_TEST*/

/** Enable Monkey test */
#define LV_USE_MONKEY 0

/** Draw random colored rectangles over the redrawn areas. */
#define LV_USE_REFR_DEBUG 0

/** Draw a red overlay for ARGB layers and a green overlay for RGB layers */
#define LV_USE_LAYER_DEBUG 0

/** Also add the index number of the draw unit on white background.
 *  For layers add the index number of the draw unit on black background.
 */
#define LV_USE_PARALLEL_DRAW_DEBUG 0



/*============================================================================
 * OTHERS
 *============================================================================*/

/** Enable `lv_obj` fragment logic */
#define LV_USE_FRAGMENT 0

/** Enable file explorer.
 *
 *  Enable: LV_USE_TABLE
 */
#define LV_USE_FILE_EXPLORER 0

#if LV_USE_FILE_EXPLORER
/** Maximum length of path */
#define LV_FILE_EXPLORER_PATH_MAX_LEN 128

/** This can save some memory, but not much.
 *  After the quick access bar is created, it can be hidden
 *  by clicking the button at the top left corner of
 *  the browsing area, which is very useful for small screen devices.
 */
#define LV_FILE_EXPLORER_QUICK_ACCESS 1

#endif /*LV_USE_FILE_EXPLORER*/


/*============================================================================
 * BUILD
 *============================================================================*/

/** Include `lvgl_private.h` in `lvgl.h` to access internal data and functions by default */
#define LV_USE_PRIVATE_API 0

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
#define LV_ENABLE_GLOBAL_CUSTOM 0

#if LV_ENABLE_GLOBAL_CUSTOM
/** Use a custom header for the declaration of:
 *        lv_global_t *lv_global_default(void);
 */
#define LV_GLOBAL_USE_CUSTOM_INCLUDE 0

#if LV_GLOBAL_USE_CUSTOM_INCLUDE
/** Path to the header that declares:
 *    lv_global_t *lv_global_default(void);
 *
 *  This header is included wherever LVGL needs to resolve the
 *  LV_GLOBAL_DEFAULT() macro. Keep it lightweight, it is included
 *  in performance-critical paths.
 */
#define LV_GLOBAL_CUSTOM_INCLUDE "lv_global.h"

#endif /*LV_GLOBAL_USE_CUSTOM_INCLUDE*/
#endif /*LV_ENABLE_GLOBAL_CUSTOM*/

/** Check if the parameter is NULL. (Very fast, recommended) */
#define LV_USE_ASSERT_NULL 1

/** Checks if the memory is successfully allocated or no. (Very fast, recommended) */
#define LV_USE_ASSERT_MALLOC 1

/** Check if the styles are properly initialized. (Very fast, recommended) */
#define LV_USE_ASSERT_STYLE 0

/** Check the integrity of `lv_mem` after critical operations. (Slow) */
#define LV_USE_ASSERT_MEM_INTEGRITY 0

/** Check NULL, the object's type and existence (e.g. not deleted). (Slow) */
#define LV_USE_ASSERT_OBJ 0

/** Disable warning saying `LV_ASSERT_HANDLER_INCLUDE` is deprecated.
 *  This setting allows you to keep using old LVGL versions which rely on
 *  LV_ASSERT_HANDLER_INCLUDE without triggering a compiler warning.
 *
 *  LV_ASSERT_HANDLER_INCLUDE has been deprecated in favor of the new approach:
 *  define LV_ASSERT_CUSTOM_INCLUDE with the header path, and place the
 *  LV_ASSERT_HANDLER macro definition inside that header instead.
 *
 *  Only enable this if you are maintaining compatibility with existing code
 *  that uses LV_ASSERT_HANDLER_INCLUDE and cannot migrate immediately.
 *  New projects should use LV_ASSERT_CUSTOM_INCLUDE and LV_ASSERT_HANDLER
 *  directly.
 */
#define LV_DISABLE_ASSERT_HANDLER_INCLUDE_WARNING 0

/** Use LV_ASSERT_CUSTOM_INCLUDE to define the path to a file defining a
 *  custom LV_ASSERT_HANDLER
 */
#define LV_ASSERT_USE_CUSTOM_INCLUDE 0

#if LV_ASSERT_USE_CUSTOM_INCLUDE
/** Optional header to override assert-related macros.
 *
 *  LV_ASSERT_HANDLER
 *    Statement to execute when an assertion fails.
 *    e.g. #define LV_ASSERT_HANDLER my_assert_failed(__FILE__, __LINE__);
 */
#define LV_ASSERT_CUSTOM_INCLUDE ""

#endif /*LV_ASSERT_USE_CUSTOM_INCLUDE*/

/** When enabled, LV_CHECK_ARG checks validate function arguments
 *  at runtime. Failed checks log a warning and execute the specified
 *  action. When disabled, all LV_CHECK_ARG checks compile to nothing.
 *  Disabling this is not recommended unless extreme care is taken and only
 *  in very resource constrained environments where it can be absolutely
 *  ensured that invariants are never violated.
 */
#define LV_USE_CHECK_ARG 1

#if LV_USE_CHECK_ARG
/** When enabled, LV_ASSERT_HANDLER is also invoked when an
 *  LV_CHECK_ARG check fails, before the action is executed.
 */
#define LV_CHECK_ARG_ASSERT_ON_FAIL 0

/** Controls what is logged when an LV_CHECK_ARG check fails.
 *  MINIMAL and VERBOSE modes require LV_USE_LOG to be enabled;
 *  selecting either when LV_USE_LOG is disabled will cause a
 *  compile-time error.
 *  Possible values:
 *  - LV_CHECK_ARG_LOG_MODE_NONE: no log output on failure
 *  - LV_CHECK_ARG_LOG_MODE_MINIMAL: log 'Check failed' only (file/line from LV_LOG_WARN) (requires LV_USE_LOG)
 *  - LV_CHECK_ARG_LOG_MODE_VERBOSE: log 'Check failed: <cond>' plus caller-supplied message (requires LV_USE_LOG)
 */
#define LV_CHECK_ARG_LOG_MODE LV_CHECK_ARG_LOG_MODE_NONE

/** When enabled, LV_CHECK_OBJ verifies that the object has the
 *  expected class (lv_obj_has_class). When disabled, the class
 *  check is skipped even if a class argument is supplied.
 */
#define LV_USE_CHECK_OBJ_CLASSTYPE 0

/** When enabled, LV_CHECK_OBJ also verifies that the object is
 *  still part of the widget tree (lv_obj_is_valid). When disabled,
 *  the validity check is skipped even if the associated argument
 *  is supplied.
 */
#define LV_USE_CHECK_OBJ_VALIDITY 0

#endif /*LV_USE_CHECK_ARG*/


/*============================================================================
 * COMPILER SETTINGS
 *============================================================================*/

/** For big endian systems set to 1 */
#define LV_BIG_ENDIAN_SYSTEM 0

/** Include the header given by LV_ATTRIBUTE_CUSTOM_INCLUDE to override the
 *  LV_ATTRIBUTE_* macros for your platform.
 */
#define LV_ATTRIBUTE_USE_CUSTOM_INCLUDE 0

#if LV_ATTRIBUTE_USE_CUSTOM_INCLUDE
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
#define LV_ATTRIBUTE_CUSTOM_INCLUDE ""

#endif /*LV_ATTRIBUTE_USE_CUSTOM_INCLUDE*/


/*============================================================================
 * EXAMPLES
 *============================================================================*/

/** Enable examples to be built with the library. */
#define LV_BUILD_EXAMPLES 1



/*============================================================================
 * DEMOS
 *============================================================================*/

/** Build the demos */
#define LV_BUILD_DEMOS 1

#if LV_BUILD_DEMOS
/** Benchmark your system
 *
 *  Enable: LV_FONT_MONTSERRAT_14, LV_FONT_MONTSERRAT_20, LV_FONT_MONTSERRAT_24, LV_FONT_MONTSERRAT_26, LV_USE_DEMO_WIDGETS
 */
#define LV_USE_DEMO_BENCHMARK 0

#if LV_USE_DEMO_BENCHMARK
/** Use static aligned fonts */
#define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0

#endif /*LV_USE_DEMO_BENCHMARK*/

#if LV_USE_GLTF
/** glTF demo */
#define LV_USE_DEMO_GLTF 0

#endif /*LV_USE_GLTF*/

/** Demonstrate the usage of encoder and keyboard */
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

/** Music player demo */
#define LV_USE_DEMO_MUSIC 0

#if LV_USE_DEMO_MUSIC
/** Enable Square */
#define LV_DEMO_MUSIC_SQUARE 0

/** Enable Landscape */
#define LV_DEMO_MUSIC_LANDSCAPE 0

/** Enable Round */
#define LV_DEMO_MUSIC_ROUND 0

/** Enable Large */
#define LV_DEMO_MUSIC_LARGE 0

/** Enable Auto play */
#define LV_DEMO_MUSIC_AUTO_PLAY 0

#endif /*LV_USE_DEMO_MUSIC*/

/** Render test for each primitives. Requires at least 480x272 display
 *
 *  Enable: LV_USE_GRID
 */
#define LV_USE_DEMO_RENDER 0

/** Stress test for LVGL */
#define LV_USE_DEMO_STRESS 0

#if LV_DRAW_HAS_VECTOR_SUPPORT
/** vector graphic demo
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_DEMO_VECTOR_GRAPHIC 0

#endif /*LV_DRAW_HAS_VECTOR_SUPPORT*/

/** Show-case LVGL widgets */
#define LV_USE_DEMO_WIDGETS 0

/** Flex layout previewer */
#define LV_USE_DEMO_FLEX_LAYOUT 0

/** multi-language demo */
#define LV_USE_DEMO_MULTILANG 0

/** Smartwatch demo */
#define LV_USE_DEMO_SMARTWATCH 0

/** Ebike demo */
#define LV_USE_DEMO_EBIKE 0

#if LV_USE_DEMO_EBIKE
/** Ebike portrait layout */
#define LV_DEMO_EBIKE_PORTRAIT 0

#endif /*LV_USE_DEMO_EBIKE*/

/** High resolution demo */
#define LV_USE_DEMO_HIGH_RES 0

#endif /*LV_BUILD_DEMOS*/


/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/

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
 *  - LV_STDLIB_BUILTIN: LVGL built-in
 *  - LV_STDLIB_CLIB: C standard library (malloc/realloc/free)
 *  - LV_STDLIB_MICROPYTHON
 *  - LV_STDLIB_RTTHREAD
 *  - LV_STDLIB_CUSTOM: Custom (implemented externally)
 */
#define LV_USE_STDLIB_MALLOC LV_STDLIB_BUILTIN

/** String functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL built-in
 *  - LV_STDLIB_CLIB: C standard library (memcpy/memset/strlen/strcpy)
 *  - LV_STDLIB_RTTHREAD: RT-Thread (rt_memcpy/rt_memset/rt_strlen/rt_strcpy)
 *  - LV_STDLIB_CUSTOM: Custom (implemented externally)
 */
#define LV_USE_STDLIB_STRING LV_STDLIB_BUILTIN

/** Sprintf functions source
 *  Possible values:
 *  - LV_STDLIB_BUILTIN: LVGL built-in
 *  - LV_STDLIB_CLIB: C standard library (vsnprintf)
 *  - LV_STDLIB_RTTHREAD: RT-Thread (rt_vsnprintf)
 *  - LV_STDLIB_CUSTOM: Custom (implemented externally)
 */
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_BUILTIN

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
/** Size of the pool `lv_malloc()` allocates from. Needs to be at least 2kB (2048). */
#define LV_MEM_SIZE 65536

/** Place the pool at a fixed address instead of allocating it as a normal array.
 *  0: unused.
 */
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

/** Operating system
 *  Possible values:
 *  - LV_OS_NONE
 *  - LV_OS_PTHREAD: POSIX threads (pthread)
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
/** Synchronize with direct task notifications instead of an intermediary object such as a
 *  binary semaphore: 45% faster and less RAM, but only usable when a single task can be
 *  the recipient of the event.
 */
#define LV_USE_FREERTOS_TASK_NOTIFY 1

/** Provide your own lv_os_get_idle_percent(). Useful on multi-core systems, where the
 *  default FreeRTOS implementation may not track idle time across all cores.
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

/** Used to initialize default sizes such as widget sizes and style paddings. */
#define LV_DPI_DEF 130

/** Align the stride of all layers and images to this many bytes. */
#define LV_DRAW_BUF_STRIDE_ALIGN 1

/** Align the start address of draw_buf addresses to this many bytes. */
#define LV_DRAW_BUF_ALIGN 4

/** 3x3 matrix API (lv_matrix_t) for transformations. Requires floating point support.
 *
 *  Enable: LV_USE_FLOAT
 */
#define LV_USE_MATRIX 0

#if LV_USE_MATRIX
/** The rendering engine must support 3x3 matrix transformations. */
#define LV_DRAW_TRANSFORM_USE_MATRIX 0

#endif /*LV_USE_MATRIX*/

/** Widgets with `style_opa < 255` (not `bg_opa`, `text_opa` etc) or a non-NORMAL blend mode are
 *  buffered into a "simple" layer in chunks of this size.
 *  "Transformed layers" (if `transform_angle/zoom` are set) use larger buffers and can't be chunked.
 */
#define LV_DRAW_LAYER_SIMPLE_BUF_SIZE 24576

/** Limit for simple and transformed layers together; 0 means no limit.
 *  Must be at least `LV_DRAW_LAYER_SIMPLE_BUF_SIZE`, and with transformed layers large enough
 *  for the largest widget too (width x height x 4).
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

/** Vector drawing API (lv_vector_dsc_*) for paths, strokes and fills.
 *  Needs a renderer with vector support, e.g. the SW renderer with ThorVG, or VG-Lite.
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_VECTOR_GRAPHIC 0

/** Render a widget and its children into an image buffer with lv_snapshot_take(). */
#define LV_USE_SNAPSHOT 0

/** Backend that gives the SW renderer vector graphics support. */
#define LV_USE_THORVG 0

#if LV_USE_THORVG
/** Build the ThorVG copy shipped with LVGL instead of linking an external one. */
#define LV_USE_THORVG_INTERNAL 1

#endif /*LV_USE_THORVG*/

/** Required to draw anything on the screen. */
#define LV_USE_DRAW_SW 1

#if LV_USE_DRAW_SW
/** RGB565 */
#define LV_DRAW_SW_SUPPORT_RGB565 1

/** RGB565 swapped */
#define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED 1

/** RGB565A8 */
#define LV_DRAW_SW_SUPPORT_RGB565A8 1

/** RGB888 */
#define LV_DRAW_SW_SUPPORT_RGB888 1

/** XRGB8888 */
#define LV_DRAW_SW_SUPPORT_XRGB8888 1

/** ARGB8888 */
#define LV_DRAW_SW_SUPPORT_ARGB8888 1

/** ARGB8888 premultiplied */
#define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 1

/** L8 */
#define LV_DRAW_SW_SUPPORT_L8 1

/** AL88 */
#define LV_DRAW_SW_SUPPORT_AL88 1

/** A8 */
#define LV_DRAW_SW_SUPPORT_A8 1

/** I1 */
#define LV_DRAW_SW_SUPPORT_I1 1

#if LV_DRAW_SW_SUPPORT_I1
/** Pixels brighter than this luminance become active (set) when rendering to I1. */
#define LV_DRAW_SW_I1_LUM_THRESHOLD 127

#endif /*LV_DRAW_SW_SUPPORT_I1*/

#if LV_USE_OS != LV_OS_NONE
/** Number of threads used to render a frame in parallel */
#define LV_DRAW_SW_DRAW_UNIT_CNT 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Requires the Arm-2D library in your project and an include path for "arm_2d.h". */
#define LV_USE_DRAW_ARM2D_SYNC 0

/** Disable to let Arm-2D work on its own (for testing only). */
#define LV_USE_NATIVE_HELIUM_ASM 0

/** Adds rounded corners, shadows, skewed lines and arcs.
 *  Without it only rectangles with gradients, images, texts and straight lines can be drawn.
 */
#define LV_DRAW_SW_COMPLEX 1

/** Each additional stop costs (sizeof(lv_color_t) + 1) bytes. */
#define LV_GRADIENT_MAX_STOPS 2

/** Adds linear gradients at an angle, plus radial and conical gradients. */
#define LV_USE_DRAW_SW_COMPLEX_GRADIENTS 0

#if LV_DRAW_SW_COMPLEX
/** Maximum shadow size to buffer, where shadow size is `shadow_width + radius`.
 *  Costs this value squared in RAM; 0 disables caching.
 */
#define LV_DRAW_SW_SHADOW_CACHE_SIZE 0

/** The circumference of a 1/4 circle is cached for anti-aliasing, costing
 *  radius * 4 bytes per circle. Set to 0 to disable caching.
 */
#define LV_DRAW_SW_CIRCLE_CACHE_SIZE 4

#endif /*LV_DRAW_SW_COMPLEX*/

/** SW assembly optimization
 *  Possible values:
 *  - LV_DRAW_SW_ASM_NONE
 *  - LV_DRAW_SW_ASM_NEON
 *  - LV_DRAW_SW_ASM_HELIUM
 *  - LV_DRAW_SW_ASM_RISCV_V: RISC-V Vector
 *  - LV_DRAW_SW_ASM_CUSTOM
 */
#define LV_USE_DRAW_SW_ASM LV_DRAW_SW_ASM_NONE

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
/** Custom assembly include file */
#define LV_DRAW_SW_ASM_CUSTOM_INCLUDE ""

#endif /*LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM*/
#endif /*LV_USE_DRAW_SW*/

/** Renders with a VG-Lite GPU. Requires a VG-Lite driver, or the VG-Lite simulator below.
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_DRAW_VG_LITE 0

#if LV_USE_DRAW_VG_LITE
/** Call an application-provided gpu_init() before the first use of the GPU. */
#define LV_VG_LITE_USE_GPU_INIT 0

/** Check internal VG-Lite state with LV_ASSERT(). Useful while debugging. */
#define LV_VG_LITE_USE_ASSERT 0

/** The GPU tries to batch this many draw tasks before flushing. */
#define LV_VG_LITE_FLUSH_MAX_COUNT 8

/** Usually faster, but does not match the rendering quality of the SW renderer. */
#define LV_VG_LITE_USE_BOX_SHADOW 1

/** The memory usage of a single gradient:
 *          linear: 4K bytes.
 *          radial: radius * 4K bytes.
 */
#define LV_VG_LITE_GRAD_CACHE_CNT 32

/** Stroke cache size (entries) */
#define LV_VG_LITE_STROKE_CACHE_CNT 32

/** Unaligned bitmap font cache size (entries) */
#define LV_VG_LITE_BITMAP_FONT_CACHE_CNT 256

/** Remove VLC_OP_CLOSE path instruction (Workaround for NXP) */
#define LV_VG_LITE_DISABLE_VLC_OP_CLOSE 0

/** Needed with older VG-Lite drivers that do not implement the extension. */
#define LV_VG_LITE_DISABLE_LINEAR_GRADIENT_EXT 0

/** Works around HW errors on some GPUs at the cost of extra blits. */
#define LV_VG_LITE_DISABLE_BLIT_RECT_OFFSET 0

/** Maximum path dump length (points) */
#define LV_VG_LITE_PATH_DUMP_MAX_LEN 1000

/** Build the VG-Lite driver bundled with LVGL instead of linking a vendor-provided one. */
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

/** GPU base address */
#define LV_VG_LITE_HAL_GPU_BASE_ADDRESS 0x40240000

#endif /*LV_USE_VG_LITE_DRIVER*/

/** Simulate VG-Lite hardware with ThorVG for debugging and testing on a PC simulator.
 *  Requires the internal or an external ThorVG library.
 *
 *  Enable: LV_USE_THORVG
 */
#define LV_USE_VG_LITE_THORVG 0

#if LV_USE_VG_LITE_THORVG
/** LVGL blend modes */
#define LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT 0

/** YUV color formats */
#define LV_VG_LITE_THORVG_YUV_SUPPORT 0

/** Linear gradient extension */
#define LV_VG_LITE_THORVG_LINEAR_GRADIENT_EXT_SUPPORT 0

/** 16 pixel alignment */
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 1

/** Buffer address alignment (bytes) */
#define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64

/** Multi-threaded rendering */
#define LV_VG_LITE_THORVG_THREAD_RENDER 0

#endif /*LV_USE_VG_LITE_THORVG*/
#endif /*LV_USE_DRAW_VG_LITE*/

/** Accelerate drawing with the Dave2D GPU found on Renesas RA platforms. */
#define LV_USE_DRAW_DAVE2D 0

/** Accelerate drawing with the Think Silicon (TSi) NemaGFX GPU library. */
#define LV_USE_NEMA_GFX 0

#if LV_USE_NEMA_GFX
/** Clean and invalidate the CPU cache around draw buffers so the GPU sees
 *  coherent data. Needed on cores with a data cache.
 */
#define LV_NEMA_USE_CACHE 0

#if LV_NEMA_USE_CACHE
/** Cache HAL header */
#define LV_NEMA_CACHE_HAL_INCLUDE "stm32u5xx_hal.h"

#endif /*LV_NEMA_USE_CACHE*/

/** Bundled NemaGFX library and headers to use, per target CPU core.
 *  None warns at build time and falls back to Cortex-M33 (Rev C).
 *  Possible values:
 *  - LV_NEMA_LIB_NONE
 *  - LV_NEMA_LIB_M33_REVC: Cortex-M33 (Rev C)
 *  - LV_NEMA_LIB_M33_NEMAPVG: Cortex-M33 (NemaPVG)
 *  - LV_NEMA_LIB_M55: Cortex-M55
 *  - LV_NEMA_LIB_M7: Cortex-M7
 */
#define LV_USE_NEMA_LIB LV_NEMA_LIB_NONE

/** STM32 uses the HAL bundled with LVGL. Custom means the application
 *  provides the NemaGFX HAL (nema_hal.c) itself.
 *  Possible values:
 *  - LV_NEMA_HAL_CUSTOM
 *  - LV_NEMA_HAL_STM32
 */
#define LV_USE_NEMA_HAL LV_NEMA_HAL_CUSTOM

#if LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32
/** STM32 HAL header */
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

/** Draw vector graphics (lv_vector_dsc) with NemaVG on top of NemaGFX. */
#define LV_USE_NEMA_VG 0

/** Canvas width NemaVG is initialized for. With the vertical resolution it also
 *  sizes the NemaGFX memory pool on STM32 (RESX * RESY + 10KB).
 */
#define LV_NEMA_GFX_MAX_RESX 800

/** Canvas height NemaVG is initialized for. */
#define LV_NEMA_GFX_MAX_RESY 600

#endif /*LV_USE_NEMA_GFX*/

/** Accelerate blends, fills and transforms with the PXP (Pixel Pipeline)
 *  engine of NXP i.MX RT SoCs.
 */
#define LV_USE_DRAW_PXP 0

#if LV_USE_DRAW_PXP
/** Rotate the rendered display buffer with the PXP instead of the CPU. */
#define LV_USE_ROTATE_PXP 0

#if LV_USE_OS != LV_OS_NONE
/** Dispatch PXP operations from their own thread so the CPU can keep
 *  rendering in parallel. Needs an OS.
 */
#define LV_USE_PXP_DRAW_THREAD 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Check the status of every PXP call and assert on failure. Useful while
 *  bringing up a board.
 */
#define LV_USE_PXP_ASSERT 0

#endif /*LV_USE_DRAW_PXP*/

/** Accelerate blends, fills and transforms with the PPA (Pixel Processing
 *  Accelerator) peripheral of Espressif SoCs.
 */
#define LV_USE_PPA 0

#if LV_USE_PPA
/** Let the PPA also draw images. Only simple cases qualify: unscaled,
 *  unrotated, fully opaque and without masks.
 */
#define LV_USE_PPA_IMG 0

#define LV_PPA_BURST_LENGTH 128

#endif /*LV_USE_PPA*/

/** Accelerate blends, fills and image decoding with the STM32 DMA2D peripheral. */
#define LV_USE_DRAW_DMA2D 0

#if LV_USE_DRAW_DMA2D
/** Header that declares the DMA2D HAL API of the target STM32 family. */
#define LV_DRAW_DMA2D_HAL_INCLUDE "stm32h7xx_hal.h"

/** Run DMA2D transfers asynchronously instead of waiting for them (needs an OS).
 *  Call lv_draw_dma2d_transfer_complete_interrupt_handler() from the DMA2D global ISR.
 */
#define LV_USE_DRAW_DMA2D_INTERRUPT 0

#endif /*LV_USE_DRAW_DMA2D*/

/** Offload drawing to an external EVE (FT81X/BT81X) graphics controller over SPI. */
#define LV_USE_DRAW_EVE 0

#if LV_USE_DRAW_EVE
/** Generation of the connected controller: 2 = FT81x, 3 = BT815/BT816,
 *  4 = BT817/BT818. Higher generations expose more commands and registers.
 */
#define LV_DRAW_EVE_EVE_GENERATION 4

/** Bytes buffered before each SPI transmission. 0 disables buffering;
 *  values below 4 are treated as 0.
 */
#define LV_DRAW_EVE_WRITE_BUFFER_SIZE 2048

#endif /*LV_USE_DRAW_EVE*/

/** Accelerate blends, fills and image blits with the NXP G2D API (i.MX 2D GPU).
 *  Requires the g2d library and its headers.
 */
#define LV_USE_DRAW_G2D 0

#if LV_USE_DRAW_G2D
/** Maximum number of buffers mapped for the G2D draw unit, including frame
 *  buffers and assets. Drawing asserts once the table is full.
 */
#define LV_G2D_HASH_TABLE_SIZE 50

#if LV_USE_OS != LV_OS_NONE
/** Dispatch G2D operations from their own thread so the CPU can keep
 *  rendering in parallel. Needs an OS.
 */
#define LV_USE_G2D_DRAW_THREAD 1

#endif /*LV_USE_OS != LV_OS_NONE*/

/** Check the status of every G2D call and assert on failure. Useful while
 *  bringing up a board.
 */
#define LV_USE_G2D_ASSERT 0

#endif /*LV_USE_DRAW_G2D*/

/** Draw with the NanoVG vector library on top of OpenGL / OpenGL ES shaders.
 *
 *  Enable: LV_USE_NANOVG, LV_USE_OPENGLES, LV_USE_MATRIX
 */
#define LV_USE_DRAW_NANOVG 0

#if LV_USE_DRAW_NANOVG
/** NanoVG shader flavor to compile. Must match the OpenGL context the
 *  display driver creates.
 *  Possible values:
 *  - LV_NANOVG_BACKEND_GL2: OpenGL 2.0
 *  - LV_NANOVG_BACKEND_GL3: OpenGL 3.0+
 *  - LV_NANOVG_BACKEND_GLES2: OpenGL ES 2.0
 *  - LV_NANOVG_BACKEND_GLES3: OpenGL ES 3.0+
 */
#define LV_NANOVG_BACKEND LV_NANOVG_BACKEND_GLES2

/** Number of decoded images kept as GPU textures. */
#define LV_NANOVG_IMAGE_CACHE_CNT 128

/** Number of rendered glyphs kept for text drawing. */
#define LV_NANOVG_LETTER_CACHE_CNT 512

#endif /*LV_USE_DRAW_NANOVG*/

/** Draw with OpenGL ES, keeping rendered widgets and images as GPU textures.
 *
 *  Enable: LV_USE_OPENGLES
 */
#define LV_USE_DRAW_OPENGLES 0

#if LV_USE_DRAW_OPENGLES
/** Number of textures kept in the cache. More entries mean fewer redraws
 *  but more GPU memory.
 */
#define LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT 64

#endif /*LV_USE_DRAW_OPENGLES*/

/** Render with the SDL renderer API, caching widgets and images as SDL textures. */
#define LV_USE_DRAW_SDL 0



/*============================================================================
 * INPUT DEVICES
 *============================================================================*/

/** Move focus between a container's children with arrow keys, based on their position. */
#define LV_USE_GRIDNAV 0

/** Detect pinch and rotate gestures from multi-touch input. Requires float support.
 *
 *  Enable: LV_USE_FLOAT
 */
#define LV_USE_GESTURE_RECOGNITION 0



/*============================================================================
 * CORE
 *============================================================================*/

/** Speed up style property lookups by adding 2 x 32 bit variables to each lv_obj_t. */
#define LV_OBJ_STYLE_CACHE 0

/** Widget names (lv_obj_set_name) */
#define LV_USE_OBJ_NAME 0

/** Widget id (lv_obj_set_id) */
#define LV_USE_OBJ_ID 0

#if LV_USE_OBJ_ID
/** Automatic ID assignment on widget creation */
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

/** Get and set any widget style or attribute through a single generic property ID based API. */
#define LV_USE_OBJ_PROPERTY 0

#if LV_USE_OBJ_PROPERTY
/** Add a name table to every widget class, so the property can be accessed by name.
 *  Note, the const table will increase flash usage.
 */
#define LV_USE_OBJ_PROPERTY_NAME 1

#endif /*LV_USE_OBJ_PROPERTY*/

/** Attach your own data plus a destructor callback to events, widgets, observers,
 *  animations, timers, groups, displays, input devices and themes, so your resources
 *  are cleaned up when LVGL frees them.
 */
#define LV_USE_EXT_DATA 0

/** Use `float` instead of `int32_t` for coordinates and values that need sub-pixel precision. */
#define LV_USE_FLOAT 0

/** Bind widgets to subject variables so they update automatically when the value changes. */
#define LV_USE_OBSERVER 1

/** Look up strings by ID for the selected language with lv_translation_get(). */
#define LV_USE_TRANSLATION 0

/** Color filter style */
#define LV_USE_COLOR_FILTER 0



/*============================================================================
 * LOGGING
 *============================================================================*/

/** Logging */
#define LV_USE_LOG 0

#if LV_USE_LOG
/** Default log verbosity
 *  Possible values:
 *  - LV_LOG_LEVEL_TRACE: detailed information about everything
 *  - LV_LOG_LEVEL_INFO: important events
 *  - LV_LOG_LEVEL_WARN: something unwanted happened but caused no problem
 *  - LV_LOG_LEVEL_ERROR: critical issues where the system may fail
 *  - LV_LOG_LEVEL_USER: only logs added by the user
 *  - LV_LOG_LEVEL_NONE: nothing is logged
 */
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/** If not set, the user needs to register a callback with lv_log_register_print_cb(). */
#define LV_LOG_PRINTF 0

/** Add a timestamp to each log */
#define LV_LOG_USE_TIMESTAMP 1

/** Add the file name and line number to each log */
#define LV_LOG_USE_FILE_LINE 1

/** Memory */
#define LV_LOG_TRACE_MEM 1

/** Timer */
#define LV_LOG_TRACE_TIMER 1

/** Input devices */
#define LV_LOG_TRACE_INDEV 1

/** Display refresh */
#define LV_LOG_TRACE_DISP_REFR 1

/** Events */
#define LV_LOG_TRACE_EVENT 1

/** Widget creation */
#define LV_LOG_TRACE_OBJ_CREATE 1

/** Layout */
#define LV_LOG_TRACE_LAYOUT 1

/** Animations */
#define LV_LOG_TRACE_ANIM 1

/** Cache */
#define LV_LOG_TRACE_CACHE 1

#endif /*LV_USE_LOG*/


/*============================================================================
 * THEMES
 *============================================================================*/

/** Full-featured theme used by LVGL's demos and examples. */
#define LV_USE_THEME_DEFAULT 1

#if LV_USE_THEME_DEFAULT
/** Dark mode */
#define LV_THEME_DEFAULT_DARK 0

/** Grow on press */
#define LV_THEME_DEFAULT_GROW 1

/** Transition time (ms) */
#define LV_THEME_DEFAULT_TRANSITION_TIME 80

#endif /*LV_USE_THEME_DEFAULT*/

/** Minimal styling, a good starting point for a custom theme. */
#define LV_USE_THEME_SIMPLE 1

/** Styling for 1-bit displays such as e-paper and dot-matrix panels. */
#define LV_USE_THEME_MONO 1



/*============================================================================
 * LAYOUTS
 *============================================================================*/

/** Arrange children in a row or column that wraps and grows, similar to Flexbox in CSS. */
#define LV_USE_FLEX 1

/** Arrange children in rows and columns, similar to Grid in CSS. */
#define LV_USE_GRID 1



/*============================================================================
 * IMAGE SETTINGS
 *============================================================================*/

/** Avoids repeatedly opening and decoding the same images, at the cost of RAM.
 *  Of little benefit with only the built-in image formats.
 */
#define LV_CACHE_DEF_SIZE 0

/** Avoids repeatedly reading image headers, at the cost of RAM.
 *  Of little benefit with only the built-in image formats.
 */
#define LV_IMAGE_HEADER_CACHE_DEF_CNT 0

/** Decoder for LVGL's run-length encoded binary image format. */
#define LV_USE_RLE 0

/** LZ4 compress/decompress functions, used by LVGL's compressed binary images. */
#define LV_USE_LZ4 0

#if LV_USE_LZ4
/** Bundled LZ4 */
#define LV_USE_LZ4_INTERNAL 1

#endif /*LV_USE_LZ4*/

/** Decode binary images to RAM */
#define LV_BIN_DECODER_RAM_LOAD 0

/** PNG decoder (LodePNG) */
#define LV_USE_LODEPNG 0

/** PNG decoder (libpng) */
#define LV_USE_LIBPNG 0

/** BMP decoder */
#define LV_USE_BMP 0

/** Also decodes split JPEG, a custom format optimized for embedded systems. */
#define LV_USE_TJPGD 0

/** High-performance decoder supporting the complete JPEG specifications. */
#define LV_USE_LIBJPEG_TURBO 0

/** WebP decoder */
#define LV_USE_LIBWEBP 0

/** SVG
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_SVG 0

#if LV_USE_SVG
/** SVG animation */
#define LV_USE_SVG_ANIMATION 0

/** SVG debug logs */
#define LV_USE_SVG_DEBUG 0

#endif /*LV_USE_SVG*/


/*============================================================================
 * TEXT & FONT SETTINGS
 *============================================================================*/

/** Your IDE or editor should be set to the same character encoding.
 *  Possible values:
 *  - LV_TXT_ENC_UTF8
 *  - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

/** Characters to break (wrap) text on */
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

/** Text recoloring control character */
#define LV_TXT_COLOR_CMD "#"

/** Allows mixing Left-to-Right and Right-to-Left texts.
 *  The direction will be processed according to the Unicode Bidirectional Algorithm:
 *  https://www.w3.org/International/articles/inline-bidi-markup/uba-basics
 */
#define LV_USE_BIDI 0

#if LV_USE_BIDI
/** Default base direction
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

/** Montserrat 10 */
#define LV_FONT_MONTSERRAT_10 0

/** Montserrat 12 */
#define LV_FONT_MONTSERRAT_12 0

/** Montserrat 14 */
#define LV_FONT_MONTSERRAT_14 1

/** Montserrat 16 */
#define LV_FONT_MONTSERRAT_16 0

/** Montserrat 18 */
#define LV_FONT_MONTSERRAT_18 0

/** Montserrat 20 */
#define LV_FONT_MONTSERRAT_20 0

/** Montserrat 22 */
#define LV_FONT_MONTSERRAT_22 0

/** Montserrat 24 */
#define LV_FONT_MONTSERRAT_24 0

/** Montserrat 26 */
#define LV_FONT_MONTSERRAT_26 0

/** Montserrat 28 */
#define LV_FONT_MONTSERRAT_28 0

/** Montserrat 30 */
#define LV_FONT_MONTSERRAT_30 0

/** Montserrat 32 */
#define LV_FONT_MONTSERRAT_32 0

/** Montserrat 34 */
#define LV_FONT_MONTSERRAT_34 0

/** Montserrat 36 */
#define LV_FONT_MONTSERRAT_36 0

/** Montserrat 38 */
#define LV_FONT_MONTSERRAT_38 0

/** Montserrat 40 */
#define LV_FONT_MONTSERRAT_40 0

/** Montserrat 42 */
#define LV_FONT_MONTSERRAT_42 0

/** Montserrat 44 */
#define LV_FONT_MONTSERRAT_44 0

/** Montserrat 46 */
#define LV_FONT_MONTSERRAT_46 0

/** Montserrat 48 */
#define LV_FONT_MONTSERRAT_48 0

/** Montserrat 28 (compressed) */
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0

/** Dejavu 16 (Persian, Hebrew, Arabic) */
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0

/** SourceHanSansSC 14 (CJK) */
#define LV_FONT_SOURCE_HAN_SANS_SC_14_CJK 0

/** SourceHanSansSC 16 (CJK) */
#define LV_FONT_SOURCE_HAN_SANS_SC_16_CJK 0

/** Pixel perfect monospaced fonts */
#define LV_FONT_UNSCII_8 0

/** UNSCII 16 */
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
/** Font used by the themes and by widgets that do not set one explicitly.
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
 *  - LV_FONT_DEFAULT_DEJAVU_16_PERSIAN_HEBREW: Dejavu 16 (Persian, Hebrew, Arabic) (enable: LV_FONT_DEJAVU_16_PERSIAN_HEBREW)
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_14_CJK (enable: LV_FONT_SOURCE_HAN_SANS_SC_14_CJK)
 *  - LV_FONT_DEFAULT_SOURCE_HAN_SANS_SC_16_CJK (enable: LV_FONT_SOURCE_HAN_SANS_SC_16_CJK)
 *  - LV_FONT_DEFAULT_UNSCII_8 (enable: LV_FONT_UNSCII_8)
 *  - LV_FONT_DEFAULT_UNSCII_16 (enable: LV_FONT_UNSCII_16)
 */
#define LV_FONT_DEFAULT LV_FONT_DEFAULT_MONTSERRAT_14

#endif /*!LV_USE_CUSTOM_FONT_DEFAULT*/

/** Use larger internal offsets so fonts with many glyphs stay addressable.
 *  The exact limit depends on font size, face and format, but enable it if
 *  you see issues with fonts above roughly 10,000 characters.
 */
#define LV_FONT_FMT_TXT_LARGE 0

/** Compressed fonts */
#define LV_USE_FONT_COMPRESSED 0

/** Draw a placeholder rectangle instead of nothing when a glyph is missing
 *  from the font.
 */
#define LV_USE_FONT_PLACEHOLDER 1

/** Load fonts by name, share them between widgets and free them by reference
 *  counting.
 */
#define LV_USE_FONT_MANAGER 0

#if LV_USE_FONT_MANAGER
/** Font name max length (characters) */
#define LV_FONT_MANAGER_NAME_MAX_LEN 32

#endif /*LV_USE_FONT_MANAGER*/

/** Use images as glyphs in label and span widgets, e.g. to render emojis. */
#define LV_USE_IMGFONT 0

/** FreeType */
#define LV_USE_FREETYPE 0

#if LV_USE_FREETYPE
/** When enabled, FreeType will use LVGL's memory allocator and file system
 *  abstraction instead of the platform defaults.
 */
#define LV_FREETYPE_USE_LVGL_PORT 0

/** Glyph cache size (glyphs) */
#define LV_FREETYPE_CACHE_FT_GLYPH_CNT 256

#if LV_USE_OS == LV_OS_NONE
/** L1 glyph metrics cache (lock-free, single-thread only) */
#define LV_FREETYPE_CACHE_FT_GLYPH_L1 1

#endif /*LV_USE_OS == LV_OS_NONE*/
#endif /*LV_USE_FREETYPE*/

/** Built-in TTF decoder */
#define LV_USE_TINY_TTF 0

#if LV_USE_TINY_TTF
/** Load TTF data from files */
#define LV_TINY_TTF_FILE_SUPPORT 0

/** Glyph cache size (glyphs) */
#define LV_TINY_TTF_CACHE_GLYPH_CNT 128

/** Kerning cache size (entries) */
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

/** 3D texture */
#define LV_USE_3DTEXTURE 0

/** Animated image */
#define LV_USE_ANIMIMG 1

/** Arc */
#define LV_USE_ARC 1

/** Arc label */
#define LV_USE_ARCLABEL 1

/** Bar */
#define LV_USE_BAR 1

/** Generates Code 128 barcodes.
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
/** Week starts on Monday */
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

/** Header with arrow buttons */
#define LV_USE_CALENDAR_HEADER_ARROW 1

/** Header with dropdowns */
#define LV_USE_CALENDAR_HEADER_DROPDOWN 1

/** Chinese calendar */
#define LV_USE_CALENDAR_CHINESE 0

#endif /*LV_USE_CALENDAR*/

/** Canvas
 *
 *  Enable: LV_USE_IMAGE
 */
#define LV_USE_CANVAS 1

/** Chart */
#define LV_USE_CHART 1

/** Checkbox */
#define LV_USE_CHECKBOX 1

/** Dropdown
 *
 *  Enable: LV_USE_LABEL
 */
#define LV_USE_DROPDOWN 1

/** Decodes images and plays videos with FFmpeg. It supports all major image
 *  formats, so do not enable other image decoders together with it.
 */
#define LV_USE_FFMPEG 0

#if LV_USE_FFMPEG
/** Print the input stream information to stderr and keep FFmpeg logging
 *  enabled (it is silenced otherwise).
 */
#define LV_FFMPEG_DUMP_FORMAT 0

/** URLs can no longer be opened once this is enabled. The FFmpeg image
 *  decoder always uses the LVGL file system regardless of this option.
 */
#define LV_FFMPEG_PLAYER_USE_LV_FS 0

#endif /*LV_USE_FFMPEG*/

/** GIF decoder */
#define LV_USE_GIF 0

#if LV_USE_GIF
/** Speeds up playback at the cost of an extra 16KB of RAM. */
#define LV_GIF_CACHE_DECODE_DATA 0

/** Wider GIFs are rejected; raising this value increases memory usage. */
#define LV_GIF_MAX_WIDTH 480

/** Taller GIFs are rejected; this value has no impact on memory usage. */
#define LV_GIF_MAX_HEIGHT 32768

#endif /*LV_USE_GIF*/

/** Parses and renders 3D models (.gltf/.glb).
 *
 *  Enable: LV_USE_3DTEXTURE
 */
#define LV_USE_GLTF 0

/** Requires the gstreamer-1.0, gstreamer-video-1.0 and gstreamer-app-1.0
 *  development libraries.
 *
 *  Enable: LV_USE_IMAGE
 */
#define LV_USE_GSTREAMER 0

/** Image */
#define LV_USE_IMAGE 1

/** Image button */
#define LV_USE_IMAGEBUTTON 1

/** Chinese input method that converts Pinyin typed on a keyboard widget
 *  into Chinese characters.
 *
 *  Enable: LV_USE_KEYBOARD
 */
#define LV_USE_IME_PINYIN 0

#if LV_USE_IME_PINYIN
/** 9-key input mode */
#define LV_IME_PINYIN_USE_K9_MODE 1

#if LV_IME_PINYIN_USE_K9_MODE
/** Maximum number of candidate panels for 9-key input mode */
#define LV_IME_PINYIN_K9_CAND_TEXT_NUM 3

#endif /*LV_IME_PINYIN_USE_K9_MODE*/

/** Without it, a dictionary must be set with lv_ime_pinyin_set_dict()
 *  before the IME is used.
 */
#define LV_IME_PINYIN_USE_DEFAULT_DICT 1

/** Adjust this to the screen size, as all candidate panels must fit on it. */
#define LV_IME_PINYIN_CAND_TEXT_NUM 6

#endif /*LV_USE_IME_PINYIN*/

/** Keyboard */
#define LV_USE_KEYBOARD 1

/** Label */
#define LV_USE_LABEL 1

#if LV_USE_LABEL
/** Text selection */
#define LV_LABEL_TEXT_SELECTION 1

/** Stores 12 extra bytes per label to speed up drawing of very long texts. */
#define LV_LABEL_LONG_TXT_HINT 1

/** Gap between the end and the restart of circularly scrolled text,
 *  measured in space-character widths.
 */
#define LV_LABEL_WAIT_CHAR_COUNT 3

#endif /*LV_USE_LABEL*/

/** LED */
#define LV_USE_LED 1

/** Line */
#define LV_USE_LINE 1

/** List */
#define LV_USE_LIST 1

#if LV_USE_THORVG
/** Plays Lottie JSON animations using the ThorVG Lottie parser.
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_LOTTIE 0

#endif /*LV_USE_THORVG*/

/** Menu */
#define LV_USE_MENU 1

/** Message box */
#define LV_USE_MSGBOX 1

/** QR code
 *
 *  Enable: LV_USE_CANVAS
 */
#define LV_USE_QRCODE 0

/** Legacy rlottie binding, kept for compatibility. Use LV_USE_LOTTIE instead. */
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
/** Maximum number of span descriptors */
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
/** Default password show time (ms) */
#define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500

#endif /*LV_USE_TEXTAREA*/

/** Table */
#define LV_USE_TABLE 1

/** Tabview */
#define LV_USE_TABVIEW 1

/** Tileview */
#define LV_USE_TILEVIEW 1

/** Window */
#define LV_USE_WIN 1



/*============================================================================
 * DRIVERS
 *============================================================================*/

/** Driver for /dev/dri/card */
#define LV_USE_LINUX_DRM 0

#if LV_USE_LINUX_DRM
/** Legacy behavior, slated for removal: the backend is inferred from
 *  LV_USE_OPENGLES (EGL when it is enabled). Disable this and pick a backend
 *  explicitly in the "Rendering backend" choice.
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
/** BSD-flavored framebuffer device */
#define LV_LINUX_FBDEV_BSD 0

/** Render mode
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full mode
 */
#define LV_LINUX_FBDEV_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL

#define LV_LINUX_FBDEV_BUFFER_COUNT 1

#if LV_LINUX_FBDEV_BUFFER_COUNT == 0
/** Custom partial buffer size (rows) */
#define LV_LINUX_FBDEV_BUFFER_SIZE 60

#endif /*LV_LINUX_FBDEV_BUFFER_COUNT == 0*/

/** Access the framebuffer through mmap() instead of write() calls. */
#define LV_LINUX_FBDEV_MMAP 1

#endif /*LV_USE_LINUX_FBDEV*/

/** Driver for FT81X EVE graphics controllers connected over SPI. */
#define LV_USE_FT81X 0

/** Display and touch driver built on the LovyanGFX C++ library, which must be available in the build. */
#define LV_USE_LOVYAN_GFX 0

#if LV_USE_LOVYAN_GFX
/** Header for LovyanGFX user configuration */
#define LV_LGFX_USER_INCLUDE "lv_lgfx_user.hpp"

#endif /*LV_USE_LOVYAN_GFX*/

/** Driver for LCD devices connected via SPI or parallel port.
 *  Use it if your LCD controller is not listed among the dedicated drivers below.
 */
#define LV_USE_GENERIC_MIPI 0

/** ST7735
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7735 0

/** ST7789
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7789 0

/** ST7796
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ST7796 0

/** ILI9341
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_ILI9341 0

/** NV3007
 *
 *  Enable: LV_USE_GENERIC_MIPI
 */
#define LV_USE_NV3007 0

/** Create a display on an already initialized NXP ELCDIF LCD controller. */
#define LV_USE_NXP_ELCDIF 0

/** Create a display driven by the Renesas GLCDC peripheral. */
#define LV_USE_RENESAS_GLCDC 0

/** Create a display bound to a layer of the ST LTDC peripheral. */
#define LV_USE_ST_LTDC 0

#if !LV_USE_DRAW_DMA2D
#if LV_USE_ST_LTDC
/** Flush partial mode LTDC displays with DMA2D, in parallel with rendering.
 *  Only available when the DMA2D renderer is disabled.
 */
#define LV_ST_LTDC_USE_DMA2D_FLUSH 0

#endif /*LV_USE_ST_LTDC*/
#endif /*!LV_USE_DRAW_DMA2D*/

/** Display driver built on the TFT_eSPI Arduino library, which must be available in the build. */
#define LV_USE_TFT_ESPI 0

/** Read touchscreen, mouse and keyboard input from Linux /dev/input event devices. */
#define LV_USE_EVDEV 0

/** Read pointer, touch and keyboard input through libinput, which must be available in the build. */
#define LV_USE_LIBINPUT 0

#if LV_USE_LIBINPUT
/** BSD variant of libinput */
#define LV_LIBINPUT_BSD 0

/** Map key events to characters with XKB layouts, so modifiers and non-US layouts work.
 *  Requires linking with -lxkbcommon.
 */
#define LV_LIBINPUT_XKB 0

#if LV_LIBINPUT_XKB
/** Run `setxkbmap -query` to find the right values for your keyboard. */
#define LV_LIBINPUT_XKB_RULES ""

/** XKB model */
#define LV_LIBINPUT_XKB_MODEL "pc101"

/** XKB layout */
#define LV_LIBINPUT_XKB_LAYOUT "us"

/** XKB variant */
#define LV_LIBINPUT_XKB_VARIANT ""

/** Pass NULL for the XKB options so the system default applies (e.g. from the
 *  XKB_DEFAULT_OPTIONS env var). Otherwise LV_LIBINPUT_XKB_OPTIONS is used.
 */
#define LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT 1

#if !LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT
/** XKB options */
#define LV_LIBINPUT_XKB_OPTIONS ""

#endif /*!LV_LIBINPUT_XKB_OPTIONS_USE_DEFAULT*/
#endif /*LV_LIBINPUT_XKB*/
#endif /*LV_USE_LIBINPUT*/

/** Display and input drivers for NuttX, including the /dev/fb framebuffer device. */
#define LV_USE_NUTTX 0

#if LV_USE_NUTTX
/** Allocate cached image buffers from a separate NuttX heap to keep the main heap from fragmenting. */
#define LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP 0

#if LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP
/** Independent image heap for the default draw buffer */
#define LV_NUTTX_DEFAULT_DRAW_BUF_USE_INDEPENDENT_IMAGE_HEAP 0

#endif /*LV_USE_NUTTX_INDEPENDENT_IMAGE_HEAP*/

/** Drive the LVGL timer handler and the display/input timers from a libuv loop instead of the default timer loop. */
#define LV_USE_NUTTX_LIBUV 0

/** Skip the display and input device creation in lv_nuttx_init() so the application can set them up itself. */
#define LV_USE_NUTTX_CUSTOM_INIT 0

/** Create the display on a NuttX LCD character device instead of the framebuffer device. */
#define LV_USE_NUTTX_LCD 0

#define LV_NUTTX_LCD_BUFFER_COUNT 0

#if LV_NUTTX_LCD_BUFFER_COUNT == 0
#if LV_USE_NUTTX_LCD
/** Custom partial buffer size (rows) */
#define LV_NUTTX_LCD_BUFFER_SIZE 60

#endif /*LV_USE_NUTTX_LCD*/
#endif /*LV_NUTTX_LCD_BUFFER_COUNT == 0*/

/** Touchscreen */
#define LV_USE_NUTTX_TOUCHSCREEN 0

#if LV_USE_NUTTX_TOUCHSCREEN
/** 0 disables the cursor. */
#define LV_NUTTX_TOUCHSCREEN_CURSOR_SIZE 0

#endif /*LV_USE_NUTTX_TOUCHSCREEN*/

/** Mouse */
#define LV_USE_NUTTX_MOUSE 0

#if LV_USE_NUTTX_MOUSE
/** Mouse movement step (pixels) */
#define LV_USE_NUTTX_MOUSE_MOVE_STEP 1

#endif /*LV_USE_NUTTX_MOUSE*/

#if LV_USE_PROFILER_BUILTIN
/** Write the built-in profiler output to a file instead of the console. */
#define LV_USE_NUTTX_TRACE_FILE 0

#endif /*LV_USE_PROFILER_BUILTIN*/

#if LV_USE_NUTTX_TRACE_FILE
/** Profiler trace file path */
#define LV_NUTTX_TRACE_FILE_PATH "/data/lvgl-trace.log"

#endif /*LV_USE_NUTTX_TRACE_FILE*/
#endif /*LV_USE_NUTTX*/

/** Generic OpenGL ES display driver, for embedding LVGL in another application or driving it through GLFW/EGL.
 *
 *  Enable: LV_USE_MATRIX
 */
#define LV_USE_OPENGLES 0

#if LV_USE_OPENGLES
/** Check for an OpenGL error after every GL call and log it with the call site. */
#define LV_USE_OPENGLES_DEBUG 0

#endif /*LV_USE_OPENGLES*/

#if !LV_USE_EGL
/** Open a window on a PC desktop with GLFW and read mouse and keyboard input.
 *
 *  Enable: LV_USE_OPENGLES
 */
#define LV_USE_GLFW 0

#endif /*!LV_USE_EGL*/

/** Create a display on a QNX Screen window and read pointer and keyboard input. */
#define LV_USE_QNX 0

#if LV_USE_QNX
/** Number of screen-sized buffers */
#define LV_QNX_BUF_COUNT 1

#endif /*LV_USE_QNX*/

/** Open a window on a PC desktop with SDL2 and read mouse and keyboard input. */
#define LV_USE_SDL 0

#if LV_USE_SDL
/** SDL include path */
#define LV_SDL_INCLUDE_PATH "SDL2/SDL.h"

/** Legacy behavior, slated for removal: EGL is inferred the legacy way
 *  (LV_USE_OPENGLES with an OpenGL draw unit). Disable this and pick a backend
 *  explicitly in the "Rendering backend" choice.
 */
#define LV_SDL_AUTO_BACKEND 1

/** Select how the SDL driver presents rendered frames.
 *  Possible values:
 *  - LV_SDL_BACKEND_SW: Software (SDL surface)
 *  - LV_SDL_BACKEND_TEXTURE: Cached SDL textures (enable: LV_USE_DRAW_SDL)
 *  - LV_SDL_BACKEND_EGL: EGL (OpenGL ES, hardware-accelerated) (requires LV_USE_DRAW_OPENGLES || LV_USE_DRAW_NANOVG)
 */
#define LV_SDL_BACKEND LV_SDL_BACKEND_SW

/** Direct mode is recommended for best performance.
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full mode
 */
#define LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT

/** Number of screen-sized buffers */
#define LV_SDL_BUF_COUNT 1

/** Ask SDL for a GPU-backed renderer instead of a software one. */
#define LV_SDL_ACCELERATED 1

/** Fullscreen */
#define LV_SDL_FULLSCREEN 0

/** Exit when all windows are closed */
#define LV_SDL_DIRECT_EXIT 1

/** SDL mousewheel mode
 *  Possible values:
 *  - LV_SDL_MOUSEWHEEL_MODE_ENCODER: Encoder input device
 *  - LV_SDL_MOUSEWHEEL_MODE_CROWN: Smart watch crown
 */
#define LV_SDL_MOUSEWHEEL_MODE LV_SDL_MOUSEWHEEL_MODE_ENCODER

#endif /*LV_USE_SDL*/

/** Display and input drivers for UEFI firmware, based on the Graphics Output, keyboard and pointer protocols. */
#define LV_USE_UEFI 0

#if LV_USE_UEFI
/** Header wrapping the UEFI framework (EDK2, gnu-efi, ...) */
#define LV_USE_UEFI_INCLUDE "myefi.h"

/** Allocate memory with the UEFI boot services instead of the C library.
 *  Only takes effect with the custom stdlib malloc implementation.
 */
#define LV_UEFI_USE_MEMORY_SERVICES 0

#endif /*LV_USE_UEFI*/

/** Open a window on a Wayland compositor on Linux or BSD and read mouse, keyboard and touch input. */
#define LV_USE_WAYLAND 0

#if LV_USE_WAYLAND
/** LVGL is deinitialized before the application exits. */
#define LV_WAYLAND_DIRECT_EXIT 1

#if !LV_USE_DRAW_OPENGLES
#if !LV_USE_DRAW_NANOVG
/** Default backend, using wl_shm for double-buffered direct rendering.
 *  Compatible with all Wayland compositors; no special hardware required.
 *  Unavailable with the OpenGL ES and NanoVG renderers, which can only
 *  render into a GPU surface.
 */
#define LV_WAYLAND_USE_SHM 1

/** Presents software-rendered frames through linear DMA-BUFs that the
 *  compositor can scan out directly, saving its copy out of shared memory.
 *  Requires a compositor with zwp_linux_dmabuf_v1 and a gbm implementation
 *  able to allocate CPU-mappable buffers, plus libdrm and gbm (-ldrm -lgbm).
 *  Needs no GPU API: unavailable with the OpenGL ES and NanoVG renderers,
 *  which can only render into a GPU surface.
 */
#define LV_WAYLAND_USE_DMABUF 0

#endif /*!LV_USE_DRAW_NANOVG*/
#endif /*!LV_USE_DRAW_OPENGLES*/

/** Hardware-accelerated rendering via OpenGL ES 2.0 and EGL, compatible
 *  with LVGL 3D/glTF rendering. Requires OpenGL ES 2.0 on the target
 *  hardware and linking with wayland-egl (-lwayland-egl).
 *  The only backend available with the OpenGL ES and NanoVG renderers.
 *
 *  Enable: LV_USE_OPENGLES
 */
#define LV_WAYLAND_USE_EGL 0

#if !LV_USE_DRAW_OPENGLES
#if !LV_USE_DRAW_NANOVG
/** Hardware-accelerated 2D rendering via NXP's G2D engine.
 *  Supports NXP i.MX6/i.MX8 platforms with G2D library installed.
 *  Unavailable with the OpenGL ES and NanoVG renderers, which can only
 *  render into a GPU surface.
 *
 *  Enable: LV_USE_DRAW_G2D
 */
#define LV_WAYLAND_USE_G2D 0

#endif /*!LV_USE_DRAW_NANOVG*/
#endif /*!LV_USE_DRAW_OPENGLES*/
#endif /*LV_USE_WAYLAND*/

#if LV_USE_OS == LV_OS_WINDOWS
/** Open a window with the Win32 API and read mouse, touch, keyboard and mousewheel input. */
#define LV_USE_WINDOWS 0

#endif /*LV_USE_OS == LV_OS_WINDOWS*/

/** Open a window on an X11 desktop and read mouse and keyboard input. */
#define LV_USE_X11 0

#if LV_USE_X11
/** Double buffering */
#define LV_X11_DOUBLE_BUFFER 1

/** Exit when all windows are closed */
#define LV_X11_DIRECT_EXIT 1

/** Render mode
 *  Possible values:
 *  - LV_DISPLAY_RENDER_MODE_PARTIAL: Partial mode
 *  - LV_DISPLAY_RENDER_MODE_DIRECT: Direct mode
 *  - LV_DISPLAY_RENDER_MODE_FULL: Full mode
 */
#define LV_X11_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL

#endif /*LV_USE_X11*/

/** Build the bundled NanoVG vector graphics library. Required by the NanoVG renderer. */
#define LV_USE_NANOVG 0



/*============================================================================
 * FILE SYSTEM
 *============================================================================*/

/** Setting a default drive letter allows skipping the driver
 *  prefix in filepaths.
 */
#define LV_FS_DEFAULT_DRIVER_LETTER 0

/** stdio */
#define LV_USE_FS_STDIO 0

#if LV_USE_FS_STDIO
/** Drive letter for stdio (65 = 'A', 0 = disabled) */
#define LV_FS_STDIO_LETTER 0

/** Working directory for stdio */
#define LV_FS_STDIO_PATH ""

/** Read cache size in bytes for stdio (0 = disabled) */
#define LV_FS_STDIO_CACHE_SIZE 0

#endif /*LV_USE_FS_STDIO*/

/** POSIX */
#define LV_USE_FS_POSIX 0

#if LV_USE_FS_POSIX
/** Drive letter for POSIX (65 = 'A', 0 = disabled) */
#define LV_FS_POSIX_LETTER 0

/** Working directory for POSIX */
#define LV_FS_POSIX_PATH ""

/** Read cache size in bytes for POSIX (0 = disabled) */
#define LV_FS_POSIX_CACHE_SIZE 0

#endif /*LV_USE_FS_POSIX*/

/** Win32 */
#define LV_USE_FS_WIN32 0

#if LV_USE_FS_WIN32
/** Drive letter for Win32 (65 = 'A', 0 = disabled) */
#define LV_FS_WIN32_LETTER 0

/** Working directory for Win32 */
#define LV_FS_WIN32_PATH ""

/** Read cache size in bytes for Win32 (0 = disabled) */
#define LV_FS_WIN32_CACHE_SIZE 0

#endif /*LV_USE_FS_WIN32*/

/** FatFS */
#define LV_USE_FS_FATFS 0

#if LV_USE_FS_FATFS
/** Drive letter for FatFS (65 = 'A', 0 = disabled) */
#define LV_FS_FATFS_LETTER 0

/** Working directory for FatFS */
#define LV_FS_FATFS_PATH ""

/** Read cache size in bytes for FatFS (0 = disabled) */
#define LV_FS_FATFS_CACHE_SIZE 0

#endif /*LV_USE_FS_FATFS*/

/** littlefs */
#define LV_USE_FS_LITTLEFS 0

#if LV_USE_FS_LITTLEFS
/** Drive letter for littlefs (65 = 'A', 0 = disabled) */
#define LV_FS_LITTLEFS_LETTER 0

/** Working directory for littlefs */
#define LV_FS_LITTLEFS_PATH ""

#endif /*LV_USE_FS_LITTLEFS*/

/** Arduino ESP littlefs */
#define LV_USE_FS_ARDUINO_ESP_LITTLEFS 0

#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
/** Drive letter for Arduino ESP littlefs (65 = 'A', 0 = disabled) */
#define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER 0

/** Working directory for Arduino ESP littlefs */
#define LV_FS_ARDUINO_ESP_LITTLEFS_PATH ""

#endif /*LV_USE_FS_ARDUINO_ESP_LITTLEFS*/

/** Arduino SD */
#define LV_USE_FS_ARDUINO_SD 0

#if LV_USE_FS_ARDUINO_SD
/** Drive letter for Arduino SD (65 = 'A', 0 = disabled) */
#define LV_FS_ARDUINO_SD_LETTER 0

/** Working directory for Arduino SD */
#define LV_FS_ARDUINO_SD_PATH ""

#endif /*LV_USE_FS_ARDUINO_SD*/

/** UEFI */
#define LV_USE_FS_UEFI 0

#if LV_USE_FS_UEFI
/** Drive letter for UEFI (65 = 'A', 0 = disabled) */
#define LV_FS_UEFI_LETTER 0

#endif /*LV_USE_FS_UEFI*/

/** FrogFS */
#define LV_USE_FS_FROGFS 0

#if LV_USE_FS_FROGFS
/** Drive letter for FrogFS (65 = 'A', 0 = disabled) */
#define LV_FS_FROGFS_LETTER 0

#endif /*LV_USE_FS_FROGFS*/

/** Reach data already in memory through the lv_fs API, without a real file system. */
#define LV_USE_FS_MEMFS 0

#if LV_USE_FS_MEMFS
/** Drive letter for memfs (65 = 'A', 0 = disabled) */
#define LV_FS_MEMFS_LETTER 0

#endif /*LV_USE_FS_MEMFS*/


/*============================================================================
 * DEBUGGING
 *============================================================================*/

/** Periodically collect CPU, FPS and memory statistics and optionally show them in an
 *  on-screen overlay.
 */
#define LV_USE_SYSMON 0

#if LV_USE_SYSMON
/** The port provides `lv_os_get_proc_idle_percent` a per-process idle percentage
 *  in addition to the system-wide one. LVGL provides a default one for linux systems.
 */
#define LV_SYSMON_PROC_IDLE_AVAILABLE 0

/** Show the CPU usage and FPS count in a label on the screen. */
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

/** Print the performance data to the log instead of showing the on-screen label. */
#define LV_USE_PERF_MONITOR_LOG_MODE 0

#endif /*LV_USE_PERF_MONITOR*/

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN
/** Show the used memory and the memory fragmentation in a label on the screen. */
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

/** Instrument LVGL internals with trace hooks to measure runtime performance. */
#define LV_USE_PROFILER 0

/** Built-in profiler
 *
 *  Enable: LV_USE_PROFILER
 */
#define LV_USE_PROFILER_BUILTIN 0

#if LV_USE_PROFILER_BUILTIN
/** Trace buffer size (bytes) */
#define LV_PROFILER_BUILTIN_BUF_SIZE 16384

/** Start profiling at init */
#define LV_PROFILER_BUILTIN_DEFAULT_ENABLE 1

/** POSIX port */
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

/** Layout */
#define LV_PROFILER_LAYOUT 1

/** Display refresh */
#define LV_PROFILER_REFR 1

/** Draw */
#define LV_PROFILER_DRAW 1

/** Input devices */
#define LV_PROFILER_INDEV 1

/** Image decoder */
#define LV_PROFILER_DECODER 1

/** Font */
#define LV_PROFILER_FONT 1

/** File system */
#define LV_PROFILER_FS 1

/** Style */
#define LV_PROFILER_STYLE 0

/** Timer */
#define LV_PROFILER_TIMER 1

/** Cache */
#define LV_PROFILER_CACHE 1

/** Events */
#define LV_PROFILER_EVENT 1

#endif /*LV_USE_PROFILER*/

/** Emulated input devices, time emulation, and screenshot compares. */
#define LV_USE_TEST 0

#if LV_USE_TEST
/** Enable `lv_test_screenshot_compare()`. Requires a few MB of extra RAM.
 *
 *  Enable: LV_USE_LODEPNG
 */
#define LV_USE_TEST_SCREENSHOT_COMPARE 0

#if LV_USE_TEST_SCREENSHOT_COMPARE
/** When a reference image does not exist yet, create it from the rendered
 *  screen instead of failing the compare.
 */
#define LV_TEST_SCREENSHOT_CREATE_REFERENCE_IMAGE 1

#endif /*LV_USE_TEST_SCREENSHOT_COMPARE*/

#if LV_USE_DRAW_NANOVG
/** Render into an off-screen EGL pbuffer instead of a window and read the
 *  pixels back into the LVGL draw buffer, so screenshot compares work the
 *  same way as with the software renderer. Requires EGL and GLESv2.
 */
#define LV_USE_NANOVG_TEST_HEADLESS 0

#endif /*LV_USE_DRAW_NANOVG*/
#endif /*LV_USE_TEST*/

/** Feed random input events to the UI for stress testing. */
#define LV_USE_MONKEY 0

/** Draw random colored rectangles over the redrawn areas. */
#define LV_USE_REFR_DEBUG 0

/** Draw a red overlay over ARGB layers and a green overlay over RGB layers. */
#define LV_USE_LAYER_DEBUG 0

/** Draw an overlay in a different color for each draw unit's tasks, with the
 *  draw unit's index number on a white background (black for layers).
 */
#define LV_USE_PARALLEL_DRAW_DEBUG 0



/*============================================================================
 * OTHERS
 *============================================================================*/

/** Manage a widget subtree with its own lifecycle and back stack, similar to Android fragments. */
#define LV_USE_FRAGMENT 0

/** File explorer
 *
 *  Enable: LV_USE_TABLE
 */
#define LV_USE_FILE_EXPLORER 0

#if LV_USE_FILE_EXPLORER
/** Maximum path length (bytes) */
#define LV_FILE_EXPLORER_PATH_MAX_LEN 128

/** Add a sidebar with shortcuts to common places. It can be hidden by clicking the
 *  button at the top left corner of the browsing area, which is useful on small screens.
 */
#define LV_FILE_EXPLORER_QUICK_ACCESS 1

#endif /*LV_USE_FILE_EXPLORER*/


/*============================================================================
 * BUILD
 *============================================================================*/

/** Include `lvgl_private.h` from `lvgl.h` so that LVGL internal data
 *  structures and functions are accessible by default.
 */
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

/** NULL checks (very fast, recommended) */
#define LV_USE_ASSERT_NULL 1

/** Allocation success checks (very fast, recommended) */
#define LV_USE_ASSERT_MALLOC 1

/** Style init checks (very fast, recommended) */
#define LV_USE_ASSERT_STYLE 0

/** lv_mem integrity checks (slow) */
#define LV_USE_ASSERT_MEM_INTEGRITY 0

/** Widget validity checks (slow) */
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

/** Include the header given by LV_ASSERT_CUSTOM_INCLUDE, which must define
 *  a custom LV_ASSERT_HANDLER.
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

/** LV_CHECK_ARG validates function arguments at runtime: a failed check
 *  logs a warning and executes the specified action. When disabled all
 *  these checks compile to nothing, which is only safe in very resource
 *  constrained environments where invariants are guaranteed to hold.
 */
#define LV_USE_CHECK_ARG 1

#if LV_USE_CHECK_ARG
/** Also invoke LV_ASSERT_HANDLER when an LV_CHECK_ARG check fails,
 *  before the action is executed.
 */
#define LV_CHECK_ARG_ASSERT_ON_FAIL 0

/** Controls what is logged when an LV_CHECK_ARG check fails.
 *  Minimal and Verbose require LV_USE_LOG; selecting either without it
 *  causes a compile-time error.
 *  Possible values:
 *  - LV_CHECK_ARG_LOG_MODE_NONE: no log output on failure
 *  - LV_CHECK_ARG_LOG_MODE_MINIMAL: log 'Check failed' only (file/line from LV_LOG_WARN) (requires LV_USE_LOG)
 *  - LV_CHECK_ARG_LOG_MODE_VERBOSE: log 'Check failed: <cond>' plus caller-supplied message (requires LV_USE_LOG)
 */
#define LV_CHECK_ARG_LOG_MODE LV_CHECK_ARG_LOG_MODE_NONE

/** LV_CHECK_OBJ verifies with lv_obj_has_class() that the object has the
 *  expected class. When disabled the check is skipped even if a class
 *  argument is supplied.
 */
#define LV_USE_CHECK_OBJ_CLASSTYPE 0

/** LV_CHECK_OBJ verifies with lv_obj_is_valid() that the object is still
 *  part of the widget tree. When disabled the check is skipped even if the
 *  associated argument is supplied.
 */
#define LV_USE_CHECK_OBJ_VALIDITY 0

#endif /*LV_USE_CHECK_ARG*/


/*============================================================================
 * COMPILER SETTINGS
 *============================================================================*/

/** Big endian system */
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

/** Build examples */
#define LV_BUILD_EXAMPLES 1



/*============================================================================
 * DEMOS
 *============================================================================*/

/** Build demos */
#define LV_BUILD_DEMOS 1

#if LV_BUILD_DEMOS
/** Benchmark demo
 *
 *  Enable: LV_FONT_MONTSERRAT_14, LV_FONT_MONTSERRAT_20, LV_FONT_MONTSERRAT_24, LV_FONT_MONTSERRAT_26, LV_USE_DEMO_WIDGETS
 */
#define LV_USE_DEMO_BENCHMARK 0

#if LV_USE_DEMO_BENCHMARK
/** Use the demo's own Montserrat fonts with aligned glyph bitmaps instead
 *  of the LV_FONT_MONTSERRAT_* fonts.
 */
#define LV_DEMO_BENCHMARK_ALIGNED_FONTS 0

#endif /*LV_USE_DEMO_BENCHMARK*/

#if LV_USE_GLTF
/** glTF demo */
#define LV_USE_DEMO_GLTF 0

#endif /*LV_USE_GLTF*/

/** Keypad and encoder demo */
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 0

/** Music player demo */
#define LV_USE_DEMO_MUSIC 0

#if LV_USE_DEMO_MUSIC
/** Square layout */
#define LV_DEMO_MUSIC_SQUARE 0

/** Landscape layout */
#define LV_DEMO_MUSIC_LANDSCAPE 0

/** Round layout */
#define LV_DEMO_MUSIC_ROUND 0

/** Large layout */
#define LV_DEMO_MUSIC_LARGE 0

/** Auto play */
#define LV_DEMO_MUSIC_AUTO_PLAY 0

#endif /*LV_USE_DEMO_MUSIC*/

/** Renders each drawing primitive for visual comparison.
 *  Requires a display of at least 480x272.
 *
 *  Enable: LV_USE_GRID
 */
#define LV_USE_DEMO_RENDER 0

/** Stress demo */
#define LV_USE_DEMO_STRESS 0

/** Vector graphic demo
 *
 *  Enable: LV_USE_VECTOR_GRAPHIC
 */
#define LV_USE_DEMO_VECTOR_GRAPHIC 0

/** Widgets demo */
#define LV_USE_DEMO_WIDGETS 0

/** Flex layout demo */
#define LV_USE_DEMO_FLEX_LAYOUT 0

/** Multi-language demo */
#define LV_USE_DEMO_MULTILANG 0

/** Smartwatch demo */
#define LV_USE_DEMO_SMARTWATCH 0

/** E-bike demo */
#define LV_USE_DEMO_EBIKE 0

#if LV_USE_DEMO_EBIKE
/** Portrait layout */
#define LV_DEMO_EBIKE_PORTRAIT 0

#endif /*LV_USE_DEMO_EBIKE*/

/** High resolution demo */
#define LV_USE_DEMO_HIGH_RES 0

#endif /*LV_BUILD_DEMOS*/


/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/

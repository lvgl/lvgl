/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

/*Handle special Kconfig options*/
#include "lv_conf_kconfig.h"

#include <stdint.h>

#if defined __has_include
#  if __has_include("lv_conf.h")
#   ifndef LV_CONF_INCLUDE_SIMPLE
#    define LV_CONF_INCLUDE_SIMPLE
#   endif
#  endif
#endif

/*If lv_conf.h is not skipped include it*/
#if !defined(LV_CONF_SKIP) && !defined(CONFIG_LV_CONF_SKIP)
#  if defined(LV_CONF_PATH)											/*If there is a path defined for lv_conf.h use it*/
#    define __LV_TO_STR_AUX(x) #x
#    define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
#    include __LV_TO_STR(LV_CONF_PATH)
#    undef __LV_TO_STR_AUX
#    undef __LV_TO_STR
#  elif defined(LV_CONF_INCLUDE_SIMPLE)        /*Or simply include lv_conf.h is enabled*/
#    include "lv_conf.h"
#  else
#    include "../../lv_conf.h"                 /*Else assume lv_conf.h is next to the lvgl folder */
#  endif
#endif

/* clang-format off */

#include <stdint.h>

/*====================
   Graphical settings
 *====================*/

/* Maximal horizontal and vertical resolution to support by the library.*/
#ifndef LV_HOR_RES_MAX
#  ifdef CONFIG_LV_HOR_RES_MAX
#    define LV_HOR_RES_MAX CONFIG_LV_HOR_RES_MAX
#  else
#    define  LV_HOR_RES_MAX          (480)
#  endif
#endif
#ifndef LV_VER_RES_MAX
#  ifdef CONFIG_LV_VER_RES_MAX
#    define LV_VER_RES_MAX CONFIG_LV_VER_RES_MAX
#  else
#    define  LV_VER_RES_MAX          (320)
#  endif
#endif

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB332
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifndef LV_COLOR_DEPTH
#  ifdef CONFIG_LV_COLOR_DEPTH
#    define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
#  else
#    define  LV_COLOR_DEPTH     16
#  endif
#endif

/* Swap the 2 bytes of RGB565 color.
 * Useful if the display has a 8 bit interface (e.g. SPI)*/
#ifndef LV_COLOR_16_SWAP
#  ifdef CONFIG_LV_COLOR_16_SWAP
#    define LV_COLOR_16_SWAP CONFIG_LV_COLOR_16_SWAP
#  else
#    define  LV_COLOR_16_SWAP   0
#  endif
#endif

/* 1: Enable screen transparency.
 * Useful for OSD or other overlapping GUIs.
 * Requires `LV_COLOR_DEPTH = 32` colors and the screen's style should be modified: `style.body.opa = ...`*/
#ifndef LV_COLOR_SCREEN_TRANSP
#  ifdef CONFIG_LV_COLOR_SCREEN_TRANSP
#    define LV_COLOR_SCREEN_TRANSP CONFIG_LV_COLOR_SCREEN_TRANSP
#  else
#    define  LV_COLOR_SCREEN_TRANSP    0
#  endif
#endif

/*Images pixels with this color will not be drawn (with chroma keying)*/
#ifndef LV_COLOR_TRANSP
#  ifdef CONFIG_LV_COLOR_TRANSP
#    define LV_COLOR_TRANSP CONFIG_LV_COLOR_TRANSP
#  else
#    define  LV_COLOR_TRANSP    LV_COLOR_LIME         /*LV_COLOR_LIME: pure green*/
#  endif
#endif

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#ifndef LV_ANTIALIAS
#  ifdef CONFIG_LV_ANTIALIAS
#    define LV_ANTIALIAS CONFIG_LV_ANTIALIAS
#  else
#    define  LV_ANTIALIAS        1
#  endif
#endif

/* Default display refresh period.
 * Can be changed in the display driver (`lv_disp_drv_t`).*/
#ifndef LV_DISP_DEF_REFR_PERIOD
#  ifdef CONFIG_LV_DISP_DEF_REFR_PERIOD
#    define LV_DISP_DEF_REFR_PERIOD CONFIG_LV_DISP_DEF_REFR_PERIOD
#  else
#    define  LV_DISP_DEF_REFR_PERIOD      30      /*[ms]*/
#  endif
#endif

/* Dot Per Inch: used to initialize default sizes.
 * E.g. a button with width = LV_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#ifndef LV_DPI
#  ifdef CONFIG_LV_DPI
#    define LV_DPI CONFIG_LV_DPI
#  else
#    define  LV_DPI              130     /*[px]*/
#  endif
#endif

/* The the real width of the display changes some default values:
 * default object sizes, layout of examples, etc.
 * According to the width of the display (hor. res. / dpi)
 * the displays fall in 4 categories.
 * The 4th is extra large which has no upper limit so not listed here
 * The upper limit of the categories are set below in 0.1 inch unit.
 */
#ifndef LV_DISP_SMALL_LIMIT
#  ifdef CONFIG_LV_DISP_SMALL_LIMIT
#    define LV_DISP_SMALL_LIMIT CONFIG_LV_DISP_SMALL_LIMIT
#  else
#    define  LV_DISP_SMALL_LIMIT  30
#  endif
#endif
#ifndef LV_DISP_MEDIUM_LIMIT
#  ifdef CONFIG_LV_DISP_MEDIUM_LIMIT
#    define LV_DISP_MEDIUM_LIMIT CONFIG_LV_DISP_MEDIUM_LIMIT
#  else
#    define  LV_DISP_MEDIUM_LIMIT 50
#  endif
#endif
#ifndef LV_DISP_LARGE_LIMIT
#  ifdef CONFIG_LV_DISP_LARGE_LIMIT
#    define LV_DISP_LARGE_LIMIT CONFIG_LV_DISP_LARGE_LIMIT
#  else
#    define  LV_DISP_LARGE_LIMIT  70
#  endif
#endif

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */

/*=========================
   Memory manager settings
 *=========================*/

/* LittelvGL's internal memory manager's settings.
 * The graphical objects and other related data are stored here. */

/* 1: use custom malloc/free, 0: use the built-in `lv_mem_alloc` and `lv_mem_free` */
#ifndef LV_MEM_CUSTOM
#  ifdef CONFIG_LV_MEM_CUSTOM
#    define LV_MEM_CUSTOM CONFIG_LV_MEM_CUSTOM
#  else
#    define  LV_MEM_CUSTOM      0
#  endif
#endif
#if LV_MEM_CUSTOM == 0
/* Size of the memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#ifndef LV_MEM_SIZE
#  ifdef CONFIG_LV_MEM_SIZE
#    define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
#  else
#    define  LV_MEM_SIZE    (32U * 1024U)
#  endif
#endif

/* Complier prefix for a big array declaration */
#ifndef LV_MEM_ATTR
#  ifdef CONFIG_LV_MEM_ATTR
#    define LV_MEM_ATTR CONFIG_LV_MEM_ATTR
#  else
#    define  LV_MEM_ATTR
#  endif
#endif

/* Set an address for the memory pool instead of allocating it as an array.
 * Can be in external SRAM too. */
#ifndef LV_MEM_ADR
#  ifdef CONFIG_LV_MEM_ADR
#    define LV_MEM_ADR CONFIG_LV_MEM_ADR
#  else
#    define  LV_MEM_ADR          0
#  endif
#endif

/* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
#ifndef LV_MEM_AUTO_DEFRAG
#  ifdef CONFIG_LV_MEM_AUTO_DEFRAG
#    define LV_MEM_AUTO_DEFRAG CONFIG_LV_MEM_AUTO_DEFRAG
#  else
#    define  LV_MEM_AUTO_DEFRAG  1
#  endif
#endif
#else       /*LV_MEM_CUSTOM*/
#ifndef LV_MEM_CUSTOM_INCLUDE
#  ifdef CONFIG_LV_MEM_CUSTOM_INCLUDE
#    define LV_MEM_CUSTOM_INCLUDE CONFIG_LV_MEM_CUSTOM_INCLUDE
#  else
#    define  LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#  endif
#endif
#ifndef LV_MEM_CUSTOM_ALLOC
#  ifdef CONFIG_LV_MEM_CUSTOM_ALLOC
#    define LV_MEM_CUSTOM_ALLOC CONFIG_LV_MEM_CUSTOM_ALLOC
#  else
#    define  LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#  endif
#endif
#ifndef LV_MEM_CUSTOM_FREE
#  ifdef CONFIG_LV_MEM_CUSTOM_FREE
#    define LV_MEM_CUSTOM_FREE CONFIG_LV_MEM_CUSTOM_FREE
#  else
#    define  LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#  endif
#endif
#endif     /*LV_MEM_CUSTOM*/

/* Use the standard memcpy and memset instead of LVGL's own functions.
 * The standard functions might or might not be faster depending on their implementation. */
#ifndef LV_MEMCPY_MEMSET_STD
#  ifdef CONFIG_LV_MEMCPY_MEMSET_STD
#    define LV_MEMCPY_MEMSET_STD CONFIG_LV_MEMCPY_MEMSET_STD
#  else
#    define  LV_MEMCPY_MEMSET_STD    0
#  endif
#endif

/* Garbage Collector settings
 * Used if lvgl is binded to higher level language and the memory is managed by that language */
#ifndef LV_ENABLE_GC
#  ifdef CONFIG_LV_ENABLE_GC
#    define LV_ENABLE_GC CONFIG_LV_ENABLE_GC
#  else
#    define  LV_ENABLE_GC 0
#  endif
#endif
#if LV_ENABLE_GC != 0
#ifndef LV_GC_INCLUDE
#  ifdef CONFIG_LV_GC_INCLUDE
#    define LV_GC_INCLUDE CONFIG_LV_GC_INCLUDE
#  else
#    define  LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#  endif
#endif
#ifndef LV_MEM_CUSTOM_REALLOC
#  ifdef CONFIG_LV_MEM_CUSTOM_REALLOC
#    define LV_MEM_CUSTOM_REALLOC CONFIG_LV_MEM_CUSTOM_REALLOC
#  else
#    define  LV_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
#  endif
#endif
#ifndef LV_MEM_CUSTOM_GET_SIZE
#  ifdef CONFIG_LV_MEM_CUSTOM_GET_SIZE
#    define LV_MEM_CUSTOM_GET_SIZE CONFIG_LV_MEM_CUSTOM_GET_SIZE
#  else
#    define  LV_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to lv_mem_get_size*/
#  endif
#endif
#endif /* LV_ENABLE_GC */

/*=======================
   Input device settings
 *=======================*/

/* Input device default settings.
 * Can be changed in the Input device driver (`lv_indev_drv_t`)*/

/* Input device read period in milliseconds */
#ifndef LV_INDEV_DEF_READ_PERIOD
#  ifdef CONFIG_LV_INDEV_DEF_READ_PERIOD
#    define LV_INDEV_DEF_READ_PERIOD CONFIG_LV_INDEV_DEF_READ_PERIOD
#  else
#    define  LV_INDEV_DEF_READ_PERIOD          30
#  endif
#endif

/* Drag threshold in pixels */
#ifndef LV_INDEV_DEF_DRAG_LIMIT
#  ifdef CONFIG_LV_INDEV_DEF_DRAG_LIMIT
#    define LV_INDEV_DEF_DRAG_LIMIT CONFIG_LV_INDEV_DEF_DRAG_LIMIT
#  else
#    define  LV_INDEV_DEF_DRAG_LIMIT           10
#  endif
#endif

/* Drag throw slow-down in [%]. Greater value -> faster slow-down */
#ifndef LV_INDEV_DEF_DRAG_THROW
#  ifdef CONFIG_LV_INDEV_DEF_DRAG_THROW
#    define LV_INDEV_DEF_DRAG_THROW CONFIG_LV_INDEV_DEF_DRAG_THROW
#  else
#    define  LV_INDEV_DEF_DRAG_THROW           10
#  endif
#endif

/* Long press time in milliseconds.
 * Time to send `LV_EVENT_LONG_PRESSSED`) */
#ifndef LV_INDEV_DEF_LONG_PRESS_TIME
#  ifdef CONFIG_LV_INDEV_DEF_LONG_PRESS_TIME
#    define LV_INDEV_DEF_LONG_PRESS_TIME CONFIG_LV_INDEV_DEF_LONG_PRESS_TIME
#  else
#    define  LV_INDEV_DEF_LONG_PRESS_TIME      400
#  endif
#endif

/* Repeated trigger period in long press [ms]
 * Time between `LV_EVENT_LONG_PRESSED_REPEAT */
#ifndef LV_INDEV_DEF_LONG_PRESS_REP_TIME
#  ifdef CONFIG_LV_INDEV_DEF_LONG_PRESS_REP_TIME
#    define LV_INDEV_DEF_LONG_PRESS_REP_TIME CONFIG_LV_INDEV_DEF_LONG_PRESS_REP_TIME
#  else
#    define  LV_INDEV_DEF_LONG_PRESS_REP_TIME  100
#  endif
#endif


/* Gesture threshold in pixels */
#ifndef LV_INDEV_DEF_GESTURE_LIMIT
#  ifdef CONFIG_LV_INDEV_DEF_GESTURE_LIMIT
#    define LV_INDEV_DEF_GESTURE_LIMIT CONFIG_LV_INDEV_DEF_GESTURE_LIMIT
#  else
#    define  LV_INDEV_DEF_GESTURE_LIMIT        50
#  endif
#endif

/* Gesture min velocity at release before swipe (pixels)*/
#ifndef LV_INDEV_DEF_GESTURE_MIN_VELOCITY
#  ifdef CONFIG_LV_INDEV_DEF_GESTURE_MIN_VELOCITY
#    define LV_INDEV_DEF_GESTURE_MIN_VELOCITY CONFIG_LV_INDEV_DEF_GESTURE_MIN_VELOCITY
#  else
#    define  LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3
#  endif
#endif

/*==================
 * Feature usage
 *==================*/

/*1: Enable the Animations */
#ifndef LV_USE_ANIMATION
#  ifdef CONFIG_LV_USE_ANIMATION
#    define LV_USE_ANIMATION CONFIG_LV_USE_ANIMATION
#  else
#    define  LV_USE_ANIMATION        1
#  endif
#endif
#if LV_USE_ANIMATION

/*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/

#endif

/* 1: Enable shadow drawing on rectangles*/
#ifndef LV_USE_SHADOW
#  ifdef CONFIG_LV_USE_SHADOW
#    define LV_USE_SHADOW CONFIG_LV_USE_SHADOW
#  else
#    define  LV_USE_SHADOW           1
#  endif
#endif
#if LV_USE_SHADOW
/* Allow buffering some shadow calculation
 * LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer,
 * where shadow size is `shadow_width + radius`
 * Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
#ifndef LV_SHADOW_CACHE_SIZE
#  ifdef CONFIG_LV_SHADOW_CACHE_SIZE
#    define LV_SHADOW_CACHE_SIZE CONFIG_LV_SHADOW_CACHE_SIZE
#  else
#    define  LV_SHADOW_CACHE_SIZE    0
#  endif
#endif
#endif

/*1: enable outline drawing on rectangles*/
#ifndef LV_USE_OUTLINE
#  ifdef CONFIG_LV_USE_OUTLINE
#    define LV_USE_OUTLINE CONFIG_LV_USE_OUTLINE
#  else
#    define  LV_USE_OUTLINE  1
#  endif
#endif

/*1: enable pattern drawing on rectangles*/
#ifndef LV_USE_PATTERN
#  ifdef CONFIG_LV_USE_PATTERN
#    define LV_USE_PATTERN CONFIG_LV_USE_PATTERN
#  else
#    define  LV_USE_PATTERN  1
#  endif
#endif

/*1: enable value string drawing on rectangles*/
#ifndef LV_USE_VALUE_STR
#  ifdef CONFIG_LV_USE_VALUE_STR
#    define LV_USE_VALUE_STR CONFIG_LV_USE_VALUE_STR
#  else
#    define  LV_USE_VALUE_STR    1
#  endif
#endif

/* 1: Use other blend modes than normal (`LV_BLEND_MODE_...`)*/
#ifndef LV_USE_BLEND_MODES
#  ifdef CONFIG_LV_USE_BLEND_MODES
#    define LV_USE_BLEND_MODES CONFIG_LV_USE_BLEND_MODES
#  else
#    define  LV_USE_BLEND_MODES      1
#  endif
#endif

/* 1: Use the `opa_scale` style property to set the opacity of an object and its children at once*/
#ifndef LV_USE_OPA_SCALE
#  ifdef CONFIG_LV_USE_OPA_SCALE
#    define LV_USE_OPA_SCALE CONFIG_LV_USE_OPA_SCALE
#  else
#    define  LV_USE_OPA_SCALE        1
#  endif
#endif

/* 1: Use image zoom and rotation*/
#ifndef LV_USE_IMG_TRANSFORM
#  ifdef CONFIG_LV_USE_IMG_TRANSFORM
#    define LV_USE_IMG_TRANSFORM CONFIG_LV_USE_IMG_TRANSFORM
#  else
#    define  LV_USE_IMG_TRANSFORM    1
#  endif
#endif

/* 1: Enable object groups (for keyboard/encoder navigation) */
#ifndef LV_USE_GROUP
#  ifdef CONFIG_LV_USE_GROUP
#    define LV_USE_GROUP CONFIG_LV_USE_GROUP
#  else
#    define  LV_USE_GROUP            1
#  endif
#endif
#if LV_USE_GROUP
#endif  /*LV_USE_GROUP*/

/* 1: Enable GPU interface*/
#ifndef LV_USE_GPU
#  ifdef CONFIG_LV_USE_GPU
#    define LV_USE_GPU CONFIG_LV_USE_GPU
#  else
#    define  LV_USE_GPU              1   /*Only enables `gpu_fill_cb` and `gpu_blend_cb` in the disp. drv- */
#  endif
#endif
#ifndef LV_USE_GPU_STM32_DMA2D
#  ifdef CONFIG_LV_USE_GPU_STM32_DMA2D
#    define LV_USE_GPU_STM32_DMA2D CONFIG_LV_USE_GPU_STM32_DMA2D
#  else
#    define  LV_USE_GPU_STM32_DMA2D  0
#  endif
#endif
/*If enabling LV_USE_GPU_STM32_DMA2D, LV_GPU_DMA2D_CMSIS_INCLUDE must be defined to include path of CMSIS header of target processor
e.g. "stm32f769xx.h" or "stm32f429xx.h" */
#ifndef LV_GPU_DMA2D_CMSIS_INCLUDE
#  ifdef CONFIG_LV_GPU_DMA2D_CMSIS_INCLUDE
#    define LV_GPU_DMA2D_CMSIS_INCLUDE CONFIG_LV_GPU_DMA2D_CMSIS_INCLUDE
#  else
#    define  LV_GPU_DMA2D_CMSIS_INCLUDE
#  endif
#endif

/*1: Use PXP for CPU off-load on NXP RTxxx platforms */
#ifndef LV_USE_GPU_NXP_PXP
#  ifdef CONFIG_LV_USE_GPU_NXP_PXP
#    define LV_USE_GPU_NXP_PXP CONFIG_LV_USE_GPU_NXP_PXP
#  else
#    define  LV_USE_GPU_NXP_PXP      0
#  endif
#endif

/*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
 *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol FSL_RTOS_FREE_RTOS
 *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
 *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
 * */
#ifndef LV_USE_GPU_NXP_PXP_AUTO_INIT
#  ifdef CONFIG_LV_USE_GPU_NXP_PXP_AUTO_INIT
#    define LV_USE_GPU_NXP_PXP_AUTO_INIT CONFIG_LV_USE_GPU_NXP_PXP_AUTO_INIT
#  else
#    define  LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#  endif
#endif

/*1: Use VG-Lite for CPU offload on NXP RTxxx platforms */
#ifndef LV_USE_GPU_NXP_VG_LITE
#  ifdef CONFIG_LV_USE_GPU_NXP_VG_LITE
#    define LV_USE_GPU_NXP_VG_LITE CONFIG_LV_USE_GPU_NXP_VG_LITE
#  else
#    define  LV_USE_GPU_NXP_VG_LITE   0
#  endif
#endif

/* 1: Enable file system (might be required for images */
#ifndef LV_USE_FILESYSTEM
#  ifdef CONFIG_LV_USE_FILESYSTEM
#    define LV_USE_FILESYSTEM CONFIG_LV_USE_FILESYSTEM
#  else
#    define  LV_USE_FILESYSTEM       1
#  endif
#endif
#if LV_USE_FILESYSTEM
/*Declare the type of the user data of file system drivers (can be e.g. `void *`, `int`, `struct`)*/
#endif

/*1: Add a `user_data` to drivers and objects*/
#ifndef LV_USE_USER_DATA
#  ifdef CONFIG_LV_USE_USER_DATA
#    define LV_USE_USER_DATA CONFIG_LV_USE_USER_DATA
#  else
#    define  LV_USE_USER_DATA        0
#  endif
#endif

/*1: Show CPU usage and FPS count in the right bottom corner*/
#ifndef LV_USE_PERF_MONITOR
#  ifdef CONFIG_LV_USE_PERF_MONITOR
#    define LV_USE_PERF_MONITOR CONFIG_LV_USE_PERF_MONITOR
#  else
#    define  LV_USE_PERF_MONITOR     0
#  endif
#endif

/*1: Use the functions and types from the older API if possible */
#ifndef LV_USE_API_EXTENSION_V6
#  ifdef CONFIG_LV_USE_API_EXTENSION_V6
#    define LV_USE_API_EXTENSION_V6 CONFIG_LV_USE_API_EXTENSION_V6
#  else
#    define  LV_USE_API_EXTENSION_V6  1
#  endif
#endif
#ifndef LV_USE_API_EXTENSION_V7
#  ifdef CONFIG_LV_USE_API_EXTENSION_V7
#    define LV_USE_API_EXTENSION_V7 CONFIG_LV_USE_API_EXTENSION_V7
#  else
#    define  LV_USE_API_EXTENSION_V7  1
#  endif
#endif

/*========================
 * Image decoder and cache
 *========================*/

/* 1: Enable indexed (palette) images */
#ifndef LV_IMG_CF_INDEXED
#  ifdef CONFIG_LV_IMG_CF_INDEXED
#    define LV_IMG_CF_INDEXED CONFIG_LV_IMG_CF_INDEXED
#  else
#    define  LV_IMG_CF_INDEXED       1
#  endif
#endif

/* 1: Enable alpha indexed images */
#ifndef LV_IMG_CF_ALPHA
#  ifdef CONFIG_LV_IMG_CF_ALPHA
#    define LV_IMG_CF_ALPHA CONFIG_LV_IMG_CF_ALPHA
#  else
#    define  LV_IMG_CF_ALPHA         1
#  endif
#endif

/* Default image cache size. Image caching keeps the images opened.
 * If only the built-in image formats are used there is no real advantage of caching.
 * (I.e. no new image decoder is added)
 * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 * However the opened images might consume additional RAM.
 * LV_IMG_CACHE_DEF_SIZE must be >= 1 */
#ifndef LV_IMG_CACHE_DEF_SIZE
#  ifdef CONFIG_LV_IMG_CACHE_DEF_SIZE
#    define LV_IMG_CACHE_DEF_SIZE CONFIG_LV_IMG_CACHE_DEF_SIZE
#  else
#    define  LV_IMG_CACHE_DEF_SIZE       1
#  endif
#endif

/*Declare the type of the user data of image decoder (can be e.g. `void *`, `int`, `struct`)*/

/*=====================
 *  Compiler settings
 *====================*/

/* For big endian systems set to 1 */
#ifndef LV_BIG_ENDIAN_SYSTEM
#  ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
#    define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
#  else
#    define  LV_BIG_ENDIAN_SYSTEM    0
#  endif
#endif

/* Define a custom attribute to `lv_tick_inc` function */
#ifndef LV_ATTRIBUTE_TICK_INC
#  ifdef CONFIG_LV_ATTRIBUTE_TICK_INC
#    define LV_ATTRIBUTE_TICK_INC CONFIG_LV_ATTRIBUTE_TICK_INC
#  else
#    define  LV_ATTRIBUTE_TICK_INC
#  endif
#endif

/* Define a custom attribute to `lv_task_handler` function */
#ifndef LV_ATTRIBUTE_TASK_HANDLER
#  ifdef CONFIG_LV_ATTRIBUTE_TASK_HANDLER
#    define LV_ATTRIBUTE_TASK_HANDLER CONFIG_LV_ATTRIBUTE_TASK_HANDLER
#  else
#    define  LV_ATTRIBUTE_TASK_HANDLER
#  endif
#endif

/* Define a custom attribute to `lv_disp_flush_ready` function */
#ifndef LV_ATTRIBUTE_FLUSH_READY
#  ifdef CONFIG_LV_ATTRIBUTE_FLUSH_READY
#    define LV_ATTRIBUTE_FLUSH_READY CONFIG_LV_ATTRIBUTE_FLUSH_READY
#  else
#    define  LV_ATTRIBUTE_FLUSH_READY
#  endif
#endif

/* Required alignment size for buffers */
#ifndef LV_ATTRIBUTE_MEM_ALIGN_SIZE
#  ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
#    define LV_ATTRIBUTE_MEM_ALIGN_SIZE CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
#  else
#    define  LV_ATTRIBUTE_MEM_ALIGN_SIZE
#  endif
#endif

/* With size optimization (-Os) the compiler might not align data to
 * 4 or 8 byte boundary. Some HW may need even 32 or 64 bytes.
 * This alignment will be explicitly applied where needed.
 * LV_ATTRIBUTE_MEM_ALIGN_SIZE should be used to specify required align size.
 * E.g. __attribute__((aligned(LV_ATTRIBUTE_MEM_ALIGN_SIZE))) */
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#  ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN
#    define LV_ATTRIBUTE_MEM_ALIGN CONFIG_LV_ATTRIBUTE_MEM_ALIGN
#  else
#    define  LV_ATTRIBUTE_MEM_ALIGN
#  endif
#endif

/* Attribute to mark large constant arrays for example
 * font's bitmaps */
#ifndef LV_ATTRIBUTE_LARGE_CONST
#  ifdef CONFIG_LV_ATTRIBUTE_LARGE_CONST
#    define LV_ATTRIBUTE_LARGE_CONST CONFIG_LV_ATTRIBUTE_LARGE_CONST
#  else
#    define  LV_ATTRIBUTE_LARGE_CONST
#  endif
#endif

/* Prefix performance critical functions to place them into a faster memory (e.g RAM)
 * Uses 15-20 kB extra memory */
#ifndef LV_ATTRIBUTE_FAST_MEM
#  ifdef CONFIG_LV_ATTRIBUTE_FAST_MEM
#    define LV_ATTRIBUTE_FAST_MEM CONFIG_LV_ATTRIBUTE_FAST_MEM
#  else
#    define  LV_ATTRIBUTE_FAST_MEM
#  endif
#endif

/* Export integer constant to binding.
 * This macro is used with constants in the form of LV_<CONST> that
 * should also appear on lvgl binding API such as Micropython
 *
 * The default value just prevents a GCC warning.
 */
#ifndef LV_EXPORT_CONST_INT
#  ifdef CONFIG_LV_EXPORT_CONST_INT
#    define LV_EXPORT_CONST_INT CONFIG_LV_EXPORT_CONST_INT
#  else
#    define  LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning
#  endif
#endif

/* Prefix variables that are used in GPU accelerated operations, often these need to be
 * placed in RAM sections that are DMA accessible */
#ifndef LV_ATTRIBUTE_DMA
#  ifdef CONFIG_LV_ATTRIBUTE_DMA
#    define LV_ATTRIBUTE_DMA CONFIG_LV_ATTRIBUTE_DMA
#  else
#    define  LV_ATTRIBUTE_DMA
#  endif
#endif

/*===================
 *  HAL settings
 *==================*/

/* 1: use a custom tick source.
 * It removes the need to manually update the tick with `lv_tick_inc`) */
#ifndef LV_TICK_CUSTOM
#  ifdef CONFIG_LV_TICK_CUSTOM
#    define LV_TICK_CUSTOM CONFIG_LV_TICK_CUSTOM
#  else
#    define  LV_TICK_CUSTOM     0
#  endif
#endif
#if LV_TICK_CUSTOM == 1
#ifndef LV_TICK_CUSTOM_INCLUDE
#  ifdef CONFIG_LV_TICK_CUSTOM_INCLUDE
#    define LV_TICK_CUSTOM_INCLUDE CONFIG_LV_TICK_CUSTOM_INCLUDE
#  else
#    define  LV_TICK_CUSTOM_INCLUDE  "Arduino.h"         /*Header for the system time function*/
#  endif
#endif
#ifndef LV_TICK_CUSTOM_SYS_TIME_EXPR
#  ifdef CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR
#    define LV_TICK_CUSTOM_SYS_TIME_EXPR CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR
#  else
#    define  LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current system time in ms*/
#  endif
#endif
#endif   /*LV_TICK_CUSTOM*/


/*================
 * Log settings
 *===============*/

/*1: Enable the log module*/
#ifndef LV_USE_LOG
#  ifdef CONFIG_LV_USE_LOG
#    define LV_USE_LOG CONFIG_LV_USE_LOG
#  else
#    define  LV_USE_LOG      0
#  endif
#endif
#if LV_USE_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 * LV_LOG_LEVEL_NONE        Do not log anything
 */
#ifndef LV_LOG_LEVEL
#  ifdef CONFIG_LV_LOG_LEVEL
#    define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
#  else
#    define  LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#  endif
#endif

/* 1: Print the log with 'printf';
 * 0: user need to register a callback with `lv_log_register_print_cb`*/
#ifndef LV_LOG_PRINTF
#  ifdef CONFIG_LV_LOG_PRINTF
#    define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
#  else
#    define  LV_LOG_PRINTF   0
#  endif
#endif
#endif  /*LV_USE_LOG*/

/*=================
 * Debug settings
 *================*/

/* If Debug is enabled LittelvGL validates the parameters of the functions.
 * If an invalid parameter is found an error log message is printed and
 * the MCU halts at the error. (`LV_USE_LOG` should be enabled)
 * If you are debugging the MCU you can pause
 * the debugger to see exactly where  the issue is.
 *
 * The behavior of asserts can be overwritten by redefining them here.
 * E.g. #define LV_ASSERT_MEM(p)  <my_assert_code>
 */
#ifndef LV_USE_DEBUG
#  ifdef CONFIG_LV_USE_DEBUG
#    define LV_USE_DEBUG CONFIG_LV_USE_DEBUG
#  else
#    define  LV_USE_DEBUG        1
#  endif
#endif
#if LV_USE_DEBUG

/*Check if the parameter is NULL. (Quite fast) */
#ifndef LV_USE_ASSERT_NULL
#  ifdef CONFIG_LV_USE_ASSERT_NULL
#    define LV_USE_ASSERT_NULL CONFIG_LV_USE_ASSERT_NULL
#  else
#    define  LV_USE_ASSERT_NULL      1
#  endif
#endif

/*Checks is the memory is successfully allocated or no. (Quite fast)*/
#ifndef LV_USE_ASSERT_MEM
#  ifdef CONFIG_LV_USE_ASSERT_MEM
#    define LV_USE_ASSERT_MEM CONFIG_LV_USE_ASSERT_MEM
#  else
#    define  LV_USE_ASSERT_MEM       1
#  endif
#endif

/*Check the integrity of `lv_mem` after critical operations. (Slow)*/
#ifndef LV_USE_ASSERT_MEM_INTEGRITY
#  ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
#    define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
#  else
#    define  LV_USE_ASSERT_MEM_INTEGRITY       0
#  endif
#endif

/* Check the strings.
 * Search for NULL, very long strings, invalid characters, and unnatural repetitions. (Slow)
 * If disabled `LV_USE_ASSERT_NULL` will be performed instead (if it's enabled) */
#ifndef LV_USE_ASSERT_STR
#  ifdef CONFIG_LV_USE_ASSERT_STR
#    define LV_USE_ASSERT_STR CONFIG_LV_USE_ASSERT_STR
#  else
#    define  LV_USE_ASSERT_STR       0
#  endif
#endif

/* Check NULL, the object's type and existence (e.g. not deleted). (Quite slow)
 * If disabled `LV_USE_ASSERT_NULL` will be performed instead (if it's enabled) */
#ifndef LV_USE_ASSERT_OBJ
#  ifdef CONFIG_LV_USE_ASSERT_OBJ
#    define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
#  else
#    define  LV_USE_ASSERT_OBJ       0
#  endif
#endif

/*Check if the styles are properly initialized. (Fast)*/
#ifndef LV_USE_ASSERT_STYLE
#  ifdef CONFIG_LV_USE_ASSERT_STYLE
#    define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
#  else
#    define  LV_USE_ASSERT_STYLE     0
#  endif
#endif

#endif /*LV_USE_DEBUG*/

/*==================
 *    FONT USAGE
 *===================*/

/* The built-in fonts contains the ASCII range and some Symbols with  4 bit-per-pixel.
 * The symbols are available via `LV_SYMBOL_...` defines
 * More info about fonts: https://docs.lvgl.io/v7/en/html/overview/font.html
 * To create a new font go to: https://lvgl.com/ttf-font-to-c-array
 */

/* Montserrat fonts with bpp = 4
 * https://fonts.google.com/specimen/Montserrat  */
#ifndef LV_FONT_MONTSERRAT_8
#  ifdef CONFIG_LV_FONT_MONTSERRAT_8
#    define LV_FONT_MONTSERRAT_8 CONFIG_LV_FONT_MONTSERRAT_8
#  else
#    define  LV_FONT_MONTSERRAT_8     0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_10
#  ifdef CONFIG_LV_FONT_MONTSERRAT_10
#    define LV_FONT_MONTSERRAT_10 CONFIG_LV_FONT_MONTSERRAT_10
#  else
#    define  LV_FONT_MONTSERRAT_10    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_12
#  ifdef CONFIG_LV_FONT_MONTSERRAT_12
#    define LV_FONT_MONTSERRAT_12 CONFIG_LV_FONT_MONTSERRAT_12
#  else
#    define  LV_FONT_MONTSERRAT_12    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_14
#  ifdef CONFIG_LV_FONT_MONTSERRAT_14
#    define LV_FONT_MONTSERRAT_14 CONFIG_LV_FONT_MONTSERRAT_14
#  else
#    define  LV_FONT_MONTSERRAT_14    1
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_16
#  ifdef CONFIG_LV_FONT_MONTSERRAT_16
#    define LV_FONT_MONTSERRAT_16 CONFIG_LV_FONT_MONTSERRAT_16
#  else
#    define  LV_FONT_MONTSERRAT_16    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_18
#  ifdef CONFIG_LV_FONT_MONTSERRAT_18
#    define LV_FONT_MONTSERRAT_18 CONFIG_LV_FONT_MONTSERRAT_18
#  else
#    define  LV_FONT_MONTSERRAT_18    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_20
#  ifdef CONFIG_LV_FONT_MONTSERRAT_20
#    define LV_FONT_MONTSERRAT_20 CONFIG_LV_FONT_MONTSERRAT_20
#  else
#    define  LV_FONT_MONTSERRAT_20    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_22
#  ifdef CONFIG_LV_FONT_MONTSERRAT_22
#    define LV_FONT_MONTSERRAT_22 CONFIG_LV_FONT_MONTSERRAT_22
#  else
#    define  LV_FONT_MONTSERRAT_22    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_24
#  ifdef CONFIG_LV_FONT_MONTSERRAT_24
#    define LV_FONT_MONTSERRAT_24 CONFIG_LV_FONT_MONTSERRAT_24
#  else
#    define  LV_FONT_MONTSERRAT_24    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_26
#  ifdef CONFIG_LV_FONT_MONTSERRAT_26
#    define LV_FONT_MONTSERRAT_26 CONFIG_LV_FONT_MONTSERRAT_26
#  else
#    define  LV_FONT_MONTSERRAT_26    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_28
#  ifdef CONFIG_LV_FONT_MONTSERRAT_28
#    define LV_FONT_MONTSERRAT_28 CONFIG_LV_FONT_MONTSERRAT_28
#  else
#    define  LV_FONT_MONTSERRAT_28    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_30
#  ifdef CONFIG_LV_FONT_MONTSERRAT_30
#    define LV_FONT_MONTSERRAT_30 CONFIG_LV_FONT_MONTSERRAT_30
#  else
#    define  LV_FONT_MONTSERRAT_30    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_32
#  ifdef CONFIG_LV_FONT_MONTSERRAT_32
#    define LV_FONT_MONTSERRAT_32 CONFIG_LV_FONT_MONTSERRAT_32
#  else
#    define  LV_FONT_MONTSERRAT_32    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_34
#  ifdef CONFIG_LV_FONT_MONTSERRAT_34
#    define LV_FONT_MONTSERRAT_34 CONFIG_LV_FONT_MONTSERRAT_34
#  else
#    define  LV_FONT_MONTSERRAT_34    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_36
#  ifdef CONFIG_LV_FONT_MONTSERRAT_36
#    define LV_FONT_MONTSERRAT_36 CONFIG_LV_FONT_MONTSERRAT_36
#  else
#    define  LV_FONT_MONTSERRAT_36    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_38
#  ifdef CONFIG_LV_FONT_MONTSERRAT_38
#    define LV_FONT_MONTSERRAT_38 CONFIG_LV_FONT_MONTSERRAT_38
#  else
#    define  LV_FONT_MONTSERRAT_38    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_40
#  ifdef CONFIG_LV_FONT_MONTSERRAT_40
#    define LV_FONT_MONTSERRAT_40 CONFIG_LV_FONT_MONTSERRAT_40
#  else
#    define  LV_FONT_MONTSERRAT_40    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_42
#  ifdef CONFIG_LV_FONT_MONTSERRAT_42
#    define LV_FONT_MONTSERRAT_42 CONFIG_LV_FONT_MONTSERRAT_42
#  else
#    define  LV_FONT_MONTSERRAT_42    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_44
#  ifdef CONFIG_LV_FONT_MONTSERRAT_44
#    define LV_FONT_MONTSERRAT_44 CONFIG_LV_FONT_MONTSERRAT_44
#  else
#    define  LV_FONT_MONTSERRAT_44    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_46
#  ifdef CONFIG_LV_FONT_MONTSERRAT_46
#    define LV_FONT_MONTSERRAT_46 CONFIG_LV_FONT_MONTSERRAT_46
#  else
#    define  LV_FONT_MONTSERRAT_46    0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_48
#  ifdef CONFIG_LV_FONT_MONTSERRAT_48
#    define LV_FONT_MONTSERRAT_48 CONFIG_LV_FONT_MONTSERRAT_48
#  else
#    define  LV_FONT_MONTSERRAT_48    0
#  endif
#endif

/* Demonstrate special features */
#ifndef LV_FONT_MONTSERRAT_12_SUBPX
#  ifdef CONFIG_LV_FONT_MONTSERRAT_12_SUBPX
#    define LV_FONT_MONTSERRAT_12_SUBPX CONFIG_LV_FONT_MONTSERRAT_12_SUBPX
#  else
#    define  LV_FONT_MONTSERRAT_12_SUBPX      0
#  endif
#endif
#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
#  ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
#    define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
#  else
#    define  LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#  endif
#endif
#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
#  ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
#    define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
#  else
#    define  LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, PErisan letters and all their forms*/
#  endif
#endif
#ifndef LV_FONT_SIMSUN_16_CJK
#  ifdef CONFIG_LV_FONT_SIMSUN_16_CJK
#    define LV_FONT_SIMSUN_16_CJK CONFIG_LV_FONT_SIMSUN_16_CJK
#  else
#    define  LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/
#  endif
#endif

/*Pixel perfect monospace font
 * http://pelulamu.net/unscii/ */
#ifndef LV_FONT_UNSCII_8
#  ifdef CONFIG_LV_FONT_UNSCII_8
#    define LV_FONT_UNSCII_8 CONFIG_LV_FONT_UNSCII_8
#  else
#    define  LV_FONT_UNSCII_8     0
#  endif
#endif

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2)
 */
#ifndef LV_FONT_CUSTOM_DECLARE
#  ifdef CONFIG_LV_FONT_CUSTOM_DECLARE
#    define LV_FONT_CUSTOM_DECLARE CONFIG_LV_FONT_CUSTOM_DECLARE
#  else
#    define  LV_FONT_CUSTOM_DECLARE
#  endif
#endif

/* Enable it if you have fonts with a lot of characters.
 * The limit depends on the font size, font face and bpp
 * but with > 10,000 characters if you see issues probably you need to enable it.*/
#ifndef LV_FONT_FMT_TXT_LARGE
#  ifdef CONFIG_LV_FONT_FMT_TXT_LARGE
#    define LV_FONT_FMT_TXT_LARGE CONFIG_LV_FONT_FMT_TXT_LARGE
#  else
#    define  LV_FONT_FMT_TXT_LARGE   0
#  endif
#endif

/* Enables/disables support for compressed fonts. If it's disabled, compressed
 * glyphs cannot be processed by the library and won't be rendered.
 */
#ifndef LV_USE_FONT_COMPRESSED
#  ifdef CONFIG_LV_USE_FONT_COMPRESSED
#    define LV_USE_FONT_COMPRESSED CONFIG_LV_USE_FONT_COMPRESSED
#  else
#    define  LV_USE_FONT_COMPRESSED 1
#  endif
#endif

/* Enable subpixel rendering */
#ifndef LV_USE_FONT_SUBPX
#  ifdef CONFIG_LV_USE_FONT_SUBPX
#    define LV_USE_FONT_SUBPX CONFIG_LV_USE_FONT_SUBPX
#  else
#    define  LV_USE_FONT_SUBPX 1
#  endif
#endif
#if LV_USE_FONT_SUBPX
/* Set the pixel order of the display.
 * Important only if "subpx fonts" are used.
 * With "normal" font it doesn't matter.
 */
#ifndef LV_FONT_SUBPX_BGR
#  ifdef CONFIG_LV_FONT_SUBPX_BGR
#    define LV_FONT_SUBPX_BGR CONFIG_LV_FONT_SUBPX_BGR
#  else
#    define  LV_FONT_SUBPX_BGR    0
#  endif
#endif
#endif

/*Declare the type of the user data of fonts (can be e.g. `void *`, `int`, `struct`)*/

/*================
 *  THEME USAGE
 *================*/

/*Always enable at least on theme*/

/* No theme, you can apply your styles as you need
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#ifndef LV_USE_THEME_EMPTY
#  ifdef CONFIG_LV_USE_THEME_EMPTY
#    define LV_USE_THEME_EMPTY CONFIG_LV_USE_THEME_EMPTY
#  else
#    define  LV_USE_THEME_EMPTY       1
#  endif
#endif

/*Simple to the create your theme based on it
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#ifndef LV_USE_THEME_TEMPLATE
#  ifdef CONFIG_LV_USE_THEME_TEMPLATE
#    define LV_USE_THEME_TEMPLATE CONFIG_LV_USE_THEME_TEMPLATE
#  else
#    define  LV_USE_THEME_TEMPLATE    1
#  endif
#endif

/* A fast and impressive theme.
 * Flags:
 * LV_THEME_MATERIAL_FLAG_LIGHT: light theme
 * LV_THEME_MATERIAL_FLAG_DARK: dark theme
 * LV_THEME_MATERIAL_FLAG_NO_TRANSITION: disable transitions (state change animations)
 * LV_THEME_MATERIAL_FLAG_NO_FOCUS: disable indication of focused state)
 * */
#ifndef LV_USE_THEME_MATERIAL
#  ifdef CONFIG_LV_USE_THEME_MATERIAL
#    define LV_USE_THEME_MATERIAL CONFIG_LV_USE_THEME_MATERIAL
#  else
#    define  LV_USE_THEME_MATERIAL    1
#  endif
#endif

/* Mono-color theme for monochrome displays.
 * If LV_THEME_DEFAULT_COLOR_PRIMARY is LV_COLOR_BLACK the
 * texts and borders will be black and the background will be
 * white. Else the colors are inverted.
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#ifndef LV_USE_THEME_MONO
#  ifdef CONFIG_LV_USE_THEME_MONO
#    define LV_USE_THEME_MONO CONFIG_LV_USE_THEME_MONO
#  else
#    define  LV_USE_THEME_MONO        1
#  endif
#endif

#ifndef LV_THEME_DEFAULT_INCLUDE
#  ifdef CONFIG_LV_THEME_DEFAULT_INCLUDE
#    define LV_THEME_DEFAULT_INCLUDE CONFIG_LV_THEME_DEFAULT_INCLUDE
#  else
#    define  LV_THEME_DEFAULT_INCLUDE            <stdint.h>      /*Include a header for the init. function*/
#  endif
#endif
#ifndef LV_THEME_DEFAULT_INIT
#  ifdef CONFIG_LV_THEME_DEFAULT_INIT
#    define LV_THEME_DEFAULT_INIT CONFIG_LV_THEME_DEFAULT_INIT
#  else
#    define  LV_THEME_DEFAULT_INIT               lv_theme_material_init
#  endif
#endif
#ifndef LV_THEME_DEFAULT_COLOR_PRIMARY
#  ifdef CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY
#    define LV_THEME_DEFAULT_COLOR_PRIMARY CONFIG_LV_THEME_DEFAULT_COLOR_PRIMARY
#  else
#    define  LV_THEME_DEFAULT_COLOR_PRIMARY      lv_color_hex(0x01a2b1)
#  endif
#endif
#ifndef LV_THEME_DEFAULT_COLOR_SECONDARY
#  ifdef CONFIG_LV_THEME_DEFAULT_COLOR_SECONDARY
#    define LV_THEME_DEFAULT_COLOR_SECONDARY CONFIG_LV_THEME_DEFAULT_COLOR_SECONDARY
#  else
#    define  LV_THEME_DEFAULT_COLOR_SECONDARY    lv_color_hex(0x44d1b6)
#  endif
#endif
#ifndef LV_THEME_DEFAULT_FLAG
#  ifdef CONFIG_LV_THEME_DEFAULT_FLAG
#    define LV_THEME_DEFAULT_FLAG CONFIG_LV_THEME_DEFAULT_FLAG
#  else
#    define  LV_THEME_DEFAULT_FLAG               LV_THEME_MATERIAL_FLAG_LIGHT
#  endif
#endif
#ifndef LV_THEME_DEFAULT_FONT_SMALL
#  ifdef CONFIG_LV_THEME_DEFAULT_FONT_SMALL
#    define LV_THEME_DEFAULT_FONT_SMALL CONFIG_LV_THEME_DEFAULT_FONT_SMALL
#  else
#    define  LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_14
#  endif
#endif
#ifndef LV_THEME_DEFAULT_FONT_NORMAL
#  ifdef CONFIG_LV_THEME_DEFAULT_FONT_NORMAL
#    define LV_THEME_DEFAULT_FONT_NORMAL CONFIG_LV_THEME_DEFAULT_FONT_NORMAL
#  else
#    define  LV_THEME_DEFAULT_FONT_NORMAL        &lv_font_montserrat_14
#  endif
#endif
#ifndef LV_THEME_DEFAULT_FONT_SUBTITLE
#  ifdef CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE
#    define LV_THEME_DEFAULT_FONT_SUBTITLE CONFIG_LV_THEME_DEFAULT_FONT_SUBTITLE
#  else
#    define  LV_THEME_DEFAULT_FONT_SUBTITLE      &lv_font_montserrat_14
#  endif
#endif
#ifndef LV_THEME_DEFAULT_FONT_TITLE
#  ifdef CONFIG_LV_THEME_DEFAULT_FONT_TITLE
#    define LV_THEME_DEFAULT_FONT_TITLE CONFIG_LV_THEME_DEFAULT_FONT_TITLE
#  else
#    define  LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_14
#  endif
#endif

/*=================
 *  Text settings
 *=================*/

/* Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 * */
#ifndef LV_TXT_ENC
#  ifdef CONFIG_LV_TXT_ENC
#    define LV_TXT_ENC CONFIG_LV_TXT_ENC
#  else
#    define  LV_TXT_ENC LV_TXT_ENC_UTF8
#  endif
#endif

 /*Can break (wrap) texts on these chars*/
#ifndef LV_TXT_BREAK_CHARS
#  ifdef CONFIG_LV_TXT_BREAK_CHARS
#    define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS
#  else
#    define  LV_TXT_BREAK_CHARS                  " ,.;:-_"
#  endif
#endif

/* If a word is at least this long, will break wherever "prettiest"
 * To disable, set to a value <= 0 */
#ifndef LV_TXT_LINE_BREAK_LONG_LEN
#  ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
#    define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
#  else
#    define  LV_TXT_LINE_BREAK_LONG_LEN          0
#  endif
#endif

/* Minimum number of characters in a long word to put on a line before a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
#  ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
#    define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
#  else
#    define  LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3
#  endif
#endif

/* Minimum number of characters in a long word to put on a line after a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
#  ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
#    define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
#  else
#    define  LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
#  endif
#endif

/* The control character to use for signalling text recoloring. */
#ifndef LV_TXT_COLOR_CMD
#  ifdef CONFIG_LV_TXT_COLOR_CMD
#    define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
#  else
#    define  LV_TXT_COLOR_CMD "#"
#  endif
#endif

/* Support bidirectional texts.
 * Allows mixing Left-to-Right and Right-to-Left texts.
 * The direction will be processed according to the Unicode Bidirectioanl Algorithm:
 * https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#ifndef LV_USE_BIDI
#  ifdef CONFIG_LV_USE_BIDI
#    define LV_USE_BIDI CONFIG_LV_USE_BIDI
#  else
#    define  LV_USE_BIDI     0
#  endif
#endif
#if LV_USE_BIDI
/* Set the default direction. Supported values:
 * `LV_BIDI_DIR_LTR` Left-to-Right
 * `LV_BIDI_DIR_RTL` Right-to-Left
 * `LV_BIDI_DIR_AUTO` detect texts base direction */
#ifndef LV_BIDI_BASE_DIR_DEF
#  ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
#    define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
#  else
#    define  LV_BIDI_BASE_DIR_DEF  LV_BIDI_DIR_AUTO
#  endif
#endif
#endif

/* Enable Arabic/Persian processing
 * In these languages characters should be replaced with
 * an other form based on their position in the text */
#ifndef LV_USE_ARABIC_PERSIAN_CHARS
#  ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
#    define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
#  else
#    define  LV_USE_ARABIC_PERSIAN_CHARS 0
#  endif
#endif

/*Change the built in (v)snprintf functions*/
#ifndef LV_SPRINTF_CUSTOM
#  ifdef CONFIG_LV_SPRINTF_CUSTOM
#    define LV_SPRINTF_CUSTOM CONFIG_LV_SPRINTF_CUSTOM
#  else
#    define  LV_SPRINTF_CUSTOM   0
#  endif
#endif
#if LV_SPRINTF_CUSTOM
#ifndef LV_SPRINTF_INCLUDE
#  ifdef CONFIG_LV_SPRINTF_INCLUDE
#    define LV_SPRINTF_INCLUDE CONFIG_LV_SPRINTF_INCLUDE
#  else
#    define  LV_SPRINTF_INCLUDE <stdio.h>
#  endif
#endif
#ifndef lv_snprintf
#  ifdef CONFIG_lv_snprintf
#    define lv_snprintf CONFIG_lv_snprintf
#  else
#    define  lv_snprintf     snprintf
#  endif
#endif
#ifndef lv_vsnprintf
#  ifdef CONFIG_lv_vsnprintf
#    define lv_vsnprintf CONFIG_lv_vsnprintf
#  else
#    define  lv_vsnprintf    vsnprintf
#  endif
#endif
#else   /*!LV_SPRINTF_CUSTOM*/
#ifndef LV_SPRINTF_DISABLE_FLOAT
#  ifdef CONFIG_LV_SPRINTF_DISABLE_FLOAT
#    define LV_SPRINTF_DISABLE_FLOAT CONFIG_LV_SPRINTF_DISABLE_FLOAT
#  else
#    define  LV_SPRINTF_DISABLE_FLOAT 1
#  endif
#endif
#endif  /*LV_SPRINTF_CUSTOM*/

/*===================
 *  LV_OBJ SETTINGS
 *==================*/

#if LV_USE_USER_DATA
/*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/
/*Provide a function to free user data*/
#ifndef LV_USE_USER_DATA_FREE
#  ifdef CONFIG_LV_USE_USER_DATA_FREE
#    define LV_USE_USER_DATA_FREE CONFIG_LV_USE_USER_DATA_FREE
#  else
#    define  LV_USE_USER_DATA_FREE 0
#  endif
#endif
#if LV_USE_USER_DATA_FREE
#ifndef LV_USER_DATA_FREE_INCLUDE
#  ifdef CONFIG_LV_USER_DATA_FREE_INCLUDE
#    define LV_USER_DATA_FREE_INCLUDE CONFIG_LV_USER_DATA_FREE_INCLUDE
#  else
#    define  LV_USER_DATA_FREE_INCLUDE  "something.h"  /*Header for user data free function*/
#  endif
#endif
/* Function prototype : void user_data_free(lv_obj_t * obj); */
#ifndef LV_USER_DATA_FREE
#  ifdef CONFIG_LV_USER_DATA_FREE
#    define LV_USER_DATA_FREE CONFIG_LV_USER_DATA_FREE
#  else
#    define  LV_USER_DATA_FREE  (user_data_free)       /*Invoking for user data free function*/
#  endif
#endif
#endif
#endif

/*1: enable `lv_obj_realign()` based on `lv_obj_align()` parameters*/
#ifndef LV_USE_OBJ_REALIGN
#  ifdef CONFIG_LV_USE_OBJ_REALIGN
#    define LV_USE_OBJ_REALIGN CONFIG_LV_USE_OBJ_REALIGN
#  else
#    define  LV_USE_OBJ_REALIGN          1
#  endif
#endif

/* Enable to make the object clickable on a larger area.
 * LV_EXT_CLICK_AREA_OFF or 0: Disable this feature
 * LV_EXT_CLICK_AREA_TINY: The extra area can be adjusted horizontally and vertically (0..255 px)
 * LV_EXT_CLICK_AREA_FULL: The extra area can be adjusted in all 4 directions (-32k..+32k px)
 */
#ifndef LV_USE_EXT_CLICK_AREA
#  ifdef CONFIG_LV_USE_EXT_CLICK_AREA
#    define LV_USE_EXT_CLICK_AREA CONFIG_LV_USE_EXT_CLICK_AREA
#  else
#    define  LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_TINY
#  endif
#endif

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.lvgl.com/#Object-types
 */

/*Arc (dependencies: -)*/
#ifndef LV_USE_ARC
#  ifdef CONFIG_LV_USE_ARC
#    define LV_USE_ARC CONFIG_LV_USE_ARC
#  else
#    define  LV_USE_ARC      1
#  endif
#endif

/*Bar (dependencies: -)*/
#ifndef LV_USE_BAR
#  ifdef CONFIG_LV_USE_BAR
#    define LV_USE_BAR CONFIG_LV_USE_BAR
#  else
#    define  LV_USE_BAR      1
#  endif
#endif

/*Button (dependencies: lv_cont*/
#ifndef LV_USE_BTN
#  ifdef CONFIG_LV_USE_BTN
#    define LV_USE_BTN CONFIG_LV_USE_BTN
#  else
#    define  LV_USE_BTN      1
#  endif
#endif

/*Button matrix (dependencies: -)*/
#ifndef LV_USE_BTNMATRIX
#  ifdef CONFIG_LV_USE_BTNMATRIX
#    define LV_USE_BTNMATRIX CONFIG_LV_USE_BTNMATRIX
#  else
#    define  LV_USE_BTNMATRIX     1
#  endif
#endif

/*Calendar (dependencies: -)*/
#ifndef LV_USE_CALENDAR
#  ifdef CONFIG_LV_USE_CALENDAR
#    define LV_USE_CALENDAR CONFIG_LV_USE_CALENDAR
#  else
#    define  LV_USE_CALENDAR 1
#  endif
#endif
#if LV_USE_CALENDAR
#ifndef LV_CALENDAR_WEEK_STARTS_MONDAY
#  ifdef CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
#    define LV_CALENDAR_WEEK_STARTS_MONDAY CONFIG_LV_CALENDAR_WEEK_STARTS_MONDAY
#  else
#    define  LV_CALENDAR_WEEK_STARTS_MONDAY    0
#  endif
#endif
#endif

/*Canvas (dependencies: lv_img)*/
#ifndef LV_USE_CANVAS
#  ifdef CONFIG_LV_USE_CANVAS
#    define LV_USE_CANVAS CONFIG_LV_USE_CANVAS
#  else
#    define  LV_USE_CANVAS   1
#  endif
#endif

/*Check box (dependencies: lv_btn, lv_label)*/
#ifndef LV_USE_CHECKBOX
#  ifdef CONFIG_LV_USE_CHECKBOX
#    define LV_USE_CHECKBOX CONFIG_LV_USE_CHECKBOX
#  else
#    define  LV_USE_CHECKBOX       1
#  endif
#endif

/*Chart (dependencies: -)*/
#ifndef LV_USE_CHART
#  ifdef CONFIG_LV_USE_CHART
#    define LV_USE_CHART CONFIG_LV_USE_CHART
#  else
#    define  LV_USE_CHART    1
#  endif
#endif
#if LV_USE_CHART
#ifndef LV_CHART_AXIS_TICK_LABEL_MAX_LEN
#  ifdef CONFIG_LV_CHART_AXIS_TICK_LABEL_MAX_LEN
#    define LV_CHART_AXIS_TICK_LABEL_MAX_LEN CONFIG_LV_CHART_AXIS_TICK_LABEL_MAX_LEN
#  else
#    define  LV_CHART_AXIS_TICK_LABEL_MAX_LEN    256
#  endif
#endif
#endif

/*Container (dependencies: -*/
#ifndef LV_USE_CONT
#  ifdef CONFIG_LV_USE_CONT
#    define LV_USE_CONT CONFIG_LV_USE_CONT
#  else
#    define  LV_USE_CONT     1
#  endif
#endif

/*Color picker (dependencies: -*/
#ifndef LV_USE_CPICKER
#  ifdef CONFIG_LV_USE_CPICKER
#    define LV_USE_CPICKER CONFIG_LV_USE_CPICKER
#  else
#    define  LV_USE_CPICKER   1
#  endif
#endif

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#ifndef LV_USE_DROPDOWN
#  ifdef CONFIG_LV_USE_DROPDOWN
#    define LV_USE_DROPDOWN CONFIG_LV_USE_DROPDOWN
#  else
#    define  LV_USE_DROPDOWN    1
#  endif
#endif
#if LV_USE_DROPDOWN != 0
/*Open and close default animation time [ms] (0: no animation)*/
#ifndef LV_DROPDOWN_DEF_ANIM_TIME
#  ifdef CONFIG_LV_DROPDOWN_DEF_ANIM_TIME
#    define LV_DROPDOWN_DEF_ANIM_TIME CONFIG_LV_DROPDOWN_DEF_ANIM_TIME
#  else
#    define  LV_DROPDOWN_DEF_ANIM_TIME     200
#  endif
#endif
#endif

/*Gauge (dependencies:lv_bar, lv_linemeter)*/
#ifndef LV_USE_GAUGE
#  ifdef CONFIG_LV_USE_GAUGE
#    define LV_USE_GAUGE CONFIG_LV_USE_GAUGE
#  else
#    define  LV_USE_GAUGE    1
#  endif
#endif

/*Image (dependencies: lv_label*/
#ifndef LV_USE_IMG
#  ifdef CONFIG_LV_USE_IMG
#    define LV_USE_IMG CONFIG_LV_USE_IMG
#  else
#    define  LV_USE_IMG      1
#  endif
#endif

/*Image Button (dependencies: lv_btn*/
#ifndef LV_USE_IMGBTN
#  ifdef CONFIG_LV_USE_IMGBTN
#    define LV_USE_IMGBTN CONFIG_LV_USE_IMGBTN
#  else
#    define  LV_USE_IMGBTN   1
#  endif
#endif
#if LV_USE_IMGBTN
/*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#ifndef LV_IMGBTN_TILED
#  ifdef CONFIG_LV_IMGBTN_TILED
#    define LV_IMGBTN_TILED CONFIG_LV_IMGBTN_TILED
#  else
#    define  LV_IMGBTN_TILED 0
#  endif
#endif
#endif

/*Keyboard (dependencies: lv_btnm)*/
#ifndef LV_USE_KEYBOARD
#  ifdef CONFIG_LV_USE_KEYBOARD
#    define LV_USE_KEYBOARD CONFIG_LV_USE_KEYBOARD
#  else
#    define  LV_USE_KEYBOARD       1
#  endif
#endif

/*Label (dependencies: -*/
#ifndef LV_USE_LABEL
#  ifdef CONFIG_LV_USE_LABEL
#    define LV_USE_LABEL CONFIG_LV_USE_LABEL
#  else
#    define  LV_USE_LABEL    1
#  endif
#endif
#if LV_USE_LABEL != 0
/*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
#ifndef LV_LABEL_DEF_SCROLL_SPEED
#  ifdef CONFIG_LV_LABEL_DEF_SCROLL_SPEED
#    define LV_LABEL_DEF_SCROLL_SPEED CONFIG_LV_LABEL_DEF_SCROLL_SPEED
#  else
#    define  LV_LABEL_DEF_SCROLL_SPEED       25
#  endif
#endif

/* Waiting period at beginning/end of animation cycle */
#ifndef LV_LABEL_WAIT_CHAR_COUNT
#  ifdef CONFIG_LV_LABEL_WAIT_CHAR_COUNT
#    define LV_LABEL_WAIT_CHAR_COUNT CONFIG_LV_LABEL_WAIT_CHAR_COUNT
#  else
#    define  LV_LABEL_WAIT_CHAR_COUNT        3
#  endif
#endif

/*Enable selecting text of the label */
#ifndef LV_LABEL_TEXT_SEL
#  ifdef CONFIG_LV_LABEL_TEXT_SEL
#    define LV_LABEL_TEXT_SEL CONFIG_LV_LABEL_TEXT_SEL
#  else
#    define  LV_LABEL_TEXT_SEL               0
#  endif
#endif

/*Store extra some info in labels (12 bytes) to speed up drawing of very long texts*/
#ifndef LV_LABEL_LONG_TXT_HINT
#  ifdef CONFIG_LV_LABEL_LONG_TXT_HINT
#    define LV_LABEL_LONG_TXT_HINT CONFIG_LV_LABEL_LONG_TXT_HINT
#  else
#    define  LV_LABEL_LONG_TXT_HINT          0
#  endif
#endif
#endif

/*LED (dependencies: -)*/
#ifndef LV_USE_LED
#  ifdef CONFIG_LV_USE_LED
#    define LV_USE_LED CONFIG_LV_USE_LED
#  else
#    define  LV_USE_LED      1
#  endif
#endif
#if LV_USE_LED
#ifndef LV_LED_BRIGHT_MIN
#  ifdef CONFIG_LV_LED_BRIGHT_MIN
#    define LV_LED_BRIGHT_MIN CONFIG_LV_LED_BRIGHT_MIN
#  else
#    define  LV_LED_BRIGHT_MIN  120      /*Minimal brightness*/
#  endif
#endif
#ifndef LV_LED_BRIGHT_MAX
#  ifdef CONFIG_LV_LED_BRIGHT_MAX
#    define LV_LED_BRIGHT_MAX CONFIG_LV_LED_BRIGHT_MAX
#  else
#    define  LV_LED_BRIGHT_MAX  255     /*Maximal brightness*/
#  endif
#endif
#endif

/*Line (dependencies: -*/
#ifndef LV_USE_LINE
#  ifdef CONFIG_LV_USE_LINE
#    define LV_USE_LINE CONFIG_LV_USE_LINE
#  else
#    define  LV_USE_LINE     1
#  endif
#endif

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#ifndef LV_USE_LIST
#  ifdef CONFIG_LV_USE_LIST
#    define LV_USE_LIST CONFIG_LV_USE_LIST
#  else
#    define  LV_USE_LIST     1
#  endif
#endif
#if LV_USE_LIST != 0
/*Default animation time of focusing to a list element [ms] (0: no animation)  */
#ifndef LV_LIST_DEF_ANIM_TIME
#  ifdef CONFIG_LV_LIST_DEF_ANIM_TIME
#    define LV_LIST_DEF_ANIM_TIME CONFIG_LV_LIST_DEF_ANIM_TIME
#  else
#    define  LV_LIST_DEF_ANIM_TIME  100
#  endif
#endif
#endif

/*Line meter (dependencies: *;)*/
#ifndef LV_USE_LINEMETER
#  ifdef CONFIG_LV_USE_LINEMETER
#    define LV_USE_LINEMETER CONFIG_LV_USE_LINEMETER
#  else
#    define  LV_USE_LINEMETER   1
#  endif
#endif
#if LV_USE_LINEMETER
/* Draw line more precisely at cost of performance.
 * Useful if there are lot of lines any minor are visible
 * 0: No extra precision
 * 1: Some extra precision
 * 2: Best precision
 */
#ifndef LV_LINEMETER_PRECISE
#  ifdef CONFIG_LV_LINEMETER_PRECISE
#    define LV_LINEMETER_PRECISE CONFIG_LV_LINEMETER_PRECISE
#  else
#    define  LV_LINEMETER_PRECISE    1
#  endif
#endif
#endif

/*Mask (dependencies: -)*/
#ifndef LV_USE_OBJMASK
#  ifdef CONFIG_LV_USE_OBJMASK
#    define LV_USE_OBJMASK CONFIG_LV_USE_OBJMASK
#  else
#    define  LV_USE_OBJMASK  1
#  endif
#endif

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#ifndef LV_USE_MSGBOX
#  ifdef CONFIG_LV_USE_MSGBOX
#    define LV_USE_MSGBOX CONFIG_LV_USE_MSGBOX
#  else
#    define  LV_USE_MSGBOX     1
#  endif
#endif

/*Page (dependencies: lv_cont)*/
#ifndef LV_USE_PAGE
#  ifdef CONFIG_LV_USE_PAGE
#    define LV_USE_PAGE CONFIG_LV_USE_PAGE
#  else
#    define  LV_USE_PAGE     1
#  endif
#endif
#if LV_USE_PAGE != 0
/*Focus default animation time [ms] (0: no animation)*/
#ifndef LV_PAGE_DEF_ANIM_TIME
#  ifdef CONFIG_LV_PAGE_DEF_ANIM_TIME
#    define LV_PAGE_DEF_ANIM_TIME CONFIG_LV_PAGE_DEF_ANIM_TIME
#  else
#    define  LV_PAGE_DEF_ANIM_TIME     400
#  endif
#endif
#endif

/*Preload (dependencies: lv_arc, lv_anim)*/
#ifndef LV_USE_SPINNER
#  ifdef CONFIG_LV_USE_SPINNER
#    define LV_USE_SPINNER CONFIG_LV_USE_SPINNER
#  else
#    define  LV_USE_SPINNER      1
#  endif
#endif
#if LV_USE_SPINNER != 0
#ifndef LV_SPINNER_DEF_ARC_LENGTH
#  ifdef CONFIG_LV_SPINNER_DEF_ARC_LENGTH
#    define LV_SPINNER_DEF_ARC_LENGTH CONFIG_LV_SPINNER_DEF_ARC_LENGTH
#  else
#    define  LV_SPINNER_DEF_ARC_LENGTH   60      /*[deg]*/
#  endif
#endif
#ifndef LV_SPINNER_DEF_SPIN_TIME
#  ifdef CONFIG_LV_SPINNER_DEF_SPIN_TIME
#    define LV_SPINNER_DEF_SPIN_TIME CONFIG_LV_SPINNER_DEF_SPIN_TIME
#  else
#    define  LV_SPINNER_DEF_SPIN_TIME    1000    /*[ms]*/
#  endif
#endif
#ifndef LV_SPINNER_DEF_ANIM
#  ifdef CONFIG_LV_SPINNER_DEF_ANIM
#    define LV_SPINNER_DEF_ANIM CONFIG_LV_SPINNER_DEF_ANIM
#  else
#    define  LV_SPINNER_DEF_ANIM         LV_SPINNER_TYPE_SPINNING_ARC
#  endif
#endif
#endif

/*Roller (dependencies: lv_ddlist)*/
#ifndef LV_USE_ROLLER
#  ifdef CONFIG_LV_USE_ROLLER
#    define LV_USE_ROLLER CONFIG_LV_USE_ROLLER
#  else
#    define  LV_USE_ROLLER    1
#  endif
#endif
#if LV_USE_ROLLER != 0
/*Focus animation time [ms] (0: no animation)*/
#ifndef LV_ROLLER_DEF_ANIM_TIME
#  ifdef CONFIG_LV_ROLLER_DEF_ANIM_TIME
#    define LV_ROLLER_DEF_ANIM_TIME CONFIG_LV_ROLLER_DEF_ANIM_TIME
#  else
#    define  LV_ROLLER_DEF_ANIM_TIME     200
#  endif
#endif

/*Number of extra "pages" when the roller is infinite*/
#ifndef LV_ROLLER_INF_PAGES
#  ifdef CONFIG_LV_ROLLER_INF_PAGES
#    define LV_ROLLER_INF_PAGES CONFIG_LV_ROLLER_INF_PAGES
#  else
#    define  LV_ROLLER_INF_PAGES         7
#  endif
#endif
#endif

/*Slider (dependencies: lv_bar)*/
#ifndef LV_USE_SLIDER
#  ifdef CONFIG_LV_USE_SLIDER
#    define LV_USE_SLIDER CONFIG_LV_USE_SLIDER
#  else
#    define  LV_USE_SLIDER    1
#  endif
#endif

/*Spinbox (dependencies: lv_ta)*/
#ifndef LV_USE_SPINBOX
#  ifdef CONFIG_LV_USE_SPINBOX
#    define LV_USE_SPINBOX CONFIG_LV_USE_SPINBOX
#  else
#    define  LV_USE_SPINBOX       1
#  endif
#endif

/*Switch (dependencies: lv_slider)*/
#ifndef LV_USE_SWITCH
#  ifdef CONFIG_LV_USE_SWITCH
#    define LV_USE_SWITCH CONFIG_LV_USE_SWITCH
#  else
#    define  LV_USE_SWITCH       1
#  endif
#endif

/*Text area (dependencies: lv_label, lv_page)*/
#ifndef LV_USE_TEXTAREA
#  ifdef CONFIG_LV_USE_TEXTAREA
#    define LV_USE_TEXTAREA CONFIG_LV_USE_TEXTAREA
#  else
#    define  LV_USE_TEXTAREA       1
#  endif
#endif
#if LV_USE_TEXTAREA != 0
#ifndef LV_TEXTAREA_DEF_CURSOR_BLINK_TIME
#  ifdef CONFIG_LV_TEXTAREA_DEF_CURSOR_BLINK_TIME
#    define LV_TEXTAREA_DEF_CURSOR_BLINK_TIME CONFIG_LV_TEXTAREA_DEF_CURSOR_BLINK_TIME
#  else
#    define  LV_TEXTAREA_DEF_CURSOR_BLINK_TIME 400     /*ms*/
#  endif
#endif
#ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
#  ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
#    define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
#  else
#    define  LV_TEXTAREA_DEF_PWD_SHOW_TIME     1500    /*ms*/
#  endif
#endif
#endif

/*Table (dependencies: lv_label)*/
#ifndef LV_USE_TABLE
#  ifdef CONFIG_LV_USE_TABLE
#    define LV_USE_TABLE CONFIG_LV_USE_TABLE
#  else
#    define  LV_USE_TABLE    1
#  endif
#endif
#if LV_USE_TABLE
#ifndef LV_TABLE_COL_MAX
#  ifdef CONFIG_LV_TABLE_COL_MAX
#    define LV_TABLE_COL_MAX CONFIG_LV_TABLE_COL_MAX
#  else
#    define  LV_TABLE_COL_MAX    12
#  endif
#endif
#ifndef LV_TABLE_CELL_STYLE_CNT
#  ifdef CONFIG_LV_TABLE_CELL_STYLE_CNT
#    define LV_TABLE_CELL_STYLE_CNT CONFIG_LV_TABLE_CELL_STYLE_CNT
#  else
#    define  LV_TABLE_CELL_STYLE_CNT 4
#  endif
#endif
#endif


/*Tab (dependencies: lv_page, lv_btnm)*/
#ifndef LV_USE_TABVIEW
#  ifdef CONFIG_LV_USE_TABVIEW
#    define LV_USE_TABVIEW CONFIG_LV_USE_TABVIEW
#  else
#    define  LV_USE_TABVIEW      1
#  endif
#endif
#  if LV_USE_TABVIEW != 0
/*Time of slide animation [ms] (0: no animation)*/
#ifndef LV_TABVIEW_DEF_ANIM_TIME
#  ifdef CONFIG_LV_TABVIEW_DEF_ANIM_TIME
#    define LV_TABVIEW_DEF_ANIM_TIME CONFIG_LV_TABVIEW_DEF_ANIM_TIME
#  else
#    define  LV_TABVIEW_DEF_ANIM_TIME    300
#  endif
#endif
#endif

/*Tileview (dependencies: lv_page) */
#ifndef LV_USE_TILEVIEW
#  ifdef CONFIG_LV_USE_TILEVIEW
#    define LV_USE_TILEVIEW CONFIG_LV_USE_TILEVIEW
#  else
#    define  LV_USE_TILEVIEW     1
#  endif
#endif
#if LV_USE_TILEVIEW
/*Time of slide animation [ms] (0: no animation)*/
#ifndef LV_TILEVIEW_DEF_ANIM_TIME
#  ifdef CONFIG_LV_TILEVIEW_DEF_ANIM_TIME
#    define LV_TILEVIEW_DEF_ANIM_TIME CONFIG_LV_TILEVIEW_DEF_ANIM_TIME
#  else
#    define  LV_TILEVIEW_DEF_ANIM_TIME   300
#  endif
#endif
#endif

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#ifndef LV_USE_WIN
#  ifdef CONFIG_LV_USE_WIN
#    define LV_USE_WIN CONFIG_LV_USE_WIN
#  else
#    define  LV_USE_WIN      1
#  endif
#endif

/*==================
 * Non-user section
 *==================*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
#ifndef _CRT_SECURE_NO_WARNINGS
#  ifdef CONFIG__CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS CONFIG__CRT_SECURE_NO_WARNINGS
#  else
#    define  _CRT_SECURE_NO_WARNINGS
#  endif
#endif
#endif



/*If running without lv_conf.h add typdesf with default value*/
#if defined(LV_CONF_SKIP) || defined(CONFIG_LV_CONF_SKIP)

  /* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
  typedef int16_t lv_coord_t;

#  if LV_USE_ANIMATION
  /*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/
  typedef void * lv_anim_user_data_t;
#  endif

#  if LV_USE_GROUP
  typedef void * lv_group_user_data_t;
#  endif

#  if LV_USE_FILESYSTEM
  typedef void * lv_fs_drv_user_data_t;
#  endif

  typedef void * lv_img_decoder_user_data_t;

  typedef void * lv_disp_drv_user_data_t;             /*Type of user data in the display driver*/
  typedef void * lv_indev_drv_user_data_t;            /*Type of user data in the input device driver*/

  typedef void * lv_font_user_data_t;

#  if LV_USE_USER_DATA
  typedef void * lv_obj_user_data_t;
#  endif

#endif

#endif  /*LV_CONF_INTERNAL_H*/

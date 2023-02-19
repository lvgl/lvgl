/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_internal.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_INTERNAL_H
#define LV_CONF_INTERNAL_H
/* clang-format off */

#include <stdint.h>

/* Handle special Kconfig options */
#ifndef LV_KCONFIG_IGNORE
    #include "lv_conf_kconfig.h"
    #ifdef CONFIG_LV_CONF_SKIP
        #define LV_CONF_SKIP
    #endif
#endif

/*If "lv_conf.h" is available from here try to use it later.*/
#ifdef __has_include
    #if __has_include("lv_conf.h")
        #ifndef LV_CONF_INCLUDE_SIMPLE
            #define LV_CONF_INCLUDE_SIMPLE
        #endif
    #endif
#endif

/*If lv_conf.h is not skipped include it*/
#ifndef LV_CONF_SKIP
    #ifdef LV_CONF_PATH                           /*If there is a path defined for lv_conf.h use it*/
        #define __LV_TO_STR_AUX(x) #x
        #define __LV_TO_STR(x) __LV_TO_STR_AUX(x)
        #include __LV_TO_STR(LV_CONF_PATH)
        #undef __LV_TO_STR_AUX
        #undef __LV_TO_STR
    #elif defined(LV_CONF_INCLUDE_SIMPLE)         /*Or simply include lv_conf.h is enabled*/
        #include "lv_conf.h"
    #else
        #include "../../lv_conf.h"                /*Else assume lv_conf.h is next to the lvgl folder*/
    #endif
    #if !defined(LV_CONF_H) && !defined(LV_CONF_SUPPRESS_DEFINE_CHECK)
        /* #include will sometimes silently fail when __has_include is used */
        /* https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80753 */
        #pragma message("Possible failure to include lv_conf.h, please read the comment in this file if you get errors")
    #endif
#endif

#ifdef CONFIG_LV_COLOR_DEPTH
    #define _LV_KCONFIG_PRESENT
#endif

/*----------------------------------
 * Start parsing lv_conf_template.h
 -----------------------------------*/

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#ifndef LV_COLOR_DEPTH
    #ifdef CONFIG_LV_COLOR_DEPTH
        #define LV_COLOR_DEPTH CONFIG_LV_COLOR_DEPTH
    #else
        #define LV_COLOR_DEPTH 16
    #endif
#endif

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
#ifndef LV_COLOR_16_SWAP
    #ifdef CONFIG_LV_COLOR_16_SWAP
        #define LV_COLOR_16_SWAP CONFIG_LV_COLOR_16_SWAP
    #else
        #define LV_COLOR_16_SWAP 0
    #endif
#endif

/*Enable features to draw on transparent background.
 *It's required if opa, and transform_* style properties are used.
 *Can be also used if the UI is above another layer, e.g. an OSD menu or video player.*/
#ifndef LV_COLOR_SCREEN_TRANSP
    #ifdef CONFIG_LV_COLOR_SCREEN_TRANSP
        #define LV_COLOR_SCREEN_TRANSP CONFIG_LV_COLOR_SCREEN_TRANSP
    #else
        #define LV_COLOR_SCREEN_TRANSP 0
    #endif
#endif

/* Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 * 0: round down, 64: round up from x.75, 128: round up from half, 192: round up from x.25, 254: round up */
#ifndef LV_COLOR_MIX_ROUND_OFS
    #ifdef CONFIG_LV_COLOR_MIX_ROUND_OFS
        #define LV_COLOR_MIX_ROUND_OFS CONFIG_LV_COLOR_MIX_ROUND_OFS
    #else
        #define LV_COLOR_MIX_ROUND_OFS 0
    #endif
#endif

/*Images pixels with this color will not be drawn if they are chroma keyed)*/
#ifndef LV_COLOR_CHROMA_KEY
    #ifdef CONFIG_LV_COLOR_CHROMA_KEY
        #define LV_COLOR_CHROMA_KEY CONFIG_LV_COLOR_CHROMA_KEY
    #else
        #define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*pure green*/
    #endif
#endif

/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
#ifndef LV_MEM_CUSTOM
    #ifdef CONFIG_LV_MEM_CUSTOM
        #define LV_MEM_CUSTOM CONFIG_LV_MEM_CUSTOM
    #else
        #define LV_MEM_CUSTOM 0
    #endif
#endif
#if LV_MEM_CUSTOM == 0
    /*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
    #ifndef LV_MEM_SIZE
        #ifdef CONFIG_LV_MEM_SIZE
            #define LV_MEM_SIZE CONFIG_LV_MEM_SIZE
        #else
            #define LV_MEM_SIZE (48U * 1024U)          /*[bytes]*/
        #endif
    #endif

    /*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
    #ifndef LV_MEM_ADR
        #ifdef CONFIG_LV_MEM_ADR
            #define LV_MEM_ADR CONFIG_LV_MEM_ADR
        #else
            #define LV_MEM_ADR 0     /*0: unused*/
        #endif
    #endif
    /*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc*/
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

#else       /*LV_MEM_CUSTOM*/
    #ifndef LV_MEM_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_MEM_CUSTOM_INCLUDE
            #define LV_MEM_CUSTOM_INCLUDE CONFIG_LV_MEM_CUSTOM_INCLUDE
        #else
            #define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
        #endif
    #endif
    #ifndef LV_MEM_CUSTOM_ALLOC
        #ifdef CONFIG_LV_MEM_CUSTOM_ALLOC
            #define LV_MEM_CUSTOM_ALLOC CONFIG_LV_MEM_CUSTOM_ALLOC
        #else
            #define LV_MEM_CUSTOM_ALLOC   malloc
        #endif
    #endif
    #ifndef LV_MEM_CUSTOM_FREE
        #ifdef CONFIG_LV_MEM_CUSTOM_FREE
            #define LV_MEM_CUSTOM_FREE CONFIG_LV_MEM_CUSTOM_FREE
        #else
            #define LV_MEM_CUSTOM_FREE    free
        #endif
    #endif
    #ifndef LV_MEM_CUSTOM_REALLOC
        #ifdef CONFIG_LV_MEM_CUSTOM_REALLOC
            #define LV_MEM_CUSTOM_REALLOC CONFIG_LV_MEM_CUSTOM_REALLOC
        #else
            #define LV_MEM_CUSTOM_REALLOC realloc
        #endif
    #endif
#endif     /*LV_MEM_CUSTOM*/

/*Number of the intermediate memory buffer used during rendering and other internal processing mechanisms.
 *You will see an error log message if there wasn't enough buffers. */
#ifndef LV_MEM_BUF_MAX_NUM
    #ifdef CONFIG_LV_MEM_BUF_MAX_NUM
        #define LV_MEM_BUF_MAX_NUM CONFIG_LV_MEM_BUF_MAX_NUM
    #else
        #define LV_MEM_BUF_MAX_NUM 16
    #endif
#endif

/*Use the standard `memcpy` and `memset` instead of LVGL's own functions. (Might or might not be faster).*/
#ifndef LV_MEMCPY_MEMSET_STD
    #ifdef CONFIG_LV_MEMCPY_MEMSET_STD
        #define LV_MEMCPY_MEMSET_STD CONFIG_LV_MEMCPY_MEMSET_STD
    #else
        #define LV_MEMCPY_MEMSET_STD 0
    #endif
#endif

/*====================
   HAL SETTINGS
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
#ifndef LV_DISP_DEF_REFR_PERIOD
    #ifdef CONFIG_LV_DISP_DEF_REFR_PERIOD
        #define LV_DISP_DEF_REFR_PERIOD CONFIG_LV_DISP_DEF_REFR_PERIOD
    #else
        #define LV_DISP_DEF_REFR_PERIOD 30      /*[ms]*/
    #endif
#endif

/*Input device read period in milliseconds*/
#ifndef LV_INDEV_DEF_READ_PERIOD
    #ifdef CONFIG_LV_INDEV_DEF_READ_PERIOD
        #define LV_INDEV_DEF_READ_PERIOD CONFIG_LV_INDEV_DEF_READ_PERIOD
    #else
        #define LV_INDEV_DEF_READ_PERIOD 30     /*[ms]*/
    #endif
#endif

/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
#ifndef LV_TICK_CUSTOM
    #ifdef CONFIG_LV_TICK_CUSTOM
        #define LV_TICK_CUSTOM CONFIG_LV_TICK_CUSTOM
    #else
        #define LV_TICK_CUSTOM 0
    #endif
#endif
#if LV_TICK_CUSTOM
    #ifndef LV_TICK_CUSTOM_INCLUDE
        #ifdef CONFIG_LV_TICK_CUSTOM_INCLUDE
            #define LV_TICK_CUSTOM_INCLUDE CONFIG_LV_TICK_CUSTOM_INCLUDE
        #else
            #define LV_TICK_CUSTOM_INCLUDE "Arduino.h"         /*Header for the system time function*/
        #endif
    #endif
    #ifndef LV_TICK_CUSTOM_SYS_TIME_EXPR
        #ifdef CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR
            #define LV_TICK_CUSTOM_SYS_TIME_EXPR CONFIG_LV_TICK_CUSTOM_SYS_TIME_EXPR
        #else
            #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())    /*Expression evaluating to current system time in ms*/
        #endif
    #endif
    /*If using lvgl as ESP32 component*/
    // #define LV_TICK_CUSTOM_INCLUDE "esp_timer.h"
    // #define LV_TICK_CUSTOM_SYS_TIME_EXPR ((esp_timer_get_time() / 1000LL))
#endif   /*LV_TICK_CUSTOM*/

/*Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 *(Not so important, you can adjust it to modify default sizes and spaces)*/
#ifndef LV_DPI_DEF
    #ifdef CONFIG_LV_DPI_DEF
        #define LV_DPI_DEF CONFIG_LV_DPI_DEF
    #else
        #define LV_DPI_DEF 130     /*[px/inch]*/
    #endif
#endif

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Drawing
 *-----------*/

/*Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations or any masks*/
#ifndef LV_DRAW_COMPLEX
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_DRAW_COMPLEX
            #define LV_DRAW_COMPLEX CONFIG_LV_DRAW_COMPLEX
        #else
            #define LV_DRAW_COMPLEX 0
        #endif
    #else
        #define LV_DRAW_COMPLEX 1
    #endif
#endif
#if LV_DRAW_COMPLEX != 0

    /*Allow buffering some shadow calculation.
    *LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
    *Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
    #ifndef LV_SHADOW_CACHE_SIZE
        #ifdef CONFIG_LV_SHADOW_CACHE_SIZE
            #define LV_SHADOW_CACHE_SIZE CONFIG_LV_SHADOW_CACHE_SIZE
        #else
            #define LV_SHADOW_CACHE_SIZE 0
        #endif
    #endif

    /* Set number of maximally cached circle data.
    * The circumference of 1/4 circle are saved for anti-aliasing
    * radius * 4 bytes are used per circle (the most often used radiuses are saved)
    * 0: to disable caching */
    #ifndef LV_CIRCLE_CACHE_SIZE
        #ifdef CONFIG_LV_CIRCLE_CACHE_SIZE
            #define LV_CIRCLE_CACHE_SIZE CONFIG_LV_CIRCLE_CACHE_SIZE
        #else
            #define LV_CIRCLE_CACHE_SIZE 4
        #endif
    #endif
#endif /*LV_DRAW_COMPLEX*/

/**
 * "Simple layers" are used when a widget has `style_opa < 255` to buffer the widget into a layer
 * and blend it as an image with the given opacity.
 * Note that `bg_opa`, `text_opa` etc don't require buffering into layer)
 * The widget can be buffered in smaller chunks to avoid using large buffers.
 *
 * - LV_LAYER_SIMPLE_BUF_SIZE: [bytes] the optimal target buffer size. LVGL will try to allocate it
 * - LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE: [bytes]  used if `LV_LAYER_SIMPLE_BUF_SIZE` couldn't be allocated.
 *
 * Both buffer sizes are in bytes.
 * "Transformed layers" (where transform_angle/zoom properties are used) use larger buffers
 * and can't be drawn in chunks. So these settings affects only widgets with opacity.
 */
#ifndef LV_LAYER_SIMPLE_BUF_SIZE
    #ifdef CONFIG_LV_LAYER_SIMPLE_BUF_SIZE
        #define LV_LAYER_SIMPLE_BUF_SIZE CONFIG_LV_LAYER_SIMPLE_BUF_SIZE
    #else
        #define LV_LAYER_SIMPLE_BUF_SIZE          (24 * 1024)
    #endif
#endif
#ifndef LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE
    #ifdef CONFIG_LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE
        #define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE CONFIG_LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE
    #else
        #define LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE (3 * 1024)
    #endif
#endif

/*Default image cache size. Image caching keeps the images opened.
 *If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 *With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 *However the opened images might consume additional RAM.
 *0: to disable caching*/
#ifndef LV_IMG_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_IMG_CACHE_DEF_SIZE
        #define LV_IMG_CACHE_DEF_SIZE CONFIG_LV_IMG_CACHE_DEF_SIZE
    #else
        #define LV_IMG_CACHE_DEF_SIZE 0
    #endif
#endif

/*Number of stops allowed per gradient. Increase this to allow more stops.
 *This adds (sizeof(lv_color_t) + 1) bytes per additional stop*/
#ifndef LV_GRADIENT_MAX_STOPS
    #ifdef CONFIG_LV_GRADIENT_MAX_STOPS
        #define LV_GRADIENT_MAX_STOPS CONFIG_LV_GRADIENT_MAX_STOPS
    #else
        #define LV_GRADIENT_MAX_STOPS 2
    #endif
#endif

/*Default gradient buffer size.
 *When LVGL calculates the gradient "maps" it can save them into a cache to avoid calculating them again.
 *LV_GRAD_CACHE_DEF_SIZE sets the size of this cache in bytes.
 *If the cache is too small the map will be allocated only while it's required for the drawing.
 *0 mean no caching.*/
#ifndef LV_GRAD_CACHE_DEF_SIZE
    #ifdef CONFIG_LV_GRAD_CACHE_DEF_SIZE
        #define LV_GRAD_CACHE_DEF_SIZE CONFIG_LV_GRAD_CACHE_DEF_SIZE
    #else
        #define LV_GRAD_CACHE_DEF_SIZE 0
    #endif
#endif

/*Allow dithering the gradients (to achieve visual smooth color gradients on limited color depth display)
 *LV_DITHER_GRADIENT implies allocating one or two more lines of the object's rendering surface
 *The increase in memory consumption is (32 bits * object width) plus 24 bits * object width if using error diffusion */
#ifndef LV_DITHER_GRADIENT
    #ifdef CONFIG_LV_DITHER_GRADIENT
        #define LV_DITHER_GRADIENT CONFIG_LV_DITHER_GRADIENT
    #else
        #define LV_DITHER_GRADIENT 0
    #endif
#endif
#if LV_DITHER_GRADIENT
    /*Add support for error diffusion dithering.
     *Error diffusion dithering gets a much better visual result, but implies more CPU consumption and memory when drawing.
     *The increase in memory consumption is (24 bits * object's width)*/
    #ifndef LV_DITHER_ERROR_DIFFUSION
        #ifdef CONFIG_LV_DITHER_ERROR_DIFFUSION
            #define LV_DITHER_ERROR_DIFFUSION CONFIG_LV_DITHER_ERROR_DIFFUSION
        #else
            #define LV_DITHER_ERROR_DIFFUSION 0
        #endif
    #endif
#endif

/*Maximum buffer size to allocate for rotation.
 *Only used if software rotation is enabled in the display driver.*/
#ifndef LV_DISP_ROT_MAX_BUF
    #ifdef CONFIG_LV_DISP_ROT_MAX_BUF
        #define LV_DISP_ROT_MAX_BUF CONFIG_LV_DISP_ROT_MAX_BUF
    #else
        #define LV_DISP_ROT_MAX_BUF (10*1024)
    #endif
#endif

/*-------------
 * GPU
 *-----------*/

/*Use Arm's 2D acceleration library Arm-2D */
#ifndef LV_USE_GPU_ARM2D
    #ifdef CONFIG_LV_USE_GPU_ARM2D
        #define LV_USE_GPU_ARM2D CONFIG_LV_USE_GPU_ARM2D
    #else
        #define LV_USE_GPU_ARM2D 0
    #endif
#endif

/*Use STM32's DMA2D (aka Chrom Art) GPU*/
#ifndef LV_USE_GPU_STM32_DMA2D
    #ifdef CONFIG_LV_USE_GPU_STM32_DMA2D
        #define LV_USE_GPU_STM32_DMA2D CONFIG_LV_USE_GPU_STM32_DMA2D
    #else
        #define LV_USE_GPU_STM32_DMA2D 0
    #endif
#endif
#if LV_USE_GPU_STM32_DMA2D
    /*Must be defined to include path of CMSIS header of target processor
    e.g. "stm32f7xx.h" or "stm32f4xx.h"*/
    #ifndef LV_GPU_DMA2D_CMSIS_INCLUDE
        #ifdef CONFIG_LV_GPU_DMA2D_CMSIS_INCLUDE
            #define LV_GPU_DMA2D_CMSIS_INCLUDE CONFIG_LV_GPU_DMA2D_CMSIS_INCLUDE
        #else
            #define LV_GPU_DMA2D_CMSIS_INCLUDE
        #endif
    #endif
#endif

/*Use SWM341's DMA2D GPU*/
#ifndef LV_USE_GPU_SWM341_DMA2D
    #ifdef CONFIG_LV_USE_GPU_SWM341_DMA2D
        #define LV_USE_GPU_SWM341_DMA2D CONFIG_LV_USE_GPU_SWM341_DMA2D
    #else
        #define LV_USE_GPU_SWM341_DMA2D 0
    #endif
#endif
#if LV_USE_GPU_SWM341_DMA2D
    #ifndef LV_GPU_SWM341_DMA2D_INCLUDE
        #ifdef CONFIG_LV_GPU_SWM341_DMA2D_INCLUDE
            #define LV_GPU_SWM341_DMA2D_INCLUDE CONFIG_LV_GPU_SWM341_DMA2D_INCLUDE
        #else
            #define LV_GPU_SWM341_DMA2D_INCLUDE "SWM341.h"
        #endif
    #endif
#endif

/*Use NXP's PXP GPU iMX RTxxx platforms*/
#ifndef LV_USE_GPU_NXP_PXP
    #ifdef CONFIG_LV_USE_GPU_NXP_PXP
        #define LV_USE_GPU_NXP_PXP CONFIG_LV_USE_GPU_NXP_PXP
    #else
        #define LV_USE_GPU_NXP_PXP 0
    #endif
#endif
#if LV_USE_GPU_NXP_PXP
    /*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
    *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol SDK_OS_FREE_RTOS
    *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
    *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
    */
    #ifndef LV_USE_GPU_NXP_PXP_AUTO_INIT
        #ifdef CONFIG_LV_USE_GPU_NXP_PXP_AUTO_INIT
            #define LV_USE_GPU_NXP_PXP_AUTO_INIT CONFIG_LV_USE_GPU_NXP_PXP_AUTO_INIT
        #else
            #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
        #endif
    #endif
#endif

/*Use NXP's VG-Lite GPU iMX RTxxx platforms*/
#ifndef LV_USE_GPU_NXP_VG_LITE
    #ifdef CONFIG_LV_USE_GPU_NXP_VG_LITE
        #define LV_USE_GPU_NXP_VG_LITE CONFIG_LV_USE_GPU_NXP_VG_LITE
    #else
        #define LV_USE_GPU_NXP_VG_LITE 0
    #endif
#endif

/*Use SDL renderer API*/
#ifndef LV_USE_GPU_SDL
    #ifdef CONFIG_LV_USE_GPU_SDL
        #define LV_USE_GPU_SDL CONFIG_LV_USE_GPU_SDL
    #else
        #define LV_USE_GPU_SDL 0
    #endif
#endif
#if LV_USE_GPU_SDL
    #ifndef LV_GPU_SDL_INCLUDE_PATH
        #ifdef CONFIG_LV_GPU_SDL_INCLUDE_PATH
            #define LV_GPU_SDL_INCLUDE_PATH CONFIG_LV_GPU_SDL_INCLUDE_PATH
        #else
            #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
        #endif
    #endif
    /*Texture cache size, 8MB by default*/
    #ifndef LV_GPU_SDL_LRU_SIZE
        #ifdef CONFIG_LV_GPU_SDL_LRU_SIZE
            #define LV_GPU_SDL_LRU_SIZE CONFIG_LV_GPU_SDL_LRU_SIZE
        #else
            #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
        #endif
    #endif
    /*Custom blend mode for mask drawing, disable if you need to link with older SDL2 lib*/
    #ifndef LV_GPU_SDL_CUSTOM_BLEND_MODE
        #ifdef CONFIG_LV_GPU_SDL_CUSTOM_BLEND_MODE
            #define LV_GPU_SDL_CUSTOM_BLEND_MODE CONFIG_LV_GPU_SDL_CUSTOM_BLEND_MODE
        #else
            #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
        #endif
    #endif
#endif

/*-------------
 * Logging
 *-----------*/

/*Enable the log module*/
#ifndef LV_USE_LOG
    #ifdef CONFIG_LV_USE_LOG
        #define LV_USE_LOG CONFIG_LV_USE_LOG
    #else
        #define LV_USE_LOG 0
    #endif
#endif
#if LV_USE_LOG

    /*How important log should be added:
    *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
    *LV_LOG_LEVEL_INFO        Log important events
    *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
    *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
    *LV_LOG_LEVEL_USER        Only logs added by the user
    *LV_LOG_LEVEL_NONE        Do not log anything*/
    #ifndef LV_LOG_LEVEL
        #ifdef CONFIG_LV_LOG_LEVEL
            #define LV_LOG_LEVEL CONFIG_LV_LOG_LEVEL
        #else
            #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
        #endif
    #endif

    /*1: Print the log with 'printf';
    *0: User need to register a callback with `lv_log_register_print_cb()`*/
    #ifndef LV_LOG_PRINTF
        #ifdef CONFIG_LV_LOG_PRINTF
            #define LV_LOG_PRINTF CONFIG_LV_LOG_PRINTF
        #else
            #define LV_LOG_PRINTF 0
        #endif
    #endif

    /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
    #ifndef LV_LOG_TRACE_MEM
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_MEM
                #define LV_LOG_TRACE_MEM CONFIG_LV_LOG_TRACE_MEM
            #else
                #define LV_LOG_TRACE_MEM 0
            #endif
        #else
            #define LV_LOG_TRACE_MEM        1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_TIMER
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_TIMER
                #define LV_LOG_TRACE_TIMER CONFIG_LV_LOG_TRACE_TIMER
            #else
                #define LV_LOG_TRACE_TIMER 0
            #endif
        #else
            #define LV_LOG_TRACE_TIMER      1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_INDEV
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_INDEV
                #define LV_LOG_TRACE_INDEV CONFIG_LV_LOG_TRACE_INDEV
            #else
                #define LV_LOG_TRACE_INDEV 0
            #endif
        #else
            #define LV_LOG_TRACE_INDEV      1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_DISP_REFR
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_DISP_REFR
                #define LV_LOG_TRACE_DISP_REFR CONFIG_LV_LOG_TRACE_DISP_REFR
            #else
                #define LV_LOG_TRACE_DISP_REFR 0
            #endif
        #else
            #define LV_LOG_TRACE_DISP_REFR  1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_EVENT
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_EVENT
                #define LV_LOG_TRACE_EVENT CONFIG_LV_LOG_TRACE_EVENT
            #else
                #define LV_LOG_TRACE_EVENT 0
            #endif
        #else
            #define LV_LOG_TRACE_EVENT      1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_OBJ_CREATE
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_OBJ_CREATE
                #define LV_LOG_TRACE_OBJ_CREATE CONFIG_LV_LOG_TRACE_OBJ_CREATE
            #else
                #define LV_LOG_TRACE_OBJ_CREATE 0
            #endif
        #else
            #define LV_LOG_TRACE_OBJ_CREATE 1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_LAYOUT
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_LAYOUT
                #define LV_LOG_TRACE_LAYOUT CONFIG_LV_LOG_TRACE_LAYOUT
            #else
                #define LV_LOG_TRACE_LAYOUT 0
            #endif
        #else
            #define LV_LOG_TRACE_LAYOUT     1
        #endif
    #endif
    #ifndef LV_LOG_TRACE_ANIM
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LOG_TRACE_ANIM
                #define LV_LOG_TRACE_ANIM CONFIG_LV_LOG_TRACE_ANIM
            #else
                #define LV_LOG_TRACE_ANIM 0
            #endif
        #else
            #define LV_LOG_TRACE_ANIM       1
        #endif
    #endif

#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts
 *-----------*/

/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
#ifndef LV_USE_ASSERT_NULL
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_NULL
            #define LV_USE_ASSERT_NULL CONFIG_LV_USE_ASSERT_NULL
        #else
            #define LV_USE_ASSERT_NULL 0
        #endif
    #else
        #define LV_USE_ASSERT_NULL          1   /*Check if the parameter is NULL. (Very fast, recommended)*/
    #endif
#endif
#ifndef LV_USE_ASSERT_MALLOC
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ASSERT_MALLOC
            #define LV_USE_ASSERT_MALLOC CONFIG_LV_USE_ASSERT_MALLOC
        #else
            #define LV_USE_ASSERT_MALLOC 0
        #endif
    #else
        #define LV_USE_ASSERT_MALLOC        1   /*Checks is the memory is successfully allocated or no. (Very fast, recommended)*/
    #endif
#endif
#ifndef LV_USE_ASSERT_STYLE
    #ifdef CONFIG_LV_USE_ASSERT_STYLE
        #define LV_USE_ASSERT_STYLE CONFIG_LV_USE_ASSERT_STYLE
    #else
        #define LV_USE_ASSERT_STYLE         0   /*Check if the styles are properly initialized. (Very fast, recommended)*/
    #endif
#endif
#ifndef LV_USE_ASSERT_MEM_INTEGRITY
    #ifdef CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
        #define LV_USE_ASSERT_MEM_INTEGRITY CONFIG_LV_USE_ASSERT_MEM_INTEGRITY
    #else
        #define LV_USE_ASSERT_MEM_INTEGRITY 0   /*Check the integrity of `lv_mem` after critical operations. (Slow)*/
    #endif
#endif
#ifndef LV_USE_ASSERT_OBJ
    #ifdef CONFIG_LV_USE_ASSERT_OBJ
        #define LV_USE_ASSERT_OBJ CONFIG_LV_USE_ASSERT_OBJ
    #else
        #define LV_USE_ASSERT_OBJ           0   /*Check the object's type and existence (e.g. not deleted). (Slow)*/
    #endif
#endif

/*Add a custom handler when assert happens e.g. to restart the MCU*/
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
        #define LV_ASSERT_HANDLER while(1);   /*Halt by default*/
    #endif
#endif

/*-------------
 * Others
 *-----------*/

/*1: Show CPU usage and FPS count*/
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
#endif

/*1: Show the used memory and the memory fragmentation
 * Requires LV_MEM_CUSTOM = 0*/
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

/*1: Draw random colored rectangles over the redrawn areas*/
#ifndef LV_USE_REFR_DEBUG
    #ifdef CONFIG_LV_USE_REFR_DEBUG
        #define LV_USE_REFR_DEBUG CONFIG_LV_USE_REFR_DEBUG
    #else
        #define LV_USE_REFR_DEBUG 0
    #endif
#endif

/*Change the built in (v)snprintf functions*/
#ifndef LV_SPRINTF_CUSTOM
    #ifdef CONFIG_LV_SPRINTF_CUSTOM
        #define LV_SPRINTF_CUSTOM CONFIG_LV_SPRINTF_CUSTOM
    #else
        #define LV_SPRINTF_CUSTOM 0
    #endif
#endif
#if LV_SPRINTF_CUSTOM
    #ifndef LV_SPRINTF_INCLUDE
        #ifdef CONFIG_LV_SPRINTF_INCLUDE
            #define LV_SPRINTF_INCLUDE CONFIG_LV_SPRINTF_INCLUDE
        #else
            #define LV_SPRINTF_INCLUDE <stdio.h>
        #endif
    #endif
    #ifndef lv_snprintf
        #ifdef CONFIG_LV_SNPRINTF
            #define lv_snprintf CONFIG_LV_SNPRINTF
        #else
            #define lv_snprintf  snprintf
        #endif
    #endif
    #ifndef lv_vsnprintf
        #ifdef CONFIG_LV_VSNPRINTF
            #define lv_vsnprintf CONFIG_LV_VSNPRINTF
        #else
            #define lv_vsnprintf vsnprintf
        #endif
    #endif
#else   /*LV_SPRINTF_CUSTOM*/
    #ifndef LV_SPRINTF_USE_FLOAT
        #ifdef CONFIG_LV_SPRINTF_USE_FLOAT
            #define LV_SPRINTF_USE_FLOAT CONFIG_LV_SPRINTF_USE_FLOAT
        #else
            #define LV_SPRINTF_USE_FLOAT 0
        #endif
    #endif
#endif  /*LV_SPRINTF_CUSTOM*/

#ifndef LV_USE_USER_DATA
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_USER_DATA
            #define LV_USE_USER_DATA CONFIG_LV_USE_USER_DATA
        #else
            #define LV_USE_USER_DATA 0
        #endif
    #else
        #define LV_USE_USER_DATA 1
    #endif
#endif

/*Garbage Collector settings
 *Used if lvgl is bound to higher level language and the memory is managed by that language*/
#ifndef LV_ENABLE_GC
    #ifdef CONFIG_LV_ENABLE_GC
        #define LV_ENABLE_GC CONFIG_LV_ENABLE_GC
    #else
        #define LV_ENABLE_GC 0
    #endif
#endif
#if LV_ENABLE_GC != 0
    #ifndef LV_GC_INCLUDE
        #ifdef CONFIG_LV_GC_INCLUDE
            #define LV_GC_INCLUDE CONFIG_LV_GC_INCLUDE
        #else
            #define LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
        #endif
    #endif
#endif /*LV_ENABLE_GC*/

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/*For big endian systems set to 1*/
#ifndef LV_BIG_ENDIAN_SYSTEM
    #ifdef CONFIG_LV_BIG_ENDIAN_SYSTEM
        #define LV_BIG_ENDIAN_SYSTEM CONFIG_LV_BIG_ENDIAN_SYSTEM
    #else
        #define LV_BIG_ENDIAN_SYSTEM 0
    #endif
#endif

/*Define a custom attribute to `lv_tick_inc` function*/
#ifndef LV_ATTRIBUTE_TICK_INC
    #ifdef CONFIG_LV_ATTRIBUTE_TICK_INC
        #define LV_ATTRIBUTE_TICK_INC CONFIG_LV_ATTRIBUTE_TICK_INC
    #else
        #define LV_ATTRIBUTE_TICK_INC
    #endif
#endif

/*Define a custom attribute to `lv_timer_handler` function*/
#ifndef LV_ATTRIBUTE_TIMER_HANDLER
    #ifdef CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
        #define LV_ATTRIBUTE_TIMER_HANDLER CONFIG_LV_ATTRIBUTE_TIMER_HANDLER
    #else
        #define LV_ATTRIBUTE_TIMER_HANDLER
    #endif
#endif

/*Define a custom attribute to `lv_disp_flush_ready` function*/
#ifndef LV_ATTRIBUTE_FLUSH_READY
    #ifdef CONFIG_LV_ATTRIBUTE_FLUSH_READY
        #define LV_ATTRIBUTE_FLUSH_READY CONFIG_LV_ATTRIBUTE_FLUSH_READY
    #else
        #define LV_ATTRIBUTE_FLUSH_READY
    #endif
#endif

/*Required alignment size for buffers*/
#ifndef LV_ATTRIBUTE_MEM_ALIGN_SIZE
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE CONFIG_LV_ATTRIBUTE_MEM_ALIGN_SIZE
        #else
            #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 0
        #endif
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1
    #endif
#endif

/*Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4)))*/
#ifndef LV_ATTRIBUTE_MEM_ALIGN
    #ifdef CONFIG_LV_ATTRIBUTE_MEM_ALIGN
        #define LV_ATTRIBUTE_MEM_ALIGN CONFIG_LV_ATTRIBUTE_MEM_ALIGN
    #else
        #define LV_ATTRIBUTE_MEM_ALIGN
    #endif
#endif

/*Attribute to mark large constant arrays for example font's bitmaps*/
#ifndef LV_ATTRIBUTE_LARGE_CONST
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_CONST
        #define LV_ATTRIBUTE_LARGE_CONST CONFIG_LV_ATTRIBUTE_LARGE_CONST
    #else
        #define LV_ATTRIBUTE_LARGE_CONST
    #endif
#endif

/*Compiler prefix for a big array declaration in RAM*/
#ifndef LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #ifdef CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY CONFIG_LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #else
        #define LV_ATTRIBUTE_LARGE_RAM_ARRAY
    #endif
#endif

/*Place performance critical functions into a faster memory (e.g RAM)*/
#ifndef LV_ATTRIBUTE_FAST_MEM
    #ifdef CONFIG_LV_ATTRIBUTE_FAST_MEM
        #define LV_ATTRIBUTE_FAST_MEM CONFIG_LV_ATTRIBUTE_FAST_MEM
    #else
        #define LV_ATTRIBUTE_FAST_MEM
    #endif
#endif

/*Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible*/
#ifndef LV_ATTRIBUTE_DMA
    #ifdef CONFIG_LV_ATTRIBUTE_DMA
        #define LV_ATTRIBUTE_DMA CONFIG_LV_ATTRIBUTE_DMA
    #else
        #define LV_ATTRIBUTE_DMA
    #endif
#endif

/*Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *should also appear on LVGL binding API such as Micropython.*/
#ifndef LV_EXPORT_CONST_INT
    #ifdef CONFIG_LV_EXPORT_CONST_INT
        #define LV_EXPORT_CONST_INT CONFIG_LV_EXPORT_CONST_INT
    #else
        #define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*The default value just prevents GCC warning*/
    #endif
#endif

/*Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t*/
#ifndef LV_USE_LARGE_COORD
    #ifdef CONFIG_LV_USE_LARGE_COORD
        #define LV_USE_LARGE_COORD CONFIG_LV_USE_LARGE_COORD
    #else
        #define LV_USE_LARGE_COORD 0
    #endif
#endif

/*==================
 *   FONT USAGE
 *===================*/

/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
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
    #ifdef _LV_KCONFIG_PRESENT
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

/*Demonstrate special features*/
#ifndef LV_FONT_MONTSERRAT_12_SUBPX
    #ifdef CONFIG_LV_FONT_MONTSERRAT_12_SUBPX
        #define LV_FONT_MONTSERRAT_12_SUBPX CONFIG_LV_FONT_MONTSERRAT_12_SUBPX
    #else
        #define LV_FONT_MONTSERRAT_12_SUBPX      0
    #endif
#endif
#ifndef LV_FONT_MONTSERRAT_28_COMPRESSED
    #ifdef CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
        #define LV_FONT_MONTSERRAT_28_COMPRESSED CONFIG_LV_FONT_MONTSERRAT_28_COMPRESSED
    #else
        #define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
    #endif
#endif
#ifndef LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #ifdef CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW CONFIG_LV_FONT_DEJAVU_16_PERSIAN_HEBREW
    #else
        #define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, Persian letters and all their forms*/
    #endif
#endif
#ifndef LV_FONT_SIMSUN_16_CJK
    #ifdef CONFIG_LV_FONT_SIMSUN_16_CJK
        #define LV_FONT_SIMSUN_16_CJK CONFIG_LV_FONT_SIMSUN_16_CJK
    #else
        #define LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/
    #endif
#endif

/*Pixel perfect monospace fonts*/
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

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#ifndef LV_FONT_CUSTOM_DECLARE
    #ifdef CONFIG_LV_FONT_CUSTOM_DECLARE
        #define LV_FONT_CUSTOM_DECLARE CONFIG_LV_FONT_CUSTOM_DECLARE
    #else
        #define LV_FONT_CUSTOM_DECLARE
    #endif
#endif

/*Always set a default font*/
#ifndef LV_FONT_DEFAULT
    #ifdef CONFIG_LV_FONT_DEFAULT
        #define LV_FONT_DEFAULT CONFIG_LV_FONT_DEFAULT
    #else
        #define LV_FONT_DEFAULT &lv_font_montserrat_14
    #endif
#endif

/*Enable handling large font and/or fonts with a lot of characters.
 *The limit depends on the font size, font face and bpp.
 *Compiler error will be triggered if a font needs it.*/
#ifndef LV_FONT_FMT_TXT_LARGE
    #ifdef CONFIG_LV_FONT_FMT_TXT_LARGE
        #define LV_FONT_FMT_TXT_LARGE CONFIG_LV_FONT_FMT_TXT_LARGE
    #else
        #define LV_FONT_FMT_TXT_LARGE 0
    #endif
#endif

/*Enables/disables support for compressed fonts.*/
#ifndef LV_USE_FONT_COMPRESSED
    #ifdef CONFIG_LV_USE_FONT_COMPRESSED
        #define LV_USE_FONT_COMPRESSED CONFIG_LV_USE_FONT_COMPRESSED
    #else
        #define LV_USE_FONT_COMPRESSED 0
    #endif
#endif

/*Enable subpixel rendering*/
#ifndef LV_USE_FONT_SUBPX
    #ifdef CONFIG_LV_USE_FONT_SUBPX
        #define LV_USE_FONT_SUBPX CONFIG_LV_USE_FONT_SUBPX
    #else
        #define LV_USE_FONT_SUBPX 0
    #endif
#endif
#if LV_USE_FONT_SUBPX
    /*Set the pixel order of the display. Physical order of RGB channels. Doesn't matter with "normal" fonts.*/
    #ifndef LV_FONT_SUBPX_BGR
        #ifdef CONFIG_LV_FONT_SUBPX_BGR
            #define LV_FONT_SUBPX_BGR CONFIG_LV_FONT_SUBPX_BGR
        #else
            #define LV_FONT_SUBPX_BGR 0  /*0: RGB; 1:BGR order*/
        #endif
    #endif
#endif

/*Enable drawing placeholders when glyph dsc is not found*/
#ifndef LV_USE_FONT_PLACEHOLDER
    #ifdef _LV_KCONFIG_PRESENT
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
 * Your IDE or editor should have the same character encoding
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

/*Can break (wrap) texts on these chars*/
#ifndef LV_TXT_BREAK_CHARS
    #ifdef CONFIG_LV_TXT_BREAK_CHARS
        #define LV_TXT_BREAK_CHARS CONFIG_LV_TXT_BREAK_CHARS
    #else
        #define LV_TXT_BREAK_CHARS " ,.;:-_"
    #endif
#endif

/*If a word is at least this long, will break wherever "prettiest"
 *To disable, set to a value <= 0*/
#ifndef LV_TXT_LINE_BREAK_LONG_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
        #define LV_TXT_LINE_BREAK_LONG_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_LEN 0
    #endif
#endif

/*Minimum number of characters in a long word to put on a line before a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
#ifndef LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3
    #endif
#endif

/*Minimum number of characters in a long word to put on a line after a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
#ifndef LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #ifdef CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN CONFIG_LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN
    #else
        #define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3
    #endif
#endif

/*The control character to use for signalling text recoloring.*/
#ifndef LV_TXT_COLOR_CMD
    #ifdef CONFIG_LV_TXT_COLOR_CMD
        #define LV_TXT_COLOR_CMD CONFIG_LV_TXT_COLOR_CMD
    #else
        #define LV_TXT_COLOR_CMD "#"
    #endif
#endif

/*Support bidirectional texts. Allows mixing Left-to-Right and Right-to-Left texts.
 *The direction will be processed according to the Unicode Bidirectional Algorithm:
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
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
    *`LV_BASE_DIR_AUTO` detect texts base direction*/
    #ifndef LV_BIDI_BASE_DIR_DEF
        #ifdef CONFIG_LV_BIDI_BASE_DIR_DEF
            #define LV_BIDI_BASE_DIR_DEF CONFIG_LV_BIDI_BASE_DIR_DEF
        #else
            #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
        #endif
    #endif
#endif

/*Enable Arabic/Persian processing
 *In these languages characters should be replaced with an other form based on their position in the text*/
#ifndef LV_USE_ARABIC_PERSIAN_CHARS
    #ifdef CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
        #define LV_USE_ARABIC_PERSIAN_CHARS CONFIG_LV_USE_ARABIC_PERSIAN_CHARS
    #else
        #define LV_USE_ARABIC_PERSIAN_CHARS 0
    #endif
#endif

/*==================
 *  WIDGET USAGE
 *================*/

/*Documentation of the widgets: https://docs.lvgl.io/latest/en/html/widgets/index.html*/

#ifndef LV_USE_ARC
    #ifdef _LV_KCONFIG_PRESENT
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
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BAR
            #define LV_USE_BAR CONFIG_LV_USE_BAR
        #else
            #define LV_USE_BAR 0
        #endif
    #else
        #define LV_USE_BAR        1
    #endif
#endif

#ifndef LV_USE_BTN
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BTN
            #define LV_USE_BTN CONFIG_LV_USE_BTN
        #else
            #define LV_USE_BTN 0
        #endif
    #else
        #define LV_USE_BTN        1
    #endif
#endif

#ifndef LV_USE_BTNMATRIX
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_BTNMATRIX
            #define LV_USE_BTNMATRIX CONFIG_LV_USE_BTNMATRIX
        #else
            #define LV_USE_BTNMATRIX 0
        #endif
    #else
        #define LV_USE_BTNMATRIX  1
    #endif
#endif

#ifndef LV_USE_CANVAS
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CANVAS
            #define LV_USE_CANVAS CONFIG_LV_USE_CANVAS
        #else
            #define LV_USE_CANVAS 0
        #endif
    #else
        #define LV_USE_CANVAS     1
    #endif
#endif

#ifndef LV_USE_CHECKBOX
    #ifdef _LV_KCONFIG_PRESENT
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
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_DROPDOWN
            #define LV_USE_DROPDOWN CONFIG_LV_USE_DROPDOWN
        #else
            #define LV_USE_DROPDOWN 0
        #endif
    #else
        #define LV_USE_DROPDOWN   1   /*Requires: lv_label*/
    #endif
#endif

#ifndef LV_USE_IMG
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMG
            #define LV_USE_IMG CONFIG_LV_USE_IMG
        #else
            #define LV_USE_IMG 0
        #endif
    #else
        #define LV_USE_IMG        1   /*Requires: lv_label*/
    #endif
#endif

#ifndef LV_USE_LABEL
    #ifdef _LV_KCONFIG_PRESENT
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
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_TEXT_SELECTION
                #define LV_LABEL_TEXT_SELECTION CONFIG_LV_LABEL_TEXT_SELECTION
            #else
                #define LV_LABEL_TEXT_SELECTION 0
            #endif
        #else
            #define LV_LABEL_TEXT_SELECTION 1 /*Enable selecting text of the label*/
        #endif
    #endif
    #ifndef LV_LABEL_LONG_TXT_HINT
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_LABEL_LONG_TXT_HINT
                #define LV_LABEL_LONG_TXT_HINT CONFIG_LV_LABEL_LONG_TXT_HINT
            #else
                #define LV_LABEL_LONG_TXT_HINT 0
            #endif
        #else
            #define LV_LABEL_LONG_TXT_HINT 1  /*Store some extra info in labels to speed up drawing of very long texts*/
        #endif
    #endif
#endif

#ifndef LV_USE_LINE
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LINE
            #define LV_USE_LINE CONFIG_LV_USE_LINE
        #else
            #define LV_USE_LINE 0
        #endif
    #else
        #define LV_USE_LINE       1
    #endif
#endif

#ifndef LV_USE_ROLLER
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ROLLER
            #define LV_USE_ROLLER CONFIG_LV_USE_ROLLER
        #else
            #define LV_USE_ROLLER 0
        #endif
    #else
        #define LV_USE_ROLLER     1   /*Requires: lv_label*/
    #endif
#endif
#if LV_USE_ROLLER
    #ifndef LV_ROLLER_INF_PAGES
        #ifdef CONFIG_LV_ROLLER_INF_PAGES
            #define LV_ROLLER_INF_PAGES CONFIG_LV_ROLLER_INF_PAGES
        #else
            #define LV_ROLLER_INF_PAGES 7 /*Number of extra "pages" when the roller is infinite*/
        #endif
    #endif
#endif

#ifndef LV_USE_SLIDER
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SLIDER
            #define LV_USE_SLIDER CONFIG_LV_USE_SLIDER
        #else
            #define LV_USE_SLIDER 0
        #endif
    #else
        #define LV_USE_SLIDER     1   /*Requires: lv_bar*/
    #endif
#endif

#ifndef LV_USE_SWITCH
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SWITCH
            #define LV_USE_SWITCH CONFIG_LV_USE_SWITCH
        #else
            #define LV_USE_SWITCH 0
        #endif
    #else
        #define LV_USE_SWITCH     1
    #endif
#endif

#ifndef LV_USE_TEXTAREA
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TEXTAREA
            #define LV_USE_TEXTAREA CONFIG_LV_USE_TEXTAREA
        #else
            #define LV_USE_TEXTAREA 0
        #endif
    #else
        #define LV_USE_TEXTAREA   1   /*Requires: lv_label*/
    #endif
#endif
#if LV_USE_TEXTAREA != 0
    #ifndef LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #ifdef CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME CONFIG_LV_TEXTAREA_DEF_PWD_SHOW_TIME
        #else
            #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /*ms*/
        #endif
    #endif
#endif

#ifndef LV_USE_TABLE
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABLE
            #define LV_USE_TABLE CONFIG_LV_USE_TABLE
        #else
            #define LV_USE_TABLE 0
        #endif
    #else
        #define LV_USE_TABLE      1
    #endif
#endif

/*==================
 * EXTRA COMPONENTS
 *==================*/

/*-----------
 * Widgets
 *----------*/
#ifndef LV_USE_ANIMIMG
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_ANIMIMG
            #define LV_USE_ANIMIMG CONFIG_LV_USE_ANIMIMG
        #else
            #define LV_USE_ANIMIMG 0
        #endif
    #else
        #define LV_USE_ANIMIMG    1
    #endif
#endif

#ifndef LV_USE_CALENDAR
    #ifdef _LV_KCONFIG_PRESENT
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
        #ifdef _LV_KCONFIG_PRESENT
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
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
                #define LV_USE_CALENDAR_HEADER_DROPDOWN CONFIG_LV_USE_CALENDAR_HEADER_DROPDOWN
            #else
                #define LV_USE_CALENDAR_HEADER_DROPDOWN 0
            #endif
        #else
            #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
        #endif
    #endif
#endif  /*LV_USE_CALENDAR*/

#ifndef LV_USE_CHART
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_CHART
            #define LV_USE_CHART CONFIG_LV_USE_CHART
        #else
            #define LV_USE_CHART 0
        #endif
    #else
        #define LV_USE_CHART      1
    #endif
#endif

#ifndef LV_USE_COLORWHEEL
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_COLORWHEEL
            #define LV_USE_COLORWHEEL CONFIG_LV_USE_COLORWHEEL
        #else
            #define LV_USE_COLORWHEEL 0
        #endif
    #else
        #define LV_USE_COLORWHEEL 1
    #endif
#endif

#ifndef LV_USE_IMGBTN
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_IMGBTN
            #define LV_USE_IMGBTN CONFIG_LV_USE_IMGBTN
        #else
            #define LV_USE_IMGBTN 0
        #endif
    #else
        #define LV_USE_IMGBTN     1
    #endif
#endif

#ifndef LV_USE_KEYBOARD
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_KEYBOARD
            #define LV_USE_KEYBOARD CONFIG_LV_USE_KEYBOARD
        #else
            #define LV_USE_KEYBOARD 0
        #endif
    #else
        #define LV_USE_KEYBOARD   1
    #endif
#endif

#ifndef LV_USE_LED
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LED
            #define LV_USE_LED CONFIG_LV_USE_LED
        #else
            #define LV_USE_LED 0
        #endif
    #else
        #define LV_USE_LED        1
    #endif
#endif

#ifndef LV_USE_LIST
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_LIST
            #define LV_USE_LIST CONFIG_LV_USE_LIST
        #else
            #define LV_USE_LIST 0
        #endif
    #else
        #define LV_USE_LIST       1
    #endif
#endif

#ifndef LV_USE_MENU
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MENU
            #define LV_USE_MENU CONFIG_LV_USE_MENU
        #else
            #define LV_USE_MENU 0
        #endif
    #else
        #define LV_USE_MENU       1
    #endif
#endif

#ifndef LV_USE_METER
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_METER
            #define LV_USE_METER CONFIG_LV_USE_METER
        #else
            #define LV_USE_METER 0
        #endif
    #else
        #define LV_USE_METER      1
    #endif
#endif

#ifndef LV_USE_MSGBOX
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_MSGBOX
            #define LV_USE_MSGBOX CONFIG_LV_USE_MSGBOX
        #else
            #define LV_USE_MSGBOX 0
        #endif
    #else
        #define LV_USE_MSGBOX     1
    #endif
#endif

#ifndef LV_USE_SPAN
    #ifdef _LV_KCONFIG_PRESENT
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
    /*A line text can contain maximum num of span descriptor */
    #ifndef LV_SPAN_SNIPPET_STACK_SIZE
        #ifdef CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
            #define LV_SPAN_SNIPPET_STACK_SIZE CONFIG_LV_SPAN_SNIPPET_STACK_SIZE
        #else
            #define LV_SPAN_SNIPPET_STACK_SIZE 64
        #endif
    #endif
#endif

#ifndef LV_USE_SPINBOX
    #ifdef _LV_KCONFIG_PRESENT
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
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_SPINNER
            #define LV_USE_SPINNER CONFIG_LV_USE_SPINNER
        #else
            #define LV_USE_SPINNER 0
        #endif
    #else
        #define LV_USE_SPINNER    1
    #endif
#endif

#ifndef LV_USE_TABVIEW
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_TABVIEW
            #define LV_USE_TABVIEW CONFIG_LV_USE_TABVIEW
        #else
            #define LV_USE_TABVIEW 0
        #endif
    #else
        #define LV_USE_TABVIEW    1
    #endif
#endif

#ifndef LV_USE_TILEVIEW
    #ifdef _LV_KCONFIG_PRESENT
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
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_WIN
            #define LV_USE_WIN CONFIG_LV_USE_WIN
        #else
            #define LV_USE_WIN 0
        #endif
    #else
        #define LV_USE_WIN        1
    #endif
#endif

/*-----------
 * Themes
 *----------*/

/*A simple, impressive and very complete theme*/
#ifndef LV_USE_THEME_DEFAULT
    #ifdef _LV_KCONFIG_PRESENT
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

    /*0: Light mode; 1: Dark mode*/
    #ifndef LV_THEME_DEFAULT_DARK
        #ifdef CONFIG_LV_THEME_DEFAULT_DARK
            #define LV_THEME_DEFAULT_DARK CONFIG_LV_THEME_DEFAULT_DARK
        #else
            #define LV_THEME_DEFAULT_DARK 0
        #endif
    #endif

    /*1: Enable grow on press*/
    #ifndef LV_THEME_DEFAULT_GROW
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_THEME_DEFAULT_GROW
                #define LV_THEME_DEFAULT_GROW CONFIG_LV_THEME_DEFAULT_GROW
            #else
                #define LV_THEME_DEFAULT_GROW 0
            #endif
        #else
            #define LV_THEME_DEFAULT_GROW 1
        #endif
    #endif

    /*Default transition time in [ms]*/
    #ifndef LV_THEME_DEFAULT_TRANSITION_TIME
        #ifdef CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
            #define LV_THEME_DEFAULT_TRANSITION_TIME CONFIG_LV_THEME_DEFAULT_TRANSITION_TIME
        #else
            #define LV_THEME_DEFAULT_TRANSITION_TIME 80
        #endif
    #endif
#endif /*LV_USE_THEME_DEFAULT*/

/*A very simple theme that is a good starting point for a custom theme*/
#ifndef LV_USE_THEME_BASIC
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_BASIC
            #define LV_USE_THEME_BASIC CONFIG_LV_USE_THEME_BASIC
        #else
            #define LV_USE_THEME_BASIC 0
        #endif
    #else
        #define LV_USE_THEME_BASIC 1
    #endif
#endif

/*A theme designed for monochrome displays*/
#ifndef LV_USE_THEME_MONO
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_THEME_MONO
            #define LV_USE_THEME_MONO CONFIG_LV_USE_THEME_MONO
        #else
            #define LV_USE_THEME_MONO 0
        #endif
    #else
        #define LV_USE_THEME_MONO 1
    #endif
#endif

/*-----------
 * Layouts
 *----------*/

/*A layout similar to Flexbox in CSS.*/
#ifndef LV_USE_FLEX
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_FLEX
            #define LV_USE_FLEX CONFIG_LV_USE_FLEX
        #else
            #define LV_USE_FLEX 0
        #endif
    #else
        #define LV_USE_FLEX 1
    #endif
#endif

/*A layout similar to Grid in CSS.*/
#ifndef LV_USE_GRID
    #ifdef _LV_KCONFIG_PRESENT
        #ifdef CONFIG_LV_USE_GRID
            #define LV_USE_GRID CONFIG_LV_USE_GRID
        #else
            #define LV_USE_GRID 0
        #endif
    #else
        #define LV_USE_GRID 1
    #endif
#endif

/*---------------------
 * 3rd party libraries
 *--------------------*/

/*File system interfaces for common APIs */

/*API for fopen, fread, etc*/
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
            #define LV_FS_STDIO_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
        #endif
    #endif
    #ifndef LV_FS_STDIO_PATH
        #ifdef CONFIG_LV_FS_STDIO_PATH
            #define LV_FS_STDIO_PATH CONFIG_LV_FS_STDIO_PATH
        #else
            #define LV_FS_STDIO_PATH ""         /*Set the working directory. File/directory paths will be appended to it.*/
        #endif
    #endif
    #ifndef LV_FS_STDIO_CACHE_SIZE
        #ifdef CONFIG_LV_FS_STDIO_CACHE_SIZE
            #define LV_FS_STDIO_CACHE_SIZE CONFIG_LV_FS_STDIO_CACHE_SIZE
        #else
            #define LV_FS_STDIO_CACHE_SIZE 0    /*>0 to cache this number of bytes in lv_fs_read()*/
        #endif
    #endif
#endif

/*API for open, read, etc*/
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
            #define LV_FS_POSIX_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
        #endif
    #endif
    #ifndef LV_FS_POSIX_PATH
        #ifdef CONFIG_LV_FS_POSIX_PATH
            #define LV_FS_POSIX_PATH CONFIG_LV_FS_POSIX_PATH
        #else
            #define LV_FS_POSIX_PATH ""         /*Set the working directory. File/directory paths will be appended to it.*/
        #endif
    #endif
    #ifndef LV_FS_POSIX_CACHE_SIZE
        #ifdef CONFIG_LV_FS_POSIX_CACHE_SIZE
            #define LV_FS_POSIX_CACHE_SIZE CONFIG_LV_FS_POSIX_CACHE_SIZE
        #else
            #define LV_FS_POSIX_CACHE_SIZE 0    /*>0 to cache this number of bytes in lv_fs_read()*/
        #endif
    #endif
#endif

/*API for CreateFile, ReadFile, etc*/
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
            #define LV_FS_WIN32_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
        #endif
    #endif
    #ifndef LV_FS_WIN32_PATH
        #ifdef CONFIG_LV_FS_WIN32_PATH
            #define LV_FS_WIN32_PATH CONFIG_LV_FS_WIN32_PATH
        #else
            #define LV_FS_WIN32_PATH ""         /*Set the working directory. File/directory paths will be appended to it.*/
        #endif
    #endif
    #ifndef LV_FS_WIN32_CACHE_SIZE
        #ifdef CONFIG_LV_FS_WIN32_CACHE_SIZE
            #define LV_FS_WIN32_CACHE_SIZE CONFIG_LV_FS_WIN32_CACHE_SIZE
        #else
            #define LV_FS_WIN32_CACHE_SIZE 0    /*>0 to cache this number of bytes in lv_fs_read()*/
        #endif
    #endif
#endif

/*API for FATFS (needs to be added separately). Uses f_open, f_read, etc*/
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
            #define LV_FS_FATFS_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
        #endif
    #endif
    #ifndef LV_FS_FATFS_CACHE_SIZE
        #ifdef CONFIG_LV_FS_FATFS_CACHE_SIZE
            #define LV_FS_FATFS_CACHE_SIZE CONFIG_LV_FS_FATFS_CACHE_SIZE
        #else
            #define LV_FS_FATFS_CACHE_SIZE 0    /*>0 to cache this number of bytes in lv_fs_read()*/
        #endif
    #endif
#endif

/*PNG decoder library*/
#ifndef LV_USE_PNG
    #ifdef CONFIG_LV_USE_PNG
        #define LV_USE_PNG CONFIG_LV_USE_PNG
    #else
        #define LV_USE_PNG 0
    #endif
#endif

/*BMP decoder library*/
#ifndef LV_USE_BMP
    #ifdef CONFIG_LV_USE_BMP
        #define LV_USE_BMP CONFIG_LV_USE_BMP
    #else
        #define LV_USE_BMP 0
    #endif
#endif

/* JPG + split JPG decoder library.
 * Split JPG is a custom format optimized for embedded systems. */
#ifndef LV_USE_SJPG
    #ifdef CONFIG_LV_USE_SJPG
        #define LV_USE_SJPG CONFIG_LV_USE_SJPG
    #else
        #define LV_USE_SJPG 0
    #endif
#endif

/*GIF decoder library*/
#ifndef LV_USE_GIF
    #ifdef CONFIG_LV_USE_GIF
        #define LV_USE_GIF CONFIG_LV_USE_GIF
    #else
        #define LV_USE_GIF 0
    #endif
#endif

/*QR code library*/
#ifndef LV_USE_QRCODE
    #ifdef CONFIG_LV_USE_QRCODE
        #define LV_USE_QRCODE CONFIG_LV_USE_QRCODE
    #else
        #define LV_USE_QRCODE 0
    #endif
#endif

/*FreeType library*/
#ifndef LV_USE_FREETYPE
    #ifdef CONFIG_LV_USE_FREETYPE
        #define LV_USE_FREETYPE CONFIG_LV_USE_FREETYPE
    #else
        #define LV_USE_FREETYPE 0
    #endif
#endif
#if LV_USE_FREETYPE
    /*Memory used by FreeType to cache characters [bytes] (-1: no caching)*/
    #ifndef LV_FREETYPE_CACHE_SIZE
        #ifdef CONFIG_LV_FREETYPE_CACHE_SIZE
            #define LV_FREETYPE_CACHE_SIZE CONFIG_LV_FREETYPE_CACHE_SIZE
        #else
            #define LV_FREETYPE_CACHE_SIZE (16 * 1024)
        #endif
    #endif
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /* 1: bitmap cache use the sbit cache, 0:bitmap cache use the image cache. */
        /* sbit cache:it is much more memory efficient for small bitmaps(font size < 256) */
        /* if font size >= 256, must be configured as image cache */
        #ifndef LV_FREETYPE_SBIT_CACHE
            #ifdef CONFIG_LV_FREETYPE_SBIT_CACHE
                #define LV_FREETYPE_SBIT_CACHE CONFIG_LV_FREETYPE_SBIT_CACHE
            #else
                #define LV_FREETYPE_SBIT_CACHE 0
            #endif
        #endif
        /* Maximum number of opened FT_Face/FT_Size objects managed by this cache instance. */
        /* (0:use system defaults) */
        #ifndef LV_FREETYPE_CACHE_FT_FACES
            #ifdef CONFIG_LV_FREETYPE_CACHE_FT_FACES
                #define LV_FREETYPE_CACHE_FT_FACES CONFIG_LV_FREETYPE_CACHE_FT_FACES
            #else
                #define LV_FREETYPE_CACHE_FT_FACES 0
            #endif
        #endif
        #ifndef LV_FREETYPE_CACHE_FT_SIZES
            #ifdef CONFIG_LV_FREETYPE_CACHE_FT_SIZES
                #define LV_FREETYPE_CACHE_FT_SIZES CONFIG_LV_FREETYPE_CACHE_FT_SIZES
            #else
                #define LV_FREETYPE_CACHE_FT_SIZES 0
            #endif
        #endif
    #endif
#endif

/*Rlottie library*/
#ifndef LV_USE_RLOTTIE
    #ifdef CONFIG_LV_USE_RLOTTIE
        #define LV_USE_RLOTTIE CONFIG_LV_USE_RLOTTIE
    #else
        #define LV_USE_RLOTTIE 0
    #endif
#endif

/*FFmpeg library for image decoding and playing videos
 *Supports all major image formats so do not enable other image decoder with it*/
#ifndef LV_USE_FFMPEG
    #ifdef CONFIG_LV_USE_FFMPEG
        #define LV_USE_FFMPEG CONFIG_LV_USE_FFMPEG
    #else
        #define LV_USE_FFMPEG 0
    #endif
#endif
#if LV_USE_FFMPEG
    /*Dump input information to stderr*/
    #ifndef LV_FFMPEG_DUMP_FORMAT
        #ifdef CONFIG_LV_FFMPEG_DUMP_FORMAT
            #define LV_FFMPEG_DUMP_FORMAT CONFIG_LV_FFMPEG_DUMP_FORMAT
        #else
            #define LV_FFMPEG_DUMP_FORMAT 0
        #endif
    #endif
#endif

/*-----------
 * Others
 *----------*/

/*1: Enable API to take snapshot for object*/
#ifndef LV_USE_SNAPSHOT
    #ifdef CONFIG_LV_USE_SNAPSHOT
        #define LV_USE_SNAPSHOT CONFIG_LV_USE_SNAPSHOT
    #else
        #define LV_USE_SNAPSHOT 0
    #endif
#endif

/*1: Enable Monkey test*/
#ifndef LV_USE_MONKEY
    #ifdef CONFIG_LV_USE_MONKEY
        #define LV_USE_MONKEY CONFIG_LV_USE_MONKEY
    #else
        #define LV_USE_MONKEY 0
    #endif
#endif

/*1: Enable grid navigation*/
#ifndef LV_USE_GRIDNAV
    #ifdef CONFIG_LV_USE_GRIDNAV
        #define LV_USE_GRIDNAV CONFIG_LV_USE_GRIDNAV
    #else
        #define LV_USE_GRIDNAV 0
    #endif
#endif

/*1: Enable lv_obj fragment*/
#ifndef LV_USE_FRAGMENT
    #ifdef CONFIG_LV_USE_FRAGMENT
        #define LV_USE_FRAGMENT CONFIG_LV_USE_FRAGMENT
    #else
        #define LV_USE_FRAGMENT 0
    #endif
#endif

/*1: Support using images as font in label or span widgets */
#ifndef LV_USE_IMGFONT
    #ifdef CONFIG_LV_USE_IMGFONT
        #define LV_USE_IMGFONT CONFIG_LV_USE_IMGFONT
    #else
        #define LV_USE_IMGFONT 0
    #endif
#endif

/*1: Enable a published subscriber based messaging system */
#ifndef LV_USE_MSG
    #ifdef CONFIG_LV_USE_MSG
        #define LV_USE_MSG CONFIG_LV_USE_MSG
    #else
        #define LV_USE_MSG 0
    #endif
#endif

/*1: Enable Pinyin input method*/
/*Requires: lv_keyboard*/
#ifndef LV_USE_IME_PINYIN
    #ifdef CONFIG_LV_USE_IME_PINYIN
        #define LV_USE_IME_PINYIN CONFIG_LV_USE_IME_PINYIN
    #else
        #define LV_USE_IME_PINYIN 0
    #endif
#endif
#if LV_USE_IME_PINYIN
    /*1: Use default thesaurus*/
    /*If you do not use the default thesaurus, be sure to use `lv_ime_pinyin` after setting the thesauruss*/
    #ifndef LV_IME_PINYIN_USE_DEFAULT_DICT
        #ifdef _LV_KCONFIG_PRESENT
            #ifdef CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
                #define LV_IME_PINYIN_USE_DEFAULT_DICT CONFIG_LV_IME_PINYIN_USE_DEFAULT_DICT
            #else
                #define LV_IME_PINYIN_USE_DEFAULT_DICT 0
            #endif
        #else
            #define LV_IME_PINYIN_USE_DEFAULT_DICT 1
        #endif
    #endif
    /*Set the maximum number of candidate panels that can be displayed*/
    /*This needs to be adjusted according to the size of the screen*/
    #ifndef LV_IME_PINYIN_CAND_TEXT_NUM
        #ifdef CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
            #define LV_IME_PINYIN_CAND_TEXT_NUM CONFIG_LV_IME_PINYIN_CAND_TEXT_NUM
        #else
            #define LV_IME_PINYIN_CAND_TEXT_NUM 6
        #endif
    #endif

    /*Use 9 key input(k9)*/
    #ifndef LV_IME_PINYIN_USE_K9_MODE
        #ifdef _LV_KCONFIG_PRESENT
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
    #endif // LV_IME_PINYIN_USE_K9_MODE
#endif

/*==================
* EXAMPLES
*==================*/

/*Enable the examples to be built with the library*/
#ifndef LV_BUILD_EXAMPLES
    #ifdef _LV_KCONFIG_PRESENT
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

/*Show some widget. It might be required to increase `LV_MEM_SIZE` */
#ifndef LV_USE_DEMO_WIDGETS
    #ifdef CONFIG_LV_USE_DEMO_WIDGETS
        #define LV_USE_DEMO_WIDGETS CONFIG_LV_USE_DEMO_WIDGETS
    #else
        #define LV_USE_DEMO_WIDGETS 0
    #endif
#endif
#if LV_USE_DEMO_WIDGETS
#ifndef LV_DEMO_WIDGETS_SLIDESHOW
    #ifdef CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
        #define LV_DEMO_WIDGETS_SLIDESHOW CONFIG_LV_DEMO_WIDGETS_SLIDESHOW
    #else
        #define LV_DEMO_WIDGETS_SLIDESHOW 0
    #endif
#endif
#endif

/*Demonstrate the usage of encoder and keyboard*/
#ifndef LV_USE_DEMO_KEYPAD_AND_ENCODER
    #ifdef CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER CONFIG_LV_USE_DEMO_KEYPAD_AND_ENCODER
    #else
        #define LV_USE_DEMO_KEYPAD_AND_ENCODER 0
    #endif
#endif

/*Benchmark your system*/
#ifndef LV_USE_DEMO_BENCHMARK
    #ifdef CONFIG_LV_USE_DEMO_BENCHMARK
        #define LV_USE_DEMO_BENCHMARK CONFIG_LV_USE_DEMO_BENCHMARK
    #else
        #define LV_USE_DEMO_BENCHMARK 0
    #endif
#endif
#if LV_USE_DEMO_BENCHMARK
/*Use RGB565A8 images with 16 bit color depth instead of ARGB8565*/
#ifndef LV_DEMO_BENCHMARK_RGB565A8
    #ifdef CONFIG_LV_DEMO_BENCHMARK_RGB565A8
        #define LV_DEMO_BENCHMARK_RGB565A8 CONFIG_LV_DEMO_BENCHMARK_RGB565A8
    #else
        #define LV_DEMO_BENCHMARK_RGB565A8 0
    #endif
#endif
#endif

/*Stress test for LVGL*/
#ifndef LV_USE_DEMO_STRESS
    #ifdef CONFIG_LV_USE_DEMO_STRESS
        #define LV_USE_DEMO_STRESS CONFIG_LV_USE_DEMO_STRESS
    #else
        #define LV_USE_DEMO_STRESS 0
    #endif
#endif

/*Music player demo*/
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



/*----------------------------------
 * End of parsing lv_conf_template.h
 -----------------------------------*/

LV_EXPORT_CONST_INT(LV_DPI_DEF);

#undef _LV_KCONFIG_PRESENT


/*Set some defines if a dependency is disabled*/
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


/*If running without lv_conf.h add typedefs with default value*/
#ifdef LV_CONF_SKIP
    #if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /*Disable warnings for Visual Studio*/
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#endif  /*defined(LV_CONF_SKIP)*/

#endif  /*LV_CONF_INTERNAL_H*/

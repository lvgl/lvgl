/**
 * @file lv_conf.h
 * Configuration file for v7.10.1
 */

/*
 * COPY THIS FILE AS `lv_conf.h` NEXT TO the `lvgl` FOLDER
 */

#if 0 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H
/* clang-format off */

#include <stdint.h>

/*====================
   Graphical settings
 *====================*/

/* Maximal horizontal and vertical resolution to support by the library.*/
#define LV_HOR_RES_MAX          (480)
#define LV_VER_RES_MAX          (320)

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB332
 * - 16: RGB565
 * - 32: ARGB8888
 */
#define LV_COLOR_DEPTH     16

/* Swap the 2 bytes of RGB565 color.
 * Useful if the display has a 8 bit interface (e.g. SPI)*/
#define LV_COLOR_16_SWAP   0

/* 1: Enable screen transparency.
 * Useful for OSD or other overlapping GUIs.
 * Requires `LV_COLOR_DEPTH = 32` colors and the screen's style should be modified: `style.body.opa = ...`*/
#define LV_COLOR_SCREEN_TRANSP    0

/*Images pixels with this color will not be drawn (with chroma keying)*/
#define LV_COLOR_TRANSP    LV_COLOR_LIME         /*LV_COLOR_LIME: pure green*/

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#define LV_ANTIALIAS        1

/* Default display refresh period.
 * Can be changed in the display driver (`lv_disp_drv_t`).*/
#define LV_DISP_DEF_REFR_PERIOD      30      /*[ms]*/

/* Dot Per Inch: used to initialize default sizes.
 * E.g. a button with width = LV_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#define LV_DPI              130     /*[px]*/

/* The the real width of the display changes some default values:
 * default object sizes, layout of examples, etc.
 * According to the width of the display (hor. res. / dpi)
 * the displays fall in 4 categories.
 * The 4th is extra large which has no upper limit so not listed here
 * The upper limit of the categories are set below in 0.1 inch unit.
 */
#define LV_DISP_SMALL_LIMIT  30
#define LV_DISP_MEDIUM_LIMIT 50
#define LV_DISP_LARGE_LIMIT  70

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */
typedef int16_t lv_coord_t;

/* Maximum buffer size to allocate for rotation. Only used if software rotation is enabled. */
#define LV_DISP_ROT_MAX_BUF  (10U * 1024U)

/*=========================
   Memory manager settings
 *=========================*/

/* LittelvGL's internal memory manager's settings.
 * The graphical objects and other related data are stored here. */

/* 1: use custom malloc/free, 0: use the built-in `lv_mem_alloc` and `lv_mem_free` */
#define LV_MEM_CUSTOM      0
#if LV_MEM_CUSTOM == 0
/* Size of the memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#  define LV_MEM_SIZE    (32U * 1024U)

/* Compiler prefix for a big array declaration */
#  define LV_MEM_ATTR

/* Set an address for the memory pool instead of allocating it as an array.
 * Can be in external SRAM too. */
#  define LV_MEM_ADR          0

/* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
#  define LV_MEM_AUTO_DEFRAG  1
#else       /*LV_MEM_CUSTOM*/
#  define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#  define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#  define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif     /*LV_MEM_CUSTOM*/

/* Use the standard memcpy and memset instead of LVGL's own functions.
 * The standard functions might or might not be faster depending on their implementation. */
#define LV_MEMCPY_MEMSET_STD    0

/* Garbage Collector settings
 * Used if lvgl is binded to higher level language and the memory is managed by that language */
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
#  define LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#  define LV_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
#  define LV_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to lv_mem_get_size*/
#endif /* LV_ENABLE_GC */

/*=======================
   Input device settings
 *=======================*/

/* Input device default settings.
 * Can be changed in the Input device driver (`lv_indev_drv_t`)*/

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD          30

/* Drag threshold in pixels */
#define LV_INDEV_DEF_DRAG_LIMIT           10

/* Drag throw slow-down in [%]. Greater value -> faster slow-down */
#define LV_INDEV_DEF_DRAG_THROW           10

/* Long press time in milliseconds.
 * Time to send `LV_EVENT_LONG_PRESSED`) */
#define LV_INDEV_DEF_LONG_PRESS_TIME      400

/* Repeated trigger period in long press [ms]
 * Time between `LV_EVENT_LONG_PRESSED_REPEAT */
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100

/* Gesture threshold in pixels */
#define LV_INDEV_DEF_GESTURE_LIMIT        50

/* Gesture min velocity at release before swipe (pixels)*/
#define LV_INDEV_DEF_GESTURE_MIN_VELOCITY 3

/*==================
 * Feature usage
 *==================*/

/*1: Enable the Animations */
#define LV_USE_ANIMATION        1
#if LV_USE_ANIMATION

/*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_anim_user_data_t;

#endif

/* 1: Enable shadow drawing on rectangles*/
#define LV_USE_SHADOW           1
#if LV_USE_SHADOW
/* Allow buffering some shadow calculation
 * LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer,
 * where shadow size is `shadow_width + radius`
 * Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
#define LV_SHADOW_CACHE_SIZE    0
#endif

/*1: enable outline drawing on rectangles*/
#define LV_USE_OUTLINE  1

/*1: enable pattern drawing on rectangles*/
#define LV_USE_PATTERN  1

/*1: enable value string drawing on rectangles*/
#define LV_USE_VALUE_STR    1

/* 1: Use other blend modes than normal (`LV_BLEND_MODE_...`)*/
#define LV_USE_BLEND_MODES      1

/* 1: Use the `opa_scale` style property to set the opacity of an object and its children at once*/
#define LV_USE_OPA_SCALE        1

/* 1: Use image zoom and rotation*/
#define LV_USE_IMG_TRANSFORM    1

/* 1: Enable object groups (for keyboard/encoder navigation) */
#define LV_USE_GROUP            1
#if LV_USE_GROUP
typedef void * lv_group_user_data_t;
#endif  /*LV_USE_GROUP*/

/* 1: Enable GPU interface*/
#define LV_USE_GPU              1   /*Only enables `gpu_fill_cb` and `gpu_blend_cb` in the disp. drv- */
#define LV_USE_GPU_STM32_DMA2D  0
/*If enabling LV_USE_GPU_STM32_DMA2D, LV_GPU_DMA2D_CMSIS_INCLUDE must be defined to include path of CMSIS header of target processor
e.g. "stm32f769xx.h" or "stm32f429xx.h" */
#define LV_GPU_DMA2D_CMSIS_INCLUDE

/*1: Use PXP for CPU off-load on NXP RTxxx platforms */
#define LV_USE_GPU_NXP_PXP      0

/*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
 *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol FSL_RTOS_FREE_RTOS
 *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
 *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
 * */
#define LV_USE_GPU_NXP_PXP_AUTO_INIT 0

/*1: Use VG-Lite for CPU offload on NXP RTxxx platforms */
#define LV_USE_GPU_NXP_VG_LITE   0

/* 1: Enable file system (might be required for images */
#define LV_USE_FILESYSTEM       1
#if LV_USE_FILESYSTEM
/*Declare the type of the user data of file system drivers (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_fs_drv_user_data_t;
#endif

/*1: Add a `user_data` to drivers and objects*/
#define LV_USE_USER_DATA        0

/*1: Show CPU usage and FPS count in the right bottom corner*/
#define LV_USE_PERF_MONITOR     0

/*1: Use the functions and types from the older API if possible */
#define LV_USE_API_EXTENSION_V6  1
#define LV_USE_API_EXTENSION_V7  1

/*========================
 * Image decoder and cache
 *========================*/

/* 1: Enable indexed (palette) images */
#define LV_IMG_CF_INDEXED       1

/* 1: Enable alpha indexed images */
#define LV_IMG_CF_ALPHA         1

/* Default image cache size. Image caching keeps the images opened.
 * If only the built-in image formats are used there is no real advantage of caching.
 * (I.e. no new image decoder is added)
 * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 * However the opened images might consume additional RAM.
 * Set it to 0 to disable caching */
#define LV_IMG_CACHE_DEF_SIZE       1

/*Declare the type of the user data of image decoder (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_img_decoder_user_data_t;

/*=====================
 *  Compiler settings
 *====================*/

/* For big endian systems set to 1 */
#define LV_BIG_ENDIAN_SYSTEM    0

/* Define a custom attribute to `lv_tick_inc` function */
#define LV_ATTRIBUTE_TICK_INC

/* Define a custom attribute to `lv_task_handler` function */
#define LV_ATTRIBUTE_TASK_HANDLER

/* Define a custom attribute to `lv_disp_flush_ready` function */
#define LV_ATTRIBUTE_FLUSH_READY

/* Required alignment size for buffers */
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE

/* With size optimization (-Os) the compiler might not align data to
 * 4 or 8 byte boundary. Some HW may need even 32 or 64 bytes.
 * This alignment will be explicitly applied where needed.
 * LV_ATTRIBUTE_MEM_ALIGN_SIZE should be used to specify required align size.
 * E.g. __attribute__((aligned(LV_ATTRIBUTE_MEM_ALIGN_SIZE))) */
#define LV_ATTRIBUTE_MEM_ALIGN

/* Attribute to mark large constant arrays for example
 * font's bitmaps */
#define LV_ATTRIBUTE_LARGE_CONST

/* Prefix performance critical functions to place them into a faster memory (e.g RAM)
 * Uses 15-20 kB extra memory */
#define LV_ATTRIBUTE_FAST_MEM

/* Export integer constant to binding.
 * This macro is used with constants in the form of LV_<CONST> that
 * should also appear on lvgl binding API such as Micropython
 *
 * The default value just prevents a GCC warning.
 */
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning

/* Prefix variables that are used in GPU accelerated operations, often these need to be
 * placed in RAM sections that are DMA accessible */
#define LV_ATTRIBUTE_DMA

/*===================
 *  HAL settings
 *==================*/

/* 1: use a custom tick source.
 * It removes the need to manually update the tick with `lv_tick_inc`) */
#define LV_TICK_CUSTOM     0
#if LV_TICK_CUSTOM == 1
#define LV_TICK_CUSTOM_INCLUDE  "Arduino.h"         /*Header for the system time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

typedef void * lv_disp_drv_user_data_t;             /*Type of user data in the display driver*/
typedef void * lv_indev_drv_user_data_t;            /*Type of user data in the input device driver*/

/*================
 * Log settings
 *===============*/

/*1: Enable the log module*/
#define LV_USE_LOG      0
#if LV_USE_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 * LV_LOG_LEVEL_NONE        Do not log anything
 */
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN

/* 1: Print the log with 'printf';
 * 0: user need to register a callback with `lv_log_register_print_cb`*/
#  define LV_LOG_PRINTF   0
#endif  /*LV_USE_LOG*/

/*=================
 * Debug settings
 *================*/

/* If Debug is enabled LittelvGL validates the parameters of the functions.
 * If an invalid parameter is found an error log message is printed and
 * the MCU halts at the error. (`LV_USE_LOG` should be enabled)
 * If you are debugging the MCU you can pause
 * the debugger to see exactly where the issue is.
 *
 * The behavior of asserts can be overwritten by redefining them here.
 * E.g. #define LV_ASSERT_MEM(p)  <my_assert_code>
 */
#define LV_USE_DEBUG        1
#if LV_USE_DEBUG

/*Check if the parameter is NULL. (Quite fast) */
#define LV_USE_ASSERT_NULL      1

/*Checks is the memory is successfully allocated or no. (Quite fast)*/
#define LV_USE_ASSERT_MEM       1

/*Check the integrity of `lv_mem` after critical operations. (Slow)*/
#define LV_USE_ASSERT_MEM_INTEGRITY       0

/* Check the strings.
 * Search for NULL, very long strings, invalid characters, and unnatural repetitions. (Slow)
 * If disabled `LV_USE_ASSERT_NULL` will be performed instead (if it's enabled) */
#define LV_USE_ASSERT_STR       0

/* Check NULL, the object's type and existence (e.g. not deleted). (Quite slow)
 * If disabled `LV_USE_ASSERT_NULL` will be performed instead (if it's enabled) */
#define LV_USE_ASSERT_OBJ       0

/*Check if the styles are properly initialized. (Fast)*/
#define LV_USE_ASSERT_STYLE     0

#endif /*LV_USE_DEBUG*/

/*==================
 *    FONT USAGE
 *===================*/

/* The built-in fonts contains the ASCII range and some Symbols with 4 bit-per-pixel.
 * The symbols are available via `LV_SYMBOL_...` defines
 * More info about fonts: https://docs.lvgl.io/v7/en/html/overview/font.html
 * To create a new font go to: https://lvgl.com/ttf-font-to-c-array
 */

/* Montserrat fonts with bpp = 4
 * https://fonts.google.com/specimen/Montserrat  */
#define LV_FONT_MONTSERRAT_8     0
#define LV_FONT_MONTSERRAT_10    0
#define LV_FONT_MONTSERRAT_12    0
#define LV_FONT_MONTSERRAT_14    1
#define LV_FONT_MONTSERRAT_16    0
#define LV_FONT_MONTSERRAT_18    0
#define LV_FONT_MONTSERRAT_20    0
#define LV_FONT_MONTSERRAT_22    0
#define LV_FONT_MONTSERRAT_24    0
#define LV_FONT_MONTSERRAT_26    0
#define LV_FONT_MONTSERRAT_28    0
#define LV_FONT_MONTSERRAT_30    0
#define LV_FONT_MONTSERRAT_32    0
#define LV_FONT_MONTSERRAT_34    0
#define LV_FONT_MONTSERRAT_36    0
#define LV_FONT_MONTSERRAT_38    0
#define LV_FONT_MONTSERRAT_40    0
#define LV_FONT_MONTSERRAT_42    0
#define LV_FONT_MONTSERRAT_44    0
#define LV_FONT_MONTSERRAT_46    0
#define LV_FONT_MONTSERRAT_48    0

/* Demonstrate special features */
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, PErisan letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/

/*Pixel perfect monospace font
 * http://pelulamu.net/unscii/ */
#define LV_FONT_UNSCII_8     0
#define LV_FONT_UNSCII_16     0

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2)
 */
#define LV_FONT_CUSTOM_DECLARE

/* Enable it if you have fonts with a lot of characters.
 * The limit depends on the font size, font face and bpp
 * but with > 10,000 characters if you see issues probably you need to enable it.*/
#define LV_FONT_FMT_TXT_LARGE   0

/* Enables/disables support for compressed fonts. If it's disabled, compressed
 * glyphs cannot be processed by the library and won't be rendered.
 */
#define LV_USE_FONT_COMPRESSED 1

/* Enable subpixel rendering */
#define LV_USE_FONT_SUBPX 1
#if LV_USE_FONT_SUBPX
/* Set the pixel order of the display.
 * Important only if "subpx fonts" are used.
 * With "normal" font it doesn't matter.
 */
#define LV_FONT_SUBPX_BGR    0
#endif

/*Declare the type of the user data of fonts (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_font_user_data_t;

/*================
 *  THEME USAGE
 *================*/

/*Always enable at least on theme*/

/* No theme, you can apply your styles as you need
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#define LV_USE_THEME_EMPTY       1

/*Simple to the create your theme based on it
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#define LV_USE_THEME_TEMPLATE    1

/* A fast and impressive theme.
 * Flags:
 * LV_THEME_MATERIAL_FLAG_LIGHT: light theme
 * LV_THEME_MATERIAL_FLAG_DARK: dark theme
 * LV_THEME_MATERIAL_FLAG_NO_TRANSITION: disable transitions (state change animations)
 * LV_THEME_MATERIAL_FLAG_NO_FOCUS: disable indication of focused state)
 * */
#define LV_USE_THEME_MATERIAL    1

/* Mono-color theme for monochrome displays.
 * If LV_THEME_DEFAULT_COLOR_PRIMARY is LV_COLOR_BLACK the
 * texts and borders will be black and the background will be
 * white. Else the colors are inverted.
 * No flags. Set LV_THEME_DEFAULT_FLAG 0 */
#define LV_USE_THEME_MONO        1

#define LV_THEME_DEFAULT_INCLUDE            <stdint.h>      /*Include a header for the init. function*/
#define LV_THEME_DEFAULT_INIT               lv_theme_material_init
#define LV_THEME_DEFAULT_COLOR_PRIMARY      lv_color_hex(0x01a2b1)
#define LV_THEME_DEFAULT_COLOR_SECONDARY    lv_color_hex(0x44d1b6)
#define LV_THEME_DEFAULT_FLAG               LV_THEME_MATERIAL_FLAG_LIGHT
#define LV_THEME_DEFAULT_FONT_SMALL         &lv_font_montserrat_14
#define LV_THEME_DEFAULT_FONT_NORMAL        &lv_font_montserrat_14
#define LV_THEME_DEFAULT_FONT_SUBTITLE      &lv_font_montserrat_14
#define LV_THEME_DEFAULT_FONT_TITLE         &lv_font_montserrat_14

/*=================
 *  Text settings
 *=================*/

/* Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 * */
#define LV_TXT_ENC LV_TXT_ENC_UTF8

 /*Can break (wrap) texts on these chars*/
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"

/* If a word is at least this long, will break wherever "prettiest"
 * To disable, set to a value <= 0 */
#define LV_TXT_LINE_BREAK_LONG_LEN          0

/* Minimum number of characters in a long word to put on a line before a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3

/* Minimum number of characters in a long word to put on a line after a break.
 * Depends on LV_TXT_LINE_BREAK_LONG_LEN. */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/* The control character to use for signalling text recoloring. */
#define LV_TXT_COLOR_CMD "#"

/* Support bidirectional texts.
 * Allows mixing Left-to-Right and Right-to-Left texts.
 * The direction will be processed according to the Unicode Bidirectional Algorithm:
 * https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI     0
#if LV_USE_BIDI
/* Set the default direction. Supported values:
 * `LV_BIDI_DIR_LTR` Left-to-Right
 * `LV_BIDI_DIR_RTL` Right-to-Left
 * `LV_BIDI_DIR_AUTO` detect texts base direction */
#define LV_BIDI_BASE_DIR_DEF  LV_BIDI_DIR_AUTO
#endif

/* Enable Arabic/Persian processing
 * In these languages characters should be replaced with
 * an other form based on their position in the text */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM   0
#if LV_SPRINTF_CUSTOM
#  define LV_SPRINTF_INCLUDE <stdio.h>
#  define lv_snprintf     snprintf
#  define lv_vsnprintf    vsnprintf
#else   /*!LV_SPRINTF_CUSTOM*/
#  define LV_SPRINTF_DISABLE_FLOAT 1
#endif  /*LV_SPRINTF_CUSTOM*/

/*===================
 *  LV_OBJ SETTINGS
 *==================*/

#if LV_USE_USER_DATA
/*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_obj_user_data_t;
/*Provide a function to free user data*/
#define LV_USE_USER_DATA_FREE 0
#if LV_USE_USER_DATA_FREE
#  define LV_USER_DATA_FREE_INCLUDE  "something.h"  /*Header for user data free function*/
/* Function prototype : void user_data_free(lv_obj_t * obj); */
#  define LV_USER_DATA_FREE  (user_data_free)       /*Invoking for user data free function*/
#endif
#endif

/*1: enable `lv_obj_realign()` based on `lv_obj_align()` parameters*/
#define LV_USE_OBJ_REALIGN          1

/* Enable to make the object clickable on a larger area.
 * LV_EXT_CLICK_AREA_OFF or 0: Disable this feature
 * LV_EXT_CLICK_AREA_TINY: The extra area can be adjusted horizontally and vertically (0..255 px)
 * LV_EXT_CLICK_AREA_FULL: The extra area can be adjusted in all 4 directions (-32k..+32k px)
 */
#define LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_TINY

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.lvgl.com/#Object-types
 */

/*Arc (dependencies: -)*/
#define LV_USE_ARC      1

/*Bar (dependencies: -)*/
#define LV_USE_BAR      1

/*Button (dependencies: lv_cont*/
#define LV_USE_BTN      1

/*Button matrix (dependencies: -)*/
#define LV_USE_BTNMATRIX     1

/*Calendar (dependencies: -)*/
#define LV_USE_CALENDAR 1
#if LV_USE_CALENDAR
#  define LV_CALENDAR_WEEK_STARTS_MONDAY    0
#endif

/*Canvas (dependencies: lv_img)*/
#define LV_USE_CANVAS   1

/*Check box (dependencies: lv_btn, lv_label)*/
#define LV_USE_CHECKBOX       1

/*Chart (dependencies: -)*/
#define LV_USE_CHART    1
#if LV_USE_CHART
#  define LV_CHART_AXIS_TICK_LABEL_MAX_LEN    256
#endif

/*Container (dependencies: -*/
#define LV_USE_CONT     1

/*Color picker (dependencies: -*/
#define LV_USE_CPICKER   1

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#define LV_USE_DROPDOWN    1
#if LV_USE_DROPDOWN != 0
/*Open and close default animation time [ms] (0: no animation)*/
#  define LV_DROPDOWN_DEF_ANIM_TIME     200
#endif

/*Gauge (dependencies:lv_bar, lv_linemeter)*/
#define LV_USE_GAUGE    1

/*Image (dependencies: lv_label*/
#define LV_USE_IMG      1

/*Image Button (dependencies: lv_btn*/
#define LV_USE_IMGBTN   1
#if LV_USE_IMGBTN
/*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#  define LV_IMGBTN_TILED 0
#endif

/*Keyboard (dependencies: lv_btnm)*/
#define LV_USE_KEYBOARD       1

/*Label (dependencies: -*/
#define LV_USE_LABEL    1
#if LV_USE_LABEL != 0
/*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
#  define LV_LABEL_DEF_SCROLL_SPEED       25

/* Waiting period at beginning/end of animation cycle */
#  define LV_LABEL_WAIT_CHAR_COUNT        3

/*Enable selecting text of the label */
#  define LV_LABEL_TEXT_SEL               0

/*Store extra some info in labels (12 bytes) to speed up drawing of very long texts*/
#  define LV_LABEL_LONG_TXT_HINT          0
#endif

/*LED (dependencies: -)*/
#define LV_USE_LED      1
#if LV_USE_LED
#  define LV_LED_BRIGHT_MIN  120      /*Minimal brightness*/
#  define LV_LED_BRIGHT_MAX  255     /*Maximal brightness*/
#endif

/*Line (dependencies: -*/
#define LV_USE_LINE     1

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#define LV_USE_LIST     1
#if LV_USE_LIST != 0
/*Default animation time of focusing to a list element [ms] (0: no animation)  */
#  define LV_LIST_DEF_ANIM_TIME  100
#endif

/*Line meter (dependencies: *;)*/
#define LV_USE_LINEMETER   1
#if LV_USE_LINEMETER
/* Draw line more precisely at cost of performance.
 * Useful if there are lot of lines any minor are visible
 * 0: No extra precision
 * 1: Some extra precision
 * 2: Best precision
 */
#  define LV_LINEMETER_PRECISE    1
#endif

/*Mask (dependencies: -)*/
#define LV_USE_OBJMASK  1

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#define LV_USE_MSGBOX     1

/*Page (dependencies: lv_cont)*/
#define LV_USE_PAGE     1
#if LV_USE_PAGE != 0
/*Focus default animation time [ms] (0: no animation)*/
#  define LV_PAGE_DEF_ANIM_TIME     400
#endif

/*Preload (dependencies: lv_arc, lv_anim)*/
#define LV_USE_SPINNER      1
#if LV_USE_SPINNER != 0
#  define LV_SPINNER_DEF_ARC_LENGTH   60      /*[deg]*/
#  define LV_SPINNER_DEF_SPIN_TIME    1000    /*[ms]*/
#  define LV_SPINNER_DEF_ANIM         LV_SPINNER_TYPE_SPINNING_ARC
#endif

/*Roller (dependencies: lv_ddlist)*/
#define LV_USE_ROLLER    1
#if LV_USE_ROLLER != 0
/*Focus animation time [ms] (0: no animation)*/
#  define LV_ROLLER_DEF_ANIM_TIME     200

/*Number of extra "pages" when the roller is infinite*/
#  define LV_ROLLER_INF_PAGES         7
#endif

/*Slider (dependencies: lv_bar)*/
#define LV_USE_SLIDER    1

/*Spinbox (dependencies: lv_ta)*/
#define LV_USE_SPINBOX       1

/*Switch (dependencies: lv_slider)*/
#define LV_USE_SWITCH       1

/*Text area (dependencies: lv_label, lv_page)*/
#define LV_USE_TEXTAREA       1
#if LV_USE_TEXTAREA != 0
#  define LV_TEXTAREA_DEF_CURSOR_BLINK_TIME 400     /*ms*/
#  define LV_TEXTAREA_DEF_PWD_SHOW_TIME     1500    /*ms*/
#endif

/*Table (dependencies: lv_label)*/
#define LV_USE_TABLE    1
#if LV_USE_TABLE
#  define LV_TABLE_COL_MAX    12
#  define LV_TABLE_CELL_STYLE_CNT 4
#endif

/*Tab (dependencies: lv_page, lv_btnm)*/
#define LV_USE_TABVIEW      1
#  if LV_USE_TABVIEW != 0
/*Time of slide animation [ms] (0: no animation)*/
#  define LV_TABVIEW_DEF_ANIM_TIME    300
#endif

/*Tileview (dependencies: lv_page) */
#define LV_USE_TILEVIEW     1
#if LV_USE_TILEVIEW
/*Time of slide animation [ms] (0: no animation)*/
#  define LV_TILEVIEW_DEF_ANIM_TIME   300
#endif

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#define LV_USE_WIN      1

/*==================
 * Non-user section
 *==================*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
#  define _CRT_SECURE_NO_WARNINGS
#endif

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/

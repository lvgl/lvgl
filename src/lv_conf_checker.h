/**
 * GENERATED FILE, DO NOT EDIT IT!
 * @file lv_conf_checker.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_CHECKER_H
#define  LV_CONF_CHECKER_H
/* clang-format off */

#include <stdint.h>

/*====================
   Graphical settings
 *====================*/

/* Maximal horizontal and vertical resolution to support by the library.*/
#ifndef LV_HOR_RES_MAX
#define LV_HOR_RES_MAX          (480)
#endif
#ifndef LV_VER_RES_MAX
#define LV_VER_RES_MAX          (320)
#endif

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB233
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifndef LV_COLOR_DEPTH
#define LV_COLOR_DEPTH     16
#endif

/* Swap the 2 bytes of RGB565 color.
 * Useful if the display has a 8 bit interface (e.g. SPI)*/
#ifndef LV_COLOR_16_SWAP
#define LV_COLOR_16_SWAP   0
#endif

/* 1: Enable screen transparency.
 * Useful for OSD or other overlapping GUIs.
 * Requires `LV_COLOR_DEPTH = 32` colors and the screen's style should be modified: `style.body.opa = ...`*/
#ifndef LV_COLOR_SCREEN_TRANSP
#define LV_COLOR_SCREEN_TRANSP    0
#endif

/*Images pixels with this color will not be drawn (with chroma keying)*/
#ifndef LV_COLOR_TRANSP
#define LV_COLOR_TRANSP    LV_COLOR_LIME         /*LV_COLOR_LIME: pure green*/
#endif

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#ifndef LV_ANTIALIAS
#define LV_ANTIALIAS        1
#endif

/* Default display refresh period.
 * Can be changed in the display driver (`lv_disp_drv_t`).*/
#ifndef LV_DISP_DEF_REFR_PERIOD
#define LV_DISP_DEF_REFR_PERIOD      30      /*[ms]*/
#endif

/* Dot Per Inch: used to initialize default sizes.
 * E.g. a button with width = LV_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#ifndef LV_DPI
#define LV_DPI              100     /*[px]*/
#endif

/* Type of coordinates. Should be `int16_t` (or `int32_t` for extreme cases) */

/*=========================
   Memory manager settings
 *=========================*/

/* LittelvGL's internal memory manager's settings.
 * The graphical objects and other related data are stored here. */

/* 1: use custom malloc/free, 0: use the built-in `lv_mem_alloc` and `lv_mem_free` */
#ifndef LV_MEM_CUSTOM
#define LV_MEM_CUSTOM      0
#endif
#if LV_MEM_CUSTOM == 0
/* Size of the memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#ifndef LV_MEM_SIZE
#  define LV_MEM_SIZE    (32U * 1024U)
#endif

/* Complier prefix for a big array declaration */
#ifndef LV_MEM_ATTR
#  define LV_MEM_ATTR
#endif

/* Set an address for the memory pool instead of allocating it as an array.
 * Can be in external SRAM too. */
#ifndef LV_MEM_ADR
#  define LV_MEM_ADR          0
#endif

/* Automatically defrag. on free. Defrag. means joining the adjacent free cells. */
#ifndef LV_MEM_AUTO_DEFRAG
#  define LV_MEM_AUTO_DEFRAG  1
#endif
#else       /*LV_MEM_CUSTOM*/
#ifndef LV_MEM_CUSTOM_INCLUDE
#  define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#endif
#ifndef LV_MEM_CUSTOM_ALLOC
#  define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#endif
#ifndef LV_MEM_CUSTOM_FREE
#  define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif
#endif     /*LV_MEM_CUSTOM*/

/* Garbage Collector settings
 * Used if lvgl is binded to higher level language and the memory is managed by that language */
#ifndef LV_ENABLE_GC
#define LV_ENABLE_GC 0
#endif
#if LV_ENABLE_GC != 0
#ifndef LV_GC_INCLUDE
#  define LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#endif
#ifndef LV_MEM_CUSTOM_REALLOC
#  define LV_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
#endif
#ifndef LV_MEM_CUSTOM_GET_SIZE
#  define LV_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to lv_mem_get_size*/
#endif
#endif /* LV_ENABLE_GC */

/*=======================
   Input device settings
 *=======================*/

/* Input device default settings.
 * Can be changed in the Input device driver (`lv_indev_drv_t`)*/

/* Input device read period in milliseconds */
#ifndef LV_INDEV_DEF_READ_PERIOD
#define LV_INDEV_DEF_READ_PERIOD          30
#endif

/* Drag threshold in pixels */
#ifndef LV_INDEV_DEF_DRAG_LIMIT
#define LV_INDEV_DEF_DRAG_LIMIT           10
#endif

/* Drag throw slow-down in [%]. Greater value -> faster slow-down */
#ifndef LV_INDEV_DEF_DRAG_THROW
#define LV_INDEV_DEF_DRAG_THROW           20
#endif

/* Long press time in milliseconds.
 * Time to send `LV_EVENT_LONG_PRESSSED`) */
#ifndef LV_INDEV_DEF_LONG_PRESS_TIME
#define LV_INDEV_DEF_LONG_PRESS_TIME      400
#endif

/* Repeated trigger period in long press [ms]
 * Time between `LV_EVENT_LONG_PRESSED_REPEAT */
#ifndef LV_INDEV_DEF_LONG_PRESS_REP_TIME
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100
#endif

/*==================
 * Feature usage
 *==================*/

/*1: Enable the Animations */
#ifndef LV_USE_ANIMATION
#define LV_USE_ANIMATION        1
#endif
#if LV_USE_ANIMATION

/*Declare the type of the user data of animations (can be e.g. `void *`, `int`, `struct`)*/

#endif

/* 1: Enable shadow drawing*/
#ifndef LV_USE_SHADOW
#define LV_USE_SHADOW           1
#endif

/* 1: Enable object groups (for keyboard/encoder navigation) */
#ifndef LV_USE_GROUP
#define LV_USE_GROUP            1
#endif
#if LV_USE_GROUP
#endif  /*LV_USE_GROUP*/

/* 1: Enable GPU interface*/
#ifndef LV_USE_GPU
#define LV_USE_GPU              1
#endif

/* 1: Enable file system (might be required for images */
#ifndef LV_USE_FILESYSTEM
#define LV_USE_FILESYSTEM       1
#endif
#if LV_USE_FILESYSTEM
/*Declare the type of the user data of file system drivers (can be e.g. `void *`, `int`, `struct`)*/
#endif

/*1: Add a `user_data` to drivers and objects*/
#ifndef LV_USE_USER_DATA
#define LV_USE_USER_DATA        1
#endif

/*========================
 * Image decoder and cache
 *========================*/

/* 1: Enable indexed (palette) images */
#ifndef LV_IMG_CF_INDEXED
#define LV_IMG_CF_INDEXED       1
#endif

/* 1: Enable alpha indexed images */
#ifndef LV_IMG_CF_ALPHA
#define LV_IMG_CF_ALPHA         1
#endif

/* Default image cache size. Image caching keeps the images opened.
 * If only the built-in image formats are used there is no real advantage of caching.
 * (I.e. no new image decoder is added)
 * With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 * However the opened images might consume additional RAM.
 * LV_IMG_CACHE_DEF_SIZE must be >= 1 */
#ifndef LV_IMG_CACHE_DEF_SIZE
#define LV_IMG_CACHE_DEF_SIZE       1
#endif

/*Declare the type of the user data of image decoder (can be e.g. `void *`, `int`, `struct`)*/

/*=====================
 *  Compiler settings
 *====================*/
/* Define a custom attribute to `lv_tick_inc` function */
#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC
#endif

/* Define a custom attribute to `lv_task_handler` function */
#ifndef LV_ATTRIBUTE_TASK_HANDLER
#define LV_ATTRIBUTE_TASK_HANDLER
#endif

/* With size optimization (-Os) the compiler might not align data to
 * 4 or 8 byte boundary. This alignment will be explicitly applied where needed.
 * E.g. __attribute__((aligned(4))) */
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

/* Attribute to mark large constant arrays for example
 * font's bitmaps */
#ifndef LV_ATTRIBUTE_LARGE_CONST
#define LV_ATTRIBUTE_LARGE_CONST
#endif

/*===================
 *  HAL settings
 *==================*/

/* 1: use a custom tick source.
 * It removes the need to manually update the tick with `lv_tick_inc`) */
#ifndef LV_TICK_CUSTOM
#define LV_TICK_CUSTOM     0
#endif
#if LV_TICK_CUSTOM == 1
#ifndef LV_TICK_CUSTOM_INCLUDE
#define LV_TICK_CUSTOM_INCLUDE  "something.h"       /*Header for the sys time function*/
#endif
#ifndef LV_TICK_CUSTOM_SYS_TIME_EXPR
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
#endif
#endif   /*LV_TICK_CUSTOM*/


/*================
 * Log settings
 *===============*/

/*1: Enable the log module*/
#ifndef LV_USE_LOG
#define LV_USE_LOG      1
#endif
#if LV_USE_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 */
#ifndef LV_LOG_LEVEL
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN
#endif

/* 1: Print the log with 'printf';
 * 0: user need to register a callback with `lv_log_register_print`*/
#ifndef LV_LOG_PRINTF
#  define LV_LOG_PRINTF   0
#endif
#endif  /*LV_USE_LOG*/

/*================
 *  THEME USAGE
 *================*/
#ifndef LV_THEME_LIVE_UPDATE
#define LV_THEME_LIVE_UPDATE    0   /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/
#endif

#ifndef LV_USE_THEME_TEMPL
#define LV_USE_THEME_TEMPL      1   /*Just for test*/
#endif
#ifndef LV_USE_THEME_DEFAULT
#define LV_USE_THEME_DEFAULT    1   /*Built mainly from the built-in styles. Consumes very few RAM*/
#endif
#ifndef LV_USE_THEME_ALIEN
#define LV_USE_THEME_ALIEN      1   /*Dark futuristic theme*/
#endif
#ifndef LV_USE_THEME_NIGHT
#define LV_USE_THEME_NIGHT      1   /*Dark elegant theme*/
#endif
#ifndef LV_USE_THEME_MONO
#define LV_USE_THEME_MONO       1   /*Mono color theme for monochrome displays*/
#endif
#ifndef LV_USE_THEME_MATERIAL
#define LV_USE_THEME_MATERIAL   1   /*Flat theme with bold colors and light shadows*/
#endif
#ifndef LV_USE_THEME_ZEN
#define LV_USE_THEME_ZEN        1   /*Peaceful, mainly light theme */
#endif
#ifndef LV_USE_THEME_NEMO
#define LV_USE_THEME_NEMO       1   /*Water-like theme based on the movie "Finding Nemo"*/
#endif

/*==================
 *    FONT USAGE
 *===================*/

/* The built-in fonts contains the ASCII range and some Symbols with  4 bit-per-pixel.
 * The symbols are available via `LV_SYMBOL_...` defines
 * More info about fonts: https://docs.littlevgl.com/#Fonts
 * To create a new font go to: https://littlevgl.com/ttf-font-to-c-array
 */

/* Robot fonts with bpp = 4
 * https://fonts.google.com/specimen/Roboto  */
#ifndef LV_FONT_ROBOTO_12
#define LV_FONT_ROBOTO_12    0
#endif
#ifndef LV_FONT_ROBOTO_16
#define LV_FONT_ROBOTO_16    1
#endif
#ifndef LV_FONT_ROBOTO_22
#define LV_FONT_ROBOTO_22    0
#endif
#ifndef LV_FONT_ROBOTO_28
#define LV_FONT_ROBOTO_28    0
#endif

/*Pixel perfect monospace font
 * http://pelulamu.net/unscii/ */
#ifndef LV_FONT_UNSCII_8
#define LV_FONT_UNSCII_8     0
#endif

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2)
 */
#ifndef LV_FONT_CUSTOM_DECLARE
#define LV_FONT_CUSTOM_DECLARE
#endif

/*Always set a default font from the built-in fonts*/
#ifndef LV_FONT_DEFAULT
#define LV_FONT_DEFAULT        &lv_font_roboto_16
#endif

/*Declare the type of the user data of fonts (can be e.g. `void *`, `int`, `struct`)*/

/*=================
 *  Text settings
 *=================*/

/* Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 * */
#ifndef LV_TXT_ENC
#define LV_TXT_ENC LV_TXT_ENC_UTF8
#endif

 /*Can break (wrap) texts on these chars*/
#ifndef LV_TXT_BREAK_CHARS
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"
#endif

/*===================
 *  LV_OBJ SETTINGS
 *==================*/

/*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/

/*1: enable `lv_obj_realaign()` based on `lv_obj_align()` parameters*/
#ifndef LV_USE_OBJ_REALIGN
#define LV_USE_OBJ_REALIGN          1
#endif

/* Enable to make the object clickable on a larger area.
 * LV_EXT_CLICK_AREA_OFF or 0: Disable this feature
 * LV_EXT_CLICK_AREA_TINY: The extra area can be adjusted horizontally and vertically (0..255 px)
 * LV_EXT_CLICK_AREA_FULL: The extra area can be adjusted in all 4 directions (-32k..+32k px)
 */
#ifndef LV_USE_EXT_CLICK_AREA
#define LV_USE_EXT_CLICK_AREA  LV_EXT_CLICK_AREA_OFF
#endif

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.littlevgl.com/#Object-types
 */

/*Arc (dependencies: -)*/
#ifndef LV_USE_ARC
#define LV_USE_ARC      1
#endif

/*Bar (dependencies: -)*/
#ifndef LV_USE_BAR
#define LV_USE_BAR      1
#endif

/*Button (dependencies: lv_cont*/
#ifndef LV_USE_BTN
#define LV_USE_BTN      1
#endif
#if LV_USE_BTN != 0
/*Enable button-state animations - draw a circle on click (dependencies: LV_USE_ANIMATION)*/
#ifndef LV_BTN_INK_EFFECT
#  define LV_BTN_INK_EFFECT   1
#endif
#endif

/*Button matrix (dependencies: -)*/
#ifndef LV_USE_BTNM
#define LV_USE_BTNM     1
#endif

/*Calendar (dependencies: -)*/
#ifndef LV_USE_CALENDAR
#define LV_USE_CALENDAR 1
#endif

/*Canvas (dependencies: lv_img)*/
#ifndef LV_USE_CANVAS
#define LV_USE_CANVAS   1
#endif

/*Check box (dependencies: lv_btn, lv_label)*/
#ifndef LV_USE_CB
#define LV_USE_CB       1
#endif

/*Chart (dependencies: -)*/
#ifndef LV_USE_CHART
#define LV_USE_CHART    1
#endif
#if LV_USE_CHART
#ifndef LV_CHART_AXIS_TICK_LABEL_MAX_LEN
#  define LV_CHART_AXIS_TICK_LABEL_MAX_LEN    20
#endif
#endif

/*Container (dependencies: -*/
#ifndef LV_USE_CONT
#define LV_USE_CONT     1
#endif

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#ifndef LV_USE_DDLIST
#define LV_USE_DDLIST    1
#endif
#if LV_USE_DDLIST != 0
/*Open and close default animation time [ms] (0: no animation)*/
#ifndef LV_DDLIST_DEF_ANIM_TIME
#  define LV_DDLIST_DEF_ANIM_TIME     200
#endif
#endif

/*Gauge (dependencies:lv_bar, lv_lmeter)*/
#ifndef LV_USE_GAUGE
#define LV_USE_GAUGE    1
#endif

/*Image (dependencies: lv_label*/
#ifndef LV_USE_IMG
#define LV_USE_IMG      1
#endif

/*Image Button (dependencies: lv_btn*/
#ifndef LV_USE_IMGBTN
#define LV_USE_IMGBTN   1
#endif
#if LV_USE_IMGBTN
/*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#ifndef LV_IMGBTN_TILED
#  define LV_IMGBTN_TILED 0
#endif
#endif

/*Keyboard (dependencies: lv_btnm)*/
#ifndef LV_USE_KB
#define LV_USE_KB       1
#endif

/*Label (dependencies: -*/
#ifndef LV_USE_LABEL
#define LV_USE_LABEL    1
#endif
#if LV_USE_LABEL != 0
/*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
#ifndef LV_LABEL_DEF_SCROLL_SPEED
#  define LV_LABEL_DEF_SCROLL_SPEED       25
#endif

/* Waiting period at beginning/end of animation cycle */
#ifndef LV_LABEL_WAIT_CHAR_COUNT
#  define LV_LABEL_WAIT_CHAR_COUNT        3
#endif

/*Enable selecting text of the label */
#ifndef LV_LABEL_TEXT_SEL
#  define LV_LABEL_TEXT_SEL               0
#endif

/*Store extra some info in labels (12 bytes) to speed up drawing of very long texts*/
#ifndef LV_LABEL_LONG_TXT_HINT
#  define LV_LABEL_LONG_TXT_HINT          0
#endif
#endif

/*LED (dependencies: -)*/
#ifndef LV_USE_LED
#define LV_USE_LED      1
#endif

/*Line (dependencies: -*/
#ifndef LV_USE_LINE
#define LV_USE_LINE     1
#endif

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#ifndef LV_USE_LIST
#define LV_USE_LIST     1
#endif
#if LV_USE_LIST != 0
/*Default animation time of focusing to a list element [ms] (0: no animation)  */
#ifndef LV_LIST_DEF_ANIM_TIME
#  define LV_LIST_DEF_ANIM_TIME  100
#endif
#endif

/*Line meter (dependencies: *;)*/
#ifndef LV_USE_LMETER
#define LV_USE_LMETER   1
#endif

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#ifndef LV_USE_MBOX
#define LV_USE_MBOX     1
#endif

/*Page (dependencies: lv_cont)*/
#ifndef LV_USE_PAGE
#define LV_USE_PAGE     1
#endif
#if LV_USE_PAGE != 0
/*Focus default animation time [ms] (0: no animation)*/
#ifndef LV_PAGE_DEF_ANIM_TIME
#  define LV_PAGE_DEF_ANIM_TIME     400
#endif
#endif

/*Preload (dependencies: lv_arc, lv_anim)*/
#ifndef LV_USE_PRELOAD
#define LV_USE_PRELOAD      1
#endif
#if LV_USE_PRELOAD != 0
#ifndef LV_PRELOAD_DEF_ARC_LENGTH
#  define LV_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
#endif
#ifndef LV_PRELOAD_DEF_SPIN_TIME
#  define LV_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
#endif
#ifndef LV_PRELOAD_DEF_ANIM
#  define LV_PRELOAD_DEF_ANIM         LV_PRELOAD_TYPE_SPINNING_ARC
#endif
#endif

/*Roller (dependencies: lv_ddlist)*/
#ifndef LV_USE_ROLLER
#define LV_USE_ROLLER    1
#endif
#if LV_USE_ROLLER != 0
/*Focus animation time [ms] (0: no animation)*/
#ifndef LV_ROLLER_DEF_ANIM_TIME
#  define LV_ROLLER_DEF_ANIM_TIME     200
#endif

/*Number of extra "pages" when the roller is infinite*/
#ifndef LV_ROLLER_INF_PAGES
#  define LV_ROLLER_INF_PAGES         7
#endif
#endif

/*Slider (dependencies: lv_bar)*/
#ifndef LV_USE_SLIDER
#define LV_USE_SLIDER    1
#endif

/*Spinbox (dependencies: lv_ta)*/
#ifndef LV_USE_SPINBOX
#define LV_USE_SPINBOX       1
#endif

/*Switch (dependencies: lv_slider)*/
#ifndef LV_USE_SW
#define LV_USE_SW       1
#endif

/*Text area (dependencies: lv_label, lv_page)*/
#ifndef LV_USE_TA
#define LV_USE_TA       1
#endif
#if LV_USE_TA != 0
#ifndef LV_TA_DEF_CURSOR_BLINK_TIME
#  define LV_TA_DEF_CURSOR_BLINK_TIME 400     /*ms*/
#endif
#ifndef LV_TA_DEF_PWD_SHOW_TIME
#  define LV_TA_DEF_PWD_SHOW_TIME     1500    /*ms*/
#endif
#endif

/*Table (dependencies: lv_label)*/
#ifndef LV_USE_TABLE
#define LV_USE_TABLE    1
#endif
#if LV_USE_TABLE
#ifndef LV_TABLE_COL_MAX
#  define LV_TABLE_COL_MAX    12
#endif
#endif

/*Tab (dependencies: lv_page, lv_btnm)*/
#ifndef LV_USE_TABVIEW
#define LV_USE_TABVIEW      1
#endif
#  if LV_USE_TABVIEW != 0
/*Time of slide animation [ms] (0: no animation)*/
#ifndef LV_TABVIEW_DEF_ANIM_TIME
#  define LV_TABVIEW_DEF_ANIM_TIME    300
#endif
#endif

/*Tileview (dependencies: lv_page) */
#ifndef LV_USE_TILEVIEW
#define LV_USE_TILEVIEW     1
#endif
#if LV_USE_TILEVIEW
/*Time of slide animation [ms] (0: no animation)*/
#ifndef LV_TILEVIEW_DEF_ANIM_TIME
#  define LV_TILEVIEW_DEF_ANIM_TIME   300
#endif
#endif

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#ifndef LV_USE_WIN
#define LV_USE_WIN      1
#endif

/*==================
 * Non-user section
 *==================*/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
#ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
#endif
#endif


#endif  /*LV_CONF_CHECKER_H*/

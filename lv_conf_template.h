/**
 * @file lv_conf.h
 *
 */

/*
 * COPY THIS FILE AS `lv_conf.h` NEXT TO the `lvgl` FOLDER
 */

#if 0 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H
/* clang-format off */

/*====================
   Graphical settings
 *====================*/

/* Maximal horizontal and vertical resolution to support by the library.*/
#define LV_HOR_RES_MAX          (480)
#define LV_VER_RES_MAX          (320)

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB233
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
#define LV_DPI              100     /*[px]*/

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

/* Complier prefix for a big array declaration */
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
#define LV_INDEV_DEF_DRAG_THROW           20

/* Long press time in milliseconds.
 * Time to send `LV_EVENT_LONG_PRESSSED`) */
#define LV_INDEV_DEF_LONG_PRESS_TIME      400

/* Repeated trigger period in long press [ms]
 * Time between `LV_EVENT_LONG_PRESSED_REPEAT */
#define LV_INDEV_DEF_LONG_PRESS_REP_TIME  100

/*==================
 * Feature usage
 *==================*/

/*1: Enable the Animations */
#define LV_USE_ANIMATION        1

/* 1: Enable shadow drawing*/
#define LV_USE_SHADOW           1

/* 1: Enable object groups (for keyboard/encoder navigation) */
#define LV_USE_GROUP            1
#if LV_USE_GROUP
typedef void * lv_group_user_data_t;
#endif  /*LV_USE_GROUP*/

/* 1: Enable GPU interface*/
#define LV_USE_GPU              1

/* 1: Enable file system (might be required for images */
#define LV_USE_FILESYSTEM       1

/* 1: Enable indexed (palette) images */
#define LV_IMG_CF_INDEXED   1

/* 1: Enable alpha indexed images */
#define LV_IMG_CF_ALPHA     1

/*1: Add a `user_data` to drivers and objects*/
#define LV_USE_USER_DATA_SINGLE 1

/*1: Add separate `user_data` for every callback*/
#define LV_USE_USER_DATA_MULTI  0

/*=====================
 *  Compiler settings
 *====================*/
/* Define a custom attribute to `lv_tick_inc` function */
#define LV_ATTRIBUTE_TICK_INC

/* Define a custom attribute to `lv_task_handler` function */
#define LV_ATTRIBUTE_TASK_HANDLER

/* 1: Variable length array is supported*/
#define LV_COMPILER_VLA_SUPPORTED            1

/* 1: Initialization with non constant values are supported */
#define LV_COMPILER_NON_CONST_INIT_SUPPORTED 1

/*===================
 *  HAL settings
 *==================*/

/* 1: use a custom tick source.
 * It removes the need to manually update the tick with `lv_tick_inc`) */
#define LV_TICK_CUSTOM     0
#if LV_TICK_CUSTOM == 1
#define LV_TICK_CUSTOM_INCLUDE  "something.h"       /*Header for the sys time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
#endif   /*LV_TICK_CUSTOM*/

typedef void * lv_disp_drv_user_data_t;             /*Type of user data in the display driver*/
typedef void * lv_indev_drv_user_data_t;            /*Type of user data in the display driver*/

/*================
 * Log settings
 *===============*/

/*1: Enable the log module*/
#define LV_USE_LOG      1
#if LV_USE_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 */
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN

/* 1: Print the log with 'printf';
 * 0: user need to register a callback with `lv_log_register_print`*/
#  define LV_LOG_PRINTF   0
#endif  /*LV_USE_LOG*/

/*================
 *  THEME USAGE
 *================*/
#define LV_THEME_LIVE_UPDATE    0   /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/

#define LV_USE_THEME_TEMPL      1   /*Just for test*/
#define LV_USE_THEME_DEFAULT    1   /*Built mainly from the built-in styles. Consumes very few RAM*/
#define LV_USE_THEME_ALIEN      1   /*Dark futuristic theme*/
#define LV_USE_THEME_NIGHT      1   /*Dark elegant theme*/
#define LV_USE_THEME_MONO       1   /*Mono color theme for monochrome displays*/
#define LV_USE_THEME_MATERIAL   1   /*Flat theme with bold colors and light shadows*/
#define LV_USE_THEME_ZEN        1   /*Peaceful, mainly light theme */
#define LV_USE_THEME_NEMO       1   /*Water-like theme based on the movie "Finding Nemo"*/

/*==================
 *    FONT USAGE
 *===================*/

/* More info about fonts: https://docs.littlevgl.com/#Fonts
 * To enable a built-in font use 1,2,4 or 8 values
 * which will determine the bit-per-pixel. Higher value means smoother fonts */
#define LV_USE_FONT_DEJAVU_10              4
#define LV_USE_FONT_DEJAVU_10_LATIN_SUP    4
#define LV_USE_FONT_DEJAVU_10_CYRILLIC     4
#define LV_USE_FONT_SYMBOL_10              4

#define LV_USE_FONT_DEJAVU_20              4
#define LV_USE_FONT_DEJAVU_20_LATIN_SUP    4
#define LV_USE_FONT_DEJAVU_20_CYRILLIC     4
#define LV_USE_FONT_SYMBOL_20              4

#define LV_USE_FONT_DEJAVU_30              4
#define LV_USE_FONT_DEJAVU_30_LATIN_SUP    4
#define LV_USE_FONT_DEJAVU_30_CYRILLIC     4
#define LV_USE_FONT_SYMBOL_30              4

#define LV_USE_FONT_DEJAVU_40              4
#define LV_USE_FONT_DEJAVU_40_LATIN_SUP    4
#define LV_USE_FONT_DEJAVU_40_CYRILLIC     4
#define LV_USE_FONT_SYMBOL_40              4

#define LV_USE_FONT_MONOSPACE_8            1

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2) \
 */
#define LV_FONT_CUSTOM_DECLARE

#define LV_FONT_DEFAULT        &lv_font_dejavu_20     /*Always set a default font from the built-in fonts*/

/*=================
 *  Text settings
 *=================*/

 /*Can break (wrap) texts on these chars*/
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"

/* If a character is at least this long, will break wherever "prettiest" */
#define LV_TXT_LINE_BREAK_LONG_LEN          12

/* Minimum number of characters of a word to put on a line before a break */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3

/* Minimum number of characters of a word to put on a line after a break */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 1

/*===================
 *  LV_OBJ SETTINGS
 *==================*/

/*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/
typedef void * lv_obj_user_data_t;

/*1: enable `lv_obj_realaign()` based on `lv_obj_align()` parameters*/
#define LV_OBJ_REALIGN          1

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.littlevgl.com/#Object-types
 */

/*Arc (dependencies: -)*/
#define LV_USE_ARC      1

/*Bar (dependencies: -)*/
#define LV_USE_BAR      1

/*Button (dependencies: lv_cont*/
#define LV_USE_BTN      1
#if LV_USE_BTN != 0
/*Enable button-state animations - draw a circle on click (dependencies: LV_USE_ANIMATION)*/
#  define LV_BTN_INK_EFFECT   1
#endif

/*Button matrix (dependencies: -)*/
#define LV_USE_BTNM     1

/*Calendar (dependencies: -)*/
#define LV_USE_CALENDAR 1

/*Canvas (dependencies: lv_img)*/
#define LV_USE_CANVAS   1

/*Check box (dependencies: lv_btn, lv_label)*/
#define LV_USE_CB       1

/*Chart (dependencies: -)*/
#define LV_USE_CHART    1

/*Container (dependencies: -*/
#define LV_USE_CONT     1

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#define LV_USE_DDLIST    1
#if LV_USE_DDLIST != 0
/*Open and close default animation time [ms] (0: no animation)*/
#  define LV_DDLIST_DEF_ANIM_TIME     200
#endif

/*Gauge (dependencies:lv_bar, lv_lmeter)*/
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
#define LV_USE_KB       1

/*Label (dependencies: -*/
#define LV_USE_LABEL    1
#if LV_USE_LABEL != 0
/*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_ROLL/ROLL_CIRC' mode*/
#  define LV_LABEL_DEF_SCROLL_SPEED       25
#endif

/*LED (dependencies: -)*/
#define LV_USE_LED      1

/*Line (dependencies: -*/
#define LV_USE_LINE     1

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#define LV_USE_LIST     1
#if LV_USE_LIST != 0
/*Default animation time of focusing to a list element [ms] (0: no animation)  */
#  define LV_LIST_DEF_ANIM_TIME  100
#endif

/*Line meter (dependencies: *;)*/
#define LV_USE_LMETER   1

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#define LV_USE_MBOX     1

/*Page (dependencies: lv_cont)*/
#define LV_USE_PAGE     1

/*Preload (dependencies: lv_arc)*/
#define LV_USE_PRELOAD      1
#if LV_USE_PRELOAD != 0
#  define LV_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
#  define LV_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
#  define LV_PRELOAD_DEF_ANIM         LV_PRELOAD_TYPE_SPINNING_ARC
#endif

/*Roller (dependencies: lv_ddlist)*/
#define LV_USE_ROLLER    1
#if LV_USE_ROLLER != 0
/*Focus animation time [ms] (0: no animation)*/
#  define LV_ROLLER_DEF_ANIM_TIME     200
#endif

/*Slider (dependencies: lv_bar)*/
#define LV_USE_SLIDER    1

/*Spinbox (dependencies: lv_ta)*/
#define LV_USE_SPINBOX       1

/*Switch (dependencies: lv_slider)*/
#define LV_USE_SW       1

/*Text area (dependencies: lv_label, lv_page)*/
#define LV_USE_TA       1
#if LV_USE_TA != 0
#  define LV_TA_CURSOR_BLINK_TIME 400     /*ms*/
#  define LV_TA_PWD_SHOW_TIME     1500    /*ms*/
#endif

/*Table (dependencies: lv_label)*/
#define LV_USE_TABLE    1
#if LV_USE_TABLE
#  define LV_TABLE_COL_MAX    12
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

/*Be sure every define has a default value*/
#include "lvgl/src/lv_conf_checker.h"

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"*/

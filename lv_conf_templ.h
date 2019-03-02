/**
 * @file lv_conf.h
 *
 */

/*
 * COPY THIS FILE AS lv_conf.h
 */

#if 0 /*Set it to "1" to enable content*/


#ifndef LV_CONF_H
#define LV_CONF_H

/*===================
   Graphical settings
 *===================*/

/* Horizontal and vertical resolution of the library.*/
#define LV_HOR_RES_MAX          (480)
#define LV_VER_RES_MAX          (320)

/*Color settings*/
#define LV_COLOR_DEPTH     16                     /*Color depth: 1/8/16/32*/
#define LV_COLOR_16_SWAP   0                      /*Swap the 2 bytes of RGB565 color. Useful if the display has a 8 bit interface (e.g. SPI)*/
#define LV_COLOR_SCREEN_TRANSP        0           /*1: Enable screen transparency. Useful for OSD or other overlapping GUIs. Requires ARGB8888 colors*/
#define LV_COLOR_TRANSP    LV_COLOR_LIME          /*Images pixels with this color will not be drawn (with chroma keying)*/

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#define LV_ANTIALIAS        1       /*1: Enable anti-aliasing*/


/*Screen refresh period in milliseconds. LittlevGL will redraw the screen with this period*/
#define LV_REFR_PERIOD      30      /*[ms]*/

/* Dot Per Inch: used to initialize default sizes. E.g. a button with width = LV_DPI / 2 -> half inch wide
 * (Not so important, you can adjust it to modify default sizes and spaces)*/
#define LV_DPI              100     /*[px]*/

/*===================
   Dynamic memory
 *===================*/

/* Memory size which will be used by the library
 * to store the graphical objects and other data */
#define LV_MEM_CUSTOM      0                /*1: use custom malloc/free, 0: use the built-in lv_mem_alloc/lv_mem_free*/
#if LV_MEM_CUSTOM == 0
#  define LV_MEM_SIZE    (32U * 1024U)        /*Size memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#  define LV_MEM_ATTR                         /*Complier prefix for big array declaration*/
#  define LV_MEM_ADR          0               /*Set an address for memory pool instead of allocation it as an array. Can be in external SRAM too.*/
#  define LV_MEM_AUTO_DEFRAG  1               /*Automatically defrag on free*/
#else       /*LV_MEM_CUSTOM*/
#  define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#  define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#  define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif     /*LV_MEM_CUSTOM*/

/* Garbage Collector settings
 * Used if lvgl is binded to higher language and the memory is managed by that language */
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
#  define LV_MEM_CUSTOM_REALLOC   your_realloc           /*Wrapper to realloc*/
#  define LV_MEM_CUSTOM_GET_SIZE  your_mem_get_size      /*Wrapper to lv_mem_get_size*/
#  define LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#endif /* LV_ENABLE_GC */

/*=================
   Misc. setting
 *=================*/

/*Input device settings*/
#define LV_INDEV_READ_PERIOD            30                     /*Input device read period in milliseconds*/
#define LV_INDEV_POINT_MARKER           0                      /*Mark the pressed points  (required: USE_LV_REAL_DRAW = 1)*/
#define LV_INDEV_DRAG_LIMIT             10                     /*Drag threshold in pixels */
#define LV_INDEV_DRAG_THROW             20                     /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_INDEV_LONG_PRESS_TIME        400                    /*Long press time in milliseconds*/
#define LV_INDEV_LONG_PRESS_REP_TIME    100                    /*Repeated trigger period in long press [ms] */

/*Text settings*/
#define LV_TXT_UTF8             1                /*Enable UTF-8 coded Unicode character usage */
#define LV_TXT_BREAK_CHARS     " ,.;:-_"         /*Can break texts on these chars*/
#define LV_TXT_LINE_BREAK_LONG_LEN 12 /* If a character is at least this long, will break wherever "prettiest" */
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3 /* Minimum number of characters of a word to put on a line before a break */
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 1 /* Minimum number of characters of a word to put on a line after a break */

/*Feature usage*/
#define USE_LV_ANIMATION        1               /*1: Enable all animations*/
#define USE_LV_SHADOW           1               /*1: Enable shadows*/
#define USE_LV_GROUP            1               /*1: Enable object groups (for keyboards)*/
#define USE_LV_GPU              1               /*1: Enable GPU interface*/
#define USE_LV_FILESYSTEM       1               /*1: Enable file system (might be required for images*/
#define USE_LV_I18N             1               /*1: Enable InternationalizatioN (multi-language) support*/
#define USE_LV_USER_DATA_SINGLE 1               /*1: Add a `user_data` to drivers and objects*/
#define USE_LV_USER_DATA_MULTI  0               /*1: Add separate `user_data` for every callback*/

/*Compiler settings*/
#define LV_ATTRIBUTE_TICK_INC                   /* Define a custom attribute to `lv_tick_inc` function */
#define LV_ATTRIBUTE_TASK_HANDLER               /* Define a custom attribute to `lv_task_handler` function */
#define LV_COMPILER_VLA_SUPPORTED            1  /* 1: Variable length array is supported*/
#define LV_COMPILER_NON_CONST_INIT_SUPPORTED 1  /* 1: Initialization with non constant values are supported */

/*HAL settings*/
#define LV_TICK_CUSTOM     0                        /*1: use a custom tick source (removing the need to manually update the tick with `lv_tick_inc`) */
#if LV_TICK_CUSTOM == 1
#define LV_TICK_CUSTOM_INCLUDE  "something.h"       /*Header for the sys time function*/
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
#endif   /*LV_TICK_CUSTOM*/

typedef void * lv_disp_drv_user_data_t;                 /*Type of user data in the display driver*/
typedef void * lv_indev_drv_user_data_t;                /*Type of user data in the display driver*/

/*Log settings*/
#define USE_LV_LOG      1   /*Enable/disable the log module*/
#if USE_LV_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 */
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN

/* 1: Print the log with 'printf'; 0: user need to register a callback*/
#  define LV_LOG_PRINTF   0
#endif  /*USE_LV_LOG*/

/*================
 *  THEME USAGE
 *================*/
#define LV_THEME_LIVE_UPDATE    0       /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/

#define USE_LV_THEME_TEMPL      0       /*Just for test*/
#define USE_LV_THEME_DEFAULT    1       /*Built mainly from the built-in styles. Consumes very few RAM*/
#define USE_LV_THEME_ALIEN      1       /*Dark futuristic theme*/
#define USE_LV_THEME_NIGHT      1       /*Dark elegant theme*/
#define USE_LV_THEME_MONO       1       /*Mono color theme for monochrome displays*/
#define USE_LV_THEME_MATERIAL   1       /*Flat theme with bold colors and light shadows*/
#define USE_LV_THEME_ZEN        1       /*Peaceful, mainly light theme */
#define USE_LV_THEME_NEMO       1       /*Water-like theme based on the movie "Finding Nemo"*/

/*==================
 *    FONT USAGE
 *===================*/

/* More info about fonts: https://docs.littlevgl.com/#Fonts
 * To enable a built-in font use 1,2,4 or 8 values
 * which will determine the bit-per-pixel. Higher value means smoother fonts */
#define USE_LV_FONT_DEJAVU_10              0
#define USE_LV_FONT_DEJAVU_10_LATIN_SUP    0
#define USE_LV_FONT_DEJAVU_10_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_10              0

#define USE_LV_FONT_DEJAVU_20              4
#define USE_LV_FONT_DEJAVU_20_LATIN_SUP    4
#define USE_LV_FONT_DEJAVU_20_CYRILLIC     4
#define USE_LV_FONT_SYMBOL_20              4

#define USE_LV_FONT_DEJAVU_30              0
#define USE_LV_FONT_DEJAVU_30_LATIN_SUP    0
#define USE_LV_FONT_DEJAVU_30_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_30              0

#define USE_LV_FONT_DEJAVU_40              0
#define USE_LV_FONT_DEJAVU_40_LATIN_SUP    0
#define USE_LV_FONT_DEJAVU_40_CYRILLIC     0
#define USE_LV_FONT_SYMBOL_40              0

#define USE_LV_FONT_MONOSPACE_8            1

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2) \
 */
#define LV_FONT_CUSTOM_DECLARE


#define LV_FONT_DEFAULT        &lv_font_dejavu_20     /*Always set a default font from the built-in fonts*/

/*===================
 *  LV_OBJ SETTINGS
 *==================*/
typedef void * lv_obj_user_data_t;          /*Declare the type of the user data of object (can be e.g. `void *`, `int`, `struct`)*/
#define LV_OBJ_REALIGN          1           /*Enable `lv_obj_realaign()` based on `lv_obj_align()` parameters*/

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://docs.littlevgl.com/#Object-types
 */

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#  define LV_LABEL_SCROLL_SPEED       25     /*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_SCROLL/ROLL' mode*/
#endif

/*Image (dependencies: lv_label*/
#define USE_LV_IMG      1
#if USE_LV_IMG != 0
#  define LV_IMG_CF_INDEXED   1       /*Enable indexed (palette) images*/
#  define LV_IMG_CF_ALPHA     1       /*Enable alpha indexed images*/
#endif

/*Line (dependencies: -*/
#define USE_LV_LINE     1

/*Arc (dependencies: -)*/
#define USE_LV_ARC      1

/*******************
 * Container objects
 *******************/

/*Container (dependencies: -*/
#define USE_LV_CONT     1

/*Page (dependencies: lv_cont)*/
#define USE_LV_PAGE     1

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#define USE_LV_WIN      1

/*Tab (dependencies: lv_page, lv_btnm)*/
#define USE_LV_TABVIEW      1
#  if USE_LV_TABVIEW != 0
#  define LV_TABVIEW_ANIM_TIME    300     /*Time of slide animation [ms] (0: no animation)*/
#endif

/*Tileview (dependencies: lv_page) */
#define USE_LV_TILEVIEW     1
#if USE_LV_TILEVIEW
#  define LV_TILEVIEW_ANIM_TIME   300     /*Time of slide animation [ms] (0: no animation)*/
#endif

/*************************
 * Data visualizer objects
 *************************/

/*Bar (dependencies: -)*/
#define USE_LV_BAR      1

/*Line meter (dependencies: *;)*/
#define USE_LV_LMETER   1

/*Gauge (dependencies:lv_bar, lv_lmeter)*/
#define USE_LV_GAUGE    1

/*Chart (dependencies: -)*/
#define USE_LV_CHART    1

/*Table (dependencies: lv_label)*/
#define USE_LV_TABLE    1
#if USE_LV_TABLE
#  define LV_TABLE_COL_MAX    12
#endif

/*LED (dependencies: -)*/
#define USE_LV_LED      1

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#define USE_LV_MBOX     1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#  define LV_TA_CURSOR_BLINK_TIME 400     /*ms*/
#  define LV_TA_PWD_SHOW_TIME     1500    /*ms*/
#endif

/*Spinbox (dependencies: lv_ta)*/
#define USE_LV_SPINBOX       1

/*Calendar (dependencies: -)*/
#define USE_LV_CALENDAR 1

/*Preload (dependencies: lv_arc)*/
#define USE_LV_PRELOAD      1
#if USE_LV_PRELOAD != 0
#  define LV_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
#  define LV_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
#  define LV_PRELOAD_DEF_ANIM         LV_PRELOAD_TYPE_SPINNING_ARC
#endif

/*Canvas (dependencies: lv_img)*/
#define USE_LV_CANVAS       1
/*************************
 * User input objects
 *************************/

/*Button (dependencies: lv_cont*/
#define USE_LV_BTN      1
#if USE_LV_BTN != 0
#  define LV_BTN_INK_EFFECT   1       /*Enable button-state animations - draw a circle on click (dependencies: USE_LV_ANIMATION)*/
#endif

/*Image Button (dependencies: lv_btn*/
#define USE_LV_IMGBTN   1
#if USE_LV_IMGBTN
#  define LV_IMGBTN_TILED 0           /*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#endif

/*Button matrix (dependencies: -)*/
#define USE_LV_BTNM     1

/*Keyboard (dependencies: lv_btnm)*/
#define USE_LV_KB       1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#define USE_LV_LIST     1
#if USE_LV_LIST != 0
#  define LV_LIST_FOCUS_TIME  100 /*Default animation time of focusing to a list element [ms] (0: no animation)  */
#endif

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#define USE_LV_DDLIST    1
#if USE_LV_DDLIST != 0
#  define LV_DDLIST_ANIM_TIME     200     /*Open and close default animation time [ms] (0: no animation)*/
#endif

/*Roller (dependencies: lv_ddlist)*/
#define USE_LV_ROLLER    1
#if USE_LV_ROLLER != 0
#  define LV_ROLLER_ANIM_TIME     200     /*Focus animation time [ms] (0: no animation)*/
#endif

/*Slider (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1

/*Switch (dependencies: lv_slider)*/
#define USE_LV_SW       1

/*************************
 * Non-user section
 *************************/
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)    /* Disable warnings for Visual Studio*/
#  define _CRT_SECURE_NO_WARNINGS
#endif

/*--END OF LV_CONF_H--*/

/*Be sure every define has a default value*/
#include "lvgl/lv_conf_checker.h"

#endif /*LV_CONF_H*/


#endif /*End of "Content enable"*/

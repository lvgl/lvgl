/**
 * @file lv_conf_checker.h
 * Make sure all the defines of lv_conf.h have a default value
**/

#ifndef LV_CONF_CHECKER_H
#define  LV_CONF_CHECKER_H

/*===================
   Dynamic memory
 *===================*/

/* Memory size which will be used by the library
 * to store the graphical objects and other data */
#ifndef LV_MEM_CUSTOM
#define LV_MEM_CUSTOM       0               /*1: use custom malloc/free, 0: use the built-in lv_mem_alloc/lv_mem_free*/
#endif
#if LV_MEM_CUSTOM == 0
#ifndef LV_MEM_SIZE
#define LV_MEM_SIZE    (32U * 1024U)        /*Size memory used by `lv_mem_alloc` in bytes (>= 2kB)*/
#endif
#ifndef LV_MEM_ATTR
#define LV_MEM_ATTR                         /*Complier prefix for big array declaration*/
#endif
#ifndef LV_MEM_AUTO_DEFRAG
#define LV_MEM_AUTO_DEFRAG  1               /*Automatically defrag on free*/
#endif
#else       /*LV_MEM_CUSTOM*/
#ifndef LV_MEM_CUSTOM_INCLUDE
#define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#endif
#ifndef LV_MEM_CUSTOM_ALLOC
#define LV_MEM_CUSTOM_ALLOC   malloc       /*Wrapper to malloc*/
#endif
#ifndef LV_MEM_CUSTOM_FREE
#define LV_MEM_CUSTOM_FREE    free         /*Wrapper to free*/
#endif
#endif     /*LV_MEM_CUSTOM*/

/*===================
   Graphical settings
 *===================*/

/* Horizontal and vertical resolution of the library.*/
#ifndef LV_HOR_RES
#define LV_HOR_RES          (320)
#endif
#ifndef LV_VER_RES
#define LV_VER_RES          (240)
#endif
#ifndef LV_DPI
#define LV_DPI              100
#endif

/* Size of VDB (Virtual Display Buffer: the internal graphics buffer).
 * Required for buffered drawing, opacity and anti-aliasing
 * VDB makes the double buffering, you don't need to deal with it!
 * Typical size: ~1/10 screen */
#ifndef LV_VDB_SIZE
#define LV_VDB_SIZE         (30 * LV_HOR_RES)  /*Size of VDB in pixel count (1/10 screen size is good for first)*/
#endif
#ifndef LV_VDB_PX_BPP
#define LV_VDB_PX_BPP       LV_COLOR_SIZE      /*Bit-per-pixel of VDB. Useful for monochrome or non-standard color format displays. (Special formats are handled with `disp_drv->vdb_wr`)*/
#endif
#ifndef LV_VDB_ADR
#define LV_VDB_ADR          0                  /*Place VDB to a specific address (e.g. in external RAM) (0: allocate automatically into RAM; LV_VDB_ADR_INV: to replace it later with `lv_vdb_set_adr()`)*/
#endif

/* Use two Virtual Display buffers (VDB) parallelize rendering and flushing (optional)
 * The flushing should use DMA to write the frame buffer in the background*/
#ifndef LV_VDB_DOUBLE
#define LV_VDB_DOUBLE       0       /*1: Enable the use of 2 VDBs*/
#endif
#ifndef LV_VDB2_ADR
#define LV_VDB2_ADR         0       /*Place VDB2 to a specific address (e.g. in external RAM) (0: allocate automatically into RAM; LV_VDB_ADR_INV: to replace it later with `lv_vdb_set_adr()`)*/
#endif

/* Enable anti-aliasing (lines, and radiuses will be smoothed) */
#ifndef LV_ANTIALIAS
#define LV_ANTIALIAS        1       /*1: Enable anti-aliasing*/
#endif

/*Screen refresh settings*/
#ifndef LV_REFR_PERIOD
#define LV_REFR_PERIOD      30    /*Screen refresh period in milliseconds*/
#endif
#ifndef LV_INV_FIFO_SIZE
#define LV_INV_FIFO_SIZE    32    /*The average count of objects on a screen */
#endif

/*=================
   Misc. setting
 *=================*/

/*Input device settings*/
#ifndef LV_INDEV_READ_PERIOD
#define LV_INDEV_READ_PERIOD            50                     /*Input device read period in milliseconds*/
#endif
#ifndef LV_INDEV_POINT_MARKER
#define LV_INDEV_POINT_MARKER           0                      /*Mark the pressed points  (required: USE_LV_REAL_DRAW = 1)*/
#endif
#ifndef LV_INDEV_DRAG_LIMIT
#define LV_INDEV_DRAG_LIMIT             10                     /*Drag threshold in pixels */
#endif
#ifndef LV_INDEV_DRAG_THROW
#define LV_INDEV_DRAG_THROW             20                     /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#endif
#ifndef LV_INDEV_LONG_PRESS_TIME
#define LV_INDEV_LONG_PRESS_TIME        400                    /*Long press time in milliseconds*/
#endif
#ifndef LV_INDEV_LONG_PRESS_REP_TIME
#define LV_INDEV_LONG_PRESS_REP_TIME    100                    /*Repeated trigger period in long press [ms] */
#endif

/*Color settings*/
#ifndef LV_COLOR_DEPTH
#define LV_COLOR_DEPTH     16                     /*Color depth: 1/8/16/32*/
#endif
#ifndef LV_COLOR_16_SWAP
#define LV_COLOR_16_SWAP   0                      /*Swap the 2 bytes of RGB565 color. Useful if the display has a 8 bit interface (e.g. SPI)*/
#endif
#ifndef LV_COLOR_SCREEN_TRANSP
#define LV_COLOR_SCREEN_TRANSP          0         /*1: Enable screen transparency. Useful for OSD or other overlapping GUIs. Requires ARGB8888 colors*/
#endif
#ifndef LV_COLOR_TRANSP
#define LV_COLOR_TRANSP    LV_COLOR_LIME          /*Images pixels with this color will not be drawn (with chroma keying)*/
#endif

/*Text settings*/
#ifndef LV_TXT_UTF8
#define LV_TXT_UTF8             1                /*Enable UTF-8 coded Unicode character usage */
#endif
#ifndef LV_TXT_BREAK_CHARS
#define LV_TXT_BREAK_CHARS     " ,.;:-_"         /*Can break texts on these chars*/
#endif

/*Graphics feature usage*/
#ifndef USE_LV_ANIMATION
#define USE_LV_ANIMATION        1               /*1: Enable all animations*/
#endif
#ifndef USE_LV_SHADOW
#define USE_LV_SHADOW           1               /*1: Enable shadows*/
#endif
#ifndef USE_LV_GROUP
#define USE_LV_GROUP            1               /*1: Enable object groups (for keyboards)*/
#endif
#ifndef USE_LV_GPU
#define USE_LV_GPU              1               /*1: Enable GPU interface*/
#endif
#ifndef USE_LV_REAL_DRAW
#define USE_LV_REAL_DRAW        1               /*1: Enable function which draw directly to the frame buffer instead of VDB (required if LV_VDB_SIZE = 0)*/
#endif
#ifndef USE_LV_FILESYSTEM
#define USE_LV_FILESYSTEM       1               /*1: Enable file system (required by images*/
#endif

/*Compiler settings*/
#ifndef LV_ATTRIBUTE_TICK_INC
#define LV_ATTRIBUTE_TICK_INC                   /* Define a custom attribute to `lv_tick_inc` function */
#endif
#ifndef LV_ATTRIBUTE_TASK_HANDLER
#define LV_ATTRIBUTE_TASK_HANDLER               /* Define a custom attribute to `lv_task_handler` function */
#endif
#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY                /* Define a custom attribute to `lv_flush_ready` function */
#endif
#ifndef LV_COMPILER_VLA_SUPPORTED
#define LV_COMPILER_VLA_SUPPORTED            1  /* 1: Variable length array is supported*/
#endif
#ifndef LV_COMPILER_NON_CONST_INIT_SUPPORTED
#define LV_COMPILER_NON_CONST_INIT_SUPPORTED 1  /* 1: Initialization with non constant values are supported */
#endif

/*HAL settings*/
#ifndef LV_TICK_CUSTOM
#define LV_TICK_CUSTOM     0                        /*1: use a custom tick source (removing the need to manually update the tick with `lv_tick_inc`) */
#endif
#if LV_TICK_CUSTOM == 1
#ifndef LV_TICK_CUSTOM_INCLUDE
#define LV_TICK_CUSTOM_INCLUDE  "Arduino.h"         /*Header for the sys time function*/
#endif
#ifndef LV_TICK_CUSTOM_SYS_TIME_EXPR
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())     /*Expression evaluating to current systime in ms*/
#endif
#endif     /*LV_TICK_CUSTOM*/

/*Log settings*/
#ifndef USE_LV_LOG
#define USE_LV_LOG      1   /*Enable/disable the log module*/
#endif
#if USE_LV_LOG
/* How important log should be added:
 * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 * LV_LOG_LEVEL_INFO        Log important events
 * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't caused problem
 * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 */
#ifndef LV_LOG_LEVEL
#define LV_LOG_LEVEL    LV_LOG_LEVEL_INFO
#endif

#ifndef LV_LOG_PRINTF
#define LV_LOG_PRINTF   0   /* 1: Print the log with 'printf'; 0: user need to register a callback*/
#endif
#endif  /*USE_LV_LOG*/

/*================
 *  THEME USAGE
 *================*/
#ifndef LV_THEME_LIVE_UPDATE
#define LV_THEME_LIVE_UPDATE    0       /*1: Allow theme switching at run time. Uses 8..10 kB of RAM*/
#endif

#ifndef USE_LV_THEME_TEMPL
#define USE_LV_THEME_TEMPL      0       /*Just for test*/
#endif
#ifndef USE_LV_THEME_DEFAULT
#define USE_LV_THEME_DEFAULT    0       /*Built mainly from the built-in styles. Consumes very few RAM*/
#endif
#ifndef USE_LV_THEME_ALIEN
#define USE_LV_THEME_ALIEN      0       /*Dark futuristic theme*/
#endif
#ifndef USE_LV_THEME_NIGHT
#define USE_LV_THEME_NIGHT      0       /*Dark elegant theme*/
#endif
#ifndef USE_LV_THEME_MONO
#define USE_LV_THEME_MONO       0       /*Mono color theme for monochrome displays*/
#endif
#ifndef USE_LV_THEME_MATERIAL
#define USE_LV_THEME_MATERIAL   0       /*Flat theme with bold colors and light shadows*/
#endif
#ifndef USE_LV_THEME_ZEN
#define USE_LV_THEME_ZEN        0       /*Peaceful, mainly light theme */
#endif
#ifndef USE_LV_THEME_NEMO
#define USE_LV_THEME_NEMO       0       /*Water-like theme based on the movie "Finding Nemo"*/
#endif

/*==================
 *    FONT USAGE
 *===================*/

/* More info about fonts: https://littlevgl.com/basics#fonts
 * To enable a built-in font use 1,2,4 or 8 values
 * which will determine the bit-per-pixel */
#ifndef USE_LV_FONT_DEJAVU_10
#define USE_LV_FONT_DEJAVU_10              0
#endif
#ifndef USE_LV_FONT_DEJAVU_10_LATIN_SUP
#define USE_LV_FONT_DEJAVU_10_LATIN_SUP    0
#endif
#ifndef USE_LV_FONT_DEJAVU_10_CYRILLIC
#define USE_LV_FONT_DEJAVU_10_CYRILLIC     0
#endif
#ifndef USE_LV_FONT_SYMBOL_10
#define USE_LV_FONT_SYMBOL_10              0
#endif

#ifndef USE_LV_FONT_DEJAVU_20
#define USE_LV_FONT_DEJAVU_20              4
#endif
#ifndef USE_LV_FONT_DEJAVU_20_LATIN_SUP
#define USE_LV_FONT_DEJAVU_20_LATIN_SUP    0
#endif
#ifndef USE_LV_FONT_DEJAVU_20_CYRILLIC
#define USE_LV_FONT_DEJAVU_20_CYRILLIC     0
#endif
#ifndef USE_LV_FONT_SYMBOL_20
#define USE_LV_FONT_SYMBOL_20              4
#endif

#ifndef USE_LV_FONT_DEJAVU_30
#define USE_LV_FONT_DEJAVU_30              0
#endif
#ifndef USE_LV_FONT_DEJAVU_30_LATIN_SUP
#define USE_LV_FONT_DEJAVU_30_LATIN_SUP    0
#endif
#ifndef USE_LV_FONT_DEJAVU_30_CYRILLIC
#define USE_LV_FONT_DEJAVU_30_CYRILLIC     0
#endif
#ifndef USE_LV_FONT_SYMBOL_30
#define USE_LV_FONT_SYMBOL_30              0
#endif

#ifndef USE_LV_FONT_DEJAVU_40
#define USE_LV_FONT_DEJAVU_40              0
#endif
#ifndef USE_LV_FONT_DEJAVU_40_LATIN_SUP
#define USE_LV_FONT_DEJAVU_40_LATIN_SUP    0
#endif
#ifndef USE_LV_FONT_DEJAVU_40_CYRILLIC
#define USE_LV_FONT_DEJAVU_40_CYRILLIC     0
#endif
#ifndef USE_LV_FONT_SYMBOL_40
#define USE_LV_FONT_SYMBOL_40              0
#endif

#ifndef USE_LV_FONT_MONOSPACE_8
#define USE_LV_FONT_MONOSPACE_8            0
#endif

/* Optionally declare your custom fonts here.
 * You can use these fonts as default font too
 * and they will be available globally. E.g.
 * #define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(my_font_1) \
 *                                LV_FONT_DECLARE(my_font_2) \
 */
#ifndef LV_FONT_CUSTOM_DECLARE
#define LV_FONT_CUSTOM_DECLARE
#endif

#ifndef LV_FONT_DEFAULT
#define LV_FONT_DEFAULT        &lv_font_dejavu_20     /*Always set a default font from the built-in fonts*/
#endif

/*===================
 *  LV_OBJ SETTINGS
 *==================*/
#ifndef LV_OBJ_FREE_NUM_TYPE
#define LV_OBJ_FREE_NUM_TYPE    uint32_t    /*Type of free number attribute (comment out disable free number)*/
#endif
#ifndef LV_OBJ_FREE_PTR
#define LV_OBJ_FREE_PTR         1           /*Enable the free pointer attribute*/
#endif
#ifndef LV_OBJ_REALIGN
#define LV_OBJ_REALIGN          1           /*Enable `lv_obj_realaign()` based on `lv_obj_align()` parameters*/
#endif

/*==================
 *  LV OBJ X USAGE
 *================*/
/*
 * Documentation of the object types: https://littlevgl.com/object-types
 */

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#ifndef USE_LV_LABEL
#define USE_LV_LABEL    1
#endif
#if USE_LV_LABEL != 0
#ifndef LV_LABEL_SCROLL_SPEED
#define LV_LABEL_SCROLL_SPEED       25     /*Hor, or ver. scroll speed [px/sec] in 'LV_LABEL_LONG_SCROLL/ROLL' mode*/
#endif
#endif

/*Image (dependencies: lv_label*/
#ifndef USE_LV_IMG
#define USE_LV_IMG      1
#endif
#if USE_LV_IMG != 0
#ifndef LV_IMG_CF_INDEXED
#define LV_IMG_CF_INDEXED   1       /*Enable indexed (palette) images*/
#endif
#ifndef LV_IMG_CF_ALPHA
#define LV_IMG_CF_ALPHA     1       /*Enable alpha indexed images*/
#endif
#endif

/*Line (dependencies: -*/
#ifndef USE_LV_LINE
#define USE_LV_LINE     1
#endif

/*Arc (dependencies: -)*/
#ifndef USE_LV_ARC
#define USE_LV_ARC      1
#endif

/*******************
 * Container objects
 *******************/

/*Container (dependencies: -*/
#ifndef USE_LV_CONT
#define USE_LV_CONT     1
#endif

/*Page (dependencies: lv_cont)*/
#ifndef USE_LV_PAGE
#define USE_LV_PAGE     1
#endif

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#ifndef USE_LV_WIN
#define USE_LV_WIN      1
#endif

/*Tab (dependencies: lv_page, lv_btnm)*/
#ifndef USE_LV_TABVIEW
#define USE_LV_TABVIEW      1
#endif
#if USE_LV_TABVIEW != 0
#ifndef LV_TABVIEW_ANIM_TIME
#define LV_TABVIEW_ANIM_TIME    300     /*Time of slide animation [ms] (0: no animation)*/
#endif
#endif

/*Tileview (dependencies: lv_page) */
#ifndef USE_LV_TILEVIEW
#define USE_LV_TILEVIEW     1
#endif
#if USE_LV_TILEVIEW
#ifndef LV_TILEVIEW_ANIM_TIME
#define LV_TILEVIEW_ANIM_TIME   300     /*Time of slide animation [ms] (0: no animation)*/
#endif
#endif

/*************************
 * Data visualizer objects
 *************************/

/*Bar (dependencies: -)*/
#ifndef USE_LV_BAR
#define USE_LV_BAR      1
#endif

/*Line meter (dependencies: *;)*/
#ifndef USE_LV_LMETER
#define USE_LV_LMETER   1
#endif

/*Gauge (dependencies:bar, lmeter)*/
#ifndef USE_LV_GAUGE
#define USE_LV_GAUGE    1
#endif

/*Chart (dependencies: -)*/
#ifndef USE_LV_CHART
#define USE_LV_CHART    1
#endif

/*Table (dependencies: lv_label)*/
#ifndef USE_LV_TABLE
#define USE_LV_TABLE    1
#endif
#if USE_LV_TABLE
#ifndef LV_TABLE_COL_MAX
#define LV_TABLE_COL_MAX    12
#endif
#endif

/*LED (dependencies: -)*/
#ifndef USE_LV_LED
#define USE_LV_LED      1
#endif

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#ifndef USE_LV_MBOX
#define USE_LV_MBOX     1
#endif

/*Text area (dependencies: lv_label, lv_page)*/
#ifndef USE_LV_TA
#define USE_LV_TA       1
#endif
#if USE_LV_TA != 0
#ifndef LV_TA_CURSOR_BLINK_TIME
#define LV_TA_CURSOR_BLINK_TIME 400     /*ms*/
#endif
#ifndef LV_TA_PWD_SHOW_TIME
#define LV_TA_PWD_SHOW_TIME     1500    /*ms*/
#endif
#endif

/*Spinbox (dependencies: lv_ta)*/
#ifndef USE_LV_SPINBOX
#define USE_LV_SPINBOX       1
#endif

/*Calendar (dependencies: -)*/
#ifndef USE_LV_CALENDAR
#define USE_LV_CALENDAR 1
#endif

/*Preload (dependencies: arc)*/
#ifndef USE_LV_PRELOAD
#define USE_LV_PRELOAD      1
#endif
#if USE_LV_PRELOAD != 0
#ifndef LV_PRELOAD_DEF_ARC_LENGTH
#define LV_PRELOAD_DEF_ARC_LENGTH   60      /*[deg]*/
#endif
#ifndef LV_PRELOAD_DEF_SPIN_TIME
#define LV_PRELOAD_DEF_SPIN_TIME    1000    /*[ms]*/
#endif
#ifndef LV_PRELOAD_DEF_ANIM
#define LV_PRELOAD_DEF_ANIM         LV_PRELOAD_TYPE_SPINNING_ARC
#endif
#endif

/*************************
 * User input objects
 *************************/

/*Button (dependencies: lv_cont*/
#ifndef USE_LV_BTN
#define USE_LV_BTN      1
#endif
#if USE_LV_BTN != 0
#ifndef LV_BTN_INK_EFFECT
#define LV_BTN_INK_EFFECT   1       /*Enable button-state animations - draw a circle on click (dependencies: USE_LV_ANIMATION)*/
#endif
#endif

/*Image Button (dependencies: lv_btn*/
#ifndef USE_LV_IMGBTN
#define USE_LV_IMGBTN   1
#endif
#if USE_LV_IMGBTN
#ifndef LV_IMGBTN_TILED
#define LV_IMGBTN_TILED 0           /*1: The imgbtn requires left, mid and right parts and the width can be set freely*/
#endif
#endif

/*Button matrix (dependencies: -)*/
#ifndef USE_LV_BTNM
#define USE_LV_BTNM     1
#endif

/*Keyboard (dependencies: lv_btnm)*/
#ifndef USE_LV_KB
#define USE_LV_KB       1
#endif

/*Check box (dependencies: lv_btn, lv_label)*/
#ifndef USE_LV_CB
#define USE_LV_CB       1
#endif

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#ifndef USE_LV_LIST
#define USE_LV_LIST     1
#endif
#if USE_LV_LIST != 0
#ifndef LV_LIST_FOCUS_TIME
#define LV_LIST_FOCUS_TIME  100 /*Default animation time of focusing to a list element [ms] (0: no animation)  */
#endif
#endif

/*Drop down list (dependencies: lv_page, lv_label, lv_symbol_def.h)*/
#ifndef USE_LV_DDLIST
#define USE_LV_DDLIST    1
#endif
#if USE_LV_DDLIST != 0
#ifndef LV_DDLIST_ANIM_TIME
#define LV_DDLIST_ANIM_TIME     200     /*Open and close default animation time [ms] (0: no animation)*/
#endif
#endif

/*Roller (dependencies: lv_ddlist)*/
#ifndef USE_LV_ROLLER
#define USE_LV_ROLLER    1
#endif
#if USE_LV_ROLLER != 0
#ifndef LV_ROLLER_ANIM_TIME
#define LV_ROLLER_ANIM_TIME     200     /*Focus animation time [ms] (0: no animation)*/
#endif
#endif

/*Slider (dependencies: lv_bar)*/
#ifndef USE_LV_SLIDER
#define USE_LV_SLIDER    1
#endif

/*Switch (dependencies: lv_slider)*/
#ifndef USE_LV_SW
#define USE_LV_SW       1
#endif

/*************************
 * Non-user section
 *************************/
#ifdef _MSC_VER                               /* Disable warnings for Visual Studio*/
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif


#endif  /*LV_CONF_CHECKER_H*/

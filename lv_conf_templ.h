/**
 * @file lv_conf.h
 * 
 */

#if 0 /*Remove this to enable the content (Delete the last #endif too!)*/

#ifndef LV_CONF_H
#define LV_CONF_H

/*===================
   Graphical settings
 *=====================*/

/* Horizontal and vertical resolution of the library.
 * Screen resolution multiplied by LV_DOWN_SCALE*/
#define LV_HOR_RES          (320 << LV_ANTIALIAS)
#define LV_VER_RES          (240 << LV_ANTIALIAS)
#define LV_DPI              (80 << LV_ANTIALIAS)
/* Enable anti-aliasing
 * If enabled everything will half-sized
 * Use LV_DOWNSCALE to compensate he down scaling effect of anti-aliasing*/
#define LV_ANTIALIAS        1
#define LV_DOWNSCALE        (1 << LV_ANTIALIAS) /*Set the downscaling value*/

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/
#define LV_VDB_SIZE         (LV_HOR_RES * LV_VER_RES / 20)
#if LV_VDB_SIZE
/* Double virtual buffering
 * One for rendering another to transfer former rendered image to frame buffer in the background*/
#define LV_VDB_DOUBLE       0
#endif

#define LV_REFR_PERIOD      40    /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average number of objects on a screen */

/*=================
   Misc. setting
 *=================*/
/*Display Input settings*/
#define LV_INDEV_READ_PERIOD      50                    /*Input device read period milliseconds*/
#define LV_INDEV_TP_MARKER        0                     /*Mark the pressed points (Value means marker point size)*/
#define LV_INDEV_DRAG_LIMIT       (10 << LV_ANTIALIAS)   /*Drag threshold in pixels */
#define LV_INDEV_DRAG_THROW       20                    /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_INDEV_LONG_PRESS_TIME        400             /*Long press time in milliseconds*/
#define LV_INDEV_LONG_PRESS_REP_TIME    100             /*Repeated trigger period in long press [ms] */

/*lv_obj (base object) settings*/
#define LV_OBJ_FREE_NUM          1           /*Enable the free number attribute*/
#define LV_OBJ_FREE_P            1           /*Enable the free pointer attribute*/
#define LV_OBJ_GROUP             1           /*Enable object groups*/

/*Others*/
#define LV_COLOR_TRANSP     COLOR_LIME      /*This could mean transparent pixel*/
#define USE_LV_EXAMPLE      1               /*Enable examples (lvgl/lv_examples/). Disable to save memory*/

/*==================
 *  LV OBJ X USAGE 
 * ================*/

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#define LV_LABEL_SCROLL_SPEED       (25 << LV_ANTIALIAS) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL' mode*/
#define LV_LABEL_SCROLL_SPEED_VER   (10 << LV_ANTIALIAS) /*Ver. scroll speed if hor. scroll is applied too*/
#define LV_LABEL_SCROLL_PLAYBACK_PAUSE  500 /*Wait before the scroll turns back in ms*/
#define LV_LABEL_SCROLL_REPEAT_PAUSE    500 /*Wait before the scroll begins again in ms*/
#endif

/*Image (dependencies: lv_label (if symbols are enabled) from misc: FSINT, UFS)*/
#define USE_LV_IMG      1
#if USE_LV_IMG != 0
//#define LV_IMG_DEF_WALLPAPER    img_square_x1  /*Comment this line to NOT use wallpaper*/
/* 1: enables to interpret the file names as symbol name
 * from symbol_def.h if they begin with a lower case letter.
 * (driver letters are always upper case)*/
#define LV_IMG_ENABLE_SYMBOLS   1
#if LV_IMG_ENABLE_SYMBOLS != 0
#define LV_IMG_DEF_SYMBOL_FONT       FONT_SYMBOL_30
#endif /*LV_IMG_ENABLE_SYMBOLS*/
#endif /*USE_LV_IMG*/

/*Line (dependencies: -*/
#define USE_LV_LINE     1

/*******************
 * Container object
 *******************/

/*Container (dependencies: -*/
#define USE_LV_CONT     1

/*Page (dependencies: lv_cont)*/
#define USE_LV_PAGE     1

/*Window (dependencies: lv_cont, lv_btn, lv_label, lv_img, lv_page)*/
#define USE_LV_WIN      1

/*Tab (dependencies: lv_page, lv_btnm)*/
#define USE_LV_TABVIEW      1
#if USE_LV_TABVIEW != 0
#define LV_TABVIEW_ANIM_TIME    300 /*Time of slide animation [ms] (0: no animation)*/
#endif

/*************************
 * Data visualizer object
 *************************/

/*Bar (dependencies: -)*/
#define USE_LV_BAR       1

/*Line meter (dependencies: bar; misc: trigo)*/
#define USE_LV_LMETER   1

/*Gauge (dependencies:bar, lmeter; misc: trigo)*/
#define USE_LV_GAUGE    1
#if USE_LV_GAUGE != 0
#define LV_GAUGE_MAX_NEEDLE     4   /*Max number of needles. Used in the style.*/
#endif

/*Chart (dependencies: -)*/
#define USE_LV_CHART    1

/*LED (dependencies: -)*/
#define USE_LV_LED      1

/*Message box (dependencies: lv_rect, lv_btn, lv_label)*/
#define USE_LV_MBOX     1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#define LV_TA_CUR_BLINK_TIME 400     /*ms*/
#define LV_TA_PWD_SHOW_TIME  1500    /*ms*/
#endif

/*************************
 * User input object
 *************************/

/*Button (dependencies: lv_cont*/
#define USE_LV_BTN      1

/*Button matrix (dependencies: -)*/
#define USE_LV_BTNM     1

/*Keyboard (dependencies: lv_btnm)*/
#define USE_LV_KB     1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*Switch (dependencies: lv_slider)*/
#define USE_LV_SW       1

/*List (dependencies: lv_page, lv_btn, lv_label, lv_img)*/
#define USE_LV_LIST     1
#if USE_LV_LIST != 0
#define LV_LIST_FOCUS_TIME  100 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#endif

/*Drop down list (dependencies: lv_page, lv_label)*/
#define USE_LV_DDLIST    1

/*Slider (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1

#endif /*LV_CONF_H*/

#endif /*Remove this to enable the content*/


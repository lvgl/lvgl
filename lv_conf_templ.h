/**
 * @file lv_conf.h
 * 
 */

#if 0 /*Remove this to enable the content (Delete the last #endif too!)*/

#ifndef LV_CONF_H
#define LV_CONF_H
/**
 * @file lv_conf.h
 *
 */


#ifndef LV_CONF_H
#define LV_CONF_H

/*===================
   Graphical settings
 *=====================*/

/* Horizontal and vertical resolution of the library.
 * Screen resolution multiplied by LV_DOWN_SCALE*/
#define LV_HOR_RES          (480 << LV_ANTIALIAS)
#define LV_VER_RES          (320 << LV_ANTIALIAS)
#define LV_DPI              (100 << LV_ANTIALIAS)

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/
#define LV_VDB_SIZE         (20 * LV_VER_RES)

/* Enable anti aliasing
 * If enabled everything will be rendered in double size and filtered to normal size */
#define LV_ANTIALIAS        1


/*=================
   Misc. setting
 *=================*/

/*Screen refresh settings*/
#define LV_REFR_PERIOD      50    /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average number of objects on a screen */

/*Input device settings*/
#define LV_INDEV_READ_PERIOD      50                        /*Input device read period milliseconds*/
#define LV_INDEV_POINT_MARKER     0                         /*Mark the pressed points*/
#define LV_INDEV_DRAG_LIMIT       (10 << LV_ANTIALIAS)      /*Drag threshold in pixels */
#define LV_INDEV_DRAG_THROW       20                        /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_INDEV_LONG_PRESS_TIME  400                       /*Long press time in milliseconds*/
#define LV_INDEV_LONG_PRESS_REP_TIME 100                    /*Repeated trigger period in long press [ms] */

/*lv_obj (base object) settings*/
#define LV_OBJ_FREE_NUM_TYPE    uint32_t    /*Type of free number attribute (comment out disable free number)*/
#define LV_OBJ_FREE_PTR         1           /*Enable the free pointer attribute*/
#define LV_OBJ_GROUP            1           /*Enable object groups*/

/*Others*/
#define LV_COLOR_TRANSP     COLOR_LIME          /*Images pixels with this color will not be drawn*/
#define LV_FONT_DEFAULT      &font_dejavu_40    /*Always set a default font from the built-in fonts*/

/*==================
 *  IMAGE USAGE
 * ================*/
#define LV_IMAGE_ENABLE_ALL    1       /*Unconditionally enable all image maps*/
//#define USE_IMG_XYZ      1           /*Enable or disable to compile you image map files*/

/*==================
 *  THEME USAGE
 * ================*/
#define USE_LV_THEME_TEMPL      1       /*Just for test*/
#define USE_LV_THEME_DEFAULT    1       /*Built manly from the built-in styles. Consumes very few RAM*/
#define USE_LV_THEME_ALIEN      1       /*Dark futuristic theme*/
#define USE_LV_THEME_MATERIAL   1       /*Flat theme with bold colors and light shadows (Planned)*/
#define USE_LV_THEME_ZEN        1       /*Peaceful, mainly black and white theme (Planned)*/
#define USE_LV_THEME_NIGHT      1       /*Dark elegant theme (Planned)*/

/*==================
 *  LV OBJ X USAGE 
 * ================*/

/*****************
 * Simple object
 *****************/

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#define LV_LABEL_SCROLL_SPEED       (25 << LV_ANTIALIAS) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL/ROLL' mode*/
#endif

/*Image (dependencies: lv_label (if symbols are enabled) from misc: FSINT, UFS)*/
#define USE_LV_IMG      1

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
#define USE_LV_BAR      1

/*Line meter (dependencies: bar; misc: trigo)*/
#define USE_LV_LMETER   1

/*Gauge (dependencies:bar, lmeter; misc: trigo)*/
#define USE_LV_GAUGE    1

/*Chart (dependencies: -)*/
#define USE_LV_CHART    1

/*LED (dependencies: -)*/
#define USE_LV_LED      1

/*Message box (dependencies: lv_rect, lv_btnm, lv_label)*/
#define USE_LV_MBOX     1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#define LV_TA_CURSOR_BLINK_TIME 400   /*ms*/
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
#define USE_LV_KB       1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*Switch (dependencies: lv_slider)*/
#define USE_LV_SW       1

/*List (dependencies: lv_page, lv_btn, lv_label, (lv_img optionally for icons ))*/
#define USE_LV_LIST     1
#if USE_LV_LIST != 0
#define LV_LIST_FOCUS_TIME  100 /*Default animation time of focusing to a list element [ms] (0: no animation)  */
#endif

/*Drop down list (dependencies: lv_page, lv_label)*/
#define USE_LV_DDLIST    1
#if USE_LV_DDLIST != 0
#define LV_DDLIST_DEF_ANIM_TIME     200     /*Open and close default animation time [ms] (0: no animation)*/
#endif

/*Drop down list (dependencies: lv_ddlist)*/
#define USE_LV_ROLLER    1

/*Slider (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1

#endif /*LV_CONF_H*/

#endif /*Remove this to enable the content*/


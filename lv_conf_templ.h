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
#define LV_HOR_RES          (320 * LV_DOWNSCALE)
#define LV_VER_RES          (240 * LV_DOWNSCALE)
#define LV_DPI              (80 * LV_DOWNSCALE)
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
#define LV_DISPI_READ_PERIOD      50                    /*Input device read period milliseconds*/
#define LV_DISPI_TP_MARKER        0                     /*Mark the pressed points (Value means marker point size)*/
#define LV_DISPI_DRAG_LIMIT       (10 * LV_DOWNSCALE)   /*Drag threshold in pixels */
#define LV_DISPI_DRAG_THROW       20                    /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_DISPI_LONG_PRESS_TIME        400             /*Long press time in milliseconds*/
#define LV_DISPI_LONG_PRESS_REP_TIME    100             /*Repeated trigger period in long press [ms] */

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
#define LV_LABEL_SCROLL_SPEED       (25 * LV_DOWNSCALE) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL' mode*/
#define LV_LABEL_SCROLL_SPEED_VER   (10 * LV_DOWNSCALE) /*Ver. scroll speed if hor. scroll is applied too*/
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
#define LV_TA_MAX_LENGTH    256
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

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*List (dependencies: lv_page, lv_btn, lv_label, lv_img)*/
#define USE_LV_LIST     1
#if USE_LV_LIST != 0
#define LV_LIST_FOCUS_TIME  100 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#endif

/*Drop down list (dependencies: lv_page, lv_label)*/
#define USE_LV_DDLIST    1

/*Slider (dependencies: lv_bar)*/
#define USE_LV_SLIDER    1

/*==================
 *  LV APP SETTINGS
 * =================*/

/*Enable the application system*/
#define LV_APP_ENABLE       0
#if LV_APP_ENABLE != 0

/****************************
 * Basic application settings
 *****************************/
#define LV_APP_DESKTOP      1                   /*Create a desktop-like environment*/

#define LV_APP_SC_WIDTH     (LV_DPI * 2)        /*Shortcut width*/
#define LV_APP_SC_HEIGHT    (3 * LV_DPI / 2)    /*Shortcut height*/
#define LV_APP_FONT_SMALL   FONT_DEJAVU_20      /*A small font*/
#define LV_APP_FONT_MEDIUM  FONT_DEFAULT        /*A medium font*/
#define LV_APP_FONT_LARGE   FONT_DEJAVU_40      /*A large font*/

/***********************
 * Animation settings
 ***********************/
#define LV_APP_ANIM_WIN      200      /*Animation time of windows [ms] (0: turn off animations)*/
#define LV_APP_ANIM_DESKTOP  200      /*Animation time the desktop [ms] (0: turn off animations)*/

/************************
 * App. utility settings
 ************************/

/*Notice*/
#define USE_LV_APP_NOTICE           1
#if USE_LV_APP_NOTICE != 0
#define LV_APP_NOTICE_SHOW_TIME         4000    /*Notices will be shown for this time [ms]*/
#define LV_APP_NOTICE_CLOSE_ANIM_TIME   300     /*Notice close animation time. [ms] 0: no animation */
#define LV_APP_NOTICE_MAX_NUM   6               /*Max. number of notices*/
#define LV_APP_NOTICE_MAX_LEN   256             /*Max. number of characters on a notice*/
#endif

/*File selector*/
#define USE_LV_APP_FSEL         1
#if USE_LV_APP_FSEL != 0
#define LV_APP_FSEL_PAGE_SIZE     8         /*Max. number of files/folder on a page*/
#define LV_APP_FSEL_FN_MAX_LEN    32        /*Max file name length*/
#define LV_APP_FSEL_PATH_MAX_LEN  256       /*Max path length*/
#endif

/*Keyboard*/
#define USE_LV_APP_KB           1
#if USE_LV_APP_KB != 0
#define LV_APP_KB_ANIM_TIME     300         /*ms*/
#endif

/*==================
 *  LV APP X USAGE 
 * ================*/

/*Example application*/
#define USE_LV_APP_EXAMPLE  1

/*Phantom application*/
#define USE_LV_APP_PHANTOM  1
#if USE_LV_APP_PHANTOM != 0
/*No settings*/
#endif

/*System monitor*/
#define USE_LV_APP_SYSMON   1
#if USE_LV_APP_SYSMON != 0
#define LV_APP_SYSMON_REFR_TIME     500         /*Mem. and CPU usage read period [ms]*/
#define LV_APP_SYSMON_PNUM          64          /*Number of point on the window's chart*/
#define LV_APP_SYSMON_MEM_WARN      (2 * 1024)  /*Make a notice less then this remaining memory [bytes]*/
#define LV_APP_SYSMON_FRAG_WARN     (70)        /*Make a notice above this fragmentation level [%]*/
#define LV_APP_SYSMON_DEFRAG_PERIOD (5000)      /*Auto-defrag period [ms]*/
#endif /*USE_LV_APP_SYSMON != 0*/

/*Terminal*/
#define USE_LV_APP_TERMINAL 1
#if USE_LV_APP_TERMINAL != 0
#define LV_APP_TERMINAL_LENGTH      512     /*Memory of the terminal [character number]*/
#endif /*USE_LV_APP_TERMINAL != 0*/

/*Files*/
#define USE_LV_APP_FILES    1
#if USE_LV_APP_FILES != 0
#define LV_APP_FILES_PAGE_SIZE      8       /*Max. number of files/folder on a page*/
#define LV_APP_FILES_FN_MAX_LEN     32      /*Max file name length*/
#define LV_APP_FILES_PATH_MAX_LEN   256     /*Max path length*/
#define LV_APP_FILES_CHUNK_DEF_SIZE 256     /*Chunk size when sending a file*/
#define LV_APP_FILES_CHUNK_DEF_TIME 100     /*Delay between sent chunks*/
#define LV_APP_FILES_CHUNK_MAX_SIZE 1024    /*Max chunk size when the user sets it*/
#endif /*USE_LV_APP_FILES != 0*/

/*Benchmark*/
#define USE_LV_APP_BENCHMARK     1
#if USE_LV_APP_BENCHMARK != 0
/*No settings*/
#endif

/*WiFi*/
#define USE_LV_APP_WIFI    0
#if USE_LV_APP_WIFI != 0
#define LV_APP_WIFI_CONF_PATH       "S:/wifi_conf.txt"  /*Save config. here. Comment to use def. value*/
#ifndef LV_APP_WIFI_CONF_PATH
#define LV_APP_WIFI_SSID_DEF        "ssid"
#define LV_APP_WIFI_PWD_DEF         "pwd"
#define LV_APP_WIFI_IP_DEF          "100.101.102.103"
#define LV_APP_WIFI_PORT_DEF        "1234" 
#endif  /*LV_APP_WIFI_CONF_PATH*/
#define LV_APP_WIFI_AUTO_CONNECT    1       /*Try to connect at start up to the deafult SSID and IP:PORT*/
#endif /*USE_LV_APP_WIFI != 0*/

/*GSM*/
#define USE_LV_APP_GSM    0
#if USE_LV_APP_GSM != 0
#define LV_APP_GSM_CONF_PATH       "S:/gsm_conf.txt"    /*Save config. here. Comment to use def. value*/
#ifndef LV_APP_GSM_CONF_PATH
#define LV_APP_GSM_APN_DEF        "apn"
#define LV_APP_GSM_IP_DEF         "101.102.103.104"
#define LV_APP_GSM_PORT_DEF       "1234"
#endif  /*LV_APP_GSM_CONF_PATH*/
#define LV_APP_GSM_AUTO_CONNECT   1       /*Try to connect at start up with the deafult APN and IP:PORT*/
#endif /*USE_LV_APP_GSM != 0*/

/*Ethernet*/
#define USE_LV_APP_ETHERNET    0
#if USE_LV_APP_ETHERNET != 0
/*No settings*/
#endif /*USE_LV_APP_ETHERNET != 0*/

#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_CONF_H*/


#endif /*Remove this to enable the content*/


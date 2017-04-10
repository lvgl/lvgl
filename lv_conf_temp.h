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
#define LV_HOR_RES          (480 * LV_DOWNSCALE)
#define LV_VER_RES          (320 * LV_DOWNSCALE)

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/

#define LV_VDB_SIZE         (LV_HOR_RES * (LV_VER_RES / 20))

/* Enable antialaiassing
 * If enabled everything will half-sized
 * Use LV_DOWNSCALE to compensate
 * the down scaling effect of antialiassing*/
#define LV_ANTIALIAS        1
#define LV_FONT_ANTIALIAS   0
/*Set the downscaling value*/
#if LV_ANTIALIAS == 0
#define LV_DOWNSCALE        1
#else
#define LV_DOWNSCALE        2
#endif

#define LV_REFR_PERIOD      40    /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average number of objects on a screen */

/*=================
   Misc. setting
 *=================*/
/*Display Input settings*/
#define LV_DISPI_READ_PERIOD      50     /*Input device read period milliseconds*/
#define LV_DISPI_TP_MARKER        0      /*Mark the pressed points*/
#define LV_DISPI_DRAG_LIMIT       10     /*Drag threshold in pixels */
#define LV_DISPI_DRAG_THROW       20     /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_DISPI_LONG_PRESS_TIME  400    /*Long press time in milliseconds*/
#define LV_DISPI_LONG_PRESS_REP_TIME 100 /*Repeated trigger period in long press [ms] */

/*Coordinates*/
#define LV_CORD_TYPE    int16_t /*Coordinate type*/
#define LV_CORD_MAX     (32000)
#define LV_CORD_MIN     (-32000)

/*Fonts and texts*/
#define USE_FONT_DEJAVU_8    1
#define USE_FONT_DEJAVU_10   1
#define USE_FONT_DEJAVU_14   1
#define USE_FONT_DEJAVU_20   1
#define USE_FONT_DEJAVU_30   1
#define USE_FONT_DEJAVU_40   1
#define USE_FONT_DEJAVU_60   1
#define USE_FONT_DEJAVU_80   1
#define USE_FONT_SYMBOL_30   1
#define USE_FONT_SYMBOL_60   1
#define LV_FONT_DEFAULT      FONT_DEJAVU_30  /*Always set a default font*/
#define LV_TXT_BREAK_CHARS  " ,.;-" /*Can break texts on these chars*/

/*lv_obj (base object) settings*/
#define LV_OBJ_FREE_P            1           /*Enable the free pointer attribute*/
#define LV_OBJ_DEF_SCR_COLOR     COLOR_SILVER /*Default screen color*/

/*Others*/
#define LV_COLOR_TRANSP     COLOR_LIME

/*==================
 *  LV OBJ X USAGE 
 * ================*/

/*Rectangle (dependencies: -*/
#define USE_LV_RECT     1

/*Label (dependencies: -*/
#define USE_LV_LABEL    1
#if USE_LV_LABEL != 0
#define LV_LABEL_SCROLL_SPEED       (25 * LV_DOWNSCALE) /*Hor, or ver. scroll speed (px/sec) in 'LV_LABEL_LONG_SCROLL' mode*/
#define LV_LABEL_SCROLL_SPEED_VER   (10 * LV_DOWNSCALE) /*Ver. scroll speed if hor. scroll is applied too*/
#define LV_LABEL_SCROLL_PLAYBACK_PAUSE  500 /*Wait before the scroll turns back in ms*/
#define LV_LABEL_SCROLL_REPEAT_PAUSE    500 /*Wait before the scroll begins again in ms*/
#endif

/*Button (dependencies: lv_rect*/
#define USE_LV_BTN      1

/*Line (dependencies: -*/
#define USE_LV_LINE     1

/*Image (dependencies: lv_label (if symbols are enabled) from misc: FSINT, UFS)*/
#define USE_LV_IMG      1
#if USE_LV_IMG != 0
#define LV_IMG_DEF_WALLPAPER    img_square_x2  /*Comment this line to NOT use wallpaper*/
/* 1: enables to interpret the file names as symbol name
 * from symbol_def.h if they begin with a lower case letter.
 * (driver letters are always upper case)*/
#define LV_IMG_ENABLE_SYMBOLS   1
#if LV_IMG_ENABLE_SYMBOLS != 0
#define LV_IMG_DEF_SYMBOL_FONT       FONT_SYMBOL_30
#endif /*LV_IMG_ENABLE_SYMBOLS*/
#endif /*USE_LV_IMG*/

/*Page (dependencies: lv_rect)*/
#define USE_LV_PAGE     1
#if USE_LV_PAGE != 0
#define LV_PAGE_ANIM_FOCUS_TIME 300 /*List focus animation time [ms] (0: turn off the animation)*/
#endif

/*List (dependencies: lv_btn, lv_label, lv_img)*/
#define USE_LV_LIST     1

/*Check box (dependencies: lv_btn, lv_label)*/
#define USE_LV_CB       1

/*Progress bar (dependencies: lv_rect, lv_label)*/
#define USE_LV_PB       1

/*LED (dependencies: lv_rect)*/
#define USE_LV_LED      1

/*Chart (dependencies: lv_rect, lv_line)*/
#define USE_LV_CHART    1

/*Text area (dependencies: lv_label, lv_page)*/
#define USE_LV_TA       1
#if USE_LV_TA != 0
#define LV_TA_MAX_LENGTH    256
#define LV_TA_CUR_BLINK_TIME 400    /*ms*/
#endif

/*Button matrix (dependencies: lv_rect, lv_label)*/
#define USE_LV_BTNM     1

/*Drop down list (dependencies: lv_page, lv_btn_t, lv_label_t)*/
#define USE_LV_DDLIST      1
#if USE_LV_DDLIST != 0
#define LV_DDLIST_ANIM_TIME    100 /*DDL open/close animation in milliseconds (0: disable animation)*/
#endif

/*Window (dependencies: lv_rect, lv_btn, lv_label, lv_img, lv_page)*/
#define USE_LV_WIN      1

/*Message box (dependencies: lv_rect, lv_btn, lv_label)*/
#define USE_LV_MBOX     1
#if USE_LV_MBOX != 0
#define LV_MBOX_ANIM_TIME   250 /*How fast animate out the message box in auto close. 0: no animation [ms]*/
#endif

/*Gauge (dependencies: lv_rect, lv_label, lv_line, misc: trigo)*/
#define USE_LV_GAUGE    1

/*==================
 *  LV APP SETTINGS
 * =================*/

/*Enable the application system*/
#define LV_APP_ENABLE       1

#if LV_APP_ENABLE != 0
#define LV_APP_SC_WIDTH     (LV_HOR_RES / 4)    /*Shortcut width*/
#define LV_APP_SC_HEIGHT    (LV_VER_RES / 3)    /*Shortcut height*/
#define LV_APP_FONT_SMALL   FONT_DEJAVU_20
#define LV_APP_FONT_MEDIUM  LV_FONT_DEFAULT
#define LV_APP_FONT_LARGE   FONT_DEJAVU_40

/* Internal icons:
 * 0: Do not use internal icons (img_close, img_add etc. icons have to be provided)
 * 1: Use simple sized icons
 * 2: Use double sized icons*/
#define LV_APP_USE_INTERNAL_ICONS   2

/*Enable or disable the internal icons individually*/
#if LV_APP_USE_INTERNAL_ICONS != 0
#define USE_IMG_CLOSE       1
#define USE_IMG_DOWN        1
#define USE_IMG_DRIVER      1
#define USE_IMG_FILE        1
#define USE_IMG_FOLDER      1
#define USE_IMG_LEFT        1
#define USE_IMG_OK          1
#define USE_IMG_RIGHT       1
#define USE_IMG_UP          1
#endif

/*Animation settings*/
#define LV_APP_EFFECT_OPA      1 /*Enable the opacity in the application style (can be modified)*/
#define LV_APP_EFFECT_ANIM     1 /*Enable the animation of the applications*/
#define LV_APP_EFFECT_OPA_ANIM 1 /*Enable the using opacity in the application animations*/
#define LV_APP_ANIM_WIN 200      /*Animation time in milliseconds (0: turn off animation)*/
#define LV_APP_ANIM_SC  200      /*Animation time in milliseconds (0: turn off animation)*/
#define LV_APP_ANIM_NOTICE 300   /*Obsolete, use LV_MBOX_ANIM. */

/* App. utility settings */
#define LV_APP_NOTICE_SHOW_TIME 4000 /*Notices will be shown for this time [ms]*/
#define LV_APP_NOTICE_MAX_NUM   6    /*Max. number of notices*/
#define LV_APP_NOTICE_MAX_LEN   256  /*Max. number of characters on a notice*/
/*==================
 *  LV APP X USAGE 
 * ================*/

#define USE_LV_APP_EXAMPLE  1

#define USE_LV_APP_SYSMON   1
#if USE_LV_APP_SYSMON != 0
#define LV_APP_SYSMON_REFR_TIME     500     /*[ms]*/
#define LV_APP_SYSMON_PNUM          64
#define LV_APP_SYSMON_MEM_WARN      (4 * 1024)
#define LV_APP_SYSMON_FRAG_WARN     (70)     /*[%]*/
#define LV_APP_SYSMON_DEFRAG_PERIOD (5000)   /*[%]*/
#endif /*USE_LV_APP_SYSMON != 0*/

#define USE_LV_APP_TERMINAL 1
#if USE_LV_APP_TERMINAL != 0
#define LV_APP_TERMINAL_LENGTH      512  /*Memory of the terminal*/
#endif /*USE_LV_APP_TERMINAL != 0*/

#define USE_LV_APP_FILES 1
#if USE_LV_APP_FILES != 0
#define LV_APP_FILES_PAGE_SIZE      8   /*Max. number of files/folder on a page*/
#define LV_APP_FILES_FN_MAX_LEN     128
#define LV_APP_FILES_PATH_MAX_LEN   256
#define LV_APP_FILES_CHUNK_DEF_SIZE 256
#define LV_APP_FILES_CHUNK_DEF_TIME 100
#define LV_APP_FILES_CHUNK_MAX_SIZE 1024
#endif /*USE_LV_APP_FILES != 0*/
#endif /*LV_APP_ENABLE != 0*/

#endif /*LV_CONF_H*/

#endif /*Remove this to enable the content*/


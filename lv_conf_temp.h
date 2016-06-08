/**
 * @file lv_conf.h
 * 
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#if 0 /*Remove this to enable the content*/

/*=================
   Basic setting
 *=================*/

/* Horizontal and vertical resolution of the library.
 * Screen resolution multiplied by LV_DOWN_SCALE*/
#define LV_HOR_RES          (320 * LV_DOWNSCALE)
#define LV_VER_RES          (240 * LV_DOWNSCALE)

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/
#define LV_VDB_SIZE         (LV_HOR_RES * LV_DOWNSCALE * 20)

/* Anti-aliasing with downscaling everything
 * 1: disabled
 * 2: 2x anti-alias -> half size
 * 4: 4x anti-alias -> quarter size */
#define LV_DOWNSCALE		1

#define LV_UPSCALE_FONT	 0 /*Scale up fonts to compensate LV_DOWNSCALE*/
#define LV_UPSCALE_MAP	 0 /*Scale up maps (e.g. images) elements to compensate LV_DOWNSCALE*/
#define LV_UPSCALE_SYTLE 0 /*Scale up default styles to compensate LV_DOWNSCALE*/

/*=====================
   Graphical settings
 *=====================*/

/* Horizontal and vertical resolution of the library.
 * Screen resolution multiplied by LV_DOWN_SCALE*/
#define LV_HOR_RES          (480 * LV_DOWNSCALE)
#define LV_VER_RES          (320 * LV_DOWNSCALE)

/* Buffered rendering: >= LV_DOWNSCALE * LV_HOR_RES or 0 to disable buffering*/
#define LV_VDB_SIZE         (LV_HOR_RES * LV_DOWNSCALE * 20)

/* Anti-aliasing with downscaling everything
 * 1: disabled
 * 2: 2x anti-alias -> half size
 * 4: 4x anti-alias -> quarter size */
#define LV_DOWNSCALE		2

#define LV_UPSCALE_FONT	 0 /*Scale up fonts to compensate LV_DOWNSCALE*/
#define LV_UPSCALE_MAP	 0 /*Scale up maps (e.g. images) elements to compensate LV_DOWNSCALE*/
#define LV_UPSCALE_SYTLE 1 /*Scale up default styles to compensate LV_DOWNSCALE*/

#define LV_REFR_PERIOD   50 	  /*Screen refresh period in milliseconds*/
#define LV_INV_FIFO_SIZE    32    /*The average number of objects on a screen */

/*=================
   Misc. setting
 *=================*/
/*Display Input settings*/
#define LV_DISPI_READ_PERIOD   	50 /*Input device read period milliseconds*/
#define LV_DISPI_TP_MARKER		0  /*Mark the pressed points*/
#define LV_DISPI_DRAG_LIMIT       10    /*Drag threshold in pixels */
#define LV_DISPI_DRAG_THROW       20    /*Drag throw slow-down in [%]. Greater value means faster slow-down */
#define LV_DISPI_LONG_PRESS_TIME  400   /*Long press time in milliseconds*/

/*Coordinates*/
#define LV_CORD_TYPE    int16_t	/*Coordinate type*/
#define LV_CORD_MAX     (32000)
#define LV_CORD_MIN     (-32000)

/*Fonts and texts*/
#define USE_FONT_DEJAVU_14   1
#define USE_FONT_DEJAVU_20   1
#define USE_FONT_DEJAVU_30   1
#define USE_FONT_DEJAVU_40   1
#define USE_FONT_DEJAVU_60   1
#define USE_FONT_DEJAVU_80   1
#define LV_FONT_DEFAULT  FONT_DEJAVU_40  /*Always set a default font*/
#define LV_TXT_BREAK_CHARS	" ,.;-" /*Can break texts on these chars*/

/*lv_obj (base object) settings*/
#define LV_OBJ_FREE_P        1 	/*Enable the free pointer attribute*/
#define LV_OBJ_DEF_SCR_COLOR     COLOR_WHITE /*Default screen color*/

/*==================
 *  LV OBJ X USAGE 
 * ================*/

#define USE_LV_RECT     1

#define USE_LV_LABEL    1

#define USE_LV_BTN      1

#define USE_LV_LINE     1

#define USE_LV_IMG      1
#if USE_LV_IMG != 0
#define LV_IMG_COLOR_TRANSP     COLOR_LIME
#endif /*USE_LV_IMG*/

#define USE_LV_PAGE     1

#define USE_LV_LIST     1

/*==================
 *  LV APP SETTINGS 
 * =================*/

/*==================
 *  LV APP X USAGE 
 * ================*/

#endif /*Remove this to enable the content*/

#endif

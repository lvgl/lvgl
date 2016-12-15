/**
 * @file lv_page.h
 * 
 */

#ifndef LV_PAGE_H
#define LV_PAGE_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PAGE != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_PAGE_SB_MODE_OFF,
	LV_PAGE_SB_MODE_ON,
	LV_PAGE_SB_MODE_AUTO,
}lv_page_sb_mode_t;

/*Style of page*/
typedef struct
{
    lv_rects_t bg_rects;  /*Style of ancestor*/
	/*New style element for this type */
    lv_rects_t scrable_rects;
    lv_rects_t sb_rects;
    cord_t sb_width;
    lv_page_sb_mode_t sb_mode;
    uint8_t sb_opa;
}lv_pages_t;

/*Built-in styles of page*/
typedef enum
{
	LV_PAGES_DEF,
	LV_PAGES_TRANSP,
}lv_pages_builtin_t;

/*Data of page*/
typedef struct
{
	lv_rect_ext_t rect_ext; /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * scrolling;	/*The scrollable object on the background*/
    area_t sbh;  				/*Horizontal scrollbar*/
    area_t sbv;  				/*Vertical scrollbar*/
    uint8_t sbh_draw :1; 		/*1: horizontal scrollbar is visible now*/
    uint8_t sbv_draw :1; 		/*1: vertical scrollbar is visible now*/
}lv_page_ext_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t * lv_page_create(lv_obj_t * par, lv_obj_t * copy);
void lv_page_glue_obj(lv_obj_t * page, bool glue);
lv_pages_t * lv_pages_get(lv_pages_builtin_t style, lv_pages_t * copy);
bool lv_page_signal(lv_obj_t * page, lv_signal_t sign, void  * param);

/**********************
 *      MACROS
 **********************/

#endif

#endif

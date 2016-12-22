/**
 * @file lv_ta.h
 * 
 */

#ifndef LV_TA_H
#define LV_TA_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_TA != 0

#include "../lv_obj/lv_obj.h"
#include "lv_page.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/
#define LV_TA_CUR_LAST (0x7FFF) /*Put the cursor after the last character*/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of text area*/
typedef struct
{
	lv_pages_t pages;	/*Style of ancestor*/
	/*New style element for this type */
	lv_labels_t labels;
	color_t cursor_color;
	cord_t cursor_width;
	uint8_t cursor_show :1;
}lv_tas_t;

/*Built-in styles of text area*/
typedef enum
{
	LV_TAS_DEF,
}lv_tas_builtin_t;

/*Data of text area*/
typedef struct
{
	lv_page_ext_t page; /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * label;
	cord_t cursor_valid_x;
	uint16_t cursor_pos;
	uint8_t cur_hide :1;	/*Indicates that the cursor is visible now or not*/
}lv_ta_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_ta_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_ta_signal(lv_obj_t * ta, lv_signal_t sign, void * param);
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy);

void lv_ta_add_char(lv_obj_t * ta, char c);
void lv_ta_add_text(lv_obj_t * ta, const char * txt);
void lv_ta_set_text(lv_obj_t * ta, const char * txt);
void lv_ta_del(lv_obj_t * ta);

void lv_ta_set_cursor_pos(lv_obj_t * ta, int16_t pos);
void lv_ta_cursor_right	(lv_obj_t * ta);
void lv_ta_cursor_left(lv_obj_t * taj);
void lv_ta_cursor_down(lv_obj_t * ta);
void lv_ta_cursor_up(lv_obj_t * ta);

const char * lv_ta_get_txt(lv_obj_t * ta);
uint16_t lv_ta_get_cursor_pos(lv_obj_t * ta);


/**********************
 *      MACROS
 **********************/

#endif

#endif

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

/**********************
 *      TYPEDEFS
 **********************/

/*Style of text area*/
typedef struct
{
	lv_pages_t pages;	/*Style of ancestor*/
	/*New style element for this type */
	lv_labels_t labels;
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
	lv_obj_t * label_dp;
	uint16_t cursor_pos;
}lv_ta_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_ta_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_ta_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_tas_t * lv_tas_get(lv_tas_builtin_t style, lv_tas_t * copy_p);

void lv_ta_add_char(lv_obj_t * obj_dp, char c);
void lv_ta_add_text(lv_obj_t * obj_dp, const char * txt);
void lv_ta_del(lv_obj_t * obj_dp);
void lv_ta_set_cursor_pos(lv_obj_t * obj_dp, uint16_t pos);
void lv_ta_cursor_down(lv_obj_t * obj_dp);
void lv_ta_cursor_up(lv_obj_t * obj_dp);


const char * lv_ta_get_txt(lv_obj_t obj_dp);
uint16_t lv_ta_get_cursor_pos(lv_obj_t * obj_dp);


/**********************
 *      MACROS
 **********************/

#endif

#endif

/**
 * @file lv_btnm.h
 * 
 */


#ifndef LV_BTNM_H
#define LV_BTNM_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BTNM != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"
#include "lv_label.h"
#include "lv_btn.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of button matrix*/
typedef struct
{
	lv_rects_t rects;	/*Style of ancestor*/
	/*New style element for this type */
	lv_btns_t btns;		/*Style of the buttons*/
	lv_labels_t labels; /*Style of the labels on the buttons*/
}lv_btnms_t;

/*Built-in styles of button matrix*/
typedef enum
{
	LV_BTNMS_DEF,
}lv_btnms_builtin_t;

/* Type of callback function which is called when a button is released
 * Parameters: button matrix,  released button index in the map string
 * return false: the released button or the button matrix is deleted else true*/
typedef lv_action_res_t (*lv_btnm_callback_t) (lv_obj_t *, uint16_t);

/*Data of button matrix*/
typedef struct
{
	lv_rect_ext_t rect;	/*Ext. of ancestor*/
	/*New data for this type */
	const char ** map_p;	/*Pointer to the current map*/
	area_t * btn_areas;
	uint16_t btn_cnt;
	uint16_t btn_pr;
	lv_btnm_callback_t cb;
}lv_btnm_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_btnm_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_btnm_signal(lv_obj_t * btnm, lv_signal_t sign, void * param);
lv_btnms_t * lv_btnms_get(lv_btnms_builtin_t style, lv_btnms_t * copy);

void lv_btnm_set_map(lv_obj_t * btnm, const char ** map);
void lv_btnm_set_cb(lv_obj_t * btnm, lv_btnm_callback_t cb);

const char ** lv_btnm_get_map(lv_obj_t * btnm);
lv_btnm_callback_t lv_btnm_get_cb(lv_obj_t * btnm);
/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_BTNM*/

#endif /*LV_BTNM_H*/

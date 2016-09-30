/**
 * @file lv_btnm.h
 * 
 */


/*Search an replace: button matrix -> object normal name with lower case (e.g. button, label etc.)
 * 					 btnm -> object short name with lower case(e.g. btn, label etc)
 *                   BTNM -> object short name with upper case (e.g. BTN, LABEL etc.)
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
	lv_btns_t btns;
	lv_labels_t labels;
}lv_btnms_t;

/*Built-in styles of button matrix*/
typedef enum
{
	LV_BTNMS_DEF,
}lv_btnms_builtin_t;

/* Type of callback function which is called when a button is released
 * Parameters: button matrix, released object, button index in the map string*/
typedef bool (*lv_btnm_callback_t) (lv_obj_t *, lv_obj_t *, uint16_t);

/*Data of button matrix*/
typedef struct
{
	lv_rect_ext_t rect;	/*Ext. of ancestor*/
	/*New data for this type */
	const char ** map_p;	/*Pointer to the current map*/
	lv_btnm_callback_t cb;
}lv_btnm_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_btnm_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_btnm_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
lv_btnms_t * lv_btnms_get(lv_btnms_builtin_t style, lv_btnms_t * copy_p);

void lv_btnm_set_map(lv_obj_t * obj_dp, const char ** map_p);
void lv_btnm_set_cb(lv_obj_t * obj_dp, lv_btnm_callback_t cb);

const char ** lv_btnm_get_map(lv_obj_t * obj_dp);
lv_btnm_callback_t lv_btnm_get_cb(lv_obj_t * obj_dp);
/**********************
 *      MACROS
 **********************/

#endif

#endif

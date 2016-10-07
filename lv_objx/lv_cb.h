/**
 * @file lv_cb.h
 * 
 */

#ifndef LV_CB_H
#define LV_CB_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_CB != 0

#include "../lv_obj/lv_obj.h"
#include "lv_btn.h"
#include "lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of check box*/
typedef struct
{
	lv_btns_t bg; /*Style of ancestor*/
	/*New style element for this type */
	lv_btns_t bullet;
	lv_labels_t label;
	cord_t bullet_size;
}lv_cbs_t;

/*Built-in styles of check box*/
typedef enum
{
	LV_CBS_DEF,
}lv_cbs_builtin_t;

/*Data of check box*/
typedef struct
{
	lv_btn_ext_t bg_btn; /*Ext. of ancestor*/
	/*New data for this type */
	lv_obj_t * bullet;
	lv_obj_t * label;
}lv_cb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t * lv_cb_create(lv_obj_t * par, lv_obj_t * copy);
bool lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param);
void lv_cb_set_text(lv_obj_t * cb, const char * txt);
const char * lv_cb_get_text(lv_obj_t * cb);
lv_cbs_t * lv_cbs_get(lv_cbs_builtin_t style, lv_cbs_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif

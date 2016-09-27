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
	lv_btns_t bg;
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
	lv_btn_ext_t btn_ext;
	lv_obj_t * bullet;
	lv_obj_t * label;
}lv_cb_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* lv_cb_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);
bool lv_cb_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
void lv_cb_set_text(lv_obj_t * obj_dp, const char * txt);
const char * lv_cb_get_text(lv_obj_t * obj_dp);
lv_cbs_t * lv_cbs_get(lv_cbs_builtin_t style, lv_cbs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif

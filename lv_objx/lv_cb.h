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
/*Testing of dependencies*/
#if USE_LV_BTN == 0
#error "lv_cb: lv_rect is required. Enable it in lv_conf.h (USE_LV_BTN  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_cb: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

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

/**
 * Create a check box objects
 * @param par pointer to an object, it will be the parent of the new check box
 * @param copy pointer to a check box object, if not NULL then the new object will be copied from it
 * @return pointer to the created check box
 */
lv_obj_t * lv_cb_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_cb_signal(lv_obj_t * cb, lv_signal_t sign, void * param);

/**
 * Set the text of a check box
 * @param cb pointer to a check box
 * @param txt the text of the check box
 */
void lv_cb_set_text(lv_obj_t * cb, const char * txt);

/**
 * Get the text of a check box
 * @param cb pointer to check box object
 * @return pointer to the text of the check box
 */
const char * lv_cb_get_text(lv_obj_t * cb);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_cbs_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_cbs_t style
 */
lv_cbs_t * lv_cbs_get(lv_cbs_builtin_t style, lv_cbs_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif

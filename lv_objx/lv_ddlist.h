/**
 * @file lv_ddlist.h
 * 
 */

#ifndef LV_DDLIST_H
#define LV_DDLIST_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_DDLIST != 0

#include "../lv_obj/lv_obj.h"
#include "../lv_objx/lv_page.h"
#include "../lv_objx/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of drop down list*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * opt_label;   /*Label for the options*/
    lv_action_res_t (*cb)(lv_obj_t *, uint16_t);
    uint16_t sel_opt;
    uint8_t opened :1;
    uint8_t auto_size :1;
}lv_ddlist_ext_t;

/*Style of drop down list*/
typedef struct
{
	lv_pages_t pages;   /*Style of ancestor*/
	/*New style element for this type */
	lv_rects_t sel_rects;
	lv_labels_t list_labels;
}lv_ddlists_t;

/*Built-in styles of drop down list*/
typedef enum
{
	LV_DDLISTS_DEF,
}lv_ddlists_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_ddlist_create(lv_obj_t * par, lv_obj_t * copy);

/**
 * Signal function of the drop down list
 * @param ddlist pointer to a drop down list object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_ddlist_signal(lv_obj_t * ddlist, lv_signal_t sign, void * param);

/**
 * Set the options in a drop down list
 * @param ddlist pointer to drop down list object
 * @param options an array of strings wit the text of the options.
 *                The lest element has to be "" (empty string)
 *                E.g. const char * opts[] = {"apple", "banana", "orange", ""};
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char ** options);

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt);

/**
 * Set a function to call when a new option is chosen
 * @param ddlist pointer to a drop down list
 * @param cb pointer to a call back function. Its prototype is:
 *           parameter 1: pointer to the drop down list
 *           parameter 2: id of the chosen item (0 ... number of options - 1)
 *           return LV_ACTION_RES_INV if the drop down list is deleted in the function else LV_ACTION_RES_OK
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_res_t (*cb)(lv_obj_t *, uint16_t));


/**
 * Set the auto size attribute. If enabled the height will reduced to be visible on the parent.
 * In this case the drop down list can be scrolled.
 * @param ddlist pointer to a drop down list
 * @param auto_size true: enable auto size, false: disable
 */
void lv_ddlist_set_auto_size(lv_obj_t * ddlist, bool auto_size);

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_ddlist_get_options(lv_obj_t * ddlist);

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(lv_obj_t * ddlist);

/**
 * Get the auto size attribute.
 * @param ddlist pointer to a drop down list
 * @return true: the auto_size is enabled, false: disabled
 */
bool lv_ddlist_get_auto_size(lv_obj_t * ddlist, bool auto_size);

/**
 * Return with a pointer to a built-in style and/or copy it to a variable
 * @param style a style name from lv_ddlists_builtin_t enum
 * @param copy copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_ddlists_t style
 */
lv_ddlists_t * lv_ddlists_get(lv_ddlists_builtin_t style, lv_ddlists_t * copy);

/**********************
 *      MACROS
 **********************/

#endif

#endif

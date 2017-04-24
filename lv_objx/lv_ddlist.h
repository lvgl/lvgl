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

/*Testing of dependencies*/
#if USE_LV_PAGE == 0
#error "lv_ddlist: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_ddlist: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

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
    lv_obj_t * opt_label;                           /*Label for the options*/
    lv_style_t * style_sel;                         /*Style of the selected option*/
    lv_action_t cb;                                 /*Pointer to function to call when an option is slected*/
    uint16_t sel_opt;                               /*Index of the current option*/
    uint8_t opened :1;                              /*1: The list is opened*/
    uint8_t auto_size :1;                           /*1: Set height to show all options. 0: Set height maximum to the parent bottom*/
}lv_ddlist_ext_t;


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
 * @param cb pointer to a call back function
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_t cb);

/**
 * Set the auto size attribute. If enabled the height will reduced to be visible on the parent.
 * In this case the drop down list can be scrolled.
 * @param ddlist pointer to a drop down list
 * @param auto_size true: enable auto size, false: disable
 */
void lv_ddlist_set_auto_size(lv_obj_t * ddlist, bool auto_size);

/**
 * Set the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @param style pointer the new style of the select rectangle
 */
void lv_dlist_set_style_select(lv_obj_t * ddlist, lv_style_t * style);

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
 * @param ddlist pointer to a drop down list object
 * @return true: the auto_size is enabled, false: disabled
 */
bool lv_ddlist_get_auto_size(lv_obj_t * ddlist, bool auto_size);

/**
 * Get the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @return pointer the style of the select rectangle
 */
lv_style_t * lv_dlist_get_style_select(lv_obj_t * ddlist);

/**********************
 *      MACROS
 **********************/

#endif

#endif

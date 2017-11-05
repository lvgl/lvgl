/**
 * @file lv_ddlist.h
 * 
 */

#ifndef LV_DDLIST_H
#define LV_DDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

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
    lv_obj_t * options_label;                           /*Label for the options*/
    lv_style_t * selected_style;                         /*Style of the selected option*/
    lv_action_t callback;                                 /*Pointer to function to call when an option is selected*/
    uint16_t option_cnt;                               /*Number of options*/
    uint16_t selected_option_id;                               /*Index of the current option*/
    uint16_t anim_time;                             /*Open/Close animation time [ms]*/
    uint8_t opened :1;                              /*1: The list is opened*/
    cord_t fix_height;                              /*Height if the ddlist is opened. (0: auto-size)*/
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
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char * options);

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt);

/**
 * Set a function to call when a new option is chosen
 * @param ddlist pointer to a drop down list
 * @param action pointer to a call back function
 */
void lv_ddlist_set_action(lv_obj_t * ddlist, lv_action_t action);

/**
 * Set the fix height value.
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, cord_t h);

/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
void lv_ddlist_set_anim_time(lv_obj_t * ddlist, uint16_t anim_time);

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim true: use animation; false: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, bool anim);

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim true: use animation; false: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, bool anim);

/**
 * Set the style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param bg pointer to the new style of the background
 * @param sb pointer to the new style of the scrollbars (only visible with fix height)
 * @param sel pointer to the new style of the select rectangle
 */
void lv_ddlist_set_style(lv_obj_t * ddlist, lv_style_t * bg, lv_style_t * sb, lv_style_t * sel);

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
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 */
void lv_ddlist_get_selected_str(lv_obj_t * ddlist, char * buf);

/**
 * Get the "option selected" callback function
 * @param ddlist pointer to a drop down list
 * @return  pointer to the call back function
 */
lv_action_t lv_ddlist_get_action(lv_obj_t * ddlist);

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
cord_t lv_ddlist_get_fix_height(lv_obj_t * ddlist);

/**
 * Get the style of the rectangle on the selected option
 * @param ddlist pointer to a drop down list object
 * @return pointer the style of the select rectangle
 */
lv_style_t * lv_ddlist_get_style_select(lv_obj_t * ddlist);

/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
uint16_t lv_ddlist_get_anim_time(lv_obj_t * ddlist);


/****************************
 * TRANSPARENT API FUNCTIONS
 ***************************/

/**
 * Set the scroll bar mode of a drop down list
 * @param ta pointer to a drop down list object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_ddlist_set_sb_mode(lv_obj_t * ta, lv_page_sb_mode_t mode)
{
    lv_page_set_sb_mode(ta, mode);
}

/**
 * Get the scroll bar mode of a drop down list
 * @param ddlist pointer to a  drop down list object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_page_sb_mode_t lv_ddlist_get_sb_mode(lv_obj_t * ta)
{
    return lv_page_get_sb_mode(ta);
}

/**
* Get the style of the drop down list background
* @param ddlist pointer to a drop down list object
* @return pointer to the style of the background
*/
static inline lv_style_t * lv_ddlist_get_style_bg(lv_obj_t * ddlist)
{
    return lv_page_get_style_bg(ddlist);
}

/**
* Get the style of the scrollbars of a drop down list
* @param ddlist pointer to a text area object
* @return pointer to the style of the scrollbars
*/
static inline lv_style_t * lv_ddlist_get_style_sb(lv_obj_t * ddlist)
{
    return lv_page_get_style_sb(ddlist);
}


/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_DDLIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DDLIST_H*/

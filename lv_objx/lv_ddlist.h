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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if USE_LV_DDLIST != 0

/*Testing of dependencies*/
#if USE_LV_PAGE == 0
#error "lv_ddlist: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#if USE_LV_LABEL == 0
#error "lv_ddlist: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
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
    lv_obj_t *label;                     /*Label for the options*/
    lv_style_t * sel_style;              /*Style of the selected option*/
    lv_action_t action;                  /*Pointer to function to call when an option is selected*/
    uint16_t option_cnt;                 /*Number of options*/
    uint16_t sel_opt_id;                 /*Index of the current option*/
    uint16_t sel_opt_id_ori;             /*Store the original index on focus*/
    uint16_t anim_time;                  /*Open/Close animation time [ms]*/
    uint8_t opened :1;                   /*1: The list is opened (handled by the library)*/
    lv_coord_t fix_height;               /*Height of the ddlist when opened. (0: auto-size)*/
} lv_ddlist_ext_t;

enum {
    LV_DDLIST_STYLE_BG,
    LV_DDLIST_STYLE_SEL,
    LV_DDLIST_STYLE_SB,
};
typedef uint8_t lv_ddlist_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_ddlist_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

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
 * Set the fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, lv_coord_t h);

/**
 * Enable or disable the horizontal fit to the content
 * @param ddlist pointer to a drop down list
 * @param fit en true: enable auto fit; false: disable auto fit
 */
void lv_ddlist_set_hor_fit(lv_obj_t * ddlist, bool fit_en);

/**
 * Set the scroll bar mode of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_ddlist_set_sb_mode(lv_obj_t * ddlist, lv_sb_mode_t mode)
{
    lv_page_set_sb_mode(ddlist, mode);
}

/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
void lv_ddlist_set_anim_time(lv_obj_t * ddlist, uint16_t anim_time);


/**
 * Set a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_ddlist_set_style(lv_obj_t *ddlist, lv_ddlist_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_ddlist_get_options(const lv_obj_t * ddlist);

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(const lv_obj_t * ddlist);

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 */
void lv_ddlist_get_selected_str(const lv_obj_t * ddlist, char * buf);

/**
 * Get the "option selected" callback function
 * @param ddlist pointer to a drop down list
 * @return  pointer to the call back function
 */
lv_action_t lv_ddlist_get_action(const lv_obj_t * ddlist);

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_ddlist_get_fix_height(const lv_obj_t * ddlist);

/**
 * Get the scroll bar mode of a drop down list
 * @param ddlist pointer to a  drop down list object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_sb_mode_t lv_ddlist_get_sb_mode(const lv_obj_t * ddlist)
{
    return lv_page_get_sb_mode(ddlist);
}

/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
uint16_t lv_ddlist_get_anim_time(const lv_obj_t * ddlist);

/**
 * Get a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * lv_ddlist_get_style(const lv_obj_t *ddlist, lv_ddlist_style_t type);

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en true: use animation; false: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, bool anim);

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim true: use animation; false: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, bool anim);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_LV_DDLIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_DDLIST_H*/

/**
 * @file lv_group.h
 *
 */

#ifndef LV_GROUP_H
#define LV_GROUP_H

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

#include "lv_obj.h"

/*********************
 *      DEFINES
 *********************/
/*Predefined keys to control the focused object via lv_group_send(group, c)*/
/*For compatibility in signal function define the keys regardless to LV_GROUP*/
#define LV_GROUP_KEY_UP             17      /*0x11*/
#define LV_GROUP_KEY_DOWN           18      /*0x12*/
#define LV_GROUP_KEY_RIGHT          19      /*0x13*/
#define LV_GROUP_KEY_LEFT           20      /*0x14*/
#define LV_GROUP_KEY_ESC            27      /*0x1B*/
#define LV_GROUP_KEY_DEL            28      /*0x1C*/
#define LV_GROUP_KEY_ENTER          10      /*0x0A, '\n'*/
#define LV_GROUP_KEY_NEXT           9       /*0x09, '\t'*/
#define LV_GROUP_KEY_PREV           11      /*0x0B, '*/

#if USE_LV_GROUP  != 0
/**********************
 *      TYPEDEFS
 **********************/
struct _lv_group_t;

typedef void (*lv_group_style_mod_func_t)(lv_style_t *);
typedef void (*lv_group_focus_cb_t)(struct _lv_group_t *);

typedef struct _lv_group_t
{
    lv_ll_t obj_ll;                         /*Linked list to store the objects in the group */
    lv_obj_t ** obj_focus;                  /*The object in focus*/
    lv_group_style_mod_func_t style_mod;    /*A function which modifies the style of the focused object*/
    lv_group_style_mod_func_t style_mod_edit;/*A function which modifies the style of the focused object*/
    lv_group_focus_cb_t focus_cb;           /*A function to call when a new object is focused (optional)*/
    lv_style_t style_tmp;                   /*Stores the modified style of the focused object */
    uint8_t frozen          :1;             /*1: can't focus to new object*/
    uint8_t editing         :1;             /*1: Edit mode, 0: Navigate mode*/
    uint8_t click_focus     :1;             /*1: If an object in a group is clicked by an indev then it will be focused */
} lv_group_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a new object group
 * @return pointer to the new object group
 */
lv_group_t * lv_group_create(void);

/**
 * Delete a group object
 * @param group pointer to a group
 */
void lv_group_del(lv_group_t * group);

/**
 * Add an object to a group
 * @param group pointer to a group
 * @param obj pointer to an object to add
 */
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj);

/**
 * Remove an object from its group
 * @param obj pointer to an object to remove
 */
void lv_group_remove_obj(lv_obj_t * obj);

/**
 * Focus on an object (defocus the current)
 * @param obj pointer to an object to focus on
 */
void lv_group_focus_obj(lv_obj_t * obj);

/**
 * Focus the next object in a group (defocus the current)
 * @param group pointer to a group
 */
void lv_group_focus_next(lv_group_t * group);

/**
 * Focus the previous object in a group (defocus the current)
 * @param group pointer to a group
 */
void lv_group_focus_prev(lv_group_t * group);

/**
 * Do not let to change the focus from the current object
 * @param group pointer to a group
 * @param en true: freeze, false: release freezing (normal mode)
 */
void lv_group_focus_freeze(lv_group_t * group, bool en);

/**
 * Send a control character to the focuses object of a group
 * @param group pointer to a group
 * @param c a character (use LV_GROUP_KEY_.. to navigate)
 */
void lv_group_send_data(lv_group_t * group, uint32_t c);

/**
 * Set a function for a group which will modify the object's style if it is in focus
 * @param group pointer to a group
 * @param style_mod_func the style modifier function pointer
 */
void lv_group_set_style_mod_cb(lv_group_t * group, lv_group_style_mod_func_t style_mod_func);

/**
 * Set a function for a group which will modify the object's style if it is in focus in edit mode
 * @param group pointer to a group
 * @param style_mod_func the style modifier function pointer
 */
void lv_group_set_style_mod_edit_cb(lv_group_t * group, lv_group_style_mod_func_t style_mod_func);

/**
 * Set a function for a group which will be called when a new object is focused
 * @param group pointer to a group
 * @param focus_cb the call back function or NULL if unused
 */
void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb);

/**
 * Manually set the current mode (edit or navigate).
 * @param group pointer to group
 * @param edit: true: edit mode; false: navigate mode
 */
void lv_group_set_editing(lv_group_t * group, bool edit);

/**
 * Set the `click_focus` attribute. If enabled then the object will be focused then it is clicked.
 * @param group pointer to group
 * @param en: true: enable `click_focus`
 */
void lv_group_set_click_focus(lv_group_t * group, bool en);

/**
 * Modify a style with the set 'style_mod' function. The input style remains unchanged.
 * @param group pointer to group
 * @param style pointer to a style to modify
 * @return a copy of the input style but modified with the 'style_mod' function
 */
lv_style_t * lv_group_mod_style(lv_group_t * group, const lv_style_t * style);

/**
 * Get the focused object or NULL if there isn't one
 * @param group pointer to a group
 * @return pointer to the focused object
 */
lv_obj_t * lv_group_get_focused(const lv_group_t * group);

/**
 * Get a the style modifier function of a group
 * @param group pointer to a group
 * @return pointer to the style modifier function
 */
lv_group_style_mod_func_t lv_group_get_style_mod_cb(const lv_group_t * group);

/**
 * Get a the style modifier function of a group in edit mode
 * @param group pointer to a group
 * @return pointer to the style modifier function
 */
lv_group_style_mod_func_t lv_group_get_style_mod_edit_cb(const lv_group_t * group);

/**
 * Get the focus callback function of a group
 * @param group pointer to a group
 * @return the call back function or NULL if not set
 */
lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t * group);

/**
 * Get the current mode (edit or navigate).
 * @param group pointer to group
 * @return true: edit mode; false: navigate mode
 */
bool lv_group_get_editing(const lv_group_t * group);

/**
 * Get the `click_focus` attribute.
 * @param group pointer to group
 * @return true: `click_focus` is enabled; false: disabled
 */
bool lv_group_get_click_focus(const lv_group_t * group);


/**********************
 *      MACROS
 **********************/

#endif /*USE_LV_GROUP != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_GROUP_H*/

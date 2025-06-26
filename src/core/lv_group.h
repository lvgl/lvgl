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
#include "../lv_conf_internal.h"

#include "../misc/lv_types.h"
#include "../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/
/** Predefined keys to control which Widget has focus via lv_group_send(group, c) */
typedef enum {
    LV_KEY_UP        = 17,  /*0x11*/
    LV_KEY_DOWN      = 18,  /*0x12*/
    LV_KEY_RIGHT     = 19,  /*0x13*/
    LV_KEY_LEFT      = 20,  /*0x14*/
    LV_KEY_ESC       = 27,  /*0x1B*/
    LV_KEY_DEL       = 127, /*0x7F*/
    LV_KEY_BACKSPACE = 8,   /*0x08*/
    LV_KEY_ENTER     = 10,  /*0x0A, '\n'*/
    LV_KEY_NEXT      = 9,   /*0x09, '\t'*/
    LV_KEY_PREV      = 11,  /*0x0B, '*/
    LV_KEY_HOME      = 2,   /*0x02, STX*/
    LV_KEY_END       = 3,   /*0x03, ETX*/
} lv_key_t;

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*lv_group_focus_cb_t)(lv_group_t *);
typedef void (*lv_group_edge_cb_t)(lv_group_t *, bool);

typedef enum {
    LV_GROUP_REFOCUS_POLICY_NEXT = 0,
    LV_GROUP_REFOCUS_POLICY_PREV = 1
} lv_group_refocus_policy_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create new Widget group.
 * @return          pointer to the new Widget group
 */
lv_group_t * lv_group_create(void);

/**
 * Delete group object.
 * @param group     pointer to a group
 */
void lv_group_delete(lv_group_t * group);

/**
 * Set default group. New Widgets will be added to this group if it's enabled in
 * their class with `add_to_def_group = true`.
 * @param group     pointer to a group (can be `NULL`)
 */
void lv_group_set_default(lv_group_t * group);

/**
 * Get default group.
 * @return          pointer to the default group
 */
lv_group_t * lv_group_get_default(void);

/**
 * Add an Widget to group.
 * @param group     pointer to a group
 * @param obj       pointer to a Widget to add
 */
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj);

/**
 * Swap 2 Widgets in group.  Widgets must be in the same group.
 * @param obj1  pointer to a Widget
 * @param obj2  pointer to another Widget
 */
void lv_group_swap_obj(lv_obj_t * obj1, lv_obj_t * obj2);

/**
 * Remove a Widget from its group.
 * @param obj       pointer to Widget to remove
 */
void lv_group_remove_obj(lv_obj_t * obj);

/**
 * Remove all Widgets from a group.
 * @param group     pointer to a group
 */
void lv_group_remove_all_objs(lv_group_t * group);

/**
 * Focus on a Widget (defocus the current).
 * @param obj       pointer to Widget to focus on
 */
void lv_group_focus_obj(lv_obj_t * obj);

/**
 * Focus on next Widget in a group (defocus the current).
 * @param group     pointer to a group
 */
void lv_group_focus_next(lv_group_t * group);

/**
 * Focus on previous Widget in a group (defocus the current).
 * @param group     pointer to a group
 */
void lv_group_focus_prev(lv_group_t * group);

/**
 * Do not allow changing focus from current Widget.
 * @param group     pointer to a group
 * @param en        true: freeze, false: release freezing (normal mode)
 */
void lv_group_focus_freeze(lv_group_t * group, bool en);

/**
 * Send a control character to Widget that has focus in a group.
 * @param group     pointer to a group
 * @param c         a character (use LV_KEY_.. to navigate)
 * @return          result of Widget with focus in group.
 */
lv_result_t lv_group_send_data(lv_group_t * group, uint32_t c);

/**
 * Set a function for a group which will be called when a new Widget has focus.
 * @param group         pointer to a group
 * @param focus_cb      the call back function or NULL if unused
 */
void lv_group_set_focus_cb(lv_group_t * group, lv_group_focus_cb_t focus_cb);

/**
 * Set a function for a group which will be called when a focus edge is reached
 * @param group         pointer to a group
 * @param edge_cb      the call back function or NULL if unused
 */
void lv_group_set_edge_cb(lv_group_t * group, lv_group_edge_cb_t edge_cb);

/**
 * Set whether the next or previous Widget in a group gets focus when Widget that has
 * focus is deleted.
 * @param group         pointer to a group
 * @param policy        new refocus policy enum
 */
void lv_group_set_refocus_policy(lv_group_t * group, lv_group_refocus_policy_t policy);

/**
 * Manually set the current mode (edit or navigate).
 * @param group         pointer to group
 * @param edit          true: edit mode; false: navigate mode
 */
void lv_group_set_editing(lv_group_t * group, bool edit);

/**
 * Set whether moving focus to next/previous Widget will allow wrapping from
 * first->last or last->first Widget.
 * @param group         pointer to group
 * @param               en true: wrapping enabled; false: wrapping disabled
 */
void lv_group_set_wrap(lv_group_t * group, bool en);

/**
 * Get Widget that has focus, or NULL if there isn't one.
 * @param group         pointer to a group
 * @return              pointer to Widget with focus
 */
lv_obj_t * lv_group_get_focused(const lv_group_t * group);

/**
 * Get focus callback function of a group.
 * @param group pointer to a group
 * @return the call back function or NULL if not set
 */
lv_group_focus_cb_t lv_group_get_focus_cb(const lv_group_t * group);

/**
 * Get edge callback function of a group.
 * @param group pointer to a group
 * @return the call back function or NULL if not set
 */
lv_group_edge_cb_t lv_group_get_edge_cb(const lv_group_t * group);

/**
 * Get current mode (edit or navigate).
 * @param group         pointer to group
 * @return              true: edit mode; false: navigate mode
 */
bool lv_group_get_editing(const lv_group_t * group);

/**
 * Get whether moving focus to next/previous Widget will allow wrapping from
 * first->last or last->first Widget.
 * @param group         pointer to group
 */
bool lv_group_get_wrap(lv_group_t * group);

/**
 * Get number of Widgets in group.
 * @param group         pointer to a group
 * @return              number of Widgets in the group
 */
uint32_t lv_group_get_obj_count(lv_group_t * group);

/**
 * Get nth Widget within group.
 * @param group         pointer to a group
 * @param index         index of Widget within the group
 * @return              pointer to Widget
 */
lv_obj_t * lv_group_get_obj_by_index(lv_group_t * group, uint32_t index);

/**
 * Get the number of groups.
 * @return              number of groups
 */
uint32_t lv_group_get_count(void);

/**
 * Get a group by its index.
 * @param index         index of the group
 * @return              pointer to the group
 */
lv_group_t  * lv_group_by_index(uint32_t index);

#if LV_USE_EXT_DATA
/**
 * @brief Attaches external user data and destructor callback to a group
 *
 * Associates custom user data with an LVGL group and specifies a destructor function
 * that will be automatically invoked when the group is deleted to properly clean up
 * the associated resources.
 *
 * @param group      Pointer to a group
 * @param data       User-defined data pointer to associate with a group
 * @param free_cb    Callback function for cleaning up ext_data when group is deleted.
 *                   Receives ext_data as parameter. NULL means no cleanup required.
 */
void lv_group_set_external_data(lv_group_t * group, void * data, void (* free_cb)(void * data));
#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GROUP_H*/

/**
 * @file lv_obj_private.h
 *
 */

#ifndef LV_OBJ_PRIVATE_H
#define LV_OBJ_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lv_obj.h"

#if LV_USE_EXT_DATA
#include "../lvgl_private.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Special, rarely used attributes.
 * They are allocated automatically if any elements is set.
 */
struct _lv_obj_spec_attr_t {
    lv_obj_t ** children;           /**< Store the pointer of the children in an array.*/
    lv_group_t * group_p;
#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_t * matrix;           /**< The transform matrix*/
#endif
    lv_event_list_t event_list;
#if LV_USE_OBJ_NAME
    const char * name;              /**< Pointer to the name */
#endif
    lv_point_t scroll;              /**< The current X/Y scroll offset*/

    int32_t ext_click_pad;          /**< Extra click padding in all direction*/
    int32_t ext_draw_size;          /**< EXTend the size in every direction for drawing.*/

    uint32_t child_cnt;             /**< Number of children*/
    uint32_t scrollbar_mode : 2;    /**< How to display scrollbars, see `lv_scrollbar_mode_t`*/
    uint32_t scroll_snap_x : 2;     /**< Where to align the snappable children horizontally, see `lv_scroll_snap_t`*/
    uint32_t scroll_snap_y : 2;     /**< Where to align the snappable children vertically*/
    uint32_t scroll_dir : 4;        /**< The allowed scroll direction(s), see `lv_dir_t`*/
    uint32_t layer_type : 2;        /**< Cache the layer type here. Element of lv_intermediate_layer_type_t */
    uint32_t name_static : 1;       /**< 1: `name` was not dynamically allocated */
    uint32_t user_bits : 8;         /**< Store custom flags */
};

struct _lv_obj_t {
#if LV_USE_EXT_DATA
    lv_ext_data_t ext_data;
#endif
    const lv_obj_class_t * class_p;
    lv_obj_t * parent;
    lv_obj_spec_attr_t * spec_attr;
    lv_obj_style_t * styles;
#if LV_OBJ_STYLE_CACHE
    uint32_t style_main_prop_is_set;
    uint32_t style_other_prop_is_set;
#endif
    void * user_data;
#if LV_USE_OBJ_ID
    void * id;
#endif
    lv_area_t coords;
    /** Make the object hidden. (Like it wasn't there at all) */
    uint32_t hidden : 1;

    /** Make the object clickable by the input devices */
    uint32_t clickable : 1;

    /** Add focused state to the object when clicked */
    uint32_t click_focusable : 1;

    /** Toggle checked state when the object is clicked */
    uint32_t checkable : 1;

    /** Make the object scrollable */
    uint32_t scrollable : 1;

    /** Allow scrolling inside but with slower speed */
    uint32_t scroll_elastic : 1;

    /** Make the object scroll further when "thrown" */
    uint32_t scroll_momentum : 1;

    /** Allow scrolling only one snappable child */
    uint32_t scroll_one : 1;

    /** Allow propagating the horizontal scroll to a parent */
    uint32_t scroll_chain_hor : 1;

    /** Allow propagating the vertical scroll to a parent */
    uint32_t scroll_chain_ver : 1;

    /** Automatically scroll object to make it visible when focused */
    uint32_t scroll_on_focus : 1;

    /** Allow scrolling the focused object with arrow keys */
    uint32_t scroll_with_arrow : 1;

    /** If scroll snap is enabled on the parent it can snap to this object */
    uint32_t snappable : 1;

    /** Keep the object pressed even if the press slid from the object */
    uint32_t press_lock : 1;

    /** Propagate the events to the parent too */
    uint32_t event_bubble : 1;

    /** Propagate the gestures to the parent */
    uint32_t gesture_bubble : 1;

    /** Allow performing more accurate hit (click) test */
    uint32_t adv_hittest : 1;

    /** Make the object not positioned by the layouts */
    uint32_t ignore_layout : 1;

    /** Do not scroll the object when the parent scrolls and ignore layout */
    uint32_t floating : 1;

    /** Send LV_EVENT_DRAW_TASK_ADDED events */
    uint32_t send_draw_task_events : 1;

    /** Do not clip the children to the parent's ext draw size */
    uint32_t overflow_visible : 1;

    /** Propagate the events to the children too */
    uint32_t event_trickle : 1;

    /** Propagate the states to the children too */
    uint32_t state_trickle : 1;

    /** Allow only one RADIO_BUTTON sibling to be checked */
    uint32_t radio_button : 1;

    /** Start a new flex track on this item */
    uint32_t flex_in_new_track : 1;

    uint16_t state;
    uint16_t layout_inv : 1;
    uint16_t readjust_scroll_after_layout : 1;
    uint16_t scr_layout_inv : 1;
    uint16_t skip_trans : 1;
    uint16_t style_cnt  : 6;
    uint16_t h_layout   : 1;
    uint16_t w_layout   : 1;
    uint16_t h_ignore_size : 1; /* ignore this obj when calculating content height of parent */
    uint16_t w_ignore_size : 1; /* ignore this obj when calculating content width of parent */
    uint16_t is_deleting : 1;

    /** The widget is rendered at least once already.
     * It's used to skip initial animations and transitions. */
    uint16_t rendered : 1;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_PRIVATE_H*/

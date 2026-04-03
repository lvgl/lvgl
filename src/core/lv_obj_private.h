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

#include "../lvgl_public.h"
#include "../misc/lv_event_private.h"

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
typedef struct _lv_obj_spec_attr_t {
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

    uint16_t child_cnt;             /**< Number of children*/
    uint16_t scrollbar_mode : 2;    /**< How to display scrollbars, see `lv_scrollbar_mode_t`*/
    uint16_t scroll_snap_x : 2;     /**< Where to align the snappable children horizontally, see `lv_scroll_snap_t`*/
    uint16_t scroll_snap_y : 2;     /**< Where to align the snappable children vertically*/
    uint16_t scroll_dir : 4;        /**< The allowed scroll direction(s), see `lv_dir_t`*/
    uint16_t layer_type : 2;        /**< Cache the layer type here. Element of lv_intermediate_layer_type_t */
    uint16_t name_static : 1;       /**< 1: `name` was not dynamically allocated */
    uint16_t user_flags : 8;         /**< Store custom flags */
} lv_obj_spec_attr_t;


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

    /**
     * Stores the ORed state of the widget, like `LV_STATE_PRESSED`, `LV_STATE_CHECKED`
     */
    uint16_t state;


    /**
     * Shows that the coordinates of the widget or its children needs to be recalculated
     * when X, Y, width height, layout other related property changes.
     */
    uint16_t coords_invalid : 1;

    /**
     * When `coords_invalid` is set this flag's the layout engine to look into this object and its children.
     * If this flag is not set the entire subtree will be skipped.
     */
    uint16_t update_children_coords: 1;

    /**
     * When a child's coordinate changes this flag is set on the parent so that the parent will know to
     * that is has to update its CONTENT size or layouts (if they set)
     */
    uint16_t child_coords_changed: 1;

    /**
     * Tells that during the layout calculation the widget's size changed further layout
     * calculation might apply and `LV_EVENT_SIZE_CHANGLED` will be sent
     */
    uint16_t size_changed   : 1;

    /**
     *If the widget was scrolled to the end and due to a layout change
     *the content got smaller make sure that the widget is scrolled inside
     *and jump back to end if needed.*/
    uint16_t readjust_scroll_after_layout : 1;
    uint16_t skip_trans : 1;
    uint16_t style_cnt  : 6;

    /**
     * The height of the widget is controlled by a layout. Its value is valid only inside
     * `lv_obj_update_layout()` */
    uint16_t h_layout_controlled   : 1;

    /**
     * The width of the widget is controlled by a layout. Its value is valid only inside
     * `lv_obj_update_layout()` */
    uint16_t w_layout_controlled   : 1;

    /**
     * The height of the widget is `LV_SIZE_CONTENT` and is not controlled by a layout, so it
     * is still unknown: it is set once the children are measured. Until then a child's
     * percentage height counts as 0, because there is nothing to take a percentage of.
     * Cleared when the height is settled. Valid only inside `lv_obj_update_layout()` */
    uint16_t h_content_pending  : 1;

    /** The same for the width */
    uint16_t w_content_pending  : 1;
    uint16_t is_deleting : 1;

    /** The widget is rendered at least once already.
     * It's used to skip initial animations and transitions. */
    uint16_t rendered : 1;

    /** The widget has blur or a drop shadow in its current state, so a change
     * behind it must invalidate its full extent. It's updated by
     * lv_obj_update_blur_status() and counted in blur_obj_cnt in lv_global_t. */
    uint16_t has_blur : 1;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Allocate special data for an object if not allocated yet.
 * @param obj   pointer to an object
 * @return the spec_attr created or NULL if something went wrong
 */
lv_obj_spec_attr_t * lv_obj_allocate_spec_attr(lv_obj_t * obj);

lv_result_t lv_obj_add_child(lv_obj_t * parent, lv_obj_t * child);
void lv_obj_remove_child(lv_obj_t * parent, lv_obj_t * child);

/**
 * Expand the display's invalidated areas to cover blur objects whose background
 * changed. It's called once per frame and invalidates the full extent of every
 * blur object that overlaps an invalidated area, repeating the walk until a
 * pass adds no new areas to cover transitive blur-over-blur cases.
 * Does nothing while no widget has blur (blur_obj_cnt in lv_global_t is zero).
 * @param disp  pointer to a display
 */
void lv_obj_invalidate_expand_blur(lv_display_t * disp);

/**
 * @brief Calculates the width in pixels of an LVGL object based on its style and parent for a given width `prop`.
 * @param obj Pointer to the LVGL object whose width is being calculated.
 * @param prop Which style width to calculate for. Valid values are: LV_STYLE_WIDTH, LV_STYLE_MIN_WIDTH, or
 * LV_STYLE_MAX_WIDTH.
 * @return The computed width for the object:
 * @note If the style width is a fixed value, that value is returned.
 * @note If the style width is `LV_SIZE_CONTENT`, `LV_SIZE_CONTENT` is returned for `LV_STYLE_WIDTH`.
 *       A min or max width does not support it and returns no limit.
 * @note If the style width is a `LV_PCT()`, the percentage is applied to the parent's width.
 *       A widget without a parent returns no limit for a min or max width, and 0 otherwise.
 */
int32_t lv_obj_calc_dynamic_width(lv_obj_t * obj, lv_style_prop_t prop);

/**
 * @brief Calculates the height in pixels of an LVGL object based on its style and parent for a given height `prop`.
 * @param obj Pointer to the LVGL object whose height is being calculated.
 * @param prop Which style height to calculate for. Valid values are: LV_STYLE_HEIGHT, LV_STYLE_MIN_HEIGHT, or
 * LV_STYLE_MAX_HEIGHT.
 * @return The computed height for the object:
 * @note If the style height is a fixed value, that value is returned.
 * @note If the style height is `LV_SIZE_CONTENT`, `LV_SIZE_CONTENT` is returned for `LV_STYLE_HEIGHT`.
 *       A min or max height does not support it and returns no limit.
 * @note If the style height is a `LV_PCT()`, the percentage is applied to the parent's height.
 *       A widget without a parent returns no limit for a min or max height, and 0 otherwise.
 */
int32_t lv_obj_calc_dynamic_height(lv_obj_t * obj, lv_style_prop_t prop);

/**
 * Move the coordinates of all the descendants of a widget. Only the coordinates change,
 * nothing is invalidated and no event is sent.
 * @param obj               pointer to a widget
 * @param x_diff            pixels to move horizontally
 * @param y_diff            pixels to move vertically
 * @param ignore_floating   true: don't move the floating children (and their descendants)
 */
void lv_obj_move_children_by(lv_obj_t * obj, int32_t x_diff, int32_t y_diff, bool ignore_floating);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_PRIVATE_H*/

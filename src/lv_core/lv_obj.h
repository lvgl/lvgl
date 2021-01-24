/**
 * @file lv_obj.h
 *
 */

#ifndef LV_OBJ_H
#define LV_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#include <stddef.h>
#include <stdbool.h>
#include "../lv_misc/lv_style.h"
#include "../lv_misc/lv_types.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_misc/lv_class.h"
#include "../lv_hal/lv_hal.h"
#include "../lv_draw/lv_draw_rect.h"
#include "../lv_draw/lv_draw_label.h"
#include "../lv_draw/lv_draw_line.h"
#include "../lv_draw/lv_draw_img.h"

/*********************
 *      DEFINES
 *********************/

/*Error check of lv_conf.h*/
#if LV_HOR_RES_MAX == 0 || LV_VER_RES_MAX == 0
#error "LVGL: LV_HOR_RES_MAX and LV_VER_RES_MAX must be greater than 0"
#endif

#if LV_ANTIALIAS > 1
#error "LVGL: LV_ANTIALIAS can be only 0 or 1"
#endif

#define LV_MAX_ANCESTOR_NUM 8
#define _LV_OBJ_PART_MAX    32

#define LV_EXT_CLICK_AREA_OFF   0
#define LV_EXT_CLICK_AREA_TINY  1
#define LV_EXT_CLICK_AREA_FULL  2

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;


enum {
    LV_EVENT_PRESSED,             /**< The object has been pressed*/
    LV_EVENT_PRESSING,            /**< The object is being pressed (called continuously while pressing)*/
    LV_EVENT_PRESS_LOST,          /**< User is still pressing but slid cursor/finger off of the object */
    LV_EVENT_SHORT_CLICKED,       /**< User pressed object for a short period of time, then released it. Not called if scrolled. */
    LV_EVENT_LONG_PRESSED,        /**< Object has been pressed for at least `LV_INDEV_LONG_PRESS_TIME`.  Not called if scrolled.*/
    LV_EVENT_LONG_PRESSED_REPEAT, /**< Called after `LV_INDEV_LONG_PRESS_TIME` in every
                                       `LV_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if scrolled.*/
    LV_EVENT_CLICKED,             /**< Called on release if not scrolled (regardless to long press)*/
    LV_EVENT_RELEASED,            /**< Called in every cases when the object has been released*/
    LV_EVENT_SCROLL_BEGIN,
    LV_EVENT_SCROLL_END,
    LV_EVENT_SCROLL,
    LV_EVENT_GESTURE,           /**< The object has been gesture*/
    LV_EVENT_KEY,
    LV_EVENT_FOCUSED,
    LV_EVENT_DEFOCUSED,
    LV_EVENT_LEAVE,
    LV_EVENT_VALUE_CHANGED,      /**< The object's value has changed (i.e. slider moved) */
    LV_EVENT_INSERT,
    LV_EVENT_REFRESH,
    LV_EVENT_APPLY,  /**< "Ok", "Apply" or similar specific button has clicked*/
    LV_EVENT_CANCEL, /**< "Close", "Cancel" or similar specific button has clicked*/
    LV_EVENT_DELETE, /**< Object is being deleted */

    LV_EVENT_COVER_CHECK,      /**< Check if the object fully covers the 'mask_p' area */
    LV_EVENT_REFR_EXT_SIZE,   /**< Draw extras on the object */

    LV_EVENT_DRAW_MAIN_BEGIN,
    LV_EVENT_DRAW_MAIN_FINISH,
    LV_EVENT_DRAW_POST_BEGIN,
    LV_EVENT_DRAW_POST_END,
    LV_EVENT_DRAW_PART_BEGIN,
    LV_EVENT_DRAW_PART_END,

    _LV_EVENT_LAST /** Number of events*/
};
typedef uint8_t lv_event_t; /**< Type of event being sent to the object. */

/**
 * @brief Event callback.
 * Events are used to notify the user of some action being taken on the object.
 * For details, see ::lv_event_t.
 */
typedef void (*lv_event_cb_t)(struct _lv_obj_t * obj, lv_event_t event);

/** Signals are for use by the object itself or to extend the object's functionality.
  * Applications should use ::lv_obj_set_event_cb to be notified of events that occur
  * on the object. */
enum {
    /*General signals*/
    LV_SIGNAL_CHILD_CHG,         /**< Child was removed/added */
    LV_SIGNAL_COORD_CHG,         /**< Object coordinates/size have changed */
    LV_SIGNAL_STYLE_CHG,         /**< Object's style has changed */
    LV_SIGNAL_BASE_DIR_CHG,      /**< The base dir has changed*/
    LV_SIGNAL_REFR_EXT_DRAW_SIZE, /**< Object's extra padding has changed */
    LV_SIGNAL_GET_SELF_SIZE,     /**< Get the internal size of a widget*/

    /*Input device related*/
    LV_SIGNAL_HIT_TEST,          /**< Advanced hit-testing */
    LV_SIGNAL_PRESSED,           /**< The object has been pressed*/
    LV_SIGNAL_PRESSING,          /**< The object is being pressed (called continuously while pressing)*/
    LV_SIGNAL_PRESS_LOST,        /**< User is still pressing but slid cursor/finger off of the object */
    LV_SIGNAL_RELEASED,          /**< User pressed object for a short period of time, then released it. Not called if scrolled. */
    LV_SIGNAL_LONG_PRESS,        /**< Object has been pressed for at least `LV_INDEV_LONG_PRESS_TIME`.  Not called if scrolled.*/
    LV_SIGNAL_LONG_PRESS_REP,    /**< Called after `LV_INDEV_LONG_PRESS_TIME` in every `LV_INDEV_LONG_PRESS_REP_TIME` ms.  Not called if scrolled.*/
    LV_SIGNAL_SCROLL_BEGIN,      /**< The scrolling has just begun  */
    LV_SIGNAL_SCROLL,            /**< The object has been scrolled */
    LV_SIGNAL_SCROLL_END,        /**< The scrolling has ended */
    LV_SIGNAL_GESTURE,          /**< The object has been gesture*/
    LV_SIGNAL_LEAVE,            /**< Another object is clicked or chosen via an input device */

    /*Group related*/
    LV_SIGNAL_FOCUS,
    LV_SIGNAL_DEFOCUS,
    LV_SIGNAL_CONTROL,
    LV_SIGNAL_GET_EDITABLE,
};
typedef uint8_t lv_signal_t;

typedef lv_res_t (*lv_signal_cb_t)(struct _lv_obj_t * obj, lv_signal_t sign, void * param);


enum {
    LV_STATE_DEFAULT  =  0x00,
    LV_STATE_CHECKED  =  0x01,
    LV_STATE_FOCUSED  =  0x02,
    LV_STATE_EDITED   =  0x04,
    LV_STATE_HOVERED  =  0x08,
    LV_STATE_PRESSED  =  0x10,
    LV_STATE_SCROLLED =  0x20,
    LV_STATE_DISABLED =  0x40,
    LV_STATE_USER     =  0x80,  /** Free to use by the user */
};

typedef uint8_t lv_state_t;

enum {
    LV_OBJ_FLAG_HIDDEN          = (1 << 0),
    LV_OBJ_FLAG_CLICKABLE       = (1 << 1),
    LV_OBJ_FLAG_CLICK_FOCUSABLE = (1 << 2),
    LV_OBJ_FLAG_CHECKABLE       = (1 << 3),
    LV_OBJ_FLAG_SCROLLABLE      = (1 << 4),
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1 << 5),
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1 << 6),
    LV_OBJ_FLAG_SCROLL_STOP     = (1 << 7),
    LV_OBJ_FLAG_SCROLL_CHAIN    = (1 << 8), /** Allow propagating the scroll to a parent */
    LV_OBJ_FLAG_SCROLL_ON_FOCUS = (1 << 9), /** Automatically scroll the focused object's ancestors to make the focused object visible*/
    LV_OBJ_FLAG_SNAPABLE        = (1 << 10),
    LV_OBJ_FLAG_PRESS_LOCK      = (1 << 11),
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1 << 12),
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1 << 13),
    LV_OBJ_FLAG_FOCUS_BUBBLE    = (1 << 14),
    LV_OBJ_FLAG_ADV_HITTEST     = (1 << 15),
    LV_OBJ_FLAG_LAYOUTABLE      = (1 << 16),
    LV_OBJ_FLAG_LAYOUT_1        = (1 << 24), /** Custom flag, free to use by layouts*/
    LV_OBJ_FLAG_LAYOUT_2        = (1 << 25), /** Custom flag, free to use by layouts*/
    LV_OBJ_FLAG_WIDGET_1        = (1 << 26), /** Custom flag, free to use by widget*/
    LV_OBJ_FLAG_WIDGET_2        = (1 << 27), /** Custom flag, free to use by widget*/
    LV_OBJ_FLAG_USER_1          = (1 << 28), /** Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_2          = (1 << 29), /** Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_3          = (1 << 30), /** Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_4          = (1 << 31), /** Custom flag, free to use by user*/
};
typedef uint32_t lv_obj_flag_t;

#include "lv_obj_pos.h"
#include "lv_obj_scroll.h"
#include "lv_obj_style.h"
#include "lv_obj_draw.h"
#include "lv_grid.h"
#include "lv_flex.h"

typedef struct {
    struct _lv_obj_t ** children;       /**< Store the pointer of the children.*/
    uint32_t child_cnt;
#if LV_USE_GROUP != 0
    void * group_p;
#endif

    const lv_layout_dsc_t * layout_dsc;

    lv_event_cb_t event_cb; /**< Event callback function */

    lv_point_t scroll; /**< The current X/Y scroll offset*/
    lv_coord_t ext_draw_size; /**< EXTend the size in every direction for drawing. */

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad; /**< Extra click padding in all direction */
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;   /**< Extra click padding area. */
#endif

    lv_scrollbar_mode_t scrollbar_mode :2; /**< How to display scrollbars*/
    lv_snap_align_t snap_align_x : 2;
    lv_snap_align_t snap_align_y : 2;
    lv_scroll_dir_t scroll_dir :4;
    lv_bidi_dir_t base_dir  : 2; /**< Base direction of texts related to this object */
}lv_obj_spec_attr_t;


#define _lv_obj_constructor

typedef struct lv_obj_class{
    const struct lv_obj_class * base_class;
    void (*constructor)(struct _lv_obj_t * obj, struct _lv_obj_t * parent, const struct _lv_obj_t * copy);
    void (*destructor)(struct _lv_obj_t * obj);
    lv_signal_cb_t signal_cb; /**< Object type specific signal function*/
    lv_draw_cb_t draw_cb; /**< Object type specific draw function*/
    uint32_t instance_size;
}lv_obj_class_t;

typedef struct _lv_obj_t{
    const lv_obj_class_t * class_p;
    struct _lv_obj_t * parent;
    lv_obj_spec_attr_t * spec_attr;
    lv_obj_style_list_t  style_list;
    lv_area_t coords;
    lv_coord_t x_set;
    lv_coord_t y_set;
    lv_coord_t w_set;
    lv_coord_t h_set;
    lv_obj_flag_t flags;
    lv_state_t state;
}lv_obj_t;

extern const lv_obj_class_t lv_obj;

enum {
    LV_PART_MAIN,
    LV_PART_SCROLLBAR,
    LV_PART_INDICATOR,
    LV_PART_KNOB,
    LV_PART_HIGHLIGHT,  //selected?
    LV_PART_PLACEHOLDER,
    LV_PART_ITEMS,
    LV_PART_MARKER,
    LV_PART_CURSOR,  //combine with marker?
};

typedef uint8_t lv_part_t;

typedef struct {
    lv_point_t * point;
    bool result;
} lv_hit_test_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the 'lv' library.
 */
void lv_init(void);


/**
 * Deinit the 'lv' library
 * Currently only implemented when not using custom allocators, or GC is enabled.
 */
#if LV_ENABLE_GC || !LV_MEM_CUSTOM
void lv_deinit(void);
#endif

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 *
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other base object to copy.
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy);

lv_obj_t * lv_obj_create_from_class(const lv_obj_class_t * class, lv_obj_t * parent, const lv_obj_t * copy);

void lv_obj_construct_base(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return LV_RES_INV because the object is deleted
 */
lv_res_t lv_obj_del(lv_obj_t * obj);

#if LV_USE_ANIMATION
/**
 * A function to be easily used in animation ready callback to delete an object when the animation is ready
 * @param a pointer to the animation
 */
void lv_obj_del_anim_ready_cb(lv_anim_t * a);
#endif

/**
 * Helper function for asynchronously deleting objects.
 * Useful for cases where you can't delete an object directly in an `LV_EVENT_DELETE` handler (i.e. parent).
 * @param obj object to delete
 * @see lv_async_call
 */
void lv_obj_del_async(struct _lv_obj_t * obj);

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void lv_obj_clean(lv_obj_t * obj);

/**
 * Mark an area of an object as invalid.
 * This area will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 * @param area the area to redraw
 */
void lv_obj_invalidate_area(const lv_obj_t * obj, const lv_area_t * area);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(const lv_obj_t * obj);


/**
 * Tell whether an area of an object is visible (even partially) now or not
 * @param obj pointer to an object
 * @param area the are to check. The visible part of the area will be written back here.
 * @return true: visible; false: not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_area_is_visible(const lv_obj_t * obj, lv_area_t * area);

/**
 * Tell whether an object is visible (even partially) now or not
 * @param obj pointer to an object
 * @return true: visible; false: not visible (hidden, out of parent, on other screen, etc)
 */
bool lv_obj_is_visible(const lv_obj_t * obj);

/*=====================
 * Setter functions
 *====================*/

/*--------------------
 * Parent/children set
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object. Can't be a screen.
 * @param parent pointer to the new parent object. (Can't be NULL)
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent);

/**
 * Move and object to the foreground
 * @param obj pointer to an object
 */
void lv_obj_move_foreground(lv_obj_t * obj);

/**
 * Move and object to the background
 * @param obj pointer to an object
 */
void lv_obj_move_background(lv_obj_t * obj);

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set the size of an extended clickable area
 * @param obj pointer to an object
 * @param left extended clickable are on the left [px]
 * @param right extended clickable are on the right [px]
 * @param top extended clickable are on the top [px]
 * @param bottom extended clickable are on the bottom [px]
 */
void lv_obj_set_ext_click_area(lv_obj_t * obj, lv_coord_t left, lv_coord_t right, lv_coord_t top, lv_coord_t bottom);

/**
 * Get the extended draw area of an object.
 * @param obj pointer to an object
 * @return the size extended draw area around the real coordinates
 */
lv_coord_t _lv_obj_get_ext_draw_pad(const lv_obj_t * obj);

/*---------------------
 * Appearance set
 *--------------------*/

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Set the base direction of the object
 * @param obj pointer to an object
 * @param dir the new base direction. `LV_BIDI_DIR_LTR/RTL/AUTO/INHERIT`
 */
void lv_obj_set_base_dir(lv_obj_t * obj, lv_bidi_dir_t dir);

void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f);

void lv_obj_clear_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Set the state (fully overwrite) of an object.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the new state
 */
void lv_obj_set_state(lv_obj_t * obj, lv_state_t state);

/**
 * Add a given state or states to the object. The other state bits will remain unchanged.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the state bits to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_add_state(lv_obj_t * obj, lv_state_t state);

/**
 * Remove a given state or states to the object. The other state bits will remain unchanged.
 * If specified in the styles a transition animation will be started
 * from the previous state to the current
 * @param obj pointer to an object
 * @param state the state bits to remove. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_clear_state(lv_obj_t * obj, lv_state_t state);


/**
 * Set a an event handler function for an object.
 * Used by the user to react on event which happens with the object.
 * @param obj pointer to an object
 * @param event_cb the new event function
 */
void lv_obj_set_event_cb(lv_obj_t * obj, lv_event_cb_t event_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send(lv_obj_t * obj, lv_event_t event, const void * data);


/**
 * Send LV_EVENT_REFRESH event to an object
 * @param obj point to an object. (Can NOT be NULL)
 * @return LV_RES_OK: success, LV_RES_INV: to object become invalid (e.g. deleted) due to this event.
 */
lv_res_t lv_event_send_refresh(lv_obj_t * obj);

/**
 * Send LV_EVENT_REFRESH event to an object and all of its children
 * @param obj pointer to an object or NULL to refresh all objects of all displays
 */
void lv_event_send_refresh_recursive(lv_obj_t * obj);

/**
 * Call an event function with an object, event, and data.
 * @param event_xcb an event callback function. If `NULL` `LV_RES_OK` will return without any actions.
 *        (the 'x' in the argument name indicates that its not a fully generic function because it not follows
 *         the `func_name(object, callback, ...)` convention)
 * @param obj pointer to an object to associate with the event (can be `NULL` to simply call the `event_cb`)
 * @param event an event
 * @param data pointer to a custom data
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send_func(lv_event_cb_t event_xcb, lv_obj_t * obj, lv_event_t event, const void * data);

/**
 * Get the `data` parameter of the current event
 * @return the `data` parameter
 */
void * lv_event_get_data(void);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @return LV_RES_OK or LV_RES_INV
 */
lv_res_t lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param);

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return pointer to the allocated ext
 */
void * lv_obj_allocate_ext_attr(lv_obj_t * obj, uint16_t ext_size);

lv_obj_spec_attr_t * lv_obj_allocate_spec_attr(lv_obj_t * obj);
/*=======================
 * Getter functions
 *======================*/

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_screen(const lv_obj_t * obj);

/**
 * Get the display of an object
 * @return pointer the object's display
 */
lv_disp_t * lv_obj_get_disp(const lv_obj_t * obj);

/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
lv_obj_t * lv_obj_get_parent(const lv_obj_t * obj);

/**
 * Get the Nth child of a an object. 0th is the lastly created.
 * @param obj pointer to an object whose children should be get
 * @param id of a child
 * @return the child or `NULL` if `id` was greater then the `number of children - 1`
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, uint32_t id);

uint32_t lv_obj_get_child_cnt(const lv_obj_t * obj);

/**
 * Get the child index of an object.
 * If this object is the firstly created child of its parent 0 will be return.
 * If its the second child return 1, etc.
 * @param obj pointer to an object whose index should be get
 * @return the child index of the object.
 */
uint32_t lv_obj_get_child_id(const lv_obj_t * obj);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint32_t lv_obj_count_children(const lv_obj_t * obj);

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint32_t lv_obj_count_children_recursive(const lv_obj_t * obj);

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Get the extended extended clickable area in a direction
 * @param obj pointer to an object
 * @param dir in which direction get the extended area (`LV_DIR_LEFT/RIGHT/TOP`)
 * @return the extended left padding
 */
lv_coord_t lv_obj_get_ext_click_area(const lv_obj_t * obj, lv_dir_t dir);


/**
 * Check if a given screen-space point is on an object's coordinates.
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool _lv_obj_is_click_point_on(lv_obj_t * obj, const lv_point_t * point);

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj object to hit-test
 * @param point screen-space point
 * @return true if the object is considered under the point
 */
bool lv_obj_hit_test(lv_obj_t * obj, lv_point_t * point);

/*-----------------
 * Attribute get
 *----------------*/

bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f);

lv_bidi_dir_t lv_obj_get_base_dir(const lv_obj_t * obj);

lv_state_t lv_obj_get_state(const lv_obj_t * obj);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_cb_t lv_obj_get_signal_cb(const lv_obj_t * obj);

/**
 * Get the draw function of an object
 * @param obj pointer to an object
 * @return the draw function
 */
lv_draw_cb_t lv_obj_get_draw_cb(const lv_obj_t * obj);

/**
 * Get the event function of an object
 * @param obj pointer to an object
 * @return the event function
 */
lv_event_cb_t lv_obj_get_event_cb(const lv_obj_t * obj);

/*------------------
 * Other get
 *-----------------*/

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj object to hit-test
 * @param point screen-space point
 * @return true if the object is considered under the point
 */
bool lv_obj_hit_test(lv_obj_t * obj, lv_point_t * point);

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext_attr(const lv_obj_t * obj);

bool lv_obj_check_type(const lv_obj_t * obj, const void * class_p);

#if LV_USE_USER_DATA
/**
 * Get the object's user data
 * @param obj pointer to an object
 * @return user data
 */
lv_obj_user_data_t lv_obj_get_user_data(const lv_obj_t * obj);

/**
 * Get a pointer to the object's user data
 * @param obj pointer to an object
 * @return pointer to the user data
 */
lv_obj_user_data_t * lv_obj_get_user_data_ptr(lv_obj_t * obj);

/**
 * Set the object's user data. The data will be copied.
 * @param obj pointer to an object
 * @param data user data
 */
void lv_obj_set_user_data(lv_obj_t * obj, lv_obj_user_data_t data);

#endif

/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(const lv_obj_t * obj);

/**
 * Tell whether the object is the focused object of a group or not.
 * @param obj pointer to an object
 * @return true: the object is focused, false: the object is not focused or not in a group
 */
bool lv_obj_is_focused(const lv_obj_t * obj);

lv_obj_t ** lv_obj_get_children(const lv_obj_t * obj);

/**
 * Get the really focused object by taking `focus_parent` into account.
 * @param obj the start object
 * @return the object to really focus
 */
lv_obj_t * _lv_obj_get_focused_obj(const lv_obj_t * obj);

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

/**
 * Check if any object has a given type
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool _lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type);

/**
 * Check if any object is still "alive", and part of the hierarchy
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool _lv_debug_check_obj_valid(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

/**
 * Helps to quickly declare an event callback function.
 * Will be expanded to: `static void <name> (lv_obj_t * obj, lv_event_t e)`
 *
 * Examples:
 * LV_EVENT_CB_DECLARE(my_event1);  //Prototype declaration
 *
 * LV_EVENT_CB_DECLARE(my_event1)
 * {
 *   if(e == LV_EVENT_CLICKED) {
 *      lv_obj_set_hidden(obj ,true);
 *   }
 * }
 */
#define LV_EVENT_CB_DECLARE(name) static void name(lv_obj_t * obj, lv_event_t e)


#if LV_USE_DEBUG

# ifndef LV_DEBUG_IS_OBJ
#  define LV_DEBUG_IS_OBJ(obj_p, obj_type) (lv_debug_check_null(obj_p) &&      \
                                            _lv_debug_check_obj_valid(obj_p) && \
                                            _lv_debug_check_obj_type(obj_p, obj_type))
# endif


# if LV_USE_ASSERT_OBJ
#  ifndef LV_ASSERT_OBJ
#   define LV_ASSERT_OBJ(obj_p, obj_type) LV_DEBUG_ASSERT(LV_DEBUG_IS_OBJ(obj_p, obj_type), "Invalid object", obj_p);
#  endif
# else /* LV_USE_ASSERT_OBJ == 0 */
#  if LV_USE_ASSERT_NULL /*Use at least LV_ASSERT_NULL if enabled*/
#    define LV_ASSERT_OBJ(obj_p, obj_type) LV_ASSERT_NULL(obj_p)
#  else
#    define LV_ASSERT_OBJ(obj_p, obj_type)
#  endif
# endif
#else
# define LV_ASSERT_OBJ(obj, obj_type)
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/

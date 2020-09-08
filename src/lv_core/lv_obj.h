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
#include "lv_style.h"
#include "lv_grid.h"
#include "../lv_misc/lv_types.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_debug.h"
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

#define LV_EXT_CLICK_AREA_OFF   0
#define LV_EXT_CLICK_AREA_TINY  1
#define LV_EXT_CLICK_AREA_FULL  2

#define _LV_OBJ_PART_VIRTUAL_FIRST 0x01
#define _LV_OBJ_PART_REAL_FIRST    0x40

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

/** Design modes */
enum {
    LV_DESIGN_DRAW_MAIN, /**< Draw the main portion of the object */
    LV_DESIGN_DRAW_POST, /**< Draw extras on the object */
    LV_DESIGN_COVER_CHK, /**< Check if the object fully covers the 'mask_p' area */
};
typedef uint8_t lv_design_mode_t;


/** Design results */
enum {
    LV_DESIGN_RES_OK,          /**< Draw ready */
    LV_DESIGN_RES_COVER,       /**< Returned on `LV_DESIGN_COVER_CHK` if the areas is fully covered*/
    LV_DESIGN_RES_NOT_COVER,   /**< Returned on `LV_DESIGN_COVER_CHK` if the areas is not covered*/
    LV_DESIGN_RES_MASKED,      /**< Returned on `LV_DESIGN_COVER_CHK` if the areas is masked out (children also not cover)*/
};
typedef uint8_t lv_design_res_t;

/**
 * The design callback is used to draw the object on the screen.
 * It accepts the object, a mask area, and the mode in which to draw the object.
 */
typedef lv_design_res_t (*lv_design_cb_t)(struct _lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode);

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
    LV_EVENT_SCROLL_THROW_BEGIN,
    LV_EVENT_SCROLL_END,
    LV_EVENT_SCROLLED,
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
    LV_SIGNAL_CLEANUP,           /**< Object is being deleted */
    LV_SIGNAL_CHILD_CHG,         /**< Child was removed/added */
    LV_SIGNAL_COORD_CHG,         /**< Object coordinates/size have changed */
    LV_SIGNAL_STYLE_CHG,         /**< Object's style has changed */
    LV_SIGNAL_BASE_DIR_CHG,      /**<The base dir has changed*/
    LV_SIGNAL_REFR_EXT_DRAW_PAD, /**< Object's extra padding has changed */
    LV_SIGNAL_GET_TYPE,          /**< LVGL needs to retrieve the object's type */
    LV_SIGNAL_GET_STYLE,         /**<Get the style of an object*/
    LV_SIGNAL_GET_STATE_DSC,     /**<Get the state of the object*/

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
    LV_SIGNAL_SCROLL_THROW_BEGIN,/**< Scroll throw started*/
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
    LV_STATE_DEFAULT   =  0x00,
    LV_STATE_CHECKED  =  0x01,
    LV_STATE_FOCUSED  =  0x02,
    LV_STATE_EDITED   =  0x04,
    LV_STATE_HOVERED  =  0x08,
    LV_STATE_PRESSED  =  0x10,
    LV_STATE_DISABLED =  0x20,
};

typedef uint8_t lv_state_t;

enum {
    LV_DIR_NONE     = 0x00,
    LV_DIR_LEFT     = (1 << 0),
    LV_DIR_RIGHT    = (1 << 1),
    LV_DIR_TOP      = (1 << 2),
    LV_DIR_BOTTOM   = (1 << 3),
    LV_DIR_HOR      = LV_DIR_LEFT | LV_DIR_RIGHT,
    LV_DIR_VER      = LV_DIR_TOP | LV_DIR_BOTTOM,
    LV_DIR_ALL      = LV_DIR_HOR | LV_DIR_VER,
};

typedef uint8_t lv_dir_t;


enum {
    LV_OBJ_FLAG_HIDDEN          = (1 << 0),
    LV_OBJ_FLAG_CLICKABLE       = (1 << 1),
    LV_OBJ_FLAG_SCROLLABLE      = (1 << 2),
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1 << 3),
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1 << 4),
    LV_OBJ_FLAG_SCROLL_STOP     = (1 << 5),
    LV_OBJ_FLAG_PRESS_LOCK      = (1 << 6),
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1 << 7),
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1 << 8),
    LV_OBJ_FLAG_FOCUS_BUBBLE    = (1 << 9),
    LV_OBJ_FLAG_ADV_HITTEST     = (1 << 10),
};
typedef uint16_t lv_obj_flag_t;


#include "lv_obj_pos.h"
#include "lv_obj_scroll.h"
#include "lv_obj_style.h"
#include "lv_obj_draw.h"


typedef struct _lv_obj_t {
    struct _lv_obj_t * parent; /**< Pointer to the parent object*/
    lv_ll_t child_ll;       /**< Linked list to store the children objects*/

    lv_area_t coords; /**< Coordinates of the object (x1, y1, x2, y2)*/
    lv_point_t scroll; /**< The current X/Y scroll offset*/

    lv_event_cb_t event_cb; /**< Event callback function */
    lv_signal_cb_t signal_cb; /**< Object type specific signal function*/
    lv_design_cb_t design_cb; /**< Object type specific design function*/

    void * ext_attr;            /**< Object type specific extended data*/
    lv_style_list_t  style_list;

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad; /**< Extra click padding in all direction */
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;   /**< Extra click padding area. */
#endif

    lv_coord_t ext_draw_pad; /**< EXTend the size in every direction for drawing. */

    /*Attributes and states*/
    lv_obj_flag_t flags;
    lv_scroll_mode_t scroll_mode :2; /**< How to display scrollbars*/
    lv_scroll_snap_align_t snap_align_x : 2;
    lv_scroll_snap_align_t snap_align_y : 2;
    lv_scroll_dir_t scroll_dir :4;
    lv_bidi_dir_t base_dir  : 2; /**< Base direction of texts related to this object */

    lv_state_t state;

    lv_coord_t x_set;
    lv_coord_t y_set;
    lv_coord_t w_set;
    lv_coord_t h_set;

#if LV_USE_GROUP != 0
    void * group_p;
#endif


#if LV_USE_USER_DATA
    lv_obj_user_data_t user_data; /**< Custom user data for object. */
#endif

    const lv_grid_t * grid;

} lv_obj_t;

enum {
    LV_OBJ_PART_MAIN,
    _LV_OBJ_PART_VIRTUAL_LAST = _LV_OBJ_PART_VIRTUAL_FIRST,
    _LV_OBJ_PART_REAL_LAST =    _LV_OBJ_PART_REAL_FIRST,
    LV_OBJ_PART_ALL = 0xFF,
};

typedef uint8_t lv_obj_part_t;

/** Used by `lv_obj_get_type()`. The object's and its ancestor types are stored here*/
typedef struct {
    const char * type[LV_MAX_ANCESTOR_NUM]; /**< [0]: the actual type, [1]: ancestor, [2] #1's ancestor
                                               ... [x]: "lv_obj" */
} lv_obj_type_t;

typedef struct {
    lv_point_t * point;
    bool result;
} lv_hit_test_info_t;

typedef struct {
    uint8_t part;
    lv_style_list_t * result;
} lv_get_style_info_t;

typedef struct {
    uint8_t part;
    lv_state_t result;
} lv_get_state_info_t;

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
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy);

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
 * @param obj point to an obejct. (Can NOT be NULL)
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
const void * lv_event_get_data(void);

/**
 * Set the a signal function of an object. Used internally by the library.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param signal_cb the new signal function
 */
void lv_obj_set_signal_cb(lv_obj_t * obj, lv_signal_cb_t signal_cb);

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`.
 * @return LV_RES_OK or LV_RES_INV
 */
lv_res_t lv_signal_send(lv_obj_t * obj, lv_signal_t signal, void * param);

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param design_cb the new design function
 */
void lv_obj_set_design_cb(lv_obj_t * obj, lv_design_cb_t design_cb);

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
 * Iterate through the children of an object (start from the "youngest, lastly created")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(const lv_obj_t * obj, const lv_obj_t * child);

/**
 * Iterate through the children of an object (start from the "oldest", firstly created)
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child_back(const lv_obj_t * obj, const lv_obj_t * child);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children(const lv_obj_t * obj);

/** Recursively count the children of an object
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children_recursive(const lv_obj_t * obj);

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

lv_state_t lv_obj_get_state(const lv_obj_t * obj, uint8_t part);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_cb_t lv_obj_get_signal_cb(const lv_obj_t * obj);

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_cb_t lv_obj_get_design_cb(const lv_obj_t * obj);

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

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
void lv_obj_get_type(const lv_obj_t * obj, lv_obj_type_t * buf);

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
lv_obj_user_data_t * lv_obj_get_user_data_ptr(const lv_obj_t * obj);

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
 * Used in the signal callback to handle `LV_SIGNAL_GET_TYPE` signal
 * @param buf pointer to `lv_obj_type_t`. (`param` in the signal callback)
 * @param name name of the object. E.g. "lv_btn". (Only the pointer is saved)
 * @return LV_RES_OK
 */
lv_res_t _lv_obj_handle_get_type_signal(lv_obj_type_t * buf, const char * name);


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
#    define LV_ASSERT_OBJ(obj_p, obj_type) true
#  endif
# endif
#else
# define LV_ASSERT_OBJ(obj, obj_type) true
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/

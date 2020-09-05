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
    LV_SCROLL_SNAP_ALIGN_NONE,
    LV_SCROLL_SNAP_ALIGN_START,
    LV_SCROLL_SNAP_ALIGN_END,
    LV_SCROLL_SNAP_ALIGN_CENTER
};
typedef uint8_t lv_scroll_snap_align_t;


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
};
typedef uint16_t lv_obj_flag_t;

/** Scrollbar modes: shows when should the scrollbars be visible*/
enum {
    LV_SCROLL_MODE_OFF    = 0x0, /**< Never show scroll bars*/
    LV_SCROLL_MODE_ON     = 0x1, /**< Always show scroll bars*/
    LV_SCROLL_MODE_ACTIVE = 0x2, /**< Show scroll bars when object is being scrolled*/
    LV_SCROLL_MODE_AUTO   = 0x3, /**< Show scroll bars when the content is large enough to be scrolled*/
};
typedef uint8_t lv_scroll_mode_t;

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
    uint8_t ext_click_pad_hor; /**< Extra click padding in horizontal direction */
    uint8_t ext_click_pad_ver; /**< Extra click padding in vertical direction */
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;   /**< Extra click padding area. */
#endif

    lv_coord_t ext_draw_pad; /**< EXTend the size in every direction for drawing. */

    /*Attributes and states*/
    lv_obj_flag_t flags     : 9;
    lv_scroll_mode_t scroll_mode :2; /**< How to display scrollbars*/
    lv_scroll_snap_align_t snap_align_x : 2;
    lv_scroll_snap_align_t snap_align_y : 2;
    lv_bidi_dir_t base_dir  : 2; /**< Base direction of texts related to this object */
    uint8_t adv_hittest     : 1; /**< 1: Use advanced hit-testing (slower) */

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

    lv_grid_t * grid;

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
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y);


void _lv_obj_calc_auto_size(lv_obj_t * obj, lv_coord_t * w, lv_coord_t * h);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width reduced by the left and right padding.
 * @param obj pointer to an object
 * @param w the width without paddings
 */
void lv_obj_set_width_fit(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height reduced by the top and bottom padding.
 * @param obj pointer to an object
 * @param h the height without paddings
 */
void lv_obj_set_height_fit(lv_obj_t * obj, lv_coord_t h);

/**
 * Set the width of an object by taking the left and right margin into account.
 * The object width will be `obj_w = w - margin_left - margin_right`
 * @param obj pointer to an object
 * @param w new height including margins
 */
void lv_obj_set_width_margin(lv_obj_t * obj, lv_coord_t w);

/**
 * Set the height of an object by taking the top and bottom margin into account.
 * The object height will be `obj_h = h - margin_top - margin_bottom`
 * @param obj pointer to an object
 * @param h new height including margins
 */
void lv_obj_set_height_margin(lv_obj_t * obj, lv_coord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_ofs x coordinate offset after alignment
 * @param y_ofs y coordinate offset after alignment
 */
void lv_obj_align(lv_obj_t * obj, const lv_obj_t * base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs);

/**
 * Moves all children with horizontally or vertically.
 * It doesn't take into account any limits so any values are possible
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void lv_obj_scroll_by_raw(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);

/**
 * Moves all children with horizontally or vertically.
 * Limits the scroll to the bounding box of the children.
 * @param obj pointer to an object whose children should be moved
 * @param x pixel to move horizontally
 * @param y pixels to move vertically
 */
void lv_obj_scroll_by(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en);

/**
 * Scroll the a given x coordinate to the left side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param x the x coordinate to scroll to
 */
void lv_obj_scroll_to_x(lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en);

/**
 * Scroll the a given y coordinate to the top side of obj.
 * @param obj pointer to an object which should be scrolled
 * @param y the y coordinate to scroll to
 */
void lv_obj_scroll_to_y(lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en);


/**
 * Return the height of the area above the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_top(const lv_obj_t * obj);

/**
 * Return the height of the area below the parent.
 * That is the number of pixels the object can be scrolled up.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_bottom(const lv_obj_t * obj);


/**
 * Return the weight of the area on the left the parent.
 * That is the number of pixels the object can be scrolled down.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_left(const lv_obj_t * obj);

/**
 * Return the width of the area below the object.
 * That is the number of pixels the object can be scrolled left.
 * Normally positive but can be negative when scrolled inside.
 * @param obj
 * @return
 */
lv_coord_t lv_obj_get_scroll_right(const lv_obj_t * obj);

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

/**
 * Add a new style to the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to add (Only its pointer will be saved)
 */
void lv_obj_add_style(lv_obj_t * obj, uint8_t part, lv_style_t * style);

/**
 * Remove a style from the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to remove
 */
void lv_obj_remove_style(lv_obj_t * obj, uint8_t part, lv_style_t * style);

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Typically used in `LV_SIGN_CLEAN_UP.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_clean_style_list(lv_obj_t * obj, uint8_t part);

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Also notifies the object about the style change.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_reset_style_list(lv_obj_t * obj, uint8_t part);

/**
 * Notify an object (and its children) about its style is modified
 * @param obj pointer to an object
 * @param prop `LV_STYLE_PROP_ALL` or an `LV_STYLE_...` property. It is used to optimize what needs to be refreshed.
 */
void lv_obj_refresh_style(lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_mod(lv_style_t * style);

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_COLOR | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has color type
 */
void _lv_obj_set_style_local_color(lv_obj_t * obj, uint8_t type, lv_style_property_t prop, lv_color_t color);

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
void _lv_obj_set_style_local_int(lv_obj_t * obj, uint8_t type, lv_style_property_t prop, lv_style_int_t value);

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_OPA | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
void _lv_obj_set_style_local_opa(lv_obj_t * obj, uint8_t type, lv_style_property_t prop, lv_opa_t opa);

/**
 * Set a local style property of a part of an object in a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @param the value to set
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
void _lv_obj_set_style_local_ptr(lv_obj_t * obj, uint8_t type, lv_style_property_t prop, const void * value);

/**
 * Remove a local style property from a part of an object with a given state.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be removed.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop a style property ORed with a state.
 * E.g. `LV_STYLE_TEXT_FONT | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @note shouldn't be used directly. Use the specific property remove functions instead.
 *       For example: `lv_obj_style_remove_border_opa()`
 * @return true: the property was found and removed; false: the property was not found
 */
bool lv_obj_remove_style_local_prop(lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Enable/disable the use of style cahche for an object
 * @param obj pointer to an object
 * @param dis true: disable; false: enable (re-enable)
 */
void _lv_obj_disable_style_caching(lv_obj_t * obj, bool dis);

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Set whether advanced hit-testing is enabled on an object
 * @param obj pointer to an object
 * @param en true: advanced hit-testing is enabled
 */
void lv_obj_set_adv_hittest(lv_obj_t * obj, bool en);


/**
 * Set how the scrollbars should behave.
 * @param obj pointer to an object
 * @param mode: LV_SCROLL_MODE_ON/OFF/AUTO/ACTIVE
 */
void lv_obj_set_scroll_mode(lv_obj_t * obj, lv_scroll_mode_t mode);

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

#if LV_USE_ANIMATION
/**
 * Finish all pending transitions on a part of an object
 * @param obj pointer to an object
 * @param part part of the object, e.g `LV_BRN_PART_MAIN` or `LV_OBJ_PART_ALL` for all parts
 */
void lv_obj_finish_transitions(lv_obj_t * obj, uint8_t part);
#endif

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

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object to refresh the extended draw area.
 * he object needs to be invalidated by `lv_obj_invalidate(obj)` manually after this function.
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_draw_pad(lv_obj_t * obj);

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
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_coords(const lv_obj_t * obj, lv_area_t * cords_p);

/**
 * Reduce area retried by `lv_obj_get_coords()` the get graphically usable area of an object.
 * (Without the size of the border or other extra graphical elements)
 * @param coords_p store the result area here
 */
void lv_obj_get_inner_coords(const lv_obj_t * obj, lv_area_t * coords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
lv_coord_t lv_obj_get_x(const lv_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
lv_coord_t lv_obj_get_y(const lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
lv_coord_t lv_obj_get_width(const lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
lv_coord_t lv_obj_get_height(const lv_obj_t * obj);

/**
 * Get that width reduced by the left and right padding.
 * @param obj pointer to an object
 * @return the width which still fits into the container
 */
lv_coord_t lv_obj_get_width_fit(const lv_obj_t * obj);

/**
 * Get that height reduced by the top an bottom padding.
 * @param obj pointer to an object
 * @return the height which still fits into the container
 */
lv_coord_t lv_obj_get_height_fit(const lv_obj_t * obj);

/**
 * Get the height of an object by taking the top and bottom margin into account.
 * The returned height will be `obj_h + margin_top + margin_bottom`
 * @param obj pointer to an object
 * @return the height including thee margins
 */
lv_coord_t lv_obj_get_height_margin(lv_obj_t * obj);

/**
 * Get the width of an object by taking the left and right margin into account.
 * The returned width will be `obj_w + margin_left + margin_right`
 * @param obj pointer to an object
 * @return the height including thee margins
 */
lv_coord_t lv_obj_get_width_margin(lv_obj_t * obj);

/**
 * Divide the width of the object and get the width of a given number of columns.
 * Take paddings into account.
 * @param obj pointer to an object
 * @param div indicates how many columns are assumed.
 * If 1 the width will be set the the parent's width
 * If 2 only half parent width - inner padding of the parent
 * If 3 only third parent width - 2 * inner padding of the parent
 * @param span how many columns are combined
 * @return the width according to the given parameters
 */
lv_coord_t lv_obj_get_width_grid(lv_obj_t * obj, uint8_t div, uint8_t span);

/**
 * Divide the height of the object and get the width of a given number of columns.
 * Take paddings into account.
 * @param obj pointer to an object
 * @param div indicates how many rows are assumed.
 * If 1 the height will be set the the parent's height
 * If 2 only half parent height - inner padding of the parent
 * If 3 only third parent height - 2 * inner padding of the parent
 * @param span how many rows are combined
 * @return the height according to the given parameters
 */
lv_coord_t lv_obj_get_height_grid(lv_obj_t * obj, uint8_t div, uint8_t span);

/**
 * Get the left padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended left padding
 */
lv_coord_t lv_obj_get_ext_click_pad_left(const lv_obj_t * obj);

/**
 * Get the right padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended right padding
 */
lv_coord_t lv_obj_get_ext_click_pad_right(const lv_obj_t * obj);

/**
 * Get the top padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended top padding
 */
lv_coord_t lv_obj_get_ext_click_pad_top(const lv_obj_t * obj);

/**
 * Get the bottom padding of extended clickable area
 * @param obj pointer to an object
 * @return the extended bottom padding
 */
lv_coord_t lv_obj_get_ext_click_pad_bottom(const lv_obj_t * obj);

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
lv_coord_t lv_obj_get_ext_draw_pad(const lv_obj_t * obj);

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style list of an object's part.
 * @param obj pointer to an object.
 * @param part part the part of the object which style list should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @return pointer to the style list. (Can be `NULL`)
 */
lv_style_list_t * lv_obj_get_style_list(const lv_obj_t * obj, uint8_t part);

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_WIDTH`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_width()`
 * @note for performance reasons it's not checked if the property really has integer type
 */
lv_style_int_t _lv_obj_get_style_int(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_COLOR`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_color()`
 * @note for performance reasons it's not checked if the property really has color type
 */
lv_color_t _lv_obj_get_style_color(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_BORDER_OPA`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has opacity type
 */
lv_opa_t _lv_obj_get_style_opa(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Get a style property of a part of an object in the object's current state.
 * If there is a running transitions it is taken into account
 * @param obj pointer to an object
 * @param part the part of the object which style property should be get.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param prop the property to get. E.g. `LV_STYLE_TEXT_FONT`.
 *  The state of the object will be added internally
 * @return the value of the property of the given part in the current state.
 * If the property is not found a default value will be returned.
 * @note shouldn't be used directly. Use the specific property get functions instead.
 *       For example: `lv_obj_style_get_border_opa()`
 * @note for performance reasons it's not checked if the property really has pointer type
 */
const void * _lv_obj_get_style_ptr(const lv_obj_t * obj, uint8_t part, lv_style_property_t prop);

/**
 * Get the local style of a part of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 * E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @return pointer to the local style if exists else `NULL`.
 */
lv_style_t * lv_obj_get_local_style(lv_obj_t * obj, uint8_t part);


#include "lv_obj_style_dec.h"

/*-----------------
 * Attribute get
 *----------------*/


bool lv_obj_has_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Get whether advanced hit-testing is enabled on an object
 * @param obj pointer to an object
 * @return true: advanced hit-testing is enabled
 */
bool lv_obj_get_adv_hittest(const lv_obj_t * obj);

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
 * Check if a given screen-space point is on an object's coordinates.
 *
 * This method is intended to be used mainly by advanced hit testing algorithms to check
 * whether the point is even within the object (as an optimization).
 * @param obj object to check
 * @param point screen-space point
 */
bool lv_obj_is_point_on_coords(lv_obj_t * obj, const lv_point_t * point);

/**
 * Hit-test an object given a particular point in screen space.
 * @param obj object to hit-test
 * @param point screen-space point
 * @return true if the object is considered under the point
 */
bool lv_obj_hittest(lv_obj_t * obj, lv_point_t * point);

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
lv_obj_t * lv_obj_get_focused_obj(const lv_obj_t * obj);

/*-------------------
 * OTHER FUNCTIONS
 *------------------*/

/**
 * Used in the signal callback to handle `LV_SIGNAL_GET_TYPE` signal
 * @param buf pointer to `lv_obj_type_t`. (`param` in the signal callback)
 * @param name name of the object. E.g. "lv_btn". (Only the pointer is saved)
 * @return LV_RES_OK
 */
lv_res_t lv_obj_handle_get_type_signal(lv_obj_type_t * buf, const char * name);

/**
 * Initialize a rectangle descriptor from an object's styles
 * @param obj pointer to an object
 * @param type type of style. E.g.  `LV_OBJ_PART_MAIN`, `LV_BTN_SLIDER_KOB`
 * @param draw_dsc the descriptor the initialize
 * @note Only the relevant fields will be set.
 * E.g. if `border width == 0` the other border properties won't be evaluated.
 */
void lv_obj_init_draw_rect_dsc(lv_obj_t * obj, uint8_t type, lv_draw_rect_dsc_t * draw_dsc);

void lv_obj_init_draw_label_dsc(lv_obj_t * obj, uint8_t type, lv_draw_label_dsc_t * draw_dsc);

void lv_obj_init_draw_img_dsc(lv_obj_t * obj, uint8_t part, lv_draw_img_dsc_t * draw_dsc);

void lv_obj_init_draw_line_dsc(lv_obj_t * obj, uint8_t part, lv_draw_line_dsc_t * draw_dsc);

/**
 * Get the required extra size (around the object's part) to draw shadow, outline, value etc.
 * @param obj pointer to an object
 * @param part part of the object
 */
lv_coord_t lv_obj_get_draw_rect_ext_pad_size(lv_obj_t * obj, uint8_t part);

/**
 * Fade in (from transparent to fully cover) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Fade out (from fully cover to transparent) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_out(lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Check if any object has a given type
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool lv_debug_check_obj_type(const lv_obj_t * obj, const char * obj_type);

/**
 * Check if any object is still "alive", and part of the hierarchy
 * @param obj pointer to an object
 * @param obj_type type of the object. (e.g. "lv_btn")
 * @return true: valid
 */
bool lv_debug_check_obj_valid(const lv_obj_t * obj);


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
                                            lv_debug_check_obj_valid(obj_p) && \
                                            lv_debug_check_obj_type(obj_p, obj_type))
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

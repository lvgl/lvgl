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
#include "../lv_hal/lv_hal.h"

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

/**
 * Options for extra click area behavior.
 * These values can be selected in `lv_conf.h`
 */
#define LV_EXT_CLICK_AREA_OFF   0  /*Disable the usage of extra click area*/
#define LV_EXT_CLICK_AREA_TINY  1  /*Use the same value in all 4 directions*/
#define LV_EXT_CLICK_AREA_FULL  2  /*Allow setting different values in every 4 directions*/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_t;

/*---------------------
 *       EVENTS
 *---------------------*/

/**
 * Type of event being sent to the object.
 */
typedef enum {
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
    LV_EVENT_DELETE, /**< Object is being deleted */

    LV_EVENT_COVER_CHECK,      /**< Check if the object fully covers the 'mask_p' area */
    LV_EVENT_REFR_EXT_SIZE,   /**< Draw extras on the object */

    LV_EVENT_DRAW_MAIN_BEGIN,
    LV_EVENT_DRAW_MAIN_FINISH,
    LV_EVENT_DRAW_POST_BEGIN,
    LV_EVENT_DRAW_POST_END,
    LV_EVENT_DRAW_PART_BEGIN,
    LV_EVENT_DRAW_PART_END,

    LV_EVENT_READY,             /**< A process has finished */
    LV_EVENT_CANCEL,             /**< A process has been cancelled */

    _LV_EVENT_LAST /** Number of default events*/
}lv_event_t;

/**
 * @brief Event callback.
 * Events are used to notify the user of some action being taken on the object.
 * For details, see ::lv_event_t.
 */
typedef void (*lv_event_cb_t)(struct _lv_obj_t * obj, lv_event_t event);


/*---------------------
 *       EVENTS
 *---------------------*/


/** Signals are for use by the object itself or to extend the object's functionality.
 * They determine a widget with a given type should behave.
 * Applications should use ::lv_obj_set_event_cb to be notified of events that occur
 * on the object. */
typedef enum {
    /*General signals*/
    LV_SIGNAL_CHILD_CHG,          /**< Child was removed/added */
    LV_SIGNAL_COORD_CHG,          /**< Object coordinates/size have changed */
    LV_SIGNAL_STYLE_CHG,          /**< Object's style has changed */
    LV_SIGNAL_BASE_DIR_CHG,       /**< The base dir has changed*/
    LV_SIGNAL_REFR_EXT_DRAW_SIZE, /**< Object's extra padding has changed */
    LV_SIGNAL_GET_SELF_SIZE,      /**< Get the internal size of a widget*/

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
    LV_SIGNAL_GESTURE,           /**< The object has been gesture*/
    LV_SIGNAL_LEAVE,             /**< Another object is clicked or chosen via an input device */
    LV_SIGNAL_FOCUS,             /**< The object was focused */
    LV_SIGNAL_DEFOCUS,           /**< The object was de-focused */
    LV_SIGNAL_CONTROL,           /**< Send a (control) character to the widget */
} lv_signal_t;

/**
 * @brief Signal callback.
 * Signals are used to notify the widget of the action related to the object.
 * For details, see ::lv_signal_t.
 */
typedef lv_res_t (*lv_signal_cb_t)(struct _lv_obj_t * obj, lv_signal_t sign, void * param);

/**
 * Possible states of a widget.
 * OR-ed values are possible
 */
enum {
    LV_STATE_DEFAULT     =  0x00,
    LV_STATE_CHECKED     =  0x01,
    LV_STATE_FOCUSED     =  0x02,
    LV_STATE_FOCUS_GROUP =  0x04,
    LV_STATE_EDITED      =  0x08,
    LV_STATE_HOVERED     =  0x10,
    LV_STATE_PRESSED     =  0x20,
    LV_STATE_SCROLLED    =  0x40,
    LV_STATE_DISABLED    =  0x80,

    LV_STATE_ANY = 0x1FF,    /**< Special value can be used in some functions to target all states */
};

typedef uint16_t lv_state_t;

/**
 * The possible parts of widgets.
 * The parts can be considered as the internal building block of the widgets.
 * E.g. slider = background + indicator + knob
 * Note every part is used by every widget
 */
enum {
    LV_PART_MAIN,        /**< A background like rectangle*/
    LV_PART_SCROLLBAR,   /**< The scrollbar(s)*/
    LV_PART_INDICATOR,   /**< Indicator, e.g. for slider, bar, switch, */
    LV_PART_KNOB,        /**< Like handle to grab to adjust the value */
    LV_PART_SELECTED,    /**< Indicate the currently selected option or section*/
    LV_PART_PLACEHOLDER, /**< A text other element used when the widget is empty*/
    LV_PART_ITEMS,       /**< Used if the widget has multiple similar elements (e.g. tabel cells)*/
    LV_PART_MARKER,      /**< Tick box of a check box, cursor of a text area or anything used to mark something*/

    LV_PART_CUSTOM_1,    /**< Extension point for custom widgets*/
    LV_PART_CUSTOM_2,    /**< Extension point for custom widgets*/
    LV_PART_CUSTOM_3,    /**< Extension point for custom widgets*/
    LV_PART_CUSTOM_4,    /**< Extension point for custom widgets*/

    LV_PART_ANY = 0xFF,  /**< Special value can be used in some functions to target all parts */
};

typedef uint16_t lv_part_t;

/**
 * On/Off features controlling the object's behavior.
 * OR-ed values are possible
 */
enum {
    LV_OBJ_FLAG_HIDDEN          = (1 << 0),  /**< Make the object hidden. (Like it wasn't there at all) */
    LV_OBJ_FLAG_CLICKABLE       = (1 << 1),  /**< Make the object clickable by the input devices */
    LV_OBJ_FLAG_CLICK_FOCUSABLE = (1 << 2),  /**< Add focused state to the object when clicked */
    LV_OBJ_FLAG_CHECKABLE       = (1 << 3),  /**< Toggle checked state when the object is clicked */
    LV_OBJ_FLAG_SCROLLABLE      = (1 << 4),  /**< Make the object scrollable*/
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1 << 5),  /**< Allow scrolling inside but with slower speed*/
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1 << 6),  /**< Make the object scroll further when "thrown"*/
    LV_OBJ_FLAG_SCROLL_ONE     = (1 << 7),   /**< Allow scrolling only one snappable children*/
    LV_OBJ_FLAG_SCROLL_CHAIN    = (1 << 8),  /**< Allow propagating the scroll to a parent */
    LV_OBJ_FLAG_SCROLL_ON_FOCUS = (1 << 9),  /**< Automatically scroll object to make it visible when focused*/
    LV_OBJ_FLAG_SNAPABLE        = (1 << 10), /**< If scroll snap is enabled it can snap to this object*/
    LV_OBJ_FLAG_PRESS_LOCK      = (1 << 11), /**< Keep the object pressed even if the press slid from the object */
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1 << 12), /**< Propagate the events to the parent too */
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1 << 13), /**< Propagate the gestures to the parent */
    LV_OBJ_FLAG_FOCUS_BUBBLE    = (1 << 14), /**< Propagate the focus to the parent */
    LV_OBJ_FLAG_ADV_HITTEST     = (1 << 15), /**< Allow performing more accurate hit (click) test. E.g. on rounded corners. */
    LV_OBJ_FLAG_LAYOUTABLE      = (1 << 16), /**< MAke the object position-able by the layouts */

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

#include "lv_obj_tree.h"
#include "lv_obj_pos.h"
#include "lv_obj_scroll.h"
#include "lv_obj_style.h"
#include "lv_obj_draw.h"
#include "lv_grid.h"
#include "lv_group.h"
#include "lv_flex.h"

/**
 * Describe the common methods of every object.
 * Similar to a C++ class.
 */
typedef struct _lv_obj_class_t{
    const struct _lv_obj_class_t * base_class;
    void (*constructor)(struct _lv_obj_t * obj, struct _lv_obj_t * parent, const struct _lv_obj_t * copy);
    void (*destructor)(struct _lv_obj_t * obj);
    lv_signal_cb_t signal_cb;       /**< Object type specific signal function*/
    lv_draw_cb_t draw_cb;           /**< Object type specific draw function*/
    uint32_t editable :1;
    uint32_t instance_size :20;
}lv_obj_class_t;

/**
 * Make the base object's class publicly available.
 */
extern const lv_obj_class_t lv_obj;

/**
 * Special, rarely used attributes.
 * They are allocated automatically if any elements is set.
 */
typedef struct {
    struct _lv_obj_t ** children;       /**< Store the pointer of the children in an array.*/
    uint32_t child_cnt;                 /**< Number of children */
    lv_group_t * group_p;

    const lv_layout_dsc_t * layout_dsc; /**< Pointer to the layout descriptor*/

    lv_event_cb_t * event_cb;             /**< Event callback function */
    lv_point_t scroll;                  /**< The current X/Y scroll offset*/

#if LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_TINY
    uint8_t ext_click_pad;      /**< Extra click padding in all direction */
#elif LV_USE_EXT_CLICK_AREA == LV_EXT_CLICK_AREA_FULL
    lv_area_t ext_click_pad;   /**< Extra click padding area. */
#endif
    lv_coord_t ext_draw_size;           /**< EXTend the size in every direction for drawing. */

    lv_scrollbar_mode_t scrollbar_mode :2; /**< How to display scrollbars*/
    lv_snap_align_t snap_align_x : 2;      /**< Where to align the snapable children horizontally*/
    lv_snap_align_t snap_align_y : 2;      /**< Where to align the snapable children horizontally*/
    lv_dir_t scroll_dir :4;                /**< The allowed scroll direction(s)*/
    lv_bidi_dir_t base_dir  : 2; /**< Base direction of texts related to this object */
    uint8_t event_cb_cnt;           /**< Number of event callabcks stored in `event_cb` array */
}lv_obj_spec_attr_t;

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


typedef struct {
    const lv_point_t * point;
    bool result;
} lv_hit_test_info_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize LVGL library.
 * Should be called before any other LVGL related function.
 */
void lv_init(void);

#if LV_ENABLE_GC || !LV_MEM_CUSTOM

/**
 * Deinit the 'lv' library
 * Currently only implemented when not using custom allocators, or GC is enabled.
 */
void lv_deinit(void);

#endif

/**
 * Create a base object (a rectangle)
 * @param parent: pointer to a parent object. If NULL then a screen will be created.
 * @param copy:   DEPRECATED, will be removed in v9.
 *                Pointer to an other base object to copy.
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, const lv_obj_t * copy);


/*---------------------
 * Event/Signal sending
 *---------------------*/

/**
 * Send an event to the object
 * @param obj pointer to an object
 * @param event the type of the event from `lv_event_t`
 * @param data arbitrary data depending on the object type and the event. (Usually `NULL`)
 * @return LV_RES_OK: `obj` was not deleted in the event; LV_RES_INV: `obj` was deleted in the event
 */
lv_res_t lv_event_send(lv_obj_t * obj, lv_event_t event, void * data);

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

/*=====================
 * Setter functions
 *====================*/


/**
 * Set one or more flags
 * @param obj: pointer to an object
 * @param f:   OR-ed values from `lv_obj_flag_t` to set.
 */
void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Clear one or more flags
 * @param obj: pointer to an object
 * @param f:   OR-ed values from `lv_obj_flag_t` to set.
 */
void lv_obj_clear_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Set the state (fully overwrite) of an object.
 * If specified in the styles, transition animations will be started from the previous state to the current.
 * @param obj:   pointer to an object
 * @param state: the new state
 */
void lv_obj_set_state(lv_obj_t * obj, lv_state_t new_state);


/**
 * Add one or more states to the object. The other state bits will remain unchanged.
 * If specified in the styles, transition animation will be started from the previous state to the current.
 * @param obj:   pointer to an object
 * @param state: the states to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_add_state(lv_obj_t * obj, lv_state_t state);

/**
 * Remove one or more states to the object. The other state bits will remain unchanged.
 * If specified in the styles, transition animation will be started from the previous state to the current.
 * @param obj:   pointer to an object
 * @param state: the states to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
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
 * Set the base direction of the object
 * @param obj pointer to an object
 * @param dir the new base direction. `LV_BIDI_DIR_LTR/RTL/AUTO/INHERIT`
 */
void lv_obj_set_base_dir(lv_obj_t * obj, lv_bidi_dir_t dir);


/*=======================
 * Getter functions
 *======================*/

/**
 * Check if a given flag or flags are set on an object.
 * @param obj pointer to an object
 * @param f the flag(s) to check (OR-ed values can be used)
 * @return true: all flags are set; false: not all flags are set
 */
bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Get the base direction of the object
 * @param obj pointer to an object
 * @return the base direction. `LV_BIDI_DIR_LTR/RTL/AUTO/INHERIT`
 */
lv_bidi_dir_t lv_obj_get_base_dir(const lv_obj_t * obj);

/**
 * Get the state of an object
 * @param obj pointer to an object
 * @return the state (OR-ed values from `lv_state_t`)
 */
lv_state_t lv_obj_get_state(const lv_obj_t * obj);

/**
 * Get the event function of an object
 * @param obj: pointer to an object
 * @param id:  the index of the event callback. 0: the firstly added
 * @return the event function
 */
lv_event_cb_t lv_obj_get_event_cb(const lv_obj_t * obj, uint32_t id);

/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(const lv_obj_t * obj);

/*=======================
 * Other functions
 *======================*/

/**
 * Allocate special data for an object if not allocated yet.
 * @param obj pointer to an object
 */
void lv_obj_allocate_spec_attr(lv_obj_t * obj);

/**
 * Get the focused object by taking `LV_OBJ_FLAG_FOCUS_BUBBLE` into account.
 * @param obj the start object
 * @return the object to to really focus
 */
lv_obj_t * lv_obj_get_focused_obj(const lv_obj_t * obj);

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
bool lv_obj_check_type(const lv_obj_t * obj, const void * class_p);

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

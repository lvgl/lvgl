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
#include "../misc/lv_types.h"
#include "../misc/lv_style.h"
#include "../misc/lv_area.h"
#include "../misc/lv_color.h"
#include "../misc/lv_assert.h"

#include "lv_obj_tree.h"
#include "lv_obj_pos.h"
#include "lv_obj_scroll.h"
#include "lv_obj_style.h"
#include "lv_obj_draw.h"
#include "lv_obj_class.h"
#include "lv_obj_event.h"
#include "lv_obj_property.h"
#include "lv_group.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Possible states of a widget.
 * OR-ed values are possible
 */
enum _lv_state_t {
    LV_STATE_DEFAULT     =  0x0000,
    LV_STATE_CHECKED     =  0x0001,
    LV_STATE_FOCUSED     =  0x0002,
    LV_STATE_FOCUS_KEY   =  0x0004,
    LV_STATE_EDITED      =  0x0008,
    LV_STATE_HOVERED     =  0x0010,
    LV_STATE_PRESSED     =  0x0020,
    LV_STATE_SCROLLED    =  0x0040,
    LV_STATE_DISABLED    =  0x0080,
    LV_STATE_USER_1      =  0x1000,
    LV_STATE_USER_2      =  0x2000,
    LV_STATE_USER_3      =  0x4000,
    LV_STATE_USER_4      =  0x8000,

    LV_STATE_ANY = 0xFFFF,    /**< Special value can be used in some functions to target all states*/
};

/**
 * The possible parts of widgets.
 * The parts can be considered as the internal building block of the widgets.
 * E.g. slider = background + indicator + knob
 * Not all parts are used by every widget
 */

enum _lv_part_t {
    LV_PART_MAIN         = 0x000000,   /**< A background like rectangle*/
    LV_PART_SCROLLBAR    = 0x010000,   /**< The scrollbar(s)*/
    LV_PART_INDICATOR    = 0x020000,   /**< Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox*/
    LV_PART_KNOB         = 0x030000,   /**< Like handle to grab to adjust the value*/
    LV_PART_SELECTED     = 0x040000,   /**< Indicate the currently selected option or section*/
    LV_PART_ITEMS        = 0x050000,   /**< Used if the widget has multiple similar elements (e.g. table cells)*/
    LV_PART_CURSOR       = 0x060000,   /**< Mark a specific place e.g. for text area's cursor or on a chart*/

    LV_PART_CUSTOM_FIRST = 0x080000,    /**< Extension point for custom widgets*/

    LV_PART_ANY          = 0x0F0000,    /**< Special value can be used in some functions to target all parts*/
};

/**
 * On/Off features controlling the object's behavior.
 * OR-ed values are possible
 *
 * Note: update obj flags corresponding properties below
 * whenever add/remove flags or change bit definition of flags.
 */
typedef enum {
    LV_OBJ_FLAG_HIDDEN          = (1L << 0),  /**< Make the object hidden. (Like it wasn't there at all)*/
    LV_OBJ_FLAG_CLICKABLE       = (1L << 1),  /**< Make the object clickable by the input devices*/
    LV_OBJ_FLAG_CLICK_FOCUSABLE = (1L << 2),  /**< Add focused state to the object when clicked*/
    LV_OBJ_FLAG_CHECKABLE       = (1L << 3),  /**< Toggle checked state when the object is clicked*/
    LV_OBJ_FLAG_SCROLLABLE      = (1L << 4),  /**< Make the object scrollable*/
    LV_OBJ_FLAG_SCROLL_ELASTIC  = (1L << 5),  /**< Allow scrolling inside but with slower speed*/
    LV_OBJ_FLAG_SCROLL_MOMENTUM = (1L << 6),  /**< Make the object scroll further when "thrown"*/
    LV_OBJ_FLAG_SCROLL_ONE      = (1L << 7),  /**< Allow scrolling only one snappable children*/
    LV_OBJ_FLAG_SCROLL_CHAIN_HOR = (1L << 8), /**< Allow propagating the horizontal scroll to a parent*/
    LV_OBJ_FLAG_SCROLL_CHAIN_VER = (1L << 9), /**< Allow propagating the vertical scroll to a parent*/
    LV_OBJ_FLAG_SCROLL_CHAIN     = (LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER),
    LV_OBJ_FLAG_SCROLL_ON_FOCUS = (1L << 10),  /**< Automatically scroll object to make it visible when focused*/
    LV_OBJ_FLAG_SCROLL_WITH_ARROW  = (1L << 11), /**< Allow scrolling the focused object with arrow keys*/
    LV_OBJ_FLAG_SNAPPABLE       = (1L << 12), /**< If scroll snap is enabled on the parent it can snap to this object*/
    LV_OBJ_FLAG_PRESS_LOCK      = (1L << 13), /**< Keep the object pressed even if the press slid from the object*/
    LV_OBJ_FLAG_EVENT_BUBBLE    = (1L << 14), /**< Propagate the events to the parent too*/
    LV_OBJ_FLAG_GESTURE_BUBBLE  = (1L << 15), /**< Propagate the gestures to the parent*/
    LV_OBJ_FLAG_ADV_HITTEST     = (1L << 16), /**< Allow performing more accurate hit (click) test. E.g. consider rounded corners.*/
    LV_OBJ_FLAG_IGNORE_LAYOUT   = (1L << 17), /**< Make the object position-able by the layouts*/
    LV_OBJ_FLAG_FLOATING        = (1L << 18), /**< Do not scroll the object when the parent scrolls and ignore layout*/
    LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS = (1L << 19), /**< Send `LV_EVENT_DRAW_TASK_ADDED` events*/
    LV_OBJ_FLAG_OVERFLOW_VISIBLE = (1L << 20),/**< Do not clip the children to the parent's ext draw size*/
#if LV_USE_FLEX
    LV_OBJ_FLAG_FLEX_IN_NEW_TRACK = (1L << 21),     /**< Start a new flex track on this item*/
#endif

    LV_OBJ_FLAG_LAYOUT_1        = (1L << 23), /**< Custom flag, free to use by layouts*/
    LV_OBJ_FLAG_LAYOUT_2        = (1L << 24), /**< Custom flag, free to use by layouts*/

    LV_OBJ_FLAG_WIDGET_1        = (1L << 25), /**< Custom flag, free to use by widget*/
    LV_OBJ_FLAG_WIDGET_2        = (1L << 26), /**< Custom flag, free to use by widget*/
    LV_OBJ_FLAG_USER_1          = (1L << 27), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_2          = (1L << 28), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_3          = (1L << 29), /**< Custom flag, free to use by user*/
    LV_OBJ_FLAG_USER_4          = (1L << 30), /**< Custom flag, free to use by user*/
} _lv_obj_flag_t;

#if LV_USE_OBJ_PROPERTY
enum {
    /*OBJ flag properties */
    LV_PROPERTY_ID(OBJ, FLAG_START,                 LV_PROPERTY_TYPE_INT,       0),
    LV_PROPERTY_ID(OBJ, FLAG_HIDDEN,                LV_PROPERTY_TYPE_INT,       0),
    LV_PROPERTY_ID(OBJ, FLAG_CLICKABLE,             LV_PROPERTY_TYPE_INT,       1),
    LV_PROPERTY_ID(OBJ, FLAG_CLICK_FOCUSABLE,       LV_PROPERTY_TYPE_INT,       2),
    LV_PROPERTY_ID(OBJ, FLAG_CHECKABLE,             LV_PROPERTY_TYPE_INT,       3),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLLABLE,            LV_PROPERTY_TYPE_INT,       4),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ELASTIC,        LV_PROPERTY_TYPE_INT,       5),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_MOMENTUM,       LV_PROPERTY_TYPE_INT,       6),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ONE,            LV_PROPERTY_TYPE_INT,       7),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_CHAIN_HOR,      LV_PROPERTY_TYPE_INT,       8),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_CHAIN_VER,      LV_PROPERTY_TYPE_INT,       9),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_ON_FOCUS,       LV_PROPERTY_TYPE_INT,       10),
    LV_PROPERTY_ID(OBJ, FLAG_SCROLL_WITH_ARROW,     LV_PROPERTY_TYPE_INT,       11),
    LV_PROPERTY_ID(OBJ, FLAG_SNAPPABLE,             LV_PROPERTY_TYPE_INT,       12),
    LV_PROPERTY_ID(OBJ, FLAG_PRESS_LOCK,            LV_PROPERTY_TYPE_INT,       13),
    LV_PROPERTY_ID(OBJ, FLAG_EVENT_BUBBLE,          LV_PROPERTY_TYPE_INT,       14),
    LV_PROPERTY_ID(OBJ, FLAG_GESTURE_BUBBLE,        LV_PROPERTY_TYPE_INT,       15),
    LV_PROPERTY_ID(OBJ, FLAG_ADV_HITTEST,           LV_PROPERTY_TYPE_INT,       16),
    LV_PROPERTY_ID(OBJ, FLAG_IGNORE_LAYOUT,         LV_PROPERTY_TYPE_INT,       17),
    LV_PROPERTY_ID(OBJ, FLAG_FLOATING,              LV_PROPERTY_TYPE_INT,       18),
    LV_PROPERTY_ID(OBJ, FLAG_SEND_DRAW_TASK_EVENTS, LV_PROPERTY_TYPE_INT,       19),
    LV_PROPERTY_ID(OBJ, FLAG_OVERFLOW_VISIBLE,      LV_PROPERTY_TYPE_INT,       20),
    LV_PROPERTY_ID(OBJ, FLAG_FLEX_IN_NEW_TRACK,     LV_PROPERTY_TYPE_INT,       21),
    LV_PROPERTY_ID(OBJ, FLAG_LAYOUT_1,              LV_PROPERTY_TYPE_INT,       23),
    LV_PROPERTY_ID(OBJ, FLAG_LAYOUT_2,              LV_PROPERTY_TYPE_INT,       24),
    LV_PROPERTY_ID(OBJ, FLAG_WIDGET_1,              LV_PROPERTY_TYPE_INT,       25),
    LV_PROPERTY_ID(OBJ, FLAG_WIDGET_2,              LV_PROPERTY_TYPE_INT,       26),
    LV_PROPERTY_ID(OBJ, FLAG_USER_1,                LV_PROPERTY_TYPE_INT,       27),
    LV_PROPERTY_ID(OBJ, FLAG_USER_2,                LV_PROPERTY_TYPE_INT,       28),
    LV_PROPERTY_ID(OBJ, FLAG_USER_3,                LV_PROPERTY_TYPE_INT,       29),
    LV_PROPERTY_ID(OBJ, FLAG_USER_4,                LV_PROPERTY_TYPE_INT,       30),
    LV_PROPERTY_ID(OBJ, FLAG_END,                   LV_PROPERTY_TYPE_INT,       30),

    LV_PROPERTY_ID(OBJ, STATE_START,                LV_PROPERTY_TYPE_INT,       31),
    LV_PROPERTY_ID(OBJ, STATE_CHECKED,              LV_PROPERTY_TYPE_INT,       31),
    LV_PROPERTY_ID(OBJ, STATE_FOCUSED,              LV_PROPERTY_TYPE_INT,       32),
    LV_PROPERTY_ID(OBJ, STATE_FOCUS_KEY,            LV_PROPERTY_TYPE_INT,       33),
    LV_PROPERTY_ID(OBJ, STATE_EDITED,               LV_PROPERTY_TYPE_INT,       34),
    LV_PROPERTY_ID(OBJ, STATE_HOVERED,              LV_PROPERTY_TYPE_INT,       35),
    LV_PROPERTY_ID(OBJ, STATE_PRESSED,              LV_PROPERTY_TYPE_INT,       36),
    LV_PROPERTY_ID(OBJ, STATE_SCROLLED,             LV_PROPERTY_TYPE_INT,       37),
    LV_PROPERTY_ID(OBJ, STATE_DISABLED,             LV_PROPERTY_TYPE_INT,       38),
    /*not used bit8-bit11*/
    LV_PROPERTY_ID(OBJ, STATE_USER_1,               LV_PROPERTY_TYPE_INT,       43),
    LV_PROPERTY_ID(OBJ, STATE_USER_2,               LV_PROPERTY_TYPE_INT,       44),
    LV_PROPERTY_ID(OBJ, STATE_USER_3,               LV_PROPERTY_TYPE_INT,       45),
    LV_PROPERTY_ID(OBJ, STATE_USER_4,               LV_PROPERTY_TYPE_INT,       46),
    LV_PROPERTY_ID(OBJ, STATE_ANY,                  LV_PROPERTY_TYPE_INT,       47),
    LV_PROPERTY_ID(OBJ, STATE_END,                  LV_PROPERTY_TYPE_INT,       47),

    /*OBJ normal properties*/
    LV_PROPERTY_ID(OBJ, PARENT,                     LV_PROPERTY_TYPE_POINTER,   31),

    LV_PROPERTY_OBJ_END,
};
#endif

/**
 * Make the base object's class publicly available.
 */
LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_obj_class;

/**
 * Special, rarely used attributes.
 * They are allocated automatically if any elements is set.
 */
typedef struct {
    lv_obj_t ** children;   /**< Store the pointer of the children in an array.*/
    lv_group_t * group_p;
    lv_event_list_t event_list;

    lv_point_t scroll;              /**< The current X/Y scroll offset*/

    int32_t ext_click_pad;          /**< Extra click padding in all direction*/
    int32_t ext_draw_size;          /**< EXTend the size in every direction for drawing.*/

    uint16_t child_cnt;             /**< Number of children*/
    uint16_t scrollbar_mode : 2;    /**< How to display scrollbars, see `lv_scrollbar_mode_t`*/
    uint16_t scroll_snap_x : 2;     /**< Where to align the snappable children horizontally, see `lv_scroll_snap_t`*/
    uint16_t scroll_snap_y : 2;     /**< Where to align the snappable children vertically*/
    uint16_t scroll_dir : 4;        /**< The allowed scroll direction(s), see `lv_dir_t`*/
    uint16_t layer_type : 2;        /**< Cache the layer type here. Element of @lv_intermediate_layer_type_t */
} _lv_obj_spec_attr_t;

struct _lv_obj_t {
    const lv_obj_class_t * class_p;
    lv_obj_t * parent;
    _lv_obj_spec_attr_t * spec_attr;
    _lv_obj_style_t * styles;
#if LV_OBJ_STYLE_CACHE
    uint32_t style_main_prop_is_set;
    uint32_t style_other_prop_is_set;
#endif
    void * user_data;
#if LV_USE_OBJ_ID
    void * id;
#endif
    lv_area_t coords;
    lv_obj_flag_t flags;
    lv_state_t state;
    uint16_t layout_inv : 1;
    uint16_t readjust_scroll_after_layout : 1;
    uint16_t scr_layout_inv : 1;
    uint16_t skip_trans : 1;
    uint16_t style_cnt  : 6;
    uint16_t h_layout   : 1;
    uint16_t w_layout   : 1;
    uint16_t is_deleting : 1;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a base object (a rectangle)
 * @param parent    pointer to a parent object. If NULL then a screen will be created.
 * @return          pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set one or more flags
 * @param obj   pointer to an object
 * @param f     R-ed values from `lv_obj_flag_t` to set.
 */
void lv_obj_add_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Remove one or more flags
 * @param obj   pointer to an object
 * @param f     OR-ed values from `lv_obj_flag_t` to set.
 */
void lv_obj_remove_flag(lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Set add or remove one or more flags.
 * @param obj   pointer to an object
 * @param f     OR-ed values from `lv_obj_flag_t` to update.
 * @param v     true: add the flags; false: remove the flags
 */
void lv_obj_update_flag(lv_obj_t * obj, lv_obj_flag_t f, bool v);

/**
 * Add one or more states to the object. The other state bits will remain unchanged.
 * If specified in the styles, transition animation will be started from the previous state to the current.
 * @param obj       pointer to an object
 * @param state     the states to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_add_state(lv_obj_t * obj, lv_state_t state);

/**
 * Remove one or more states to the object. The other state bits will remain unchanged.
 * If specified in the styles, transition animation will be started from the previous state to the current.
 * @param obj       pointer to an object
 * @param state     the states to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 */
void lv_obj_remove_state(lv_obj_t * obj, lv_state_t state);

/**
 * Add or remove one or more states to the object. The other state bits will remain unchanged.
 * @param obj       pointer to an object
 * @param state     the states to add. E.g `LV_STATE_PRESSED | LV_STATE_FOCUSED`
 * @param v         true: add the states; false: remove the states
 */
void lv_obj_set_state(lv_obj_t * obj, lv_state_t state, bool v);

/**
 * Set the user_data field of the object
 * @param obj   pointer to an object
 * @param user_data   pointer to the new user_data.
 */
static inline void lv_obj_set_user_data(lv_obj_t * obj, void * user_data)
{
    obj->user_data = user_data;
}

/*=======================
 * Getter functions
 *======================*/

/**
 * Check if a given flag or all the given flags are set on an object.
 * @param obj   pointer to an object
 * @param f     the flag(s) to check (OR-ed values can be used)
 * @return      true: all flags are set; false: not all flags are set
 */
bool lv_obj_has_flag(const lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Check if a given flag or any of the flags are set on an object.
 * @param obj   pointer to an object
 * @param f     the flag(s) to check (OR-ed values can be used)
 * @return      true: at lest one flag flag is set; false: none of the flags are set
 */
bool lv_obj_has_flag_any(const lv_obj_t * obj, lv_obj_flag_t f);

/**
 * Get the state of an object
 * @param obj   pointer to an object
 * @return      the state (OR-ed values from `lv_state_t`)
 */
lv_state_t lv_obj_get_state(const lv_obj_t * obj);

/**
 * Check if the object is in a given state or not.
 * @param obj       pointer to an object
 * @param state     a state or combination of states to check
 * @return          true: `obj` is in `state`; false: `obj` is not in `state`
 */
bool lv_obj_has_state(const lv_obj_t * obj, lv_state_t state);

/**
 * Get the group of the object
 * @param       obj pointer to an object
 * @return      the pointer to group of the object
 */
lv_group_t * lv_obj_get_group(const lv_obj_t * obj);

/**
 * Get the user_data field of the object
 * @param obj   pointer to an object
 * @return      the pointer to the user_data of the object
 */
static inline void * lv_obj_get_user_data(lv_obj_t * obj)
{
    return obj->user_data;
}

/*=======================
 * Other functions
 *======================*/

/**
 * Allocate special data for an object if not allocated yet.
 * @param obj   pointer to an object
 */
void lv_obj_allocate_spec_attr(lv_obj_t * obj);

/**
 * Check the type of obj.
 * @param obj       pointer to an object
 * @param class_p   a class to check (e.g. `lv_slider_class`)
 * @return          true: `class_p` is the `obj` class.
 */
bool lv_obj_check_type(const lv_obj_t * obj, const lv_obj_class_t * class_p);

/**
 * Check if any object has a given class (type).
 * It checks the ancestor classes too.
 * @param obj       pointer to an object
 * @param class_p   a class to check (e.g. `lv_slider_class`)
 * @return          true: `obj` has the given class
 */
bool lv_obj_has_class(const lv_obj_t * obj, const lv_obj_class_t * class_p);

/**
 * Get the class (type) of the object
 * @param obj   pointer to an object
 * @return      the class (type) of the object
 */
const lv_obj_class_t * lv_obj_get_class(const lv_obj_t * obj);

/**
 * Check if any object is still "alive".
 * @param obj       pointer to an object
 * @return          true: valid
 */
bool lv_obj_is_valid(const lv_obj_t * obj);

#if LV_USE_OBJ_ID

/**
 * Assign an id to an object if not previously assigned
 * Set `LV_USE_OBJ_ID_BUILTIN` to 1 to use builtin method to generate object ID.
 * Otherwise, these functions including `lv_obj_[assign|free|stringify]_id` should be implemented externally.
 *
 * @param class_p   the class this obj belongs to. Note obj->class_p is the class currently being constructed.
 * @param obj   pointer to an object
 */
void lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * obj);

/**
 * Free resources allocated by `lv_obj_assign_id`
 * @param obj   pointer to an object
 */
void lv_obj_free_id(lv_obj_t * obj);

/**
 * Format an object's id into a string.
 * @param obj   pointer to an object
 * @param buf   buffer to write the string into
 * @param len   length of the buffer
 */
const char * lv_obj_stringify_id(lv_obj_t * obj, char * buf, uint32_t len);

#if LV_USE_OBJ_ID_BUILTIN
/**
 * Free resources used by builtin ID generator.
 */
void lv_objid_builtin_destroy(void);

#endif

#endif /*LV_USE_OBJ_ID*/

/**********************
 *      MACROS
 **********************/

#if LV_USE_ASSERT_OBJ
#  define LV_ASSERT_OBJ(obj_p, obj_class)                                                               \
    do {                                                                                                \
        LV_ASSERT_MSG(obj_p != NULL, "The object is NULL");                                             \
        LV_ASSERT_MSG(lv_obj_has_class(obj_p, obj_class) == true, "Incompatible object type.");         \
        LV_ASSERT_MSG(lv_obj_is_valid(obj_p)  == true, "The object is invalid, deleted or corrupted?"); \
    } while(0)
# else
#  define LV_ASSERT_OBJ(obj_p, obj_class) do{}while(0)
#endif

#if LV_USE_LOG && LV_LOG_TRACE_OBJ_CREATE
#  define LV_TRACE_OBJ_CREATE(...) LV_LOG_TRACE(__VA_ARGS__)
#else
#  define LV_TRACE_OBJ_CREATE(...)
#endif

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_H*/

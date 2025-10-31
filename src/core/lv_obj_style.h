/**
 * @file lv_obj_style.h
 *
 */

#ifndef LV_OBJ_STYLE_H
#define LV_OBJ_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../misc/lv_bidi.h"
#include "../misc/lv_style.h"
#include "../misc/lv_types.h"

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
typedef enum {
    LV_STATE_DEFAULT     = 0x0000,
    LV_STATE_CHECKED     = 0x0001,
    LV_STATE_FOCUSED     = 0x0002,
    LV_STATE_FOCUS_KEY   = 0x0004,
    LV_STATE_EDITED      = 0x0008,
    LV_STATE_HOVERED     = 0x0010,
    LV_STATE_PRESSED     = 0x0020,
    LV_STATE_SCROLLED    = 0x0040,
    LV_STATE_DISABLED    = 0x0080,
    LV_STATE_USER_1      = 0x1000,
    LV_STATE_USER_2      = 0x2000,
    LV_STATE_USER_3      = 0x4000,
    LV_STATE_USER_4      = 0x8000,

    LV_STATE_ANY         = 0xFFFF,  /**< Special value can be used in some functions to target all states*/
} lv_state_t;

/**
 * The possible parts of widgets.
 * The parts can be considered as the internal building block of the widgets.
 * E.g. slider = background + indicator + knob
 * Not all parts are used by every widget
 */

typedef enum {
    LV_PART_MAIN         = 0x000000,  /**< A background like rectangle*/
    LV_PART_SCROLLBAR    = 0x010000,  /**< The scrollbar(s)*/
    LV_PART_INDICATOR    = 0x020000,  /**< Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox*/
    LV_PART_KNOB         = 0x030000,  /**< Like handle to grab to adjust the value*/
    LV_PART_SELECTED     = 0x040000,  /**< Indicate the currently selected option or section*/
    LV_PART_ITEMS        = 0x050000,  /**< Used if the widget has multiple similar elements (e.g. table cells)*/
    LV_PART_CURSOR       = 0x060000,  /**< Mark a specific place e.g. for text area's cursor or on a chart*/

    LV_PART_CUSTOM_FIRST = 0x080000,  /**< Extension point for custom widgets*/

    LV_PART_ANY          = 0x0F0000,  /**< Special value can be used in some functions to target all parts*/
} lv_part_t;

typedef enum {
    LV_STYLE_STATE_CMP_SAME,           /**< The style properties in the 2 states are identical */
    LV_STYLE_STATE_CMP_DIFF_REDRAW,    /**< The differences can be shown with a simple redraw */
    LV_STYLE_STATE_CMP_DIFF_DRAW_PAD,  /**< The differences can be shown with a simple redraw */
    LV_STYLE_STATE_CMP_DIFF_LAYOUT,    /**< The differences can be shown with a simple redraw */
} lv_style_state_cmp_t;

/**
 * A joint type for `lv_part_t` and `lv_state_t`. Example values
 * - `0`: means `LV_PART_MAIN | LV_STATE_DEFAULT`
 * - `LV_STATE_PRSSED`
 * - `LV_PART_KNOB`
 * - `LV_PART_KNOB | LV_STATE_PRESSED | LV_STATE_CHECKED`
 */
typedef uint32_t lv_style_selector_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Add a style to an object.
 * @param obj       pointer to an object
 * @param style     pointer to a style to add
 * @param selector  OR-ed value of parts and state to which the style should be added
 *
 * Examples:
 * @code
 * lv_obj_add_style(btn, &style_btn, 0); //Default button style
 *
 * lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED); //Overwrite only some colors to red when pressed
 * @endcode
 */
void lv_obj_add_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);

/**
 * Replaces a style of an object, preserving the order of the style stack (local styles and transitions are ignored).
 * @param obj           pointer to an object
 * @param old_style     pointer to a style to replace.
 * @param new_style     pointer to a style to replace the old style with.
 * @param selector      OR-ed values of states and a part to replace only styles with matching selectors. LV_STATE_ANY and LV_PART_ANY can be used
 *
 * Examples:
 * @code
 * lv_obj_replace_style(obj, &yellow_style, &blue_style, LV_PART_ANY | LV_STATE_ANY); //Replace a specific style
 *
 * lv_obj_replace_style(obj, &yellow_style, &blue_style, LV_PART_MAIN | LV_STATE_PRESSED); //Replace a specific style assigned to the main part when it is pressed
 * @endcode
 */
bool lv_obj_replace_style(lv_obj_t * obj, const lv_style_t * old_style, const lv_style_t * new_style,
                          lv_style_selector_t selector);

/**
 * Remove a style from an object.
 * @param obj       pointer to an object
 * @param style     pointer to a style to remove. Can be NULL to check only the selector
 * @param selector  OR-ed values of states and a part to remove only styles with matching selectors. LV_STATE_ANY and LV_PART_ANY can be used
 *
 * Examples:
 * @code
 * lv_obj_remove_style(obj, &style, LV_PART_ANY | LV_STATE_ANY); //Remove a specific style
 *
 * lv_obj_remove_style(obj, NULL, LV_PART_MAIN | LV_STATE_ANY); //Remove all styles from the main part
 *
 * lv_obj_remove_style(obj, NULL, LV_PART_ANY | LV_STATE_ANY); //Remove all styles
 * @endcode
 */
void lv_obj_remove_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);

/**
 * Remove all styles from an object
 * @param obj       pointer to an object
 */
void lv_obj_remove_style_all(lv_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style     pointer to a style. Only the objects with this style will be notified
 *                  (NULL to notify all objects)
 */
void lv_obj_report_style_change(lv_style_t * style);

/**
 * Notify an object and its children about its style is modified.
 * @param obj       pointer to an object
 * @param part      the part whose style was changed. E.g. `LV_PART_ANY`, `LV_PART_MAIN`
 * @param prop      `LV_STYLE_PROP_ANY` or an `LV_STYLE_...` property.
 *                  It is used to optimize what needs to be refreshed.
 *                  `LV_STYLE_PROP_INV` to perform only a style cache update
 */
void lv_obj_refresh_style(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Temporary disable a style for a selector. It will look like is the style wasn't added
 * @param obj       pointer to an object
 * @param style     pointer to a style
 * @param selector  the selector of a style (e.g. LV_STATE_PRESSED | LV_PART_KNOB)
 * @param dis       true: disable the style, false: enable the style
 */
void lv_obj_style_set_disabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector, bool dis);

/**
 * Get if a given style is disabled on an object.
 * @param obj       pointer to an object
 * @param style     pointer to a style
 * @param selector  the selector of a style (e.g. LV_STATE_PRESSED | LV_PART_KNOB)
 * @return          true: disable the style, false: enable the style
 */
bool lv_obj_style_get_disabled(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector);

/**
 * Enable or disable automatic style refreshing when a new style is added/removed to/from an object
 * or any other style change happens.
 * @param en        true: enable refreshing; false: disable refreshing
 */
void lv_obj_enable_style_refresh(bool en);

/**
 * Get the value of a style property. The current state of the object will be considered.
 * Inherited properties will be inherited.
 * If a property is not set a default value will be returned.
 * @param obj       pointer to an object
 * @param part      a part from which the property should be get
 * @param prop      the property to get
 * @return          the value of the property.
 *                  Should be read from the correct field of the `lv_style_value_t` according to the type of the property.
 */
lv_style_value_t lv_obj_get_style_prop(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Check if an object has a specified style property for a given style selector.
 * @param obj       pointer to an object
 * @param selector  the style selector to be checked, defining the scope of the style to be examined.
 * @param prop      the property to be checked.
 * @return          true if the object has the specified selector and property, false otherwise.
 */
bool lv_obj_has_style_prop(const lv_obj_t * obj, lv_style_selector_t selector, lv_style_prop_t prop);

/**
 * Set local style property on an object's part and state.
 * @param obj       pointer to an object
 * @param prop      the property
 * @param value     value of the property. The correct element should be set according to the type of the property
 * @param selector  OR-ed value of parts and state for which the style should be set
 */
void lv_obj_set_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t value,
                                 lv_style_selector_t selector);

lv_style_res_t lv_obj_get_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t * value,
                                           lv_style_selector_t selector);

/**
 * Remove a local style property from a part of an object with a given state.
 * @param obj       pointer to an object
 * @param prop      a style property to remove.
 * @param selector  OR-ed value of parts and state for which the style should be removed
 * @return true     the property was found and removed; false: the property was not found
 */
bool lv_obj_remove_local_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector);

/**
 * Used internally for color filtering
 */
lv_style_value_t lv_obj_style_apply_color_filter(const lv_obj_t * obj, lv_part_t part, lv_style_value_t v);

/**
 * Fade in an an object and all its children.
 * @param obj       the object to fade in
 * @param time      time of fade
 * @param delay     delay to start the animation
 */
void lv_obj_fade_in(lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Fade out an an object and all its children.
 * @param obj       the object to fade out
 * @param time      time of fade
 * @param delay     delay to start the animation
 */
void lv_obj_fade_out(lv_obj_t * obj, uint32_t time, uint32_t delay);

static inline lv_state_t lv_obj_style_get_selector_state(lv_style_selector_t selector)
{
    return (lv_state_t)(selector & 0xFFFF);
}

static inline lv_part_t lv_obj_style_get_selector_part(lv_style_selector_t selector)
{
    return (lv_part_t)(selector & 0xFF0000);
}

#include "lv_obj_style_gen.h"

static inline void lv_obj_set_style_pad_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
}

static inline void lv_obj_set_style_pad_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_margin_all(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_margin_hor(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_left(obj, value, selector);
    lv_obj_set_style_margin_right(obj, value, selector);
}

static inline void lv_obj_set_style_margin_ver(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_margin_top(obj, value, selector);
    lv_obj_set_style_margin_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_gap(lv_obj_t * obj, int32_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_row(obj, value, selector);
    lv_obj_set_style_pad_column(obj, value, selector);
}

static inline void lv_obj_set_style_size(lv_obj_t * obj, int32_t width, int32_t height,
                                         lv_style_selector_t selector)
{
    lv_obj_set_style_width(obj, width, selector);
    lv_obj_set_style_height(obj, height, selector);
}

static inline void lv_obj_set_style_transform_scale(lv_obj_t * obj, int32_t value,
                                                    lv_style_selector_t selector)
{
    lv_obj_set_style_transform_scale_x(obj, value, selector);
    lv_obj_set_style_transform_scale_y(obj, value, selector);
}

static inline int32_t lv_obj_get_style_space_left(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_left(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_LEFT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_right(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_right(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_RIGHT) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_top(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_top(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_TOP) ? padding + border_width : padding;
}

static inline int32_t lv_obj_get_style_space_bottom(const lv_obj_t * obj, lv_part_t part)
{
    int32_t padding = lv_obj_get_style_pad_bottom(obj, part);
    int32_t border_width = lv_obj_get_style_border_width(obj, part);
    lv_border_side_t border_side = lv_obj_get_style_border_side(obj, part);
    return (border_side & LV_BORDER_SIDE_BOTTOM) ? padding + border_width : padding;
}

lv_text_align_t lv_obj_calculate_style_text_align(const lv_obj_t * obj, lv_part_t part, const char * txt);

static inline int32_t lv_obj_get_style_transform_scale_x_safe(const lv_obj_t * obj, lv_part_t part)
{
    int32_t scale = lv_obj_get_style_transform_scale_x(obj, part);
    return scale > 0 ? scale : 1;
}

static inline int32_t lv_obj_get_style_transform_scale_y_safe(const lv_obj_t * obj, lv_part_t part)
{
    int32_t scale = lv_obj_get_style_transform_scale_y(obj, part);
    return scale > 0 ? scale : 1;
}

/**
 * Get the `opa` style property from all parents and multiply and `>> 8` them.
 * @param obj       the object whose opacity should be get
 * @param part      the part whose opacity should be get. Non-MAIN parts will consider the `opa` of the MAIN part too
 * @return          the final opacity considering the parents' opacity too
 */
lv_opa_t lv_obj_get_style_opa_recursive(const lv_obj_t * obj, lv_part_t part);


/**
 * Apply recolor effect to the input color based on the object's style properties.
 * @param obj       the target object containing recolor style properties
 * @param part      the part to retrieve recolor styles.
 * @param color     the original color to be modified
 * @return          the blended color after applying recolor and opacity
 */
lv_color32_t lv_obj_style_apply_recolor(const lv_obj_t * obj, lv_part_t part, lv_color32_t color);

/**
 * Get the `recolor` style property from all parents and blend them recursively.
 * @param obj       the object whose recolor value should be retrieved
 * @param part      the target part to check. Non-MAIN parts will also consider
 *                  the `recolor` value from the MAIN part during calculation
 * @return          the final blended recolor value combining all parent's recolor values
 */
lv_color32_t lv_obj_get_style_recolor_recursive(const lv_obj_t * obj, lv_part_t part);

#if LV_USE_OBSERVER
/**
 * Disable a style if a subject's value is not equal to a reference value
 * @param obj           pointer to Widget
 * @param style         pointer to a style
 * @param selector      pointer to a selector
 * @param subject       pointer to Subject
 * @param ref_value     reference value to compare Subject's value with
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_style(lv_obj_t * obj, const lv_style_t * style, lv_style_selector_t selector,
                                  lv_subject_t * subject, int32_t ref_value);

/**
 * Connect a subject's value to a style property of a widget.
 * @param obj       pointer to a Widget
 * @param prop      a style property
 * @param selector  a selector for which the property should be added, e.g. `LV_PART_KNOB | LV_STATE_PRESSED`
 * @param subject   pointer a Subject to which value the property should be bound
 * @return              pointer to newly-created Observer
 */
lv_observer_t * lv_obj_bind_style_prop(lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector,
                                       lv_subject_t * subject);

#endif

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_STYLE_H*/

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
#include <stdint.h>
#include <stdbool.h>
#include "../misc/lv_bidi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Can't include lv_obj.h because it includes this header file*/
struct _lv_obj_t;

typedef enum {
    _LV_STYLE_STATE_CMP_SAME,           /*The style properties in the 2 states are identical*/
    _LV_STYLE_STATE_CMP_DIFF_REDRAW,    /*The differences can be shown with a simple redraw*/
    _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD,  /*The differences can be shown with a simple redraw*/
    _LV_STYLE_STATE_CMP_DIFF_LAYOUT,    /*The differences can be shown with a simple redraw*/
} _lv_style_state_cmp_t;

typedef uint32_t lv_style_selector_t;

typedef struct {
    lv_style_t * style;
    uint32_t selector : 24;
    uint32_t is_local : 1;
    uint32_t is_trans : 1;
} _lv_obj_style_t;

typedef struct {
    uint16_t time;
    uint16_t delay;
    lv_style_selector_t selector;
    lv_style_prop_t prop;
    lv_anim_path_cb_t path_cb;
#if LV_USE_USER_DATA
    void * user_data;
#endif
} _lv_obj_style_transition_dsc_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the object related style manager module.
 * Called by LVGL in `lv_init()`
 */
void _lv_obj_style_init(void);

/**
 * Add a style to an object.
 * @param obj       pointer to an object
 * @param style     pointer to a style to add
 * @param selector  OR-ed value of parts and state to which the style should be added
 * @example         lv_obj_add_style(btn, &style_btn, 0); //Default button style
 * @example         lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED); //Overwrite only some colors to red when pressed
 */
void lv_obj_add_style(struct _lv_obj_t * obj, lv_style_t * style, lv_style_selector_t selector);

/**
 * Add a style to an object.
 * @param obj       pointer to an object
 * @param style     pointer to a style to remove. Can be NULL to check only the selector
 * @param selector  OR-ed values of states and a part to remove only styles with matching selectors. LV_STATE_ANY and LV_PART_ANY can be used
 * @example lv_obj_remove_style(obj, &style, LV_PART_ANY | LV_STATE_ANY); //Remove a specific style
 * @example lv_obj_remove_style(obj, NULL, LV_PART_MAIN | LV_STATE_ANY); //Remove all styles from the main part
 * @example lv_obj_remove_style(obj, NULL, LV_PART_ANY | LV_STATE_ANY); //Remove all styles
 */
void lv_obj_remove_style(struct _lv_obj_t * obj, lv_style_t * style, lv_style_selector_t selector);

/**
 * Remove all styles from an object
 * @param obj       pointer to an object
 */
static inline void lv_obj_remove_style_all(struct _lv_obj_t * obj)
{
    lv_obj_remove_style(obj, NULL, LV_PART_ANY | LV_STATE_ANY);
}

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
void lv_obj_refresh_style(struct _lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

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
lv_style_value_t lv_obj_get_style_prop(const struct _lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Set local style property on an object's part and state.
 * @param obj       pointer to an object
 * @param part      a part to which the property should be added
 * @param state     a state to which the property should be added
 * @param prop      the property
 * @param value     value of the property. The correct element should be set according to the type of the property
 */
void lv_obj_set_local_style_prop(struct _lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t value,
                                 lv_style_selector_t selector);

lv_res_t lv_obj_get_local_style_prop(struct _lv_obj_t * obj, lv_style_prop_t prop, lv_style_value_t * value,
                                     lv_style_selector_t selector);

/**
 * Remove a local style property from a part of an object with a given state.
 * @param obj       pointer to an object
 * @param part      the part of the object which style property should be removed.
 * @param state     the state from which the property should be removed.
 * @param prop      a style property to remove.
 * @return true     the property was found and removed; false: the property was not found
 */
bool lv_obj_remove_local_style_prop(struct _lv_obj_t * obj, lv_style_prop_t prop, lv_style_selector_t selector);

/**
 * Used internally to create a style transition
 * @param obj
 * @param part
 * @param prev_state
 * @param new_state
 * @param tr
 */
void _lv_obj_style_create_transition(struct _lv_obj_t * obj, lv_part_t part, lv_state_t prev_state,
                                     lv_state_t new_state, const _lv_obj_style_transition_dsc_t * tr);

/**
 * Used internally to compare the appearance of an object in 2 states
 * @param obj
 * @param state1
 * @param state2
 * @return
 */
_lv_style_state_cmp_t _lv_obj_style_state_compare(struct _lv_obj_t * obj, lv_state_t state1, lv_state_t state2);

/**
 * Fade in an an object and all its children.
 * @param obj       the object to fade in
 * @param time      time of fade
 * @param delay     delay to start the animation
 */
void lv_obj_fade_in(struct _lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Fade out an an object and all its children.
 * @param obj       the object to fade out
 * @param time      time of fade
 * @param delay     delay to start the animation
 */
void lv_obj_fade_out(struct _lv_obj_t * obj, uint32_t time, uint32_t delay);

lv_state_t lv_obj_style_get_selector_state(lv_style_selector_t selector);

lv_part_t lv_obj_style_get_selector_part(lv_style_selector_t selector);

#include "lv_obj_style_gen.h"

static inline void lv_obj_set_style_pad_all(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_hor(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_left(obj, value, selector);
    lv_obj_set_style_pad_right(obj, value, selector);
}

static inline void lv_obj_set_style_pad_ver(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_top(obj, value, selector);
    lv_obj_set_style_pad_bottom(obj, value, selector);
}

static inline void lv_obj_set_style_pad_gap(struct _lv_obj_t * obj,  lv_coord_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_pad_row(obj, value, selector);
    lv_obj_set_style_pad_column(obj, value, selector);
}

static inline void lv_obj_set_style_size(struct _lv_obj_t * obj,  lv_coord_t value, lv_style_selector_t selector)
{
    lv_obj_set_style_width(obj, value, selector);
    lv_obj_set_style_height(obj, value, selector);
}

lv_text_align_t lv_obj_calculate_style_text_align(const struct _lv_obj_t * obj, lv_part_t part, const char * txt);

static inline lv_coord_t lv_obj_get_x_aligned(const struct _lv_obj_t * obj)
{
    return lv_obj_get_style_x(obj, LV_PART_MAIN);
}

static inline lv_coord_t lv_obj_get_y_aligned(const struct _lv_obj_t * obj)
{
    return lv_obj_get_style_y(obj, LV_PART_MAIN);
}

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEMPL_H*/

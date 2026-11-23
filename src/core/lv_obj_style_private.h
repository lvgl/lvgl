/**
 * @file lv_obj_style_private.h
 *
 */

#ifndef LV_OBJ_STYLE_PRIVATE_H
#define LV_OBJ_STYLE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../lvgl_public.h"

/*********************
 *      DEFINES
 *********************/

/*The bit of a property in the `style_main/other_prop_is_set` cache*/
#define LV_OBJ_STYLE_PROP_SHIFTED(prop) ((uint32_t)1 << ((prop) >> 3))

/*Property flags that can change the area where the widget is drawn.
 *Before changing such a property the old area needs to be invalidated,
 *else the already drawn pixels are left on the screen.*/
#define LV_OBJ_STYLE_DRAW_AREA_FLAGS (LV_STYLE_PROP_FLAG_TRANSFORM | LV_STYLE_PROP_FLAG_EXT_DRAW_UPDATE)

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_obj_style_t {
    const lv_style_t * style;
    uint32_t selector : 24;
    uint32_t is_local : 1;
    uint32_t is_trans : 1;
    uint32_t is_disabled : 1;
    uint32_t is_theme : 1;  /**< The style is added by a theme */
};

struct _lv_obj_style_transition_dsc_t {
    lv_anim_path_cb_t path_cb;
    void * user_data;
    lv_style_selector_t selector;
    uint16_t time;
    uint16_t delay;
    lv_style_prop_t prop;
};

/**
 * Describes how `obj->styles` looks before and after a style change,
 * so that the start and end values of the transitions can be read.
 */
typedef struct {
    uint64_t mask;                  /**< The affected entries of `obj->styles`*/
    const lv_style_t * style_before;
    const lv_style_t * style_after;
    bool disabled_before;
    bool disabled_after;
} lv_obj_style_change_t;

/**
 * Transitions collected before a style change and started after it
 */
typedef struct {
    struct _lv_obj_style_pending_trans_t * items;
    uint32_t cnt;
} lv_obj_style_trans_batch_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the object related style manager module.
 * Called by LVGL in `lv_init()`
 */
void lv_obj_style_init(void);

/**
 * Deinitialize the object related style manager module.
 * Called by LVGL in `lv_deinit()`
 */
void lv_obj_style_deinit(void);

/**
 * Used internally to create a style transition
 * @param obj
 * @param part
 * @param prev_state
 * @param new_state
 * @param tr_dsc
 */
void lv_obj_style_create_transition(lv_obj_t * obj, lv_part_t part, lv_state_t prev_state,
                                    lv_state_t new_state, const lv_obj_style_transition_dsc_t * tr_dsc);

/**
 * Used internally to start the transitions of a state change.
 * Only the transitions having the `LV_STYLE_TRANSITION_TYPE_STATE` type are started.
 * @param obj           pointer to a widget
 * @param prev_state    the previous state
 * @param new_state     the new state (already set in `obj`)
 */
void lv_obj_style_start_state_transitions(lv_obj_t * obj, lv_state_t prev_state, lv_state_t new_state);

/**
 * Initialize the style transition module. Called from `lv_obj_style_init()`
 */
void lv_obj_style_transition_init(void);

/**
 * Deinitialize the style transition module. Called from `lv_obj_style_deinit()`
 */
void lv_obj_style_transition_deinit(void);

/**
 * Stop the running transitions of a widget and remove their values from the transition style.
 * It doesn't update the style cache.
 * @param obj       pointer to a widget
 * @param part      the part of the transitions or `LV_PART_ANY`
 * @param prop      the property of the transitions or `LV_STYLE_PROP_ANY`
 * @return          true: at least one transition was removed
 */
bool lv_obj_style_remove_transitions(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);

/**
 * Quick check whether a style change on a part can start transitions at all.
 * @param obj       pointer to a widget
 * @param part      the changed part
 * @return          false: no transitions can be started, true: there might be transitions
 */
bool lv_obj_style_transition_allowed(const lv_obj_t * obj, lv_part_t part);

/**
 * Set the affected style entries of a widget to their before or after state
 * @param obj       pointer to a widget
 * @param c         the style change descriptor
 * @param after     true: set the after state; false: set the before state
 */
void lv_obj_style_change_apply(lv_obj_t * obj, const lv_obj_style_change_t * c, bool after);

/**
 * Collect the `LV_STYLE_TRANSITION_TYPE_STYLE` transitions of a style change
 * and read their start and end values.
 * Before calling it, the widget's styles need to be in the "after" state and the style cache
 * needs to contain the properties of both the "before" and "after" state.
 * The styles are left in the "after" state.
 * @param obj       pointer to a widget
 * @param part      the changed part
 * @param c         the style change descriptor
 * @param batch     the collected transitions are stored here. Pass it to `lv_obj_style_transition_start()`.
 */
void lv_obj_style_transition_prepare_change(lv_obj_t * obj, lv_part_t part, const lv_obj_style_change_t * c,
                                            lv_obj_style_trans_batch_t * batch);

/**
 * Collect the `LV_STYLE_TRANSITION_TYPE_LOCAL` transition of a local style property
 * and save its currently shown value. Call it before changing the local property.
 * The end value is read in `lv_obj_style_transition_start()`.
 * @param obj       pointer to a widget
 * @param part      the changed part
 * @param prop      the changed property
 * @param batch     the collected transition is stored here. Pass it to `lv_obj_style_transition_start()`.
 */
void lv_obj_style_transition_prepare_local(lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop,
                                           lv_obj_style_trans_batch_t * batch);

/**
 * Start the prepared transitions whose start and end values differ, and free the batch.
 * The style of the widget is not refreshed.
 * @param obj       pointer to a widget
 * @param batch     the prepared transitions
 */
void lv_obj_style_transition_start(lv_obj_t * obj, lv_obj_style_trans_batch_t * batch);

/**
 * Recalculate the style property cache of a widget
 * @param obj       pointer to a widget
 * @param part      the part to recalculate or `LV_PART_ANY`
 */
void lv_obj_style_full_cache_refresh(lv_obj_t * obj, lv_part_t part);

/**
 * Used internally to compare the appearance of an object in 2 states
 * @param obj
 * @param state1
 * @param state2
 * @param changed_part  optional (may be NULL) out-param. Receives the part that
 *                      the differing styles belong to, or `LV_PART_ANY` if more
 *                      than one part differs (or none does). Lets the caller scope
 *                      the refresh to a single part when possible.
 * @return
 */
lv_style_state_cmp_t lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2,
                                                lv_part_t * changed_part);

/**
 * Update the layer type of a widget bayed on its current styles.
 * The result will be stored in `obj->spec_attr->layer_type`
 * @param obj       the object whose layer should be updated
 */
void lv_obj_update_layer_type(lv_obj_t * obj);

/**
 * Internal implementation of @ref lv_obj_get_style_prop
 * Does not do any argument checking
 */
lv_style_value_t lv_obj_get_style_prop_internal(const lv_obj_t * obj, lv_part_t part, lv_style_prop_t prop);


/**
 * Internal implementation of @ref lv_obj_style_apply_color_filter
 * Does not do any argument checking
 */
lv_style_value_t lv_obj_style_apply_color_filter_internal(const lv_obj_t * obj, lv_part_t part, lv_style_value_t v);

/**
 * Update the cached blur status of a widget based on its current styles and
 * state, and maintain the global count of widgets with blur (blur_obj_cnt in
 * lv_global_t). The result will be stored in `obj->has_blur`.
 * @param obj       the object whose blur status should be updated
 */
void lv_obj_update_blur_status(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_STYLE_PRIVATE_H*/

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

#include "lv_obj_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_obj_style_t {
    const lv_style_t * style;
    uint32_t selector : 24;
    uint32_t is_local : 1;
    uint32_t is_trans : 1;
};

struct lv_obj_style_transition_dsc_t {
    uint16_t time;
    uint16_t delay;
    lv_style_selector_t selector;
    lv_style_prop_t prop;
    lv_anim_path_cb_t path_cb;
    void * user_data;
};


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
 * @param tr
 */
void lv_obj_style_create_transition(lv_obj_t * obj, lv_part_t part, lv_state_t prev_state,
                                    lv_state_t new_state, const lv_obj_style_transition_dsc_t * tr);

/**
 * Used internally to compare the appearance of an object in 2 states
 * @param obj
 * @param state1
 * @param state2
 * @return
 */
lv_style_state_cmp_t lv_obj_style_state_compare(lv_obj_t * obj, lv_state_t state1, lv_state_t state2);

/**
 * Update the layer type of a widget bayed on its current styles.
 * The result will be stored in `obj->spec_attr->layer_type`
 * @param obj       the object whose layer should be updated
 */
void lv_obj_update_layer_type(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_OBJ_STYLE_PRIVATE_H*/

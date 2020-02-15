
/**
 * @file lv_obj_style_dec.h
 *
 */

#ifndef LV_OBJ_STYLE_DEC_H
#define LV_OBJ_STYLE_DEC_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/**
 * Macro to declare the most important style set/get API functions.
 *
 * - Get the value of a style property from an object in the object's current state.
 *   If there is a transition animation in progress calculate the value accordingly.
 *   If the property is not set in the object's style check the parent(s) if the property can be inherited
 *   If still not found return a default value.
 *   For example:
 *      `lv_obj_get_style_border_width(btn1, LV_BTN_PART_MAIN);`
 *
 * - Set a local style property for an object in a given state
 *   For example:
 *      `lv_obj_set_style_border_width(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, 2);`
 *
 *  - Get the value from a style in a given state:
 *    For example
 *      `int16_t weight = lv_style_get_border_width(&style1, LV_STATE_PRESSED, &result);`
 *      `if(weight > 0) ...the property is found and loaded into result...`
 *
 *  - Set a value in a style in a given state
 *     For example
 *       `lv_style_set_border_width(&style1, LV_STATE_PRESSED, 2);`
 */


#define _LV_OBJ_STYLE_DECLARE_GET_scalar(prop_name, func_name, value_type, style_type) \
static inline value_type lv_obj_get_style_##func_name (const lv_obj_t * obj, uint8_t part)  \
{                                                                                           \
    return (value_type) _lv_obj_get_style##style_type (obj, part, LV_STYLE_##prop_name);     \
}

#define _LV_OBJ_STYLE_DECLARE_GET_nonscalar(prop_name, func_name, value_type, style_type) \
static inline value_type lv_obj_get_style_##func_name (const lv_obj_t * obj, uint8_t part)  \
{                                                                                           \
    return _lv_obj_get_style##style_type (obj, part, LV_STYLE_##prop_name);     \
}

#define _LV_OBJ_STYLE_SET_GET_DECLARE(prop_name, func_name, value_type, style_type, scalar)          \
_LV_OBJ_STYLE_DECLARE_GET_##scalar(prop_name, func_name, value_type, style_type) \
static inline void lv_obj_set_style_##func_name (lv_obj_t * obj, uint8_t part, lv_state_t state, value_type value)  \
{                                                                                           \
    _lv_obj_set_style##style_type (obj, part, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);                  \
}                                                                                           \
static inline int16_t lv_style_get_##func_name (lv_style_t * style, lv_state_t state, void * res)             \
{                                                                                           \
    return _lv_style_get##style_type (style, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), res);                     \
}                                                                                           \
static inline void lv_style_set_##func_name (lv_style_t * style, lv_state_t state, value_type value)          \
{                                                                                           \
    _lv_style_set##style_type (style, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);                          \
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/

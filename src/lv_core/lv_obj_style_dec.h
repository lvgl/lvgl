
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
 * Get the value of a style property from an object in the object's current state
 * -----------------------------------------------------------------------------
 *  - Get the value of a style property from an object in the object's current state.
 *  - Transition animation is taken into account.
 *  - If the property is not set in the object's styles check the parent(s) if the property can be inherited
 *  - If still not found return a default value.
 *  - For example:
 *      `lv_style_int_t w = lv_obj_get_style_border_width(btn1, LV_BTN_PART_MAIN);`
 *
 * Set a local style property for an object in a given state
 * ---------------------------------------------------------
 *  - For example:
 *      `lv_obj_set_style_local_border_width(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, 2);`
 *
 * Get a local style property's value of an object in a given state
 * ----------------------------------------------------------------
 *  - Return the best matching property in the given state.
 *  - E.g. if `state` parameter is LV_STATE_PRESSED | LV_STATE_CHECKED` but the property defined only in
 *    `LV_STATE_PRESSED` and `LV_STATE_DEFAULT` the best matching state is `LV_STATE_PRESSED`
 *    (because it has higher precedence) and it will be returned.
 *  - If the property is not found even in `LV_STATE_DEFAULT` `-1` is returned.
 *  - For example:
 *      `//Type of result should be lv_style_int_t/lv_opa_t/lv_color_t/const void * according to the type of the property`
 *      `lv_style_int_t result;`
 *      `lv_obj_get_style_local_border_width(btn1, LV_BTN_PART_MAIN, LV_STATE_PRESSED, &result);`
 *      `if(weight > 0) ...the property is found and loaded into result...`
 *
 * Get the value from a style in a given state
 * -------------------------------------------
 * - The same rules applies to the return value then for "lv_obj_get_style_local_...()" above
 * - For example
 *      `int16_t weight = lv_style_get_border_width(&style1, LV_STATE_PRESSED, &result);`
 *      `if(weight > 0) ...the property is found and loaded into result...`

 * Set a value in a style in a given state
 * ---------------------------------------
 * - For example
 *      `lv_style_set_border_width(&style1, LV_STATE_PRESSED, 2);`
 */

#define _OBJ_GET_STYLE_scalar(prop_name, func_name, value_type, style_type)                         \
    static inline value_type lv_obj_get_style_##func_name (const lv_obj_t * obj, uint8_t part)      \
    {                                                                                               \
        return (value_type) _lv_obj_get_style##style_type (obj, part, LV_STYLE_##prop_name);        \
    }

#define _OBJ_GET_STYLE_nonscalar(prop_name, func_name, value_type, style_type)                      \
    static inline value_type lv_obj_get_style_##func_name (const lv_obj_t * obj, uint8_t part)      \
    {                                                                                               \
        return _lv_obj_get_style##style_type (obj, part, LV_STYLE_##prop_name);                     \
    }

#define _OBJ_SET_STYLE_LOCAL_scalar(prop_name, func_name, value_type, style_type)                                               \
    static inline void lv_obj_set_style_local_##func_name (lv_obj_t * obj, uint8_t part, lv_state_t state, value_type value)    \
    {                                                                                                                           \
        _lv_obj_set_style_local##style_type (obj, part, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);           \
    }

#define _OBJ_SET_STYLE_LOCAL_nonscalar(prop_name, func_name, value_type, style_type)                                            \
    static inline void lv_obj_set_style_local_##func_name (lv_obj_t * obj, uint8_t part, lv_state_t state, value_type value)    \
    {                                                                                                                           \
        _lv_obj_set_style_local##style_type (obj, part, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);           \
    }

#define _OBJ_SET_STYLE_scalar(prop_name, func_name, value_type, style_type)                                             \
    static inline void lv_style_set_##func_name (lv_style_t * style, lv_state_t state, value_type value)                 \
    {                                                                                                                    \
        _lv_style_set##style_type (style, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);                  \
    }

#define _OBJ_SET_STYLE_nonscalar(prop_name, func_name, value_type, style_type)                                          \
    static inline void lv_style_set_##func_name (lv_style_t * style, lv_state_t state, value_type value)                 \
    {                                                                                                                    \
        _lv_style_set##style_type (style, LV_STYLE_##prop_name | (state << LV_STYLE_STATE_POS), value);                  \
    }

#define _LV_OBJ_STYLE_SET_GET_DECLARE(prop_name, func_name, value_type, style_type, scalar)                             \
    _OBJ_GET_STYLE_##scalar(prop_name, func_name, value_type, style_type)                                               \
    _OBJ_SET_STYLE_LOCAL_##scalar(prop_name, func_name, value_type, style_type)                                         \
    _OBJ_SET_STYLE_##scalar(prop_name, func_name, value_type, style_type)

_LV_OBJ_STYLE_SET_GET_DECLARE(RADIUS, radius, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(CLIP_CORNER, clip_corner, bool, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SIZE, size, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_WIDTH, transform_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_HEIGHT, transform_height, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_ANGLE, transform_angle, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSFORM_ZOOM, transform_zoom, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OPA_SCALE, opa_scale, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_TOP, pad_top, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_BOTTOM, pad_bottom, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_LEFT, pad_left, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_RIGHT, pad_right, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PAD_INNER, pad_inner, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_TOP, margin_top, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_BOTTOM, margin_bottom, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_LEFT, margin_left, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(MARGIN_RIGHT, margin_right, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_BLEND_MODE, bg_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_MAIN_STOP, bg_main_stop, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_STOP, bg_grad_stop, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_DIR, bg_grad_dir, lv_grad_dir_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_COLOR, bg_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_GRAD_COLOR, bg_grad_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BG_OPA, bg_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_WIDTH, border_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_SIDE, border_side, lv_border_side_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_BLEND_MODE, border_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_POST, border_post, bool, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_COLOR, border_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(BORDER_OPA, border_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_WIDTH, outline_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_PAD, outline_pad, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_BLEND_MODE, outline_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_COLOR, outline_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(OUTLINE_OPA, outline_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_WIDTH, shadow_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OFS_X, shadow_ofs_x, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OFS_Y, shadow_ofs_y, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_SPREAD, shadow_spread, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_BLEND_MODE, shadow_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_COLOR, shadow_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SHADOW_OPA, shadow_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_REPEAT, pattern_repeat, bool, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_BLEND_MODE, pattern_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_RECOLOR, pattern_recolor, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_OPA, pattern_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_RECOLOR_OPA, pattern_recolor_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(PATTERN_IMAGE, pattern_image, const void *, _ptr, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_LETTER_SPACE, value_letter_space, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_LINE_SPACE, value_line_space, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_BLEND_MODE, value_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OFS_X, value_ofs_x, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OFS_Y, value_ofs_y, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_ALIGN, value_align, lv_align_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_COLOR, value_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_OPA, value_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_FONT, value_font, const lv_font_t *, _ptr, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(VALUE_STR, value_str, const char *, _ptr, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_LETTER_SPACE, text_letter_space, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_LINE_SPACE, text_line_space, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_DECOR, text_decor, lv_text_decor_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_BLEND_MODE, text_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_COLOR, text_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_SEL_COLOR, text_sel_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_SEL_BG_COLOR, text_sel_bg_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_OPA, text_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TEXT_FONT, text_font, const lv_font_t *, _ptr, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_WIDTH, line_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_BLEND_MODE, line_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_DASH_WIDTH, line_dash_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_DASH_GAP, line_dash_gap, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_ROUNDED, line_rounded, bool, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_COLOR, line_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(LINE_OPA, line_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_BLEND_MODE, image_blend_mode, lv_blend_mode_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_RECOLOR, image_recolor, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_OPA, image_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(IMAGE_RECOLOR_OPA, image_recolor_opa, lv_opa_t, _opa, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_TIME, transition_time, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_DELAY, transition_delay, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_1, transition_prop_1, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_2, transition_prop_2, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_3, transition_prop_3, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_4, transition_prop_4, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_5, transition_prop_5, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PROP_6, transition_prop_6, lv_style_int_t, _int, scalar)
#if LV_USE_ANIMATION
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PATH, transition_path, lv_anim_path_t *, _ptr, scalar)
#else
/*For compatibility*/
_LV_OBJ_STYLE_SET_GET_DECLARE(TRANSITION_PATH, transition_path, const void *, _ptr, scalar)
#endif
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_WIDTH, scale_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_BORDER_WIDTH, scale_border_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_BORDER_WIDTH, scale_end_border_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_LINE_WIDTH, scale_end_line_width, lv_style_int_t, _int, scalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_GRAD_COLOR, scale_grad_color, lv_color_t, _color, nonscalar)
_LV_OBJ_STYLE_SET_GET_DECLARE(SCALE_END_COLOR, scale_end_color, lv_color_t, _color, nonscalar)

#undef _LV_OBJ_STYLE_SET_GET_DECLARE


static inline void lv_obj_set_style_local_pad_all(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_top(obj, part, state, value);
    lv_obj_set_style_local_pad_bottom(obj, part, state, value);
    lv_obj_set_style_local_pad_left(obj, part, state, value);
    lv_obj_set_style_local_pad_right(obj, part, state, value);
}


static inline void lv_style_set_pad_all(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_top(style, state, value);
    lv_style_set_pad_bottom(style, state, value);
    lv_style_set_pad_left(style, state, value);
    lv_style_set_pad_right(style, state, value);
}


static inline void lv_obj_set_style_local_pad_hor(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_left(obj, part, state, value);
    lv_obj_set_style_local_pad_right(obj, part, state, value);
}


static inline void lv_style_set_pad_hor(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_left(style, state, value);
    lv_style_set_pad_right(style, state, value);
}


static inline void lv_obj_set_style_local_pad_ver(lv_obj_t * obj, uint8_t part, lv_state_t state, lv_style_int_t value)
{
    lv_obj_set_style_local_pad_top(obj, part, state, value);
    lv_obj_set_style_local_pad_bottom(obj, part, state, value);
}


static inline void lv_style_set_pad_ver(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_pad_top(style, state, value);
    lv_style_set_pad_bottom(style, state, value);
}


static inline void lv_obj_set_style_local_margin_all(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_top(obj, part, state, value);
    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
    lv_obj_set_style_local_margin_left(obj, part, state, value);
    lv_obj_set_style_local_margin_right(obj, part, state, value);
}


static inline void lv_style_set_margin_all(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_top(style, state, value);
    lv_style_set_margin_bottom(style, state, value);
    lv_style_set_margin_left(style, state, value);
    lv_style_set_margin_right(style, state, value);
}


static inline void lv_obj_set_style_local_margin_hor(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_left(obj, part, state, value);
    lv_obj_set_style_local_margin_right(obj, part, state, value);
}


static inline void lv_style_set_margin_hor(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_left(style, state, value);
    lv_style_set_margin_right(style, state, value);
}


static inline void lv_obj_set_style_local_margin_ver(lv_obj_t * obj, uint8_t part, lv_state_t state,
                                                     lv_style_int_t value)
{
    lv_obj_set_style_local_margin_top(obj, part, state, value);
    lv_obj_set_style_local_margin_bottom(obj, part, state, value);
}


static inline void lv_style_set_margin_ver(lv_style_t * style, lv_state_t state, lv_style_int_t value)
{
    lv_style_set_margin_top(style, state, value);
    lv_style_set_margin_bottom(style, state, value);
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/

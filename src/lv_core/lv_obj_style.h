/**
 * @file lv_obj_style.h
 *
 */

#ifndef LV_OBJ_STYLE_H
#define LV_OB_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stdbool.h>
#include "lv_style.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/* Can't include lv_obj.h because it includes this header file */
struct _lv_obj_t;

typedef enum {
    _LV_STYLE_STATE_CMP_SAME,           /*The style properties in the 2 states are identical*/
    _LV_STYLE_STATE_CMP_DIFF_REDRAW,    /*The differences can be shown with a simple redraw*/
    _LV_STYLE_STATE_CMP_DIFF_DRAW_PAD,  /*The differences can be shown with a simple redraw*/
    _LV_STYLE_STATE_CMP_DIFF_LAYOUT,    /*The differences can be shown with a simple redraw*/
} _lv_style_state_cmp_t;

typedef struct {
    lv_style_t * style;
    uint16_t state;
    uint8_t part;
    uint8_t is_local :1;
    uint8_t is_trans :1;
}lv_obj_style_t;

typedef struct {
    lv_obj_style_t * styles;
    uint8_t style_cnt;
    uint8_t skip_trans :1;
}lv_obj_style_list_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the object related style manager module.
 * Called by LVGL in `lv_init()`
 */
void _lv_obj_style_init(void);

/**
 * Add a new style to the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to add (Only its pointer will be saved)
 */
void lv_obj_add_style(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style);

/**
 * Remove a style from the style list of an object.
 * @param obj pointer to an object
 * @param part the part of the object which style property should be set.
 *              E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 * @param style pointer to a style to remove
 */
void lv_obj_remove_style(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style);

/**
 * Reset a style to the default (empty) state.
 * Release all used memories and cancel pending related transitions.
 * Also notifies the object about the style change.
 * @param obj pointer to an object
 * @param part the part of the object which style list should be reseted.
 *             E.g. `LV_OBJ_PART_MAIN`, `LV_BTN_PART_MAIN`, `LV_SLIDER_PART_KNOB`
 */
void lv_obj_remove_all_styles(struct _lv_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_change(lv_style_t * style);

#if LV_USE_ANIMATION

/**
 * Fade in (from transparent to fully cover) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_in(struct _lv_obj_t * obj, uint32_t time, uint32_t delay);

/**
 * Fade out (from fully cover to transparent) an object and all its children using an `opa_scale` animation.
 * @param obj the object to fade in
 * @param time duration of the animation [ms]
 * @param delay wait before the animation starts [ms]
 */
void lv_obj_fade_out(struct _lv_obj_t * obj, uint32_t time, uint32_t delay);

#endif

lv_style_value_t lv_obj_get_style_prop(const struct _lv_obj_t * obj, uint8_t part, lv_style_prop_t prop);

/**
 * Notify an object and its children about its style is modified
 * @param obj pointer to an object
 * @param part the part of the object which style property should be refreshed.
 * @param prop `LV_STYLE_PROP_ALL` or an `LV_STYLE_...` property. It is used to optimize what needs to be refreshed.
 */
void _lv_obj_refresh_style(struct _lv_obj_t * obj, lv_style_prop_t prop);

lv_style_t * lv_obj_get_local_style(lv_obj_t * obj, uint32_t part, uint32_t state);
/**
 * Remove all transitions from an object
 * @param obj pointer to an object
 */
void _lv_obj_remove_style_trans(struct _lv_obj_t * obj);
void lv_obj_set_style_prop(lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_prop_t prop, lv_style_value_t value);

#if LV_USE_ANIMATION

/**
 * Allocate and initialize a transition for a property of an object if the properties value is different in the new state.
 * It allocates `lv_style_trans_t` in `_lv_obj_style_trans_ll` and set only `start/end_values`. No animation will be created here.
 * @param obj and object to add the transition
 * @param prop the property to apply the transaction
 * @param part the part of the object to apply the transaction
 * @param prev_state the previous state of the objects
 * @param new_state the new state of the object
 * @param time duration of transition in [ms]
 * @param delay delay before starting the transition in [ms]
 * @param path the path of the transition
 * @return pointer to the allocated `the transaction` variable or `NULL` if no transition created
 */
void _lv_obj_create_style_transition(struct _lv_obj_t * obj, lv_style_prop_t prop, uint8_t part, lv_state_t prev_state,
                                       lv_state_t new_state, uint32_t time, uint32_t delay, const lv_anim_path_t * path);

#endif

/**
 * Compare the style properties of an object in 2 different states
 * @param obj pointer to an object
 * @param state1 a state
 * @param state2 an other state
 * @return an element of `_lv_style_state_cmp_t`
 */
_lv_style_state_cmp_t _lv_obj_style_state_compare(struct _lv_obj_t * obj, lv_state_t state1, lv_state_t state2);


/*********************
 * OBJ STYLE GET
 *********************/
static inline lv_coord_t lv_obj_get_style_radius(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RADIUS); return v._int; }

static inline bool lv_obj_get_style_clip_corner(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CLIP_CORNER); return v._int; }

static inline lv_coord_t lv_obj_get_style_transform_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_WIDTH); return v._int; }

static inline lv_coord_t lv_obj_get_style_transform_height(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_HEIGHT); return v._int; }

static inline lv_coord_t lv_obj_get_style_transform_zoom(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ZOOM); return v._int; }

static inline lv_coord_t lv_obj_get_style_transform_angle(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ANGLE); return v._int; }

static inline lv_opa_t lv_obj_get_style_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OPA); return v._int; }

static inline lv_coord_t lv_obj_get_style_pad_top(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_TOP); return v._int; }

static inline lv_coord_t lv_obj_get_style_pad_bottom(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_BOTTOM); return v._int; }

static inline lv_coord_t lv_obj_get_style_pad_left(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_LEFT); return v._int; }

static inline lv_coord_t lv_obj_get_style_pad_right(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RIGHT); return v._int; }

static inline lv_coord_t lv_obj_get_style_margin_top(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_TOP); return v._int; }

static inline lv_coord_t lv_obj_get_style_margin_bottom(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_BOTTOM); return v._int; }

static inline lv_coord_t lv_obj_get_style_margin_left(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_LEFT); return v._int; }

static inline lv_coord_t lv_obj_get_style_margin_right(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MARGIN_RIGHT); return v._int; }

static inline lv_color_t lv_obj_get_style_bg_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_bg_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_OPA); return v._int; }

static inline lv_color_t lv_obj_get_style_bg_grad_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR); return v._color; }

static inline lv_grad_dir_t lv_obj_get_style_bg_grad_dir(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_DIR); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_bg_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_BLEND_MODE); return v._int; }

static inline lv_coord_t lv_obj_get_style_bg_main_stop(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_MAIN_STOP); return v._int; }

static inline lv_coord_t lv_obj_get_style_bg_grad_stop(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_STOP); return v._int; }

static inline lv_color_t lv_obj_get_style_border_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_border_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_OPA); return v._int; }

static inline lv_coord_t lv_obj_get_style_border_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_WIDTH); return v._int; }

static inline lv_border_side_t lv_obj_get_style_border_side(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_SIDE); return v._int; }

static inline bool lv_obj_get_style_border_post(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_POST); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_border_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_BLEND_MODE); return v._int; }

static inline lv_color_t lv_obj_get_style_text_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_text_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OPA); return v._int; }

static inline const lv_font_t * lv_obj_get_style_text_font(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_FONT); return v._ptr; }

static inline lv_coord_t lv_obj_get_style_text_letter_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LETTER_SPACE); return v._int; }

static inline lv_coord_t lv_obj_get_style_text_line_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LINE_SPACE); return v._int; }

static inline lv_text_decor_t lv_obj_get_style_text_decor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_DECOR); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_text_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_BLEND_MODE); return v._int; }

static inline lv_opa_t lv_obj_get_style_img_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_OPA); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_img_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_BLEND_MODE); return v._int; }

static inline lv_color_t lv_obj_get_style_img_recolor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_img_recolor_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR_OPA); return v._int; }

static inline lv_coord_t lv_obj_get_style_outline_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_WIDTH); return v._int; }

static inline lv_color_t lv_obj_get_style_outline_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_outline_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_OPA); return v._int; }

static inline lv_coord_t lv_obj_get_style_outline_pad(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_PAD); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_outline_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_BLEND_MODE); return v._int; }

static inline lv_coord_t lv_obj_get_style_shadow_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_WIDTH); return v._int; }

static inline lv_coord_t lv_obj_get_style_shadow_ofs_x(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_X); return v._int; }

static inline lv_coord_t lv_obj_get_style_shadow_ofs_y(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_Y); return v._int; }

static inline lv_coord_t lv_obj_get_style_shadow_spread(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_SPREAD); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_shadow_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_BLEND_MODE); return v._int; }

static inline lv_color_t lv_obj_get_style_shadow_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_shadow_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OPA); return v._int; }

static inline lv_coord_t lv_obj_get_style_line_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_WIDTH); return v._int; }

static inline lv_blend_mode_t lv_obj_get_style_line_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_BLEND_MODE); return v._int; }

static inline lv_coord_t lv_obj_get_style_line_dash_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_WIDTH); return v._int; }

static inline lv_coord_t lv_obj_get_style_line_dash_gap(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_GAP); return v._int; }

static inline lv_coord_t lv_obj_get_style_line_rounded(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_ROUNDED); return v._int; }

static inline lv_color_t lv_obj_get_style_line_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR); return v._color; }

static inline lv_opa_t lv_obj_get_style_line_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_OPA); return v._int; }

static inline const char * lv_obj_get_style_content_text(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_SRC); return v._ptr; }

static inline lv_align_t lv_obj_get_style_content_align(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_ALIGN); return v._int; }

static inline lv_coord_t lv_obj_get_style_content_ofs_x(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_OFS_X); return v._int; }

static inline lv_coord_t lv_obj_get_style_content_ofs_y(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_OFS_Y); return v._int; }

static inline uint16_t lv_obj_get_style_transition_time(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_TIME); return v._int; }

static inline uint16_t lv_obj_get_style_transition_delay(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_DELAY); return v._int; }

static inline const lv_anim_path_t * lv_obj_get_style_transition_path(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PATH); return v._ptr; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_1(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_1); return v._int; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_2(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_2); return v._int; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_3(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_3); return v._int; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_4(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_4); return v._int; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_5(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_5); return v._int; }

static inline lv_style_prop_t lv_obj_get_style_transition_prop_6(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION_PROP_6); return v._int; }



/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/

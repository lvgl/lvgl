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

/*********************
 *      DEFINES
 *********************/
#define LV_OBJ_STYLE_CACHE_STATE_INVALID    LV_STATE_ANY

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
    uint32_t skip_trans :1;
    uint32_t style_cnt  :5;
    uint32_t cache_state :10;

    uint32_t cache_opa_set :1;
    uint32_t cache_radius_zero:1;
    uint32_t cache_pad_zero:1;
    uint32_t cache_transform_zero:1;
    uint32_t cache_blend_mode_zero:1;
    uint32_t cache_filter_zero:1;
    uint32_t cache_clip_corner_enable:1;

    uint32_t cache_bg_opa_cover:1;

    uint32_t cache_border_width_zero:1;
    uint32_t cache_border_post_enable:1;

    uint32_t cache_text_any_set:1;

    uint32_t cache_img_opa_cover:1;
    uint32_t cache_outline_width_zero:1;
    uint32_t cache_shadow_width_zero:1;
    uint32_t cache_content_text_zero:1;
    uint32_t cache_bg_img_src_zero:1;
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
 * Add a style to an object.
 * @param obj       pointer to an object
 * @param part      a part of the object to which the style should be added E.g. `LV_PART_MAIN` or `LV_PART_KNOB`
 * @param state     a state or combination of states to which the style should be assigned
 * @param style     pointer to a style to add
 * @example         lv_obj_add_style_no_refresh(slider, LV_PART_KNOB, LV_STATE_PRESSED, &style1);
 */
void lv_obj_add_style(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_t * style);

/**
 * Add a style to an object.
 * @param obj       pointer to an object
 * @param part      a part of the object from which the style should be removed E.g. `LV_PART_MAIN` or `LV_PART_KNOB`
 * @param state     a state or combination of states from which the style should be removed
 * @param style     pointer to a style to remove
 * @example lv_obj_remove_style(obj, LV_PART_ANY, LV_STATE_ANY, &style); //Remove a specific style
 * @example lv_obj_remove_style(obj, LV_PART_MAIN, LV_STATE_ANY, &style); //Remove all styles from the main part
 * @example lv_obj_remove_style(obj, LV_PART_ANY, LV_STATE_ANY, NULL); //Remove all styles
 */
void lv_obj_remove_style(struct _lv_obj_t * objj, uint32_t part, uint32_t state, lv_style_t * style);

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
 * @param prop      `LV_STYLE_PROP_ALL` or an `LV_STYLE_...` property.
 *                  It is used to optimize what needs to be refreshed.
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
lv_style_value_t lv_obj_get_style_prop(const struct _lv_obj_t * obj, uint8_t part, lv_style_prop_t prop);

/**
 * Set local style property on an object's part and state.
 * @param obj       pointer to an object
 * @param part      a part to which the property should be added
 * @param state     a state to which the property should be added
 * @param prop      the property
 * @param value     value of the property. The correct element should be set according to the type of the property
 */
void lv_obj_set_local_style_prop(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_prop_t prop, lv_style_value_t value);

/**
 * Remove a local style property from a part of an object with a given state.
 * @param obj       pointer to an object
 * @param part      the part of the object which style property should be removed.
 * @param state     the state from which the property should be removed.
 * @param prop      a style property to remove.
 * @return true     the property was found and removed; false: the property was not found
 */
bool lv_obj_remove_local_style_prop(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_style_prop_t prop);

/**
 * Used internally to create a style tarnsition
 * @param obj
 * @param prop
 * @param part
 * @param prev_state
 * @param new_state
 * @param time
 * @param delay
 * @param path
 */
void _lv_obj_style_create_transition(struct _lv_obj_t * obj, lv_style_prop_t prop, uint8_t part, lv_state_t prev_state,
                                       lv_state_t new_state, uint32_t time, uint32_t delay, const lv_anim_path_t * path);

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

/*********************
 * OBJ STYLE GET
 *********************/
static inline lv_coord_t lv_obj_get_style_radius(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RADIUS); return (lv_coord_t) v.num; }

static inline bool lv_obj_get_style_clip_corner(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CLIP_CORNER); return (bool) v.num; }

static inline lv_coord_t lv_obj_get_style_transform_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_WIDTH); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_transform_height(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_HEIGHT); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_transform_zoom(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ZOOM); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_transform_angle(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ANGLE); return (lv_coord_t) v.num; }

static inline lv_opa_t lv_obj_get_style_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OPA); return (lv_opa_t) v.num; }

static inline lv_color_filter_cb_t lv_obj_get_style_color_filter_cb(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_CB); return (lv_color_filter_cb_t) v.func; }

static inline lv_opa_t lv_obj_get_style_color_filter_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_OPA); return (lv_opa_t) v.num; }

static inline uint32_t lv_obj_get_style_anim_time(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM_TIME); return (uint32_t) v.num; }

static inline const lv_style_transition_dsc_t * lv_obj_get_style_transition(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION); return (const lv_style_transition_dsc_t *) v.ptr; }

static inline lv_coord_t lv_obj_get_style_size(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SIZE); return (lv_coord_t) v.num; }

static inline lv_blend_mode_t lv_obj_get_style_blend_mode(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLEND_MODE); return (lv_blend_mode_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_top(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_TOP); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_bottom(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_BOTTOM); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_left(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_LEFT); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_right(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RIGHT); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_row(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_ROW); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_pad_column(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_COLUMN); return (lv_coord_t) v.num; }

static inline lv_color_t lv_obj_get_style_bg_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_bg_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_bg_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_OPA); return (lv_opa_t) v.num; }

static inline lv_color_t lv_obj_get_style_bg_grad_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_bg_grad_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_grad_dir_t lv_obj_get_style_bg_grad_dir(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_DIR); return (lv_grad_dir_t) v.num; }

static inline lv_coord_t lv_obj_get_style_bg_main_stop(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_MAIN_STOP); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_bg_grad_stop(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_STOP); return (lv_coord_t) v.num; }

static inline const void * lv_obj_get_style_bg_img_src(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_SRC); return (const void *) v.ptr; }

static inline lv_opa_t lv_obj_get_style_bg_img_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_OPA); return (lv_opa_t) v.num; }

static inline lv_color_t lv_obj_get_style_bg_img_recolor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_bg_img_recolor_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_bg_img_recolor_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR_OPA); return (lv_opa_t) v.num; }

static inline bool lv_obj_get_style_bg_img_tiled(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_TILED); return (bool) v.num; }

static inline lv_color_t lv_obj_get_style_border_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_border_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_border_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_OPA); return (lv_opa_t) v.num; }

static inline lv_coord_t lv_obj_get_style_border_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_WIDTH); return (lv_coord_t) v.num; }

static inline lv_border_side_t lv_obj_get_style_border_side(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_SIDE); return (lv_border_side_t) v.num; }

static inline bool lv_obj_get_style_border_post(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_POST); return (bool) v.num; }

static inline lv_color_t lv_obj_get_style_text_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_text_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_text_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OPA); return (lv_opa_t) v.num; }

static inline const lv_font_t * lv_obj_get_style_text_font(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_FONT); return (const lv_font_t *) v.ptr; }

static inline lv_coord_t lv_obj_get_style_text_letter_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LETTER_SPACE); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_text_line_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LINE_SPACE); return (lv_coord_t) v.num; }

static inline lv_text_decor_t lv_obj_get_style_text_decor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_DECOR); return (lv_text_decor_t) v.num; }

static inline lv_text_align_t lv_obj_get_style_text_align(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_ALIGN); return (lv_text_align_t) v.num; }

static inline lv_opa_t lv_obj_get_style_img_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_OPA); return (lv_opa_t) v.num; }

static inline lv_color_t lv_obj_get_style_img_recolor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_img_recolor_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_img_recolor_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR_OPA); return (lv_opa_t) v.num; }

static inline lv_coord_t lv_obj_get_style_outline_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_WIDTH); return (lv_coord_t) v.num; }

static inline lv_color_t lv_obj_get_style_outline_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_outline_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_outline_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_OPA); return (lv_opa_t) v.num; }

static inline lv_coord_t lv_obj_get_style_outline_pad(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_PAD); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_shadow_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_WIDTH); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_shadow_ofs_x(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_X); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_shadow_ofs_y(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_Y); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_shadow_spread(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_SPREAD); return (lv_coord_t) v.num; }

static inline lv_color_t lv_obj_get_style_shadow_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_shadow_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_shadow_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OPA); return (lv_opa_t) v.num; }

static inline lv_coord_t lv_obj_get_style_line_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_WIDTH); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_line_dash_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_WIDTH); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_line_dash_gap(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_GAP); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_line_rounded(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_ROUNDED); return (lv_coord_t) v.num; }

static inline lv_color_t lv_obj_get_style_line_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_line_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_line_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_OPA); return (lv_opa_t) v.num; }

static inline lv_coord_t lv_obj_get_style_arc_width(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_WIDTH); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_arc_rounded(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_ROUNDED); return (lv_coord_t) v.num; }

static inline lv_color_t lv_obj_get_style_arc_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_arc_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_opa_t lv_obj_get_style_arc_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_OPA); return (lv_opa_t) v.num; }

static inline const void * lv_obj_get_style_arc_img_src(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_IMG_SRC); return (const void *) v.ptr; }

static inline const char * lv_obj_get_style_content_text(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_TEXT); return (const char *) v.ptr; }

static inline lv_align_t lv_obj_get_style_content_align(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_ALIGN); return (lv_align_t) v.num; }

static inline lv_coord_t lv_obj_get_style_content_ofs_x(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_OFS_X); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_content_ofs_y(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_OFS_Y); return (lv_coord_t) v.num; }

static inline lv_opa_t lv_obj_get_style_content_opa(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_OPA); return (lv_opa_t) v.num; }

static inline const lv_font_t * lv_obj_get_style_content_font(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_FONT); return (const lv_font_t *) v.ptr; }

static inline lv_color_t lv_obj_get_style_content_color(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_COLOR); return (lv_color_t) v.color; }

static inline lv_color_t lv_obj_get_style_content_color_filtered(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_COLOR_FILTERED); return (lv_color_t) v.color; }

static inline lv_coord_t lv_obj_get_style_content_letter_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_LETTER_SPACE); return (lv_coord_t) v.num; }

static inline lv_coord_t lv_obj_get_style_content_line_space(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_LINE_SPACE); return (lv_coord_t) v.num; }

static inline lv_text_decor_t lv_obj_get_style_content_decor(const struct _lv_obj_t * obj, uint32_t part) {
  lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CONTENT_DECOR); return (lv_text_decor_t) v.num; }


/*********************
 * LOCAL STYLE SET
 *********************/

static inline void lv_obj_set_style_radius(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_RADIUS, v); }

static inline void lv_obj_set_style_clip_corner(struct _lv_obj_t * obj, uint32_t part, uint32_t state, bool value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CLIP_CORNER, v); }

static inline void lv_obj_set_style_transform_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TRANSFORM_WIDTH, v); }

static inline void lv_obj_set_style_transform_height(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TRANSFORM_HEIGHT, v); }

static inline void lv_obj_set_style_transform_zoom(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TRANSFORM_ZOOM, v); }

static inline void lv_obj_set_style_transform_angle(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TRANSFORM_ANGLE, v); }

static inline void lv_obj_set_style_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OPA, v); }

static inline void lv_obj_set_style_color_filter_cb(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_filter_cb_t value) {
  lv_style_value_t v = {.func = (void(*)(void))value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_COLOR_FILTER_CB, v); }

static inline void lv_obj_set_style_color_filter_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_COLOR_FILTER_OPA, v); }

static inline void lv_obj_set_style_anim_time(struct _lv_obj_t * obj, uint32_t part, uint32_t state, uint32_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ANIM_TIME, v); }

static inline void lv_obj_set_style_transition(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const lv_style_transition_dsc_t * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TRANSITION, v); }

static inline void lv_obj_set_style_size(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SIZE, v); }

static inline void lv_obj_set_style_blend_mode(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_blend_mode_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BLEND_MODE, v); }

static inline void lv_obj_set_style_pad_top(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_TOP, v); }

static inline void lv_obj_set_style_pad_bottom(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_BOTTOM, v); }

static inline void lv_obj_set_style_pad_left(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_LEFT, v); }

static inline void lv_obj_set_style_pad_right(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_RIGHT, v); }

static inline void lv_obj_set_style_pad_row(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_ROW, v); }

static inline void lv_obj_set_style_pad_column(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_PAD_COLUMN, v); }

static inline void lv_obj_set_style_bg_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_COLOR, v); }

static inline void lv_obj_set_style_bg_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_bg_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_OPA, v); }

static inline void lv_obj_set_style_bg_grad_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_GRAD_COLOR, v); }

static inline void lv_obj_set_style_bg_grad_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_GRAD_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_bg_grad_dir(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_grad_dir_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_GRAD_DIR, v); }

static inline void lv_obj_set_style_bg_main_stop(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_MAIN_STOP, v); }

static inline void lv_obj_set_style_bg_grad_stop(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_GRAD_STOP, v); }

static inline void lv_obj_set_style_bg_img_src(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const void * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_SRC, v); }

static inline void lv_obj_set_style_bg_img_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_OPA, v); }

static inline void lv_obj_set_style_bg_img_recolor(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_RECOLOR, v); }

static inline void lv_obj_set_style_bg_img_recolor_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_RECOLOR_FILTERED, v); }

static inline void lv_obj_set_style_bg_img_recolor_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_RECOLOR_OPA, v); }

static inline void lv_obj_set_style_bg_img_tiled(struct _lv_obj_t * obj, uint32_t part, uint32_t state, bool value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BG_IMG_TILED, v); }

static inline void lv_obj_set_style_border_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_COLOR, v); }

static inline void lv_obj_set_style_border_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_border_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_OPA, v); }

static inline void lv_obj_set_style_border_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_WIDTH, v); }

static inline void lv_obj_set_style_border_side(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_border_side_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_SIDE, v); }

static inline void lv_obj_set_style_border_post(struct _lv_obj_t * obj, uint32_t part, uint32_t state, bool value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_BORDER_POST, v); }

static inline void lv_obj_set_style_text_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_COLOR, v); }

static inline void lv_obj_set_style_text_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_text_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_OPA, v); }

static inline void lv_obj_set_style_text_font(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const lv_font_t * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_FONT, v); }

static inline void lv_obj_set_style_text_letter_space(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_LETTER_SPACE, v); }

static inline void lv_obj_set_style_text_line_space(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_LINE_SPACE, v); }

static inline void lv_obj_set_style_text_decor(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_text_decor_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_DECOR, v); }

static inline void lv_obj_set_style_text_align(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_text_align_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_TEXT_ALIGN, v); }

static inline void lv_obj_set_style_img_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_IMG_OPA, v); }

static inline void lv_obj_set_style_img_recolor(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_IMG_RECOLOR, v); }

static inline void lv_obj_set_style_img_recolor_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_IMG_RECOLOR_FILTERED, v); }

static inline void lv_obj_set_style_img_recolor_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_IMG_RECOLOR_OPA, v); }

static inline void lv_obj_set_style_outline_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OUTLINE_WIDTH, v); }

static inline void lv_obj_set_style_outline_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OUTLINE_COLOR, v); }

static inline void lv_obj_set_style_outline_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OUTLINE_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_outline_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OUTLINE_OPA, v); }

static inline void lv_obj_set_style_outline_pad(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_OUTLINE_PAD, v); }

static inline void lv_obj_set_style_shadow_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_WIDTH, v); }

static inline void lv_obj_set_style_shadow_ofs_x(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_OFS_X, v); }

static inline void lv_obj_set_style_shadow_ofs_y(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_OFS_Y, v); }

static inline void lv_obj_set_style_shadow_spread(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_SPREAD, v); }

static inline void lv_obj_set_style_shadow_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_COLOR, v); }

static inline void lv_obj_set_style_shadow_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_shadow_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_SHADOW_OPA, v); }

static inline void lv_obj_set_style_line_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_WIDTH, v); }

static inline void lv_obj_set_style_line_dash_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_DASH_WIDTH, v); }

static inline void lv_obj_set_style_line_dash_gap(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_DASH_GAP, v); }

static inline void lv_obj_set_style_line_rounded(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_ROUNDED, v); }

static inline void lv_obj_set_style_line_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_COLOR, v); }

static inline void lv_obj_set_style_line_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_line_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_LINE_OPA, v); }

static inline void lv_obj_set_style_arc_width(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_WIDTH, v); }

static inline void lv_obj_set_style_arc_rounded(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_ROUNDED, v); }

static inline void lv_obj_set_style_arc_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_COLOR, v); }

static inline void lv_obj_set_style_arc_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_arc_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_OPA, v); }

static inline void lv_obj_set_style_arc_img_src(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const void * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_ARC_IMG_SRC, v); }

static inline void lv_obj_set_style_content_text(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const char * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_TEXT, v); }

static inline void lv_obj_set_style_content_align(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_align_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_ALIGN, v); }

static inline void lv_obj_set_style_content_ofs_x(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_OFS_X, v); }

static inline void lv_obj_set_style_content_ofs_y(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_OFS_Y, v); }

static inline void lv_obj_set_style_content_opa(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_opa_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_OPA, v); }

static inline void lv_obj_set_style_content_font(struct _lv_obj_t * obj, uint32_t part, uint32_t state, const lv_font_t * value) {
  lv_style_value_t v = {.ptr = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_FONT, v); }

static inline void lv_obj_set_style_content_color(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_COLOR, v); }

static inline void lv_obj_set_style_content_color_filtered(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_color_t value) {
  lv_style_value_t v = {.color = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_COLOR_FILTERED, v); }

static inline void lv_obj_set_style_content_letter_space(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_LETTER_SPACE, v); }

static inline void lv_obj_set_style_content_line_space(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_coord_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_LINE_SPACE, v); }

static inline void lv_obj_set_style_content_decor(struct _lv_obj_t * obj, uint32_t part, uint32_t state, lv_text_decor_t value) {
  lv_style_value_t v = {.num = value}; lv_obj_set_local_style_prop(obj, part, state, LV_STYLE_CONTENT_DECOR, v); }


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/

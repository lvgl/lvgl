#include "lv_obj.h"
void lv_obj_set_style_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_WIDTH, v, selector);
}

void lv_obj_set_style_min_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_WIDTH, v, selector);
}

void lv_obj_set_style_max_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_WIDTH, v, selector);
}

void lv_obj_set_style_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_HEIGHT, v, selector);
}

void lv_obj_set_style_min_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_HEIGHT, v, selector);
}

void lv_obj_set_style_max_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_HEIGHT, v, selector);
}

void lv_obj_set_style_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_X, v, selector);
}

void lv_obj_set_style_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_Y, v, selector);
}

void lv_obj_set_style_align(struct _lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ALIGN, v, selector);
}

void lv_obj_set_style_transform_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_WIDTH, v, selector);
}

void lv_obj_set_style_transform_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_HEIGHT, v, selector);
}

void lv_obj_set_style_translate_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_X, v, selector);
}

void lv_obj_set_style_translate_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_Y, v, selector);
}

void lv_obj_set_style_transform_zoom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ZOOM, v, selector);
}

void lv_obj_set_style_transform_angle(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ANGLE, v, selector);
}

void lv_obj_set_style_pad_top(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, v, selector);
}

void lv_obj_set_style_pad_bottom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, v, selector);
}

void lv_obj_set_style_pad_left(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, v, selector);
}

void lv_obj_set_style_pad_right(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, v, selector);
}

void lv_obj_set_style_pad_row(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_ROW, v, selector);
}

void lv_obj_set_style_pad_column(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_COLUMN, v, selector);
}

void lv_obj_set_style_radius(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_RADIUS, v, selector);
}

void lv_obj_set_style_clip_corner(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_CLIP_CORNER, v, selector);
}

void lv_obj_set_style_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OPA, v, selector);
}

void lv_obj_set_style_color_filter_dsc(struct _lv_obj_t * obj, const lv_color_filter_dsc_t * value,
                                       lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_DSC, v, selector);
}

void lv_obj_set_style_color_filter_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_OPA, v, selector);
}

void lv_obj_set_style_anim_time(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_TIME, v, selector);
}

void lv_obj_set_style_anim_speed(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_SPEED, v, selector);
}

void lv_obj_set_style_transition(struct _lv_obj_t * obj, const lv_style_transition_dsc_t * value,
                                 lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSITION, v, selector);
}

void lv_obj_set_style_blend_mode(struct _lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BLEND_MODE, v, selector);
}

void lv_obj_set_style_layout(struct _lv_obj_t * obj, uint16_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LAYOUT, v, selector);
}

void lv_obj_set_style_base_dir(struct _lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BASE_DIR, v, selector);
}

void lv_obj_set_style_bg_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_COLOR, v, selector);
}

void lv_obj_set_style_bg_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_bg_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, v, selector);
}

void lv_obj_set_style_bg_grad_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_COLOR, v, selector);
}

void lv_obj_set_style_bg_grad_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_bg_grad_dir(struct _lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_DIR, v, selector);
}

void lv_obj_set_style_bg_main_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_MAIN_STOP, v, selector);
}

void lv_obj_set_style_bg_grad_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_STOP, v, selector);
}

void lv_obj_set_style_bg_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_SRC, v, selector);
}

void lv_obj_set_style_bg_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_OPA, v, selector);
}

void lv_obj_set_style_bg_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_bg_img_recolor_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR_FILTERED, v, selector);
}

void lv_obj_set_style_bg_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_bg_img_tiled(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_TILED, v, selector);
}

void lv_obj_set_style_border_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_COLOR, v, selector);
}

void lv_obj_set_style_border_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_border_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_OPA, v, selector);
}

void lv_obj_set_style_border_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, v, selector);
}

void lv_obj_set_style_border_side(struct _lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_SIDE, v, selector);
}

void lv_obj_set_style_border_post(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_POST, v, selector);
}

void lv_obj_set_style_text_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_COLOR, v, selector);
}

void lv_obj_set_style_text_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_text_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_OPA, v, selector);
}

void lv_obj_set_style_text_font(struct _lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_FONT, v, selector);
}

void lv_obj_set_style_text_letter_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LETTER_SPACE, v, selector);
}

void lv_obj_set_style_text_line_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LINE_SPACE, v, selector);
}

void lv_obj_set_style_text_decor(struct _lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_DECOR, v, selector);
}

void lv_obj_set_style_text_align(struct _lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_ALIGN, v, selector);
}

void lv_obj_set_style_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_OPA, v, selector);
}

void lv_obj_set_style_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_img_recolor_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR_FILTERED, v, selector);
}

void lv_obj_set_style_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_outline_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_WIDTH, v, selector);
}

void lv_obj_set_style_outline_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_COLOR, v, selector);
}

void lv_obj_set_style_outline_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_outline_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_OPA, v, selector);
}

void lv_obj_set_style_outline_pad(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_PAD, v, selector);
}

void lv_obj_set_style_shadow_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_WIDTH, v, selector);
}

void lv_obj_set_style_shadow_ofs_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_X, v, selector);
}

void lv_obj_set_style_shadow_ofs_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_Y, v, selector);
}

void lv_obj_set_style_shadow_spread(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_SPREAD, v, selector);
}

void lv_obj_set_style_shadow_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_COLOR, v, selector);
}

void lv_obj_set_style_shadow_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_shadow_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OPA, v, selector);
}

void lv_obj_set_style_line_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_WIDTH, v, selector);
}

void lv_obj_set_style_line_dash_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_WIDTH, v, selector);
}

void lv_obj_set_style_line_dash_gap(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_GAP, v, selector);
}

void lv_obj_set_style_line_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_ROUNDED, v, selector);
}

void lv_obj_set_style_line_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_COLOR, v, selector);
}

void lv_obj_set_style_line_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_line_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_OPA, v, selector);
}

void lv_obj_set_style_arc_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_WIDTH, v, selector);
}

void lv_obj_set_style_arc_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_ROUNDED, v, selector);
}

void lv_obj_set_style_arc_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_COLOR, v, selector);
}

void lv_obj_set_style_arc_color_filtered(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_COLOR_FILTERED, v, selector);
}

void lv_obj_set_style_arc_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_OPA, v, selector);
}

void lv_obj_set_style_arc_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_IMG_SRC, v, selector);
}


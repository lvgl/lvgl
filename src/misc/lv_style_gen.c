#include "lv_style.h"

void lv_style_set_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_WIDTH, v);
}

void lv_style_set_min_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_WIDTH, v);
}

void lv_style_set_max_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_WIDTH, v);
}

void lv_style_set_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_HEIGHT, v);
}

void lv_style_set_min_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_HEIGHT, v);
}

void lv_style_set_max_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_HEIGHT, v);
}

void lv_style_set_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_X, v);
}

void lv_style_set_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_Y, v);
}

void lv_style_set_align(lv_style_t * style, lv_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ALIGN, v);
}

void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_WIDTH, v);
}

void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_HEIGHT, v);
}

void lv_style_set_translate_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_X, v);
}

void lv_style_set_translate_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_Y, v);
}

void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ZOOM, v);
}

void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ANGLE, v);
}

void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_TOP, v);
}

void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_BOTTOM, v);
}

void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_LEFT, v);
}

void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_RIGHT, v);
}

void lv_style_set_pad_row(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_ROW, v);
}

void lv_style_set_pad_column(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_COLUMN, v);
}

void lv_style_set_bg_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_COLOR, v);
}

void lv_style_set_bg_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_COLOR_FILTERED, v);
}

void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_OPA, v);
}

void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR, v);
}

void lv_style_set_bg_grad_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR_FILTERED, v);
}

void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_DIR, v);
}

void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_MAIN_STOP, v);
}

void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_STOP, v);
}

void lv_style_set_bg_grad(lv_style_t * style, const lv_grad_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD, v);
}

void lv_style_set_bg_dither_mode(lv_style_t * style, lv_dither_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_DITHER_MODE, v);
}

void lv_style_set_bg_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_SRC, v);
}

void lv_style_set_bg_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_OPA, v);
}

void lv_style_set_bg_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR, v);
}

void lv_style_set_bg_img_recolor_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR_FILTERED, v);
}

void lv_style_set_bg_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR_OPA, v);
}

void lv_style_set_bg_img_tiled(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_TILED, v);
}

void lv_style_set_border_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_COLOR, v);
}

void lv_style_set_border_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_COLOR_FILTERED, v);
}

void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_OPA, v);
}

void lv_style_set_border_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_WIDTH, v);
}

void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_SIDE, v);
}

void lv_style_set_border_post(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_POST, v);
}

void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_WIDTH, v);
}

void lv_style_set_outline_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR, v);
}

void lv_style_set_outline_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR_FILTERED, v);
}

void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_OPA, v);
}

void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_PAD, v);
}

void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_WIDTH, v);
}

void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_X, v);
}

void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_Y, v);
}

void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_SPREAD, v);
}

void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR, v);
}

void lv_style_set_shadow_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR_FILTERED, v);
}

void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OPA, v);
}

void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_OPA, v);
}

void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR, v);
}

void lv_style_set_img_recolor_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_FILTERED, v);
}

void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_OPA, v);
}

void lv_style_set_line_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_WIDTH, v);
}

void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_WIDTH, v);
}

void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_GAP, v);
}

void lv_style_set_line_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_ROUNDED, v);
}

void lv_style_set_line_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_COLOR, v);
}

void lv_style_set_line_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_COLOR_FILTERED, v);
}

void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_OPA, v);
}

void lv_style_set_arc_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_WIDTH, v);
}

void lv_style_set_arc_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_ROUNDED, v);
}

void lv_style_set_arc_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_COLOR, v);
}

void lv_style_set_arc_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_COLOR_FILTERED, v);
}

void lv_style_set_arc_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_OPA, v);
}

void lv_style_set_arc_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_IMG_SRC, v);
}

void lv_style_set_text_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_COLOR, v);
}

void lv_style_set_text_color_filtered(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_COLOR_FILTERED, v);
}

void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_OPA, v);
}

void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_FONT, v);
}

void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LETTER_SPACE, v);
}

void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LINE_SPACE, v);
}

void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_DECOR, v);
}

void lv_style_set_text_align(lv_style_t * style, lv_text_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_ALIGN, v);
}

void lv_style_set_radius(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_RADIUS, v);
}

void lv_style_set_clip_corner(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_CLIP_CORNER, v);
}

void lv_style_set_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OPA, v);
}

void lv_style_set_color_filter_dsc(lv_style_t * style, const lv_color_filter_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_DSC, v);
}

void lv_style_set_color_filter_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_OPA, v);
}

void lv_style_set_anim(lv_style_t * style, const lv_anim_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM, v);
}

void lv_style_set_anim_time(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_TIME, v);
}

void lv_style_set_anim_speed(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_SPEED, v);
}

void lv_style_set_transition(lv_style_t * style, const lv_style_transition_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSITION, v);
}

void lv_style_set_blend_mode(lv_style_t * style, lv_blend_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BLEND_MODE, v);
}

void lv_style_set_layout(lv_style_t * style, uint16_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LAYOUT, v);
}

void lv_style_set_base_dir(lv_style_t * style, lv_base_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BASE_DIR, v);
}

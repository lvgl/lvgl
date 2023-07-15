#include "lv_obj.h"

void lv_obj_set_style_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_WIDTH, v, selector);
}

void lv_obj_set_style_min_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_WIDTH, v, selector);
}

void lv_obj_set_style_max_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_WIDTH, v, selector);
}

void lv_obj_set_style_height(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_HEIGHT, v, selector);
}

void lv_obj_set_style_min_height(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_HEIGHT, v, selector);
}

void lv_obj_set_style_max_height(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_HEIGHT, v, selector);
}

void lv_obj_set_style_x(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_X, v, selector);
}

void lv_obj_set_style_y(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_Y, v, selector);
}

void lv_obj_set_style_align(lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ALIGN, v, selector);
}

void lv_obj_set_style_transform_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_WIDTH, v, selector);
}

void lv_obj_set_style_transform_height(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_HEIGHT, v, selector);
}

void lv_obj_set_style_translate_x(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_X, v, selector);
}

void lv_obj_set_style_translate_y(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_Y, v, selector);
}

void lv_obj_set_style_transform_zoom(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ZOOM, v, selector);
}

void lv_obj_set_style_transform_angle(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ANGLE, v, selector);
}

void lv_obj_set_style_transform_pivot_x(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_PIVOT_X, v, selector);
}

void lv_obj_set_style_transform_pivot_y(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_PIVOT_Y, v, selector);
}

void lv_obj_set_style_pad_top(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, v, selector);
}

void lv_obj_set_style_pad_bottom(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, v, selector);
}

void lv_obj_set_style_pad_left(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, v, selector);
}

void lv_obj_set_style_pad_right(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, v, selector);
}

void lv_obj_set_style_pad_row(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_ROW, v, selector);
}

void lv_obj_set_style_pad_column(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_COLUMN, v, selector);
}

void lv_obj_set_style_margin_top(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MARGIN_TOP, v, selector);
}

void lv_obj_set_style_margin_bottom(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MARGIN_BOTTOM, v, selector);
}

void lv_obj_set_style_margin_left(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MARGIN_LEFT, v, selector);
}

void lv_obj_set_style_margin_right(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MARGIN_RIGHT, v, selector);
}

void lv_obj_set_style_bg_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_COLOR, v, selector);
}

void lv_obj_set_style_bg_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, v, selector);
}

void lv_obj_set_style_bg_grad_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_COLOR, v, selector);
}

void lv_obj_set_style_bg_grad_dir(lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_DIR, v, selector);
}

void lv_obj_set_style_bg_main_stop(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_MAIN_STOP, v, selector);
}

void lv_obj_set_style_bg_grad_stop(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_STOP, v, selector);
}

void lv_obj_set_style_bg_grad(lv_obj_t * obj, const lv_grad_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD, v, selector);
}

void lv_obj_set_style_bg_dither_mode(lv_obj_t * obj, lv_dither_mode_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_DITHER_MODE, v, selector);
}

void lv_obj_set_style_bg_img_src(lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_SRC, v, selector);
}

void lv_obj_set_style_bg_img_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_OPA, v, selector);
}

void lv_obj_set_style_bg_img_recolor(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_bg_img_recolor_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_bg_img_tiled(lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_TILED, v, selector);
}

void lv_obj_set_style_border_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_COLOR, v, selector);
}

void lv_obj_set_style_border_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_OPA, v, selector);
}

void lv_obj_set_style_border_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, v, selector);
}

void lv_obj_set_style_border_side(lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_SIDE, v, selector);
}

void lv_obj_set_style_border_post(lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_POST, v, selector);
}

void lv_obj_set_style_outline_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_WIDTH, v, selector);
}

void lv_obj_set_style_outline_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_COLOR, v, selector);
}

void lv_obj_set_style_outline_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_OPA, v, selector);
}

void lv_obj_set_style_outline_pad(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_PAD, v, selector);
}

void lv_obj_set_style_shadow_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_WIDTH, v, selector);
}

void lv_obj_set_style_shadow_ofs_x(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_X, v, selector);
}

void lv_obj_set_style_shadow_ofs_y(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_Y, v, selector);
}

void lv_obj_set_style_shadow_spread(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_SPREAD, v, selector);
}

void lv_obj_set_style_shadow_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_COLOR, v, selector);
}

void lv_obj_set_style_shadow_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OPA, v, selector);
}

void lv_obj_set_style_img_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_OPA, v, selector);
}

void lv_obj_set_style_img_recolor(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_img_recolor_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_line_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_WIDTH, v, selector);
}

void lv_obj_set_style_line_dash_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_WIDTH, v, selector);
}

void lv_obj_set_style_line_dash_gap(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_GAP, v, selector);
}

void lv_obj_set_style_line_rounded(lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_ROUNDED, v, selector);
}

void lv_obj_set_style_line_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_COLOR, v, selector);
}

void lv_obj_set_style_line_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_OPA, v, selector);
}

void lv_obj_set_style_arc_width(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_WIDTH, v, selector);
}

void lv_obj_set_style_arc_rounded(lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_ROUNDED, v, selector);
}

void lv_obj_set_style_arc_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_COLOR, v, selector);
}

void lv_obj_set_style_arc_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_OPA, v, selector);
}

void lv_obj_set_style_arc_img_src(lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_IMG_SRC, v, selector);
}

void lv_obj_set_style_text_color(lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_COLOR, v, selector);
}

void lv_obj_set_style_text_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_OPA, v, selector);
}

void lv_obj_set_style_text_font(lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_FONT, v, selector);
}

void lv_obj_set_style_text_letter_space(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LETTER_SPACE, v, selector);
}

void lv_obj_set_style_text_line_space(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LINE_SPACE, v, selector);
}

void lv_obj_set_style_text_decor(lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_DECOR, v, selector);
}

void lv_obj_set_style_text_align(lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_ALIGN, v, selector);
}

void lv_obj_set_style_radius(lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_RADIUS, v, selector);
}

void lv_obj_set_style_clip_corner(lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_CLIP_CORNER, v, selector);
}

void lv_obj_set_style_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OPA, v, selector);
}

void lv_obj_set_style_color_filter_dsc(lv_obj_t * obj, const lv_color_filter_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_DSC, v, selector);
}

void lv_obj_set_style_color_filter_opa(lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_OPA, v, selector);
}

void lv_obj_set_style_anim(lv_obj_t * obj, const lv_anim_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM, v, selector);
}

void lv_obj_set_style_anim_time(lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_TIME, v, selector);
}

void lv_obj_set_style_anim_speed(lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_SPEED, v, selector);
}

void lv_obj_set_style_transition(lv_obj_t * obj, const lv_style_transition_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSITION, v, selector);
}

void lv_obj_set_style_blend_mode(lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BLEND_MODE, v, selector);
}

void lv_obj_set_style_layout(lv_obj_t * obj, uint16_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LAYOUT, v, selector);
}

void lv_obj_set_style_base_dir(lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BASE_DIR, v, selector);
}

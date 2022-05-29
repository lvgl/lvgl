#include "lv_style.h"

void lv_style_set_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_WIDTH, v);
}

void lv_style_set_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_min_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_WIDTH, v);
}

void lv_style_set_min_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MIN_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_min_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MIN_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_max_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_WIDTH, v);
}

void lv_style_set_max_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MAX_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_max_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MAX_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_HEIGHT, v);
}

void lv_style_set_height_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_HEIGHT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_height_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_HEIGHT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_min_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_HEIGHT, v);
}

void lv_style_set_min_height_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MIN_HEIGHT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_min_height_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MIN_HEIGHT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_max_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_HEIGHT, v);
}

void lv_style_set_max_height_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MAX_HEIGHT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_max_height_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_MAX_HEIGHT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_X, v);
}

void lv_style_set_x_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_X, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_x_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_X, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_Y, v);
}

void lv_style_set_y_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_Y, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_y_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_Y, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_align(lv_style_t * style, lv_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ALIGN, v);
}

void lv_style_set_align_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ALIGN, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_align_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ALIGN, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_WIDTH, v);
}

void lv_style_set_transform_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_HEIGHT, v);
}

void lv_style_set_transform_height_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_height_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_translate_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_X, v);
}

void lv_style_set_translate_x_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSLATE_X, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_translate_x_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSLATE_X, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_translate_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_Y, v);
}

void lv_style_set_translate_y_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSLATE_Y, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_translate_y_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSLATE_Y, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ZOOM, v);
}

void lv_style_set_transform_zoom_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_zoom_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ANGLE, v);
}

void lv_style_set_transform_angle_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_ANGLE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_angle_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_ANGLE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_pivot_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_PIVOT_X, v);
}

void lv_style_set_transform_pivot_x_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_PIVOT_X, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_pivot_x_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_PIVOT_X, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transform_pivot_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_PIVOT_Y, v);
}

void lv_style_set_transform_pivot_y_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_PIVOT_Y, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transform_pivot_y_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSFORM_PIVOT_Y, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_TOP, v);
}

void lv_style_set_pad_top_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_TOP, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_top_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_TOP, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_BOTTOM, v);
}

void lv_style_set_pad_bottom_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_BOTTOM, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_bottom_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_BOTTOM, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_LEFT, v);
}

void lv_style_set_pad_left_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_LEFT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_left_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_LEFT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_RIGHT, v);
}

void lv_style_set_pad_right_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_RIGHT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_right_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_RIGHT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_row(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_ROW, v);
}

void lv_style_set_pad_row_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_ROW, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_row_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_ROW, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_pad_column(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_COLUMN, v);
}

void lv_style_set_pad_column_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_COLUMN, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_pad_column_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_PAD_COLUMN, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_COLOR, v);
}

void lv_style_set_bg_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_OPA, v);
}

void lv_style_set_bg_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR, v);
}

void lv_style_set_bg_grad_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_grad_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_DIR, v);
}

void lv_style_set_bg_grad_dir_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_DIR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_grad_dir_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_DIR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_MAIN_STOP, v);
}

void lv_style_set_bg_main_stop_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_MAIN_STOP, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_main_stop_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_MAIN_STOP, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_STOP, v);
}

void lv_style_set_bg_grad_stop_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_STOP, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_grad_stop_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD_STOP, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_grad(lv_style_t * style, const lv_grad_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD, v);
}

void lv_style_set_bg_grad_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_grad_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_GRAD, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_dither_mode(lv_style_t * style, lv_dither_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_DITHER_MODE, v);
}

void lv_style_set_bg_dither_mode_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_DITHER_MODE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_dither_mode_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_DITHER_MODE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_SRC, v);
}

void lv_style_set_bg_img_src_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_SRC, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_img_src_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_SRC, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_OPA, v);
}

void lv_style_set_bg_img_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_img_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR, v);
}

void lv_style_set_bg_img_recolor_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_RECOLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_img_recolor_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_RECOLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR_OPA, v);
}

void lv_style_set_bg_img_recolor_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_img_recolor_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_bg_img_tiled(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_TILED, v);
}

void lv_style_set_bg_img_tiled_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_TILED, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_bg_img_tiled_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BG_IMG_TILED, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_border_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_COLOR, v);
}

void lv_style_set_border_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_border_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_OPA, v);
}

void lv_style_set_border_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_border_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_border_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_WIDTH, v);
}

void lv_style_set_border_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_border_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_SIDE, v);
}

void lv_style_set_border_side_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_SIDE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_border_side_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_SIDE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_border_post(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_POST, v);
}

void lv_style_set_border_post_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_POST, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_border_post_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BORDER_POST, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_WIDTH, v);
}

void lv_style_set_outline_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_outline_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_outline_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR, v);
}

void lv_style_set_outline_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_outline_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_OPA, v);
}

void lv_style_set_outline_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_outline_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_PAD, v);
}

void lv_style_set_outline_pad_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_PAD, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_outline_pad_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OUTLINE_PAD, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_WIDTH, v);
}

void lv_style_set_shadow_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_X, v);
}

void lv_style_set_shadow_ofs_x_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OFS_X, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_ofs_x_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OFS_X, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_Y, v);
}

void lv_style_set_shadow_ofs_y_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OFS_Y, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_ofs_y_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OFS_Y, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_SPREAD, v);
}

void lv_style_set_shadow_spread_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_SPREAD, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_spread_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_SPREAD, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR, v);
}

void lv_style_set_shadow_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OPA, v);
}

void lv_style_set_shadow_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_shadow_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_SHADOW_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_OPA, v);
}

void lv_style_set_img_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_img_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR, v);
}

void lv_style_set_img_recolor_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_RECOLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_img_recolor_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_RECOLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_OPA, v);
}

void lv_style_set_img_recolor_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_img_recolor_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_WIDTH, v);
}

void lv_style_set_line_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_WIDTH, v);
}

void lv_style_set_line_dash_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_DASH_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_dash_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_DASH_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_GAP, v);
}

void lv_style_set_line_dash_gap_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_DASH_GAP, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_dash_gap_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_DASH_GAP, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_ROUNDED, v);
}

void lv_style_set_line_rounded_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_ROUNDED, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_rounded_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_ROUNDED, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_COLOR, v);
}

void lv_style_set_line_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_OPA, v);
}

void lv_style_set_line_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_line_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LINE_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_arc_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_WIDTH, v);
}

void lv_style_set_arc_width_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_WIDTH, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_arc_width_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_WIDTH, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_arc_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_ROUNDED, v);
}

void lv_style_set_arc_rounded_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_ROUNDED, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_arc_rounded_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_ROUNDED, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_arc_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_COLOR, v);
}

void lv_style_set_arc_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_arc_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_arc_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_OPA, v);
}

void lv_style_set_arc_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_arc_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_arc_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_IMG_SRC, v);
}

void lv_style_set_arc_img_src_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_IMG_SRC, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_arc_img_src_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ARC_IMG_SRC, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_COLOR, v);
}

void lv_style_set_text_color_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_color_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_OPA, v);
}

void lv_style_set_text_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_FONT, v);
}

void lv_style_set_text_font_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_FONT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_font_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_FONT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LETTER_SPACE, v);
}

void lv_style_set_text_letter_space_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_LETTER_SPACE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_letter_space_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_LETTER_SPACE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LINE_SPACE, v);
}

void lv_style_set_text_line_space_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_LINE_SPACE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_line_space_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_LINE_SPACE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_DECOR, v);
}

void lv_style_set_text_decor_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_DECOR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_decor_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_DECOR, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_text_align(lv_style_t * style, lv_text_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_ALIGN, v);
}

void lv_style_set_text_align_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_ALIGN, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_text_align_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TEXT_ALIGN, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_radius(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_RADIUS, v);
}

void lv_style_set_radius_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_RADIUS, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_radius_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_RADIUS, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_clip_corner(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_CLIP_CORNER, v);
}

void lv_style_set_clip_corner_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_CLIP_CORNER, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_clip_corner_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_CLIP_CORNER, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OPA, v);
}

void lv_style_set_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_color_filter_dsc(lv_style_t * style, const lv_color_filter_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_DSC, v);
}

void lv_style_set_color_filter_dsc_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_COLOR_FILTER_DSC, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_color_filter_dsc_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_COLOR_FILTER_DSC, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_color_filter_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_OPA, v);
}

void lv_style_set_color_filter_opa_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_color_filter_opa_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_anim(lv_style_t * style, const lv_anim_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM, v);
}

void lv_style_set_anim_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_anim_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_anim_time(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_TIME, v);
}

void lv_style_set_anim_time_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM_TIME, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_anim_time_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM_TIME, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_anim_speed(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_SPEED, v);
}

void lv_style_set_anim_speed_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM_SPEED, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_anim_speed_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_ANIM_SPEED, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_transition(lv_style_t * style, const lv_style_transition_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSITION, v);
}

void lv_style_set_transition_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSITION, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_transition_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_TRANSITION, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_blend_mode(lv_style_t * style, lv_blend_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BLEND_MODE, v);
}

void lv_style_set_blend_mode_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BLEND_MODE, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_blend_mode_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BLEND_MODE, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_layout(lv_style_t * style, uint16_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LAYOUT, v);
}

void lv_style_set_layout_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LAYOUT, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_layout_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_LAYOUT, LV_STYLE_PROP_META_INITIAL);
}

void lv_style_set_base_dir(lv_style_t * style, lv_base_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BASE_DIR, v);
}

void lv_style_set_base_dir_inherit(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BASE_DIR, LV_STYLE_PROP_META_INHERIT);
}

void lv_style_set_base_dir_initial(lv_style_t * style)
{
    lv_style_set_prop_meta(style, LV_STYLE_BASE_DIR, LV_STYLE_PROP_META_INITIAL);
}

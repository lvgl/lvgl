#include "lv_obj.h"

void lv_obj_set_style_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_WIDTH, v, selector);
}

void lv_obj_set_style_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_min_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_WIDTH, v, selector);
}

void lv_obj_set_style_min_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MIN_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_min_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MIN_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_max_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_WIDTH, v, selector);
}

void lv_obj_set_style_max_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MAX_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_max_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MAX_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_HEIGHT, v, selector);
}

void lv_obj_set_style_height_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_HEIGHT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_height_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_HEIGHT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_min_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MIN_HEIGHT, v, selector);
}

void lv_obj_set_style_min_height_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MIN_HEIGHT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_min_height_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MIN_HEIGHT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_max_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_MAX_HEIGHT, v, selector);
}

void lv_obj_set_style_max_height_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MAX_HEIGHT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_max_height_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_MAX_HEIGHT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_X, v, selector);
}

void lv_obj_set_style_x_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_X, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_x_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_X, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_Y, v, selector);
}

void lv_obj_set_style_y_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_Y, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_y_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_Y, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_align(struct _lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ALIGN, v, selector);
}

void lv_obj_set_style_align_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ALIGN, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_align_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ALIGN, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_WIDTH, v, selector);
}

void lv_obj_set_style_transform_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_HEIGHT, v, selector);
}

void lv_obj_set_style_transform_height_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_height_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_translate_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_X, v, selector);
}

void lv_obj_set_style_translate_x_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSLATE_X, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_translate_x_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSLATE_X, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_translate_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSLATE_Y, v, selector);
}

void lv_obj_set_style_translate_y_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSLATE_Y, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_translate_y_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSLATE_Y, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_zoom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ZOOM, v, selector);
}

void lv_obj_set_style_transform_zoom_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_zoom_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_ZOOM, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_angle(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_ANGLE, v, selector);
}

void lv_obj_set_style_transform_angle_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_ANGLE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_angle_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_ANGLE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_pivot_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_PIVOT_X, v, selector);
}

void lv_obj_set_style_transform_pivot_x_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_PIVOT_X, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_pivot_x_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_PIVOT_X, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transform_pivot_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSFORM_PIVOT_Y, v, selector);
}

void lv_obj_set_style_transform_pivot_y_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_PIVOT_Y, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transform_pivot_y_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSFORM_PIVOT_Y, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_top(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_TOP, v, selector);
}

void lv_obj_set_style_pad_top_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_TOP, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_top_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_TOP, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_bottom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_BOTTOM, v, selector);
}

void lv_obj_set_style_pad_bottom_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_BOTTOM, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_bottom_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_BOTTOM, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_left(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_LEFT, v, selector);
}

void lv_obj_set_style_pad_left_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_LEFT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_left_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_LEFT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_right(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_RIGHT, v, selector);
}

void lv_obj_set_style_pad_right_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_RIGHT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_right_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_RIGHT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_row(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_ROW, v, selector);
}

void lv_obj_set_style_pad_row_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_ROW, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_row_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_ROW, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_pad_column(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_PAD_COLUMN, v, selector);
}

void lv_obj_set_style_pad_column_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_COLUMN, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_pad_column_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_PAD_COLUMN, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_COLOR, v, selector);
}

void lv_obj_set_style_bg_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_OPA, v, selector);
}

void lv_obj_set_style_bg_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_grad_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_COLOR, v, selector);
}

void lv_obj_set_style_bg_grad_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_grad_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_grad_dir(struct _lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_DIR, v, selector);
}

void lv_obj_set_style_bg_grad_dir_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_DIR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_grad_dir_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_DIR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_main_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_MAIN_STOP, v, selector);
}

void lv_obj_set_style_bg_main_stop_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_MAIN_STOP, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_main_stop_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_MAIN_STOP, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_grad_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD_STOP, v, selector);
}

void lv_obj_set_style_bg_grad_stop_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_STOP, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_grad_stop_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD_STOP, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_grad(struct _lv_obj_t * obj, const lv_grad_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_GRAD, v, selector);
}

void lv_obj_set_style_bg_grad_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_grad_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_GRAD, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_dither_mode(struct _lv_obj_t * obj, lv_dither_mode_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_DITHER_MODE, v, selector);
}

void lv_obj_set_style_bg_dither_mode_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_DITHER_MODE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_dither_mode_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_DITHER_MODE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_SRC, v, selector);
}

void lv_obj_set_style_bg_img_src_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_SRC, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_img_src_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_SRC, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_OPA, v, selector);
}

void lv_obj_set_style_bg_img_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_img_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_bg_img_recolor_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_RECOLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_img_recolor_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_RECOLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_bg_img_recolor_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_img_recolor_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_bg_img_tiled(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BG_IMG_TILED, v, selector);
}

void lv_obj_set_style_bg_img_tiled_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_TILED, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_bg_img_tiled_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BG_IMG_TILED, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_border_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_COLOR, v, selector);
}

void lv_obj_set_style_border_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_border_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_border_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_OPA, v, selector);
}

void lv_obj_set_style_border_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_border_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_border_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_WIDTH, v, selector);
}

void lv_obj_set_style_border_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_border_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_border_side(struct _lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_SIDE, v, selector);
}

void lv_obj_set_style_border_side_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_SIDE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_border_side_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_SIDE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_border_post(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BORDER_POST, v, selector);
}

void lv_obj_set_style_border_post_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_POST, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_border_post_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BORDER_POST, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_outline_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_WIDTH, v, selector);
}

void lv_obj_set_style_outline_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_outline_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_outline_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_COLOR, v, selector);
}

void lv_obj_set_style_outline_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_outline_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_outline_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_OPA, v, selector);
}

void lv_obj_set_style_outline_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_outline_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_outline_pad(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OUTLINE_PAD, v, selector);
}

void lv_obj_set_style_outline_pad_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_PAD, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_outline_pad_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OUTLINE_PAD, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_WIDTH, v, selector);
}

void lv_obj_set_style_shadow_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_ofs_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_X, v, selector);
}

void lv_obj_set_style_shadow_ofs_x_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OFS_X, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_ofs_x_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OFS_X, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_ofs_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OFS_Y, v, selector);
}

void lv_obj_set_style_shadow_ofs_y_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OFS_Y, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_ofs_y_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OFS_Y, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_spread(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_SPREAD, v, selector);
}

void lv_obj_set_style_shadow_spread_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_SPREAD, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_spread_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_SPREAD, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_COLOR, v, selector);
}

void lv_obj_set_style_shadow_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_shadow_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_SHADOW_OPA, v, selector);
}

void lv_obj_set_style_shadow_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_shadow_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_SHADOW_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_OPA, v, selector);
}

void lv_obj_set_style_img_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_img_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR, v, selector);
}

void lv_obj_set_style_img_recolor_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_RECOLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_img_recolor_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_RECOLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_IMG_RECOLOR_OPA, v, selector);
}

void lv_obj_set_style_img_recolor_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_img_recolor_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_IMG_RECOLOR_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_WIDTH, v, selector);
}

void lv_obj_set_style_line_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_dash_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_WIDTH, v, selector);
}

void lv_obj_set_style_line_dash_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_DASH_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_dash_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_DASH_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_dash_gap(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_DASH_GAP, v, selector);
}

void lv_obj_set_style_line_dash_gap_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_DASH_GAP, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_dash_gap_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_DASH_GAP, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_ROUNDED, v, selector);
}

void lv_obj_set_style_line_rounded_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_ROUNDED, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_rounded_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_ROUNDED, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_COLOR, v, selector);
}

void lv_obj_set_style_line_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_line_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LINE_OPA, v, selector);
}

void lv_obj_set_style_line_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_line_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LINE_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_arc_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_WIDTH, v, selector);
}

void lv_obj_set_style_arc_width_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_WIDTH, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_arc_width_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_WIDTH, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_arc_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_ROUNDED, v, selector);
}

void lv_obj_set_style_arc_rounded_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_ROUNDED, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_arc_rounded_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_ROUNDED, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_arc_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_COLOR, v, selector);
}

void lv_obj_set_style_arc_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_arc_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_arc_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_OPA, v, selector);
}

void lv_obj_set_style_arc_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_arc_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_arc_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ARC_IMG_SRC, v, selector);
}

void lv_obj_set_style_arc_img_src_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_IMG_SRC, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_arc_img_src_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ARC_IMG_SRC, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_COLOR, v, selector);
}

void lv_obj_set_style_text_color_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_color_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_COLOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_OPA, v, selector);
}

void lv_obj_set_style_text_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_font(struct _lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_FONT, v, selector);
}

void lv_obj_set_style_text_font_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_FONT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_font_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_FONT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_letter_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LETTER_SPACE, v, selector);
}

void lv_obj_set_style_text_letter_space_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_LETTER_SPACE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_letter_space_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_LETTER_SPACE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_line_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_LINE_SPACE, v, selector);
}

void lv_obj_set_style_text_line_space_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_LINE_SPACE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_line_space_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_LINE_SPACE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_decor(struct _lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_DECOR, v, selector);
}

void lv_obj_set_style_text_decor_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_DECOR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_decor_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_DECOR, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_text_align(struct _lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TEXT_ALIGN, v, selector);
}

void lv_obj_set_style_text_align_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_ALIGN, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_text_align_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TEXT_ALIGN, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_radius(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_RADIUS, v, selector);
}

void lv_obj_set_style_radius_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_RADIUS, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_radius_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_RADIUS, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_clip_corner(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_CLIP_CORNER, v, selector);
}

void lv_obj_set_style_clip_corner_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_CLIP_CORNER, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_clip_corner_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_CLIP_CORNER, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_OPA, v, selector);
}

void lv_obj_set_style_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_color_filter_dsc(struct _lv_obj_t * obj, const lv_color_filter_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_DSC, v, selector);
}

void lv_obj_set_style_color_filter_dsc_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_COLOR_FILTER_DSC, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_color_filter_dsc_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_COLOR_FILTER_DSC, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_color_filter_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_COLOR_FILTER_OPA, v, selector);
}

void lv_obj_set_style_color_filter_opa_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_color_filter_opa_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_COLOR_FILTER_OPA, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_anim(struct _lv_obj_t * obj, const lv_anim_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM, v, selector);
}

void lv_obj_set_style_anim_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_anim_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_anim_time(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_TIME, v, selector);
}

void lv_obj_set_style_anim_time_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM_TIME, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_anim_time_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM_TIME, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_anim_speed(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_ANIM_SPEED, v, selector);
}

void lv_obj_set_style_anim_speed_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM_SPEED, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_anim_speed_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_ANIM_SPEED, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_transition(struct _lv_obj_t * obj, const lv_style_transition_dsc_t * value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_TRANSITION, v, selector);
}

void lv_obj_set_style_transition_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSITION, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_transition_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_TRANSITION, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_blend_mode(struct _lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BLEND_MODE, v, selector);
}

void lv_obj_set_style_blend_mode_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BLEND_MODE, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_blend_mode_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BLEND_MODE, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_layout(struct _lv_obj_t * obj, uint16_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_LAYOUT, v, selector);
}

void lv_obj_set_style_layout_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LAYOUT, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_layout_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_LAYOUT, LV_STYLE_PROP_META_INITIAL, selector);
}

void lv_obj_set_style_base_dir(struct _lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_obj_set_local_style_prop(obj, LV_STYLE_BASE_DIR, v, selector);
}

void lv_obj_set_style_base_dir_inherit(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BASE_DIR, LV_STYLE_PROP_META_INHERIT, selector);
}

void lv_obj_set_style_base_dir_initial(struct _lv_obj_t * obj, lv_style_selector_t selector)
{
    lv_obj_set_local_style_prop_meta(obj, LV_STYLE_BASE_DIR, LV_STYLE_PROP_META_INITIAL, selector);
}

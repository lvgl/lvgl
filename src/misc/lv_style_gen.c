#include "lv_style.h"

void lv_style_set_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_WIDTH = LV_STYLE_WIDTH;

void lv_style_set_min_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_MIN_WIDTH = LV_STYLE_MIN_WIDTH;

void lv_style_set_max_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_MAX_WIDTH = LV_STYLE_MAX_WIDTH;

void lv_style_set_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_HEIGHT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_HEIGHT = LV_STYLE_HEIGHT;

void lv_style_set_min_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MIN_HEIGHT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_MIN_HEIGHT = LV_STYLE_MIN_HEIGHT;

void lv_style_set_max_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_MAX_HEIGHT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_MAX_HEIGHT = LV_STYLE_MAX_HEIGHT;

void lv_style_set_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_X, v);
}

const lv_style_prop_t _lv_style_const_prop_id_X = LV_STYLE_X;

void lv_style_set_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_Y, v);
}

const lv_style_prop_t _lv_style_const_prop_id_Y = LV_STYLE_Y;

void lv_style_set_align(lv_style_t * style, lv_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ALIGN, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ALIGN = LV_STYLE_ALIGN;

void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_WIDTH = LV_STYLE_TRANSFORM_WIDTH;

void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_HEIGHT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_HEIGHT = LV_STYLE_TRANSFORM_HEIGHT;

void lv_style_set_translate_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_X, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSLATE_X = LV_STYLE_TRANSLATE_X;

void lv_style_set_translate_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSLATE_Y, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSLATE_Y = LV_STYLE_TRANSLATE_Y;

void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ZOOM, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_ZOOM = LV_STYLE_TRANSFORM_ZOOM;

void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_ANGLE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_ANGLE = LV_STYLE_TRANSFORM_ANGLE;

void lv_style_set_transform_pivot_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_PIVOT_X, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_PIVOT_X = LV_STYLE_TRANSFORM_PIVOT_X;

void lv_style_set_transform_pivot_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSFORM_PIVOT_Y, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSFORM_PIVOT_Y = LV_STYLE_TRANSFORM_PIVOT_Y;

void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_TOP, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_TOP = LV_STYLE_PAD_TOP;

void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_BOTTOM, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_BOTTOM = LV_STYLE_PAD_BOTTOM;

void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_LEFT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_LEFT = LV_STYLE_PAD_LEFT;

void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_RIGHT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_RIGHT = LV_STYLE_PAD_RIGHT;

void lv_style_set_pad_row(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_ROW, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_ROW = LV_STYLE_PAD_ROW;

void lv_style_set_pad_column(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_PAD_COLUMN, v);
}

const lv_style_prop_t _lv_style_const_prop_id_PAD_COLUMN = LV_STYLE_PAD_COLUMN;

void lv_style_set_bg_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_COLOR = LV_STYLE_BG_COLOR;

void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_OPA = LV_STYLE_BG_OPA;

void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_GRAD_COLOR = LV_STYLE_BG_GRAD_COLOR;

void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_DIR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_GRAD_DIR = LV_STYLE_BG_GRAD_DIR;

void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_MAIN_STOP, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_MAIN_STOP = LV_STYLE_BG_MAIN_STOP;

void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD_STOP, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_GRAD_STOP = LV_STYLE_BG_GRAD_STOP;

void lv_style_set_bg_grad(lv_style_t * style, const lv_grad_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_GRAD, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_GRAD = LV_STYLE_BG_GRAD;

void lv_style_set_bg_dither_mode(lv_style_t * style, lv_dither_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_DITHER_MODE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_DITHER_MODE = LV_STYLE_BG_DITHER_MODE;

void lv_style_set_bg_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_SRC, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_IMG_SRC = LV_STYLE_BG_IMG_SRC;

void lv_style_set_bg_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_IMG_OPA = LV_STYLE_BG_IMG_OPA;

void lv_style_set_bg_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_IMG_RECOLOR = LV_STYLE_BG_IMG_RECOLOR;

void lv_style_set_bg_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_RECOLOR_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_IMG_RECOLOR_OPA = LV_STYLE_BG_IMG_RECOLOR_OPA;

void lv_style_set_bg_img_tiled(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BG_IMG_TILED, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BG_IMG_TILED = LV_STYLE_BG_IMG_TILED;

void lv_style_set_border_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BORDER_COLOR = LV_STYLE_BORDER_COLOR;

void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BORDER_OPA = LV_STYLE_BORDER_OPA;

void lv_style_set_border_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BORDER_WIDTH = LV_STYLE_BORDER_WIDTH;

void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_SIDE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BORDER_SIDE = LV_STYLE_BORDER_SIDE;

void lv_style_set_border_post(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BORDER_POST, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BORDER_POST = LV_STYLE_BORDER_POST;

void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_OUTLINE_WIDTH = LV_STYLE_OUTLINE_WIDTH;

void lv_style_set_outline_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_OUTLINE_COLOR = LV_STYLE_OUTLINE_COLOR;

void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_OUTLINE_OPA = LV_STYLE_OUTLINE_OPA;

void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OUTLINE_PAD, v);
}

const lv_style_prop_t _lv_style_const_prop_id_OUTLINE_PAD = LV_STYLE_OUTLINE_PAD;

void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_WIDTH = LV_STYLE_SHADOW_WIDTH;

void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_X, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_OFS_X = LV_STYLE_SHADOW_OFS_X;

void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OFS_Y, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_OFS_Y = LV_STYLE_SHADOW_OFS_Y;

void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_SPREAD, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_SPREAD = LV_STYLE_SHADOW_SPREAD;

void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_COLOR = LV_STYLE_SHADOW_COLOR;

void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_SHADOW_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_SHADOW_OPA = LV_STYLE_SHADOW_OPA;

void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_IMG_OPA = LV_STYLE_IMG_OPA;

void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_IMG_RECOLOR = LV_STYLE_IMG_RECOLOR;

void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_IMG_RECOLOR_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_IMG_RECOLOR_OPA = LV_STYLE_IMG_RECOLOR_OPA;

void lv_style_set_line_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_WIDTH = LV_STYLE_LINE_WIDTH;

void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_DASH_WIDTH = LV_STYLE_LINE_DASH_WIDTH;

void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_DASH_GAP, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_DASH_GAP = LV_STYLE_LINE_DASH_GAP;

void lv_style_set_line_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_ROUNDED, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_ROUNDED = LV_STYLE_LINE_ROUNDED;

void lv_style_set_line_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_COLOR = LV_STYLE_LINE_COLOR;

void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LINE_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LINE_OPA = LV_STYLE_LINE_OPA;

void lv_style_set_arc_width(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_WIDTH, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ARC_WIDTH = LV_STYLE_ARC_WIDTH;

void lv_style_set_arc_rounded(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_ROUNDED, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ARC_ROUNDED = LV_STYLE_ARC_ROUNDED;

void lv_style_set_arc_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ARC_COLOR = LV_STYLE_ARC_COLOR;

void lv_style_set_arc_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ARC_OPA = LV_STYLE_ARC_OPA;

void lv_style_set_arc_img_src(lv_style_t * style, const void * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ARC_IMG_SRC, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ARC_IMG_SRC = LV_STYLE_ARC_IMG_SRC;

void lv_style_set_text_color(lv_style_t * style, lv_color_t value)
{
    lv_style_value_t v = {
        .color = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_COLOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_COLOR = LV_STYLE_TEXT_COLOR;

void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_OPA = LV_STYLE_TEXT_OPA;

void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_FONT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_FONT = LV_STYLE_TEXT_FONT;

void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LETTER_SPACE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_LETTER_SPACE = LV_STYLE_TEXT_LETTER_SPACE;

void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_LINE_SPACE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_LINE_SPACE = LV_STYLE_TEXT_LINE_SPACE;

void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_DECOR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_DECOR = LV_STYLE_TEXT_DECOR;

void lv_style_set_text_align(lv_style_t * style, lv_text_align_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_TEXT_ALIGN, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TEXT_ALIGN = LV_STYLE_TEXT_ALIGN;

void lv_style_set_radius(lv_style_t * style, lv_coord_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_RADIUS, v);
}

const lv_style_prop_t _lv_style_const_prop_id_RADIUS = LV_STYLE_RADIUS;

void lv_style_set_clip_corner(lv_style_t * style, bool value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_CLIP_CORNER, v);
}

const lv_style_prop_t _lv_style_const_prop_id_CLIP_CORNER = LV_STYLE_CLIP_CORNER;

void lv_style_set_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_OPA = LV_STYLE_OPA;

void lv_style_set_color_filter_dsc(lv_style_t * style, const lv_color_filter_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_DSC, v);
}

const lv_style_prop_t _lv_style_const_prop_id_COLOR_FILTER_DSC = LV_STYLE_COLOR_FILTER_DSC;

void lv_style_set_color_filter_opa(lv_style_t * style, lv_opa_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_COLOR_FILTER_OPA, v);
}

const lv_style_prop_t _lv_style_const_prop_id_COLOR_FILTER_OPA = LV_STYLE_COLOR_FILTER_OPA;

void lv_style_set_anim(lv_style_t * style, const lv_anim_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ANIM = LV_STYLE_ANIM;

void lv_style_set_anim_time(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_TIME, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ANIM_TIME = LV_STYLE_ANIM_TIME;

void lv_style_set_anim_speed(lv_style_t * style, uint32_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_ANIM_SPEED, v);
}

const lv_style_prop_t _lv_style_const_prop_id_ANIM_SPEED = LV_STYLE_ANIM_SPEED;

void lv_style_set_transition(lv_style_t * style, const lv_style_transition_dsc_t * value)
{
    lv_style_value_t v = {
        .ptr = value
    };
    lv_style_set_prop(style, LV_STYLE_TRANSITION, v);
}

const lv_style_prop_t _lv_style_const_prop_id_TRANSITION = LV_STYLE_TRANSITION;

void lv_style_set_blend_mode(lv_style_t * style, lv_blend_mode_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BLEND_MODE, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BLEND_MODE = LV_STYLE_BLEND_MODE;

void lv_style_set_layout(lv_style_t * style, uint16_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_LAYOUT, v);
}

const lv_style_prop_t _lv_style_const_prop_id_LAYOUT = LV_STYLE_LAYOUT;

void lv_style_set_base_dir(lv_style_t * style, lv_base_dir_t value)
{
    lv_style_value_t v = {
        .num = (int32_t)value
    };
    lv_style_set_prop(style, LV_STYLE_BASE_DIR, v);
}

const lv_style_prop_t _lv_style_const_prop_id_BASE_DIR = LV_STYLE_BASE_DIR;

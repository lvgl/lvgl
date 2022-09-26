static inline lv_coord_t lv_obj_get_style_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_min_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MIN_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_max_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MAX_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_height(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_HEIGHT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_min_height(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MIN_HEIGHT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_max_height(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_MAX_HEIGHT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_x(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_X);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_y(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_Y);
    return (lv_coord_t)v.num;
}

static inline lv_align_t lv_obj_get_style_align(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ALIGN);
    return (lv_align_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_height(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_HEIGHT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_translate_x(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSLATE_X);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_translate_y(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSLATE_Y);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_zoom(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ZOOM);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_angle(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_ANGLE);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_pivot_x(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_PIVOT_X);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_transform_pivot_y(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSFORM_PIVOT_Y);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_top(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_TOP);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_bottom(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_BOTTOM);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_left(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_LEFT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_right(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_RIGHT);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_row(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_ROW);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_pad_column(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_PAD_COLUMN);
    return (lv_coord_t)v.num;
}

static inline lv_color_t lv_obj_get_style_bg_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_bg_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_bg_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_color_t lv_obj_get_style_bg_grad_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_bg_grad_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_COLOR));
    return v.color;
}

static inline lv_grad_dir_t lv_obj_get_style_bg_grad_dir(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_DIR);
    return (lv_grad_dir_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_bg_main_stop(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_MAIN_STOP);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_bg_grad_stop(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD_STOP);
    return (lv_coord_t)v.num;
}

static inline const lv_grad_dsc_t * lv_obj_get_style_bg_grad(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_GRAD);
    return (const lv_grad_dsc_t *)v.ptr;
}

static inline lv_dither_mode_t lv_obj_get_style_bg_dither_mode(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_DITHER_MODE);
    return (lv_dither_mode_t)v.num;
}

static inline const void * lv_obj_get_style_bg_img_src(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_SRC);
    return (const void *)v.ptr;
}

static inline lv_opa_t lv_obj_get_style_bg_img_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_color_t lv_obj_get_style_bg_img_recolor(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_bg_img_recolor_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_bg_img_recolor_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_RECOLOR_OPA);
    return (lv_opa_t)v.num;
}

static inline bool lv_obj_get_style_bg_img_tiled(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BG_IMG_TILED);
    return (bool)v.num;
}

static inline lv_color_t lv_obj_get_style_border_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_border_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_border_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_border_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_border_side_t lv_obj_get_style_border_side(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_SIDE);
    return (lv_border_side_t)v.num;
}

static inline bool lv_obj_get_style_border_post(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BORDER_POST);
    return (bool)v.num;
}

static inline lv_coord_t lv_obj_get_style_outline_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_color_t lv_obj_get_style_outline_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_outline_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_outline_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_outline_pad(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OUTLINE_PAD);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_shadow_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_shadow_ofs_x(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_X);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_shadow_ofs_y(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OFS_Y);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_shadow_spread(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_SPREAD);
    return (lv_coord_t)v.num;
}

static inline lv_color_t lv_obj_get_style_shadow_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_shadow_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_shadow_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_SHADOW_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_opa_t lv_obj_get_style_img_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_color_t lv_obj_get_style_img_recolor(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_img_recolor_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_img_recolor_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_IMG_RECOLOR_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_line_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_line_dash_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_WIDTH);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_line_dash_gap(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_DASH_GAP);
    return (lv_coord_t)v.num;
}

static inline bool lv_obj_get_style_line_rounded(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_ROUNDED);
    return (bool)v.num;
}

static inline lv_color_t lv_obj_get_style_line_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_line_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_line_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LINE_OPA);
    return (lv_opa_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_arc_width(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_WIDTH);
    return (lv_coord_t)v.num;
}

static inline bool lv_obj_get_style_arc_rounded(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_ROUNDED);
    return (bool)v.num;
}

static inline lv_color_t lv_obj_get_style_arc_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_arc_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_arc_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_OPA);
    return (lv_opa_t)v.num;
}

static inline const void * lv_obj_get_style_arc_img_src(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ARC_IMG_SRC);
    return (const void *)v.ptr;
}

static inline lv_color_t lv_obj_get_style_text_color(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR);
    return v.color;
}

static inline lv_color_t lv_obj_get_style_text_color_filtered(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_COLOR));
    return v.color;
}

static inline lv_opa_t lv_obj_get_style_text_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_OPA);
    return (lv_opa_t)v.num;
}

static inline const lv_font_t * lv_obj_get_style_text_font(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_FONT);
    return (const lv_font_t *)v.ptr;
}

static inline lv_coord_t lv_obj_get_style_text_letter_space(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LETTER_SPACE);
    return (lv_coord_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_text_line_space(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_LINE_SPACE);
    return (lv_coord_t)v.num;
}

static inline lv_text_decor_t lv_obj_get_style_text_decor(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_DECOR);
    return (lv_text_decor_t)v.num;
}

static inline lv_text_align_t lv_obj_get_style_text_align(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TEXT_ALIGN);
    return (lv_text_align_t)v.num;
}

static inline lv_coord_t lv_obj_get_style_radius(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_RADIUS);
    return (lv_coord_t)v.num;
}

static inline bool lv_obj_get_style_clip_corner(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_CLIP_CORNER);
    return (bool)v.num;
}

static inline lv_opa_t lv_obj_get_style_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_OPA);
    return (lv_opa_t)v.num;
}

static inline const lv_color_filter_dsc_t * lv_obj_get_style_color_filter_dsc(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_DSC);
    return (const lv_color_filter_dsc_t *)v.ptr;
}

static inline lv_opa_t lv_obj_get_style_color_filter_opa(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_COLOR_FILTER_OPA);
    return (lv_opa_t)v.num;
}

static inline const lv_anim_t * lv_obj_get_style_anim(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM);
    return (const lv_anim_t *)v.ptr;
}

static inline uint32_t lv_obj_get_style_anim_time(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM_TIME);
    return (uint32_t)v.num;
}

static inline uint32_t lv_obj_get_style_anim_speed(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_ANIM_SPEED);
    return (uint32_t)v.num;
}

static inline const lv_style_transition_dsc_t * lv_obj_get_style_transition(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_TRANSITION);
    return (const lv_style_transition_dsc_t *)v.ptr;
}

static inline lv_blend_mode_t lv_obj_get_style_blend_mode(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BLEND_MODE);
    return (lv_blend_mode_t)v.num;
}

static inline uint16_t lv_obj_get_style_layout(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_LAYOUT);
    return (uint16_t)v.num;
}

static inline lv_base_dir_t lv_obj_get_style_base_dir(const struct _lv_obj_t * obj, uint32_t part)
{
    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_BASE_DIR);
    return (lv_base_dir_t)v.num;
}

void lv_obj_set_style_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_min_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_max_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_min_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_max_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_align(struct _lv_obj_t * obj, lv_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_height(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_translate_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_translate_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_zoom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_angle(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_pivot_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_transform_pivot_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_top(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_bottom(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_left(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_right(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_row(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_pad_column(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_dir(struct _lv_obj_t * obj, lv_grad_dir_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_main_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad_stop(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_grad(struct _lv_obj_t * obj, const lv_grad_dsc_t * value, lv_style_selector_t selector);
void lv_obj_set_style_bg_dither_mode(struct _lv_obj_t * obj, lv_dither_mode_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_bg_img_tiled(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_border_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_side(struct _lv_obj_t * obj, lv_border_side_t value, lv_style_selector_t selector);
void lv_obj_set_style_border_post(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_outline_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_outline_pad(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_ofs_x(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_ofs_y(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_spread(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_shadow_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_recolor(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_img_recolor_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_dash_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_dash_gap(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_line_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_line_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_width(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_rounded(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_arc_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_arc_img_src(struct _lv_obj_t * obj, const void * value, lv_style_selector_t selector);
void lv_obj_set_style_text_color(struct _lv_obj_t * obj, lv_color_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_font(struct _lv_obj_t * obj, const lv_font_t * value, lv_style_selector_t selector);
void lv_obj_set_style_text_letter_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_line_space(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_decor(struct _lv_obj_t * obj, lv_text_decor_t value, lv_style_selector_t selector);
void lv_obj_set_style_text_align(struct _lv_obj_t * obj, lv_text_align_t value, lv_style_selector_t selector);
void lv_obj_set_style_radius(struct _lv_obj_t * obj, lv_coord_t value, lv_style_selector_t selector);
void lv_obj_set_style_clip_corner(struct _lv_obj_t * obj, bool value, lv_style_selector_t selector);
void lv_obj_set_style_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_color_filter_dsc(struct _lv_obj_t * obj, const lv_color_filter_dsc_t * value, lv_style_selector_t selector);
void lv_obj_set_style_color_filter_opa(struct _lv_obj_t * obj, lv_opa_t value, lv_style_selector_t selector);
void lv_obj_set_style_anim(struct _lv_obj_t * obj, const lv_anim_t * value, lv_style_selector_t selector);
void lv_obj_set_style_anim_time(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);
void lv_obj_set_style_anim_speed(struct _lv_obj_t * obj, uint32_t value, lv_style_selector_t selector);
void lv_obj_set_style_transition(struct _lv_obj_t * obj, const lv_style_transition_dsc_t * value, lv_style_selector_t selector);
void lv_obj_set_style_blend_mode(struct _lv_obj_t * obj, lv_blend_mode_t value, lv_style_selector_t selector);
void lv_obj_set_style_layout(struct _lv_obj_t * obj, uint16_t value, lv_style_selector_t selector);
void lv_obj_set_style_base_dir(struct _lv_obj_t * obj, lv_base_dir_t value, lv_style_selector_t selector);

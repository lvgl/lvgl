void lv_style_set_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_min_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_max_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_height(lv_style_t * style, lv_coord_t value);
void lv_style_set_min_height(lv_style_t * style, lv_coord_t value);
void lv_style_set_max_height(lv_style_t * style, lv_coord_t value);
void lv_style_set_x(lv_style_t * style, lv_coord_t value);
void lv_style_set_y(lv_style_t * style, lv_coord_t value);
void lv_style_set_align(lv_style_t * style, lv_align_t value);
void lv_style_set_transform_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_transform_height(lv_style_t * style, lv_coord_t value);
void lv_style_set_translate_x(lv_style_t * style, lv_coord_t value);
void lv_style_set_translate_y(lv_style_t * style, lv_coord_t value);
void lv_style_set_transform_zoom(lv_style_t * style, lv_coord_t value);
void lv_style_set_transform_angle(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_top(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_bottom(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_left(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_right(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_row(lv_style_t * style, lv_coord_t value);
void lv_style_set_pad_column(lv_style_t * style, lv_coord_t value);
void lv_style_set_radius(lv_style_t * style, lv_coord_t value);
void lv_style_set_clip_corner(lv_style_t * style, bool value);
void lv_style_set_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_color_filter_dsc(lv_style_t * style, const lv_color_filter_dsc_t * value);
void lv_style_set_color_filter_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_anim_time(lv_style_t * style, uint32_t value);
void lv_style_set_anim_speed(lv_style_t * style, uint32_t value);
void lv_style_set_transition(lv_style_t * style, const lv_style_transition_dsc_t * value);
void lv_style_set_blend_mode(lv_style_t * style, lv_blend_mode_t value);
void lv_style_set_layout(lv_style_t * style, uint16_t value);
void lv_style_set_base_dir(lv_style_t * style, lv_base_dir_t value);
void lv_style_set_bg_color(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_bg_grad_color(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_grad_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_grad_dir(lv_style_t * style, lv_grad_dir_t value);
void lv_style_set_bg_main_stop(lv_style_t * style, lv_coord_t value);
void lv_style_set_bg_grad_stop(lv_style_t * style, lv_coord_t value);
void lv_style_set_bg_img_src(lv_style_t * style, const void * value);
void lv_style_set_bg_img_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_bg_img_recolor(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_img_recolor_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_bg_img_recolor_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_bg_img_tiled(lv_style_t * style, bool value);
void lv_style_set_border_color(lv_style_t * style, lv_color_t value);
void lv_style_set_border_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_border_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_border_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_border_side(lv_style_t * style, lv_border_side_t value);
void lv_style_set_border_post(lv_style_t * style, bool value);
void lv_style_set_text_color(lv_style_t * style, lv_color_t value);
void lv_style_set_text_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_text_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_text_font(lv_style_t * style, const lv_font_t * value);
void lv_style_set_text_letter_space(lv_style_t * style, lv_coord_t value);
void lv_style_set_text_line_space(lv_style_t * style, lv_coord_t value);
void lv_style_set_text_decor(lv_style_t * style, lv_text_decor_t value);
void lv_style_set_text_align(lv_style_t * style, lv_text_align_t value);
void lv_style_set_img_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_img_recolor(lv_style_t * style, lv_color_t value);
void lv_style_set_img_recolor_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_img_recolor_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_outline_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_outline_color(lv_style_t * style, lv_color_t value);
void lv_style_set_outline_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_outline_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_outline_pad(lv_style_t * style, lv_coord_t value);
void lv_style_set_shadow_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_shadow_ofs_x(lv_style_t * style, lv_coord_t value);
void lv_style_set_shadow_ofs_y(lv_style_t * style, lv_coord_t value);
void lv_style_set_shadow_spread(lv_style_t * style, lv_coord_t value);
void lv_style_set_shadow_color(lv_style_t * style, lv_color_t value);
void lv_style_set_shadow_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_shadow_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_line_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_line_dash_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_line_dash_gap(lv_style_t * style, lv_coord_t value);
void lv_style_set_line_rounded(lv_style_t * style, bool value);
void lv_style_set_line_color(lv_style_t * style, lv_color_t value);
void lv_style_set_line_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_line_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_arc_width(lv_style_t * style, lv_coord_t value);
void lv_style_set_arc_rounded(lv_style_t * style, bool value);
void lv_style_set_arc_color(lv_style_t * style, lv_color_t value);
void lv_style_set_arc_color_filtered(lv_style_t * style, lv_color_t value);
void lv_style_set_arc_opa(lv_style_t * style, lv_opa_t value);
void lv_style_set_arc_img_src(lv_style_t * style, const void * value);
#define LV_STYLE_CONST_WIDTH(val) \
    { \
        .prop = LV_STYLE_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_MIN_WIDTH(val) \
    { \
        .prop = LV_STYLE_MIN_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_MAX_WIDTH(val) \
    { \
        .prop = LV_STYLE_MAX_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_HEIGHT(val) \
    { \
        .prop = LV_STYLE_HEIGHT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_MIN_HEIGHT(val) \
    { \
        .prop = LV_STYLE_MIN_HEIGHT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_MAX_HEIGHT(val) \
    { \
        .prop = LV_STYLE_MAX_HEIGHT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_X(val) \
    { \
        .prop = LV_STYLE_X, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_Y(val) \
    { \
        .prop = LV_STYLE_Y, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ALIGN(val) \
    { \
        .prop = LV_STYLE_ALIGN, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSFORM_WIDTH(val) \
    { \
        .prop = LV_STYLE_TRANSFORM_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSFORM_HEIGHT(val) \
    { \
        .prop = LV_STYLE_TRANSFORM_HEIGHT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSLATE_X(val) \
    { \
        .prop = LV_STYLE_TRANSLATE_X, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSLATE_Y(val) \
    { \
        .prop = LV_STYLE_TRANSLATE_Y, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSFORM_ZOOM(val) \
    { \
        .prop = LV_STYLE_TRANSFORM_ZOOM, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSFORM_ANGLE(val) \
    { \
        .prop = LV_STYLE_TRANSFORM_ANGLE, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_TOP(val) \
    { \
        .prop = LV_STYLE_PAD_TOP, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_BOTTOM(val) \
    { \
        .prop = LV_STYLE_PAD_BOTTOM, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_LEFT(val) \
    { \
        .prop = LV_STYLE_PAD_LEFT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_RIGHT(val) \
    { \
        .prop = LV_STYLE_PAD_RIGHT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_ROW(val) \
    { \
        .prop = LV_STYLE_PAD_ROW, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_PAD_COLUMN(val) \
    { \
        .prop = LV_STYLE_PAD_COLUMN, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_RADIUS(val) \
    { \
        .prop = LV_STYLE_RADIUS, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_CLIP_CORNER(val) \
    { \
        .prop = LV_STYLE_CLIP_CORNER, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_OPA(val) \
    { \
        .prop = LV_STYLE_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_COLOR_FILTER_DSC(val) \
    { \
        .prop = LV_STYLE_COLOR_FILTER_DSC, \
        .value = { \
            .ptr = val \
        } \
    }

#define LV_STYLE_CONST_COLOR_FILTER_OPA(val) \
    { \
        .prop = LV_STYLE_COLOR_FILTER_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ANIM_TIME(val) \
    { \
        .prop = LV_STYLE_ANIM_TIME, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ANIM_SPEED(val) \
    { \
        .prop = LV_STYLE_ANIM_SPEED, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TRANSITION(val) \
    { \
        .prop = LV_STYLE_TRANSITION, \
        .value = { \
            .ptr = val \
        } \
    }

#define LV_STYLE_CONST_BLEND_MODE(val) \
    { \
        .prop = LV_STYLE_BLEND_MODE, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LAYOUT(val) \
    { \
        .prop = LV_STYLE_LAYOUT, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BASE_DIR(val) \
    { \
        .prop = LV_STYLE_BASE_DIR, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_COLOR(val) \
    { \
        .prop = LV_STYLE_BG_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_BG_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_OPA(val) \
    { \
        .prop = LV_STYLE_BG_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_GRAD_COLOR(val) \
    { \
        .prop = LV_STYLE_BG_GRAD_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_GRAD_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_BG_GRAD_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_GRAD_DIR(val) \
    { \
        .prop = LV_STYLE_BG_GRAD_DIR, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_MAIN_STOP(val) \
    { \
        .prop = LV_STYLE_BG_MAIN_STOP, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_GRAD_STOP(val) \
    { \
        .prop = LV_STYLE_BG_GRAD_STOP, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_SRC(val) \
    { \
        .prop = LV_STYLE_BG_IMG_SRC, \
        .value = { \
            .ptr = val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_OPA(val) \
    { \
        .prop = LV_STYLE_BG_IMG_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_RECOLOR(val) \
    { \
        .prop = LV_STYLE_BG_IMG_RECOLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_RECOLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_BG_IMG_RECOLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_RECOLOR_OPA(val) \
    { \
        .prop = LV_STYLE_BG_IMG_RECOLOR_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BG_IMG_TILED(val) \
    { \
        .prop = LV_STYLE_BG_IMG_TILED, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BORDER_COLOR(val) \
    { \
        .prop = LV_STYLE_BORDER_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BORDER_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_BORDER_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_BORDER_OPA(val) \
    { \
        .prop = LV_STYLE_BORDER_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BORDER_WIDTH(val) \
    { \
        .prop = LV_STYLE_BORDER_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BORDER_SIDE(val) \
    { \
        .prop = LV_STYLE_BORDER_SIDE, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_BORDER_POST(val) \
    { \
        .prop = LV_STYLE_BORDER_POST, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TEXT_COLOR(val) \
    { \
        .prop = LV_STYLE_TEXT_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_TEXT_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_TEXT_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_TEXT_OPA(val) \
    { \
        .prop = LV_STYLE_TEXT_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TEXT_FONT(val) \
    { \
        .prop = LV_STYLE_TEXT_FONT, \
        .value = { \
            .ptr = val \
        } \
    }

#define LV_STYLE_CONST_TEXT_LETTER_SPACE(val) \
    { \
        .prop = LV_STYLE_TEXT_LETTER_SPACE, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TEXT_LINE_SPACE(val) \
    { \
        .prop = LV_STYLE_TEXT_LINE_SPACE, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TEXT_DECOR(val) \
    { \
        .prop = LV_STYLE_TEXT_DECOR, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_TEXT_ALIGN(val) \
    { \
        .prop = LV_STYLE_TEXT_ALIGN, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_IMG_OPA(val) \
    { \
        .prop = LV_STYLE_IMG_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_IMG_RECOLOR(val) \
    { \
        .prop = LV_STYLE_IMG_RECOLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_IMG_RECOLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_IMG_RECOLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_IMG_RECOLOR_OPA(val) \
    { \
        .prop = LV_STYLE_IMG_RECOLOR_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_OUTLINE_WIDTH(val) \
    { \
        .prop = LV_STYLE_OUTLINE_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_OUTLINE_COLOR(val) \
    { \
        .prop = LV_STYLE_OUTLINE_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_OUTLINE_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_OUTLINE_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_OUTLINE_OPA(val) \
    { \
        .prop = LV_STYLE_OUTLINE_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_OUTLINE_PAD(val) \
    { \
        .prop = LV_STYLE_OUTLINE_PAD, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_WIDTH(val) \
    { \
        .prop = LV_STYLE_SHADOW_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_OFS_X(val) \
    { \
        .prop = LV_STYLE_SHADOW_OFS_X, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_OFS_Y(val) \
    { \
        .prop = LV_STYLE_SHADOW_OFS_Y, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_SPREAD(val) \
    { \
        .prop = LV_STYLE_SHADOW_SPREAD, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_COLOR(val) \
    { \
        .prop = LV_STYLE_SHADOW_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_SHADOW_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_SHADOW_OPA(val) \
    { \
        .prop = LV_STYLE_SHADOW_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LINE_WIDTH(val) \
    { \
        .prop = LV_STYLE_LINE_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LINE_DASH_WIDTH(val) \
    { \
        .prop = LV_STYLE_LINE_DASH_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LINE_DASH_GAP(val) \
    { \
        .prop = LV_STYLE_LINE_DASH_GAP, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LINE_ROUNDED(val) \
    { \
        .prop = LV_STYLE_LINE_ROUNDED, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_LINE_COLOR(val) \
    { \
        .prop = LV_STYLE_LINE_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_LINE_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_LINE_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_LINE_OPA(val) \
    { \
        .prop = LV_STYLE_LINE_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ARC_WIDTH(val) \
    { \
        .prop = LV_STYLE_ARC_WIDTH, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ARC_ROUNDED(val) \
    { \
        .prop = LV_STYLE_ARC_ROUNDED, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ARC_COLOR(val) \
    { \
        .prop = LV_STYLE_ARC_COLOR, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_ARC_COLOR_FILTERED(val) \
    { \
        .prop = LV_STYLE_ARC_COLOR_FILTERED, \
        .value = { \
            .color = val \
        } \
    }

#define LV_STYLE_CONST_ARC_OPA(val) \
    { \
        .prop = LV_STYLE_ARC_OPA, \
        .value = { \
            .num = (int32_t)val \
        } \
    }

#define LV_STYLE_CONST_ARC_IMG_SRC(val) \
    { \
        .prop = LV_STYLE_ARC_IMG_SRC, \
        .value = { \
            .ptr = val \
        } \
    }


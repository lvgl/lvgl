/**
 * @file lv_demo_ebike_settings.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_ebike.h"
#if LV_USE_DEMO_EBIKE

#include "translations/lv_i18n.h"
#include "lv_demo_ebike_settings.h"
#include "lv_demo_ebike_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * left_cont_create(lv_obj_t * parent);
static lv_obj_t * right_cont_create(lv_obj_t * parent);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_ebike_settings_create(lv_obj_t * parent)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(main_cont, LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);

    lv_obj_t * left_cont = left_cont_create(main_cont);
#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_size(left_cont, lv_pct(100), 120);
#else
    lv_obj_set_size(left_cont, 164, lv_pct(100));
#endif
    lv_obj_t * right_cont = right_cont_create(main_cont);
    lv_obj_set_size(right_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(right_cont, 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * left_cont_create(lv_obj_t * parent)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * label = lv_label_create(cont);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    lv_label_set_text(label, _("SETTINGS"));
    lv_obj_set_style_text_font(label, EBIKE_FONT_MEDIUM, 0);

    lv_obj_t * settings_img;
#if LV_USE_LOTTIE
    extern const uint8_t lottie_ebike_settings[];
    extern const size_t lottie_ebike_settings_size;
    settings_img = lv_lottie_create(cont);
    lv_lottie_set_src_data(settings_img, lottie_ebike_settings, lottie_ebike_settings_size);
    lv_lottie_set_draw_buf(settings_img, lv_demo_ebike_get_lottie_draw_buf());
#else
    settings_img = lv_image_create(cont);
    LV_IMAGE_DECLARE(img_ebike_settings_large);
    lv_image_set_src(settings_img, &img_ebike_settings_large);
#endif

#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_align(settings_img, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#else
    lv_obj_align(settings_img, LV_ALIGN_BOTTOM_MID, 0, 0);
#endif
    return cont;
}

static lv_obj_t * slider_create(lv_obj_t * parent, const char * title)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_text_font(cont, EBIKE_FONT_SMALL, 0);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_style_pad_column(cont, 4, 0);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text(label, title);
    lv_obj_set_width(label, lv_pct(100));

    lv_obj_t * slider = lv_slider_create(cont);
    lv_obj_set_size(slider, lv_pct(100), 4);
    lv_obj_set_ext_click_area(slider, 24);
    lv_obj_set_style_bg_opa(slider, LV_OPA_30, 0);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(slider, EBIKE_COLOR_TURQUOISE, 0);
    lv_obj_set_style_bg_color(slider, EBIKE_COLOR_TURQUOISE, LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_pad_all(slider, 8, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_border_width(slider, 4, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider, EBIKE_COLOR_TURQUOISE, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_margin_top(slider, 12, 0);

    return cont;
}

static lv_obj_t * switch_create(lv_obj_t * parent, const char * title, lv_subject_t * subject)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_flex_cross_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(cont, EBIKE_FONT_SMALL, 0);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_style_pad_column(cont, 4, 0);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text(label, title);
    lv_obj_set_width(label, lv_pct(100));

    lv_obj_t * sw = lv_switch_create(cont);
    lv_obj_set_size(sw, 40, 24);
    lv_obj_set_ext_click_area(sw, 32);
    lv_obj_set_style_radius(sw, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(sw, EBIKE_COLOR_TURQUOISE, 0);

    lv_obj_set_style_radius(sw, LV_RADIUS_CIRCLE, LV_PART_KNOB);
    lv_obj_set_style_bg_color(sw, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_all(sw, -2, LV_PART_KNOB);

    if(subject) lv_button_bind_checked(sw, subject);

    return cont;
}


static lv_obj_t * dropdown_create(lv_obj_t * parent, const char * title, const char * options, lv_subject_t * subject)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_flex_cross_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(cont, EBIKE_FONT_SMALL, 0);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_style_pad_column(cont, 4, 0);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text(label, title);

    LV_IMAGE_DECLARE(img_ebike_dropdown_icon);
    lv_obj_t * dd = lv_dropdown_create(cont);
    lv_dropdown_set_options(dd, options);
    lv_obj_set_style_bg_color(dd, EBIKE_COLOR_TURQUOISE, 0);
    lv_obj_set_style_bg_opa(dd, LV_OPA_40, 0);
    lv_obj_set_style_radius(dd, 4, 0);
    lv_obj_set_width(dd, 150);
    lv_obj_set_style_pad_all(dd, 8, 0);
    lv_dropdown_set_symbol(dd, &img_ebike_dropdown_icon);
    lv_dropdown_bind_value(dd, subject);

    lv_obj_t * list = lv_dropdown_get_list(dd);
    lv_obj_set_style_bg_color(list, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(list, EBIKE_COLOR_TURQUOISE, LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(list, LV_OPA_20, LV_PART_SELECTED | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(list, EBIKE_COLOR_TURQUOISE, LV_PART_SELECTED | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(list, LV_OPA_40, LV_PART_SELECTED | LV_STATE_PRESSED);
    lv_obj_set_style_radius(list, 4, 0);
    lv_obj_set_style_text_line_space(list, 16, 0);
    lv_obj_set_style_text_font(list, EBIKE_FONT_SMALL, 0);
    lv_obj_set_style_pad_all(list, 16, 0);
    return cont;
}

static lv_obj_t * right_cont_create(lv_obj_t * parent)
{
    lv_obj_t * right_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(right_cont, 0, 0);
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(right_cont, 12, 0);
    lv_obj_set_style_pad_right(right_cont, 8, 0);
    lv_obj_set_style_pad_gap(right_cont, 8, 0);
    lv_obj_set_style_width(right_cont, 3, LV_PART_SCROLLBAR);
    lv_obj_set_style_pad_ver(right_cont, 8, LV_PART_SCROLLBAR);
    lv_obj_set_style_radius(right_cont, 2, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_opa(right_cont, LV_OPA_40, LV_PART_SCROLLBAR);
    lv_obj_set_style_bg_color(right_cont, EBIKE_COLOR_TURQUOISE, LV_PART_SCROLLBAR);
    if(lv_strcmp(lv_i18n_get_current_locale(), "ar") == 0) {
        lv_obj_set_style_base_dir(right_cont, LV_BASE_DIR_RTL, 0);
    }
    else {
        lv_obj_set_style_base_dir(right_cont, LV_BASE_DIR_LTR, 0);
    }

    dropdown_create(right_cont, _("Language"), "English\n简体中文\nعربي", &ebike_subject_language);
    switch_create(right_cont, _("Bluetooth"), NULL);
    switch_create(right_cont, _("Lights"), NULL);
    slider_create(right_cont, _("Brightness"));
    slider_create(right_cont, _("Volume"));
    slider_create(right_cont, _("Max. speed"));
    slider_create(right_cont, _("Light level"));

    return right_cont;
}

#endif /*LV_USE_DEMO_EBIKE*/


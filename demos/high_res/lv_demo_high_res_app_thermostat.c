/**
 * @file lv_demo_high_res_app_thermostat.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"
#include "../../src/widgets/chart/lv_chart.h"
#include "../../src/widgets/slider/lv_slider.h"
#include "../../src/widgets/scale/lv_scale.h"
#include "../../src/widgets/arc/lv_arc.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void back_clicked_cb(lv_event_t * e);
static lv_obj_t * widget1_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text);
static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void widget2_slider_changed_cb(lv_event_t * e);
static void widget2_temperature_button_clicked_cb(lv_event_t * e);
static lv_obj_t * create_widget2_value(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                       const char * range_val, int32_t temperature_val);
static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static lv_obj_t * create_widget3_setting(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const lv_image_dsc_t * img_dsc,
                                         const char * text_val, bool active);
static void widget3_setting_clicked_cb(lv_event_t * e);
static void widget3_scale_and_arc_box_ext_draw_size_event_cb(lv_event_t * e);
static void temperature_arc_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void temperature_arc_changed_cb(lv_event_t * e);
static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_app_thermostat(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img,
                                &c->imgs[IMG_LIGHT_BG_THERMOSTAT]);

    lv_obj_t * bg_cont = lv_obj_create(bg);
    lv_obj_remove_style_all(bg_cont);
    lv_obj_set_size(bg_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);
    int32_t app_padding = c->sz == &lv_demo_high_res_sizes_all[SIZE_SM] ? c->sz->gap[9] : c->sz->gap[10];
    lv_obj_set_style_pad_bottom(bg_cont, app_padding, 0);
    lv_obj_set_style_pad_hor(bg_cont, app_padding, 0);

    /* top margin */

    lv_obj_t * top_margin = lv_demo_high_res_top_margin_create(bg_cont, 0, true, c);

    /* app info */

    lv_obj_t * app_info = lv_demo_high_res_simple_container_create(bg_cont, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_align_to(app_info, top_margin, LV_ALIGN_OUT_BOTTOM_LEFT, 0, c->sz->gap[7]);

    lv_obj_t * back = lv_demo_high_res_simple_container_create(app_info, false, c->sz->gap[2], LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(back, back_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * back_icon = lv_image_create(back);
    lv_image_set_src(back_icon, c->imgs[IMG_ARROW_LEFT]);
    lv_obj_add_style(back_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);
    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * back_label = lv_label_create(back);
    lv_label_set_text_static(back_label, "Back");
    lv_obj_set_style_text_opa(back_label, LV_OPA_60, 0);
    lv_obj_add_style(back_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(back_label, &c->fonts[FONT_HEADING_MD], 0);

    lv_obj_t * app_label = lv_label_create(app_info);
    lv_label_set_text_static(app_label, "Thermostat");
    lv_obj_add_style(app_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(app_label, &c->fonts[FONT_HEADING_LG], 0);

    /* widgets */

    int32_t widget_gap_padding = c->sz == &lv_demo_high_res_sizes_all[SIZE_SM] ? c->sz->gap[4] : c->sz->gap[7];
    lv_obj_t * widgets = lv_demo_high_res_simple_container_create(bg_cont, false, widget_gap_padding, LV_FLEX_ALIGN_END);
    lv_obj_set_align(widgets, LV_ALIGN_BOTTOM_RIGHT);

    create_widget1(c, widgets);
    create_widget2(c, widgets);
    create_widget3(c, widgets);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void back_clicked_cb(lv_event_t * e)
{
    lv_obj_t * back = lv_event_get_target_obj(e);

    lv_obj_t * base_obj = lv_obj_get_parent(lv_obj_get_parent(lv_obj_get_parent(back)));
    lv_obj_clean(base_obj);
    lv_demo_high_res_home(base_obj);
}

static lv_obj_t * widget1_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text_static(label, text);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(label, LV_OPA_40, 0);
    return label;
}

static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET1_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(widget, 18, 0);
    lv_obj_remove_flag(widget, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * title_label = lv_label_create(widget);
    lv_label_set_text_static(title_label, "Last 7 days");
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * chart_grid = lv_obj_create(widget);
    lv_obj_remove_style_all(chart_grid);
    lv_obj_set_size(chart_grid, LV_PCT(100), LV_SIZE_CONTENT);
    static const int32_t column_dsc[] = {LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static const int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(chart_grid, column_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(chart_grid, row_dsc, 0);
    lv_obj_set_layout(chart_grid, LV_LAYOUT_GRID);
    lv_obj_set_style_pad_column(chart_grid, c->sz->gap[3], 0);
    lv_obj_set_style_pad_row(chart_grid, 5, 0);

    lv_obj_t * chart = lv_chart_create(chart_grid);
    lv_obj_set_height(chart, c->sz->small_chart_height);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(chart, c->sz->gap[1], 0);
    lv_obj_set_style_radius(chart, c->sz->gap[3], LV_PART_ITEMS);
    lv_obj_set_style_border_width(chart, 2, 0);
    lv_obj_set_style_border_color(chart, lv_color_black(), 0);
    lv_obj_set_style_border_side(chart, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_post(chart, true, 0);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 109);
    lv_chart_set_div_line_count(chart, 3, 0);
    lv_obj_set_style_line_opa(chart, LV_OPA_40, 0);
    lv_obj_set_style_line_color(chart, lv_color_black(), 0);
    lv_obj_set_style_line_dash_width(chart, 2, 0);
    lv_obj_set_style_line_dash_gap(chart, 1, 0);
    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_color_white(), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t chart_values[] = {14, 95, 52, 34, 52, 83, 19};
    lv_chart_set_ext_y_array(chart, ser, (int32_t *)chart_values);
    lv_chart_set_point_count(chart, sizeof(chart_values) / sizeof(*chart_values));
    lv_chart_refresh(chart);

    lv_obj_t * hscale_label_1 = widget1_chart_label(c, chart_grid, "Aug 26");
    lv_obj_set_grid_cell(hscale_label_1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_t * hscale_label_2 = widget1_chart_label(c, chart_grid, "Sep 1");
    lv_obj_set_grid_cell(hscale_label_2, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_t * vscale_label_1 = widget1_chart_label(c, chart_grid, "20\nkWh");
    lv_obj_set_grid_cell(vscale_label_1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_t * vscale_label_2 = widget1_chart_label(c, chart_grid, "10");
    lv_obj_set_grid_cell(vscale_label_2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t * vscale_label_3 = widget1_chart_label(c, chart_grid, "0");
    lv_obj_set_grid_cell(vscale_label_3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_END, 0, 1);

    lv_obj_t * consumed_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * consumed_label = lv_label_create(consumed_box);
    lv_label_set_text_static(consumed_label, "Total energy consumed");
    lv_obj_add_style(consumed_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(consumed_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * amount_box = lv_demo_high_res_simple_container_create(consumed_box, false, c->sz->gap[1], LV_FLEX_ALIGN_END);

    lv_obj_t * amount_number = lv_label_create(amount_box);
    lv_label_set_text_static(amount_number, "102");
    lv_obj_add_style(amount_number, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(amount_number, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * amount_kwh = lv_label_create(amount_box);
    lv_label_set_text_static(amount_kwh, "kWh");
    lv_obj_add_style(amount_kwh, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(amount_kwh, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(amount_kwh, LV_OPA_40, 0);
}

static void widget2_slider_changed_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_current_target_obj(e);
    lv_obj_t * slider_box = lv_obj_get_parent(slider);
    lv_obj_t * label = lv_event_get_user_data(e);
    int32_t val = lv_slider_get_value(slider);

    if(slider == lv_obj_get_child(slider_box, 0)) {
        lv_label_set_text_fmt(label, "9pm - %"PRId32"am", val != 0 ? val : 12);
    }
    else {
        val = 6 - val + 6;
        lv_label_set_text_fmt(label, "%"PRId32"%s - 9pm", val, val < 12 ? "am" : "pm");
    }
}

static void widget2_temperature_button_clicked_cb(lv_event_t * e)
{
    lv_obj_t * button = lv_event_get_current_target_obj(e);
    lv_obj_t * box = lv_obj_get_parent(button);
    lv_obj_t * label = lv_obj_get_child(box, 1);
    int32_t label_val = (int32_t)(intptr_t)lv_obj_get_user_data(label);
    int32_t change = button == lv_obj_get_child(box, 0) ? -1 : 1;
    label_val += change;
    lv_label_set_text_fmt(label, "%"LV_PRId32, label_val);
    lv_obj_set_user_data(label, (void *)(intptr_t)label_val);
}

static lv_obj_t * create_widget2_value(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                       const char * range_val, int32_t temperature_val)
{
    lv_obj_t * box = lv_demo_high_res_simple_container_create(parent, true, c->sz->gap[2], LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(box, 1);

    lv_obj_t * title_label = lv_label_create(box);
    lv_label_set_text_static(title_label, title_val);
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * range_label = lv_label_create(box);
    lv_label_set_text_static(range_label, range_val);
    lv_obj_add_style(range_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(range_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(range_label, LV_OPA_40, 0);

    lv_obj_t * temperature_box = lv_demo_high_res_simple_container_create(box, false, c->sz->gap[2], LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(temperature_box, LV_PCT(100));

    lv_obj_t * minus = lv_image_create(temperature_box);
    lv_image_set_inner_align(minus, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_size(minus, c->sz->icon[ICON_MD], c->sz->icon[ICON_MD]);
    lv_obj_set_style_bg_color(minus, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(minus, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(minus, LV_COORD_MAX, 0);
    lv_obj_add_flag(minus, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(minus, widget2_temperature_button_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_image_set_src(minus, c->imgs[IMG_MINUS]);

    lv_obj_t * temperature_label = lv_label_create(temperature_box);
    lv_obj_set_flex_grow(temperature_label, 1);
    lv_obj_set_style_text_align(temperature_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text_fmt(temperature_label, "%"LV_PRId32, temperature_val);
    lv_obj_set_user_data(temperature_label, (void *)(intptr_t)temperature_val);
    lv_obj_add_style(temperature_label, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(temperature_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * plus = lv_image_create(temperature_box);
    lv_image_set_inner_align(plus, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_size(plus, c->sz->icon[ICON_MD], c->sz->icon[ICON_MD]);
    lv_obj_set_style_bg_color(plus, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(plus, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(plus, LV_COORD_MAX, 0);
    lv_obj_add_flag(plus, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(plus, widget2_temperature_button_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_image_set_src(plus, c->imgs[IMG_PLUS]);

    return box;
}

static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET1_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(widget, 0, 0);

    lv_obj_t * title_label = lv_label_create(widget);
    lv_label_set_text_static(title_label, "Scheduler");
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * outer_slider_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[4],
                                                                           LV_FLEX_ALIGN_START);
    lv_obj_set_width(outer_slider_box, LV_PCT(100));
    lv_obj_set_style_pad_top(outer_slider_box, c->sz->gap[5], 0);

    lv_obj_t * slider_box = lv_demo_high_res_simple_container_create(outer_slider_box, false, 0, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(slider_box, LV_PCT(100));
    lv_obj_set_style_radius(slider_box, c->sz->gap[4], 0);
    lv_obj_set_style_bg_color(slider_box, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider_box, LV_OPA_20, 0);

    lv_obj_t * slider_left = lv_slider_create(slider_box);
    lv_slider_set_range(slider_left, 0, 6);
    lv_slider_set_value(slider_left, 4, LV_ANIM_OFF);
    lv_obj_set_size(slider_left, LV_PCT(50), c->sz->slider_width);
    lv_obj_set_style_bg_opa(slider_left, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_color(slider_left, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider_left, c->sz->gap[4], 0);
    lv_obj_set_style_radius(slider_left, c->sz->gap[4], LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_left, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_right(slider_left, -(c->sz->slider_width - 6) / 2 - 8, LV_PART_KNOB);
    lv_obj_set_style_pad_left(slider_left, -(c->sz->slider_width - 6) / 2 + 8, LV_PART_KNOB);
    lv_obj_set_style_pad_ver(slider_left, -(c->sz->slider_width - 12) / 2, LV_PART_KNOB);

    lv_obj_t * slider_right = lv_slider_create(slider_box);
    lv_slider_set_range(slider_right, 6, 0);
    lv_slider_set_value(slider_right, 5, LV_ANIM_OFF);
    lv_obj_set_size(slider_right, LV_PCT(50), c->sz->slider_width);
    lv_obj_set_style_bg_opa(slider_right, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_color(slider_right, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider_right, c->sz->gap[4], 0);
    lv_obj_set_style_radius(slider_right, c->sz->gap[4], LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_right, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_right(slider_right, -(c->sz->slider_width - 6) / 2 + 8, LV_PART_KNOB);
    lv_obj_set_style_pad_left(slider_right, -(c->sz->slider_width - 6) / 2 - 8, LV_PART_KNOB);
    lv_obj_set_style_pad_ver(slider_right, -(c->sz->slider_width - 12) / 2, LV_PART_KNOB);

    lv_obj_t * slider_label_box = lv_obj_create(outer_slider_box);
    lv_obj_remove_style_all(slider_label_box);
    lv_obj_set_size(slider_label_box, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(slider_label_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(slider_label_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * slider_label_left = lv_label_create(slider_label_box);
    lv_label_set_text_static(slider_label_left, "12am");
    lv_obj_add_style(slider_label_left, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(slider_label_left, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(slider_label_left, LV_OPA_40, 0);

    lv_obj_t * slider_label_right = lv_label_create(slider_label_box);
    lv_label_set_text_static(slider_label_right, "12pm");
    lv_obj_add_style(slider_label_right, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(slider_label_right, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(slider_label_right, LV_OPA_40, 0);

    lv_obj_t * values_box = lv_obj_create(outer_slider_box);
    lv_obj_remove_style_all(values_box);
    lv_obj_set_size(values_box, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(values_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(values_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(values_box, c->sz->gap[7], 0);
    lv_obj_set_style_pad_column(values_box, c->sz->gap[5], 0);

    lv_obj_t * value1 = create_widget2_value(c, values_box, "Sleep", "9pm - 4am", 21);
    lv_obj_add_event_cb(slider_left, widget2_slider_changed_cb, LV_EVENT_VALUE_CHANGED, lv_obj_get_child(value1, 1));
    lv_obj_t * value2 = create_widget2_value(c, values_box, "Home", "7am - 9pm", 23);
    lv_obj_add_event_cb(slider_right, widget2_slider_changed_cb, LV_EVENT_VALUE_CHANGED, lv_obj_get_child(value2, 1));
}

static lv_obj_t * create_widget3_setting(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const lv_image_dsc_t * img_dsc,
                                         const char * text_val, bool active)
{
    lv_obj_t * setting = lv_obj_create(parent);
    lv_obj_remove_style_all(setting);
    lv_obj_set_height(setting, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(setting, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(setting, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(setting, c->sz->gap[5], 0);
    lv_obj_set_flex_grow(setting, 1);
    lv_obj_set_style_bg_color(setting, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(setting, active ? LV_OPA_60 : LV_OPA_20, 0);
    lv_obj_set_style_radius(setting, c->sz->gap[3], 0);
    lv_obj_set_style_pad_all(setting, c->sz->gap[5], 0);
    lv_obj_add_flag(setting, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * icon = lv_image_create(setting);
    lv_image_set_src(icon, img_dsc);
    lv_image_set_inner_align(icon, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_size(icon, c->sz->icon[ICON_XL], c->sz->icon[ICON_XL]);
    lv_obj_set_style_bg_color(icon, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(icon, active ? LV_OPA_COVER : LV_OPA_40, 0);
    lv_obj_set_style_radius(icon, LV_COORD_MAX, 0);
    lv_obj_set_style_shadow_width(icon, 50, 0);
    lv_obj_set_style_shadow_opa(icon, 15 * 255 / 100, 0);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * label = lv_label_create(setting);
    lv_label_set_text_static(label, text_val);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    return setting;
}

static void widget3_setting_clicked_cb(lv_event_t * e)
{
    lv_obj_t * setting = lv_event_get_target_obj(e);
    lv_obj_t * setting_box = lv_obj_get_parent(setting);
    lv_obj_t * setting1 = lv_obj_get_child(setting_box, 0);
    lv_obj_t * setting2 = lv_obj_get_child(setting_box, 1);
    lv_obj_t * setting3 = lv_obj_get_child(setting_box, 2);
    if(setting == setting1) {
        lv_obj_set_style_bg_opa(setting1, LV_OPA_60, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting1, 0), LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(setting2, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting2, 0), LV_OPA_40, 0);
        lv_obj_set_style_bg_opa(setting3, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting3, 0), LV_OPA_40, 0);
    }
    else if(setting == setting2) {
        lv_obj_set_style_bg_opa(setting1, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting1, 0), LV_OPA_40, 0);
        lv_obj_set_style_bg_opa(setting2, LV_OPA_60, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting2, 0), LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(setting3, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting3, 0), LV_OPA_40, 0);
    }
    else {
        lv_obj_set_style_bg_opa(setting1, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting1, 0), LV_OPA_40, 0);
        lv_obj_set_style_bg_opa(setting2, LV_OPA_20, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting2, 0), LV_OPA_40, 0);
        lv_obj_set_style_bg_opa(setting3, LV_OPA_60, 0);
        lv_obj_set_style_bg_opa(lv_obj_get_child(setting3, 0), LV_OPA_COVER, 0);
    }
}

static void widget3_scale_and_arc_box_ext_draw_size_event_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, 100);
}

static void temperature_arc_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * arc = lv_observer_get_target_obj(observer);
    lv_arc_set_value(arc, (lv_subject_get_int(subject) + 5) / 10);
}

static void temperature_arc_changed_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target_obj(e);
    lv_subject_t * temperature_subject = lv_event_get_user_data(e);

    int32_t arc_val = lv_arc_get_value(arc);
    lv_subject_set_int(temperature_subject, arc_val * 10);
}

static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->imgs[IMG_LIGHT_WIDGET5_BG]->header.w, c->imgs[IMG_LIGHT_WIDGET5_BG]->header.h);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET5_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(widget, c->sz->gap[5], 0);
    lv_obj_remove_flag(widget, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * title_label = lv_label_create(widget);
    lv_obj_set_width(title_label, LV_PCT(100));
    lv_label_set_text_static(title_label, "Temperature");
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * scale_and_arc_box = lv_obj_create(widget);
    lv_obj_remove_style_all(scale_and_arc_box);
    lv_obj_set_size(scale_and_arc_box, LV_PCT(100), 57 * c->sz->card_long_edge / 100);
    lv_obj_add_flag(scale_and_arc_box, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_add_event_cb(scale_and_arc_box, widget3_scale_and_arc_box_ext_draw_size_event_cb, LV_EVENT_REFR_EXT_DRAW_SIZE,
                        c);
    lv_obj_remove_flag(scale_and_arc_box, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * scale = lv_scale_create(scale_and_arc_box);
    lv_obj_set_align(scale, LV_ALIGN_TOP_MID);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_set_size(scale, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_obj_set_style_arc_opa(scale, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radial_offset(scale, 12, LV_PART_ITEMS);
    lv_obj_set_style_line_rounded(scale, true, LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_line_width(scale, 4, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 1, LV_PART_ITEMS);
    lv_obj_set_style_radial_offset(scale, 12, LV_PART_INDICATOR);
    lv_obj_set_style_line_rounded(scale, true, LV_PART_INDICATOR);
    lv_obj_set_style_line_color(scale, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_line_width(scale, 8, LV_PART_INDICATOR);
    lv_obj_set_style_length(scale, 1, LV_PART_INDICATOR);
    lv_scale_set_total_tick_count(scale, 16);
    lv_scale_set_angle_range(scale, 180);
    lv_scale_set_rotation(scale, 180);
    lv_scale_set_major_tick_every(scale, 5);
    lv_scale_set_label_show(scale, false);

    lv_obj_t * arc = lv_arc_create(scale_and_arc_box);
    lv_obj_set_align(arc, LV_ALIGN_TOP_MID);
    lv_obj_set_size(arc, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_arc_set_rotation(arc, 180);
    lv_arc_set_bg_angles(arc, 0, 180);
    lv_arc_set_range(arc, 15, 30);

    lv_obj_set_style_arc_rounded(arc, false, 0);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 9, 0);
    lv_obj_set_style_arc_width(arc, 9, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_white(), 0);
    lv_obj_set_style_arc_color(arc, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc, LV_OPA_20, 0);

    lv_obj_set_style_bg_color(arc, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_pad_all(arc, (32 - 9) / 2, LV_PART_KNOB);
    lv_obj_set_style_shadow_width(arc, 24, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(arc, 16 * 255 / 100, LV_PART_KNOB);

    lv_obj_t * current_temperature_box = lv_demo_high_res_simple_container_create(scale_and_arc_box, true, c->sz->gap[2],
                                                                                  LV_FLEX_ALIGN_CENTER);
    lv_obj_set_align(current_temperature_box, LV_ALIGN_BOTTOM_MID);

    lv_obj_t * current_temperature_val_label = lv_label_create(current_temperature_box);
    lv_obj_add_style(current_temperature_val_label, &c->fonts[FONT_LABEL_2XL], 0);
    lv_obj_add_style(current_temperature_val_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_subject_add_observer_obj(&c->api.subjects.thermostat_target_temperature, temperature_arc_observer_cb, arc, NULL);
    lv_obj_add_event_cb(arc, temperature_arc_changed_cb, LV_EVENT_VALUE_CHANGED,
                        &c->api.subjects.thermostat_target_temperature);
    lv_demo_high_res_label_bind_temperature(current_temperature_val_label, &c->api.subjects.thermostat_target_temperature,
                                            c);

    lv_obj_t * current_temperature_label = lv_label_create(current_temperature_box);
    lv_label_set_text_static(current_temperature_label, "Current\ntemperature");
    lv_obj_add_style(current_temperature_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(current_temperature_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_align(current_temperature_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t * setting_box = lv_demo_high_res_simple_container_create(widget, false, c->sz->gap[5], LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(setting_box, LV_PCT(100));
    lv_obj_t * setting1 = create_widget3_setting(c, setting_box, c->imgs[IMG_COLD_ICON], "Cold", true);
    lv_obj_t * setting2 = create_widget3_setting(c, setting_box, c->imgs[IMG_DRY_ICON], "Dry", false);
    lv_obj_t * setting3 = create_widget3_setting(c, setting_box, c->imgs[IMG_HEAT_ICON], "Heat", false);

    lv_obj_add_event_cb(setting1, widget3_setting_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(setting2, widget3_setting_clicked_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(setting3, widget3_setting_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * slider_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_set_width(slider_box, LV_PCT(100));
    lv_obj_set_style_pad_top(slider_box, 16, 0);

    lv_obj_t * fan_speed_label = lv_label_create(slider_box);
    lv_label_set_text_static(fan_speed_label, "Fan Speed");
    lv_obj_add_style(fan_speed_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(fan_speed_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * slider = lv_slider_create(slider_box);
    lv_obj_set_size(slider, LV_PCT(100), c->sz->slider_width);
    lv_obj_set_style_bg_color(slider, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider, 30, 0);
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, c->sz->gap[4], 0);
    lv_obj_set_style_radius(slider, c->sz->gap[4], LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_right(slider, -(c->sz->slider_width - 6) / 2 - 8, LV_PART_KNOB);
    lv_obj_set_style_pad_left(slider, -(c->sz->slider_width - 6) / 2 + 8, LV_PART_KNOB);
    lv_obj_set_style_pad_ver(slider, -(c->sz->slider_width - 12) / 2, LV_PART_KNOB);

    lv_obj_t * slider_pct_label = lv_label_create(slider);
    lv_obj_add_style(slider_pct_label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(slider_pct_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_align(slider_pct_label, LV_ALIGN_RIGHT_MID, -10, 0);

    lv_slider_bind_value(slider, &c->api.subjects.thermostat_fan_speed);
    lv_label_bind_text(slider_pct_label, &c->api.subjects.thermostat_fan_speed, "%"PRId32"%%");

    lv_label_set_text_static(slider_pct_label, "40%");
    lv_slider_set_value(slider, 40, LV_ANIM_OFF);

    lv_obj_t * slider_volume_img = lv_image_create(slider);
    lv_image_set_src(slider_volume_img, c->imgs[IMG_FAN]);
    lv_obj_align(slider_volume_img, LV_ALIGN_LEFT_MID, 8, 0);
}

#endif /*LV_USE_DEMO_HIGH_RES*/

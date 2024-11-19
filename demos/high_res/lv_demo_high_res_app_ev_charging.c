/**
 * @file lv_demo_high_res_app_ev_charging.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"
#include "../../src/widgets/span/lv_span_private.h"
#include "../../src/widgets/chart/lv_chart.h"
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
static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static lv_obj_t * widget2_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text);
static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void create_widget3_info(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const lv_image_dsc_t * img_dsc,
                                const char * text, const char * number, const char * unit);
static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void charging_status_label_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_app_ev_charging(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img, &c->imgs[IMG_LIGHT_BG_HOME]);

    lv_obj_t * bg_cont = lv_obj_create(bg);
    lv_obj_remove_style_all(bg_cont);
    lv_obj_set_size(bg_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);
    lv_obj_set_style_pad_bottom(bg_cont, c->sz->gap[10], 0);
    lv_obj_set_style_pad_left(bg_cont, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(bg_cont, c->sz->gap[10], 0);

    /* top margin */

    lv_obj_t * top_margin = lv_demo_high_res_top_margin_create(base_obj, bg_cont, 0, true, c);

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
    lv_label_set_text_static(app_label, "EV Charging");
    lv_obj_add_style(app_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(app_label, &c->fonts[FONT_HEADING_LG], 0);

    /* widgets */

    lv_obj_t * widgets = lv_demo_high_res_simple_container_create(bg_cont, false, c->sz->gap[7], LV_FLEX_ALIGN_END);
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

static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET1_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * title_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[1], LV_FLEX_ALIGN_START);

    lv_obj_t * gas_savings_label = lv_label_create(title_box);
    lv_label_set_text_static(gas_savings_label, "Gas Savings");
    lv_obj_add_style(gas_savings_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(gas_savings_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_t * estimated_values_label = lv_label_create(title_box);
    lv_label_set_text_static(estimated_values_label, "Estimated values");
    lv_obj_add_style(estimated_values_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(estimated_values_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(estimated_values_label, LV_OPA_40, 0);

    lv_obj_t * saved_box = lv_obj_create(widget);
    lv_obj_remove_style_all(saved_box);
    lv_obj_set_width(saved_box, LV_PCT(100));
    lv_obj_set_flex_grow(saved_box, 1);
    lv_obj_set_flex_flow(saved_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(saved_box, c->sz->gap[2], 0);
    lv_obj_set_flex_align(saved_box, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t * saved_amount_label = lv_label_create(saved_box);
    lv_label_set_text_static(saved_amount_label, "$212");
    lv_obj_add_style(saved_amount_label, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(saved_amount_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_t * saved_label = lv_label_create(saved_box);
    lv_label_set_text_static(saved_label, "Saved");
    lv_obj_add_style(saved_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(saved_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * spent_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * total_spent_span = lv_spangroup_create(spent_box);
    lv_obj_add_style(total_spent_span, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(total_spent_span, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_span_t * total_spent_amount = lv_spangroup_new_span(total_spent_span);
    lv_span_set_text_static(total_spent_amount, "$128 - ");
    lv_span_t * total_spent_label = lv_spangroup_new_span(total_spent_span);
    lv_span_set_text_static(total_spent_label, "Total spent");
    lv_style_set_text_opa(&total_spent_label->style, LV_OPA_40);

    lv_obj_t * total_spent_bar = lv_demo_high_res_simple_container_create(spent_box, false, c->sz->gap[1],
                                                                          LV_FLEX_ALIGN_START);
    for(int32_t i = 0; i < 11; i++) {
        lv_obj_t * bar = lv_obj_create(total_spent_bar);
        lv_obj_set_style_border_width(bar, 0, 0);
        lv_obj_set_size(bar, c->sz->gap[2], c->sz->gap[8]);
        lv_obj_set_style_radius(bar, LV_COORD_MAX, 0);
        lv_obj_set_style_opa(bar, LV_OPA_TRANSP, LV_PART_SCROLLBAR);
    }

    lv_obj_t * gas_equivalent_bar = lv_demo_high_res_simple_container_create(spent_box, false, c->sz->gap[1],
                                                                             LV_FLEX_ALIGN_START);
    for(int32_t i = 0; i < 26; i++) {
        lv_obj_t * bar = lv_obj_create(gas_equivalent_bar);
        lv_obj_set_style_border_width(bar, 0, 0);
        lv_obj_set_size(bar, c->sz->gap[2], c->sz->gap[8]);
        lv_obj_set_style_radius(bar, LV_COORD_MAX, 0);
        lv_obj_set_style_opa(bar, LV_OPA_30, 0);
        lv_obj_set_style_opa(bar, LV_OPA_TRANSP, LV_PART_SCROLLBAR);
    }

    lv_obj_t * gas_equivalent_span = lv_spangroup_create(spent_box);
    lv_obj_add_style(gas_equivalent_span, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(gas_equivalent_span, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_span_t * gas_equivalent_amount = lv_spangroup_new_span(gas_equivalent_span);
    lv_span_set_text_static(gas_equivalent_amount, "$340 - ");
    lv_span_t * gas_equivalent_label = lv_spangroup_new_span(gas_equivalent_span);
    lv_span_set_text_static(gas_equivalent_label, "Gas Equivalent");
    lv_style_set_text_opa(&gas_equivalent_label->style, LV_OPA_40);
}

static lv_obj_t * widget2_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text_static(label, text);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(label, LV_OPA_40, 0);
    return label;
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
    lv_obj_set_style_pad_row(widget, 18, 0);

    lv_obj_t * title_label = lv_label_create(widget);
    lv_label_set_text_static(title_label, "Last 14 days");
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
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 96);
    lv_chart_set_div_line_count(chart, 3, 0);
    lv_obj_set_style_line_opa(chart, LV_OPA_40, 0);
    lv_obj_set_style_line_color(chart, lv_color_black(), 0);
    lv_obj_set_style_line_dash_width(chart, 2, 0);
    lv_obj_set_style_line_dash_gap(chart, 1, 0);
    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_color_white(), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t chart_values[] = {45, 14, 78, 95, 45, 52, 78, 34, 63, 52, 38, 83, 63, 19, LV_CHART_POINT_NONE};
    lv_chart_set_ext_y_array(chart, ser, (int32_t *)chart_values);
    lv_chart_set_point_count(chart, sizeof(chart_values) / sizeof(*chart_values));
    lv_chart_refresh(chart);

    lv_obj_t * hscale_label_1 = widget2_chart_label(c, chart_grid, "Aug 2");
    lv_obj_set_grid_cell(hscale_label_1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_t * hscale_label_2 = widget2_chart_label(c, chart_grid, "Sep 1");
    lv_obj_set_grid_cell(hscale_label_2, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_t * vscale_label_1 = widget2_chart_label(c, chart_grid, "95\nkWh");
    lv_obj_set_grid_cell(vscale_label_1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_t * vscale_label_2 = widget2_chart_label(c, chart_grid, "48");
    lv_obj_set_grid_cell(vscale_label_2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t * vscale_label_3 = widget2_chart_label(c, chart_grid, "0");
    lv_obj_set_grid_cell(vscale_label_3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_END, 0, 1);

    lv_obj_t * totals_box = lv_obj_create(widget);
    lv_obj_remove_style_all(totals_box);
    lv_obj_set_size(totals_box, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(totals_box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_grow(totals_box, 1);
    lv_obj_set_flex_align(totals_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    lv_obj_t * charged_box = lv_demo_high_res_simple_container_create(totals_box, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * charged_label = lv_label_create(charged_box);
    lv_label_set_text_static(charged_label, "Total Charged");
    lv_obj_add_style(charged_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(charged_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * charged_amount_box = lv_demo_high_res_simple_container_create(charged_box, false, c->sz->gap[1],
                                                                             LV_FLEX_ALIGN_END);

    lv_obj_t * charged_amount_number = lv_label_create(charged_amount_box);
    lv_label_set_text_static(charged_amount_number, "683");
    lv_obj_add_style(charged_amount_number, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(charged_amount_number, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * charged_amount_kwh = lv_label_create(charged_amount_box);
    lv_label_set_text_static(charged_amount_kwh, "kWh");
    lv_obj_add_style(charged_amount_kwh, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(charged_amount_kwh, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(charged_amount_kwh, LV_OPA_40, 0);

    lv_obj_t * spent_box = lv_demo_high_res_simple_container_create(totals_box, true, c->sz->gap[2], LV_FLEX_ALIGN_START);
    lv_obj_set_width(spent_box, LV_PCT(45));

    lv_obj_t * spent_label = lv_label_create(spent_box);
    lv_label_set_text_static(spent_label, "Total Spent");
    lv_obj_add_style(spent_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(spent_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * spent_amount_number = lv_label_create(spent_box);
    lv_label_set_text_static(spent_amount_number, "$128");
    lv_obj_add_style(spent_amount_number, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(spent_amount_number, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
}

static void create_widget3_info(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const lv_image_dsc_t * img_dsc,
                                const char * text, const char * number, const char * unit)
{
    lv_obj_t * info = lv_obj_create(parent);
    lv_obj_remove_style_all(info);
    lv_obj_set_height(info, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(info, c->sz->gap[4], 0);
    lv_obj_set_flex_grow(info, 1);
    lv_obj_set_style_bg_color(info, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(info, 16 * 255 / 100, 0);
    lv_obj_set_style_radius(info, c->sz->gap[3], 0);
    lv_obj_set_style_pad_all(info, c->sz->gap[5], 0);

    lv_obj_t * sub_box1 = lv_demo_high_res_simple_container_create(info, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * img = lv_image_create(sub_box1);
    lv_image_set_src(img, img_dsc);
    lv_obj_set_style_image_recolor_opa(img, LV_OPA_COVER, 0);
    lv_obj_set_style_image_recolor(img, lv_color_white(), 0);

    lv_obj_t * label = lv_label_create(sub_box1);
    lv_label_set_text_static(label, text);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);

    lv_obj_t * sub_box2 = lv_demo_high_res_simple_container_create(info, false, c->sz->gap[1], LV_FLEX_ALIGN_END);

    lv_obj_t * label_number = lv_label_create(sub_box2);
    lv_label_set_text_static(label_number, number);
    lv_obj_add_style(label_number, &c->fonts[FONT_LABEL_LG], 0);
    lv_obj_set_style_text_color(label_number, lv_color_white(), 0);

    lv_obj_t * label_unit = lv_label_create(sub_box2);
    lv_label_set_text_static(label_unit, unit);
    lv_obj_add_style(label_unit, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(label_unit, lv_color_white(), 0);
    lv_obj_set_style_text_opa(label_unit, LV_OPA_60, 0);
}

static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->imgs[IMG_LIGHT_WIDGET5_BG]->header.w, c->imgs[IMG_LIGHT_WIDGET5_BG]->header.h);
    lv_obj_set_style_bg_image_src(widget, c->imgs[IMG_EV_CHARGING_WIDGET3_BG], 0);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Charging");
    lv_obj_set_width(top_label, LV_PCT(100));
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_set_style_text_color(top_label, lv_color_white(), 0);

    lv_obj_t * arc = lv_arc_create(widget);
    lv_obj_set_size(arc, c->sz->ev_charging_arc_diameter, c->sz->ev_charging_arc_diameter);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_rounded(arc, false, 0);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 7, 0);
    lv_obj_set_style_arc_width(arc, 7, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_white(), 0);
    lv_obj_set_style_arc_color(arc, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc, LV_OPA_20, 0);
    lv_obj_set_flex_flow(arc, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(arc, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(arc, c->sz->gap[2], 0);
    lv_arc_bind_value(arc, &c->subjects.ev_charge_percent);

    lv_obj_t * percent_label = lv_label_create(arc);
    lv_obj_add_style(percent_label, &c->fonts[FONT_LABEL_2XL], 0);
    lv_obj_set_style_text_color(percent_label, lv_color_white(), 0);
    lv_label_bind_text(percent_label, &c->subjects.ev_charge_percent, "%d%%");

    lv_obj_t * charging_status_box = lv_obj_create(arc);
    lv_obj_remove_style_all(charging_status_box);
    lv_obj_set_size(charging_status_box, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_hor(charging_status_box, c->sz->gap[6], 0);
    lv_obj_set_style_pad_ver(charging_status_box, c->sz->gap[3], 0);
    lv_obj_set_style_bg_color(charging_status_box, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(charging_status_box, LV_OPA_30, 0);
    lv_obj_set_style_radius(charging_status_box, LV_COORD_MAX, 0);
    lv_obj_add_flag(charging_status_box, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(charging_status_box, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_bind_checked(charging_status_box, &c->subjects.ev_is_charging);

    lv_obj_t * charging_status_label = lv_label_create(charging_status_box);
    lv_obj_add_style(charging_status_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(charging_status_label, lv_color_white(), 0);
    lv_obj_center(charging_status_label);
    lv_subject_add_observer_obj(&c->subjects.ev_is_charging, charging_status_label_cb, charging_status_label, NULL);

    lv_obj_t * info_box = lv_demo_high_res_simple_container_create(widget, false, c->sz->gap[5], LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(info_box, LV_PCT(100));
    create_widget3_info(c, info_box, c->imgs[IMG_TIME_ICON], "Time to\nfull charge", "1.2", "h");
    create_widget3_info(c, info_box, c->imgs[IMG_ENERGY_ICON], "Energy\nconsumed", "10.12", "kW");
    create_widget3_info(c, info_box, c->imgs[IMG_RANGE_ICON], "Driving\nRange", "209", "km");
}

static void charging_status_label_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * charging_status_label = lv_observer_get_target_obj(observer);
    bool is_charging = lv_subject_get_int(subject);
    lv_label_set_text_static(charging_status_label, is_charging ? "Stop charging" : "Start charging");
}

#endif /*LV_USE_DEMO_HIGH_RES*/

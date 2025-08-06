/**
 * @file lv_demo_widgets_analytics.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets_analytics.h"
#if LV_USE_DEMO_WIDGETS

#include "lv_demo_widgets_components.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void scale1_indic1_anim_cb(void * var, int32_t v);
static void scale2_timer_cb(lv_timer_t * timer);
static void scale3_anim_cb(void * var, int32_t v);
static void scale3_size_changed_event_cb(lv_event_t * e);
static void delete_timer_event_cb(lv_event_t * e);
static void scale3_delete_event_cb(lv_event_t * e);
static lv_obj_t * create_scale_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3);
static lv_obj_t * create_chart_with_scales(lv_obj_t * parent, const char * title,  const char * hor_text[]);

static void chart_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * scale1;
static lv_obj_t * scale2;
static lv_obj_t * scale3;

lv_obj_t * chart1;
lv_obj_t * chart2;

lv_chart_series_t * ser1;
lv_chart_series_t * ser2;
lv_chart_series_t * ser3;

static uint32_t session_desktop = 1000;
static uint32_t session_tablet = 1000;
static uint32_t session_mobile = 1000;

static lv_style_t scale3_section1_main_style;
static lv_style_t scale3_section1_indicator_style;
static lv_style_t scale3_section1_tick_style;
static lv_style_t scale3_section2_main_style;
static lv_style_t scale3_section2_indicator_style;
static lv_style_t scale3_section2_tick_style;
static lv_style_t scale3_section3_main_style;
static lv_style_t scale3_section3_indicator_style;
static lv_style_t scale3_section3_tick_style;

static lv_obj_t * scale3_needle;
static lv_obj_t * scale3_mbps_label;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_widgets_analytics_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW);

    lv_obj_t * chart1_cont = lv_obj_create(parent);
    lv_obj_set_height(chart1_cont, lv_pct(100));
    lv_obj_set_style_max_height(chart1_cont, 300, 0);
    lv_obj_set_flex_grow(chart1_cont, 1);

    static const char * chart1_texts[] = {"Jan", "Feb", "March", "April", "May", "Jun", "July", "Aug", "Sept", "Oct", "Nov", "Dec", NULL};
    chart1 = create_chart_with_scales(chart1_cont, "Unique visitors", chart1_texts);

    ser1 = lv_chart_add_series(chart1, lv_theme_get_color_primary(chart1), LV_CHART_AXIS_PRIMARY_Y);
    uint32_t i;
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart1, ser1, lv_rand(10, 80));
    }

    lv_obj_t * chart2_cont = lv_obj_create(parent);
    lv_obj_set_height(chart2_cont, lv_pct(100));
    lv_obj_set_style_max_height(chart2_cont, 300, 0);
    lv_obj_set_flex_grow(chart2_cont, 1);
    lv_obj_add_flag(chart2_cont, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    static const char * chart2_texts[] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII", NULL};
    chart2 = create_chart_with_scales(chart2_cont, "Monthly revenue", chart2_texts);
    lv_chart_set_type(chart2, LV_CHART_TYPE_BAR);

    ser2 = lv_chart_add_series(chart2, lv_palette_main(LV_PALETTE_GREY), LV_CHART_AXIS_PRIMARY_Y);
    ser3 = lv_chart_add_series(chart2, lv_theme_get_color_primary(chart2), LV_CHART_AXIS_PRIMARY_Y);
    for(i = 0; i < 12; i++) {
        lv_chart_set_next_value(chart2, ser2, lv_rand(10, 80));
        lv_chart_set_next_value(chart2, ser3, lv_rand(10, 80));
    }
    lv_obj_t * chart2_hor_scale = lv_obj_get_sibling(chart2, 1);
    lv_obj_set_style_pad_hor(chart2_hor_scale, lv_chart_get_first_point_center_offset(chart2), 0);

    /*Create all 3 scales first to have their size resolved*/
    scale1 = create_scale_box(parent, "Monthly Target", "Revenue: -", "Sales: -", "Costs: -");
    lv_obj_add_flag(lv_obj_get_parent(scale1), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    scale2 = create_scale_box(parent, "Sessions", "Desktop: -", "Tablet: -", "Mobile: -");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(scale2), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    scale3 = create_scale_box(parent, "Network Speed", "Low speed", "Normal Speed", "High Speed");
    if(disp_size < DISP_LARGE) lv_obj_add_flag(lv_obj_get_parent(scale3), LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    lv_obj_update_layout(parent);
    int32_t scale_w;
    if(disp_size == DISP_MEDIUM) {
        scale_w = 200;
        lv_obj_set_size(scale1, scale_w, scale_w);
        lv_obj_set_size(scale2, scale_w, scale_w);
        lv_obj_set_size(scale3, scale_w, scale_w);
    }
    else {
        scale_w = lv_obj_get_width(scale1);
        lv_obj_set_height(scale1, scale_w);
        lv_obj_set_height(scale2, scale_w);
        lv_obj_set_height(scale3, scale_w);
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_values(&a, 20, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    /*Scale 1*/
    lv_scale_set_mode(scale1, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_set_style_pad_all(scale1, 30, 0);
    lv_obj_t * arc;
    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_style(arc, NULL, LV_PART_MAIN);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 4100);
    lv_anim_set_reverse_duration(&a, 2700);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 20, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2600);
    lv_anim_set_reverse_duration(&a, 3200);
    lv_anim_start(&a);

    arc = lv_arc_create(scale1);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 40, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_arc_width(arc, 15, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_anim_set_exec_cb(&a, scale1_indic1_anim_cb);
    lv_anim_set_var(&a, arc);
    lv_anim_set_duration(&a, 2800);
    lv_anim_set_reverse_duration(&a, 1800);
    lv_anim_start(&a);

    /*Scale 2*/
    static const char * scale2_text[] = {"0", "10", "20", "30", "40", "50", "60", "70", "80", "90", NULL};
    lv_scale_set_angle_range(scale2, 360);
    lv_scale_set_text_src(scale2, scale2_text);
    lv_scale_set_total_tick_count(scale2, 11);
    lv_obj_set_style_length(scale2, 30, LV_PART_INDICATOR);
    lv_scale_set_major_tick_every(scale2, 1);
    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 10, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 10, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_margin_all(arc, 5, 0);
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 20, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_RED), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    arc = lv_arc_create(scale2);
    lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
    lv_obj_set_style_arc_opa(arc, 0, 0);
    lv_obj_set_style_bg_opa(arc, 0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, 0, LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 30, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arc);

    lv_timer_t * scale2_timer = lv_timer_create(scale2_timer_cb, 100, scale2);
    lv_obj_add_event_cb(scale2, delete_timer_event_cb, LV_EVENT_DELETE, scale2_timer);

    /*Scale 3*/
    lv_scale_set_range(scale3, 10, 60);
    lv_scale_set_total_tick_count(scale3, 21);
    lv_scale_set_major_tick_every(scale3, 4);
    lv_obj_set_style_length(scale3, 10, LV_PART_ITEMS);
    lv_obj_set_style_length(scale3, 20, LV_PART_INDICATOR);
    lv_scale_set_angle_range(scale3, 225);
    lv_scale_set_rotation(scale3, 135);

    lv_style_init(&scale3_section1_main_style);
    lv_style_set_arc_width(&scale3_section1_main_style, 8);
    lv_style_set_arc_color(&scale3_section1_main_style, lv_palette_main(LV_PALETTE_RED));

    lv_style_init(&scale3_section1_indicator_style);
    lv_style_set_line_width(&scale3_section1_indicator_style, 4);
    lv_style_set_line_color(&scale3_section1_indicator_style, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_init(&scale3_section1_tick_style);
    lv_style_set_line_width(&scale3_section1_tick_style, 4);
    lv_style_set_line_color(&scale3_section1_tick_style, lv_palette_darken(LV_PALETTE_RED, 2));

    lv_style_init(&scale3_section2_main_style);
    lv_style_set_arc_width(&scale3_section2_main_style, 8);
    lv_style_set_arc_color(&scale3_section2_main_style, lv_palette_main(LV_PALETTE_BLUE));

    lv_style_init(&scale3_section2_indicator_style);
    lv_style_set_line_width(&scale3_section2_indicator_style, 4);
    lv_style_set_line_color(&scale3_section2_indicator_style, lv_palette_darken(LV_PALETTE_BLUE, 2));

    lv_style_init(&scale3_section2_tick_style);
    lv_style_set_line_width(&scale3_section2_tick_style, 4);
    lv_style_set_line_color(&scale3_section2_tick_style, lv_palette_darken(LV_PALETTE_BLUE, 2));

    lv_style_init(&scale3_section3_main_style);
    lv_style_set_arc_width(&scale3_section3_main_style, 8);
    lv_style_set_arc_color(&scale3_section3_main_style, lv_palette_main(LV_PALETTE_GREEN));

    lv_style_init(&scale3_section3_indicator_style);
    lv_style_set_line_width(&scale3_section3_indicator_style, 4);
    lv_style_set_line_color(&scale3_section3_indicator_style, lv_palette_darken(LV_PALETTE_GREEN, 2));

    lv_style_init(&scale3_section3_tick_style);
    lv_style_set_line_width(&scale3_section3_tick_style, 4);
    lv_style_set_line_color(&scale3_section3_tick_style, lv_palette_darken(LV_PALETTE_GREEN, 2));

    lv_obj_add_event_cb(scale3, scale3_delete_event_cb, LV_EVENT_DELETE, NULL);

    lv_scale_section_t * section;
    section = lv_scale_add_section(scale3);
    lv_scale_set_section_range(scale3, section, 0, 20);
    lv_scale_set_section_style_main(scale3, section, &scale3_section2_main_style);
    lv_scale_set_section_style_indicator(scale3, section, &scale3_section2_indicator_style);
    lv_scale_set_section_style_items(scale3, section, &scale3_section2_tick_style);

    section = lv_scale_add_section(scale3);
    lv_scale_set_section_range(scale3, section, 40, 60);
    lv_scale_set_section_style_main(scale3, section, &scale3_section3_main_style);
    lv_scale_set_section_style_indicator(scale3, section, &scale3_section3_indicator_style);
    lv_scale_set_section_style_items(scale3, section, &scale3_section3_tick_style);

    LV_IMAGE_DECLARE(img_demo_widgets_needle);
    scale3_needle = lv_image_create(scale3);
    lv_image_set_src(scale3_needle, &img_demo_widgets_needle);
    lv_image_set_pivot(scale3_needle, 3, 4);
    lv_obj_align(scale3_needle, LV_ALIGN_CENTER, 47, -2);

    scale3_mbps_label = lv_demo_widgets_title_create(scale3, "-");

    lv_obj_t * mbps_unit_label = lv_label_create(scale3);
    lv_label_set_text(mbps_unit_label, "Mbps");

    lv_anim_init(&a);
    lv_anim_set_values(&a, 10, 60);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, scale3_anim_cb);
    lv_anim_set_var(&a, scale3);
    lv_anim_set_duration(&a, 4100);
    lv_anim_set_reverse_duration(&a, 800);
    lv_anim_start(&a);

    lv_obj_align(scale3_mbps_label, LV_ALIGN_TOP_MID, 10, lv_pct(55));
    lv_obj_align_to(mbps_unit_label, scale3_mbps_label, LV_ALIGN_OUT_RIGHT_BOTTOM, 10, 0);

    lv_obj_add_event_cb(scale3, scale3_size_changed_event_cb, LV_EVENT_SIZE_CHANGED, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void scale3_delete_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DELETE) {
        lv_style_reset(&scale3_section1_main_style);
        lv_style_reset(&scale3_section1_indicator_style);
        lv_style_reset(&scale3_section1_tick_style);
        lv_style_reset(&scale3_section2_main_style);
        lv_style_reset(&scale3_section2_indicator_style);
        lv_style_reset(&scale3_section2_tick_style);
        lv_style_reset(&scale3_section3_main_style);
        lv_style_reset(&scale3_section3_indicator_style);
        lv_style_reset(&scale3_section3_tick_style);
    }
}

static void scale1_indic1_anim_cb(void * var, int32_t v)
{
    lv_arc_set_value(var, v);

    lv_obj_t * card = lv_obj_get_parent(scale1);
    lv_obj_t * label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Revenue: %"LV_PRId32" %%", v);
}

static void scale2_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    static bool down1 = false;
    static bool down2 = false;
    static bool down3 = false;

    if(down1) {
        session_desktop -= 137;
        if(session_desktop < 1400) down1 = false;
    }
    else {
        session_desktop += 116;
        if(session_desktop > 4500) down1 = true;
    }

    if(down2) {
        session_tablet -= 3;
        if(session_tablet < 1400) down2 = false;
    }
    else {
        session_tablet += 9;
        if(session_tablet > 4500) down2 = true;
    }

    if(down3) {
        session_mobile -= 57;
        if(session_mobile < 1400) down3 = false;
    }
    else {
        session_mobile += 76;
        if(session_mobile > 4500) down3 = true;
    }

    uint32_t all = session_desktop + session_tablet + session_mobile;
    uint32_t angle1 = (session_desktop * 354) / all;
    uint32_t angle2 = (session_tablet * 354) / all;

    lv_arc_set_angles(lv_obj_get_child(scale2, 0), 0, angle1);
    lv_arc_set_angles(lv_obj_get_child(scale2, 1), angle1 + 2, angle1 + 2 + angle2);
    lv_arc_set_angles(lv_obj_get_child(scale2, 2), angle1 + 2 + angle2 + 2, 358);

    lv_obj_t * card = lv_obj_get_parent(scale2);
    lv_obj_t * label;

    label = lv_obj_get_child(card, -5);
    lv_label_set_text_fmt(label, "Desktop: %"LV_PRIu32, session_desktop);

    label = lv_obj_get_child(card, -3);
    lv_label_set_text_fmt(label, "Tablet: %"LV_PRIu32, session_tablet);

    label = lv_obj_get_child(card, -1);
    lv_label_set_text_fmt(label, "Mobile: %"LV_PRIu32, session_mobile);
}

static void scale3_anim_cb(void * var, int32_t v)
{
    lv_obj_t * needle = lv_obj_get_child(var, 0);
    lv_scale_set_image_needle_value(var, needle, v);

    lv_obj_t * label = lv_obj_get_child(var, 1);
    lv_label_set_text_fmt(label, "%"LV_PRId32, v);
}

static void scale3_size_changed_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);

    /* the center of the scale is half of the smaller dimension */
    int32_t width = lv_obj_get_width(scale3);
    int32_t height = lv_obj_get_height(scale3);
    int32_t minor_dim = LV_MIN(width, height);
    int32_t minor_dim_half = minor_dim / 2;

    /* Update needle position */
    lv_obj_align(scale3_needle, LV_ALIGN_TOP_LEFT, minor_dim_half, minor_dim_half);

    /* Update labels position */
    lv_obj_align(scale3_mbps_label, LV_ALIGN_TOP_LEFT, minor_dim_half, minor_dim * 55 / 100);
}

static void delete_timer_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_DELETE) {
        lv_timer_t * t = lv_event_get_user_data(e);
        if(t) lv_timer_delete(t);
    }
}

static lv_obj_t * create_scale_box(lv_obj_t * parent, const char * title, const char * text1, const char * text2,
                                   const char * text3)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(cont, 1);

    lv_obj_t * title_label = lv_demo_widgets_title_create(cont, title);

    lv_obj_t * scale = lv_scale_create(cont);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_post_draw(scale, true);
    lv_obj_set_width(scale, LV_PCT(100));

    lv_obj_t * bullet1 = lv_obj_create(cont);
    lv_obj_set_size(bullet1, 13, 13);
    lv_obj_remove_style(bullet1, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet1, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet1, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_t * label1 = lv_label_create(cont);
    lv_label_set_text_static(label1, text1);

    lv_obj_t * bullet2 = lv_obj_create(cont);
    lv_obj_set_size(bullet2, 13, 13);
    lv_obj_remove_style(bullet2, NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet2, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet2, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_t * label2 = lv_label_create(cont);
    lv_label_set_text_static(label2, text2);

    lv_obj_t * bullet3 = lv_obj_create(cont);
    lv_obj_set_size(bullet3, 13, 13);
    lv_obj_remove_style(bullet3,  NULL, LV_PART_SCROLLBAR);
    lv_obj_add_style(bullet3, &style_bullet, 0);
    lv_obj_set_style_bg_color(bullet3, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_t * label3 = lv_label_create(cont);
    lv_label_set_text_static(label3, text3);

    if(disp_size == DISP_MEDIUM) {
        static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_FR(8), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 4, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(scale, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 3);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    }
    else {
        static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);
        lv_obj_set_grid_cell(title_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(scale, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(bullet1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(bullet2, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(bullet3, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(label1, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(label3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 4, 1);
    }
    return scale;
}

static lv_obj_t * create_chart_with_scales(lv_obj_t * parent, const char * title,  const char * hor_text[])
{
    static const int32_t col_dsc[] = {40, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), 40, LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(parent, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(parent, 0, 0);
    lv_obj_set_style_pad_row(parent, 0, 0);

    lv_obj_t * label = lv_demo_widgets_title_create(parent, title);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    lv_obj_t * scale_ver = lv_scale_create(parent);
    lv_scale_set_mode(scale_ver, LV_SCALE_MODE_VERTICAL_LEFT);
    lv_obj_set_grid_cell(scale_ver, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_scale_set_total_tick_count(scale_ver, 11);
    lv_scale_set_major_tick_every(scale_ver, 2);
    lv_scale_set_range(scale_ver, 0, 100);

    lv_obj_t * wrapper = lv_obj_create(parent);
    lv_obj_remove_style(wrapper, NULL, LV_PART_MAIN);
    lv_obj_set_grid_dsc_array(wrapper, NULL, NULL);
    lv_obj_set_grid_cell(wrapper, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 2);
    lv_obj_set_scroll_dir(wrapper, LV_DIR_HOR);

    lv_obj_t * chart = lv_chart_create(wrapper);
    lv_group_add_obj(lv_group_get_default(), chart);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SCROLL_ON_FOCUS | LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_chart_set_div_line_count(chart, 0, 12);
    lv_chart_set_point_count(chart, 12);
    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_border_width(chart, 0, 0);
    lv_obj_add_event_cb(chart, chart_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_width(chart, lv_pct(200));
    lv_obj_set_style_radius(chart, 0, 0);

    lv_obj_t * scale_hor = lv_scale_create(wrapper);
    lv_scale_set_mode(scale_hor, LV_SCALE_MODE_HORIZONTAL_BOTTOM);
    lv_obj_set_grid_cell(scale_hor, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_scale_set_total_tick_count(scale_hor, 12);
    lv_scale_set_major_tick_every(scale_hor, 1);
    lv_scale_set_text_src(scale_hor, hor_text);
    lv_obj_set_width(scale_hor, lv_pct(200));
    lv_obj_set_height(scale_hor, 40);
    lv_obj_set_style_pad_hor(scale_hor, lv_obj_get_style_pad_left(chart, LV_PART_MAIN), 0);
    lv_obj_set_style_pad_ver(scale_ver, lv_obj_get_style_pad_top(chart, LV_PART_MAIN), 0);
    return chart;
}


static void chart_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_PRESSED || code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(obj); /*To make the value boxes visible*/
    }
    else if(code == LV_EVENT_DRAW_TASK_ADDED) {
        lv_draw_task_t * draw_task = lv_event_get_param(e);
        lv_draw_dsc_base_t * base_dsc = lv_draw_task_get_draw_dsc(draw_task);

        lv_draw_line_dsc_t * draw_line_dsc = lv_draw_task_get_line_dsc(draw_task);
        if(base_dsc->part == LV_PART_ITEMS && draw_line_dsc) {
            lv_area_t obj_coords;
            lv_obj_get_coords(obj, &obj_coords);
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) ser = lv_chart_get_series_next(obj, ser);

            lv_draw_triangle_dsc_t tri_dsc;
            lv_draw_triangle_dsc_init(&tri_dsc);
            tri_dsc.p[0].x = (int32_t)draw_line_dsc->p1.x;
            tri_dsc.p[0].y = (int32_t)draw_line_dsc->p1.y;
            tri_dsc.p[1].x = (int32_t)draw_line_dsc->p2.x;
            tri_dsc.p[1].y = (int32_t)draw_line_dsc->p2.y;
            tri_dsc.p[2].x = (int32_t)(draw_line_dsc->p1.y < draw_line_dsc->p2.y ? draw_line_dsc->p1.x : draw_line_dsc->p2.x);
            tri_dsc.p[2].y = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
            tri_dsc.grad.dir = LV_GRAD_DIR_VER;

            int32_t full_h = lv_obj_get_height(obj);
            int32_t fract_uppter = (int32_t)(LV_MIN(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj_coords.y1) * 255 / full_h;
            int32_t fract_lower = (int32_t)(LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj_coords.y1) * 255 / full_h;
            tri_dsc.grad.stops[0].color = lv_chart_get_series_color(obj, ser);
            tri_dsc.grad.stops[0].opa = 255 - fract_uppter;
            tri_dsc.grad.stops[0].frac = 0;
            tri_dsc.grad.stops[1].color = lv_chart_get_series_color(obj, ser);
            tri_dsc.grad.stops[1].opa = 255 - fract_lower;
            tri_dsc.grad.stops[1].frac = 255;

            lv_draw_triangle(base_dsc->layer, &tri_dsc);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
            rect_dsc.bg_grad.stops[0].color = lv_chart_get_series_color(obj, ser);
            rect_dsc.bg_grad.stops[0].frac = 0;
            rect_dsc.bg_grad.stops[0].opa = 255 - fract_lower;
            rect_dsc.bg_grad.stops[1].color = lv_chart_get_series_color(obj, ser);
            rect_dsc.bg_grad.stops[1].frac = 255;
            rect_dsc.bg_grad.stops[1].opa = 0;

            lv_area_t rect_area;
            rect_area.x1 = (int32_t)draw_line_dsc->p1.x;
            rect_area.x2 = (int32_t)draw_line_dsc->p2.x;
            rect_area.y1 = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
            rect_area.y2 = (int32_t)obj_coords.y2;
            lv_draw_rect(base_dsc->layer, &rect_dsc, &rect_area);
        }

        bool add_value = false;
        if(base_dsc->part == LV_PART_INDICATOR && lv_chart_get_pressed_point(obj) == base_dsc->id2) {
            if(lv_chart_get_type(obj) == LV_CHART_TYPE_LINE) {
                lv_draw_rect_dsc_t outline_dsc;
                lv_draw_rect_dsc_init(&outline_dsc);
                outline_dsc.bg_opa = LV_OPA_TRANSP;
                outline_dsc.outline_color = lv_color_white();
                outline_dsc.outline_width = 2;
                outline_dsc.radius = LV_RADIUS_CIRCLE;
                lv_area_t draw_task_area;
                lv_draw_task_get_area(draw_task, &draw_task_area);
                lv_draw_rect(base_dsc->layer, &outline_dsc, &draw_task_area);
                add_value = true;
            }
        }
        if(base_dsc->part == LV_PART_ITEMS && lv_chart_get_pressed_point(obj) == base_dsc->id2) {
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) ser = lv_chart_get_series_next(obj, ser);

            if(lv_chart_get_type(obj) == LV_CHART_TYPE_BAR) {
                lv_draw_fill_dsc_t * fill_dsc = lv_draw_task_get_draw_dsc(draw_task);
                lv_draw_rect_dsc_t shadow_dsc;
                lv_draw_rect_dsc_init(&shadow_dsc);
                shadow_dsc.radius = fill_dsc->radius;
                shadow_dsc.bg_opa = LV_OPA_TRANSP;
                shadow_dsc.shadow_color = lv_chart_get_series_color(obj, ser);
                shadow_dsc.shadow_width = 15;
                lv_area_t draw_task_area;
                lv_draw_task_get_area(draw_task, &draw_task_area);
                lv_draw_rect(base_dsc->layer, &shadow_dsc, &draw_task_area);
                add_value = true;
            }
        }

        if(add_value) {
            const lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
            if(base_dsc->id1 == 1) {
                ser = lv_chart_get_series_next(obj, ser);
            }

            char buf[8];
            lv_snprintf(buf, sizeof(buf), "%"LV_PRId32, lv_chart_get_series_y_array(obj, (lv_chart_series_t *)ser)[base_dsc->id2]);

            lv_text_attributes_t attributes;
            lv_text_attributes_init(&attributes);
            attributes.max_width = LV_COORD_MAX;
            attributes.text_flags = LV_TEXT_FLAG_NONE;
            lv_point_t text_size;
            lv_text_get_size(&text_size, buf, font_normal, &attributes);

            lv_area_t txt_area;
            lv_area_t draw_task_area;
            lv_draw_task_get_area(draw_task, &draw_task_area);
            txt_area.y2 = draw_task_area.y1 - LV_DPX(15);
            txt_area.y1 = txt_area.y2 - text_size.y;
            txt_area.x1 = draw_task_area.x1 + (lv_area_get_width(&draw_task_area) - text_size.x) / 2;
            txt_area.x2 = txt_area.x1 + text_size.x;

            lv_area_t bg_area;
            bg_area.x1 = txt_area.x1 - LV_DPX(8);
            bg_area.x2 = txt_area.x2 + LV_DPX(8);
            bg_area.y1 = txt_area.y1 - LV_DPX(8);
            bg_area.y2 = txt_area.y2 + LV_DPX(8);

            lv_draw_rect_dsc_t rect_dsc;
            lv_draw_rect_dsc_init(&rect_dsc);
            rect_dsc.bg_color = lv_chart_get_series_color(obj, ser);
            rect_dsc.radius = LV_DPX(5);
            lv_draw_rect(base_dsc->layer, &rect_dsc, &bg_area);

            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.color = lv_color_white();
            label_dsc.font = font_normal;
            label_dsc.text = buf;
            label_dsc.text_local = true;
            lv_draw_label(base_dsc->layer, &label_dsc, &txt_area);
        }

    }
}

#endif /* LV_USE_DEMO_WIDGETS */

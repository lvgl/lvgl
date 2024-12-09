/**
 * @file lv_demo_high_res_app_smart_meter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"
#include "../../src/widgets/chart/lv_chart_private.h"
#include "../../src/core/lv_obj_private.h"
#include "../../src/draw/lv_draw_private.h"
#include "../../src/misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

#define WIDGET3_POINT_COUNT 24
#define WIDGET3_ANIM_RANGE_END 10000

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    int32_t current[WIDGET3_POINT_COUNT];
    int32_t start[WIDGET3_POINT_COUNT];
} widget3_chart_anim_values_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void back_clicked_cb(lv_event_t * e);
static lv_obj_t * create_widget1_part(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                      const char * daily_val, const char * weekly_val);
static void widget1_anim_height_range(lv_demo_high_res_ctx_t * c, int32_t * min, int32_t * max);
static void widget1_open_part1_anim_cb(void * arg, int32_t val);
static void widget1_open_part2_anim_cb(void * arg, int32_t val);
static void widget1_clicked_cb(lv_event_t * e);
static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static lv_obj_t * widget23_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text);
static void widget2_chart_event_draw_task_cb(lv_event_t * e);
static void widget2_chart_bar_clicked_cb(lv_event_t * e);
static void widget2_chart_selected_day_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void widget3_chart_draw_task_event_cb(lv_event_t * e);
static void widget3_chart_free_anim_values(lv_event_t * e);
static void widget3_chart_anim_cb(void * var, int32_t v);
static void widget3_chart_selected_day_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_widget3_stat(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                      const char * kwh_val, const char * detail_val);
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

void lv_demo_high_res_app_smart_meter(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img,
                                &c->imgs[IMG_LIGHT_BG_SMART_METER]);

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
    lv_label_set_text_static(app_label, "Smart Meter");
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

static lv_obj_t * create_widget1_part(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                      const char * daily_val, const char * weekly_val)
{
    lv_obj_t * part = lv_obj_create(parent);
    lv_obj_remove_style_all(part);
    lv_obj_remove_flag(part, LV_OBJ_FLAG_SCROLLABLE);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, part,
                                &c->imgs[IMG_LIGHT_WIDGET1_BG]);
    lv_obj_set_style_bg_color(part, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(part, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(part, c->sz->card_radius, 0);
    lv_obj_set_width(part, LV_PCT(100));
    lv_obj_set_style_pad_all(part, c->sz->gap[7], 0);
    lv_obj_set_style_clip_corner(part, true, 0);

    lv_obj_t * title = lv_label_create(part);
    lv_label_set_text_static(title, title_val);
    lv_obj_add_style(title, &c->fonts[FONT_LABEL_MD], 0);

    lv_obj_t * box = lv_obj_create(part);
    lv_obj_remove_style_all(box);
    lv_obj_add_flag(box, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_size(box, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_align(box, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t * daily_box = lv_demo_high_res_simple_container_create(box, true, c->sz->gap[2], LV_FLEX_ALIGN_START);
    lv_obj_add_flag(daily_box, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * daily_label = lv_label_create(daily_box);
    lv_label_set_text_static(daily_label, "Daily");
    lv_obj_add_style(daily_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(daily_label, lv_color_black(), 0);

    lv_obj_t * daily_value_box = lv_demo_high_res_simple_container_create(daily_box, false, c->sz->gap[1],
                                                                          LV_FLEX_ALIGN_END);
    lv_obj_add_flag(daily_value_box, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * daily_value_label = lv_label_create(daily_value_box);
    lv_label_set_text_static(daily_value_label, daily_val);
    lv_obj_add_style(daily_value_label, &c->fonts[FONT_LABEL_LG], 0);
    lv_obj_set_style_text_color(daily_value_label, lv_color_black(), 0);
    lv_obj_t * daily_kwh_label = lv_label_create(daily_value_box);
    lv_label_set_text_static(daily_kwh_label, "kWh");
    lv_obj_add_style(daily_kwh_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(daily_kwh_label, lv_color_black(), 0);
    lv_obj_set_style_text_opa(daily_kwh_label, LV_OPA_40, 0);

    lv_obj_t * weekly_box = lv_demo_high_res_simple_container_create(box, true, c->sz->gap[2], LV_FLEX_ALIGN_START);
    lv_obj_add_flag(weekly_box, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_set_width(weekly_box, LV_PCT(45));

    lv_obj_t * weekly_label = lv_label_create(weekly_box);
    lv_label_set_text_static(weekly_label, "Weekly");
    lv_obj_add_style(weekly_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(weekly_label, lv_color_black(), 0);

    lv_obj_t * weekly_value_box = lv_demo_high_res_simple_container_create(weekly_box, false, c->sz->gap[1],
                                                                           LV_FLEX_ALIGN_END);
    lv_obj_add_flag(weekly_value_box, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * weekly_value_label = lv_label_create(weekly_value_box);
    lv_label_set_text_static(weekly_value_label, weekly_val);
    lv_obj_add_style(weekly_value_label, &c->fonts[FONT_LABEL_LG], 0);
    lv_obj_set_style_text_color(weekly_value_label, lv_color_black(), 0);
    lv_obj_t * weekly_kwh_label = lv_label_create(weekly_value_box);
    lv_label_set_text_static(weekly_kwh_label, "kWh");
    lv_obj_add_style(weekly_kwh_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(weekly_kwh_label, lv_color_black(), 0);
    lv_obj_set_style_text_opa(weekly_kwh_label, LV_OPA_40, 0);

    return part;
}

static void widget1_anim_height_range(lv_demo_high_res_ctx_t * c, int32_t * min, int32_t * max)
{
    *min = c->sz->smart_meter_collapsed_part_height;
    *max = c->sz->card_long_edge - c->sz->gap[7] - c->sz->smart_meter_collapsed_part_height;
}

static void widget1_open_part1_anim_cb(void * arg, int32_t val)
{
    lv_obj_t * box = arg;
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(box);
    lv_obj_t * part1 = lv_obj_get_child(box, 0);
    lv_obj_t * part2 = lv_obj_get_child(box, 1);
    int32_t min;
    int32_t max;
    widget1_anim_height_range(c, &min, &max);
    lv_obj_set_height(part1, lv_map(val, 0, 1000, min, max));
    lv_obj_set_style_bg_image_opa(part1, lv_map(val, 0, 1000, 255, 0), 0);
    lv_obj_set_style_bg_image_opa(part2, lv_map(val, 0, 1000, 0, 255), 0);
    lv_obj_set_style_opa(lv_obj_get_child(part1, 1), lv_map(val, 0, 1000, 0, 255), 0);
    lv_obj_set_style_opa(lv_obj_get_child(part2, 1), lv_map(val, 0, 1000, 255, 0), 0);
    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(&c->th);
    if(th == &lv_demo_high_res_theme_dark) {
        int32_t g = lv_map(val, 0, 1000, 0, 255);
        int32_t ginv = 255 - g;
        lv_obj_set_style_text_color(lv_obj_get_child(part1, 0), th == &lv_demo_high_res_theme_light
                                    ? lv_color_make(g, g, g) : lv_color_make(ginv, ginv, ginv), 0);
        lv_obj_set_style_text_color(lv_obj_get_child(part2, 0), th == &lv_demo_high_res_theme_light
                                    ? lv_color_make(ginv, ginv, ginv) : lv_color_make(g, g, g), 0);
    }
}

static void widget1_open_part2_anim_cb(void * arg, int32_t val)
{
    lv_obj_t * box = arg;
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(box);
    lv_obj_t * part1 = lv_obj_get_child(box, 0);
    lv_obj_t * part2 = lv_obj_get_child(box, 1);
    int32_t min;
    int32_t max;
    widget1_anim_height_range(c, &min, &max);
    lv_obj_set_height(part1, lv_map(val, 0, 1000, max, min));
    lv_obj_set_style_bg_image_opa(part1, lv_map(val, 0, 1000, 0, 255), 0);
    lv_obj_set_style_bg_image_opa(part2, lv_map(val, 0, 1000, 255, 0), 0);
    lv_obj_set_style_opa(lv_obj_get_child(part1, 1), lv_map(val, 0, 1000, 255, 0), 0);
    lv_obj_set_style_opa(lv_obj_get_child(part2, 1), lv_map(val, 0, 1000, 0, 255), 0);
    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(&c->th);
    if(th == &lv_demo_high_res_theme_dark) {
        int32_t g = lv_map(val, 0, 1000, 0, 255);
        int32_t ginv = 255 - g;
        lv_obj_set_style_text_color(lv_obj_get_child(part1, 0), th == &lv_demo_high_res_theme_light
                                    ? lv_color_make(ginv, ginv, ginv) : lv_color_make(g, g, g), 0);
        lv_obj_set_style_text_color(lv_obj_get_child(part2, 0), th == &lv_demo_high_res_theme_light
                                    ? lv_color_make(g, g, g) : lv_color_make(ginv, ginv, ginv), 0);
    }
}

static void widget1_clicked_cb(lv_event_t * e)
{
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    lv_obj_t * part = lv_event_get_current_target_obj(e);
    lv_obj_t * box = lv_obj_get_parent(part);
    lv_obj_t * part1 = lv_obj_get_child(box, 0);
    bool open_part1 = part == part1;

    if(lv_anim_get(box, NULL)) {
        return;
    }
    int32_t part1_height = lv_obj_get_height(part1);
    if((open_part1 && part1_height != c->sz->smart_meter_collapsed_part_height)
       || (!open_part1 && part1_height == c->sz->smart_meter_collapsed_part_height)) {
        return;
    }

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_duration(&a, 325);
    lv_anim_set_exec_cb(&a, open_part1 ? widget1_open_part1_anim_cb : widget1_open_part2_anim_cb);
    lv_anim_set_values(&a, 0, 1000);
    lv_anim_set_var(&a, box);
    lv_anim_start(&a);
}

static void create_widget1(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * box = lv_obj_create(widgets);
    lv_obj_remove_style_all(box);
    lv_obj_set_size(box, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(box, c->sz->gap[7], 0);
    lv_obj_set_user_data(box, c);

    lv_obj_t * part_title;

    lv_obj_t * part1 = create_widget1_part(c, box, "Self-consumption", "14.71", "89.32");
    lv_obj_set_height(part1, c->sz->smart_meter_collapsed_part_height);
    lv_obj_set_style_opa(lv_obj_get_child(part1, 1), LV_OPA_TRANSP, 0);
    part_title = lv_obj_get_child(part1, 0);
    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(&c->th);
    lv_obj_set_style_text_color(part_title, th == &lv_demo_high_res_theme_light ? lv_color_black() : lv_color_white(), 0);

    lv_obj_t * part2 = create_widget1_part(c, box, "Production", "12.56", "90.28");
    lv_obj_set_flex_grow(part2, 1);
    lv_obj_set_style_bg_image_opa(part2, LV_OPA_TRANSP, 0);
    part_title = lv_obj_get_child(part2, 0);
    lv_obj_set_style_text_color(part_title, lv_color_black(), 0);

    lv_obj_add_event_cb(part1, widget1_clicked_cb, LV_EVENT_CLICKED, c);
    lv_obj_add_event_cb(part2, widget1_clicked_cb, LV_EVENT_CLICKED, c);
}

static lv_obj_t * widget23_chart_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_label_set_text_static(label, text);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(label, LV_OPA_40, 0);
    return label;
}

static void widget2_chart_event_draw_task_cb(lv_event_t * e)
{
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    uint32_t idx = lv_subject_get_int(&c->smart_meter_selected_bar);
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_task_type_t type = lv_draw_task_get_type(t);
    if(type != LV_DRAW_TASK_TYPE_FILL) {
        return;
    }
    lv_draw_dsc_base_t * base_draw_dsc = lv_draw_task_get_draw_dsc(t);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(base_draw_dsc->part == LV_PART_MAIN) {
        lv_draw_rect_dsc_t rect_draw_dsc;
        lv_draw_rect_dsc_init(&rect_draw_dsc);
        rect_draw_dsc.bg_color = lv_subject_get_pointer(&c->th) == &lv_demo_high_res_theme_light ? lv_color_black() :
                                 lv_color_white();
        rect_draw_dsc.bg_opa = 16 * 255 / 100;

        lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
        lv_point_t col_pos;
        lv_chart_get_point_pos_by_id(obj, ser, idx, &col_pos);

        int32_t w = lv_obj_get_content_width(obj) / lv_chart_get_point_count(obj);

        lv_area_t a;
        a.x1 = obj->coords.x1 + col_pos.x - w / 2;
        a.x2 = obj->coords.x1 + col_pos.x + w / 2;
        a.y1 = obj->coords.y1;
        a.y2 = obj->coords.y2;

        lv_draw_rect(base_draw_dsc->layer, &rect_draw_dsc, &a);
    }
    else if(base_draw_dsc->part == LV_PART_ITEMS) {
        lv_draw_fill_dsc_t * fill_dsc = lv_draw_task_get_fill_dsc(t);
        if(fill_dsc->base.id2 != idx) {
            fill_dsc->opa = LV_OPA_40;
        }
    }
}

static void widget2_chart_bar_clicked_cb(lv_event_t * e)
{
    lv_obj_t * chart = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    uint32_t clicked_bar = lv_chart_get_pressed_point(chart);

    lv_subject_set_int(&c->smart_meter_selected_bar, clicked_bar);
}

static void widget2_chart_selected_day_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * chart = lv_observer_get_target_obj(observer);
    lv_obj_invalidate(chart);
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
    lv_label_set_text_static(title_label, "Daily usage");
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * usage_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * date_label = lv_label_create(usage_box);
    lv_label_set_text_static(date_label, "August 30, 2024");
    lv_obj_add_style(date_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(date_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * amount_box = lv_demo_high_res_simple_container_create(usage_box, false, c->sz->gap[1], LV_FLEX_ALIGN_END);

    lv_obj_t * amount_number = lv_label_create(amount_box);
    lv_label_set_text_static(amount_number, "15");
    lv_obj_add_style(amount_number, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(amount_number, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * amount_kwh = lv_label_create(amount_box);
    lv_label_set_text_static(amount_kwh, "kWh");
    lv_obj_add_style(amount_kwh, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(amount_kwh, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(amount_kwh, LV_OPA_40, 0);

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
    lv_obj_set_style_bg_opa(chart, LV_OPA_MIN + 1, 0);
    lv_obj_set_style_pad_column(chart, c->sz->gap[1], 0);
    lv_obj_set_style_radius(chart, c->sz->gap[3], LV_PART_ITEMS);
    lv_obj_set_style_border_width(chart, 2, 0);
    lv_obj_set_style_border_color(chart, lv_color_black(), 0);
    lv_obj_set_style_border_side(chart, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_post(chart, true, 0);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 0, 0);

    lv_obj_add_event_cb(chart, widget2_chart_event_draw_task_cb, LV_EVENT_DRAW_TASK_ADDED, c);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 101);
    lv_chart_set_div_line_count(chart, 3, 0);
    lv_obj_set_style_line_opa(chart, LV_OPA_40, 0);
    lv_obj_set_style_line_color(chart, lv_color_black(), 0);
    lv_obj_set_style_line_dash_width(chart, 2, 0);
    lv_obj_set_style_line_dash_gap(chart, 1, 0);
    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_color_white(), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t chart_values[] = {45, 14, 45, 52, 69, 52, 63};
    lv_chart_set_ext_y_array(chart, ser, (int32_t *)chart_values);
    lv_chart_set_point_count(chart, sizeof(chart_values) / sizeof(*chart_values));
    lv_chart_refresh(chart);

    lv_obj_add_event_cb(chart, widget2_chart_bar_clicked_cb, LV_EVENT_VALUE_CHANGED, c);
    lv_subject_add_observer_obj(&c->smart_meter_selected_bar, widget2_chart_selected_day_observer_cb, chart, NULL);

    lv_obj_t * hscale_label_1 = widget23_chart_label(c, chart_grid, "Aug 2");
    lv_obj_set_grid_cell(hscale_label_1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_t * hscale_label_2 = widget23_chart_label(c, chart_grid, "Sep 1");
    lv_obj_set_grid_cell(hscale_label_2, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_t * vscale_label_1 = widget23_chart_label(c, chart_grid, "20");
    lv_obj_set_grid_cell(vscale_label_1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_t * vscale_label_2 = widget23_chart_label(c, chart_grid, "10");
    lv_obj_set_grid_cell(vscale_label_2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t * vscale_label_3 = widget23_chart_label(c, chart_grid, "0");
    lv_obj_set_grid_cell(vscale_label_3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_END, 0, 1);
}

static void widget3_chart_draw_task_event_cb(lv_event_t * e)
{
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_task_type_t task_type = lv_draw_task_get_type(t);

    if(task_type == LV_DRAW_TASK_TYPE_FILL) {
        lv_draw_dsc_base_t * base_draw_dsc = lv_draw_task_get_draw_dsc(t);
        if(base_draw_dsc->part != LV_PART_ITEMS) return;

        lv_obj_t * obj = lv_event_get_target(e);
        lv_chart_series_t * ser = lv_chart_get_series_next(obj, NULL);
        int32_t * ser_array = ser->y_points;
        int32_t p_idx = base_draw_dsc->id2;
        int32_t v = ser_array[p_idx];
        int32_t mid_y = obj->coords.y1 + lv_obj_get_height(obj) / 2;
        if(v < 0) {
            t->area.y2 = t->area.y1;
            t->area.y1 = mid_y;
        }
        else {
            t->area.y2 = mid_y;
        }
    }
    else if(task_type == LV_DRAW_TASK_TYPE_LINE) {
        lv_draw_line_dsc_t * line_draw_dsc = lv_draw_task_get_draw_dsc(t);
        if(line_draw_dsc->base.id1 != 1) return;
        line_draw_dsc->dash_gap = 0;
        line_draw_dsc->color = lv_color_black();
        line_draw_dsc->opa = LV_OPA_COVER;
        line_draw_dsc->width = 2;
        line_draw_dsc->p1.y -= 1;
        line_draw_dsc->p2.y -= 1;
    }
}

static void widget3_chart_free_anim_values(lv_event_t * e)
{
    lv_obj_t * chart = lv_event_get_target_obj(e);
    widget3_chart_anim_values_t * anim_values = lv_obj_get_user_data(chart);
    lv_free(anim_values);
}

static void widget3_chart_anim_cb(void * var, int32_t v)
{
    lv_obj_t * chart = var;
    widget3_chart_anim_values_t * anim_values = lv_obj_get_user_data(chart);
    int32_t * end_values = lv_anim_get_user_data(lv_anim_get(var, widget3_chart_anim_cb));

    for(int32_t i = 0; i < WIDGET3_POINT_COUNT; i++) {
        anim_values->current[i] = lv_map(v, 0, WIDGET3_ANIM_RANGE_END, anim_values->start[i], end_values[i]);
    }

    lv_chart_refresh(chart);
}

static void widget3_chart_selected_day_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    static bool data_is_init = false;
    static int32_t data[WIDGET3_POINT_COUNT * 7];
    if(!data_is_init) {
        data_is_init = true;
        for(int32_t i = 0; i < WIDGET3_POINT_COUNT * 7; i++) {
            data[i] = lv_map(-lv_trigo_cos(i * 15), -32767, 32767, -105, 105) + lv_rand(-20, 20);
        }
    }

    lv_obj_t * chart = lv_observer_get_target_obj(observer);
    int32_t idx = lv_subject_get_int(subject);

    widget3_chart_anim_values_t * anim_values = lv_obj_get_user_data(chart);
    if(!anim_values) {
        anim_values = lv_malloc_zeroed(sizeof(widget3_chart_anim_values_t));
        LV_ASSERT_MALLOC(anim_values);
        lv_obj_set_user_data(chart, anim_values);
        lv_obj_add_event_cb(chart, widget3_chart_free_anim_values, LV_EVENT_DELETE, NULL);

        lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);
        lv_chart_set_ext_y_array(chart, ser, anim_values->current);
    }

    lv_memcpy(anim_values->start, anim_values->current, sizeof(anim_values->current));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, chart);
    lv_anim_set_duration(&a, 400);
    lv_anim_set_values(&a, 0, WIDGET3_ANIM_RANGE_END);
    lv_anim_set_exec_cb(&a, widget3_chart_anim_cb);
    lv_anim_set_user_data(&a, &data[idx * WIDGET3_POINT_COUNT]);
    lv_anim_set_early_apply(&a, true);
    lv_anim_start(&a);
}

static lv_obj_t * create_widget3_stat(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * title_val,
                                      const char * kwh_val, const char * detail_val)
{
    lv_obj_t * obj = lv_demo_high_res_simple_container_create(parent, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(obj, 1);

    lv_obj_t * title_label = lv_label_create(obj);
    lv_label_set_text_static(title_label, title_val);
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * kwh_box = lv_demo_high_res_simple_container_create(obj, false, c->sz->gap[1], LV_FLEX_ALIGN_END);

    lv_obj_t * kwh_value_label = lv_label_create(kwh_box);
    lv_label_set_text_static(kwh_value_label, kwh_val);
    lv_obj_add_style(kwh_value_label, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_add_style(kwh_value_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * kwh_label = lv_label_create(kwh_box);
    lv_label_set_text_static(kwh_label, "kWh");
    lv_obj_add_style(kwh_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(kwh_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(kwh_label, LV_OPA_60, 0);

    lv_obj_t * detail_label = lv_label_create(obj);
    lv_label_set_text_static(detail_label, detail_val);
    lv_obj_add_style(detail_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(detail_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(detail_label, LV_OPA_60, 0);

    return obj;
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
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t * title_label = lv_label_create(widget);
    lv_label_set_text_static(title_label, "Balance");
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
    lv_obj_set_height(chart, c->sz->large_chart_height);
    lv_obj_set_style_bg_opa(chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(chart, c->sz->gap[1], 0);
    lv_obj_set_style_radius(chart, c->sz->gap[3], LV_PART_ITEMS);
    lv_obj_set_style_border_side(chart, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 5, 0);
    lv_obj_set_grid_cell(chart, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -125, 125);
    lv_chart_set_div_line_count(chart, 3, 0);
    lv_obj_set_style_line_opa(chart, LV_OPA_40, 0);
    lv_obj_set_style_line_color(chart, lv_color_black(), 0);
    lv_obj_set_style_line_dash_width(chart, 2, 0);
    lv_obj_set_style_line_dash_gap(chart, 1, 0);
    lv_obj_add_event_cb(chart, widget3_chart_draw_task_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_chart_add_series(chart, lv_color_white(), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(chart, WIDGET3_POINT_COUNT);
    lv_subject_add_observer_obj(&c->smart_meter_selected_bar, widget3_chart_selected_day_observer_cb, chart, NULL);

    lv_obj_t * hscale_label_1 = widget23_chart_label(c, chart_grid, "0h");
    lv_obj_set_grid_cell(hscale_label_1, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
    lv_obj_t * hscale_label_2 = widget23_chart_label(c, chart_grid, "24h");
    lv_obj_set_grid_cell(hscale_label_2, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 1, 1);

    lv_obj_t * vscale_label_1 = widget23_chart_label(c, chart_grid, "5");
    lv_obj_set_grid_cell(vscale_label_1, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_obj_t * vscale_label_2 = widget23_chart_label(c, chart_grid, "0\nkWh");
    lv_obj_set_grid_cell(vscale_label_2, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_t * vscale_label_3 = widget23_chart_label(c, chart_grid, "-5");
    lv_obj_set_grid_cell(vscale_label_3, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_END, 0, 1);

    lv_obj_t * stat_box = lv_demo_high_res_simple_container_create(widget, false, c->sz->gap[5], LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(stat_box, LV_PCT(100));
    create_widget3_stat(c, stat_box, "Sep 1, 2024", "+12", "P:  +36kWh U: -18kWh");
    create_widget3_stat(c, stat_box, "Week 41", "-56", "P:  +243kWh U: -299kWh");
}

#endif /*LV_USE_DEMO_HIGH_RES*/

/**
 * @file lv_demo_high_res_app_smart_home.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"
#include "../../src/widgets/slider/lv_slider.h"
#include "../../src/widgets/arc/lv_arc.h"
#include "../../src/widgets/switch/lv_switch.h"

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
static void charging_arc_observer(lv_observer_t * observer, lv_subject_t * subject);
static void charging_percent_label_observer(lv_observer_t * observer, lv_subject_t * subject);
static void charging_time_until_full_label_observer(lv_observer_t * observer, lv_subject_t * subject);
static void create_widget_charging(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void handle_locked_value(lv_obj_t * slider, bool locked);
static void widget2_slider_released_cb(lv_event_t * e);
static void widget2_slider_locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void widget3_play_pause_clicked_cb(lv_event_t * e);
static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void create_widget4(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void door_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_widget5_door(lv_demo_high_res_ctx_t * c, lv_obj_t * parent);
static void switch_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_switch(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, lv_image_dsc_t ** img_dsc_pair,
                                lv_subject_t * subject);
static lv_obj_t * create_lightbulbs_switch_with_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent,
                                                      const char * label_text, lv_subject_t * subject);
static lv_obj_t * create_widget5_lightbulbs(lv_demo_high_res_ctx_t * c, lv_obj_t * parent);
static lv_obj_t * create_widget5_zigbee_fan(lv_demo_high_res_ctx_t * c, lv_obj_t * parent);
static void slider_draw_start_cb(lv_event_t * e);
static void slider_draw_end_cb(lv_event_t * e);
static void slider_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void slider_label_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_slider(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * label_text,
                                lv_image_dsc_t ** img_dsc_pair, lv_subject_t * subject);
static void create_widget5(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_app_smart_home(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img,
                                &c->imgs[IMG_LIGHT_BG_SMART_HOME]);

    lv_obj_t * bg_cont = lv_obj_create(bg);
    lv_obj_remove_style_all(bg_cont);
    lv_obj_set_size(bg_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(bg_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);

    /* top margin */

    lv_obj_t * top_margin = lv_demo_high_res_top_margin_create(bg_cont,
                                                               c->sz == &lv_demo_high_res_sizes_all[SIZE_SM] ? c->sz->gap[9] : c->sz->gap[10], true, c);

    /* app info */

    lv_obj_t * app_info = lv_demo_high_res_simple_container_create(bg_cont, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_add_flag(app_info, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align_to(app_info, top_margin, LV_ALIGN_OUT_BOTTOM_LEFT, c->sz->gap[10], c->sz->gap[10]);

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
    lv_label_set_text_static(app_label, "Smart Home");
    lv_obj_add_style(app_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(app_label, &c->fonts[FONT_HEADING_LG], 0);

    /* widgets */

    lv_obj_t * widgets = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(widgets);
    lv_obj_set_width(widgets, LV_PCT(100));
    lv_obj_set_flex_grow(widgets, 1);
    lv_obj_set_style_pad_bottom(widgets, c->sz->gap[10], 0);
    lv_obj_set_style_pad_left(widgets, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(widgets, c->sz->gap[10], 0);
    lv_obj_set_flex_flow(widgets, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(widgets, c->sz->gap[7], 0);
    lv_obj_set_flex_align(widgets, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    create_widget1(c, widgets);
    create_widget_charging(c, widgets);
    create_widget2(c, widgets);
    create_widget3(c, widgets);
    create_widget4(c, widgets);
    create_widget5(c, widgets);

    /* bring app info to top so the back button can be clicked */
    lv_obj_move_to_index(app_info, -1);
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
    lv_obj_set_style_bg_image_src(widget, c->imgs[IMG_SMART_HOME_WIDGET1_BG], 0);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Climate Control");
    lv_obj_set_width(top_label, LV_PCT(100));
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_set_style_text_color(top_label, lv_color_white(), 0);

    lv_obj_t * main_temp_box = lv_obj_create(widget);
    lv_obj_remove_style_all(main_temp_box);
    lv_obj_set_size(main_temp_box, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_ver(main_temp_box, c->sz->gap[6], 0);
    lv_obj_set_style_bg_color(main_temp_box, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(main_temp_box, LV_OPA_20, 0);
    lv_obj_set_style_radius(main_temp_box, LV_COORD_MAX, 0);

    lv_obj_t * main_temp = lv_label_create(main_temp_box);
    lv_obj_add_style(main_temp, &c->fonts[FONT_LABEL_2XL], 0);
    lv_obj_set_style_text_color(main_temp, lv_color_white(), 0);
    lv_obj_center(main_temp);
    lv_demo_high_res_label_bind_temperature(main_temp, &c->api.subjects.temperature_indoor, c);

    lv_obj_t * outdoor_temp_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[2],
                                                                           LV_FLEX_ALIGN_CENTER);

    lv_obj_t * outdoor_temp = lv_label_create(outdoor_temp_box);
    lv_obj_add_style(outdoor_temp, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_set_style_text_color(outdoor_temp, lv_color_white(), 0);
    lv_demo_high_res_label_bind_temperature(outdoor_temp, &c->api.subjects.temperature_outdoor, c);

    lv_obj_t * outdoor_label = lv_label_create(outdoor_temp_box);
    lv_label_set_text_static(outdoor_label, "Outdoor");
    lv_obj_add_style(outdoor_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(outdoor_label, lv_color_white(), 0);
}

static void charging_arc_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * arc = lv_observer_get_target_obj(observer);
    lv_arc_set_value(arc, lv_map(lv_subject_get_int(subject), 0, EV_CHARGING_RANGE_END, 0, 100));
}

static void charging_percent_label_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    lv_label_set_text_fmt(label, "%"LV_PRId32"%%", lv_map(lv_subject_get_int(subject), 0, EV_CHARGING_RANGE_END, 0, 100));
}

static void charging_time_until_full_label_observer(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    int32_t v_range_time_to_full = lv_map(lv_subject_get_int(subject), 0, EV_CHARGING_RANGE_END, 72, 0);
    int32_t whole = v_range_time_to_full / 10;
    int32_t fraction = v_range_time_to_full % 10;
    lv_label_set_text_fmt(label, "%"LV_PRId32".%"LV_PRId32, whole, fraction);
}

static void create_widget_charging(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_obj_set_style_bg_image_src(widget, c->imgs[IMG_SMART_HOME_WIDGET2_BG], 0);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Charging");
    lv_obj_set_width(top_label, LV_PCT(100));
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_set_style_text_color(top_label, lv_color_white(), 0);

    lv_obj_t * arc_cont = lv_obj_create(widget);
    lv_obj_remove_style_all(arc_cont);
    lv_obj_set_width(arc_cont, LV_PCT(100));
    lv_obj_set_flex_grow(arc_cont, 1);

    lv_obj_t * arc = lv_arc_create(arc_cont);
    lv_obj_set_align(arc, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_size(arc, c->sz->smart_home_arc_diameter, c->sz->smart_home_arc_diameter);
    lv_arc_set_rotation(arc, 270);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_rounded(arc, false, 0);
    lv_obj_set_style_arc_rounded(arc, false, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, 8, 0);
    lv_obj_set_style_arc_width(arc, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_white(), 0);
    lv_obj_set_style_arc_color(arc, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc, LV_OPA_20, 0);
    lv_subject_add_observer_obj(&c->ev_charging_progress, charging_arc_observer, arc, NULL);

    lv_obj_t * percent_label = lv_label_create(arc);
    lv_obj_add_style(percent_label, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_set_style_text_color(percent_label, lv_color_white(), 0);
    lv_obj_center(percent_label);
    lv_subject_add_observer_obj(&c->ev_charging_progress, charging_percent_label_observer, percent_label, NULL);

    lv_obj_t * num_label_cont = lv_demo_high_res_simple_container_create(widget,
                                                                         false,
                                                                         c->sz->gap[1],
                                                                         LV_FLEX_ALIGN_END);
    lv_obj_set_width(num_label_cont, LV_PCT(100));

    lv_obj_t * time_to_full_num_label = lv_label_create(num_label_cont);
    lv_obj_add_style(time_to_full_num_label, &c->fonts[FONT_LABEL_XL], 0);
    lv_obj_set_style_text_color(time_to_full_num_label, lv_color_white(), 0);
    lv_subject_add_observer_obj(&c->ev_charging_progress, charging_time_until_full_label_observer, time_to_full_num_label,
                                NULL);

    lv_obj_t * h_label = lv_label_create(num_label_cont);
    lv_label_set_text_static(h_label, "h");
    lv_obj_add_style(h_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(h_label, lv_color_white(), 0);

    lv_obj_t * time_to_full_label = lv_label_create(widget);
    lv_label_set_text_static(time_to_full_label, "Time to full charge");
    lv_obj_add_style(time_to_full_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_set_style_text_color(time_to_full_label, lv_color_white(), 0);
    lv_obj_set_width(time_to_full_label, LV_PCT(100));
}

static void handle_locked_value(lv_obj_t * slider, bool locked)
{
    lv_slider_set_value(slider, locked ? 100 : 0, LV_ANIM_ON);
    lv_obj_update_flag(slider, LV_OBJ_FLAG_CLICKABLE, !locked);
}

static void widget2_slider_released_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    bool locked = lv_slider_get_value(slider) >= 100;
    handle_locked_value(slider, locked);
    lv_subject_set_int(&c->api.subjects.locked, locked);
}
static void widget2_slider_locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * slider = lv_observer_get_target_obj(observer);
    bool locked = lv_subject_get_int(subject);
    handle_locked_value(slider, locked);
}

static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET2_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Lock");
    lv_obj_set_width(top_label, LV_PCT(100));
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(top_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * slider = lv_slider_create(widget);
    lv_obj_set_size(slider, LV_PCT(100), c->sz->icon[ICON_2XL]);
    lv_obj_set_style_pad_hor(slider, c->sz->icon[ICON_2XL] / 2, 0);
    lv_obj_set_style_pad_all(slider, 0, LV_PART_KNOB);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_INDICATOR);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_set_style_bg_image_src(slider, c->imgs[IMG_UNLOCK], LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider, LV_OPA_30, 0);
    lv_slider_set_value(slider, lv_subject_get_int(&c->api.subjects.locked) ? 100 : 0, LV_ANIM_OFF);
    lv_obj_set_style_anim_duration(slider, lv_anim_speed(2000), 0);
    lv_obj_add_event_cb(slider, widget2_slider_released_cb, LV_EVENT_RELEASED, c);
    lv_subject_add_observer_obj(&c->api.subjects.locked, widget2_slider_locked_observer_cb, slider, c);

    lv_obj_t * bottom_label = lv_label_create(widget);
    lv_label_set_text_static(bottom_label, "Swipe to lock screen");
    lv_obj_add_style(bottom_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(bottom_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
}

static void widget3_play_pause_clicked_cb(lv_event_t * e)
{
    lv_obj_t * play_pause_img = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);

    bool was_playing = lv_image_get_src(play_pause_img) == c->imgs[IMG_PLAY_ICON];
    lv_image_set_src(play_pause_img, c->imgs[was_playing ? IMG_PLAY_ICON_1 : IMG_PLAY_ICON]);
    lv_subject_set_int(&c->api.subjects.music_play, !was_playing);
}

static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->card_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET3_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Speaker");
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(top_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_align(top_label, LV_ALIGN_TOP_LEFT);

    lv_obj_t * music_labels_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[1],
                                                                           LV_FLEX_ALIGN_START);
    lv_obj_set_align(music_labels_box, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_t * song_title = lv_label_create(music_labels_box);
    lv_label_set_text_static(song_title, "there, then, now");
    lv_obj_add_style(song_title, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(song_title, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_t * song_artist = lv_label_create(music_labels_box);
    lv_label_set_text_static(song_artist, "imagiro, Lowswimm...");
    lv_obj_add_style(song_artist, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(song_artist, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(song_artist, LV_OPA_40, 0);

    lv_obj_t * slider = lv_slider_create(widget);
    lv_obj_set_size(slider, c->sz->slider_width, LV_PCT(100));
    lv_obj_set_align(slider, LV_ALIGN_RIGHT_MID);
    lv_obj_set_style_bg_color(slider, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider, 30, 0);
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, c->sz->gap[4], 0);
    lv_obj_set_style_radius(slider, c->sz->gap[4], LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_top(slider, -(c->sz->slider_width - 6) / 2 - 8, LV_PART_KNOB);
    lv_obj_set_style_pad_bottom(slider, -(c->sz->slider_width - 6) / 2 + 8, LV_PART_KNOB);
    lv_obj_set_style_pad_hor(slider, -(c->sz->slider_width - 12) / 2, LV_PART_KNOB);

    lv_obj_t * slider_pct_label = lv_label_create(slider);
    lv_obj_add_style(slider_pct_label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(slider_pct_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_align(slider_pct_label, LV_ALIGN_TOP_MID, 0, 10);

    lv_slider_bind_value(slider, &c->api.subjects.volume);
    lv_label_bind_text(slider_pct_label, &c->api.subjects.volume, "%"PRId32"%%");

    lv_obj_t * slider_volume_img = lv_image_create(slider);
    lv_image_set_src(slider_volume_img, c->imgs[IMG_VOLUME]);
    lv_obj_align(slider_volume_img, LV_ALIGN_BOTTOM_MID, 0, -8);

    lv_obj_t * album_art = lv_image_create(widget);
    lv_image_set_src(album_art, c->imgs[IMG_ALBUM_ART]);

    lv_obj_t * controls = lv_demo_high_res_simple_container_create(widget, false, 0, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_align(controls, LV_ALIGN_LEFT_MID);

    lv_obj_t * prev_song = lv_image_create(controls);
    lv_image_set_src(prev_song, c->imgs[IMG_BACKWARD_ICON]);
    lv_obj_set_style_image_recolor(prev_song, lv_color_white(), 0);
    lv_obj_set_style_image_recolor_opa(prev_song, LV_OPA_COVER, 0);

    lv_obj_t * play_pause_img = lv_image_create(controls);
    lv_image_set_src(play_pause_img, c->imgs[lv_subject_get_int(&c->api.subjects.music_play) ? IMG_PLAY_ICON :
                                             IMG_PLAY_ICON_1]);
    lv_obj_set_style_image_recolor(play_pause_img, lv_color_white(), 0);
    lv_obj_set_style_image_recolor_opa(play_pause_img, LV_OPA_COVER, 0);
    lv_obj_add_flag(play_pause_img, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(play_pause_img, widget3_play_pause_clicked_cb, LV_EVENT_CLICKED, c);

    lv_obj_t * next_song = lv_image_create(controls);
    lv_image_set_src(next_song, c->imgs[IMG_FORWARD_ICON]);
    lv_obj_set_style_image_recolor(next_song, lv_color_white(), 0);
    lv_obj_set_style_image_recolor_opa(next_song, LV_OPA_COVER, 0);

    lv_obj_align_to(album_art, controls, LV_ALIGN_CENTER, 0, 0);
}

static void create_widget4(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
{
    lv_obj_t * widget = lv_obj_create(widgets);
    lv_obj_remove_style_all(widget);
    lv_obj_set_size(widget, c->sz->widget_long_edge, c->sz->card_long_edge);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_obj_bg_image_src_cb, widget,
                                &c->imgs[IMG_LIGHT_WIDGET4_BG]);
    lv_obj_set_style_pad_all(widget, c->sz->gap[7], 0);
    lv_obj_set_flex_flow(widget, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(widget, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(widget, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * top_label = lv_label_create(widget);
    lv_label_set_text_static(top_label, "Main Light");
    lv_obj_add_style(top_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(top_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * temperature_box = lv_demo_high_res_simple_container_create(widget, true, c->sz->gap[5], LV_FLEX_ALIGN_START);
    lv_obj_set_width(temperature_box, LV_PCT(100));
    lv_obj_set_style_pad_ver(temperature_box, 8, 0); /* so the slider knob is not clipped */

    lv_obj_t * temperature_slider = lv_slider_create(temperature_box);
    lv_obj_set_size(temperature_slider, LV_PCT(100), c->imgs[IMG_MAIN_LIGHT_SLIDER]->header.h);
    lv_obj_set_style_bg_opa(temperature_slider, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(temperature_slider, LV_OPA_TRANSP, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(temperature_slider, lv_color_white(), LV_PART_KNOB);
    lv_obj_set_style_radius(temperature_slider, 0, 0);
    lv_obj_set_style_pad_ver(temperature_slider, 4, 0);
    lv_obj_set_style_pad_hor(temperature_slider, -((c->sz->slider_width - 6) / 2), LV_PART_KNOB);
    lv_obj_set_style_bg_image_src(temperature_slider, c->imgs[IMG_MAIN_LIGHT_SLIDER], 0);
    lv_slider_set_range(temperature_slider, 0, 20000);

    lv_obj_t * temperature_label_box = lv_demo_high_res_simple_container_create(temperature_box, true, c->sz->gap[1],
                                                                                LV_FLEX_ALIGN_START);
    lv_obj_t * temperature_label = lv_label_create(temperature_label_box);
    lv_label_set_text_static(temperature_label, "Light temperature");
    lv_obj_add_style(temperature_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(temperature_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_t * temperature_value_label = lv_label_create(temperature_label_box);
    lv_obj_add_style(temperature_value_label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(temperature_value_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(temperature_value_label, LV_OPA_40, 0);

    lv_slider_bind_value(temperature_slider, &c->api.subjects.main_light_temperature);
    lv_label_bind_text(temperature_value_label, &c->api.subjects.main_light_temperature, "%"PRId32" K");

    lv_obj_t * slider = lv_slider_create(widget);
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

    lv_slider_bind_value(slider, &c->api.subjects.main_light_intensity);
    lv_label_bind_text(slider_pct_label, &c->api.subjects.main_light_intensity, "%"PRId32"%%");

    lv_obj_t * slider_lamp_img = lv_image_create(slider);
    lv_image_set_src(slider_lamp_img, c->imgs[IMG_LAMP]);
    lv_obj_align(slider_lamp_img, LV_ALIGN_LEFT_MID, 8, 0);
}

static void door_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);
    lv_obj_t * door_box = lv_observer_get_target_obj(observer);
    lv_obj_t * icon = lv_obj_get_child(door_box, 0);
    lv_obj_t * label = lv_obj_get_child(door_box, 1);
    bool door_is_open = lv_subject_get_int(subject);
    lv_obj_set_style_bg_opa(door_box, door_is_open ? LV_OPA_60 : LV_OPA_20, 0);
    lv_image_set_src(icon, c->imgs[door_is_open ? IMG_DOOR_1 : IMG_DOOR]);
    lv_obj_set_style_bg_opa(icon, door_is_open ? LV_OPA_COVER : LV_OPA_40, 0);
    lv_label_set_text_static(label, door_is_open ? "Door open" : "Door closed");
}

static lv_obj_t * create_widget5_door(lv_demo_high_res_ctx_t * c, lv_obj_t * parent)
{
    lv_obj_t * door_box = lv_obj_create(parent);
    lv_obj_remove_style_all(door_box);
    lv_obj_set_size(door_box, c->sz->indicator_width, c->sz->indicator_height);
    lv_obj_set_flex_flow(door_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(door_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(door_box, 0, 0);
    lv_obj_set_style_bg_color(door_box, lv_color_white(), 0);
    lv_obj_set_style_radius(door_box, c->sz->gap[3], 0);
    lv_obj_set_style_pad_ver(door_box, c->sz->gap[5], 0);

    lv_obj_t * icon = lv_image_create(door_box);
    lv_image_set_inner_align(icon, LV_IMAGE_ALIGN_CENTER);
    lv_obj_set_size(icon, c->sz->icon[ICON_XL], c->sz->icon[ICON_XL]);
    lv_obj_set_style_bg_color(icon, lv_color_white(), 0);
    lv_obj_set_style_radius(icon, LV_COORD_MAX, 0);
    lv_obj_set_style_shadow_width(icon, 50, 0);
    lv_obj_set_style_shadow_opa(icon, 15 * 255 / 100, 0);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * label = lv_label_create(door_box);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_subject_add_observer_obj(&c->api.subjects.door, door_observer_cb, door_box, c);

    return door_box;
}

static void switch_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * sw = lv_observer_get_target_obj(observer);
    lv_image_dsc_t ** pair = lv_observer_get_user_data(observer);
    bool is_on = lv_subject_get_int(subject);
    lv_obj_set_style_bg_image_src(sw, pair[is_on], LV_PART_KNOB);
}

static lv_obj_t * create_switch(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, lv_image_dsc_t ** img_dsc_pair,
                                lv_subject_t * subject)
{
    lv_obj_t * sw = lv_switch_create(parent);
    lv_obj_set_size(sw, c->sz->slider_width * 2, c->sz->slider_width);
    lv_obj_set_style_pad_all(sw, 0, LV_PART_KNOB);
    lv_obj_set_style_bg_color(sw, lv_color_hex(0xCAF0A2), LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(sw, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(sw, LV_OPA_30, 0);
    lv_obj_set_style_shadow_width(sw, 25, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(sw, 15 * 255 / 100, LV_PART_KNOB);

    lv_obj_bind_checked(sw, subject);
    lv_subject_add_observer_obj(subject, switch_observer_cb, sw, img_dsc_pair);

    return sw;
}

static lv_obj_t * create_lightbulbs_switch_with_label(lv_demo_high_res_ctx_t * c, lv_obj_t * parent,
                                                      const char * label_text, lv_subject_t * subject)
{
    lv_obj_t * box = lv_demo_high_res_simple_container_create(parent, true, c->sz->gap[3], LV_FLEX_ALIGN_CENTER);

    lv_obj_t * label = lv_label_create(box);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(label, LV_OPA_50, 0);
    lv_label_set_text_static(label, label_text);

    create_switch(c, box, &c->imgs[IMG_LAMP2], subject);

    return box;
}

static lv_obj_t * create_widget5_lightbulbs(lv_demo_high_res_ctx_t * c, lv_obj_t * parent)
{
    lv_obj_t * lightbulbs_box = lv_obj_create(parent);
    lv_obj_remove_style_all(lightbulbs_box);
    lv_obj_set_height(lightbulbs_box, c->sz->indicator_height);
    lv_obj_set_flex_flow(lightbulbs_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(lightbulbs_box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(lightbulbs_box, 0, 0);
    lv_obj_set_style_bg_color(lightbulbs_box, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(lightbulbs_box, LV_OPA_20, 0);
    lv_obj_set_style_radius(lightbulbs_box, c->sz->gap[3], 0);
    lv_obj_set_style_pad_ver(lightbulbs_box, c->sz->gap[5], 0);

    lv_obj_t * switch_box = lv_demo_high_res_simple_container_create(lightbulbs_box, false, c->sz->gap[7],
                                                                     LV_FLEX_ALIGN_END);
    create_lightbulbs_switch_with_label(c, switch_box, "MATTER", &c->api.subjects.lightbulb_matter);
    create_lightbulbs_switch_with_label(c, switch_box, "ZIGBEE", &c->api.subjects.lightbulb_zigbee);

    lv_obj_t * label = lv_label_create(lightbulbs_box);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_label_set_text_static(label, "Lightbulbs Sensors");

    return lightbulbs_box;
}

static lv_obj_t * create_widget5_zigbee_fan(lv_demo_high_res_ctx_t * c, lv_obj_t * parent)
{
    lv_obj_t * box = lv_demo_high_res_simple_container_create(parent, true, c->sz->gap[9], LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_top(box, c->sz->gap[5], 0);

    lv_obj_t * label = lv_label_create(box);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_label_set_text_static(label, "Zigbee Fan");

    create_switch(c, box, &c->imgs[IMG_FAN2], &c->api.subjects.fan_zigbee);

    return box;
}

static void slider_draw_start_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_area_t indic_area;
    lv_obj_get_coords(slider, &indic_area);
    int32_t indic_w = lv_area_get_width(&indic_area);
    int32_t indic_h = lv_area_get_height(&indic_area);
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    lv_area_t rect_area;
    lv_layer_t * layer = lv_event_get_layer(e);

    /* draw the ticks */
    lv_area_set(&rect_area, 0, 0, 1, indic_h / 4);
    rect_dsc.bg_opa = LV_OPA_40;

    for(int32_t i = 1; i <= 3; i++) {
        lv_area_set_width(&indic_area, indic_w * i / 3);
        int32_t offset_from_right = -indic_h * i / 4;
        lv_area_align(&indic_area, &rect_area, LV_ALIGN_RIGHT_MID, offset_from_right, 0);
        lv_draw_rect(layer, &rect_dsc, &rect_area);
    }
}

static void slider_draw_end_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    if(lv_slider_get_value(slider) == 0) {
        /* Add a rectangle for the off state instead of no indicator
         * showing at all.
         */
        lv_draw_rect_dsc_t rect_dsc;
        lv_draw_rect_dsc_init(&rect_dsc);
        rect_dsc.radius = lv_obj_get_style_radius(slider, LV_PART_INDICATOR);

        lv_area_t rect_area;
        lv_obj_get_coords(slider, &rect_area);
        int32_t indic_h = lv_area_get_height(&rect_area);
        lv_area_set_width(&rect_area, indic_h * 5 / 4);

        lv_layer_t * layer = lv_event_get_layer(e);
        lv_draw_rect(layer, &rect_dsc, &rect_area);

        rect_dsc.radius = 0;
    }
}

static void slider_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * slider = lv_observer_get_target_obj(observer);
    lv_obj_t * slider_img = lv_obj_get_child(slider, 0);
    lv_image_dsc_t ** pair = lv_observer_get_user_data(observer);
    bool is_on = lv_slider_get_value(slider) > 0;
    lv_image_set_src(slider_img, pair[is_on]);
}

static void slider_label_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * slider = lv_observer_get_target_obj(observer);
    lv_obj_t * label_box = lv_observer_get_user_data(observer);
    int32_t selected = lv_slider_get_value(slider);
    for(int32_t i = 0; i < 4; i++) {
        lv_obj_t * label = lv_obj_get_child(label_box, i);
        lv_obj_set_style_opa(label, i == selected ? LV_OPA_100 : LV_OPA_50, 0);
    }
}

static lv_obj_t * create_slider(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * label_text,
                                lv_image_dsc_t ** img_dsc_pair, lv_subject_t * subject)
{
    lv_obj_t * box = lv_demo_high_res_simple_container_create(parent, true, c->sz->gap[9], LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_top(box, c->sz->gap[5], 0);

    lv_obj_t * label = lv_label_create(box);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_label_set_text_static(label, label_text);

    lv_obj_t * slider = lv_slider_create(box);
    lv_obj_set_size(slider, LV_PCT(100), c->sz->slider_width);
    lv_slider_set_range(slider, 0, 3);
    lv_obj_set_style_bg_color(slider, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(slider, 30, 0);
    lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(slider, c->sz->gap[4], 0);
    lv_obj_set_style_radius(slider, c->sz->gap[4], LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider, lv_color_black(), LV_PART_KNOB);
    lv_obj_set_style_pad_right(slider, -(c->sz->slider_width - 6) / 2 - 8, LV_PART_KNOB);
    lv_obj_set_style_pad_left(slider, -(c->sz->slider_width - 6) / 2 + 8, LV_PART_KNOB);
    lv_obj_set_style_pad_ver(slider, -(c->sz->slider_width - 12) / 2, LV_PART_KNOB);
    lv_obj_add_event_cb(slider, slider_draw_start_cb, LV_EVENT_DRAW_MAIN_BEGIN, NULL);
    lv_obj_add_event_cb(slider, slider_draw_end_cb, LV_EVENT_DRAW_MAIN_END, NULL);

    lv_obj_t * slider_img = lv_image_create(slider);
    lv_obj_align(slider_img, LV_ALIGN_LEFT_MID, 8, 0);

    lv_slider_bind_value(slider, subject);
    lv_subject_add_observer_obj(subject, slider_observer_cb, slider, img_dsc_pair);

    lv_obj_t * label_box = lv_demo_high_res_simple_container_create(box, false, 0, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_width(label_box, LV_PCT(100));
    lv_obj_set_flex_align(label_box, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(label_box, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_style_value_t font;
    lv_style_get_prop(&c->fonts[FONT_LABEL_XS], LV_STYLE_TEXT_FONT, &font);
    int32_t offset = lv_font_get_line_height(font.ptr) + c->sz->slider_width * 13 / 40;
    lv_obj_align_to(label_box, slider, LV_ALIGN_OUT_TOP_MID, 0, -offset);

    static const char * const texts[4] = {"OFF", "LOW", "MIDDLE", "HIGH"};
    for(int32_t i = 0; i < 4; i++) {
        label = lv_label_create(label_box);
        lv_obj_add_style(label, &c->fonts[FONT_LABEL_XS], 0);
        lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
        lv_label_set_text_static(label, texts[i]);
    }

    lv_subject_add_observer_obj(subject, slider_label_observer_cb, slider, label_box);

    return box;
}

static void create_widget5(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets)
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
    lv_label_set_text_static(title_label, "Sensor Controls");
    lv_obj_add_style(title_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_obj_add_style(title_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);

    lv_obj_t * cluster_1 = lv_obj_create(widget);
    lv_obj_remove_style_all(cluster_1);
    lv_obj_set_size(cluster_1, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cluster_1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cluster_1, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(cluster_1, c->sz->gap[9], 0);

    create_widget5_door(c, cluster_1);

    lv_obj_t * lightbulbs_box = create_widget5_lightbulbs(c, cluster_1);
    lv_obj_set_flex_grow(lightbulbs_box, 1);

    lv_obj_t * cluster_2 = lv_obj_create(widget);
    lv_obj_remove_style_all(cluster_2);
    lv_obj_set_size(cluster_2, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cluster_2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cluster_2, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    lv_obj_set_style_pad_column(cluster_2, c->sz->gap[10], 0);

    lv_obj_t * matter_fan_box = create_slider(c, cluster_2, "Matter Fan", &c->imgs[IMG_FAN2], &c->api.subjects.fan_matter);
    lv_obj_set_flex_grow(matter_fan_box, 1);

    create_widget5_zigbee_fan(c, cluster_2);

    lv_obj_t * air_purifier_box = create_slider(c, widget, "Air Purifier", &c->imgs[IMG_WIND],
                                                &c->api.subjects.air_purifier);
    lv_obj_set_width(air_purifier_box, LV_PCT(100));
}

#endif /*LV_USE_DEMO_HIGH_RES*/

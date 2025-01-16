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
static void widget2_slider_released_cb(lv_event_t * e);
static void widget2_slider_locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void create_widget2(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void widget3_play_pause_clicked_cb(lv_event_t * e);
static void create_widget3(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);
static void create_widget4(lv_demo_high_res_ctx_t * c, lv_obj_t * widgets);

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

    lv_demo_high_res_top_margin_create(bg_cont,
                                       c->sz == &lv_demo_high_res_sizes_all[SIZE_SM] ? c->sz->gap[9] : c->sz->gap[10], true, c);

    /* app info */

    lv_obj_t * app_info = lv_demo_high_res_simple_container_create(bg_cont, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_top(app_info, c->sz->gap[10], 0);
    lv_obj_set_style_pad_left(app_info, c->sz->gap[10], 0);

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

static void widget2_slider_released_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    lv_subject_set_int(&c->api.subjects.locked, lv_slider_get_value(slider) >= 100);
}

static void widget2_slider_locked_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * slider = lv_observer_get_target_obj(observer);
    bool locked = lv_subject_get_int(subject);
    lv_slider_set_value(slider, locked ? 100 : 0, LV_ANIM_ON);
    lv_obj_update_flag(slider, LV_OBJ_FLAG_CLICKABLE, !locked);
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

#endif /*LV_USE_DEMO_HIGH_RES*/

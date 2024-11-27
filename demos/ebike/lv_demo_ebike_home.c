/**
 * @file lv_demo_ebike_home.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_ebike.h"
#if LV_USE_DEMO_EBIKE

#include "lv_demo_ebike_private.h"
#include "lv_demo_ebike_home.h"
#include "translations/lv_i18n.h"
#include "../../lvgl_private.h"

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
static lv_obj_t * roller_create(lv_obj_t * parent, const char * opts, lv_roller_mode_t mode);
static lv_obj_t * card_labels_create(lv_obj_t * parent, const char * value, const char * unit, const char * title);
static void roller_anim_timer_cb(lv_timer_t * t);

/**********************
 *  STATIC VARIABLES
 **********************/
/*Subjects used only by the home page to it easier to sync widgets*/
static lv_subject_t ebike_subject_speed_arc;
static lv_subject_t ebike_subject_speed_roller;

static lv_timer_t * roller_anim_timer;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_ebike_home_init(void)
{
    lv_subject_init_int(&ebike_subject_speed_arc, 0);
    lv_subject_init_int(&ebike_subject_speed_roller, 0);
    roller_anim_timer = lv_timer_create(roller_anim_timer_cb, 2000, NULL);
}

void lv_demo_ebike_home_deinit(void)
{
    lv_subject_deinit(&ebike_subject_speed_arc);
    lv_subject_deinit(&ebike_subject_speed_roller);
    lv_timer_delete(roller_anim_timer);
}

void lv_demo_ebike_home_create(lv_obj_t * parent)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(main_cont, LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, 0);

    lv_obj_t * left_cont = left_cont_create(main_cont);
    lv_obj_t * right_cont = right_cont_create(main_cont);

#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_style_flex_cross_place(main_cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_width(left_cont, lv_pct(100));
    lv_obj_set_style_max_width(left_cont, 450, 0);
    lv_obj_set_flex_grow(left_cont, 2);
    lv_obj_set_width(right_cont, lv_pct(100));
    lv_obj_set_flex_grow(right_cont, 1);
#else
    lv_obj_set_size(left_cont, lv_pct(40), lv_pct(100));
    lv_obj_set_style_min_width(left_cont, 220, 0);
    lv_obj_set_size(right_cont, lv_pct(40), lv_pct(100));
    lv_obj_set_style_min_width(right_cont, 150, 0);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void set_subject_exec_cb(void * var, int32_t v)
{
    lv_subject_set_int(var, v);
}

static void roller_anim_timer_cb(lv_timer_t * t)
{
    LV_UNUSED(t);
    int32_t v = lv_rand(0, 90);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, &ebike_subject_speed_arc);
    lv_anim_set_values(&a, lv_subject_get_int(&ebike_subject_speed_arc), v);
    lv_anim_set_duration(&a, 1000);
    lv_anim_set_exec_cb(&a, set_subject_exec_cb);
    lv_anim_start(&a);

    lv_subject_set_int(&ebike_subject_speed_roller, v);
}

static void speed_label_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    int32_t label_v = (int32_t)((lv_uintptr_t)observer->user_data);
    int32_t speed = lv_subject_get_int(subject);

    label_v = LV_ABS(label_v - speed);
    uint32_t zoom = lv_map(label_v, 0, 20, 512, 256);
    lv_obj_set_style_transform_scale(label, zoom, 0);
}

static void speed_roller_10_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * roller = lv_observer_get_target_obj(observer);
    int32_t speed = lv_subject_get_int(subject);
    lv_roller_set_selected(roller, speed / 10, LV_ANIM_ON);
}

static void speed_roller_1_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * roller = lv_observer_get_target_obj(observer);
    int32_t speed = lv_subject_get_int(subject);

    lv_roller_set_selected(roller, speed, LV_ANIM_ON);
}

static lv_obj_t * info_box_create(lv_obj_t * parent, const void * icon, const char * big_text,  const char * small_text)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_set_height(main_cont, lv_pct(100));
    lv_obj_set_flex_grow(main_cont, 1);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(main_cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(main_cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_size(main_cont, lv_pct(100), LV_SIZE_CONTENT);

    lv_obj_t * img;
    img = lv_image_create(main_cont);
    lv_image_set_src(img, icon);

    lv_obj_center(main_cont);
    lv_obj_t * label;
    label = lv_label_create(main_cont);
    lv_label_set_text(label, big_text);
    lv_obj_set_style_text_font(label, EBIKE_FONT_MEDIUM, 0);

    label = lv_label_create(main_cont);
    lv_label_set_text(label, small_text);
    lv_obj_set_style_text_font(label, EBIKE_FONT_SMALL, 0);
    lv_obj_set_style_margin_left(label, 32, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    return main_cont;
}

static lv_obj_t * left_cont_create(lv_obj_t * parent)
{
    lv_obj_t * left_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(left_cont, 0, 0);
    lv_obj_remove_flag(left_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * scale = lv_image_create(left_cont);
#if LV_DEMO_EBIKE_PORTRAIT
    LV_IMAGE_DECLARE(img_ebike_scale_large);
    lv_image_set_src(scale, &img_ebike_scale_large);
    lv_obj_align(scale, LV_ALIGN_LEFT_MID, 0, 0);
#else
    LV_IMAGE_DECLARE(img_ebike_scale);
    lv_image_set_src(scale, &img_ebike_scale);
    lv_obj_align(scale, LV_ALIGN_LEFT_MID, 0, 0);
#endif

    lv_obj_t * arc = lv_arc_create(left_cont);

#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_size(arc, 660, 660);
    lv_obj_align(arc, LV_ALIGN_LEFT_MID, 82, 0);
#else
    lv_obj_set_size(arc, 440, 440);
    lv_obj_align(arc, LV_ALIGN_LEFT_MID, 52, 0);
#endif
    lv_obj_set_style_arc_width(arc, 20, 0);
    lv_obj_set_style_arc_width(arc, 20, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(arc, LV_OPA_0, LV_PART_KNOB);
    lv_obj_set_style_arc_opa(arc, LV_OPA_0, 0);
    lv_obj_set_style_arc_color(arc, EBIKE_COLOR_TURQUOISE, LV_PART_INDICATOR);
    lv_arc_set_bg_angles(arc, 0, 90);
    lv_arc_set_rotation(arc, 130);
    lv_arc_set_range(arc, -20, 110);
    lv_arc_bind_value(arc, &ebike_subject_speed_arc);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE);

    uint32_t i;
    for(i = 0; i < 5; i++) {
        lv_obj_t * obj = lv_obj_create(left_cont);
        lv_obj_set_style_bg_opa(obj, LV_OPA_0, 0);

#if LV_DEMO_EBIKE_PORTRAIT
        lv_obj_set_size(obj, 60, 80);
        lv_obj_align(obj, LV_ALIGN_LEFT_MID, -10, 0);
        lv_obj_set_style_transform_pivot_x(obj, 390, 0);
        lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
        lv_obj_set_style_transform_rotation(obj, -240 + i * 135, 0);
#else
        lv_obj_set_size(obj, 40, 50);
        lv_obj_align(obj, LV_ALIGN_LEFT_MID, -10, 0);
        lv_obj_set_style_transform_pivot_x(obj, 260, 0);
        lv_obj_set_style_transform_pivot_y(obj, lv_pct(50), 0);
        lv_obj_set_style_transform_rotation(obj, -260 + i * 150, 0);
#endif

        lv_obj_t * label = lv_label_create(obj);
        lv_obj_align(label, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_label_set_text_fmt(label, "%d", (i + 1) * 20);
        lv_obj_set_style_text_font(label, EBIKE_FONT_MEDIUM, 0);
        lv_obj_set_style_transform_pivot_x(label, lv_pct(100), 0);
        lv_obj_set_style_transform_pivot_y(label, lv_pct(50), 0);
        lv_subject_add_observer_obj(&ebike_subject_speed_arc, speed_label_observer_cb, label,
                                    (void *)((lv_uintptr_t)((i + 1) * 20)));
    }

    lv_obj_t * dashboard_center_cont = lv_obj_create(left_cont);
    lv_obj_set_style_bg_opa(dashboard_center_cont, 0, 0);
#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_align(dashboard_center_cont, LV_ALIGN_LEFT_MID, 32, 0);
    lv_obj_set_size(dashboard_center_cont, lv_pct(80), 240);
#else
    lv_obj_align(dashboard_center_cont, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_size(dashboard_center_cont, lv_pct(100), 240);
#endif
    lv_obj_set_style_pad_left(dashboard_center_cont, 110, 0);
    lv_obj_remove_flag(dashboard_center_cont, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * top_cont = lv_obj_create(dashboard_center_cont);
    lv_obj_set_style_bg_opa(top_cont, 0, 0);
    lv_obj_set_size(top_cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(top_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    LV_IMAGE_DECLARE(img_ebike_arrow_left);
    lv_obj_t * icon1 = lv_image_create(top_cont);
    lv_image_set_src(icon1, &img_ebike_arrow_left);

    LV_IMAGE_DECLARE(img_ebike_lamp);
    lv_obj_t * icon2 = lv_image_create(top_cont);
    lv_image_set_src(icon2, &img_ebike_lamp);

    LV_IMAGE_DECLARE(img_ebike_arrow_right);
    lv_obj_t * icon3 = lv_image_create(top_cont);
    lv_image_set_src(icon3, &img_ebike_arrow_right);


    lv_obj_t * roller_cont = lv_obj_create(dashboard_center_cont);
    lv_obj_set_size(roller_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(roller_cont, 0, 0);
    lv_obj_align(roller_cont, LV_ALIGN_CENTER, 0, 0);

    const char * opts1 = "0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
    lv_obj_t * roller_1 = roller_create(roller_cont, opts1, LV_ROLLER_MODE_NORMAL);
    lv_obj_align(roller_1, LV_ALIGN_LEFT_MID, 0, 0);
    lv_subject_add_observer_obj(&ebike_subject_speed_roller, speed_roller_10_observer_cb, roller_1, NULL);

    const char * opts2 =
        "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n0\n1\n2\n3\n4\n5\n6\n7\n8\n9";
    lv_obj_t * roller_10 = roller_create(roller_cont, opts2, LV_ROLLER_MODE_NORMAL);
    lv_obj_align(roller_10, LV_ALIGN_LEFT_MID, 50, 0);
    lv_subject_add_observer_obj(&ebike_subject_speed_roller, speed_roller_1_observer_cb, roller_10, NULL);

    lv_obj_t * bottom_cont = lv_obj_create(left_cont);
    lv_obj_set_style_bg_opa(bottom_cont, 0, 0);
    lv_obj_set_size(bottom_cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(bottom_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_left(bottom_cont, 110, 0);
    lv_obj_align(bottom_cont, LV_ALIGN_LEFT_MID, 0, 105);

    LV_IMAGE_DECLARE(img_ebike_clock);
    info_box_create(bottom_cont, &img_ebike_clock, " 03:58 PM", _("March 29"));

    return left_cont;
}

static lv_obj_t * roller_create(lv_obj_t * parent, const char * opts, lv_roller_mode_t mode)
{
    LV_FONT_DECLARE(font_ebike_130);
    lv_obj_t * roller = lv_roller_create(parent);
    lv_obj_set_style_text_font(roller, &font_ebike_130, 0);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, 0);
    lv_roller_set_options(roller, opts, mode);
    lv_roller_set_visible_row_count(roller, 1);
    lv_obj_set_width(roller, 50);
    lv_obj_set_style_anim_duration(roller, 1000, 0);
    lv_obj_set_style_bg_opa(roller, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(roller, 0, LV_PART_SELECTED);

    return roller;
}


static lv_obj_t * card_labels_create(lv_obj_t * parent, const char * value, const char * unit, const char * title)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_opa(main_cont, 0, 0);

    lv_obj_t * center_cont = lv_obj_create(main_cont);
    lv_obj_set_flex_flow(center_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(center_cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(center_cont, LV_FLEX_ALIGN_START, 0);
    lv_obj_set_size(center_cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(center_cont, 0, 0);

    lv_obj_center(center_cont);
    lv_obj_t * label;
    label = lv_label_create(center_cont);
    lv_label_set_text(label, value);
    lv_obj_set_style_text_font(label, EBIKE_FONT_LARGE, 0);

    label = lv_label_create(center_cont);
    lv_label_set_text(label, unit);
    lv_obj_set_style_text_font(label, EBIKE_FONT_SMALL, 0);

    label = lv_label_create(main_cont);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_font(label, EBIKE_FONT_SMALL, 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -4);

    return main_cont;
}

static void bullet_scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * main_cont = lv_event_get_target(e);
    lv_obj_t * bullet_cont = lv_event_get_user_data(e);
    int32_t scroll_x = lv_obj_get_scroll_x(main_cont);
    int32_t w = lv_obj_get_content_width(main_cont);
    int32_t idx = scroll_x / w;

    uint32_t i;
    uint32_t cnt = lv_obj_get_child_count(bullet_cont);
    for(i = 0; i < cnt; i++) {
        lv_obj_remove_state(lv_obj_get_child(bullet_cont, i), LV_STATE_CHECKED);
    }
    lv_obj_add_state(lv_obj_get_child(bullet_cont, idx), LV_STATE_CHECKED);
}

static lv_obj_t * bullets_create(lv_obj_t * parent)
{
    static bool inited = false;
    static const lv_style_prop_t props[] = {LV_STYLE_BG_OPA, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_PROP_INV};
    static lv_style_transition_dsc_t dsc;

    if(!inited) {
        lv_style_transition_dsc_init(&dsc, props, lv_anim_path_linear, 200, 0, NULL);
        inited = true;
    }

    uint32_t cnt = lv_obj_get_child_count(parent);

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(cont, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_align(cont, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_pad_all(cont, 4, 0);

    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_gap(cont, 8, 0);

    uint32_t i;
    for(i = 0; i < cnt; i++) {
        lv_obj_t * bullet = lv_obj_create(cont);
        lv_obj_set_size(bullet, 5, 5);
        lv_obj_set_style_transition(bullet, &dsc, 0);
        lv_obj_set_style_radius(bullet, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(bullet, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(bullet, LV_OPA_50, 0);
        lv_obj_set_style_bg_opa(bullet, LV_OPA_COVER, LV_STATE_CHECKED);
        lv_obj_set_style_transform_width(bullet, 2, LV_STATE_CHECKED);
        lv_obj_set_style_transform_height(bullet, 2, LV_STATE_CHECKED);
    }

    lv_obj_add_state(lv_obj_get_child(cont, 0), LV_STATE_CHECKED);
    lv_obj_add_event_cb(parent, bullet_scroll_event_cb, LV_EVENT_SCROLL_END, cont);
    return cont;
}


static lv_obj_t * right_cont_create(lv_obj_t * parent)
{
    lv_obj_t * right_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(right_cont, 0, 0);

    lv_obj_set_flex_flow(right_cont, LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_ROW : LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_ver(right_cont, 12, 0);
    lv_obj_set_style_pad_hor(right_cont, 8, 0);
    lv_obj_set_style_pad_gap(right_cont, 8, 0);

    lv_obj_t * battery = lv_bar_create(right_cont);
    lv_obj_set_size(battery, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(battery, 1);
    lv_obj_set_flex_flow(battery, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_snap_x(battery, LV_SCROLL_SNAP_CENTER);
    lv_obj_add_flag(battery, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_color(battery, EBIKE_COLOR_LIME, 0);
    lv_obj_set_style_border_width(battery, 4, 0);
    lv_obj_set_style_bg_opa(battery, 0, 0);
    lv_obj_set_style_pad_all(battery, 10, 0);
    lv_obj_set_style_bg_opa(battery, 0, 0);
    lv_obj_set_style_radius(battery, 12, 0);
    lv_obj_set_style_bg_color(battery, EBIKE_COLOR_LIME, LV_PART_INDICATOR);
    lv_obj_set_style_pad_column(battery, 16, 0);
    lv_obj_set_style_text_color(battery, lv_color_black(), 0);
    lv_obj_set_style_radius(battery, 6, LV_PART_INDICATOR);
    lv_bar_set_value(battery, 82, LV_ANIM_ON);
    lv_obj_set_scrollbar_mode(battery, LV_SCROLLBAR_MODE_OFF);

    card_labels_create(battery, "82", "%", _("Battery"));
    card_labels_create(battery, "29:31", "", _("Battery"));

    bullets_create(battery);

    lv_obj_t * dist = lv_obj_create(right_cont);
    lv_obj_set_size(dist, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_ver(dist, 14, 0);
    lv_obj_set_style_pad_hor(dist, 4, 0);
    lv_obj_set_flex_grow(dist, 1);
    lv_obj_set_style_radius(dist, 12, 0);
    lv_obj_set_style_bg_color(dist, EBIKE_COLOR_TURQUOISE, 0);
    lv_obj_set_style_text_color(dist, lv_color_black(), 0);
    lv_obj_set_style_pad_column(dist, 16, 0);
    lv_obj_set_flex_flow(dist, LV_FLEX_FLOW_ROW);
    lv_obj_set_scroll_snap_x(dist, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(dist, LV_SCROLLBAR_MODE_OFF);

    card_labels_create(dist, "16.4", "km", _("Distance today"));
    card_labels_create(dist, "20.1", "km/h", _("Speed today"));
    card_labels_create(dist, "43:12", "", _("Time today"));

    bullets_create(dist);

    return right_cont;
}

#endif /*LV_USE_DEMO_EBIKE*/

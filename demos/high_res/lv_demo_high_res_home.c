/**
 * @file lv_demo_high_res_home.c
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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef void (*app_cb_t)(lv_obj_t * base_obj);

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void theme_observer_accent_span_cb(lv_observer_t * observer, lv_subject_t * subject);
static void app_card_click_cb(lv_event_t * e);
static void app_card_create(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text,
                            const lv_image_dsc_t * icon_img_dsc, app_cb_t app_cb);
static void set_light_theme_event_cb(lv_event_t * e);
static void set_dark_theme_event_cb(lv_event_t * e);
static void time_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void date_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void hi_lo_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_home(lv_obj_t * base_obj)
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
    lv_obj_set_flex_flow(bg_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);

    /* top margin */

    lv_demo_high_res_top_margin_create(bg_cont,
                                       c->sz == &lv_demo_high_res_sizes_all[SIZE_SM] ? c->sz->gap[9] : c->sz->gap[10], false, c);

    /* info area */

    lv_obj_t * info_area = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(info_area);
    lv_obj_set_width(info_area, LV_PCT(100));
    lv_obj_set_style_pad_left(info_area, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(info_area, c->sz->gap[10], 0);
    lv_obj_set_flex_grow(info_area, 1);
    lv_obj_set_flex_flow(info_area, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(info_area, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * date_and_time = lv_demo_high_res_simple_container_create(info_area, true, c->sz->gap[4],
                                                                        LV_FLEX_ALIGN_START);

    lv_obj_t * date = lv_spangroup_create(date_and_time);
    lv_obj_add_style(date, &c->fonts[FONT_HEADING_MD], 0);
    lv_obj_add_style(date, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_spangroup_new_span(date);
    lv_span_t * day_and_month = lv_spangroup_new_span(date);
    lv_subject_add_observer_obj(&c->th, theme_observer_accent_span_cb, date, day_and_month);
    lv_subject_add_observer_obj(&c->subject_groups.date.group, date_observer_cb, date, c);

    lv_obj_t * time = lv_spangroup_create(date_and_time);
    lv_obj_add_style(time, &c->fonts[FONT_HEADING_XXL], 0);
    lv_obj_add_style(time, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_spangroup_new_span(time);
    lv_span_t * minute = lv_spangroup_new_span(time);
    lv_subject_add_observer_obj(&c->th, theme_observer_accent_span_cb, time, minute);
    lv_subject_add_observer_obj(&c->subject_groups.time.group, time_observer_cb, time, c);

    lv_obj_t * weather = lv_demo_high_res_simple_container_create(info_area, false, 50, LV_FLEX_ALIGN_END);
    lv_obj_t * weather_left = lv_demo_high_res_simple_container_create(weather, true, c->sz->gap[2], LV_FLEX_ALIGN_START);

    lv_obj_t * weather_img = lv_image_create(weather_left);
    lv_image_set_src(weather_img, c->imgs[IMG_WEATHER]);
    lv_obj_add_style(weather_img, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);

    lv_obj_t * weather_left_bottom = lv_demo_high_res_simple_container_create(weather_left, true, c->sz->gap[3],
                                                                              LV_FLEX_ALIGN_START);

    lv_obj_t * weather_label = lv_label_create(weather_left_bottom);
    lv_label_set_text_static(weather_label, "Cloudy");
    lv_obj_add_style(weather_label, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(weather_label, &c->fonts[FONT_LABEL_MD], 0);

    lv_obj_t * weather_hi_lo_label = lv_label_create(weather_left_bottom);
    lv_obj_add_style(weather_hi_lo_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(weather_hi_lo_label, &c->fonts[FONT_LABEL_MD], 0);
    lv_subject_add_observer_obj(&c->temperature_units_are_celsius, hi_lo_observer_cb, weather_hi_lo_label, NULL);

    lv_obj_t * weather_right = lv_demo_high_res_simple_container_create(weather, true, c->sz->gap[2], LV_FLEX_ALIGN_CENTER);

    lv_obj_t * weather_temperature_label = lv_label_create(weather_right);
    lv_obj_add_style(weather_temperature_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(weather_temperature_label, &c->fonts[FONT_HEADING_XL], 0);
    lv_demo_high_res_label_bind_temperature(weather_temperature_label, &c->api.subjects.temperature_outdoor, c);

    lv_obj_t * weather_temperature_location_label = lv_label_create(weather_right);
    lv_label_set_text_static(weather_temperature_location_label, "Outdoor");
    lv_obj_add_style(weather_temperature_location_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(weather_temperature_location_label, &c->fonts[FONT_LABEL_MD], 0);

    /* apps */

    lv_obj_t * apps = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(apps);
    lv_obj_set_size(apps, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_bottom(apps, c->sz->gap[10], 0);
    lv_obj_set_style_pad_left(apps, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(apps, c->sz->gap[10], 0);
    lv_obj_set_style_border_side(apps, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_border_color(apps, lv_color_black(), 0);
    lv_obj_set_style_border_width(apps, 1, 0);
    lv_obj_set_style_border_opa(apps, 8 * 255 / 100, 0);
    lv_obj_set_flex_flow(apps, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(apps, c->sz->gap[6], 0);
    app_card_create(c, apps, "EV Charging", c->imgs[IMG_EV_CHARGING_APP_ICON], lv_demo_high_res_app_ev_charging);
    app_card_create(c, apps, "Smart Home", c->imgs[IMG_SMART_HOME_APP_ICON], lv_demo_high_res_app_smart_home);
    app_card_create(c, apps, "Smart Meter", c->imgs[IMG_SMART_METER_APP_ICON], lv_demo_high_res_app_smart_meter);
    app_card_create(c, apps, "Thermostat", c->imgs[IMG_THERMOSTAT_APP_ICON], lv_demo_high_res_app_thermostat);
    app_card_create(c, apps, "About", c->imgs[IMG_ABOUT_APP_ICON], lv_demo_high_res_app_about);

    /* bottom_margin */

    lv_obj_t * bottom_margin = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(bottom_margin);
    lv_obj_set_size(bottom_margin, LV_PCT(100), c->sz->home_bottom_margin_height);
    lv_obj_set_flex_flow(bottom_margin, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bottom_margin, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_left(bottom_margin, c->sz->gap[10], 0);
    lv_obj_set_style_pad_right(bottom_margin, c->sz->gap[10], 0);

    lv_obj_t * logo = lv_image_create(bottom_margin);
    lv_obj_set_height(logo, c->sz->icon[1]);
    lv_image_set_inner_align(logo, LV_IMAGE_ALIGN_STRETCH);
    lv_image_set_src(logo, c->logo_path);
    int32_t scale = lv_image_get_scale_y(logo);
    lv_image_set_inner_align(logo, LV_IMAGE_ALIGN_DEFAULT);
    lv_image_set_scale(logo, scale);

    lv_obj_t * theme_selector = lv_demo_high_res_simple_container_create(bottom_margin, false, c->sz->gap[4],
                                                                         LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(theme_selector, c->sz->gap[1], 0); /* for icon outline */
    lv_obj_set_style_pad_right(bottom_margin, lv_obj_get_style_pad_right(bottom_margin, 0) - c->sz->gap[1],
                               0); /* compensate parent `bottom_margin` */

    lv_obj_t * theme_label = lv_label_create(theme_selector);
    lv_label_set_text_static(theme_label, "Theme:");
    lv_obj_add_style(theme_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(theme_label, &c->fonts[FONT_LABEL_SM], 0);

    lv_obj_t * light_theme_icon = lv_image_create(theme_selector);
    lv_obj_set_style_radius(light_theme_icon, LV_COORD_MAX, 0);
    lv_obj_set_style_outline_pad(light_theme_icon, -1, 0); /* FIXME */
    if(((const lv_demo_high_res_theme_t *) lv_subject_get_pointer(&c->th)) == &lv_demo_high_res_theme_light)
        lv_obj_set_style_outline_width(light_theme_icon, 2, 0);
    lv_obj_add_style(light_theme_icon, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_OBJ], 0);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, light_theme_icon,
                                &c->imgs[IMG_LIGHT_LIGHT_THEME_ICON]);

    lv_obj_t * dark_theme_icon = lv_image_create(theme_selector);
    lv_obj_set_style_radius(dark_theme_icon, LV_COORD_MAX, 0);
    lv_obj_set_style_outline_pad(dark_theme_icon, -1, 0); /* FIXME */
    if(((const lv_demo_high_res_theme_t *) lv_subject_get_pointer(&c->th)) == &lv_demo_high_res_theme_dark)
        lv_obj_set_style_outline_width(dark_theme_icon, 2, 0);
    lv_obj_add_style(dark_theme_icon, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_OBJ], 0);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, dark_theme_icon,
                                &c->imgs[IMG_LIGHT_DARK_THEME_ICON]);

    lv_obj_add_flag(light_theme_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(light_theme_icon, dark_theme_icon);
    lv_obj_add_event_cb(light_theme_icon, set_light_theme_event_cb, LV_EVENT_CLICKED, c);
    lv_obj_add_flag(dark_theme_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_user_data(dark_theme_icon, light_theme_icon);
    lv_obj_add_event_cb(dark_theme_icon, set_dark_theme_event_cb, LV_EVENT_CLICKED, c);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void theme_observer_accent_span_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_span_t * span = lv_observer_get_user_data(observer);
    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(subject);

    lv_style_set_text_color(&span->style, th->accent);
}

static void app_card_click_cb(lv_event_t * e)
{
    lv_obj_t * app_card = lv_event_get_target_obj(e);
    app_cb_t app_cb = (app_cb_t)((uintptr_t)lv_event_get_user_data(e));

    lv_obj_t * base_obj = lv_obj_get_parent(lv_obj_get_parent(lv_obj_get_parent(app_card)));
    lv_obj_clean(base_obj);
    app_cb(base_obj);
}

static void app_card_create(lv_demo_high_res_ctx_t * c, lv_obj_t * parent, const char * text,
                            const lv_image_dsc_t * icon_img_dsc, app_cb_t app_cb)
{
    if(icon_img_dsc == NULL) return;
    lv_obj_t * app_card = lv_obj_create(parent);
    lv_obj_remove_style_all(app_card);
    lv_obj_set_size(app_card, c->sz->card_long_edge, c->sz->card_short_edge);
    lv_obj_set_style_radius(app_card, c->sz->gap[6], 0);
    lv_obj_set_style_bg_opa(app_card, 16 * 255 / 100, 0);
    lv_obj_set_style_pad_all(app_card, c->sz->gap[7], 0);
    lv_obj_add_style(app_card, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_OBJ], 0);
    lv_obj_add_event_cb(app_card, app_card_click_cb, LV_EVENT_CLICKED, (void *)((lv_uintptr_t)app_cb));

    lv_obj_t * label = lv_label_create(app_card);
    lv_label_set_text(label, text);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_MD], 0);

    lv_obj_t * icon = lv_image_create(app_card);
    lv_obj_set_size(icon, icon_img_dsc->header.w, icon_img_dsc->header.h);
    lv_image_set_inner_align(icon, LV_IMAGE_ALIGN_CENTER);
    lv_image_set_src(icon, icon_img_dsc);
    lv_obj_set_style_bg_opa(icon, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(icon, lv_color_white(), 0);
    lv_obj_set_style_radius(icon, LV_COORD_MAX, 0);
    lv_obj_add_style(icon, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_A8_IMG], 0);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_EVENT_BUBBLE);
}

static void set_light_theme_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_obj_set_style_outline_width(obj, 2, 0);
    obj = lv_obj_get_user_data(obj);
    lv_obj_set_style_outline_width(obj, 0, 0);

    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);

    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(&c->th);
    if(th == &lv_demo_high_res_theme_light) return;

    lv_subject_set_pointer(&c->th, (void *)&lv_demo_high_res_theme_light);
}

static void set_dark_theme_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_obj_set_style_outline_width(obj, 2, 0);
    obj = lv_obj_get_user_data(obj);
    lv_obj_set_style_outline_width(obj, 0, 0);

    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);

    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(&c->th);
    if(th == &lv_demo_high_res_theme_dark) return;

    lv_subject_set_pointer(&c->th, (void *)&lv_demo_high_res_theme_dark);
}

static void time_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * spangroup = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);

    char buf[16];

    lv_snprintf(buf, sizeof(buf), "%02"LV_PRId32, lv_subject_get_int(&c->api.subjects.hour));
    lv_span_set_text(lv_spangroup_get_child(spangroup, 0), buf);

    lv_snprintf(buf, sizeof(buf), ":%02"LV_PRId32, lv_subject_get_int(&c->api.subjects.minute));
    lv_span_set_text(lv_spangroup_get_child(spangroup, 1), buf);
}

static void date_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * spangroup = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);

    char buf[32];

    lv_snprintf(buf, sizeof(buf), "%s, ", lv_subject_get_pointer(&c->api.subjects.week_day_name));
    lv_span_set_text(lv_spangroup_get_child(spangroup, 0), buf);
    lv_snprintf(buf, sizeof(buf), "%"PRId32" %s", lv_subject_get_int(&c->api.subjects.month_day),
                lv_subject_get_pointer(&c->api.subjects.month_name));
    lv_span_set_text(lv_spangroup_get_child(spangroup, 1), buf);
}

static void hi_lo_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * weather_hi_lo_label = lv_observer_get_target_obj(observer);
    lv_label_set_text_static(weather_hi_lo_label,
                             lv_subject_get_int(subject) ? "H: 19\xc2\xb0   L: 10\xc2\xb0" : "H: 66.2\xc2\xb0   L: 50\xc2\xb0");
}

#endif /*LV_USE_DEMO_HIGH_RES*/

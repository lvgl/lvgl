/**
 * @file lv_demo_high_res_top_margin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"
#include "../../src/widgets/line/lv_line.h"
#include "../../src/display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void logout_cb(lv_event_t * e);
static lv_obj_t * create_icon(lv_obj_t * parent, lv_subject_t * subject, lv_image_dsc_t ** img_dsc_pair,
                              lv_demo_high_res_ctx_t * c);
static void delete_modal_cb(lv_event_t * e);
static void icon_clicked_cb(lv_event_t * e);
static void sys_layer_clicked_cb(lv_event_t * e);
static void wifi_ssid_ip_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_wifi(lv_obj_t * parent, lv_demo_high_res_ctx_t * c);
static void wifi_icon_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static lv_obj_t * create_perfmon(lv_obj_t * parent, lv_demo_high_res_ctx_t * c);
#if LV_USE_PERF_MONITOR
    static void perfmon_data_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif
static lv_obj_t * create_settings(lv_obj_t * parent, lv_demo_high_res_ctx_t * c);
static lv_obj_t * create_setting_label_cont(lv_obj_t * parent, const char * text, lv_demo_high_res_ctx_t * c);
static void setting_clicked_cb(lv_event_t * e);
static void date_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void time_observer_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(*arr))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_demo_high_res_top_margin_create(lv_obj_t * parent, int32_t pad_hor, bool show_time,
                                              lv_demo_high_res_ctx_t * c)
{
    if(!c->top_margin_subjects_are_init) {
        c->top_margin_subjects_are_init = true;
        lv_subject_init_int(&c->top_margin_wifi_subject, 0);
        lv_subject_init_int(&c->top_margin_health_subject, 0);
        lv_subject_init_int(&c->top_margin_setting_subject, 0);
    }

    lv_obj_t * top_margin = lv_obj_create(parent);
    lv_obj_remove_style_all(top_margin);
    lv_obj_set_size(top_margin, LV_PCT(100), c->sz->gap[10]);
    lv_obj_set_style_pad_hor(top_margin, pad_hor, 0);
    lv_obj_set_flex_flow(top_margin, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_margin, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    if(show_time) {
        lv_obj_t * date_label = lv_label_create(top_margin);
        lv_obj_add_style(date_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
        lv_obj_add_style(date_label, &c->fonts[FONT_LABEL_SM], 0);
        lv_subject_add_observer_obj(&c->subject_groups.date.group, date_observer_cb, date_label, c);

        lv_obj_t * time_label = lv_label_create(top_margin);
        lv_obj_add_style(time_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
        lv_obj_add_style(time_label, &c->fonts[FONT_LABEL_SM], 0);
        lv_subject_add_observer_obj(&c->subject_groups.time.group, time_observer_cb, time_label, c);
    }
    else {
        lv_obj_t * logout_icon = lv_image_create(top_margin);
        lv_image_set_src(logout_icon, c->imgs[IMG_LOGOUT_ICON]);
        lv_obj_add_style(logout_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);
        lv_obj_add_flag(logout_icon, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_event_cb(logout_icon, logout_cb, LV_EVENT_CLICKED, c);
    }

    lv_obj_t * top_margin_right_cluster = lv_demo_high_res_simple_container_create(top_margin, false, c->sz->gap[6],
                                                                                   LV_FLEX_ALIGN_CENTER);

    lv_obj_t * wifi_icon = create_icon(top_margin_right_cluster, &c->top_margin_wifi_subject, NULL, c);
    lv_image_set_src(wifi_icon, c->imgs[IMG_WIFI_ICON]);
    lv_subject_add_observer_obj(&c->subject_groups.wifi.group, wifi_icon_observer_cb, wifi_icon, c);
    lv_obj_t * wifi = create_wifi(lv_layer_sys(), c);
    lv_obj_bind_flag_if_eq(wifi, &c->top_margin_wifi_subject, LV_OBJ_FLAG_HIDDEN, 0);
    lv_obj_add_event_cb(wifi_icon, delete_modal_cb, LV_EVENT_DELETE, wifi);

    lv_obj_t * health_icon = create_icon(top_margin_right_cluster, &c->top_margin_health_subject, &c->imgs[IMG_HEALTH_ICON],
                                         c);
    lv_image_set_src(health_icon, c->imgs[lv_subject_get_int(&c->top_margin_health_subject) ? IMG_HEALTH_ICON_BOLD :
                                          IMG_HEALTH_ICON]);
    lv_obj_t * perfmon = create_perfmon(lv_layer_sys(), c);
    lv_obj_bind_flag_if_eq(perfmon, &c->top_margin_health_subject, LV_OBJ_FLAG_HIDDEN, 0);
    lv_obj_add_event_cb(health_icon, delete_modal_cb, LV_EVENT_DELETE, perfmon);

    lv_obj_t * settings_icon = create_icon(top_margin_right_cluster, &c->top_margin_setting_subject,
                                           &c->imgs[IMG_SETTING_ICON], c);
    lv_image_set_src(settings_icon, c->imgs[lv_subject_get_int(&c->top_margin_setting_subject) ? IMG_SETTING_ICON_BOLD :
                                            IMG_SETTING_ICON]);
    lv_obj_t * settings = create_settings(lv_layer_sys(), c);
    lv_obj_bind_flag_if_eq(settings, &c->top_margin_setting_subject, LV_OBJ_FLAG_HIDDEN, 0);
    lv_obj_add_event_cb(settings_icon, delete_modal_cb, LV_EVENT_DELETE, settings);

    lv_obj_update_layout(top_margin_right_cluster);
    lv_obj_align_to(wifi, wifi_icon, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    lv_obj_align_to(perfmon, health_icon, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);
    lv_obj_align_to(settings, settings_icon, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 0);

    return top_margin;
}

void lv_demo_high_res_top_margin_deinit_subjects(lv_demo_high_res_ctx_t * c)
{
    if(!c->top_margin_subjects_are_init) return;
    c->top_margin_subjects_are_init = false;
    lv_subject_deinit(&c->top_margin_wifi_subject);
    lv_subject_deinit(&c->top_margin_health_subject);
    lv_subject_deinit(&c->top_margin_setting_subject);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void logout_cb(lv_event_t * e)
{
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    if(c->exit_cb) c->exit_cb(&c->api);
}

static lv_obj_t * create_icon(lv_obj_t * parent, lv_subject_t * subject, lv_image_dsc_t ** img_dsc_pair,
                              lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * icon = lv_image_create(parent);
    lv_obj_add_style(icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);
    subject->user_data = icon;
    lv_obj_set_user_data(icon, img_dsc_pair);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(icon, icon_clicked_cb, LV_EVENT_CLICKED, c);
    return icon;
}

static void delete_modal_cb(lv_event_t * e)
{
    if(lv_layer_sys() == NULL) return;
    lv_obj_t * modal = lv_event_get_user_data(e);
    lv_obj_delete(modal);
}

static void icon_clicked_cb(lv_event_t * e)
{
    lv_obj_t * clicked_icon = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    lv_subject_t * icon_subjects[] = {
        &c->top_margin_wifi_subject,
        &c->top_margin_health_subject,
        &c->top_margin_setting_subject
    };
    for(int32_t i = 0; i < (int32_t)ARRAY_LEN(icon_subjects); i++) {
        lv_subject_t * subject = icon_subjects[i];
        lv_obj_t * icon = subject->user_data;
        lv_image_dsc_t ** pair = lv_obj_get_user_data(icon);
        int32_t value = icon == clicked_icon && !lv_subject_get_int(subject);
        lv_subject_set_int(subject, value);
        if(pair) lv_image_set_src(icon, pair[value]);
        if(value) {
            lv_obj_add_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);
            lv_obj_add_event_cb(lv_layer_sys(), sys_layer_clicked_cb, LV_EVENT_CLICKED, clicked_icon);
        }
    }
}

static void sys_layer_clicked_cb(lv_event_t * e)
{
    lv_obj_remove_event_cb(lv_layer_sys(), sys_layer_clicked_cb);
    lv_obj_remove_flag(lv_layer_sys(), LV_OBJ_FLAG_CLICKABLE);

    lv_obj_t * relevant_icon = lv_event_get_user_data(e);
    lv_obj_send_event(relevant_icon, LV_EVENT_CLICKED, NULL);
}

static void wifi_ssid_ip_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    const char * ssid_ip = lv_observer_get_user_data(observer);
    const char * ssid_ip_value = lv_subject_get_pointer(subject);
    if(ssid_ip_value) {
        lv_label_set_text_fmt(label, "%s: %s", ssid_ip, ssid_ip_value);
    }
    else {
        lv_label_set_text_fmt(label, "%s: (offline)", ssid_ip);
    }
}

static lv_obj_t * create_wifi(lv_obj_t * parent, lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * settings = lv_obj_create(parent);
    lv_obj_remove_style_all(settings);
    lv_obj_add_flag(settings, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_bg_opa(settings, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(settings, c->sz->gap[3], 0);
    lv_obj_set_size(settings, c->sz->settings_panel_width, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(settings, c->sz->gap[7], 0);
    lv_obj_add_style(settings, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_OBJ], 0);

    lv_obj_t * cont = lv_demo_high_res_simple_container_create(settings, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_set_width(cont, LV_PCT(100));

    lv_obj_t * ssid = lv_label_create(cont);
    lv_obj_add_style(ssid, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(ssid, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_subject_add_observer_obj(&c->api.subjects.wifi_ssid, wifi_ssid_ip_observer_cb, ssid, (void *)"SSID");
    lv_obj_t * ip = lv_label_create(cont);
    lv_obj_add_style(ip, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(ip, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_subject_add_observer_obj(&c->api.subjects.wifi_ip, wifi_ssid_ip_observer_cb, ip, (void *)"IP");

    return settings;
}

static void wifi_icon_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * wifi_icon = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);
    lv_obj_set_style_opa(wifi_icon, lv_subject_get_pointer(&c->api.subjects.wifi_ssid)
                         || lv_subject_get_pointer(&c->api.subjects.wifi_ip)
                         ? LV_OPA_COVER : LV_OPA_50, 0);
}

static lv_obj_t * create_perfmon(lv_obj_t * parent, lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * perfmon = lv_obj_create(parent);
    lv_obj_add_flag(perfmon, LV_OBJ_FLAG_FLOATING);

    lv_obj_set_style_border_opa(perfmon, LV_OPA_TRANSP, 0);
    lv_obj_set_style_radius(perfmon, c->sz->gap[3], 0);
    lv_obj_set_size(perfmon, c->sz->health_panel_width, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(perfmon, c->sz->gap[7], 0);
    lv_obj_add_style(perfmon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_OBJ], 0);

    lv_obj_t * cont = lv_demo_high_res_simple_container_create(perfmon, true, 0, LV_FLEX_ALIGN_START);
    lv_obj_set_width(cont, LV_PCT(100));
    lv_obj_t * lab1 = lv_label_create(cont);
    lv_obj_add_style(lab1, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(lab1, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_label_set_text(lab1, "Perf. monitor is not enabled");

#if LV_USE_PERF_MONITOR
    lv_obj_t * divider = lv_line_create(cont);
    lv_obj_set_size(divider, LV_PCT(100), c->sz->gap[4]);
    /* static const lv_point_precise_t points[] = {{LV_PCT(0), LV_PCT(50)}, {LV_PCT(100), LV_PCT(50)}}; */
    /* lv_line_set_points(divider, points, 2); */
    lv_obj_set_style_line_width(divider, 1, 0);
    lv_obj_add_style(divider, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_line_opa(divider, LV_OPA_10, 0);
    lv_obj_t * lab2 = lv_label_create(cont);
    lv_obj_add_style(lab2, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(lab2, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_t * lab3 = lv_label_create(cont);
    lv_obj_add_style(lab3, &c->fonts[FONT_LABEL_XS], 0);
    lv_obj_add_style(lab3, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_style_text_opa(lab3, LV_OPA_60, 0);

    lv_display_t * disp = lv_display_get_default();
    lv_subject_add_observer_obj(&disp->perf_sysmon_backend.subject, perfmon_data_cb, cont, NULL);
#endif
    return perfmon;
}

#if LV_USE_PERF_MONITOR
static void perfmon_data_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * cont = lv_observer_get_target_obj(observer);
    const lv_sysmon_perf_info_t * perf = lv_subject_get_pointer(subject);

    lv_obj_t * lab1 = lv_obj_get_child(cont, 0);
    lv_obj_t * lab2 = lv_obj_get_child(cont, 2);
    lv_obj_t * lab3 = lv_obj_get_child(cont, 3);

    lv_label_set_text_fmt(lab1, "%"PRIu32" FPS, %"PRIu32"%% CPU", perf->calculated.fps, perf->calculated.cpu);
    lv_label_set_text_fmt(lab2, "Refresh: %"PRIu32"ms", perf->calculated.render_avg_time + perf->calculated.flush_avg_time);
    lv_label_set_text_fmt(lab3, "Render / Flush: %"PRIu32"ms / %"PRIu32"ms", perf->calculated.render_avg_time,
                          perf->calculated.flush_avg_time);

}
#endif

static lv_obj_t * create_settings(lv_obj_t * parent, lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * settings = lv_obj_create(parent);
    lv_obj_remove_style_all(settings);
    lv_obj_add_flag(settings, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_bg_opa(settings, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(settings, c->sz->gap[3], 0);
    lv_obj_set_size(settings, c->sz->settings_panel_width, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_ver(settings, c->sz->gap[5], 0);
    lv_obj_add_style(settings, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_OBJ], 0);

    lv_obj_set_flex_flow(settings, LV_FLEX_FLOW_COLUMN);

    lv_obj_t * temperature_label_cont = create_setting_label_cont(settings, "Temperature", c);
    lv_obj_set_style_text_opa(lv_obj_get_child(temperature_label_cont, 0), LV_OPA_60, 0);

    lv_obj_t * celsius_label_cont = create_setting_label_cont(settings, "Celsius (\xc2\xb0""C)", c);
    lv_obj_set_style_bg_opa(celsius_label_cont, 255 * lv_subject_get_int(&c->temperature_units_are_celsius), 0);

    lv_obj_t * fahrenheit_label_cont = create_setting_label_cont(settings, "Fahrenheit (\xc2\xb0""F)", c);
    lv_obj_set_style_bg_opa(fahrenheit_label_cont, 255 * !lv_subject_get_int(&c->temperature_units_are_celsius), 0);

    lv_obj_add_event_cb(celsius_label_cont, setting_clicked_cb, LV_EVENT_CLICKED, c);
    lv_obj_add_event_cb(fahrenheit_label_cont, setting_clicked_cb, LV_EVENT_CLICKED, c);

    return settings;
}

static lv_obj_t * create_setting_label_cont(lv_obj_t * parent, const char * text, lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * label_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(label_cont);
    lv_obj_set_size(label_cont, LV_PCT(100), c->sz->icon[1]);
    lv_obj_set_style_bg_color(label_cont, lv_color_hex(0xCC0000), 0);
    lv_obj_set_style_pad_left(label_cont, c->sz->gap[7], 0);

    lv_obj_t * label = lv_label_create(label_cont);
    lv_label_set_text_static(label, text);
    lv_obj_add_style(label, &c->fonts[FONT_LABEL_SM], 0);
    lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_set_align(label, LV_ALIGN_LEFT_MID);

    return label_cont;
}

static void setting_clicked_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_obj_t * parent = lv_obj_get_parent(obj);
    lv_demo_high_res_ctx_t * c = lv_event_get_user_data(e);
    lv_obj_t * celsius = lv_obj_get_child(parent, 1);
    lv_obj_t * fahrenheit = lv_obj_get_child(parent, 2);
    if(obj == celsius) {
        lv_obj_set_style_bg_opa(celsius, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(fahrenheit, LV_OPA_TRANSP, 0);
        lv_subject_set_int(&c->temperature_units_are_celsius, 1);
    }
    else {
        lv_obj_set_style_bg_opa(fahrenheit, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_opa(celsius, LV_OPA_TRANSP, 0);
        lv_subject_set_int(&c->temperature_units_are_celsius, 0);
    }
}

static void date_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * date_label = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);

    lv_label_set_text_fmt(date_label, "%s, %"PRId32" %s",
                          (char *)lv_subject_get_pointer(&c->api.subjects.week_day_name),
                          lv_subject_get_int(&c->api.subjects.month_day),
                          (char *)lv_subject_get_pointer(&c->api.subjects.month_name));
}

static void time_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * time_label = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);

    lv_label_set_text_fmt(time_label, "%02d:%02d", lv_subject_get_int(&c->api.subjects.hour),
                          lv_subject_get_int(&c->api.subjects.minute));
}

#endif /*LV_USE_DEMO_HIGH_RES*/

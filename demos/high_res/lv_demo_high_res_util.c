/**
 * @file lv_demo_high_res_util.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/canvas/lv_canvas.h"
#include "../../src/widgets/label/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_12)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_16)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_24)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_30)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_36)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_60)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_120)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_160)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_30)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_60)

LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_20)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_40)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_45)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_regular_90)

LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_80)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_107)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_180)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_light_240)

LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_8)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_11)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_18)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_medium_36)

LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_20)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_24)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_40)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_45)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_54)
LV_FONT_DECLARE(font_lv_demo_high_res_roboto_slab_bold_90)

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void init_fonts_sm(lv_style_t * fonts);
static void init_fonts_md(lv_style_t * fonts);
static void init_fonts_lg(lv_style_t * fonts);
static void theme_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void free_ctx_event_cb(lv_event_t * e);
static void label_text_temperature_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_demo_high_res_theme_t lv_demo_high_res_theme_light = {
    .base = LV_COLOR_MAKE(0xff, 0xff, 0xff),
    .accent = LV_COLOR_MAKE(0xcc, 0x00, 0x00),
};

const lv_demo_high_res_theme_t lv_demo_high_res_theme_dark = {
    .base = LV_COLOR_MAKE(0x00, 0x00, 0x00),
    .accent = LV_COLOR_MAKE(0xe1, 0x2b, 0x17),
};

const lv_demo_high_res_sizes_t lv_demo_high_res_sizes_all[SIZE_COUNT] = {
    {
        .gap = {0, 2, 4, 6, 9, 12, 14, 16, 20, 24, 32},
        .icon = {16, 21, 32, 42, 64},
        .card_long_edge = 200,
        .widget_long_edge = 256,
        .card_short_edge = 120,
        .smart_home_arc_diameter = 107,
        .ev_charging_arc_diameter = 160,
        .smart_meter_collapsed_part_height = 48,
        .slider_width = 27,
        .small_chart_height = 67,
        .large_chart_height = 167,
        .card_radius = 12,
        .health_panel_width = 133,
        .settings_panel_width = 179,
        .home_bottom_margin_height = 53,
        .init_fonts_cb = init_fonts_sm
    },
    {
        .gap = {0, 3, 6, 9, 12, 16, 20, 24, 30, 36, 48},
        .icon = {24, 32, 48, 64, 96},
        .card_long_edge = 300,
        .widget_long_edge = 384,
        .card_short_edge = 180,
        .smart_home_arc_diameter = 160,
        .ev_charging_arc_diameter = 240,
        .smart_meter_collapsed_part_height = 72,
        .slider_width = 40,
        .small_chart_height = 100,
        .large_chart_height = 250,
        .card_radius = 18,
        .health_panel_width = 200,
        .settings_panel_width = 268,
        .home_bottom_margin_height = 80,
        .init_fonts_cb = init_fonts_md
    },
    {
        .gap = {0, 5, 10, 15, 18, 24, 30, 36, 45, 54, 72},
        .icon = {36, 48, 72, 96, 144},
        .card_long_edge = 450,
        .widget_long_edge = 576,
        .card_short_edge = 270,
        .smart_home_arc_diameter = 240,
        .ev_charging_arc_diameter = 360,
        .smart_meter_collapsed_part_height = 108,
        .slider_width = 60,
        .small_chart_height = 150,
        .large_chart_height = 375,
        .card_radius = 27,
        .health_panel_width = 300,
        .settings_panel_width = 402,
        .home_bottom_margin_height = 120,
        .init_fonts_cb = init_fonts_lg
    }
};

/**********************
 *      MACROS
 **********************/

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(*arr))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_demo_high_res_base_obj_create(const char * assets_path,
                                            const char * logo_path,
                                            const char * slides_path,
                                            lv_demo_high_res_exit_cb_t exit_cb)
{
    lv_demo_high_res_ctx_t * c = lv_malloc_zeroed(sizeof(lv_demo_high_res_ctx_t));
    LV_ASSERT_MALLOC(c);
    lv_obj_t * base_obj = lv_obj_create(lv_screen_active());
    lv_obj_set_user_data(base_obj, c);
    lv_obj_add_event_cb(base_obj, free_ctx_event_cb, LV_EVENT_DELETE, NULL);

    lv_display_t * disp = lv_display_get_default();
    int32_t hres = lv_display_get_horizontal_resolution(disp);
    int32_t vres = lv_display_get_vertical_resolution(disp);
    bool is_exact;
    int32_t size;
    if(hres < 1280 && vres < 720) {
        is_exact = hres == 800 && vres == 480;
        size = SIZE_SM;
    }
    else if(hres < 1920 && vres < 1080) {
        is_exact = hres == 1280 && vres == 720;
        size = SIZE_MD;
    }
    else {
        is_exact = hres == 1920 && vres == 1080;
        size = SIZE_LG;
    }
    if(!is_exact) {
        LV_LOG_WARN("a display size of exactly 800x480, 1280x720, or 1920x1080 is recommended for the high-res demo");
    }
    c->sz = &lv_demo_high_res_sizes_all[size];

    static const struct {
        const char * name;
        lv_color_format_t cf;
    } image_details[IMG_COUNT] = {
        {"about_app_icon", LV_COLOR_FORMAT_ARGB8888},
        {"album_art", LV_COLOR_FORMAT_ARGB8888},
        {"arrow_left", LV_COLOR_FORMAT_ARGB8888},
        {"backward_icon", LV_COLOR_FORMAT_ARGB8888},
        {"cold_icon", LV_COLOR_FORMAT_ARGB8888},
        {"dry_icon", LV_COLOR_FORMAT_ARGB8888},
        {"energy_icon", LV_COLOR_FORMAT_ARGB8888},
        {"ev_charging_app_icon", LV_COLOR_FORMAT_ARGB8888},
        {"ev_charging_widget3_1_bg", LV_COLOR_FORMAT_ARGB8888},
        {"ev_charging_widget3_bg", LV_COLOR_FORMAT_ARGB8888},
        {"fan", LV_COLOR_FORMAT_ARGB8888},
        {"forward_icon", LV_COLOR_FORMAT_ARGB8888},
        {"health_icon", LV_COLOR_FORMAT_ARGB8888},
        {"health_icon_bold", LV_COLOR_FORMAT_ARGB8888},
        {"heat_icon", LV_COLOR_FORMAT_ARGB8888},
        {"lamp", LV_COLOR_FORMAT_ARGB8888},
        {"logout_icon", LV_COLOR_FORMAT_ARGB8888},
        {"main_light_slider", LV_COLOR_FORMAT_ARGB8888},
        {"minus", LV_COLOR_FORMAT_ARGB8888},
        {"pager_left", LV_COLOR_FORMAT_ARGB8888},
        {"pager_pause", LV_COLOR_FORMAT_ARGB8888},
        {"pager_play", LV_COLOR_FORMAT_ARGB8888},
        {"pager_right", LV_COLOR_FORMAT_ARGB8888},
        {"play_icon", LV_COLOR_FORMAT_ARGB8888},
        {"play_icon_1", LV_COLOR_FORMAT_ARGB8888},
        {"plus", LV_COLOR_FORMAT_ARGB8888},
        {"range_icon", LV_COLOR_FORMAT_ARGB8888},
        {"setting_icon", LV_COLOR_FORMAT_ARGB8888},
        {"setting_icon_bold", LV_COLOR_FORMAT_ARGB8888},
        {"smart_home_app_icon", LV_COLOR_FORMAT_ARGB8888},
        {"smart_home_widget1_bg", LV_COLOR_FORMAT_ARGB8888},
        {"smart_home_widget2_bg", LV_COLOR_FORMAT_ARGB8888},
        {"smart_meter_app_icon", LV_COLOR_FORMAT_ARGB8888},
        {"thermostat_app_icon", LV_COLOR_FORMAT_ARGB8888},
        {"time_icon", LV_COLOR_FORMAT_ARGB8888},
        {"unlock", LV_COLOR_FORMAT_ARGB8888},
        {"volume", LV_COLOR_FORMAT_ARGB8888},
        {"weather", LV_COLOR_FORMAT_ARGB8888},
        {"wifi_icon", LV_COLOR_FORMAT_ARGB8888},
        {"light_bg_about", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_about", LV_COLOR_FORMAT_NATIVE},
        {"light_bg_ev_charging", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_ev_charging", LV_COLOR_FORMAT_NATIVE},
        {"light_bg_home", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_home", LV_COLOR_FORMAT_NATIVE},
        {"light_bg_smart_home", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_smart_home", LV_COLOR_FORMAT_NATIVE},
        {"light_bg_smart_meter", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_smart_meter", LV_COLOR_FORMAT_NATIVE},
        {"light_bg_thermostat", LV_COLOR_FORMAT_NATIVE},
        {"dark_bg_thermostat", LV_COLOR_FORMAT_NATIVE},
        {"light_dark_theme_icon", LV_COLOR_FORMAT_ARGB8888},
        {"dark_dark_theme_icon", LV_COLOR_FORMAT_ARGB8888},
        {"light_light_theme_icon", LV_COLOR_FORMAT_ARGB8888},
        {"dark_light_theme_icon", LV_COLOR_FORMAT_ARGB8888},
        {"light_widget1_bg", LV_COLOR_FORMAT_ARGB8888},
        {"dark_widget1_bg", LV_COLOR_FORMAT_ARGB8888},
        {"light_widget2_bg", LV_COLOR_FORMAT_ARGB8888},
        {"dark_widget2_bg", LV_COLOR_FORMAT_ARGB8888},
        {"light_widget3_bg", LV_COLOR_FORMAT_ARGB8888},
        {"dark_widget3_bg", LV_COLOR_FORMAT_ARGB8888},
        {"light_widget4_bg", LV_COLOR_FORMAT_ARGB8888},
        {"dark_widget4_bg", LV_COLOR_FORMAT_ARGB8888},
        {"light_widget5_bg", LV_COLOR_FORMAT_ARGB8888},
        {"dark_widget5_bg", LV_COLOR_FORMAT_ARGB8888},
    };
    const char * size_prefix = size == SIZE_SM ? "sm" : size == SIZE_MD ? "md" : "lg";
    for(uint32_t i = 0; i < IMG_COUNT; i++) {
        char path_buf[256];
        int chars = lv_snprintf(path_buf, sizeof(path_buf), "%s/img_lv_demo_high_res_%s_%s.png",
                                assets_path, image_details[i].name, size_prefix);
        LV_ASSERT(chars < (int)sizeof(path_buf));
        c->imgs[i] = lv_demo_high_res_image_preload(path_buf, image_details[i].cf);
    }

    for(uint32_t i = 0; i < STYLE_COLOR_COUNT; i++) {
        for(uint32_t j = 0; j < STYLE_TYPE_COUNT; j++) {
            lv_style_init(&c->styles[i][j]);
        }
    }

    for(uint32_t i = 0; i < FONT_COUNT; i++) {
        lv_style_init(&c->fonts[i]);
    }
    c->sz->init_fonts_cb(c->fonts);

    lv_subject_init_pointer(&c->th, (void *)&lv_demo_high_res_theme_light);
    c->th.user_data = c;
    lv_subject_add_observer(&c->th, theme_observer_cb, c);

    c->logo_path = lv_strdup(logo_path);
    LV_ASSERT_MALLOC(c->logo_path);
    c->slides_path = lv_strdup(slides_path);
    LV_ASSERT_MALLOC(c->slides_path);

    c->exit_cb = exit_cb;

    lv_subject_init_int(&c->temperature_units_are_celsius, 1);
    c->ev_charging_bg_cont = NULL;
    lv_subject_init_int(&c->ev_charging_progress, EV_CHARGING_RANGE_END * 0 / 100);
    lv_subject_init_int(&c->smart_meter_selected_bar, 4);

    c->top_margin_subjects_are_init = false;

    /* API subjects */

    /* input subjects */
    lv_subject_init_int(&c->api.subjects.hour, 9);
    lv_subject_init_int(&c->api.subjects.minute, 36);
    lv_subject_init_pointer(&c->api.subjects.week_day_name, "Tuesday");
    lv_subject_init_int(&c->api.subjects.month_day, 31);
    lv_subject_init_pointer(&c->api.subjects.month_name, "October");
    lv_subject_init_int(&c->api.subjects.temperature_outdoor, 140); /* tenths of a degree */
    lv_subject_init_int(&c->api.subjects.temperature_indoor, 225); /* tenths of a degree */
    lv_subject_init_pointer(&c->api.subjects.wifi_ssid, NULL);
    lv_subject_init_pointer(&c->api.subjects.wifi_ip, NULL);

    /* output subjects */
    lv_subject_init_int(&c->api.subjects.music_play, 1);

    /* input+output subjects */
    lv_subject_init_int(&c->api.subjects.locked, 0);
    lv_subject_init_int(&c->api.subjects.volume, 63);
    lv_subject_init_int(&c->api.subjects.main_light_temperature, 4000);
    lv_subject_init_int(&c->api.subjects.main_light_intensity, 52);
    lv_subject_init_int(&c->api.subjects.thermostat_fan_speed, 40);
    lv_subject_init_int(&c->api.subjects.thermostat_target_temperature, 240); /* tenths of a degree */

    c->api.base_obj = base_obj;
    c->api.user_data = NULL;

    c->subject_groups.time.members[0] = &c->api.subjects.hour;
    c->subject_groups.time.members[1] = &c->api.subjects.minute;
    lv_subject_init_group(&c->subject_groups.time.group, c->subject_groups.time.members,
                          ARRAY_LEN(c->subject_groups.time.members));
    c->subject_groups.date.members[0] = &c->api.subjects.week_day_name;
    c->subject_groups.date.members[1] = &c->api.subjects.month_day;
    c->subject_groups.date.members[2] = &c->api.subjects.month_name;
    lv_subject_init_group(&c->subject_groups.date.group, c->subject_groups.date.members,
                          ARRAY_LEN(c->subject_groups.date.members));
    c->subject_groups.wifi.members[0] = &c->api.subjects.wifi_ssid;
    c->subject_groups.wifi.members[1] = &c->api.subjects.wifi_ip;
    lv_subject_init_group(&c->subject_groups.wifi.group, c->subject_groups.wifi.members,
                          ARRAY_LEN(c->subject_groups.wifi.members));

    return base_obj;
}

lv_obj_t * lv_demo_high_res_simple_container_create(lv_obj_t * parent, bool vertical, int32_t pad,
                                                    lv_flex_align_t align_cross_place)
{
    lv_obj_t * obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    if(vertical) {
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(obj, pad, 0);
    }
    else {
        lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
        lv_obj_set_style_pad_column(obj, pad, 0);
    }
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_START, align_cross_place, align_cross_place);
    return obj;
}

void lv_demo_high_res_label_bind_temperature(lv_obj_t * label, lv_subject_t * subject, lv_demo_high_res_ctx_t * c)
{
    lv_obj_set_user_data(label, c);
    lv_subject_add_observer_obj(subject, label_text_temperature_cb, label, subject);
    lv_subject_add_observer_obj(&c->temperature_units_are_celsius, label_text_temperature_cb, label, subject);
}

void lv_demo_high_res_theme_observer_image_src_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_image_dsc_t ** pair = lv_observer_get_user_data(observer);

    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(subject);

    if(th == &lv_demo_high_res_theme_light) {
        lv_image_set_src(obj, pair[0]);
    }
    else {
        lv_image_set_src(obj, pair[1]);
    }
}

void lv_demo_high_res_theme_observer_obj_bg_image_src_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_image_dsc_t ** pair = lv_observer_get_user_data(observer);

    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(subject);

    if(th == &lv_demo_high_res_theme_light) {
        lv_obj_set_style_bg_image_src(obj, pair[0], 0);
    }
    else {
        lv_obj_set_style_bg_image_src(obj, pair[1], 0);
    }
}

lv_image_dsc_t * lv_demo_high_res_image_preload(const void * src, lv_color_format_t cf)
{
    lv_image_header_t header;
    lv_result_t res = lv_image_decoder_get_info(src, &header);
    if(res == LV_RESULT_INVALID) {
        LV_LOG_WARN("Couldn't read the header info of source");
        return NULL;
    }

    lv_draw_buf_t * dest;
    dest = lv_draw_buf_create(header.w, header.h, cf, LV_STRIDE_AUTO);

    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, dest);
    lv_canvas_fill_bg(canvas, lv_color_hex3(0x000), LV_OPA_TRANSP);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = src;

    lv_area_t coords = {0, 0, LV_MIN(header.w, dest->header.w) - 1, LV_MIN(header.h, dest->header.h) - 1};
    lv_draw_image(&layer, &dsc, &coords);
    lv_canvas_finish_layer(canvas, &layer);

    lv_obj_delete(canvas);

    return (lv_image_dsc_t *) dest;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void init_fonts_sm(lv_style_t * fonts)
{
    lv_style_set_text_font(&fonts[FONT_HEADING_MD], &font_lv_demo_high_res_roboto_slab_regular_20);
    lv_style_set_text_font(&fonts[FONT_HEADING_LG], &font_lv_demo_high_res_roboto_slab_regular_40);
    lv_style_set_text_font(&fonts[FONT_HEADING_XL], &font_lv_demo_high_res_roboto_slab_light_80);
    lv_style_set_text_font(&fonts[FONT_HEADING_XXL], &font_lv_demo_high_res_roboto_slab_light_107);
    lv_style_set_text_font(&fonts[FONT_LABEL_XS], &font_lv_demo_high_res_roboto_medium_8);
    lv_style_set_text_line_space(&fonts[FONT_LABEL_XS],
                                 30 * lv_font_get_line_height(&font_lv_demo_high_res_roboto_medium_8) / 100); /* +30% */
    /* lv_style_set_text_letter_space(&fonts[FONT_LABEL_XS], 197 * lv_font_get_letter_space(&font_lv_demo_high_res_roboto_medium_8) / 200); */ /* -1.5% */
    lv_style_set_text_font(&fonts[FONT_LABEL_SM], &font_lv_demo_high_res_roboto_medium_11);
    lv_style_set_text_font(&fonts[FONT_LABEL_MD], &font_lv_demo_high_res_roboto_medium_16);
    lv_style_set_text_font(&fonts[FONT_LABEL_LG], &font_lv_demo_high_res_roboto_slab_bold_20);
    lv_style_set_text_font(&fonts[FONT_LABEL_XL], &font_lv_demo_high_res_roboto_slab_bold_24);
    lv_style_set_text_font(&fonts[FONT_LABEL_2XL], &font_lv_demo_high_res_roboto_slab_bold_40);
}

static void init_fonts_md(lv_style_t * fonts)
{
    lv_style_set_text_font(&fonts[FONT_HEADING_MD], &font_lv_demo_high_res_roboto_slab_regular_30);
    lv_style_set_text_font(&fonts[FONT_HEADING_LG], &font_lv_demo_high_res_roboto_slab_regular_60);
    lv_style_set_text_font(&fonts[FONT_HEADING_XL], &font_lv_demo_high_res_roboto_slab_light_120);
    lv_style_set_text_font(&fonts[FONT_HEADING_XXL], &font_lv_demo_high_res_roboto_slab_light_160);
    lv_style_set_text_font(&fonts[FONT_LABEL_XS], &font_lv_demo_high_res_roboto_medium_12);
    lv_style_set_text_line_space(&fonts[FONT_LABEL_XS],
                                 30 * lv_font_get_line_height(&font_lv_demo_high_res_roboto_medium_12) / 100); /* +30% */
    /* lv_style_set_text_letter_space(&fonts[FONT_LABEL_XS], 197 * lv_font_get_letter_space(&font_lv_demo_high_res_roboto_medium_12) / 200); */ /* -1.5% */
    lv_style_set_text_font(&fonts[FONT_LABEL_SM], &font_lv_demo_high_res_roboto_medium_16);
    lv_style_set_text_font(&fonts[FONT_LABEL_MD], &font_lv_demo_high_res_roboto_medium_24);
    lv_style_set_text_font(&fonts[FONT_LABEL_LG], &font_lv_demo_high_res_roboto_slab_bold_30);
    lv_style_set_text_font(&fonts[FONT_LABEL_XL], &font_lv_demo_high_res_roboto_slab_bold_36);
    lv_style_set_text_font(&fonts[FONT_LABEL_2XL], &font_lv_demo_high_res_roboto_slab_bold_60);
}

static void init_fonts_lg(lv_style_t * fonts)
{
    lv_style_set_text_font(&fonts[FONT_HEADING_MD], &font_lv_demo_high_res_roboto_slab_regular_45);
    lv_style_set_text_font(&fonts[FONT_HEADING_LG], &font_lv_demo_high_res_roboto_slab_regular_90);
    lv_style_set_text_font(&fonts[FONT_HEADING_XL], &font_lv_demo_high_res_roboto_slab_light_180);
    lv_style_set_text_font(&fonts[FONT_HEADING_XXL], &font_lv_demo_high_res_roboto_slab_light_240);
    lv_style_set_text_font(&fonts[FONT_LABEL_XS], &font_lv_demo_high_res_roboto_medium_18);
    lv_style_set_text_line_space(&fonts[FONT_LABEL_XS],
                                 30 * lv_font_get_line_height(&font_lv_demo_high_res_roboto_medium_18) / 100); /* +30% */
    /* lv_style_set_text_letter_space(&fonts[FONT_LABEL_XS], 197 * lv_font_get_letter_space(&font_lv_demo_high_res_roboto_medium_18) / 200); */ /* -1.5% */
    lv_style_set_text_font(&fonts[FONT_LABEL_SM], &font_lv_demo_high_res_roboto_medium_24);
    lv_style_set_text_font(&fonts[FONT_LABEL_MD], &font_lv_demo_high_res_roboto_medium_36);
    lv_style_set_text_font(&fonts[FONT_LABEL_LG], &font_lv_demo_high_res_roboto_slab_bold_45);
    lv_style_set_text_font(&fonts[FONT_LABEL_XL], &font_lv_demo_high_res_roboto_slab_bold_54);
    lv_style_set_text_font(&fonts[FONT_LABEL_2XL], &font_lv_demo_high_res_roboto_slab_bold_90);
}

static void theme_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const lv_demo_high_res_theme_t * th = lv_subject_get_pointer(subject);
    const lv_demo_high_res_theme_t * th_other = th == &lv_demo_high_res_theme_light ? &lv_demo_high_res_theme_dark :
                                                &lv_demo_high_res_theme_light;
    lv_demo_high_res_ctx_t * c = lv_observer_get_user_data(observer);

    for(uint32_t i = 0; i < STYLE_COLOR_COUNT; i++) {
        lv_color_t color = i == STYLE_COLOR_BASE ? th->base : th->accent;
        lv_color_t color_inv = i == STYLE_COLOR_ACCENT ? th->accent : th_other->base;

        lv_style_set_bg_color(&c->styles[i][STYLE_TYPE_OBJ], color);
        lv_style_set_outline_color(&c->styles[i][STYLE_TYPE_OBJ], color);

        lv_style_set_text_color(&c->styles[i][STYLE_TYPE_TEXT], color_inv);
        lv_style_set_line_color(&c->styles[i][STYLE_TYPE_TEXT], color_inv);

        lv_style_set_image_recolor_opa(&c->styles[i][STYLE_TYPE_A8_IMG], LV_OPA_COVER);
        lv_style_set_image_recolor(&c->styles[i][STYLE_TYPE_A8_IMG], color_inv);
    }

    for(uint32_t i = 0; i < STYLE_COLOR_COUNT; i++) {
        for(uint32_t j = 0; j < STYLE_TYPE_COUNT; j++) {
            lv_obj_report_style_change(&c->styles[i][j]);
        }
    }
}

static void free_ctx_event_cb(lv_event_t * e)
{
    lv_obj_t * base_obj = lv_event_get_target_obj(e);
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    lv_subject_deinit(&c->th);

    for(uint32_t i = 0; i < IMG_COUNT; i++) {
        lv_draw_buf_destroy((lv_draw_buf_t *)c->imgs[i]);
    }

    for(uint32_t i = 0; i < STYLE_COLOR_COUNT; i++) {
        for(uint32_t j = 0; j < STYLE_TYPE_COUNT; j++) {
            lv_style_reset(&c->styles[i][j]);
        }
    }

    for(uint32_t i = 0; i < FONT_COUNT; i++) {
        lv_style_reset(&c->fonts[i]);
    }

    lv_free(c->logo_path);
    lv_free(c->slides_path);

    lv_subject_deinit(&c->temperature_units_are_celsius);
    lv_subject_deinit(&c->ev_charging_progress);
    lv_subject_deinit(&c->smart_meter_selected_bar);

    lv_demo_high_res_top_margin_deinit_subjects(c);

    lv_subject_deinit(&c->subject_groups.time.group);
    lv_subject_deinit(&c->subject_groups.date.group);
    lv_subject_deinit(&c->subject_groups.wifi.group);

    lv_subject_t * subjects = (lv_subject_t *) &c->api.subjects;
    for(uint32_t i = 0; i < sizeof(c->api.subjects) / sizeof(lv_subject_t); i++) {
        lv_subject_deinit(&subjects[i]);
    }

    lv_free(c);

    lv_obj_set_user_data(base_obj, NULL);
}

static void label_text_temperature_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_subject_t * temperature_subject = lv_observer_get_user_data(observer);
    int32_t val = lv_subject_get_int(temperature_subject);
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(label);

    if(!lv_subject_get_int(&c->temperature_units_are_celsius)) {
        /* convert to fahrenheit */
        /* + 320 instead of 32 because temperatures in are tenths of a degree */
        val = val * 9 / 5 + 320;
    }

    int32_t full = val / 10;
    int32_t fraction = val % 10;
    char buf[16];
    if(fraction) {
        lv_snprintf(buf, sizeof(buf), "%"PRId32".%"PRId32"\xc2\xb0", full, fraction);
    }
    else {
        lv_snprintf(buf, sizeof(buf), "%"PRId32"\xc2\xb0", full);
    }

    lv_label_set_text(label, buf);
}

#endif /*LV_USE_DEMO_HIGH_RES*/

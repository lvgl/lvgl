/**
 * @file lv_demo_smartwatch_weather.c
 * Weather screen layout & functions. Shows the current weather details as well as
 * hourly and daily forecasts.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_weather.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void weather_screen_events(lv_event_t * e);

static lv_obj_t * create_info_card(lv_obj_t * parent, const char * title, const void * icon);
static lv_obj_t * create_forecast_container(lv_obj_t * parent, const char * title);
static void add_hourly_forecast(lv_obj_t * parent, int32_t hour, const void * icon, int32_t temp);
static void add_weekly_forecast(lv_obj_t * parent, const char * day, const void * icon, int32_t low, int32_t high);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * weather_screen;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_weather_create(void)
{

    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_text_opa(&main_style, LV_OPA_100);
        lv_style_set_bg_color(&main_style, lv_color_hex(0x316bb6));
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
        lv_style_set_translate_x(&main_style, 384);

    }

    weather_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(weather_screen);
    lv_obj_add_style(weather_screen, &main_style, 0);
    lv_obj_set_size(weather_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(weather_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_scroll_dir(weather_screen, LV_DIR_VER);
    lv_obj_remove_flag(weather_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_add_event_cb(weather_screen, weather_screen_events, LV_EVENT_ALL, NULL);

    LV_IMAGE_DECLARE(image_weather_bg);
    lv_obj_t * background = lv_image_create(weather_screen);
    lv_obj_set_pos(background, 0, 0);
    lv_obj_set_size(background, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_image_set_src(background, &image_weather_bg);

    LV_FONT_DECLARE(font_inter_regular_28);
    lv_obj_t * label = lv_label_create(weather_screen);
    lv_label_set_text(label, "13:37");
    lv_obj_set_align(label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(label, 30);
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    lv_obj_t * main_cont = lv_obj_create(weather_screen);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, 384, LV_SIZE_CONTENT);
    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_scroll_dir(main_cont, LV_DIR_VER);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(main_cont, 40, 0);
    lv_obj_set_style_pad_bottom(main_cont, 64, 0);
    lv_obj_set_style_pad_row(main_cont, 20, 0);

    /* Weather details container */
    lv_obj_t * cont = lv_obj_create(main_cont);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 384, 335);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    label = lv_label_create(cont);
    lv_label_set_text(label, "Budapest");
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    lv_obj_t * temp_cont = lv_obj_create(cont);
    lv_obj_remove_style_all(temp_cont);
    lv_obj_set_size(temp_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_column(temp_cont, 5, 0);
    lv_obj_set_layout(temp_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(temp_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(temp_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    extern uint8_t lottie_sun_cloud[];
    extern size_t lottie_sun_cloud_size;
    lv_obj_t * sun_icon = lv_lottie_create(temp_cont);
    lv_lottie_set_src_data(sun_icon, lottie_sun_cloud, lottie_sun_cloud_size);
    lv_obj_set_size(sun_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_align(sun_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(sun_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(sun_icon, LV_OBJ_FLAG_SCROLLABLE);
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    /*If there are no special requirements, just declare a buffer
        x4 because the Lottie is rendered in ARGB8888 format*/
    static uint8_t sun_buf[100 * 89 * 4];
    lv_lottie_set_buffer(sun_icon, 100, 89, sun_buf);
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(sun_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(sun_icon, &sun_buf);
#endif


    LV_FONT_DECLARE(font_inter_light_124);
    label = lv_label_create(temp_cont);
    lv_label_set_text(label, "12°");
    lv_obj_set_style_text_font(label, &font_inter_light_124, 0);

    lv_obj_t * text_cont = lv_obj_create(cont);
    lv_obj_remove_style_all(text_cont);
    lv_obj_set_size(text_cont, 334, 50);
    lv_obj_set_style_pad_column(text_cont, 5, 0);
    lv_obj_set_layout(text_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(text_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(text_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    label = lv_label_create(text_cont);
    lv_label_set_text(label, "Max. 28°");
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    label = lv_label_create(text_cont);
    lv_label_set_text(label, "Min. 07°");
    lv_obj_set_style_text_color(label, lv_color_hex(0x94d4fd), 0);
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);


    /* Info cards container */
    lv_obj_t * cont_cards = lv_obj_create(main_cont);
    lv_obj_remove_style_all(cont_cards);
    lv_obj_set_size(cont_cards, 385, LV_SIZE_CONTENT);
    lv_obj_set_layout(cont_cards, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont_cards, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_cards, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(cont_cards, 22, 0);


    /* Pressure info card */
    LV_IMAGE_DECLARE(image_pressure_icon);
    lv_obj_t * card = create_info_card(cont_cards, "PRESSURE", &image_pressure_icon);

    LV_IMAGE_DECLARE(image_pressure_scale);
    lv_obj_t * image = lv_image_create(card);
    lv_image_set_src(image, &image_pressure_scale);
    lv_obj_set_style_align(image, LV_ALIGN_CENTER, 0);
    lv_obj_set_y(image, 5);

    LV_FONT_DECLARE(font_inter_bold_32);
    lv_obj_t * pressure = lv_label_create(card);
    lv_label_set_text(pressure, "1013");
    lv_obj_set_y(pressure, 5);
    lv_obj_set_style_align(pressure, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(pressure, &font_inter_bold_32, 0);

    LV_FONT_DECLARE(font_inter_regular_24);
    pressure = lv_label_create(card);
    lv_obj_set_pos(pressure, 0, 35);
    lv_label_set_text(pressure, "hPa");
    lv_obj_set_style_align(pressure, LV_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(pressure, &font_inter_regular_24, 0);

    lv_obj_t * text = lv_label_create(card);
    lv_label_set_text(text, "Low");
    lv_obj_set_style_align(text, LV_ALIGN_BOTTOM_LEFT, 0);
    lv_obj_set_style_text_font(text, &font_inter_regular_24, 0);

    text = lv_label_create(card);
    lv_label_set_text(text, "High");
    lv_obj_set_style_align(text, LV_ALIGN_BOTTOM_RIGHT, 0);
    lv_obj_set_style_text_font(text, &font_inter_regular_24, 0);


    /* UV Index info card */
    LV_IMAGE_DECLARE(image_uv_icon);
    LV_IMAGE_DECLARE(image_uv_scale);
    card = create_info_card(cont_cards, "UV INDEX", &image_uv_icon);
    lv_obj_t * slider = lv_slider_create(card);
    lv_obj_set_pos(slider, 0, -59);
    lv_obj_set_size(slider, 160, 6);
    lv_slider_set_value(slider, 25, LV_ANIM_OFF);
    lv_obj_set_align(slider, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_bg_image_src(slider, &image_uv_scale, 0);
    lv_obj_set_style_bg_opa(slider, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(slider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(slider, 3, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(slider, lv_color_hex(0xff114370), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(slider, lv_color_hex(0xffffffff), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(slider, 255, LV_PART_KNOB | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(font_inter_regular_48);
    lv_obj_t * uv_value = lv_label_create(card);
    lv_obj_set_pos(uv_value, 0, 5);
    lv_label_set_text(uv_value, "2");
    lv_obj_set_align(uv_value, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_text_font(uv_value, &font_inter_regular_48, 0);

    LV_FONT_DECLARE(font_inter_light_32);
    text = lv_label_create(card);
    lv_label_set_text(text, "Low");
    lv_obj_set_align(text, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_font(text, &font_inter_light_32, 0);

    /* Humidity info card */
    LV_IMAGE_DECLARE(image_humidity_icon);
    LV_FONT_DECLARE(font_inter_regular_70);
    card = create_info_card(cont_cards, "HUMIDITY", &image_humidity_icon);
    lv_obj_t * humidity_value = lv_label_create(card);
    lv_label_set_text(humidity_value, "75%");
    lv_obj_set_align(humidity_value, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_set_style_text_color(humidity_value, lv_color_hex(0xffffffff), 0);
    lv_obj_set_style_text_font(humidity_value, &font_inter_regular_70, 0);


    /* Today's forecast container */
    lv_obj_t * panel = create_forecast_container(main_cont, "Today's Forecast");

    /* Add the hourly forecast */
    LV_IMAGE_DECLARE(image_sunshine_icon);
    for(int32_t i = 0; i < 10; i++) {
        add_hourly_forecast(panel, i, &image_sunshine_icon, 20 + i);
    }

    /* 10 days forecast container */
    panel = create_forecast_container(main_cont, "10 Days Forecast");
    /* Add the weekly forecast*/
    for(int32_t i = 0; i < 10; i++) {
        add_weekly_forecast(panel, "Mon", &image_sunshine_icon, 5 + i, 17 + i);
    }

    /* Update time label */
    LV_FONT_DECLARE(font_inter_light_24);
    label = lv_label_create(main_cont);
    lv_label_set_text(label, "Updated Today\nat 12 pm");
    lv_obj_set_style_text_font(label, &font_inter_light_24, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

}

lv_obj_t * lv_demo_smartwatch_get_weather_screen(void)
{
    if(!weather_screen) {
        lv_demo_smartwatch_weather_create();
    }
    return weather_screen;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * create_info_card(lv_obj_t * parent, const char * title, const void * icon)
{

    lv_obj_t * card = lv_obj_create(parent);
    lv_obj_remove_style_all(card);
    lv_obj_set_size(card, 200, 214);
    lv_obj_set_align(card, LV_ALIGN_TOP_MID);

    lv_obj_set_style_bg_color(card, lv_color_hex(0xff000000), 0);
    lv_obj_set_style_bg_opa(card, 50, 0);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_pad_top(card, 16, 0);
    lv_obj_set_style_pad_bottom(card, 24, 0);
    lv_obj_set_style_pad_left(card, 20, 0);
    lv_obj_set_style_pad_right(card, 20, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * cont = lv_obj_create(card);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, 156, LV_SIZE_CONTENT);

    lv_obj_t * image = lv_image_create(cont);
    lv_obj_set_size(image, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_image_set_src(image, icon);

    LV_FONT_DECLARE(font_inter_regular_24);
    lv_obj_t * label = lv_label_create(cont);
    lv_obj_set_pos(label, 35, 0);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_font(label, &font_inter_regular_24, 0);

    return card;

}

static lv_obj_t * create_forecast_container(lv_obj_t * parent, const char * title)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, 384, LV_SIZE_CONTENT);
    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_top(main_cont, 40, 0);
    lv_obj_set_style_pad_row(main_cont, 24, 0);

    LV_FONT_DECLARE(font_inter_regular_28);
    lv_obj_t * label = lv_label_create(main_cont);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    lv_obj_t * forecast_cont = lv_obj_create(main_cont);
    lv_obj_remove_style_all(forecast_cont);
    lv_obj_set_size(forecast_cont, 384, LV_SIZE_CONTENT);
    lv_obj_set_layout(forecast_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(forecast_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(forecast_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_set_style_pad_top(forecast_cont, 24, 0);
    lv_obj_set_style_pad_bottom(forecast_cont, 24, 0);
    lv_obj_set_style_pad_row(forecast_cont, 26, 0);

    return forecast_cont;
}

static void add_hourly_forecast(lv_obj_t * parent, int32_t hour, const void * icon, int32_t temp)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, 336, LV_SIZE_CONTENT);
    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    LV_FONT_DECLARE(font_inter_regular_28);
    lv_obj_t * label = lv_label_create(main_cont);
    lv_label_set_text_fmt(label, "%02d", hour);
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    lv_obj_t * image = lv_image_create(main_cont);
    lv_image_set_src(image, icon);

    lv_obj_t * temp_label = lv_label_create(main_cont);
    lv_label_set_text_fmt(temp_label, "%02d°", temp);
    lv_obj_set_style_text_font(temp_label, &font_inter_regular_28, 0);

}

static void add_weekly_forecast(lv_obj_t * parent, const char * day, const void * icon, int32_t low, int32_t high)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_remove_style_all(main_cont);
    lv_obj_set_size(main_cont, 336, LV_SIZE_CONTENT);

    lv_obj_set_layout(main_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    LV_FONT_DECLARE(font_inter_regular_28);
    lv_obj_t * label = lv_label_create(main_cont);
    lv_label_set_text(label, day);
    lv_obj_set_style_text_font(label, &font_inter_regular_28, 0);

    LV_IMAGE_DECLARE(image_sunshine_icon);
    lv_obj_t * image = lv_image_create(main_cont);
    lv_image_set_src(image, icon);

    lv_obj_t * temp_label = lv_label_create(main_cont);
    lv_label_set_text_fmt(temp_label, "%02d° / %02d°", low, high);
    lv_obj_set_style_text_font(temp_label, &font_inter_regular_28, 0);

}

static void weather_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if(dir == LV_DIR_RIGHT) {
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_weather_screen(), 384, 1000, 0);
            lv_smartwatch_animate_arc(arc_cont, ARC_EXPAND_RIGHT, 700, 300);
            lv_smartwatch_anim_opa(main_arc, 255, 500, 500);
            lv_smartwatch_anim_opa(overlay, 0, 100, 0);
        }

        if(dir == LV_DIR_LEFT) {

            lv_smartwatch_anim_opa(overlay, 255, 100, 0);
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_weather_screen(), -384 - TRANSITION_GAP, 1000, 0);
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_health_screen(), 384 + TRANSITION_GAP, 0, 1000, 0);
        }

    }

}

#endif /*LV_USE_DEMO_SMARTWATCH*/
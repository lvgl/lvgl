/**
 * @file lv_demo_smartwatch_health.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_health.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void health_screen_events(lv_event_t * e);
static void button_click_event(lv_event_t * e);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * health_screen;

static lv_obj_t * lottie_ecg;
static lv_obj_t * heart_bg_2;
static lv_obj_t * heart_icon;
static lv_obj_t * image_button;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_health_create(void)
{

    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_bg_color(&main_style, lv_color_hex(0x000000));
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
        lv_style_set_translate_x(&main_style, 384);

    }

    health_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(health_screen);
    lv_obj_add_style(health_screen, &main_style, 0);
    lv_obj_set_size(health_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(health_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(health_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_remove_flag(health_screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(health_screen, health_screen_events, LV_EVENT_ALL, NULL);


    LV_IMAGE_DECLARE(image_heart_bg_large);
    lv_obj_t * heart_bg_1 = lv_image_create(health_screen);
    lv_image_set_src(heart_bg_1, &image_heart_bg_large);
    lv_obj_set_x(heart_bg_1, 0);
    lv_obj_set_y(heart_bg_1, 2);
    lv_obj_set_align(heart_bg_1, LV_ALIGN_CENTER);
    lv_obj_add_flag(heart_bg_1, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(heart_bg_1, LV_OBJ_FLAG_SCROLLABLE);

    LV_IMAGE_DECLARE(image_heart_bg_small);
    heart_bg_2 = lv_image_create(health_screen);
    lv_image_set_src(heart_bg_2, &image_heart_bg_small);
    lv_obj_set_x(heart_bg_2, 0);
    lv_obj_set_y(heart_bg_2, -23);
    lv_obj_set_align(heart_bg_2, LV_ALIGN_CENTER);
    lv_obj_add_flag(heart_bg_2, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(heart_bg_2, LV_OBJ_FLAG_SCROLLABLE);

    extern uint8_t lottie_ecg_wave[];
    extern size_t lottie_ecg_wave_size;
    lottie_ecg = lv_lottie_create(health_screen);
    lv_lottie_set_src_data(lottie_ecg, lottie_ecg_wave, lottie_ecg_wave_size);
    lv_obj_remove_style_all(lottie_ecg);
    lv_obj_set_size(lottie_ecg, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_x(lottie_ecg, 0);
    lv_obj_set_y(lottie_ecg, -60);
    lv_obj_set_align(lottie_ecg, LV_ALIGN_CENTER);
    lv_obj_remove_flag(lottie_ecg, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(lottie_ecg, LV_OBJ_FLAG_HIDDEN);
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    /*If there are no special requirements, just declare a buffer
        x4 because the Lottie is rendered in ARGB8888 format*/
    static uint8_t ecg_buf[410 * 176 * 4];
    lv_lottie_set_buffer(lottie_ecg, 410, 176, ecg_buf);
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(ecg_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(lottie_ecg, &ecg_buf);
#endif


    LV_FONT_DECLARE(font_space_grotesk_regular_28);
    lv_obj_t * label = lv_label_create(health_screen);
    lv_obj_set_x(label, 0);
    lv_obj_set_y(label, 25);
    lv_obj_set_align(label, LV_ALIGN_TOP_MID);
    lv_label_set_text(label, "13:37");
    lv_obj_set_style_text_font(label, &font_space_grotesk_regular_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * info_cont = lv_obj_create(health_screen);
    lv_obj_remove_style_all(info_cont);
    lv_obj_set_width(info_cont, 229);
    lv_obj_set_height(info_cont, 116);
    lv_obj_set_x(info_cont, 54);
    lv_obj_set_y(info_cont, 201);
    lv_obj_remove_flag(info_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    LV_FONT_DECLARE(font_space_grotesk_regular_23);
    label = lv_label_create(info_cont);
    lv_label_set_text(label, "Current");
    lv_obj_set_style_text_font(label, &font_space_grotesk_regular_23, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * text_cont = lv_obj_create(info_cont);
    lv_obj_remove_style_all(text_cont);
    lv_obj_set_width(text_cont, 185);
    lv_obj_set_height(text_cont, 71);
    lv_obj_set_x(text_cont, 0);
    lv_obj_set_y(text_cont, -30);
    lv_obj_set_align(text_cont, LV_ALIGN_BOTTOM_LEFT);
    lv_obj_remove_flag(text_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_row(text_cont, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(text_cont, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(font_space_grotesk_regular_80);
    label = lv_label_create(text_cont);
    lv_obj_set_x(label, 0);
    lv_obj_set_y(label, 18);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label, "64");
    lv_obj_set_style_text_font(label, &font_space_grotesk_regular_80, LV_PART_MAIN | LV_STATE_DEFAULT);

    label = lv_label_create(text_cont);
    lv_obj_set_x(label, -3);
    lv_obj_set_y(label, 5);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_RIGHT);
    lv_label_set_text(label, "BPM");
    lv_obj_set_style_text_color(label, lv_color_hex(0xE3573D), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &font_space_grotesk_regular_28, LV_PART_MAIN | LV_STATE_DEFAULT);

    label = lv_label_create(info_cont);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_LEFT);
    lv_label_set_text(label, "49bpm, 3 min ago");
    lv_obj_set_style_text_font(label, &font_space_grotesk_regular_23, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_IMAGE_DECLARE(image_health_off);
    image_button = lv_image_create(health_screen);
    lv_image_set_src(image_button, &image_health_off);
    lv_obj_set_width(image_button, LV_SIZE_CONTENT);
    lv_obj_set_height(image_button, LV_SIZE_CONTENT);
    lv_obj_set_x(image_button, 270);
    lv_obj_set_y(image_button, 191);
    lv_obj_add_flag(image_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(image_button, LV_OBJ_FLAG_SCROLLABLE);


    extern uint8_t lottie_heart_small[];
    extern size_t lottie_heart_small_size;
    heart_icon = lv_lottie_create(health_screen);
    lv_lottie_set_src_data(heart_icon, lottie_heart_small, lottie_heart_small_size);
    lv_obj_set_size(heart_icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_x(heart_icon, 0);
    lv_obj_set_y(heart_icon, -30);
    lv_obj_set_align(heart_icon, LV_ALIGN_CENTER);
    lv_obj_add_flag(heart_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(heart_icon, LV_OBJ_FLAG_SCROLLABLE);
#if LV_DRAW_BUF_ALIGN == 4 && LV_DRAW_BUF_STRIDE_ALIGN == 1
    /*If there are no special requirements, just declare a buffer
        x4 because the Lottie is rendered in ARGB8888 format*/
    static uint8_t heart_buf[90 * 77 * 4];
    lv_lottie_set_buffer(heart_icon, 90, 77, heart_buf);
#else
    /*For GPUs and special alignment/strid setting use a draw_buf instead*/
    LV_DRAW_BUF_DEFINE(heart_buf, 64, 64, LV_COLOR_FORMAT_ARGB8888);
    lv_lottie_set_draw_buf(heart_icon, &heart_buf);
#endif

    lv_obj_t * click_cont = lv_obj_create(health_screen);
    lv_obj_remove_style_all(click_cont);
    lv_obj_set_width(click_cont, 63);
    lv_obj_set_height(click_cont, 65);
    lv_obj_set_x(click_cont, 115);
    lv_obj_set_y(click_cont, 37);
    lv_obj_set_align(click_cont, LV_ALIGN_CENTER);
    lv_obj_remove_flag(click_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(click_cont, button_click_event, LV_EVENT_CLICKED, NULL);

}

lv_obj_t * lv_demo_smartwatch_get_health_screen(void)
{
    if(!health_screen) {
        lv_demo_smartwatch_health_create();
    }
    return health_screen;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void button_click_event(lv_event_t * e)
{
    LV_UNUSED(e);
    LV_IMAGE_DECLARE(image_health_on);
    LV_IMAGE_DECLARE(image_health_off);
    if(lv_obj_has_flag(lottie_ecg, LV_OBJ_FLAG_HIDDEN)) {
        lv_obj_remove_flag(lottie_ecg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(heart_bg_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(heart_icon, LV_OBJ_FLAG_HIDDEN);
        lv_image_set_src(image_button, &image_health_on);
    }
    else {
        lv_obj_add_flag(lottie_ecg, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(heart_bg_2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(heart_icon, LV_OBJ_FLAG_HIDDEN);
        lv_image_set_src(image_button, &image_health_off);
    }
}

static void health_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if(dir == LV_DIR_LEFT) {
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_health_screen(), -384 - TRANSITION_GAP, 1000, 0);
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_sports_screen(), 384 + TRANSITION_GAP, 0, 1000, 0);
        }
        if(dir == LV_DIR_RIGHT) {
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_weather_screen(), -384 - TRANSITION_GAP, 0, 1000, 0);
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_health_screen(), 384 + TRANSITION_GAP, 1000, 0);
        }

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
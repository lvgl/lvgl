/**
 * @file lv_demo_smartwatch.c
 * Smartwatch general functions.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_control.h"
#include "lv_demo_smartwatch_weather.h"
#include "lv_demo_smartwatch_health.h"
#include "lv_demo_smartwatch_sports.h"
#include "lv_demo_smartwatch_music.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void rotate_image(void * var, int32_t v);

static void shrink_to_left(void * var, int32_t v);
static void shrink_to_down(void * var, int32_t v);
static void expand_from_left(void * var, int32_t v);
static void expand_from_down(void * var, int32_t v);

static void translate_x_anim(void * var, int32_t v);
static void translate_y_anim(void * var, int32_t v);
static void opa_anim(void * var, int32_t v);

static void home_screen_events(lv_event_t * e);


static uint32_t arc_colors[] = {

    0xB70074, /* Purple */
    0x792672, /* Violet */
    0x193E8D, /* Deep Blue */
    0x179DD6, /* Light Blue */
    0x009059, /* Dark Green */
    0x5FB136, /* Light Green */
    0xF9EE19, /* Yellow */
    0xF6D10E, /* Ocher */
    0xEF7916, /* Cream */

    0xEA4427, /* Orange */
    0xEB0F13, /* Red */
    0xD50059, /* Garnet */
};

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_theme_t * theme_original;
static lv_obj_t * home_screen;

static lv_anim_t rotate;

/**********************
 *  GLOBAL VARIABLES
 **********************/

lv_obj_t * arc_cont;
lv_obj_t * arc;
lv_obj_t * overlay;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch(void)
{

    /*Use the simple theme to make styling simpler*/
    lv_display_t * display = lv_display_get_default();
    theme_original = lv_display_get_theme(display);
    lv_theme_t * theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_display_set_theme(display, theme);

    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_bg_color(&main_style, lv_color_black());
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
    }

    lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), 0);

    home_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(home_screen);
    lv_obj_set_size(home_screen, lv_pct(100), lv_pct(100));
    lv_obj_add_style(home_screen, &main_style, 0);
    lv_obj_remove_flag(home_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_remove_flag(home_screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(home_screen, home_screen_events, LV_EVENT_ALL, NULL);

    /* The home screen watchface */
    LV_IMAGE_DECLARE(image_arcoiris_background);
    lv_obj_t * image = lv_image_create(home_screen);
    lv_image_set_src(image, &image_arcoiris_background);

    LV_FONT_DECLARE(font_roboto_serif_bold_164);
    lv_obj_t * label = lv_label_create(home_screen);
    lv_label_set_text(label, "23");
    lv_obj_set_y(label, -172);
    lv_obj_set_align(label, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_text_font(label, &font_roboto_serif_bold_164, 0);

    label = lv_label_create(home_screen);
    lv_label_set_text(label, "57");
    lv_obj_set_align(label, LV_ALIGN_TOP_MID);
    lv_obj_set_y(label, 172);
    lv_obj_set_style_text_font(label, &font_roboto_serif_bold_164, 0);

    lv_obj_t * date_cont = lv_obj_create(home_screen);
    lv_obj_remove_style_all(date_cont);
    lv_obj_set_pos(date_cont, 285, 175);
    lv_obj_set_flex_flow(date_cont, LV_FLEX_FLOW_COLUMN);

    LV_FONT_DECLARE(font_inter_regular_24);
    label = lv_label_create(date_cont);
    lv_label_set_text(label, "07/12");
    lv_obj_set_style_text_letter_space(label, 1, 0);
    lv_obj_set_style_text_font(label, &font_inter_regular_24, 0);

    LV_FONT_DECLARE(font_inter_bold_24);
    label = lv_label_create(date_cont);
    lv_label_set_text(label, "WED");
    lv_obj_set_style_text_letter_space(label, 3, 0);
    lv_obj_set_style_text_font(label, &font_inter_bold_24, 0);

    LV_IMAGE_DECLARE(image_sun_icon);
    image = lv_image_create(home_screen);
    lv_image_set_src(image, &image_sun_icon);
    lv_obj_set_x(image, 56);
    lv_obj_set_align(image, LV_ALIGN_LEFT_MID);

    /* Black overlay for screen transitions */
    overlay = lv_obj_create(home_screen);
    lv_obj_remove_style_all(overlay);
    lv_obj_set_size(overlay, lv_pct(100), lv_pct(100));
    lv_obj_remove_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(overlay, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(overlay, 255, 0);
    lv_obj_set_style_opa(overlay, 0, 0);


    /* Create the other screens */
    lv_demo_smartwatch_control_create();
    lv_demo_smartwatch_weather_create();
    lv_demo_smartwatch_health_create();
    lv_demo_smartwatch_sports_create();
    lv_demo_smartwatch_music_create();



    /* Container for the outer arcs */
    arc_cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(arc_cont);
    lv_obj_set_size(arc_cont, lv_pct(100), lv_pct(100));
    lv_obj_remove_flag(arc_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);

    /* Create the outer arcs */
    for(uint32_t i = 0; i < 12; i++) {
        lv_obj_t * obj = lv_arc_create(arc_cont);
        lv_obj_remove_style_all(obj);
        lv_obj_set_size(obj, 384, 384);
        lv_obj_set_align(obj, LV_ALIGN_CENTER);
        lv_arc_set_bg_start_angle(obj, 0);
        lv_arc_set_bg_end_angle(obj, 30);
        lv_arc_set_rotation(obj, ARC_POS(i));
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);

        lv_obj_set_style_arc_width(obj, 27, 0);
        lv_obj_set_style_arc_rounded(obj, false, 0);
        lv_obj_set_style_arc_color(obj, lv_color_hex(arc_colors[i]), 0);
        lv_obj_set_style_arc_opa(obj, 255, 0);

    }


    /* The rotating arc */
    arc = lv_arc_create(lv_screen_active());
    lv_obj_remove_style_all(arc);
    lv_obj_set_size(arc, 384, 384);
    lv_obj_set_align(arc, LV_ALIGN_CENTER);
    lv_arc_set_bg_start_angle(arc, 0);
    lv_arc_set_bg_end_angle(arc, 45);
    lv_arc_set_rotation(arc, 0);
    lv_obj_remove_flag(arc, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_arc_width(arc, 192, 0);
    lv_obj_set_style_arc_rounded(arc, false, 0);
    lv_obj_set_style_arc_color(arc, lv_color_white(), 0);
    lv_obj_set_style_arc_opa(arc, 255, 0);
    lv_obj_set_style_blend_mode(arc, LV_BLEND_MODE_DIFFERENCE, 0);
    lv_obj_set_style_opa(arc, 255, 0);

    /* Animate the rotating arc */
    lv_anim_init(&rotate);
    lv_anim_set_var(&rotate, arc);
    lv_anim_set_values(&rotate, 0, 360);
    lv_anim_set_duration(&rotate, 30000);
    lv_anim_set_exec_cb(&rotate, rotate_image);
    lv_anim_set_repeat_count(&rotate, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&rotate);




}


void animate_arc(lv_obj_t * obj, arc_animation_t animation, int32_t duration, int32_t delay)
{

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);
    switch(animation) {
        case ARC_SHRINK_DOWN:
            lv_anim_set_exec_cb(&a, shrink_to_down);
            break;
        case ARC_EXPAND_UP:
            lv_anim_set_exec_cb(&a, expand_from_down);
            break;
        case ARC_SHRINK_LEFT:
            lv_anim_set_exec_cb(&a, shrink_to_left);
            break;
        case ARC_EXPAND_RIGHT:
            lv_anim_set_exec_cb(&a, expand_from_left);
            break;
    }

    lv_anim_start(&a);
}

void ui_animate_x(lv_obj_t * obj, int32_t x, int32_t duration, int32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_values(&a, lv_obj_get_style_translate_x(obj, 0), x);
    lv_anim_set_exec_cb(&a, translate_x_anim);
    lv_anim_start(&a);
}

void ui_animate_x_from(lv_obj_t * obj, int32_t start, int32_t x, int32_t duration, int32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_values(&a, start, x);
    lv_anim_set_exec_cb(&a, translate_x_anim);
    lv_anim_start(&a);
}

void ui_animate_y(lv_obj_t * obj, int32_t y, int32_t duration, int32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_values(&a, lv_obj_get_style_translate_y(obj, 0), y);
    lv_anim_set_exec_cb(&a, translate_y_anim);
    lv_anim_start(&a);
}

void ui_anim_opa(lv_obj_t * obj, lv_opa_t opa, int32_t duration, int32_t delay)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, duration);
    lv_anim_set_delay(&a, delay);
    lv_anim_set_values(&a, lv_obj_get_style_opa(obj, 0), opa);
    lv_anim_set_exec_cb(&a, opa_anim);
    lv_anim_start(&a);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void translate_x_anim(void * var, int32_t v)
{
    lv_obj_set_style_translate_x(var, v, 0);
}

static void translate_y_anim(void * var, int32_t v)
{
    lv_obj_set_style_translate_y(var, v, 0);
}

static void opa_anim(void * var, int32_t v)
{
    lv_obj_set_style_opa(var, v, 0);
}

static void rotate_image(void * var, int32_t v)
{
    lv_arc_set_rotation(var, v);
}

static void shrink_to_left(void * var, int32_t v)
{
    lv_obj_t * parent = var;
    for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * obj = lv_obj_get_child(parent, i);
        int32_t orig = ARC_POS(i);
        int32_t target = 165; /* Left side is 180 - (half_arc) [180 - 15]*/
        int32_t final = orig + ((target - orig) * v) / 100;
        int32_t thick = 27 + ((5 - 27) * v) / 100;
        lv_color_hsv_t hsv = lv_color_to_hsv(lv_color_hex(arc_colors[i]));
        hsv.s = 100 - v;
        hsv.v = hsv.v + ((100 - hsv.v) * v) / 100;
        lv_arc_set_rotation(obj, final);
        lv_obj_set_style_arc_width(obj, thick, 0);
        lv_obj_set_style_arc_color(obj, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), 0);
    }
}

static void shrink_to_down(void * var, int32_t v)
{
    lv_obj_t * parent = var;
    for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * obj = lv_obj_get_child(parent, i);
        int32_t orig = ARC_POS(i);
        int32_t target = 75;  /* Bottom side is 90 - (half_arc) [90 - 15]*/
        int32_t final = orig + ((target - orig) * v) / 100;
        int32_t thick = 27 + ((5 - 27) * v) / 100;
        lv_color_hsv_t hsv = lv_color_to_hsv(lv_color_hex(arc_colors[i]));
        hsv.s = 100 - v;
        hsv.v = hsv.v + ((100 - hsv.v) * v) / 100;
        lv_arc_set_rotation(obj, final);
        lv_obj_set_style_arc_width(obj, thick, 0);
        lv_obj_set_style_arc_color(obj, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), 0);
    }
}

static void expand_from_left(void * var, int32_t v)
{
    lv_obj_t * parent = var;
    for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * obj = lv_obj_get_child(parent, i);
        int32_t orig = 165;
        int32_t target = ARC_POS(i);
        int32_t final = orig + ((target - orig) * v) / 100;
        int32_t thick = 5 + ((27 - 5) * v) / 100;
        lv_color_hsv_t hsv = lv_color_to_hsv(lv_color_hex(arc_colors[i]));
        hsv.s = v;
        hsv.v = 100 + ((hsv.v - 100) * v) / 100;
        lv_arc_set_rotation(obj, final);
        lv_obj_set_style_arc_width(obj, thick, 0);
        lv_obj_set_style_arc_color(obj, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), 0);
    }
}

static void expand_from_down(void * var, int32_t v)
{
    lv_obj_t * parent = var;
    for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
        lv_obj_t * obj = lv_obj_get_child(parent, i);
        int32_t orig = 75;
        int32_t target = ARC_POS(i);
        int32_t final = orig + ((target - orig) * v) / 100;
        int32_t thick = 5 + ((27 - 5) * v) / 100;
        lv_color_hsv_t hsv = lv_color_to_hsv(lv_color_hex(arc_colors[i]));
        hsv.s = v;
        hsv.v = 100 + ((hsv.v - 100) * v) / 100;
        lv_arc_set_rotation(obj, final);
        lv_obj_set_style_arc_width(obj, thick, 0);
        lv_obj_set_style_arc_color(obj, lv_color_hsv_to_rgb(hsv.h, hsv.s, hsv.v), 0);

    }

}

static void home_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
        if(dir == LV_DIR_BOTTOM) {
            ui_animate_y(lv_demo_smartwatch_get_control_screen(), 0, 800, 200);
            animate_arc(arc_cont, ARC_SHRINK_DOWN, 1000, 0);
            ui_anim_opa(arc, 0, 700, 0);

        }
        if(dir == LV_DIR_LEFT) {
            ui_animate_x_from(lv_demo_smartwatch_get_weather_screen(), 384, 0, 800, 200);
            animate_arc(arc_cont, ARC_SHRINK_LEFT, 1000, 0);
            ui_anim_opa(arc, 0, 700, 0);
        }
    }
}



#endif /*LV_USE_DEMO_SMARTWATCH*/

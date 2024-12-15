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
#include "lv_demo_smartwatch_home.h"
#include "lv_demo_smartwatch_list.h"
#include "lv_demo_smartwatch_notifications.h"
#include "lv_demo_smartwatch_settings.h"
#include "lv_demo_smartwatch_control.h"
#include "lv_demo_smartwatch_weather.h"
#include "lv_demo_smartwatch_qr.h"
#include "lv_demo_smartwatch_easter_egg.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void create_dialog_window(void);
static void dialog_close_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_theme_t * theme_original;
static bool circular_scroll;
static bool load_app_list;

static lv_obj_t * dialog_parent;
static lv_obj_t * dialog_window;
static lv_obj_t * dialog_panel;
static lv_obj_t * dialog_title;
static lv_obj_t * dialog_message;
static lv_obj_t * dialog_close;
static lv_obj_t * dialog_close_label;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_set_circular_scroll(bool state)
{
    circular_scroll = state;
}

void lv_demo_smartwatch(void)
{

    /*Use the simple the to make styling simpler*/
    lv_display_t * display = lv_display_get_default();
    theme_original = lv_display_get_theme(display);
    lv_theme_t * theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_display_set_theme(display, theme);

    lv_demo_smartwatch_set_circular_scroll(true);

    lv_demo_smartwatch_home_create();

    lv_demo_smartwatch_list_create();

    lv_demo_smartwatch_notifications_create();

    lv_demo_smartwatch_settings_create();

    lv_demo_smartwatch_control_create();

    lv_demo_smartwatch_weather_create();

    lv_demo_smartwatch_qr_create();

    lv_demo_smartwatch_easter_egg_create();

    create_dialog_window();

    /* load the logo screen immediately, more like a boot logo */
    lv_demo_smartwatch_easter_egg_load(LV_SCR_LOAD_ANIM_FADE_IN, 0, 0);

    /* load the home screen after 2500ms */
    lv_demo_smartwatch_home_load(LV_SCR_LOAD_ANIM_FADE_IN, 500, 2500);
}

void lv_demo_smartwatch_scroll_event(lv_event_t * e)
{

    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t * list = lv_event_get_target(e);
    if(event_code == LV_EVENT_SCROLL) {
        lv_area_t list_a;
        lv_obj_get_coords(list, &list_a);
        lv_coord_t list_y_center = list_a.y1 + lv_area_get_height(&list_a) / 2;

        lv_coord_t r = lv_obj_get_height(list) * 7 / 10;
        uint32_t i;
        uint32_t child_cnt = lv_obj_get_child_count(list);
        for(i = 0; i < child_cnt; i++) {
            lv_obj_t * child = lv_obj_get_child(list, i);
            lv_area_t child_a;
            lv_obj_get_coords(child, &child_a);

            lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

            lv_coord_t diff_y = child_y_center - list_y_center;
            diff_y = LV_ABS(diff_y);

            /*Get the x of diff_y on a circle.*/
            lv_coord_t x;
            /*If diff_y is out of the circle use the last point of the circle (the radius)*/
            if(diff_y >= r) {
                x = r;
            }
            else {
                /*Use Pythagoras theorem to get x from radius and y*/
                uint32_t x_sqr = r * r - diff_y * diff_y;
                lv_sqrt_res_t res;
                lv_sqrt(x_sqr, &res, 0x8000); /*Use lvgl's built in sqrt root function*/
                x = r - res.i;
            }

            /*Translate the item by the calculated X coordinate*/
            lv_obj_set_style_translate_x(child, circular_scroll ? x : 0, 0);

        }
    }
}

void lv_demo_smartwatch_show_dialog(const char * title, const char * message)
{
    lv_disp_t * display = lv_display_get_default();
    lv_obj_t * active_screen = lv_display_get_screen_active(display);

    /*  attach the dialog window to current active screen */
    lv_obj_set_parent(dialog_window, active_screen);

    lv_label_set_text(dialog_title, title);
    lv_label_set_text(dialog_message, message);
    lv_obj_remove_flag(dialog_window, LV_OBJ_FLAG_HIDDEN); /* show the dialog */
    lv_obj_scroll_to_y(dialog_window, 0, LV_ANIM_ON);
}

void lv_demo_smartwatch_set_load_app_list(bool state)
{
    load_app_list = state;
}

bool lv_demo_smartwatch_get_load_app_list(void)
{
    return load_app_list;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void dialog_close_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_CLICKED) {
        lv_obj_add_flag(dialog_window, LV_OBJ_FLAG_HIDDEN);
    }

}

static void create_dialog_window(void)
{
    dialog_parent = lv_obj_create(NULL);

    dialog_window = lv_obj_create(dialog_parent);
    lv_obj_set_width(dialog_window, lv_pct(100));
    lv_obj_set_height(dialog_window, lv_pct(100));
    lv_obj_set_align(dialog_window, LV_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(dialog_window, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(dialog_window, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(dialog_window, LV_OBJ_FLAG_GESTURE_BUBBLE); /* block gesture events when active */
    lv_obj_set_scroll_dir(dialog_window, LV_DIR_VER);
    lv_obj_set_style_radius(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(dialog_window, lv_color_hex(0x555555), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(dialog_window, 150, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(dialog_window, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(dialog_window, 60, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_panel = lv_obj_create(dialog_window);
    lv_obj_set_width(dialog_panel, 167);
    lv_obj_set_height(dialog_panel, LV_SIZE_CONTENT);
    lv_obj_set_x(dialog_panel, 0);
    lv_obj_set_y(dialog_panel, 60);
    lv_obj_set_align(dialog_panel, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(dialog_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(dialog_panel, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(dialog_panel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(dialog_panel, lv_color_hex(0x080404), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(dialog_panel, 235, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(dialog_panel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(dialog_panel, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dialog_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(dialog_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(dialog_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(dialog_panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(dialog_panel, 8, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_title = lv_label_create(dialog_panel);
    lv_obj_set_width(dialog_title, 140);
    lv_obj_set_height(dialog_title, LV_SIZE_CONTENT);
    lv_obj_set_align(dialog_title, LV_ALIGN_TOP_MID);
    lv_label_set_long_mode(dialog_title, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(dialog_title, "Title");
    lv_obj_set_style_text_align(dialog_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(dialog_title, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(dialog_title, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(dialog_title, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(dialog_title, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(dialog_title, LV_BORDER_SIDE_BOTTOM, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(dialog_title, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(dialog_title, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_message = lv_label_create(dialog_panel);
    lv_obj_set_width(dialog_message, 140);
    lv_obj_set_height(dialog_message, LV_SIZE_CONTENT);
    lv_obj_set_align(dialog_message, LV_ALIGN_CENTER);
    lv_label_set_text(dialog_message, "Dialog message");
    lv_obj_set_style_text_font(dialog_message, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_close = lv_button_create(dialog_panel);
    lv_obj_set_width(dialog_close, 100);
    lv_obj_set_height(dialog_close, LV_SIZE_CONTENT);
    lv_obj_set_align(dialog_close, LV_ALIGN_BOTTOM_MID);
    lv_obj_add_flag(dialog_close, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
    lv_obj_remove_flag(dialog_close, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(dialog_close, 20, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_close_label = lv_label_create(dialog_close);
    lv_obj_set_width(dialog_close_label, LV_SIZE_CONTENT);
    lv_obj_set_height(dialog_close_label, LV_SIZE_CONTENT);
    lv_obj_set_align(dialog_close_label, LV_ALIGN_CENTER);
    lv_label_set_text(dialog_close_label, "Close");
    lv_obj_set_style_text_font(dialog_close_label, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(dialog_close, dialog_close_event_cb, LV_EVENT_ALL, NULL);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/

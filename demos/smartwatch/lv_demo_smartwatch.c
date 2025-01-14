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
static void create_scroll_hints(void);
static void lv_demo_obj_set_hidden(lv_obj_t * obj, bool state);
static void dialog_close_event_cb(lv_event_t * e);
static void lv_create_home_tile(void);
static void home_tileview_event_cb(lv_event_t * e);

static lv_anim_t * obj_set_opacity_anim(lv_obj_t * obj);
static void anim_set_opacity_cb(lv_anim_t * a, int32_t v);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_theme_t * theme_original;
static bool circular_scroll;
static bool show_scroll_hints;
static bool load_app_list;
static bool first_load;
static lv_scrollbar_mode_t scrollbar_mode;

static lv_obj_t * home_tile;
static lv_obj_t * dialog_parent;
static lv_obj_t * dialog_window;
static lv_obj_t * dialog_panel;
static lv_obj_t * dialog_title;
static lv_obj_t * dialog_message;
static lv_obj_t * dialog_close;
static lv_obj_t * dialog_close_label;
static lv_obj_t * hint_panel;
static lv_obj_t * hint_up;
static lv_obj_t * hint_down;
static lv_obj_t * hint_left;
static lv_obj_t * hint_right;

static lv_anim_t * anim_up;
static lv_anim_t * anim_down;
static lv_anim_t * anim_left;
static lv_anim_t * anim_right;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch(void)
{

    /*Use the simple the to make styling simpler*/
    lv_display_t * display = lv_display_get_default();
    theme_original = lv_display_get_theme(display);
    lv_theme_t * theme = lv_theme_default_init(display, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),
                                               true, LV_FONT_DEFAULT);
    lv_display_set_theme(display, theme);

    create_dialog_window();

    create_scroll_hints();

    lv_create_home_tile();

    lv_demo_smartwatch_set_circular_scroll(true);

    lv_demo_smartwatch_control_create(home_tile);

    lv_demo_smartwatch_home_create(home_tile);

    lv_demo_smartwatch_list_create(home_tile);

    lv_tileview_set_tile_by_index(home_tile, 0, 1, LV_ANIM_OFF);

    lv_demo_smartwatch_notifications_create();

    lv_demo_smartwatch_settings_create();

    lv_demo_smartwatch_weather_create();

    lv_demo_smartwatch_qr_create();

    lv_demo_smartwatch_easter_egg_create();

    lv_demo_smartwatch_set_default_scrollbar_mode(LV_SCROLLBAR_MODE_OFF);

    lv_demo_smartwatch_set_default_hints_state(true);

    lv_demo_smartwatch_set_default_brightness(100);

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
    lv_obj_t * active_screen = lv_screen_active();

    if(lv_obj_check_type(active_screen, &lv_tileview_class)) {
        /* attach the dialog window to the current active tile */
        lv_obj_set_parent(dialog_window, lv_tileview_get_tile_active(active_screen));
    }
    else {
        /*  attach the dialog window to current active screen */
        lv_obj_set_parent(dialog_window, active_screen);
    }

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

lv_obj_t * lv_demo_smartwatch_get_tileview(void)
{
    return home_tile;
}

void lv_demo_smartwatch_home_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{
    lv_screen_load_anim(home_tile, anim_type, time, delay, false);
}

void lv_demo_smartwatch_load_home_watchface(void)
{
    lv_obj_t * active_screen = lv_screen_active();
    if(active_screen != home_tile) {
        lv_screen_load_anim(home_tile, LV_SCR_LOAD_ANIM_FADE_IN, 500, 0, false);
    }
    if(lv_tileview_get_tile_active(home_tile) != lv_demo_smartwatch_get_tile_home()) {
        lv_tileview_set_tile_by_index(home_tile, 0, 1, LV_ANIM_OFF);
    }
}

lv_scrollbar_mode_t lv_demo_smartwatch_get_scrollbar_mode(void)
{
    return scrollbar_mode;
}

void lv_demo_smartwatch_set_scrollbar_mode(lv_scrollbar_mode_t mode)
{
    scrollbar_mode = mode;
}

void lv_demo_smartwatch_set_circular_scroll(bool state)
{
    circular_scroll = state;
}

void lv_demo_smartwatch_set_scroll_hint(bool state)
{
    show_scroll_hints = state;
}

bool lv_demo_smartwatch_get_scroll_hint(void)
{
    return show_scroll_hints;
}

void lv_demo_smartwatch_show_scroll_hint(lv_dir_t dir)
{

    if(!show_scroll_hints) {
        /* scroll hints are disabled, make sure hint panel is hidden */
        lv_demo_obj_set_hidden(hint_panel, true);
        return;
    }

    lv_demo_obj_set_hidden(hint_up, (dir & LV_DIR_TOP) != LV_DIR_TOP);
    lv_demo_obj_set_hidden(hint_down, (dir & LV_DIR_BOTTOM) != LV_DIR_BOTTOM);
    lv_demo_obj_set_hidden(hint_left, (dir & LV_DIR_LEFT) != LV_DIR_LEFT);
    lv_demo_obj_set_hidden(hint_right, (dir & LV_DIR_RIGHT) != LV_DIR_RIGHT);

    lv_obj_t * active_screen = lv_screen_active();
    if(lv_obj_check_type(active_screen, &lv_tileview_class)) {
        /* attach the hint panel to the current active tile */
        lv_obj_set_parent(hint_panel, lv_tileview_get_tile_active(active_screen));
    }
    else {
        /*  attach the hint panel to current active screen */
        lv_obj_set_parent(hint_panel, active_screen);
    }

    lv_demo_obj_set_hidden(hint_panel, dir == LV_DIR_NONE);


    /* Delete animations if they exist*/
    lv_anim_delete(hint_up, NULL);
    lv_anim_delete(hint_down, NULL);
    lv_anim_delete(hint_left, NULL);
    lv_anim_delete(hint_right, NULL);

    /* Set max opacity to make icons visible */
    lv_obj_set_style_opa(hint_down, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(hint_up, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(hint_left, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(hint_right, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    if(dir != LV_DIR_NONE) {
        anim_up = obj_set_opacity_anim(hint_up);
        anim_down = obj_set_opacity_anim(hint_down);
        anim_left = obj_set_opacity_anim(hint_left);
        anim_right = obj_set_opacity_anim(hint_right);
    }
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void home_tileview_event_cb(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if(event_code == LV_EVENT_SCREEN_LOADED) {
        if(!first_load) {
            first_load = true;
            /* run the analog seconds animation on first load */
            lv_demo_smartwatch_face_update_seconds(0);
        }

        /* show the possible scroll directions hint */
        if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_home()) {
            lv_demo_smartwatch_show_home_hint(true);
        }
        else if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_app_list()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
        else if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_control()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM);
        }
    }

    if(event_code == LV_EVENT_SCREEN_LOAD_START) {
        lv_obj_set_scrollbar_mode(home_tile, lv_demo_smartwatch_get_scrollbar_mode());
        lv_demo_smartwatch_app_list_loading();
    }

    if(event_code == LV_EVENT_VALUE_CHANGED) {
        if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_home()) {
            lv_demo_smartwatch_show_home_hint(true);
        }
        else if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_app_list()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
        else if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_control()) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM);
        }
    }

    if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_home() && lv_screen_active() == home_tile) {

        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
            lv_demo_smartwatch_set_load_app_list(false); /* flag was not open from app list */
            lv_demo_smartwatch_notifications_load(LV_SCR_LOAD_ANIM_OVER_RIGHT, 500, 0);
        }

        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_TOP) {
            lv_demo_smartwatch_set_load_app_list(false); /* flag was not open from app list */
            lv_demo_smartwatch_weather_load(LV_SCR_LOAD_ANIM_MOVE_TOP, 500, 0);
        }
    }

    if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_control() && lv_screen_active() == home_tile) {
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_BOTTOM) {
            lv_demo_smartwatch_qr_load(LV_SCR_LOAD_ANIM_MOVE_BOTTOM, 500, 0);
        }
        else if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) != LV_DIR_TOP) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_TOP | LV_DIR_BOTTOM);
        }
    }

    if(lv_tileview_get_tile_active(home_tile) == lv_demo_smartwatch_get_tile_app_list() &&
       lv_screen_active() == home_tile) {
        if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) != LV_DIR_RIGHT) {
            lv_demo_smartwatch_show_scroll_hint(LV_DIR_LEFT);
        }
    }


}

static void lv_create_home_tile(void)
{
    home_tile = lv_tileview_create(NULL);
    lv_obj_set_scrollbar_mode(home_tile, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_style_bg_color(home_tile, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(home_tile, 55, LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_add_event_cb(home_tile, home_tileview_event_cb, LV_EVENT_ALL, NULL);

}

static void dialog_close_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_obj_add_flag(dialog_window, LV_OBJ_FLAG_HIDDEN);
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
    lv_obj_set_width(dialog_panel, lv_pct(80));
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
    lv_obj_set_width(dialog_title, lv_pct(80));
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
    lv_obj_set_width(dialog_message, lv_pct(80));
    lv_obj_set_height(dialog_message, LV_SIZE_CONTENT);
    lv_obj_set_align(dialog_message, LV_ALIGN_CENTER);
    lv_label_set_text(dialog_message, "Dialog message");
    lv_obj_set_style_text_font(dialog_message, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    dialog_close = lv_button_create(dialog_panel);
    lv_obj_set_width(dialog_close, lv_pct(60));
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

    lv_obj_add_event_cb(dialog_close, dialog_close_event_cb, LV_EVENT_CLICKED, NULL);
}

static void create_scroll_hints(void)
{
    hint_panel = lv_obj_create(dialog_parent);

    lv_obj_set_width(hint_panel, lv_pct(100));
    lv_obj_set_height(hint_panel, lv_pct(100));
    lv_obj_set_align(hint_panel, LV_ALIGN_CENTER);
    lv_obj_add_flag(hint_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(hint_panel, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_panel, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(hint_panel, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hint_panel, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_up = lv_image_create(hint_panel);
    lv_image_set_src(hint_up, &img_up_arrow_icon);
    lv_image_set_scale(hint_up, 200);
    lv_obj_set_width(hint_up, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_up, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_up, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(hint_up, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_up, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_up, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_up, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_up, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(hint_up, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hint_up, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_down = lv_image_create(hint_panel);
    lv_image_set_src(hint_down, &img_up_arrow_icon);
    lv_image_set_rotation(hint_down, 1800);
    lv_image_set_scale(hint_down, 200);
    lv_obj_set_width(hint_down, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_down, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_down, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(hint_down, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_down, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_down, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_down, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_down, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(hint_down, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(hint_down, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_left = lv_image_create(hint_panel);
    lv_image_set_src(hint_left, &img_up_arrow_icon);
    lv_image_set_rotation(hint_left, 2700);
    lv_image_set_scale(hint_left, 200);
    lv_obj_set_width(hint_left, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_left, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_left, LV_ALIGN_LEFT_MID);
    lv_obj_remove_flag(hint_left, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_left, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_left, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_left, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_left, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hint_left, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hint_left, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

    hint_right = lv_image_create(hint_panel);
    lv_image_set_src(hint_right, &img_up_arrow_icon);
    lv_image_set_rotation(hint_right, 900);
    lv_image_set_scale(hint_right, 200);
    lv_obj_set_width(hint_right, LV_SIZE_CONTENT);
    lv_obj_set_height(hint_right, LV_SIZE_CONTENT);
    lv_obj_set_align(hint_right, LV_ALIGN_RIGHT_MID);
    lv_obj_remove_flag(hint_right, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(hint_right, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_radius(hint_right, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(hint_right, lv_color_hex(0x333333), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(hint_right, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(hint_right, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(hint_right, 5, LV_PART_MAIN | LV_STATE_DEFAULT);
}

static void lv_demo_obj_set_hidden(lv_obj_t * obj, bool state)
{
    if(state) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void anim_set_opacity_cb(lv_anim_t * a, int32_t v)
{
    lv_obj_t * target = (lv_obj_t *)a->user_data;
    lv_obj_set_style_opa(target, v, 0);
}

static lv_anim_t * obj_set_opacity_anim(lv_obj_t * obj)
{
    lv_anim_t anim_opa;
    lv_anim_init(&anim_opa);
    lv_anim_set_var(&anim_opa, obj);
    lv_anim_set_values(&anim_opa, 255, 0);
    lv_anim_set_duration(&anim_opa, 1000);
    lv_anim_set_delay(&anim_opa, 1000);
    lv_anim_set_user_data(&anim_opa, obj);
    lv_anim_set_custom_exec_cb(&anim_opa, anim_set_opacity_cb);
    return lv_anim_start(&anim_opa);
}

#endif /*LV_USE_DEMO_SMARTWATCH*/

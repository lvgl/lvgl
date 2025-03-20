/**
 * @file lv_demo_smartwatch_control.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_control.h"

/*********************
 *      DEFINES
 *********************/

LV_IMAGE_DECLARE(image_mute_icon);
LV_IMAGE_DECLARE(image_dnd_icon);
LV_IMAGE_DECLARE(image_flashlight_icon);
LV_IMAGE_DECLARE(image_flight_icon);
LV_IMAGE_DECLARE(image_cellular_icon);
LV_IMAGE_DECLARE(image_repeat_icon);
LV_IMAGE_DECLARE(image_night_icon);
LV_IMAGE_DECLARE(image_brightness_icon);
LV_IMAGE_DECLARE(image_video_icon);
LV_IMAGE_DECLARE(image_battery_icon);
LV_IMAGE_DECLARE(image_wifi_icon);
LV_IMAGE_DECLARE(image_silence_icon);
LV_IMAGE_DECLARE(image_bluetooth_icon);
LV_IMAGE_DECLARE(image_qr_icon);
LV_IMAGE_DECLARE(image_settings_icon);
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void control_screen_events(lv_event_t * e);
static void scroll_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * control_screen;


static const lv_image_dsc_t * control_icons[] = {
    &image_mute_icon,
    &image_dnd_icon,
    &image_flashlight_icon,
    &image_flight_icon,
    &image_cellular_icon,
    &image_repeat_icon,
    &image_night_icon,
    &image_brightness_icon,
    &image_video_icon,
    &image_battery_icon,
    &image_wifi_icon,
    &image_silence_icon,
    &image_bluetooth_icon,
    &image_qr_icon,
    &image_settings_icon
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_control_create(void)
{

    static bool inited = false;
    static lv_style_t main_style;

    if(!inited) {
        lv_style_init(&main_style);
        lv_style_set_text_color(&main_style, lv_color_white());
        lv_style_set_bg_color(&main_style, lv_color_hex(0xff4a4a4a));
        lv_style_set_bg_opa(&main_style, LV_OPA_100);
        lv_style_set_clip_corner(&main_style, true);
        lv_style_set_radius(&main_style, LV_RADIUS_CIRCLE);
        lv_style_set_translate_y(&main_style, -384);

        lv_style_set_layout(&main_style, LV_LAYOUT_FLEX);
        lv_style_set_flex_flow(&main_style, LV_FLEX_FLOW_COLUMN);
        lv_style_set_flex_main_place(&main_style, LV_FLEX_ALIGN_START);
        lv_style_set_flex_cross_place(&main_style, LV_FLEX_ALIGN_CENTER);
        lv_style_set_flex_track_place(&main_style, LV_FLEX_ALIGN_CENTER);
        lv_style_set_pad_top(&main_style, 80);
        lv_style_set_pad_bottom(&main_style, 0);
        lv_style_set_pad_row(&main_style, 20);
    }

    control_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(control_screen);
    lv_obj_add_style(control_screen, &main_style, 0);
    lv_obj_set_size(control_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scroll_dir(control_screen, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(control_screen, LV_SCROLL_SNAP_START);
    lv_obj_remove_flag(control_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_add_event_cb(control_screen, control_screen_events, LV_EVENT_ALL, NULL);
    lv_obj_add_event_cb(control_screen, scroll_event_cb, LV_EVENT_SCROLL, NULL);


    /* Create the 5 row containers */
    for(uint32_t i = 0; i < 5; i++) {
        lv_obj_t * inner_cont = lv_obj_create(control_screen);
        lv_obj_remove_style_all(inner_cont);
        lv_obj_set_size(inner_cont, 345, 102);

        lv_obj_set_flex_flow(inner_cont, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(inner_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_add_flag(inner_cont, LV_OBJ_FLAG_SNAPPABLE);
        lv_obj_add_flag(inner_cont, LV_OBJ_FLAG_SCROLL_ON_FOCUS);


        /* Add 3 buttons to each row container */
        for(uint32_t j = 0; j < 3; j++) {
            lv_obj_t * item = lv_obj_create(inner_cont);
            lv_obj_remove_style_all(item);
            lv_obj_set_size(item, 102, 102);

            lv_obj_set_style_bg_color(item, lv_color_hex(0xffffffff), 0);
            lv_obj_set_style_bg_opa(item, 46, 0);
            lv_obj_set_style_radius(item, 50, 0);
            lv_obj_set_style_bg_color(item, lv_color_hex(0xff438bff), LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_set_style_bg_opa(item, 255, LV_PART_MAIN | LV_STATE_CHECKED);
            lv_obj_add_flag(item, LV_OBJ_FLAG_CHECKABLE);

            lv_obj_t * image = lv_image_create(item);
            lv_image_set_src(image, control_icons[(i * 3) + j]);
            lv_obj_set_align(image, LV_ALIGN_CENTER);
            lv_obj_remove_flag(image, LV_OBJ_FLAG_CLICKABLE);
        }
    }

    lv_obj_update_snap(control_screen, LV_ANIM_ON);
    lv_obj_scroll_by(control_screen, 0, -1, LV_ANIM_ON);

}


lv_obj_t * lv_demo_smartwatch_get_control_screen(void)
{
    if(!control_screen) {
        lv_demo_smartwatch_control_create();
    }
    return control_screen;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void control_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());

        if(dir == LV_DIR_LEFT) {
            lv_smartwatch_animate_y(lv_demo_smartwatch_get_control_screen(), -384, 1000, 0);
            lv_smartwatch_animate_arc(arc_cont, ARC_EXPAND_UP, 700, 300);
            lv_smartwatch_anim_opa(main_arc, 255, 500, 500);
        }

    }
    if(event_code == LV_EVENT_LONG_PRESSED) {
        lv_smartwatch_animate_y(lv_demo_smartwatch_get_control_screen(), -384, 1000, 0);
        lv_smartwatch_animate_arc(arc_cont, ARC_EXPAND_UP, 700, 300);
        lv_smartwatch_anim_opa(main_arc, 255, 500, 500);
    }
}

static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = (lv_obj_t *)lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    int32_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    int32_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_count(cont);
    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        int32_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        int32_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /* Smallest size default parameters */
        int32_t icon_size = 82; /* Full size is 102 */
        int32_t cont_width = 262; /* Full size is 345 */
        int32_t img_scale = 208; /* Full size is 256 */

        /*If diff_y is within the circle calculate size */
        if(diff_y < r) {
            /* Calculate sizes the based on diff_y and radius */
            icon_size = (int32_t)(82 + (20 * lv_pow(lv_trigo_cos((90 * diff_y) / r), 2) / LV_TRIGO_SIN_MAX / LV_TRIGO_SIN_MAX));
            cont_width = (int32_t)(262 + (83 * lv_pow(lv_trigo_cos((90 * diff_y) / r), 2) / LV_TRIGO_SIN_MAX / LV_TRIGO_SIN_MAX));
            img_scale = (int32_t)(208 + (48 * lv_pow(lv_trigo_cos((90 * diff_y) / r), 2) / LV_TRIGO_SIN_MAX / LV_TRIGO_SIN_MAX));

        }
        /* Adjust the width of the container */
        lv_obj_set_width(child, cont_width);

        for(uint32_t c = 0; c < lv_obj_get_child_count(child); c++) {
            lv_obj_t * grand = lv_obj_get_child(child, c);
            /* Adjust the size of icon container */
            lv_obj_set_size(grand, icon_size, icon_size);

            /* Adjust the scale of the image inside the icon container */
            lv_image_set_scale(lv_obj_get_child(grand, 0), img_scale);
        }
    }
}



#endif /*LV_USE_DEMO_SMARTWATCH*/
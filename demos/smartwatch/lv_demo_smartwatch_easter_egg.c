/**
 * @file lv_demo_smartwatch_easter_egg.c
 * Easter Egg screen - LVGL animated logo.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_easter_egg.h"
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
static void create_screen_easter_egg(void);
static void create_screen_event_cb(lv_event_t * e);

static void get_random_position(lv_coord_t * x, lv_coord_t * y, uint64_t * a);
static void set_obj_pos_angle(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, uint64_t a);
static void animate_drift(lv_obj_t * img);
static void animate_back_to_center(lv_obj_t * img, uint32_t delay);
static void drift_timer_cb(lv_timer_t * timer);
static void start_drift_animation(void);
static void stop_drift_animation(void);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_obj_t * easter_egg_screen;
static lv_obj_t * lvgl_logo_black;
static lv_obj_t * lvgl_logo_blue;
static lv_obj_t * lvgl_logo_green;
static lv_obj_t * lvgl_logo_red;

static lv_timer_t * drift_timer_black = NULL; /* Timer for the black logo */
static lv_timer_t * drift_timer_blue = NULL; /* Timer for the blue logo */
static lv_timer_t * drift_timer_green = NULL; /* Timer for the green logo */
static lv_timer_t * drift_timer_red = NULL;  /* Timer for the red logo */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_easter_egg_create(void)
{
    create_screen_easter_egg();
}

void lv_demo_smartwatch_easter_egg_load(lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay)
{

    /* set positions and angle on load */
    set_obj_pos_angle(lvgl_logo_black, -88, 88, 2700);
    set_obj_pos_angle(lvgl_logo_blue, -88, -88, 2700);
    set_obj_pos_angle(lvgl_logo_green, 88, 88, 2700);
    set_obj_pos_angle(lvgl_logo_red, 88, -88, 2700);

    /* Return all images to the center with an animation */
    animate_back_to_center(lvgl_logo_black, 1000);
    animate_back_to_center(lvgl_logo_blue, 1000);
    animate_back_to_center(lvgl_logo_green, 1000);
    animate_back_to_center(lvgl_logo_red, 1000);

    lv_screen_load_anim(easter_egg_screen, anim_type, time, delay, false);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Function to generate random target positions within the screen, keeping the images visible
 * @param x random x coordinate to be returned
 * @param y random y coordinate to be returned
 * @param a random rotation angle to be returned
 */
static void get_random_position(lv_coord_t * x, lv_coord_t * y, uint64_t * a)
{
    *x = lv_rand(-88, 88);
    *y = lv_rand(-88, 88);
    *a = lv_rand(0, 3600); /* Random rotation between (0 and 360 degrees) * 10 */
}


static void set_obj_pos_angle(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, uint64_t a)
{
    if(obj == NULL) {
        LV_LOG_WARN("set_obj_pos_angle() object is null");
        return;
    }
    lv_obj_set_x(obj, x);
    lv_obj_set_y(obj, y);
    lv_image_set_rotation(obj, a);
}

/**
 * Animation callback to move images to a new random position
 * @param img the image object to animate
 */
static void animate_drift(lv_obj_t * img)
{
    lv_coord_t x_target;
    lv_coord_t y_target;
    uint64_t a_target;
    get_random_position(&x_target, &y_target, &a_target);

    lv_anim_t anim_x;
    lv_anim_init(&anim_x);
    lv_anim_set_var(&anim_x, img);
    lv_anim_set_values(&anim_x, lv_obj_get_x_aligned(img), x_target);
    lv_anim_set_duration(&anim_x, 3000);
    lv_anim_set_exec_cb(&anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&anim_x);

    lv_anim_t anim_y;
    lv_anim_init(&anim_y);
    lv_anim_set_var(&anim_y, img);
    lv_anim_set_values(&anim_y, lv_obj_get_y_aligned(img), y_target);
    lv_anim_set_duration(&anim_y, 3000);
    lv_anim_set_exec_cb(&anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&anim_y);

    lv_anim_t anim_a;
    lv_anim_init(&anim_a);
    lv_anim_set_var(&anim_a, img);
    lv_anim_set_values(&anim_a, lv_image_get_rotation(img), a_target);
    lv_anim_set_duration(&anim_a, 3000);
    lv_anim_set_exec_cb(&anim_a, (lv_anim_exec_xcb_t)lv_image_set_rotation);
    lv_anim_start(&anim_a);
}

/**
 * Function to gradually drift images back to the center (0, 0) when touched
 * @param img the image object to animate
 */
static void animate_back_to_center(lv_obj_t * img, uint32_t delay)
{
    lv_anim_t anim_x;
    lv_anim_init(&anim_x);
    lv_anim_set_var(&anim_x, img);
    lv_anim_set_values(&anim_x, lv_obj_get_x_aligned(img), 0);
    lv_anim_set_duration(&anim_x, 1000);
    lv_anim_set_delay(&anim_x, delay);
    lv_anim_set_exec_cb(&anim_x, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_start(&anim_x);

    lv_anim_t anim_y;
    lv_anim_init(&anim_y);
    lv_anim_set_var(&anim_y, img);
    lv_anim_set_values(&anim_y, lv_obj_get_y_aligned(img), 0);
    lv_anim_set_duration(&anim_y, 1000);
    lv_anim_set_delay(&anim_y, delay);
    lv_anim_set_exec_cb(&anim_y, (lv_anim_exec_xcb_t)lv_obj_set_y);
    lv_anim_start(&anim_y);

    lv_anim_t anim_a;
    lv_anim_init(&anim_a);
    lv_anim_set_var(&anim_a, img);
    lv_anim_set_values(&anim_a, lv_image_get_rotation(img), 0);
    lv_anim_set_duration(&anim_a, 1000);
    lv_anim_set_delay(&anim_a, delay);
    lv_anim_set_exec_cb(&anim_a, (lv_anim_exec_xcb_t)lv_image_set_rotation);
    lv_anim_start(&anim_a);
}

/**
 * Timer callback for continuous drift
 */
static void drift_timer_cb(lv_timer_t * timer)
{
    lv_obj_t * img = timer->user_data;
    animate_drift(img);
}

/**
 * Start drift animations on screen load
 */
static void start_drift_animation(void)
{
    /* Create a timer for each image to trigger the drift animation repeatedly */
    drift_timer_black = lv_timer_create(drift_timer_cb, 3000, lvgl_logo_black);
    lv_timer_set_repeat_count(drift_timer_black, -1);

    drift_timer_blue = lv_timer_create(drift_timer_cb, 3000, lvgl_logo_blue);
    lv_timer_set_repeat_count(drift_timer_blue, -1);

    drift_timer_green = lv_timer_create(drift_timer_cb, 3000, lvgl_logo_green);
    lv_timer_set_repeat_count(drift_timer_green, -1);

    drift_timer_red = lv_timer_create(drift_timer_cb, 3000, lvgl_logo_red);
    lv_timer_set_repeat_count(drift_timer_red, -1);
}

/**
 * Stop animations on screen unload
 */
static void stop_drift_animation(void)
{
    /* Stop and delete each timer individually */
    if(drift_timer_black != NULL) {
        lv_timer_delete(drift_timer_black);
        drift_timer_black = NULL;
    }

    if(drift_timer_blue != NULL) {
        lv_timer_delete(drift_timer_blue);
        drift_timer_blue = NULL;
    }

    if(drift_timer_green != NULL) {
        lv_timer_delete(drift_timer_green);
        drift_timer_green = NULL;
    }

    if(drift_timer_red != NULL) {
        lv_timer_delete(drift_timer_red);
        drift_timer_red = NULL;
    }
}

static void create_screen_easter_egg(void)
{

    easter_egg_screen = lv_obj_create(NULL);
    lv_obj_remove_flag(easter_egg_screen, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(easter_egg_screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(easter_egg_screen, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    lvgl_logo_black = lv_image_create(easter_egg_screen);
    lv_image_set_src(lvgl_logo_black, &img_lvgl_logo_black_icon);
    lv_obj_set_width(lvgl_logo_black, LV_SIZE_CONTENT);
    lv_obj_set_height(lvgl_logo_black, LV_SIZE_CONTENT);
    lv_obj_set_align(lvgl_logo_black, LV_ALIGN_CENTER);
    lv_obj_remove_flag(lvgl_logo_black, LV_OBJ_FLAG_SCROLLABLE);

    lvgl_logo_blue = lv_image_create(easter_egg_screen);
    lv_image_set_src(lvgl_logo_blue, &img_lvgl_logo_blue_icon);
    lv_obj_set_width(lvgl_logo_blue, LV_SIZE_CONTENT);
    lv_obj_set_height(lvgl_logo_blue, LV_SIZE_CONTENT);
    lv_obj_set_align(lvgl_logo_blue, LV_ALIGN_CENTER);
    lv_obj_remove_flag(lvgl_logo_blue, LV_OBJ_FLAG_SCROLLABLE);

    lvgl_logo_red = lv_image_create(easter_egg_screen);
    lv_image_set_src(lvgl_logo_red, &img_lvgl_logo_red_icon);
    lv_obj_set_width(lvgl_logo_red, LV_SIZE_CONTENT);
    lv_obj_set_height(lvgl_logo_red, LV_SIZE_CONTENT);
    lv_obj_set_align(lvgl_logo_red, LV_ALIGN_CENTER);
    lv_obj_remove_flag(lvgl_logo_red, LV_OBJ_FLAG_SCROLLABLE);

    lvgl_logo_green = lv_image_create(easter_egg_screen);
    lv_image_set_src(lvgl_logo_green, &img_lvgl_logo_green_icon);
    lv_obj_set_width(lvgl_logo_green, LV_SIZE_CONTENT);
    lv_obj_set_height(lvgl_logo_green, LV_SIZE_CONTENT);
    lv_obj_set_align(lvgl_logo_green, LV_ALIGN_CENTER);
    lv_obj_remove_flag(lvgl_logo_green, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_add_event_cb(easter_egg_screen, create_screen_event_cb, LV_EVENT_ALL, NULL);
}

static void create_screen_event_cb(lv_event_t * e)
{
    lv_obj_t * active_screen = lv_screen_active();
    if(active_screen != easter_egg_screen) {
        /* event was triggered but the current screen is no longer active */
        return;
    }

    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE && lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        lv_demo_smartwatch_settings_load(LV_SCR_LOAD_ANIM_FADE_OUT, 500, 0);
        return;
    }
    if(event_code == LV_EVENT_SCREEN_LOAD_START) {

    }
    if(event_code == LV_EVENT_SCREEN_LOADED) {
        start_drift_animation();
    }
    if(event_code == LV_EVENT_SCREEN_UNLOAD_START) {
        stop_drift_animation();

        animate_back_to_center(lvgl_logo_black, 0);
        animate_back_to_center(lvgl_logo_blue, 0);
        animate_back_to_center(lvgl_logo_green, 0);
        animate_back_to_center(lvgl_logo_red, 0);

    }

    if(event_code == LV_EVENT_PRESSED) {
        stop_drift_animation();

        /* Return all images to the center */
        animate_back_to_center(lvgl_logo_black, 0);
        animate_back_to_center(lvgl_logo_blue, 0);
        animate_back_to_center(lvgl_logo_green, 0);
        animate_back_to_center(lvgl_logo_red, 0);
    }
    else if(event_code == LV_EVENT_RELEASED) {
        start_drift_animation();
    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/

/**
 * @file lv_demo_smartwatch_music.c
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_smartwatch.h"
#if LV_USE_DEMO_SMARTWATCH

#include "lv_demo_smartwatch_private.h"
#include "lv_demo_smartwatch_music.h"

/*********************
 *      DEFINES
 *********************/

LV_IMAGE_DECLARE(image_album_cover_1);
LV_IMAGE_DECLARE(image_album_cover_2);
LV_IMAGE_DECLARE(image_album_cover_3);
LV_IMAGE_DECLARE(image_album_cover_4);

#define MUSIC_OBJECTS 4
#define RADIUS_SMALL 40
#define RADIUS_LARGE 78

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint32_t color;
    const void * cover;
    const char * artist;
    const char * track;
} music_info_t;

typedef struct {
    lv_obj_t * obj;
    int position;
} music_object_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void music_screen_events(lv_event_t * e);

static void music_play_event_cb(lv_event_t * e);
static void music_next_event_cb(lv_event_t * e);
static void music_previous_event_cb(lv_event_t * e);
static void drag_event_handler(lv_event_t * e);

static void music_rotation_cb(void * var, int32_t v);
static void animate_rotation(int32_t angle);
static void rotate_objects(int32_t angle);
static void rotation_complete_cb(lv_anim_t * a);
static void radius_animation_cb(void * var, int32_t v);
static void animate_radius(int32_t target);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * music_screen;
static lv_obj_t * label_time;
static lv_obj_t * arc_volume;
static lv_obj_t * cont_music;
static lv_obj_t * button_play;
static lv_obj_t * arc_progress;
static lv_obj_t * button_previous;
static lv_obj_t * button_next;
static lv_obj_t * cont_info;
static lv_obj_t * label_artist;
static lv_obj_t * label_track;
static lv_obj_t * image_volume;

static int32_t radius = RADIUS_SMALL;

static music_info_t music_list[] = {
    {0x36A1C7, &image_album_cover_1, "Jeff Black", "Wired"},
    {0xFB4129, &image_album_cover_2, "Primal Scream", "Screamdelica"},
    {0x7b5845, &image_album_cover_3, "Bill Evanz", "Portrait in Jazz"},
    {0xF4E36A, &image_album_cover_4, "The Doors", "Morrison Hotel"},
};

static music_object_t objects[MUSIC_OBJECTS];
static int32_t last_angle;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_smartwatch_music_create(void)
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

        lv_style_set_bg_grad_color(&main_style, lv_color_hex(0x000000));
        lv_style_set_bg_main_stop(&main_style, 0);
        lv_style_set_bg_grad_stop(&main_style, 255);
        lv_style_set_bg_grad_dir(&main_style, LV_GRAD_DIR_VER);

    }

    music_screen = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(music_screen);
    lv_obj_add_style(music_screen, &main_style, 0);
    lv_obj_set_size(music_screen, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(music_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_remove_flag(music_screen, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(music_screen, LV_OBJ_FLAG_CLICKABLE);

    lv_obj_add_event_cb(music_screen, music_screen_events, LV_EVENT_ALL, NULL);


    LV_FONT_DECLARE(font_inter_regular_28);
    label_time = lv_label_create(music_screen);
    lv_obj_set_width(label_time, LV_SIZE_CONTENT);
    lv_obj_set_height(label_time, LV_SIZE_CONTENT);
    lv_obj_set_x(label_time, 0);
    lv_obj_set_y(label_time, 20);
    lv_obj_set_align(label_time, LV_ALIGN_TOP_MID);
    lv_label_set_text(label_time, "13:37");
    lv_obj_set_style_text_font(label_time, &font_inter_regular_28, 0);

    arc_volume = lv_arc_create(music_screen);
    lv_obj_set_width(arc_volume, 350);
    lv_obj_set_height(arc_volume, 350);
    lv_obj_set_align(arc_volume, LV_ALIGN_CENTER);
    lv_arc_set_value(arc_volume, 50);
    lv_arc_set_bg_angles(arc_volume, 345, 25);
    lv_arc_set_mode(arc_volume, LV_ARC_MODE_REVERSE);
    lv_obj_set_style_arc_width(arc_volume, 12, 0);
    lv_obj_set_style_arc_color(arc_volume, lv_color_hex(0xBBBBBB), 0);
    lv_obj_set_style_arc_opa(arc_volume, 100, 0);

    lv_obj_set_style_arc_color(arc_volume, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_volume, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_volume, 12, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_volume, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_volume, 0, LV_PART_KNOB | LV_STATE_DEFAULT);


    cont_music = lv_obj_create(music_screen);
    lv_obj_remove_style_all(cont_music);
    lv_obj_set_width(cont_music, 216);
    lv_obj_set_height(cont_music, 80);
    lv_obj_set_x(cont_music, 0);
    lv_obj_set_y(cont_music, 135);
    lv_obj_set_align(cont_music, LV_ALIGN_CENTER);
    lv_obj_remove_flag(cont_music, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    LV_IMAGE_DECLARE(image_pause_icon);
    button_play = lv_obj_create(cont_music);
    lv_obj_remove_style_all(button_play);
    lv_obj_set_width(button_play, 68);
    lv_obj_set_height(button_play, 68);
    lv_obj_set_align(button_play, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(button_play, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(button_play, 34, 0);
    lv_obj_set_style_bg_color(button_play, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(button_play, 50, 0);
    lv_obj_set_style_bg_image_src(button_play, &image_pause_icon, 0);
    lv_obj_set_style_bg_color(button_play, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button_play, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(button_play, music_play_event_cb, LV_EVENT_CLICKED, NULL);

    arc_progress = lv_arc_create(cont_music);
    lv_obj_set_width(arc_progress, 68);
    lv_obj_set_height(arc_progress, 68);
    lv_obj_set_align(arc_progress, LV_ALIGN_BOTTOM_MID);
    lv_obj_remove_flag(arc_progress, LV_OBJ_FLAG_CLICKABLE);
    lv_arc_set_value(arc_progress, 50);
    lv_arc_set_bg_angles(arc_progress, 0, 360);
    lv_arc_set_rotation(arc_progress, 270);
    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0xBBBBBB), 0);
    lv_obj_set_style_arc_opa(arc_progress, 100, 0);
    lv_obj_set_style_arc_width(arc_progress, 3, 0);

    lv_obj_set_style_arc_color(arc_progress, lv_color_hex(0x07D685), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(arc_progress, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(arc_progress, 3, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(arc_progress, false, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(arc_progress, lv_color_hex(0xFFFFFF), LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(arc_progress, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    LV_IMAGE_DECLARE(image_previous_icon);
    button_previous = lv_obj_create(cont_music);
    lv_obj_remove_style_all(button_previous);
    lv_obj_set_width(button_previous, 56);
    lv_obj_set_height(button_previous, 56);
    lv_obj_remove_flag(button_previous, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(button_previous, 34, 0);
    lv_obj_set_style_bg_color(button_previous, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(button_previous, 50, 0);
    lv_obj_set_style_bg_image_src(button_previous, &image_previous_icon, 0);
    lv_obj_set_style_bg_color(button_previous, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button_previous, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(button_previous, music_previous_event_cb, LV_EVENT_CLICKED, NULL);

    LV_IMAGE_DECLARE(image_next_icon);
    button_next = lv_obj_create(cont_music);
    lv_obj_remove_style_all(button_next);
    lv_obj_set_width(button_next, 56);
    lv_obj_set_height(button_next, 56);
    lv_obj_set_align(button_next, LV_ALIGN_TOP_RIGHT);
    lv_obj_remove_flag(button_next, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(button_next, 34, 0);
    lv_obj_set_style_bg_color(button_next, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(button_next, 50, 0);
    lv_obj_set_style_bg_image_src(button_next, &image_next_icon, 0);
    lv_obj_set_style_bg_color(button_next, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_set_style_bg_opa(button_next, 200, LV_PART_MAIN | LV_STATE_PRESSED);
    lv_obj_add_event_cb(button_next, music_next_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * image_cont = lv_obj_create(music_screen);
    lv_obj_remove_style_all(image_cont);
    lv_obj_set_width(image_cont, 312);
    lv_obj_set_height(image_cont, 156);
    lv_obj_set_x(image_cont, -1);
    lv_obj_set_y(image_cont, 66);
    lv_obj_set_align(image_cont, LV_ALIGN_TOP_MID);
    lv_obj_remove_flag(image_cont, LV_OBJ_FLAG_GESTURE_BUBBLE | LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(image_cont, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_event_cb(image_cont, drag_event_handler, LV_EVENT_ALL, NULL);

    for(int i = 0; i < MUSIC_OBJECTS;  i++) {
        objects[i].position = i * 90;
        objects[i].obj = lv_image_create(image_cont);
        lv_image_set_src(objects[i].obj, music_list[i].cover);
        lv_obj_set_x(objects[i].obj, objects[i].position);
        lv_obj_set_align(objects[i].obj, LV_ALIGN_TOP_MID);
        lv_obj_remove_flag(objects[i].obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_flag(objects[i].obj, LV_OBJ_FLAG_SCROLLABLE);
    }

    cont_info = lv_obj_create(music_screen);
    lv_obj_remove_style_all(cont_info);
    lv_obj_set_width(cont_info, 245);
    lv_obj_set_height(cont_info, 55);
    lv_obj_set_x(cont_info, 4);
    lv_obj_set_y(cont_info, 227);
    lv_obj_set_align(cont_info, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_flow(cont_info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont_info, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_flag(cont_info, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);

    LV_FONT_DECLARE(font_inter_regular_24);
    label_artist = lv_label_create(cont_info);
    lv_obj_set_align(label_artist, LV_ALIGN_CENTER);
    lv_label_set_text(label_artist, "Jeff Black");
    lv_obj_set_style_text_font(label_artist, &font_inter_regular_24, 0);

    label_track = lv_label_create(cont_info);
    lv_obj_set_align(label_track, LV_ALIGN_CENTER);
    lv_label_set_text(label_track, "Wired");
    lv_obj_set_style_text_font(label_track, &font_inter_regular_24, 0);
    lv_obj_set_style_opa(label_track, LV_OPA_80, 0);

    LV_IMAGE_DECLARE(image_volume_icon);
    image_volume = lv_image_create(music_screen);
    lv_image_set_src(image_volume, &image_volume_icon);
    lv_obj_set_x(image_volume, 313);
    lv_obj_set_y(image_volume, 274);
    lv_obj_add_flag(image_volume, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_remove_flag(image_volume, LV_OBJ_FLAG_SCROLLABLE);


    /* Re-arrange the album objects */
    rotate_objects(0);

}

lv_obj_t * lv_demo_smartwatch_get_music_screen(void)
{
    if(!music_screen) {
        lv_demo_smartwatch_music_create();
    }
    return music_screen;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Rotate the music album objects
 * @param angle the angle to rotate the objects. This is the difference rather that absolute position.
 */
static void rotate_objects(int32_t angle)
{
    for(int i = 0; i < MUSIC_OBJECTS; i++) {
        objects[i].position = (objects[i].position + angle) % 360;
        int32_t x = (lv_trigo_cos(objects[i].position) * radius) / LV_TRIGO_SIN_MAX;
        int32_t y = (lv_trigo_sin(objects[i].position) * radius) / LV_TRIGO_SIN_MAX;

        /* Here we use y variable as the z-index */
        if(y > radius / 2) {
            lv_obj_move_foreground(objects[i].obj);

            lv_label_set_text(label_artist, music_list[i].artist);
            lv_label_set_text(label_track, music_list[i].track);
            lv_obj_set_style_bg_color(music_screen, lv_color_hex(music_list[i].color), 0);

        }
        else if(y < -radius / 2) {
            lv_obj_move_background(objects[i].obj);
        }

        int32_t min_scale = lv_map(radius, RADIUS_SMALL, RADIUS_LARGE, 128, 156);
        int32_t max_scale = lv_map(radius, RADIUS_SMALL, RADIUS_LARGE, 240, 256);

        lv_image_set_scale(objects[i].obj, lv_map(y, -radius, radius, min_scale, max_scale));
        lv_obj_set_x(objects[i].obj, x);
    }

}


static void music_rotation_cb(void * var, int32_t v)
{
    LV_UNUSED(var);
    int32_t delta = v - last_angle;  /* Compute the difference from the last step */
    last_angle = v;  /* Update last_angle for the next call */
    rotate_objects(delta);
}

static void radius_animation_cb(void * var, int32_t v)
{
    LV_UNUSED(var);
    radius = v;
    rotate_objects(0);
}

static void animate_radius(int32_t target)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, radius_animation_cb);
    lv_anim_set_values(&a, radius, target);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

static void rotation_complete_cb(lv_anim_t * a)
{
    LV_UNUSED(a);

    int32_t end_angle = objects[0].position;

    if(end_angle % 90 != 0) {
        /* Animation completed but angle was not multiple of 90 */
        LV_LOG_WARN("end angle: %d", end_angle);

        /* Compute the nearest 90-degree target */
        int32_t nearest_angle = (end_angle + (end_angle >= 0 ? 45 : -45)) / 90 * 90;

        /* Calculate delta to nearest 90-degree */
        int32_t angle = nearest_angle - end_angle;

        animate_rotation(angle);
    }
}

static void animate_rotation(int32_t angle)
{
    last_angle = 0;
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, NULL);
    lv_anim_set_exec_cb(&a, music_rotation_cb);
    lv_anim_set_completed_cb(&a, rotation_complete_cb);
    lv_anim_set_values(&a, 0, angle);
    lv_anim_set_duration(&a, 500);
    lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
    lv_anim_start(&a);
}

static void music_next_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    /* Move forward by 90 degrees */
    animate_rotation(90);
}

static void music_previous_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    /* Move backward by 90 degrees */
    animate_rotation(-90);
}

static void music_play_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    static bool playing = true;
    playing = !playing;

    if(playing) {
        lv_arc_set_value(arc_progress, 50);
    }
    else {
        lv_arc_set_value(arc_progress, 0);
    }

}

static void drag_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED) {
    }

    if(code == LV_EVENT_FOCUSED) {
        animate_radius(RADIUS_LARGE);
    }
    if(code == LV_EVENT_DEFOCUSED) {
        animate_radius(RADIUS_SMALL);
    }


    if(code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_event_get_indev(e);
        lv_point_t vect;
        lv_indev_get_vect(indev, &vect); /* Get drag vector */

        /* Move the objects relative to the drag */
        rotate_objects(vect.x * -1); /* Vector is inverted, so multiply by -1 */

    }

    if(code == LV_EVENT_RELEASED) {
        /* When released calculate nearest 90 degree and move objects */
        int32_t current_angle = objects[0].position;

        /* Compute the nearest 90-degree target */
        int32_t nearest_angle = (current_angle + (current_angle >= 0 ? 45 : -45)) / 90 * 90;

        /* Calculate delta to nearest 90-degree */
        int32_t angle = nearest_angle - current_angle;

        animate_rotation(angle);

    }
}

static void music_screen_events(lv_event_t * e)
{
    lv_event_code_t event_code = lv_event_get_code(e);

    if(event_code == LV_EVENT_GESTURE) {
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());


        if(dir == LV_DIR_LEFT) {
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_music_screen(), -384, 1000, 0);
            lv_smartwatch_animate_arc(arc_cont, ARC_EXPAND_RIGHT, 700, 300);
            lv_smartwatch_anim_opa(main_arc, 255, 500, 500);
            lv_smartwatch_anim_opa(overlay, 0, 100, 0);
        }
        if(dir == LV_DIR_RIGHT) {
            lv_smartwatch_animate_x_from(lv_demo_smartwatch_get_sports_screen(), -384 - TRANSITION_GAP, 0, 1000, 0);
            lv_smartwatch_animate_x(lv_demo_smartwatch_get_music_screen(), 384 + TRANSITION_GAP, 1000, 0);
        }

    }
}

#endif /*LV_USE_DEMO_SMARTWATCH*/
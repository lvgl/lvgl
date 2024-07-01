#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_SLIDER && LV_USE_BUTTON && LV_USE_SWITCH

static struct { // globals
    lv_anim_t * animation;
    lv_obj_t * button_label;
    lv_obj_t * slider;
} g;

static void anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static int32_t lv_anim_path_bouncing(const lv_anim_t * a)
{
    /* ease in during reverse */
    if(a->playback_now)
        return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0), LV_BEZIER_VAL_FLOAT(0.2),
                                         LV_BEZIER_VAL_FLOAT(0.5), LV_BEZIER_VAL_FLOAT(1));
    /* ease out during forward */
    else
        return lv_anim_path_cubic_bezier(a, LV_BEZIER_VAL_FLOAT(0.2), LV_BEZIER_VAL_FLOAT(0),
                                         LV_BEZIER_VAL_FLOAT(1), LV_BEZIER_VAL_FLOAT(0.5));
}

static void btn_start_event_handler(lv_event_t * a)
{
    LV_UNUSED(a);
    lv_anim_toggle_running(g.animation);
}

static void on_animation_pause(lv_anim_t * a)
{
    LV_UNUSED(a);
    lv_label_set_text(g.button_label, "resume");
}

static void on_animation_resume(lv_anim_t * a)
{
    LV_UNUSED(a);
    lv_label_set_text(g.button_label, "pause");
}

static void on_slider_value_change(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_anim_set_current_time(g.animation, lv_slider_get_value(g.slider));
}

static void on_slider_pressed(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_anim_pause(g.animation);
}

static void on_slider_press_lost(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_anim_resume(g.animation);
}

/**
 * Demonstrate pause, resume, seek
 */
void lv_example_anim_4(void)
{
    lv_obj_t * red_dot = lv_obj_create(lv_screen_active());
    lv_obj_set_style_bg_color(red_dot, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_set_style_radius(red_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(red_dot, 15, 15);
    lv_obj_align(red_dot, LV_ALIGN_TOP_LEFT, 50, 10);

    g.animation = lv_anim_create();
    lv_anim_set_var(g.animation, red_dot);
    lv_anim_set_values(g.animation, 10, 220);
    lv_anim_set_duration(g.animation, 400);
    lv_anim_set_playback_duration(g.animation, 400);
    lv_anim_set_repeat_count(g.animation, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_path_cb(g.animation, lv_anim_path_bouncing);
    lv_anim_set_exec_cb(g.animation, anim_y_cb);
    lv_anim_set_pause_cb(g.animation, on_animation_pause);
    lv_anim_set_resume_cb(g.animation, on_animation_resume);
    lv_anim_trigger(g.animation);

    lv_obj_t * btn_start = lv_button_create(lv_screen_active());
    lv_obj_add_event_cb(btn_start, btn_start_event_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_flag(btn_start, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(btn_start, LV_ALIGN_BOTTOM_LEFT, 10, -10);

    g.button_label = lv_label_create(btn_start);
    lv_label_set_text(g.button_label, "pause");
    lv_obj_center(g.button_label);

    g.slider = lv_slider_create(lv_screen_active());
    lv_obj_add_event_cb(g.slider, on_slider_value_change, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(g.slider, on_slider_pressed, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(g.slider, on_slider_press_lost, LV_EVENT_RELEASED, NULL);
    lv_obj_add_flag(g.slider, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(g.slider, LV_ALIGN_BOTTOM_LEFT, 120, -20);
    lv_obj_set_size(g.slider, 150, 10);
    lv_slider_set_range(g.slider, 0, 400);
}

#endif

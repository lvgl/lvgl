#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_ANIM_TIMELINE_CUSTOM_EXEC && LV_USE_USER_DATA

static lv_obj_t * obj1 = NULL;
static lv_obj_t * obj2 = NULL;
static lv_obj_t * obj3 = NULL;

static lv_obj_t * btn = NULL;
static lv_obj_t * slider = NULL;

static const lv_coord_t obj_width = 150;
static const lv_coord_t obj_height = 200;

static void set_width(lv_anim_t * a, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *)a->user_data;
    lv_obj_set_width(obj, v);
}

static void set_height(lv_anim_t * a, int32_t v)
{
    lv_obj_t * obj = (lv_obj_t *)a->user_data;
    lv_obj_set_height(obj, v);
}

static void event_handler(lv_event_t * e)
{
    lv_anim_timeline_t anim_timeline[] = {
        {
            .start_time = 0,
            .var = obj1,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_width,
            .start_value = 0,
            .end_value = obj_width,
            .duration = 300,
            .path_cb = lv_anim_path_overshoot,
            .user_data = obj1
        },
        {
            .start_time = 0,
            .var = obj1,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_height,
            .start_value = 0,
            .end_value = obj_height,
            .duration = 300,
            .path_cb = lv_anim_path_ease_out,
            .user_data = obj1
        },
        {
            .start_time = 200,
            .var = obj2,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_width,
            .start_value = 0,
            .end_value = obj_width,
            .duration = 300,
            .path_cb = lv_anim_path_overshoot,
            .user_data = obj2
        },
        {
            .start_time = 200,
            .var = obj2,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_height,
            .start_value = 0,
            .end_value = obj_height,
            .duration = 300,
            .path_cb = lv_anim_path_ease_out,
            .user_data = obj2
        },
        {
            .start_time = 400,
            .var = obj3,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_width,
            .start_value = 0,
            .end_value = obj_width,
            .duration = 300,
            .path_cb = lv_anim_path_overshoot,
            .user_data = obj3
        },
        {
            .start_time = 400,
            .var = obj3,
            .exec_cb = (lv_anim_timeline_exec_cb_t)set_height,
            .start_value = 0,
            .end_value = obj_height,
            .duration = 300,
            .path_cb = lv_anim_path_ease_out,
            .user_data = obj3
        },
        LV_ANIM_TIMELINE_END
    };

    lv_obj_t * obj = lv_event_get_target(e);

    if (obj == btn) {
        bool playback = lv_obj_has_state(btn, LV_STATE_CHECKED);
        lv_anim_timeline_start(anim_timeline, playback);
    }
    else if (obj == slider) {
        int32_t progress = lv_slider_get_value(slider);
        lv_anim_timeline_set_progress(anim_timeline, progress);
    }
}

/**
 * Create an animation timeline
 */
void lv_example_anim_timeline_2(void)
{
    lv_obj_t * par = lv_scr_act();
    lv_obj_set_flex_flow(par, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(par, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    btn = lv_btn_create(par);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);

    slider = lv_slider_create(par);
    lv_obj_add_event_cb(slider, event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
    lv_slider_set_range(slider, 0, 65535);

    obj1 = lv_obj_create(par);
    lv_obj_set_size(obj1, obj_width, obj_height);

    obj2 = lv_obj_create(par);
    lv_obj_set_size(obj2, obj_width, obj_height);

    obj3 = lv_obj_create(par);
    lv_obj_set_size(obj3, obj_width, obj_height);
}

#endif

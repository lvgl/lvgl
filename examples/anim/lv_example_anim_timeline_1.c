#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES

static lv_obj_t * obj1 = NULL;
static lv_obj_t * obj2 = NULL;
static lv_obj_t * obj3 = NULL;

static lv_obj_t * btn = NULL;
static lv_obj_t * slider = NULL;

static lv_coord_t obj_width = 0;
static lv_coord_t obj_height = 0;

static void set_width(void * var, int32_t v)
{
    lv_obj_set_width((lv_obj_t *)var, v);
}

static void set_height(void * var, int32_t v)
{
    lv_obj_set_height((lv_obj_t *)var, v);
}

static void event_handler(lv_event_t * e)
{
    lv_anim_timeline_t anim_timeline[] = {
        {0,   obj1, (lv_anim_exec_xcb_t)set_width,  0, obj_width,  300, lv_anim_path_overshoot, false},
        {0,   obj1, (lv_anim_exec_xcb_t)set_height, 0, obj_height, 300, lv_anim_path_ease_out,  false},
        {200, obj2, (lv_anim_exec_xcb_t)set_width,  0, obj_width,  300, lv_anim_path_overshoot, false},
        {200, obj2, (lv_anim_exec_xcb_t)set_height, 0, obj_height, 300, lv_anim_path_ease_out,  false},
        {400, obj3, (lv_anim_exec_xcb_t)set_width,  0, obj_width,  300, lv_anim_path_overshoot, false},
        {400, obj3, (lv_anim_exec_xcb_t)set_height, 0, obj_height, 300, lv_anim_path_ease_out,  false},
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
void lv_example_anim_timeline_1(void)
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
    obj2 = lv_obj_create(par);
    obj3 = lv_obj_create(par);

    obj_width = lv_obj_get_style_width(obj1, 0);
    obj_height = lv_obj_get_style_height(obj1, 0);
}

#endif

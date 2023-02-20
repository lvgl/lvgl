#include "../../lv_examples.h"
#if LV_USE_MSG && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

static int32_t limit_value(int32_t v);
static void btn_event_cb(lv_event_t * e);
static void label_event_cb(lv_event_t * e);
static void slider_event_cb(lv_event_t * e);

static int32_t power_value;

/**
 * Show how an increment button, a decrement button, as slider can set a value
 * and a label display it.
 * The current value (i.e. the system's state) is stored only in one static variable in a function
 * and no global variables are required.
 */
void lv_example_msg_3(void)
{

    lv_obj_t * panel = lv_obj_create(lv_scr_act());
    lv_obj_set_size(panel, 250, LV_SIZE_CONTENT);
    lv_obj_center(panel);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t * btn;
    lv_obj_t * label;

    /*Up button*/
    btn = lv_btn_create(panel);
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_add_event(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_center(label);

    /*Current value*/
    label = lv_label_create(panel);
    lv_obj_set_flex_grow(label, 2);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label, "?");
    lv_msg_subscribe_obj((lv_msg_id_t)&power_value, label, NULL);
    lv_obj_add_event(label, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);

    /*Down button*/
    btn = lv_btn_create(panel);
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_add_event(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_RIGHT);
    lv_obj_center(label);

    /*Slider*/
    lv_obj_t * slider = lv_slider_create(panel);
    lv_obj_set_flex_grow(slider, 1);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_msg_subscribe_obj((lv_msg_id_t)&power_value, slider, NULL);
    lv_obj_add_event(slider, slider_event_cb, LV_EVENT_ALL, NULL);

    power_value = 30;
    lv_msg_update_value(&power_value);
}

static int32_t limit_value(int32_t v)
{
    return LV_CLAMP(30, v, 80);
}


static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        if(lv_obj_get_index(btn) == 0) {    /*First object is the dec. button*/
            power_value = limit_value(power_value - 1);
            lv_msg_update_value(&power_value);
        }
        else {
            power_value = limit_value(power_value + 1);
            lv_msg_update_value(&power_value);
        }
    }
}

static void label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        const int32_t * v = lv_msg_get_payload(m);
        lv_label_set_text_fmt(label, "%"LV_PRId32" %%", *v);
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        power_value = limit_value(lv_slider_get_value(slider));
        lv_msg_update_value(&power_value);
    }
    else if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        const int32_t * v = lv_msg_get_payload(m);
        lv_slider_set_value(slider, *v, LV_ANIM_OFF);
    }
}

#endif

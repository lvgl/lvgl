#include "../../lv_examples.h"
#if LV_USE_MSG && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

/*Define a message ID*/
#define MSG_INC             1
#define MSG_DEC             2
#define MSG_SET             3
#define MSG_UPDATE          4
#define MSG_UPDATE_REQUEST  5

static void value_handler(void * s, lv_msg_t * m);
static void value_handler(void * s, lv_msg_t * m);
static void btn_event_cb(lv_event_t * e);
static void label_event_cb(lv_event_t * e);
static void slider_event_cb(lv_event_t * e);

/**
 * Show how an increment button, a decrement button, as slider can set a value
 * and a label display it.
 * The current value (i.e. the system's state) is stored only in one static variable in a function
 * and no global variables are required.
 */
void lv_example_msg_3(void)
{

    lv_msg_subsribe(MSG_INC, value_handler, NULL);
    lv_msg_subsribe(MSG_DEC, value_handler, NULL);
    lv_msg_subsribe(MSG_SET, value_handler, NULL);
    lv_msg_subsribe(MSG_UPDATE, value_handler, NULL);
    lv_msg_subsribe(MSG_UPDATE_REQUEST, value_handler, NULL);

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
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_LEFT);
    lv_obj_center(label);

    /*Current value*/
    label = lv_label_create(panel);
    lv_obj_set_flex_grow(label, 2);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(label, "?");
    lv_msg_subsribe_obj(MSG_UPDATE, label, NULL);
    lv_obj_add_event_cb(label, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);

    /*Down button*/
    btn = lv_btn_create(panel);
    lv_obj_set_flex_grow(btn, 1);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);
    label = lv_label_create(btn);
    lv_label_set_text(label, LV_SYMBOL_RIGHT);
    lv_obj_center(label);

    /*Slider*/
    lv_obj_t * slider = lv_slider_create(panel);
    lv_obj_set_flex_grow(slider, 1);
    lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_ALL, NULL);
    lv_msg_subsribe_obj(MSG_UPDATE, slider, NULL);

    /* As there are new UI elements that don't know the system's state
     * send an UPDATE REQUEST message which will trigger an UPDATE message with the current value*/
    lv_msg_send(MSG_UPDATE_REQUEST, NULL);
}

static void value_handler(void * s, lv_msg_t * m)
{
    LV_UNUSED(s);

    static int32_t value = 10;
    int32_t old_value = value;
    switch(lv_msg_get_id(m)) {
        case MSG_INC:
            if(value < 100) value++;
            break;
        case MSG_DEC:
            if(value > 0) value--;
            break;
        case MSG_SET: {
                const int32_t * new_value = lv_msg_get_payload(m);
                value = *new_value;
            }
            break;
        case MSG_UPDATE_REQUEST:
            lv_msg_send(MSG_UPDATE, &value);
            break;
        default:
            break;
    }

    if(value != old_value) {
        lv_msg_send(MSG_UPDATE, &value);
    }
}

static void btn_event_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        if(lv_obj_get_index(btn) == 0) {    /*First object is the dec. button*/
            lv_msg_send(MSG_DEC, NULL);
        }
        else {
            lv_msg_send(MSG_INC, NULL);
        }
    }
}

static void label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        if(lv_msg_get_id(m) == MSG_UPDATE) {
            const int32_t * v = lv_msg_get_payload(m);
            lv_label_set_text_fmt(label, "%d %%", *v);
        }
    }
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        int32_t v = lv_slider_get_value(slider);
        lv_msg_send(MSG_SET, &v);
    }
    else if(code == LV_EVENT_MSG_RECEIVED) {
        lv_msg_t * m = lv_event_get_msg(e);
        if(lv_msg_get_id(m) == MSG_UPDATE) {
            const int32_t * v = lv_msg_get_payload(m);
            lv_slider_set_value(slider, *v, LV_ANIM_OFF);
        }
    }
}

#endif

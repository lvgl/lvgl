#include "../../lv_examples.h"
#if LV_USE_MSG && LV_USE_SLIDER && LV_USE_LABEL && LV_BUILD_EXAMPLES

/*Define a message ID*/
#define MSG_NEW_TEMPERATURE     1

static void slider_event_cb(lv_event_t * e);
static void label_event_cb(lv_event_t * e);

/**
 * A slider sends a message on value change and a label display's that value
 */
void lv_example_msg_1(void)
{
    /*Create a slider in the center of the display*/
    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_center(slider);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /*Create a label below the slider*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_event_cb(label, label_event_cb, LV_EVENT_MSG_RECEIVED, NULL);
    lv_label_set_text(label, "0%");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 30);

    /*Subscribe the label to a message. Also use the user_data to set a format string here.*/
    lv_msg_subsribe_obj(MSG_NEW_TEMPERATURE, label, "%d °C");
}

static void slider_event_cb(lv_event_t * e)
{
    /*Notify all subscribers (only the label now) that the slider value has been changed*/
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(slider);
    lv_msg_send(MSG_NEW_TEMPERATURE, &v);
}

static void label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_msg_t * m = lv_event_get_msg(e);

    const char * fmt = lv_msg_get_user_data(m);
    const int32_t * v = lv_msg_get_payload(m);

    lv_label_set_text_fmt(label, fmt, *v);
}

#endif

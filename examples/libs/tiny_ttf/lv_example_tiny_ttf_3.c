#include "../../lv_examples.h"
#if LV_USE_TINY_TTF && LV_BUILD_EXAMPLES && LV_USE_MSG

#define DISPLAY_TEXT    "Hello World!"
#define MSG_NEW_SIZE    1

static void slider_event_cb(lv_event_t * e);
static void label_event_cb(lv_event_t * e);

/**
 * Change font size with Tiny_TTF
 */
void lv_example_tiny_ttf_3(void)
{
    extern const uint8_t ubuntu_font[];
    extern const int ubuntu_font_size;

    int lsize = 25;

    /*Create style with the new font*/
    static lv_style_t style;
    lv_style_init(&style);
    lv_font_t * font = lv_tiny_ttf_create_data(ubuntu_font, ubuntu_font_size, lsize);
    lv_style_set_text_font(&style, font);
    lv_style_set_text_align(&style, LV_TEXT_ALIGN_CENTER);

    lv_obj_t * slider = lv_slider_create(lv_scr_act());
    lv_obj_center(slider);
    lv_slider_set_range(slider, 5, 50);
    lv_slider_set_value(slider, lsize, LV_ANIM_OFF);
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -50);

    lv_obj_t * slider_label = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(slider_label, "%d", lsize);
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_add_event(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);

    /*Create a label with the new style*/
    lv_obj_t * label = lv_label_create(lv_scr_act());
    lv_obj_add_style(label, &style, 0);
    lv_obj_add_event(label, label_event_cb, LV_EVENT_MSG_RECEIVED, font);
    lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(label);

    lv_msg_subscribe_obj(MSG_NEW_SIZE, label, NULL);
    lv_msg_send(MSG_NEW_SIZE, &lsize);
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    int lsize;

    lsize = (int)lv_slider_get_value(slider);

    lv_label_set_text_fmt(label, "%d", lsize);

    lv_msg_send(MSG_NEW_SIZE, &lsize);
}

static void label_event_cb(lv_event_t * e)
{
    lv_obj_t * label = lv_event_get_target(e);
    lv_font_t * font = lv_event_get_user_data(e);
    lv_msg_t * m = lv_event_get_msg(e);
    const int32_t * v = lv_msg_get_payload(m);

    lv_tiny_ttf_set_size(font, *v);
    lv_label_set_text(label, DISPLAY_TEXT);
}
#endif

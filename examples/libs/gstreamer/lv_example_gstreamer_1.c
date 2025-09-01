#include "../../lv_examples.h"
#if LV_USE_GSTREAMER && LV_BUILD_EXAMPLES

static lv_subject_t position_subject;
typedef struct {
    lv_obj_t * streamer;
    lv_obj_t * button_label;
} btn_user_data_t;


static void play_pause_pressed(lv_event_t * e)
{
    lv_obj_t * button = lv_event_get_target_obj(e);
    btn_user_data_t * data = (btn_user_data_t *)lv_event_get_user_data(e);

    lv_obj_t * button_label = data->button_label;
    lv_obj_t * streamer = data->streamer;

    if(lv_streq(lv_label_get_text(button_label), LV_SYMBOL_PLAY)) {
        lv_label_set_text(button_label, LV_SYMBOL_PAUSE);
        lv_gstreamer_play(streamer);
    }
    else {
        lv_label_set_text(button_label, LV_SYMBOL_PLAY);
        lv_gstreamer_pause(streamer);
    }

}
static void streamer_ready(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * play_pause_button = (lv_obj_t *)lv_event_get_user_data(e);
    if(code == LV_EVENT_READY) {
        lv_obj_align(play_pause_button, LV_ALIGN_BOTTOM_MID, -20, 0);
    }
}

/**
 * Loads a video from the internet using the gstreamer widget
 */
void lv_example_gstreamer_1(void)
{
    streamer = lv_gstreamer_create(lv_screen_active());
    static btn_user_data_t btn_user_data;
    lv_obj_t * streamer = lv_gstreamer_create(lv_screen_active());
    lv_obj_center(streamer);

    /* the gstreamer widget inherits the `lv_image` widget,
     * meaning you can also call specific image functions with it */
    lv_image_set_scale(streamer, 200);

    lv_gstreamer_set_src(streamer, LV_GSTREAMER_FACTORY_URI_DECODE, LV_GSTREAMER_PROPERTY_URI_DECODE,
                         "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm");

    lv_obj_t * position_slider = lv_slider_create(lv_screen_active());
    lv_obj_align(position_slider, LV_ALIGN_BOTTOM_MID, 0, 50);
    lv_subject_init_int(&position_subject, 0);
    lv_slider_bind_value(position_slider, &position_subject);
    lv_slider_set_min_value(position_slider, 0);
    lv_slider_set_max_value(position_slider, 100);

    lv_obj_t * play_pause_button = lv_button_create(lv_screen_active());
    lv_obj_align(position_slider, LV_ALIGN_BOTTOM_RIGHT, 0, 50);
    lv_obj_t * pp_btn_label = lv_label_create(play_pause_button);
    lv_label_set_text_static(pp_btn_label, LV_SYMBOL_PLAY);

    btn_user_data.button_label = pp_btn_label;
    btn_user_data.streamer = streamer;

    lv_obj_add_event_cb(play_pause_button, play_pause_pressed, LV_EVENT_CLICKED, &btn_user_data);
    lv_obj_add_event_cb(streamer, streamer_ready, LV_EVENT_READY, play_pause_button);
}

#endif

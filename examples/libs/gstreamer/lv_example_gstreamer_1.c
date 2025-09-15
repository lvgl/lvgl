#include "../../lv_examples.h"

#if LV_BUILD_EXAMPLES

#if LV_USE_GSTREAMER
typedef struct {
    lv_obj_t * streamer;
    lv_obj_t * pp_button;
    lv_obj_t * button_label;
    lv_obj_t * position_label;
    lv_obj_t * duration_label;
    lv_subject_t position_subject;
} event_data_t;

static void volume_setter_create(event_data_t * event_data);
static void control_bar_create(event_data_t * event_data);
static void update_duration_label(lv_obj_t * label, uint32_t duration);
static void volume_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
static void update_position_slider(lv_timer_t * timer);
static void play_pause_pressed(lv_event_t * e);
static void streamer_ready(lv_event_t * e);

/**
 * Loads a video from the internet using the gstreamer widget
 */
void lv_example_gstreamer_1(void)
{
    static event_data_t event_data;

    event_data.streamer = lv_gstreamer_create(lv_screen_active());

    /* the gstreamer widget inherits the `lv_image` widget,
     * meaning you can also provide it lv_image functions, like
    lv_image_set_scale(event_data.streamer, 100);
    lv_image_set_rotation(event_data.streamer, 100);
    */

    /* Set the current src of the streamer.
     * Using the `URI` "factory", we can
     * specify various URI schemes as media sources including local files (file://),
     * web streams (http://, https://), RTSP streams (rtsp://), UDP streams (udp://),
     * and many others. GStreamer's uridecodebin automatically selects the appropriate
     * source element and decoder based on the URI scheme and media format. */
    lv_gstreamer_set_src(event_data.streamer, LV_GSTREAMER_FACTORY_URI_DECODE, LV_GSTREAMER_PROPERTY_URI_DECODE,
                         "https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm");

    lv_obj_center(event_data.streamer);

    /* The LV_EVENT_READY will fire when the stream is ready at that point you can query the stream
     * information like its resolution and duration. See `streamer_ready` */
    lv_obj_add_event_cb(event_data.streamer, streamer_ready, LV_EVENT_READY, &event_data);

    /* Play the stream immediately */
    lv_gstreamer_play(event_data.streamer);

    /* Create a slider to modify the stream volume and a label to visualize the current value */
    volume_setter_create(&event_data);

    /* Create a slider to see the position in the stream with 2 text label on each side
     * One for the current position in the stream and the other for the total duration of the stream
     * Also add a pause/play button*/
    control_bar_create(&event_data);

    /* Create a timer that will update the slider position based on the stream position
     * Make it 3 times faster than the refresh rate for a smoother effect */
    lv_timer_create(update_position_slider, LV_DEF_REFR_PERIOD, &event_data);
}

static void volume_setter_create(event_data_t * event_data)
{
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_pad_all(cont, 8, 0);
    lv_obj_set_style_pad_gap(cont, 8, 0);
    lv_obj_set_style_radius(cont, 8, 0);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_70, 0);
    lv_obj_set_size(cont, 40, lv_pct(60));
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(cont, LV_ALIGN_RIGHT_MID, -8, -40);

    lv_obj_t * volume_slider = lv_slider_create(cont);
    lv_obj_set_size(volume_slider, 13, LV_PCT(50));
    lv_obj_set_flex_grow(volume_slider, 1);

    lv_obj_t * volume_label = lv_label_create(cont);
    lv_obj_set_style_text_align(volume_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(volume_label, 50);

    /* We use `lv_subject` to simplify binding the data between multiple objects.
     * Here the data is shared between the slider, the label and the gstreamer widgets */
    static lv_subject_t volume_subject;
    lv_subject_init_int(&volume_subject, 50);
    lv_subject_add_observer_obj(&volume_subject, volume_observer_cb, event_data->streamer, NULL);
    lv_slider_bind_value(volume_slider, &volume_subject);
    lv_label_bind_text(volume_label, &volume_subject, LV_SYMBOL_VOLUME_MID "\n%3" LV_PRId32 "%%");

}


static void control_bar_create(event_data_t * event_data)
{
    lv_subject_init_int(&event_data->position_subject, 0);

    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(cont);
    lv_obj_set_style_pad_all(cont, 8, 0);
    lv_obj_set_style_margin_hor(cont, 8, 0);
    lv_obj_set_style_pad_gap(cont, 8, 0);
    lv_obj_set_style_radius(cont, 8, 0);
    lv_obj_set_style_bg_color(cont, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(cont, LV_OPA_70, 0);

    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, -10);

    event_data->position_label = lv_label_create(cont);
    lv_obj_set_width(event_data->position_label, 80);
    lv_label_set_text_static(event_data->position_label, "0:00:000");

    /* Create a button a button to play/pause the stream */
    event_data->pp_button = lv_button_create(cont);
    lv_obj_center(event_data->pp_button);
    lv_obj_add_event_cb(event_data->pp_button, play_pause_pressed, LV_EVENT_CLICKED, event_data);
    event_data->button_label = lv_label_create(event_data->pp_button);
    lv_label_set_text_static(event_data->button_label, LV_SYMBOL_PAUSE);

    lv_obj_t * position_slider = lv_bar_create(cont);
    lv_bar_set_range(position_slider, 0, 1000);
    lv_obj_set_flex_grow(position_slider, 1);
    lv_slider_bind_value(position_slider, &event_data->position_subject);

    event_data->duration_label = lv_label_create(cont);
    lv_obj_set_width(event_data->duration_label, 80);
    lv_label_set_text_static(event_data->duration_label, "0:00:000");
    lv_obj_set_style_text_align(event_data->duration_label, LV_TEXT_ALIGN_RIGHT, 0);

}

static void update_duration_label(lv_obj_t * label, uint32_t duration)
{
    const uint32_t minutes = duration / 60000;
    const uint32_t seconds = (duration / 1000) % 60;
    const uint32_t milliseconds = duration % 1000;

    lv_label_set_text_fmt(label, "%" LV_PRIu32 ":%02" LV_PRIu32 ":%03" LV_PRIu32, minutes, seconds, milliseconds);
}

static void volume_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    lv_obj_t * streamer = lv_observer_get_target_obj(observer);
    int32_t volume = lv_subject_get_int(subject);
    LV_LOG_USER("Setting volume %" PRId32, volume);
    lv_gstreamer_set_volume(streamer, (uint8_t)volume);
}

static void update_position_slider(lv_timer_t * timer)
{
    event_data_t * event_data = (event_data_t *)lv_timer_get_user_data(timer);

    uint32_t duration = lv_gstreamer_get_duration(event_data->streamer);
    uint32_t position = lv_gstreamer_get_position(event_data->streamer);
    int32_t position_perc = lv_map(position, 0, duration, 0, 1000);
    lv_subject_set_int(&event_data->position_subject, position_perc);
    update_duration_label(event_data->position_label, position);
}


static void play_pause_pressed(lv_event_t * e)
{
    event_data_t * event_data = (event_data_t *)lv_event_get_user_data(e);

    if(lv_streq(lv_label_get_text(event_data->button_label), LV_SYMBOL_PLAY)) {
        lv_label_set_text(event_data->button_label, LV_SYMBOL_PAUSE);
        lv_gstreamer_play(event_data->streamer);
    }
    else {
        lv_label_set_text(event_data->button_label, LV_SYMBOL_PLAY);
        lv_gstreamer_pause(event_data->streamer);
    }
}
static void streamer_ready(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    event_data_t * event_data = (event_data_t *)lv_event_get_user_data(e);
    lv_obj_t * btn = event_data->pp_button;
    lv_obj_t * streamer = event_data->streamer;

    if(code == LV_EVENT_READY) {
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
        uint32_t duration = lv_gstreamer_get_duration(streamer);
        LV_LOG_USER("Video is starting");
        LV_LOG_USER("\tStream resolution %" LV_PRId32 "x%" LV_PRId32, lv_image_get_src_width(streamer),
                    lv_image_get_src_height(streamer));
        LV_LOG_USER("\tStream duration %" LV_PRIu32, duration);
        update_duration_label(event_data->duration_label, duration);
    }
}

#else

void lv_example_gstreamer_1(void)
{
    /*TODO
     *fallback for online examples*/

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "GStreamer web support is coming soon");
    lv_obj_center(label);
}

#endif

#endif

/**
 * @file lv_gstreamer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gstreamer_internal.h"

#if LV_USE_GSTREAMER

#include <glib.h>
#include <gst/gstelementfactory.h>
#include "../../core/lv_obj_class_private.h"

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS (&lv_gstreamer_class)


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_gstreamer_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gstreamer_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static lv_result_t gstreamer_create_pipeline(lv_gstreamer_t * streamer, GstElement * pipeline, GstElement * head);
static void on_decode_pad_added(GstElement * element, GstPad * pad, gpointer user_data);
static GstFlowReturn on_new_sample(GstElement * sink, gpointer user_data);
static void gstreamer_timer_cb(lv_timer_t * timer);
static void gstreamer_poll_bus(lv_gstreamer_t * streamer);
static void gstreamer_update_frame(lv_gstreamer_t * streamer);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_gstreamer_class = {
    .constructor_cb = lv_gstreamer_constructor,
    .destructor_cb = lv_gstreamer_destructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_gstreamer_t),
    .base_class = &lv_image_class,
    .name = "lv_gstreamer",
};

/**********************
 *      MACROS
 **********************/

#if LV_COLOR_DEPTH == 16
    #define GST_FORMAT   "RGB16"
    #define IMAGE_FORMAT LV_COLOR_FORMAT_RGB565
#elif LV_COLOR_DEPTH == 24
    #define GST_FORMAT   "BGR"
    #define IMAGE_FORMAT LV_COLOR_FORMAT_RGB888
#elif LV_COLOR_DEPTH == 32
    #define GST_FORMAT   "BGRA"
    #define IMAGE_FORMAT LV_COLOR_FORMAT_ARGB8888
#else
    #error Unsupported LV_COLOR_DEPTH
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_gstreamer_create(lv_obj_t * parent)
{
    if(!gst_is_initialized()) {
        gst_init(0, NULL);
    }

    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    LV_TRACE_OBJ_CREATE("end");
    return obj;
}

lv_result_t lv_gstreamer_set_src(lv_obj_t * obj, const char * factory_name, const char * property, const char * source)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    LV_ASSERT_NULL(factory_name);

    if(!obj || !factory_name) {
        LV_LOG_WARN("Refusing to set source with invalid params. Obj: %p Factory Name: %s", obj, factory_name);
        return LV_RESULT_INVALID;
    }

    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(streamer->pipeline) {
        LV_LOG_WARN("LVGL doesn't allow modifying the GStreamer source. Create a new widget with a new src instead");
        return LV_RESULT_INVALID;
    }
    GstElement * pipeline = gst_pipeline_new("lv_gstreamer_pipeline");
    if(!pipeline) {
        LV_LOG_ERROR("Failed to create gstreamer pipeline");
        return LV_RESULT_INVALID;
    }

    GstElement * head = gst_element_factory_make(factory_name, "lv_gstreamer_source");
    if(!head) {
        gst_object_unref(pipeline);
        LV_LOG_ERROR("Failed to create source from factory '%s'", factory_name);
        return LV_RESULT_INVALID;
    }

    if(!gst_bin_add(GST_BIN(pipeline), head)) {
        gst_object_unref(head);
        gst_object_unref(pipeline);
        LV_LOG_ERROR("Failed to add source element to pipeline");
        return LV_RESULT_INVALID;
    }

    if(property != NULL && source != NULL) {
        g_object_set(G_OBJECT(head), property, source, NULL);
    }

    /* The uri decode source element will automatically handle parsing and decoding for us
     * for other source types, we need to add a parser and a decoder ourselves element*/
    if(!lv_streq(LV_GSTREAMER_FACTORY_URI_DECODE, factory_name)) {
        GstElement * decodebin = gst_element_factory_make("decodebin", "lv_gstreamer_decodebin");
        if(!decodebin) {
            gst_object_unref(pipeline);
            LV_LOG_ERROR("Failed to create decodebin element");
            return LV_RESULT_INVALID;
        }
        if(!gst_bin_add(GST_BIN(pipeline), decodebin)) {
            gst_object_unref(decodebin);
            gst_object_unref(pipeline);
            LV_LOG_ERROR("Failed to add decodebin element to pipeline");
            return LV_RESULT_INVALID;
        }

        if(!gst_element_link(head, decodebin)) {
            gst_object_unref(pipeline);
            LV_LOG_ERROR("Failed to link source with parsebin elements");
            return LV_RESULT_INVALID;
        }
        head = decodebin;
    }

    lv_result_t res = gstreamer_create_pipeline(streamer, pipeline, head);
    if(res == LV_RESULT_INVALID) {
        LV_LOG_ERROR("Pipeline creation failed");
        gst_object_unref(pipeline);
        return res;
    }

    streamer->pipeline = pipeline;
    return LV_RESULT_OK;
}

void lv_gstreamer_play(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(!obj) {
        return;
    }
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;
    if(!streamer->pipeline) {
        LV_LOG_WARN("Cannot play: GStreamer pipeline not initialized");
        return;
    }
    GstStateChangeReturn ret = gst_element_set_state(streamer->pipeline, GST_STATE_PLAYING);
    if(ret == GST_STATE_CHANGE_FAILURE) {
        LV_LOG_ERROR("Unable to play pipeline");
    }
}

void lv_gstreamer_pause(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(!obj) {
        return;
    }
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;
    if(!streamer->pipeline) {
        LV_LOG_WARN("Cannot pause: GStreamer pipeline not initialized");
        return;
    }
    GstStateChangeReturn ret = gst_element_set_state(streamer->pipeline, GST_STATE_PAUSED);

    if(ret == GST_STATE_CHANGE_FAILURE) {
        LV_LOG_ERROR("Unable to pause pipeline");
    }
}

void lv_gstreamer_stop(lv_obj_t * obj)
{

    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(!obj) {
        return;
    }
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;
    if(!streamer->pipeline) {
        LV_LOG_WARN("Cannot stop: GStreamer pipeline not initialized");
        return;
    }
    GstStateChangeReturn ret = gst_element_set_state(streamer->pipeline, GST_STATE_READY);
    if(ret == GST_STATE_CHANGE_FAILURE) {
        LV_LOG_ERROR("Unable to stop pipeline");
    }
}
void lv_gstreamer_set_position(lv_obj_t * obj, uint32_t position)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    if(!obj) {
        return;
    }
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;
    if(!streamer->pipeline) {
        LV_LOG_WARN("Cannot set position: GStreamer pipeline not initialized");
        return;
    }
    gint64 seek_position = (gint64)position * GST_MSECOND;

    if(!gst_element_seek_simple(streamer->pipeline, GST_FORMAT_TIME,
                                GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
                                seek_position)) {
        LV_LOG_WARN("Seek operation failed");
    }
}

uint32_t lv_gstreamer_get_duration(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(!streamer->pipeline) {
        return 0;
    }

    gint64 duration;
    if(gst_element_query_duration(streamer->pipeline, GST_FORMAT_TIME, &duration)) {
        return (uint32_t)(duration / GST_MSECOND);
    }

    return 0;
}

uint32_t lv_gstreamer_get_position(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(!streamer->pipeline) {
        return 0;
    }

    gint64 position;
    if(gst_element_query_position(streamer->pipeline, GST_FORMAT_TIME, &position)) {
        return (uint32_t)(position / GST_MSECOND);
    }

    return 0;
}

lv_gstreamer_state_t lv_gstreamer_get_state(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(!streamer->pipeline) {
        return LV_GSTREAMER_STATE_NULL;
    }

    GstState state, pending;
    GstStateChangeReturn ret = gst_element_get_state(streamer->pipeline, &state, &pending, 0);

    if(ret == GST_STATE_CHANGE_FAILURE) {
        return LV_GSTREAMER_STATE_NULL;
    }

    switch(state) {
        case GST_STATE_NULL:
            return LV_GSTREAMER_STATE_NULL;
        case GST_STATE_READY:
            return LV_GSTREAMER_STATE_READY;
        case GST_STATE_PAUSED:
            return LV_GSTREAMER_STATE_PAUSED;
        case GST_STATE_PLAYING:
            return LV_GSTREAMER_STATE_PLAYING;
        default:
            return LV_GSTREAMER_STATE_NULL;
    }
}

void lv_gstreamer_set_volume(lv_obj_t * obj, uint8_t volume)
{

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(!streamer->pipeline) {
        return;
    }

    g_object_set(streamer->audio_volume, "volume", volume / 100.f, NULL);
}

uint8_t lv_gstreamer_get_volume(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(!streamer->pipeline) {
        return 0;
    }

    gdouble volume;
    g_object_get(streamer->audio_volume, "volume",  &volume, NULL);

    return (uint8_t)(volume * 100.f);
}

/**
 * Set the speed rate of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param rate      the rate factor.  Example values:
 *                      - 256:   1x
 *                      - <256:  slow down
 *                      - >256:  speed up
 *                      - 128:   0.5x
 *                      - 512:   2x
 */
void lv_gstreamer_set_rate(lv_obj_t * obj, uint32_t rate)
{

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    gdouble gst_rate = (gdouble)rate / 256.0;

    gint64 current_pos;

    if(!gst_element_query_position(streamer->pipeline, GST_FORMAT_TIME, &current_pos)) {
        LV_LOG_WARN("Failed to query current position which is required to set the stream rate");
        return;
    }

    /* Perform the seek with new rate from the current position */
    if(!gst_element_seek(streamer->pipeline, gst_rate,
                         GST_FORMAT_TIME,
                         GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                         GST_SEEK_TYPE_SET, current_pos,
                         GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
        LV_LOG_WARN("Failed to change stream rate");
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gstreamer_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;
    lv_memzero(&streamer->frame, sizeof(streamer->frame));

    streamer->gstreamer_timer = lv_timer_create(gstreamer_timer_cb, LV_DEF_REFR_PERIOD / 5, streamer);
    LV_ASSERT_NULL(streamer->gstreamer_timer);

    streamer->frame_queue = g_async_queue_new();
    LV_ASSERT_NULL(streamer->frame_queue);
    streamer->last_sample = NULL;

    LV_TRACE_OBJ_CREATE("finished");
}

static void gstreamer_poll_bus(lv_gstreamer_t * streamer)
{
    GstBus * bus = gst_element_get_bus(streamer->pipeline);
    GstMessage * msg;

    while((msg = gst_bus_pop(bus)) != NULL) {
        const GstMessageType message_type = GST_MESSAGE_TYPE(msg);
        switch(message_type) {
            case GST_MESSAGE_ERROR: {
                    GError * err;
                    gchar * debug;
                    gst_message_parse_error(msg, &err, &debug);
                    LV_LOG_ERROR("GStreamer error: %s", err->message);
                    g_error_free(err);
                    g_free(debug);
                    break;
                }
            case GST_MESSAGE_EOS:
                LV_LOG_INFO("End of stream");
                break;
            case GST_MESSAGE_STATE_CHANGED: {
                    GstState old_state, new_state;
                    gst_message_parse_state_changed(msg, &old_state, &new_state, NULL);
                    LV_LOG_TRACE("State changed: %s -> %s", gst_element_state_get_name(old_state),
                                 gst_element_state_get_name(new_state));
                    break;
                }
            default:
                LV_LOG_TRACE("Received message %d", message_type);
                break;
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);
}

static void gstreamer_update_frame(lv_gstreamer_t * streamer)
{
    GstSample * sample = g_async_queue_try_pop(streamer->frame_queue);

    if(!sample) {
        return;
    }

    const bool first_frame = !streamer->is_video_info_valid;
    if(first_frame) {
        GstCaps * caps = gst_sample_get_caps(sample);
        if(!caps || !gst_video_info_from_caps(&streamer->video_info, caps)) {
            LV_LOG_ERROR("Failed to get video info from caps");
            gst_sample_unref(sample);
            return;
        }
        streamer->is_video_info_valid = true;
    }


    GstBuffer * buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if(buffer && gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        streamer->frame = (lv_image_dsc_t) {
            .data = map.data,
            .data_size = map.size,
            .header = {
                .magic = LV_IMAGE_HEADER_MAGIC,
                .cf = IMAGE_FORMAT,
                .h = GST_VIDEO_INFO_HEIGHT(&streamer->video_info),
                .w = GST_VIDEO_INFO_WIDTH(&streamer->video_info),
                .stride = GST_VIDEO_INFO_PLANE_STRIDE(&streamer->video_info, 0),
            }
        };
        lv_image_set_src((lv_obj_t *)streamer, &streamer->frame);
    }
    /* We send the event AFTER setting the image source so that users can query the
     * resolution on this specific event callback */
    if(first_frame) {
        lv_obj_send_event((lv_obj_t *)streamer, LV_EVENT_READY, streamer);
    }

    if(streamer->last_sample) {
        gst_sample_unref(streamer->last_sample);
    }
    streamer->last_sample = sample;
}
static void gstreamer_timer_cb(lv_timer_t * timer)
{
    lv_gstreamer_t * streamer = lv_timer_get_user_data(timer);

    if(!streamer->pipeline) {
        return;
    }

    gstreamer_poll_bus(streamer);
    gstreamer_update_frame(streamer);
}

static void lv_gstreamer_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)obj;

    if(streamer->pipeline) {
        gst_element_set_state(streamer->pipeline, GST_STATE_NULL);
        gst_object_unref(streamer->pipeline);
    }
    if(streamer->last_sample) {
        gst_sample_unref(streamer->last_sample);
    }

    g_async_queue_unref(streamer->frame_queue);
}

static lv_result_t gstreamer_create_pipeline(lv_gstreamer_t * streamer, GstElement * pipeline,
                                             GstElement * decode_element)
{

    /* The caller has already added head and whatever comes before it to the pipeline.
     * So inside this function, we only need to handle adding the elements that are created here */
    GstElement * video_app_sink;
    GstElement * video_rate;
    GstElement * video_queue;
    GstElement * audio_resample;
    GstElement * audio_sink;
    struct {
        const char * factory;
        const char * name;
        GstElement ** store;
    } const elements[] = {
        {"videoconvert",  "lv_gstreamer_video_convert",  &streamer->video_convert},
        {"audioconvert",  "lv_gstreamer_audio_convert",  &streamer->audio_convert},
        {"volume",        "lv_gstreamer_audio_volume",   &streamer->audio_volume},
        {"videorate",     "lv_gstreamer_video_rate",     &video_rate},
        {"queue",         "lv_gstreamer_video_queue",    &video_queue},
        {"appsink",       "lv_gstreamer_video_sink",     &video_app_sink},
        {"audioresample", "lv_gstreamer_audio_resample", &audio_resample},
        {"autoaudiosink", "lv_gstreamer_audio_sink",     &audio_sink},
    };
    const size_t element_count = sizeof(elements) / sizeof(elements[0]);
    for(size_t i = 0; i < element_count; ++i) {
        GstElement * el = gst_element_factory_make(elements[i].factory, elements[i].name);
        if(!el) {
            /* The previous elements were added to the pipeline so we don't need to unref them explicitly
             * Unrefing the pipeline is enough and is done by caller*/
            LV_LOG_ERROR("Failed to create %s element", elements[i].name);
            return LV_RESULT_INVALID;
        }
        *(elements[i].store) = el;
        if(!gst_bin_add(GST_BIN(pipeline), el)) {
            gst_object_unref(el);
            LV_LOG_ERROR("Failed to add %s element to pipeline", elements[i].name);
            return LV_RESULT_INVALID;
        }
    }

    /* Here we set the fps we want the pipeline to produce and the color format
     * This is achieved by the video_convert and video_rate elements that will automaticall throttle and
     * convert the image to the format we desire*/
    uint32_t target_fps = 1000 / LV_DEF_REFR_PERIOD;
    char caps[128];
    lv_snprintf(caps, sizeof(caps), "video/x-raw,format=%s,framerate=%" LV_PRIu32 "/1", GST_FORMAT, target_fps);

    GstCaps * appsink_caps = gst_caps_from_string(caps);
    g_object_set(G_OBJECT(video_app_sink), "emit-signals", TRUE, "sync", TRUE, "max-buffers", 1, "drop", TRUE, "caps",
                 appsink_caps, NULL);
    gst_caps_unref(appsink_caps);

    if(!gst_element_link_many(streamer->video_convert, video_rate, video_queue, video_app_sink, NULL)) {
        LV_LOG_ERROR("Failed to link video convert to sink");
        return LV_RESULT_INVALID;
    }

    if(!gst_element_link_many(streamer->audio_convert, audio_resample, streamer->audio_volume, audio_sink, NULL)) {
        LV_LOG_ERROR("Failed to link audio convert to sink");
        return LV_RESULT_INVALID;
    }

    g_signal_connect(video_app_sink, "new-sample", G_CALLBACK(on_new_sample), streamer);

    /* At this point we don't yet know the input format
     * Once the source starts receiving the data, it will create the necessary pads,
     * i.e one pad for audio and one for video
     * We add a callback so that we automatically connect to the data once it's figured out*/
    g_signal_connect(decode_element, "pad-added", G_CALLBACK(on_decode_pad_added), streamer);
    return LV_RESULT_OK;
}

static void on_decode_pad_added(GstElement * element, GstPad * pad, gpointer user_data)
{
    LV_UNUSED(element);
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)user_data;
    GstCaps * caps = gst_pad_get_current_caps(pad);

    GstStructure * structure = gst_caps_get_structure(caps, 0);
    const gchar * name = gst_structure_get_name(structure);

    LV_LOG_TRACE("Pad discovered %s", name);

    if(g_str_has_prefix(name, "video/")) {
        GstPad * video_convert_sink_pad = gst_element_get_static_pad(streamer->video_convert, "sink");
        if(!gst_pad_is_linked(video_convert_sink_pad)) {
            if(gst_pad_link(pad, video_convert_sink_pad) != GST_PAD_LINK_OK) {
                LV_LOG_ERROR("Failed to link discovered pad '%s' to videoconvert", name);
            }
        }
        else {
            LV_LOG_WARN("Received another video pad '%s' but our video pipeline is already linked - Ignoring", name);
        }
        gst_object_unref(video_convert_sink_pad);
    }
    else if(g_str_has_prefix(name, "audio/")) {
        GstPad * audio_convert_sink_pad = gst_element_get_static_pad(streamer->audio_convert, "sink");
        if(!gst_pad_is_linked(audio_convert_sink_pad)) {
            if(gst_pad_link(pad, audio_convert_sink_pad) != GST_PAD_LINK_OK) {
                LV_LOG_ERROR("Failed to link discovered pad '%s' to audioconvert", name);
            }
        }
        else {
            LV_LOG_WARN("Received another audio pad  '%s' but our audio pipeline is already linked - Ignoring", name);
        }
        gst_object_unref(audio_convert_sink_pad);
    }

    gst_caps_unref(caps);
}

static GstFlowReturn on_new_sample(GstElement * sink, gpointer user_data)
{
    /* This function is called from a thread other than the main one so we can't call anything related to LVGL here
     * Instead, we acquire the new sample (the new frame) and push it to the queue so that we can retrieve it from an LVGL timer
     * Note that the pipeline spits out a new frame every LV_DEF_REFR_PERIOD as per the way it's set up so we shouldn't ever lose any
     * frames with this method*/
    lv_gstreamer_t * streamer = (lv_gstreamer_t *)user_data;
    GstSample * sample;

    g_signal_emit_by_name(sink, "pull-sample", &sample);
    if(!sample) {
        return GST_FLOW_OK;
    }

    g_async_queue_push(streamer->frame_queue, sample);
    return GST_FLOW_OK;
}
#endif

/**
 * @file lv_gstreamer.h
 *
 */

#ifndef LV_GSTREAMER_H
#define LV_GSTREAMER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../config/lv_conf_internal.h"

#if LV_USE_GSTREAMER
#include "../core/lv_obj.h"


/*********************
 *      DEFINES
 *********************/


/* Using the `URI` "factory", we can specify various URI schemes as media sources including
 * - local files (file://)
 * - web streams (http://, https://)
 * - RTSP streams (rtsp://)
 * - UDP streams (udp://)
 * and many others.
 * GStreamer's uridecodebin automatically selects the appropriate
 * source element and decoder based on the URI scheme and media format. */
#define LV_GSTREAMER_FACTORY_URI_DECODE      "uridecodebin"
#define LV_GSTREAMER_PROPERTY_URI_DECODE      "uri"

#define LV_GSTREAMER_FACTORY_FILE            "filesrc"
#define LV_GSTREAMER_PROPERTY_FILE            "location"

#define LV_GSTREAMER_FACTORY_HTTP            "souphttpsrc"
#define LV_GSTREAMER_PROPERTY_HTTP            "location"

#define LV_GSTREAMER_FACTORY_HTTPS           "souphttpsrc"
#define LV_GSTREAMER_PROPERTY_HTTPS           "location"

#define LV_GSTREAMER_FACTORY_V4L2_CAMERA     "v4l2src"
#define LV_GSTREAMER_PROPERTY_V4L2_CAMERA     "device"

#define LV_GSTREAMER_FACTORY_ALSA_AUDIO      "alsasrc"
#define LV_GSTREAMER_PROPERTY_ALSA_AUDIO      "device"

#define LV_GSTREAMER_FACTORY_PULSE_AUDIO     "pulsesrc"
#define LV_GSTREAMER_PROPERTY_PULSE_AUDIO     "device"

#define LV_GSTREAMER_FACTORY_TEST_AUDIO      "audiotestsrc"
#define LV_GSTREAMER_PROPERTY_TEST_AUDIO      NULL

#define LV_GSTREAMER_FACTORY_TEST_VIDEO      "videotestsrc"
#define LV_GSTREAMER_PROPERTY_TEST_VIDEO      NULL

#define LV_GSTREAMER_FACTORY_APP             "appsrc"
#define LV_GSTREAMER_PROPERTY_APP             NULL

#define LV_GSTREAMER_FACTORY_WEBRTCSRC       "webrtcsrc"
#define LV_GSTREAMER_PROPERTY_WEBRTCSRC      "signaller::uri"

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GSTREAMER_STATE_NULL,
    LV_GSTREAMER_STATE_READY,
    LV_GSTREAMER_STATE_PAUSED,
    LV_GSTREAMER_STATE_PLAYING
} lv_gstreamer_state_t;

typedef enum {
    LV_GSTREAMER_STREAM_STATE_START,
    LV_GSTREAMER_STREAM_STATE_PLAY,
    LV_GSTREAMER_STREAM_STATE_PAUSE,
    LV_GSTREAMER_STREAM_STATE_STOP,
    LV_GSTREAMER_STREAM_STATE_END
} lv_gstreamer_stream_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gstreamer object
 * @param parent    pointer to a parent widget @nullable. When NULL, the widget
 *                  is created as a screen on the default display.
 * @return          pointer to the created gstreamer
 */
lv_obj_t * lv_gstreamer_create(lv_obj_t * parent);

/**
 * Add a source to this gstreamer object
 * @param obj           pointer to a gstreamer object
 * @param factory_name  the factory name for the source of this gstreamer object.
 *                      for common factory names, check `LV_GSTREAMER_FACTORY_XXX` defines
 * @param property      the property name for the gstreamer source object
 *                      for common properties, see `LV_GSTREAMER_PROPERTY_XXX` defines
 *                      @nullable When NULL the source object is created but its source is not set.
 * @param source        the property value for the gstreamer source object
 *                      @nullable When NULL the source object is created but its source is not set.
 * @return LV_RESULT_OK if the source was correctly set else LV_RESULT_INVALID
 */
lv_result_t lv_gstreamer_set_src(lv_obj_t * obj, const char * factory_name, const char * property,
                                 const char * source);

/**
 * Play this gstreamer
 * @param obj           pointer to a gstreamer object
 */
void lv_gstreamer_play(lv_obj_t * obj);

/**
 * Pause this gstreamer
 * @param obj           pointer to a gstreamer object
 */
void lv_gstreamer_pause(lv_obj_t * obj);

/**
 * Stop this gstreamer
 * @param obj           pointer to a gstreamer object
 */
void lv_gstreamer_stop(lv_obj_t * obj);

/**
 * Seek a position in this gstreamer
 * @param obj           pointer to a gstreamer object
 * @param position      position to seek to
 */
void lv_gstreamer_set_position(lv_obj_t * obj, uint32_t position);

/**
 * Get the duration of this gstreamer
 * @param obj           pointer to a gstreamer object
 * @return              the duration (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_duration(lv_obj_t * obj);

/**
 * Get the position of this gstreamer
 * @param obj           pointer to a gstreamer object
 * @return              the position (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_position(lv_obj_t * obj);

/**
 * Get the state of this gstreamer
 * @param obj           pointer to a gstreamer object
 */
lv_gstreamer_state_t lv_gstreamer_get_state(lv_obj_t * obj);

/**
 * Set the volume of this gstreamer
 * @param obj           pointer to a gstreamer object
 * @param volume         the value to set in the range [0..100]. Higher values are clamped
 */
void lv_gstreamer_set_volume(lv_obj_t * obj, uint8_t volume);

/**
 * Get the volume of this gstreamer
 * @param obj           pointer to a gstreamer object
 * @return      the volume for this gstreamer
 */
uint8_t lv_gstreamer_get_volume(lv_obj_t * obj);

/**
 * Set the speed rate of this gstreamer
 * @param obj           pointer to a gstreamer object
 * @param rate      the rate factor.  Example values:
 *                      - 256:   1x
 *                      - <256:  slow down
 *                      - >256:  speed up
 *                      - 128:   0.5x
 *                      - 512:   2x
 */
void lv_gstreamer_set_rate(lv_obj_t * obj, uint32_t rate);

/**
 * Retrieve the stream state from a STATE_CHANGED event callback
 * @param e     pointer to the event
 * @return the stream state or -1 if `e` is invalid (i.e. NULL or does not match expected event)
 */
lv_gstreamer_stream_state_t lv_gstreamer_get_stream_state(lv_event_t * e);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GSTREAMER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GSTREAMER_H*/

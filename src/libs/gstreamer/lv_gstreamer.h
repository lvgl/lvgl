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
#include "../../lv_conf_internal.h"

#if LV_USE_GSTREAMER
#include "../../core/lv_obj.h"


/*********************
 *      DEFINES
 *********************/

LV_ATTRIBUTE_EXTERN_DATA extern const lv_obj_class_t lv_gstreamer_class;

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

/** These sources are untested. For most of them, URI_DECODE can probably be used instead */
#ifdef LV_GSTREAMER_ENABLE_UNTESTED_SOURCES
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
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GSTREAMER_STATE_NULL,
    LV_GSTREAMER_STATE_READY,
    LV_GSTREAMER_STATE_PAUSED,
    LV_GSTREAMER_STATE_PLAYING
} lv_gstreamer_state_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a gstreamer object
 * @param parent    pointer to an object, it will be the parent of the new gstreamer
 * @return          pointer to the created gstreamer
 */
lv_obj_t * lv_gstreamer_create(lv_obj_t * parent);

/**
 * Add a source to this gstreamer object
 * @param gstreamer     pointer to a gstreamer object
 * @param factory_name  the factory name for the source of this gstreamer object.
 *                      for common factory names, check `LV_GSTREAMER_FACTORY_XXX` defines
 * @param property      the property name for the gstreamer source object
 *                      for common properties, see `LV_GSTREAMER_PROPERTY_XXX` defines
 *                      Passing NULL will create the source object but not set its source
 * @param source        the property value for the gstreamer source object
 *                      Passing NULL will create the source object but not set its source
 * @return LV_RESULT_OK if the source was correctly set else LV_RESULT_INVALID
 */
lv_result_t lv_gstreamer_set_src(lv_obj_t * gstreamer, const char * factory_name, const char * property,
                                 const char * source);

/**
 * Play this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_play(lv_obj_t * gstreamer);

/**
 * Pause this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_pause(lv_obj_t * gstreamer);

/**
 * Stop this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
void lv_gstreamer_stop(lv_obj_t * gstreamer);

/**
 * Seek a position in this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param position      position to seek to
 */
void lv_gstreamer_set_position(lv_obj_t * gstreamer, uint32_t position);

/**
 * Get the duration of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return              the duration (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_duration(lv_obj_t * gstreamer);

/**
 * Get the position of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return              the position (in ms) of the gstreamer object
 */
uint32_t lv_gstreamer_get_position(lv_obj_t * gstreamer);

/**
 * Get the state of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 */
lv_gstreamer_state_t lv_gstreamer_get_state(lv_obj_t * gstreamer);

/**
 * Set the volume of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @param volume         the value to set in the range [0..100]. Higher values are clamped
 */
void lv_gstreamer_set_volume(lv_obj_t * gstreamer, uint8_t volume);

/**
 * Get the volume of this gstreamer
 * @param gstreamer     pointer to a gstreamer object
 * @return      the volume for this gstreamer
 */
uint8_t lv_gstreamer_get_volume(lv_obj_t * gstreamer);

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
void lv_gstreamer_set_rate(lv_obj_t * gstreamer, uint32_t rate);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GSTREAMER*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GSTREAMER_H*/

/**
 * @file lv_gstreamer_internal.h
 *
 */

#ifndef LV_GSTREAMER_INTERNAL_H
#define LV_GSTREAMER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GSTREAMER

#include <gst/gst.h>
#include <gst/video/video.h>

#include "../../widgets/image/lv_image_private.h"

#include "lv_gstreamer.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_gstreamer_t {
    lv_image_t image;
    lv_image_dsc_t frame;
    GstVideoInfo video_info;
    GstElement * pipeline;
    GstElement * audio_convert;
    GstElement * video_convert;
    GstElement * audio_volume;
    GstSample * last_sample;
    lv_timer_t * gstreamer_timer;
    GAsyncQueue * frame_queue;
    bool is_video_info_valid;
};

typedef struct {
    uint8_t * frame_data;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    size_t data_size;
} frame_data_t;

typedef struct _lv_gstreamer_t lv_gstreamer_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_GSTREAMER != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GSTREAMER_INTERNAL_H*/

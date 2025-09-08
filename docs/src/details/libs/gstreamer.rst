.. _gstreamer:

=========
GStreamer
=========

**GStreamer** is a pipeline-based multimedia framework that links together a wide variety of media processing systems to complete complex workflows.

The framework uses a pipeline model where media data flows from source elements through various processing elements (decoders, converters, filters) to sink elements (display, speakers, files).

The GStreamer extension in LVGL provides video and audio playback capabilities with support for various media formats, streaming protocols, and media sources. It leverages GStreamer's powerful pipeline architecture to handle media decoding, processing, and rendering.

For detailed information about GStreamer, see: https://gstreamer.freedesktop.org/

Features
********

LVGL's GStreamer implementation provides comprehensive media playback capabilities:

**Media Source Support:**

* Local files via file:// URIs
* Network streaming with HTTP/HTTPS support
* RTSP streaming for live video feeds
* UDP streaming for low-latency applications
* Multicast streaming for efficient network distribution
* Video4Linux2 (V4L2) camera devices on Linux
* Audio capture from ALSA and PulseAudio devices
* Test sources for audio and video development

**URI Scheme Support:**

Using the URI factory (``LV_GSTREAMER_FACTORY_URI_DECODE``), you can specify various URI schemes as media sources:

* **Local files**: ``file://path/to/video.mp4``
* **Web streams**: ``http://example.com/stream.webm``, ``https://secure.example.com/video.mp4``
* **RTSP streams**: ``rtsp://camera.local/stream``
* **UDP streams**: ``udp://239.255.12.42:1234``
* **Multicast**: ``multicast://239.255.12.42:1234``
* **V4L2 cameras**: ``v4l2:///dev/video0``
* **Audio devices**: ``alsa://hw:0,0``, ``pulse://default``

GStreamer's ``uridecodebin`` automatically selects the appropriate source element and decoder based on the URI scheme and media format.

**Playback Control:**

* Play, pause, and stop operations
* Precise seeking to specific positions
* Volume control with 0-100% range
* Playback rate control (slow motion and fast forward)
* Real-time position and duration queries
* State management (NULL, READY, PAUSED, PLAYING)

**Media Format Support:**

GStreamer supports a wide variety of media formats through its plugin system:

* **Video**: H.264, H.265/HEVC, VP8, VP9, AV1, MPEG-4, WebM, and many more
* **Audio**: AAC, MP3, Ogg Vorbis, FLAC, Opus, PCM, and others
* **Containers**: MP4, WebM, AVI, MKV, MOV, FLV, and more


Requirements
************

The GStreamer extension requires **GStreamer 1.0** or later with the following components:

:gstreamer-1.0:        Core GStreamer framework
:gstreamer-video-1.0:  Video handling and processing utilities  
:gstreamer-app-1.0:    Application integration utilities

Dependencies
------------

Follow the official GStreamer documentation to install its development libraries on your system: https://gstreamer.freedesktop.org/documentation/installing/index.html?gi-language=c

Setup
*****

1. **Install Dependencies**

   Install the GStreamer development libraries for your platform as shown in the Dependencies section above.

2. **Enable GStreamer Support**

   Set :c:macro:`LV_USE_GSTREAMER` to ``1`` in ``lv_conf.h``.

3. **CMake Integration**

   **Option 1: Direct linking with LVGL (Recommended)**

.. code-block:: cmake

    find_package(PkgConfig REQUIRED)
    
    # Find GStreamer packages
    pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)
    pkg_check_modules(GSTREAMER_VIDEO REQUIRED gstreamer-video-1.0)
    pkg_check_modules(GSTREAMER_APP REQUIRED gstreamer-app-1.0)
    
    # Link with LVGL
    target_include_directories(lvgl PUBLIC 
        ${GSTREAMER_INCLUDE_DIRS}
        ${GSTREAMER_VIDEO_INCLUDE_DIRS} 
        ${GSTREAMER_APP_INCLUDE_DIRS})
    target_link_libraries(lvgl PUBLIC 
        ${GSTREAMER_LIBRARIES}
        ${GSTREAMER_VIDEO_LIBRARIES}
        ${GSTREAMER_APP_LIBRARIES})

4. **Manual Compilation with pkg-config**

   You can also compile manually using pkg-config to query the necessary flags:

.. code-block:: bash

    # Get compilation flags
    gcc $(pkg-config --cflags --libs gstreamer-1.0 gstreamer-video-1.0 gstreamer-app-1.0) \
        -o your_app your_app.c lvgl.a

5. **Basic Setup Example**

.. code-block:: c

    int main(void)
    {
        /* Initialize LVGL */
        lv_init();
        
        /* Setup display driver */
        lv_display_t *display = lv_display_create(800, 480);
        /* ... configure display driver ... */
        
        /* Create and run your GStreamer application */
        lv_example_gstreamer_1();
        
        while (1) {
            lv_timer_handler();
        }
        
        return 0;
    }

Usage
*****

Basic GStreamer Player Creation
-------------------------------

Here's how to create a basic GStreamer player and load media:

.. code-block:: c

    /* Create a GStreamer object */
    lv_obj_t * streamer = lv_gstreamer_create(lv_screen_active());
    
    /* Set the media source using URI factory */
    lv_result_t result = lv_gstreamer_set_src(streamer, 
        LV_GSTREAMER_FACTORY_URI_DECODE,
        LV_GSTREAMER_PROPERTY_URI_DECODE,
        "https://example.com/video.webm");
    
    if (result != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to set GStreamer source");
        return;
    }
    
    /* Start playback */
    lv_gstreamer_play(streamer);

Media Source Configuration
--------------------------

The GStreamer widget supports various media sources through different factories:

**URI Factory (Recommended):**

.. code-block:: c

    /* Load from web URL */
    lv_gstreamer_set_src(streamer, LV_GSTREAMER_FACTORY_URI_DECODE, 
                         LV_GSTREAMER_PROPERTY_URI_DECODE,
                         "https://example.com/stream.webm");
    
    /* Load from local file */
    lv_gstreamer_set_src(streamer, LV_GSTREAMER_FACTORY_URI_DECODE,
                         LV_GSTREAMER_PROPERTY_URI_DECODE, 
                         "file:///path/to/video.mp4");
    
    /* RTSP stream */
    lv_gstreamer_set_src(streamer, LV_GSTREAMER_FACTORY_URI_DECODE,
                         LV_GSTREAMER_PROPERTY_URI_DECODE,
                         "rtsp://camera.local/stream");

**File Factory:**

.. code-block:: c

    /* Direct file access */
    lv_gstreamer_set_src(streamer, LV_GSTREAMER_FACTORY_FILE,
                         LV_GSTREAMER_PROPERTY_FILE,
                         "/path/to/video.mp4");


Playback Control
----------------

Control media playback with these functions:

.. code-block:: c

    /* Basic playback control */
    lv_gstreamer_play(streamer);
    lv_gstreamer_pause(streamer);
    lv_gstreamer_stop(streamer);
    
    /* Get current state */
    lv_gstreamer_state_t state = lv_gstreamer_get_state(streamer);
    
    /* Seek to position (in milliseconds) */
    lv_gstreamer_set_position(streamer, 30000);  /* Seek to 30 seconds */
    
    /* Get current position and duration */
    uint32_t position = lv_gstreamer_get_position(streamer);
    uint32_t duration = lv_gstreamer_get_duration(streamer);
    
    /* Set playback rate - values relative to 256 (1x speed) */
    lv_gstreamer_set_rate(streamer, 128);   /* 0.5x speed */
    lv_gstreamer_set_rate(streamer, 256);   /* 1.0x speed (normal) */
    lv_gstreamer_set_rate(streamer, 512);   /* 2.0x speed */

Volume Control
--------------

Manage audio volume with built-in controls:

.. code-block:: c

    /* Set volume (0-100%) */
    lv_gstreamer_set_volume(streamer, 75);
    
    /* Get current volume */
    uint8_t volume = lv_gstreamer_get_volume(streamer);

Event Handling
--------------

Handle GStreamer events using LVGL's event system:

.. code-block:: c

    static void gstreamer_event_cb(lv_event_t * e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        lv_obj_t * streamer = lv_event_get_target_obj(e);
        
        if(code == LV_EVENT_READY) {
                LV_LOG_USER("Stream ready - Duration: %" LV_PRIu32 " ms", 
                           lv_gstreamer_get_duration(streamer));
                LV_LOG_USER("Resolution: %" LV_PRId32 "x%" LV_PRId32,
                           lv_image_get_src_width(streamer),
                           lv_image_get_src_height(streamer));
        }
    }
    
    /* Add event callback */
    lv_obj_add_event_cb(streamer, gstreamer_event_cb, LV_EVENT_ALL, NULL);

Widget Architecture
*******************

The GStreamer widget extends the ``lv_image`` widget, which means:

- All standard ``lv_obj`` functions work with GStreamer widgets (positioning, sizing, styling, events)
- All ``lv_image`` functions are available for image-related operations
- Video frames are rendered as image content that updates automatically during playback

State Management
----------------

The GStreamer widget maintains these states:

- ``LV_GSTREAMER_STATE_NULL``: Initial state, no media loaded
- ``LV_GSTREAMER_STATE_READY``: Media loaded and ready to play
- ``LV_GSTREAMER_STATE_PAUSED``: Playback paused
- ``LV_GSTREAMER_STATE_PLAYING``: Active playback

Media Information Access
------------------------

Once media is loaded (LV_EVENT_READY), you can access:

- Video resolution via ``lv_image_get_src_width()`` and ``lv_image_get_src_height()``
- Media duration via ``lv_gstreamer_get_duration()``  
- Current playback position via ``lv_gstreamer_get_position()``
- Current volume level via ``lv_gstreamer_get_volume()``
- Current playback state via ``lv_gstreamer_get_state()``

.. _gstreamer_example:

Examples
********

.. include:: ../../examples/libs/gstreamer/index.rst

.. _gstreamer_api:

API
***

.. API startswith:  lv_gstreamer_

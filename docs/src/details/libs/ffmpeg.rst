.. _ffmpeg:

==============
FFmpeg Support
==============

**FFmpeg** is a complete, cross-platform solution to record, convert and stream audio and video.

The FFmpeg is an LVGL extension that interfaces with the official FFmpeg library to help
you add platform-independent recording, converting and streaming audio and video into
your LVGL UI.

The set-up steps below are for Linux, but they can be adapted for other platforms.

For a detailed introduction, see:  https://www.ffmpeg.org



Installing FFmpeg
*****************

Download the FFmpeg library from `its official download page
<https://www.ffmpeg.org/download.html>`__, then install it:

.. code-block:: shell

    ./configure --disable-all --disable-autodetect --disable-podpages --disable-asm --enable-avcodec --enable-avformat --enable-decoders --enable-encoders --enable-demuxers --enable-parsers --enable-protocol='file' --enable-swscale --enable-zlib
    make
    sudo make install



Adding FFmpeg to Your Project
*****************************

To use the ``FFmpeg`` library in your project, you will need to link against these
libraries:

:libavformat:   part of FFmpeg library
:libavcodec:    part of FFmpeg library
:libavutil:     part of FFmpeg library
:libswscale:    part of FFmpeg library
:libm:
:libz:
:libpthread:

If you are using GCC-based toolchain, this can be taken care of by adding the
following command-line options:

.. code-block:: shell

    -lavformat -lavcodec -lavutil -lswscale -lm -lz -lpthread



.. _ffmpeg_usage:

Usage
*****

Set the :c:macro:`LV_USE_FFMPEG` in ``lv_conf.h`` to ``1``.

Also set :c:macro:`LV_FFMPEG_PLAYER_USE_LV_FS` in ``lv_conf.h`` to ``1`` if you want
to integrate the LVGL :ref:`file_system` extension into FFmpeg.
This library can load videos and images. The LVGL file system
will always be used when an image is loaded with :cpp:func:`lv_image_set_src`
regardless of the value of :c:macro:`LV_FFMPEG_PLAYER_USE_LV_FS`.

See the examples below for how to correctly use this library.



.. _ffmpeg_example:

Events
******

- :cpp:enumerator:`LV_EVENT_READY` Sent when playback is complete and auto-restart is not enabled.

Learn more about :ref:`events`.



Examples
********

.. include:: ../../examples/libs/ffmpeg/index.rst



.. _ffmpeg_api:

API
***


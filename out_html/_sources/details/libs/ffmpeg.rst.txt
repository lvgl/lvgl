.. _ffmpeg:

==============
FFmpeg support
==============

A complete, cross-platform solution to record, convert and stream audio and video.

Detailed introduction: https://www.ffmpeg.org

Install FFmpeg
--------------

Download first FFmpeg from `here <https://www.ffmpeg.org/download.html>`__, then install it:

.. code-block:: shell

    ./configure --disable-all --disable-autodetect --disable-podpages --disable-asm --enable-avcodec --enable-avformat --enable-decoders --enable-encoders --enable-demuxers --enable-parsers --enable-protocol='file' --enable-swscale --enable-zlib
    make
    sudo make install

Add FFmpeg to your project
--------------------------

-  Add library: ``FFmpeg`` (for GCC: ``-lavformat -lavcodec -lavutil -lswscale -lm -lz -lpthread``)

.. _ffmpeg_usage:

Usage
-----

Enable :c:macro:`LV_USE_FFMPEG` in ``lv_conf.h``.

See the examples below.

:note: Enable :c:macro:`LV_FFMPEG_PLAYER_USE_LV_FS` in ``lv_conf.h`` if you want to integrate the lvgl file system into FFmpeg.


.. _ffmpeg_example:

Events
------

- :cpp:enumerator:`LV_EVENT_READY` Sent when playback is complete and auto-restart is not enabled.

Learn more about :ref:`events`.

Example
-------

.. include:: ../../examples/libs/ffmpeg/index.rst

.. _ffmpeg_api:

API
---


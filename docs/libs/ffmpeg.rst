.. _ffmpeg:

==============
FFmpeg support
==============

A complete, cross-platform solution to record, convert and stream audio and video.

Detailed introduction: https://www.ffmpeg.org

Install FFmpeg
--------------

Download first FFmpeg from `here <https://www.ffmpeg.org/download.html>`__, then install it:

.. code:: shell

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

:Note: FFmpeg extension doesn't use LVGL's file system. You can
simply pass the path to the image or video as usual on your operating
system or platform.

.. _ffmpeg_example:

Example
-------

.. include:: ../examples/libs/ffmpeg/index.rst

.. _ffmpeg_api:

API
---


```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/ffmpeg.md
```

# FFmpeg support
[FFmpeg](https://www.ffmpeg.org/) A complete, cross-platform solution to record, convert and stream audio and video.

## Install FFmpeg
- Download FFmpeg from [here](https://www.ffmpeg.org/download.html)
- `./configure --disable-all --disable-autodetect --disable-podpages --disable-asm --enable-avcodec --enable-avformat --enable-decoders --enable-encoders --enable-demuxers --enable-parsers --enable-protocol='file' --enable-swscale --enable-zlib`
- `make`
- `sudo make install`

## Add FFmpeg to your project
- Add library: `FFmpeg` (for GCC: `-lavformat -lavcodec -lavutil -lswscale -lm -lz -lpthread`)

## Usage

Enable `LV_USE_FFMPEG` in `lv_conf.h`.

See the examples below.

Note that, the FFmpeg extension doesn't use LVGL's file system. 
You can simply pass the path to the image or video as usual on your operating system or platform.

## Example
```eval_rst

.. include:: ../../examples/libs/ffmpeg/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_ffmpeg.h
  :project: lvgl

```
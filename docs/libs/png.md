```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/png.md
```

# PNG decoder

Allow the use of PNG images in LVGL. This implementation uses [lodepng](https://github.com/lvandeve/lodepng) library.

If enabled in `lv_conf.h` by `LV_USE_PNG` LVGL will register a new image decoder automatically so PNG files can be directly used as any other image  sources.

Note that, a file system driver needs to registered to open images from files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...` 

The whole PNG image is decoded so during decoding RAM equals to `image width x image height x 4` bytes are required.

As it might take significant time to decode PNG images LVGL's [images caching](https://docs.lvgl.io/master/overview/image.html#image-caching) feature can be useful. 

## Example
```eval_rst

.. include:: ../../examples/libs/png/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_png.h
  :project: lvgl


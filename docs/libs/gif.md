```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/gif.md
```

# GIF decoder
Allow using GIF images in LVGL. Based on https://github.com/lecram/gifdec

When enabled in `lv_conf.h` with `LV_USE_GIF` `lv_gif_create(parent)` can be used to create a gif widget.

`lv_gif_set_src(obj, src)` works very similarly to `lv_img_set_src`. As source, it also accepts images as variables (`lv_img_dsc_t`) or files.


## Convert GIF files to C array
To convert a GIF file to byte values array use [LVGL's online converter](https://lvgl.io/tools/imageconverter). Select "Raw" color format and "C array" Output format.
 

## Use GIF images from file
For example:
```c
lv_gif_set_src(obj, "S:path/to/example.gif");
```

Note that, a file system driver needs to be registered to open images from files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...` 


## Memory requirements
To decode and display a GIF animation the following amount of RAM is required:
- `LV_COLOR_DEPTH 8`: 3 x image width x image height 
- `LV_COLOR_DEPTH 16`: 4 x image width x image height 
- `LV_COLOR_DEPTH 32`: 5 x image width x image height 

## Example
```eval_rst
.. include:: ../../examples/libs/gif/index.rst
```

## API

```eval_rst
.. doxygenfile:: lv_gif.h
  :project: lvgl
```
```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/bmp.md
```

# BMP decoder 

This extension allows the use of BMP images in LVGL. 
This implementation uses [bmp-decoder](https://github.com/caj-johnson/bmp-decoder) library.
The pixel are read on demand (not the whole image is loaded) so using BMP images requires very little RAM. 

If enabled in `lv_conf.h` by `LV_USE_BMP` LVGL will register a new image decoder automatically so BMP files can be directly used as image sources. For example:
```
lv_img_set_src(my_img, "S:path/to/picture.bmp");
```

Note that, a file system driver needs to registered to open images from files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...` 

## Limitations
- Only BMP files are supported and BMP images as C array (`lv_img_dsc_t`) are not. It's because there is no practical differences between how the BMP files and LVGL's image format stores the image data. 
- BMP files can be loaded only from file. If you want to store them in flash it's better to convert them to C array with [LVGL's image converter](https://lvgl.io/tools/imageconverter).
- The BMP files color format needs to match with `LV_COLOR_DEPTH`. Use GIMP to save the image in the required format.
  Both RGB888 and ARGB888 works with `LV_COLOR_DEPTH 32`
- Palette is not supported.
- Because not the whole image is read in can not be zoomed or rotated.


## Example
```eval_rst

.. include:: ../../examples/libs/bmp/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_bmp.h
  :project: lvgl

```

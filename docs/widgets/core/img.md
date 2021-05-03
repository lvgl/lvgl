```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/img.md
```
# Image (lv_img)


## Overview

*Images* are the basic object to display from the flash (as arrays) or externally as files. Images can display symbols (`LV_SYMBOL_...`) too.

Using the [Image decoder interface](/overview/image.html#image-decoder) custom image formats can be supported as well.

## Parts and Styles
The images has only a main part called `LV_IMG_PART_MAIN` which uses the typical background style propeties to draw a background rectangle and the *image* properties.
The padding values are used to make the background virtually larger. (It won't change the image's real size but the size modification is applied only during drawing)

## Usage

### Image source
To provide maximum flexibility, the source of the image can be:

- a variable in the code (a C array with the pixels).
- a file stored externally (like on an SD card).
- a text with [Symbols](/overview/font).

To set the source of an image, use `lv_img_set_src(img, src)`.

To generate a **pixel array** from a PNG, JPG or BMP image, use the [Online image converter tool](https://lvgl.io/tools/imageconverter) and set the converted image with its pointer: `lv_img_set_src(img1, &converted_img_var);`
To make the variable visible in the C file, you need to declare it with `LV_IMG_DECLARE(converted_img_var)`.

To use **external files**, you also need to convert the image files using the online converter tool but now you should select the binary Output format.
You also need to use LVGL's file system module and register a driver with some functions for the basic file operation. Got to the [File system](/overview/file-system) to learn more.
To set an image sourced from a file, use `lv_img_set_src(img, "S:folder1/my_img.bin")`.


You can set a **symbol** similarly to [Labels](/widgets/label). In this case, the image will be rendered as text according to the *font* specified in the style.  It enables to use of light-weighted mono-color
"letters" instead of real images. You can set symbol like `lv_img_set_src(img1, LV_SYMBOL_OK)`.

### Label as an image
Images and labels are sometimes used to convey the same thing. For example, to describe what a button does. Therefore, images and labels are somewhat interchangeable.
To handle these images can even display texts by using `LV_SYMBOL_DUMMY` as the prefix of the text. For example, `lv_img_set_src(img, LV_SYMBOL_DUMMY "Some text")`.


### Transparency
The internal (variable) and external images support 2 transparency handling methods:

- **Chrome keying** - Pixels with `LV_COLOR_TRANSP` (*lv_conf.h*) color will be transparent.
- **Alpha byte** - An alpha byte is added to every pixel.

### Palette and Alpha index
Besides *True color* (RGB) color format, the following formats are also supported:
- **Indexed** - Image has a palette.
- **Alpha indexed** - Only alpha values are stored.

These options can be selected in the font converter. To learn more about the color formats, read the [Images](/overview/image) section.

### Recolor
The images can be re-colored in run-time to any color according to the brightness of the pixels.
It is very useful to show different states (selected, inactive, pressed, etc.) of an image without storing more versions of the same image.
This feature can be enabled in the style by setting `img.intense` between `LV_OPA_TRANSP` (no recolor, value: 0) and `LV_OPA_COVER` (full recolor, value: 255).
The default value is `LV_OPA_TRANSP` so this feature is disabled.

### Auto-size
It is possible to automatically set the size of the image object to the image source's width and height if enabled by the `lv_img_set_auto_size(image, true)` function.
If *auto-size* is enabled, then when a new file is set, the object size is automatically changed. Later, you can modify the size manually. The *auto-size* is enabled by default if the image is not a screen.

### Mosaic
If the object size is greater than the image size in any directions, then the image will be repeated like a mosaic.
It's a very useful feature to create a large image from only a very narrow source.
For example, you can have a *300 x 1* image with a special gradient and set it as a wallpaper using the mosaic feature.

### Offset
With `lv_img_set_offset_x(img, x_ofs)` and `lv_img_set_offset_y(img, y_ofs)`, you can add some offset to the displayed image.
It is useful if the object size is smaller than the image source size.
Using the offset parameter a [Texture atlas](https://en.wikipedia.org/wiki/Texture_atlas) or a "running image" effect can be created by [Animating](/overview/animation) the x or y offset.

## Transformations

Using the `lv_img_set_zoom(img, factor)` the images will be zoomed. Set `factor` to `256` or `LV_IMG_ZOOM_NONE` to disable zooming. 
A larger value enlarges the images (e.g. `512` double size), a smaller value shrinks it (e.g. `128` half size).
Fractional scale works as well. E.g. `281` for 10% enlargement.

To rotate the image use `lv_img_set_angle(img, angle)`. Angle has 0.1 degree precision, so for 45.8° set 458.

By default, the pivot point of the rotation is the center of the image. It can be changed with `lv_img_set_pivot(img, pivot_x, pivot_y)`. `0;0` is the top left corner.

The quality of the transformation can be adjusted with `lv_img_set_antialias(img, true/false)`. With enabled anti-aliasing the transformations has a higher quality but they are slower.

The transformations require the whole image to be available. Therefore indexed images (`LV_IMG_CF_INDEXED_...`), alpha only images (`LV_IMG_CF_ALPHA_...`) or images from files can not be transformed. 
In other words transformations work only on true color images stored as C array, or if a custom [Image decoder](/overview/images#image-edecoder) returns the whole image.

Note that, the real coordinates of image object won't change during transformation. That is `lv_obj_get_width/height/x/y()` will returned the original, non-zoomed coordinates. 

## Rotate
The images can be rotated with 

## Events
As by default, clicking of the image objects is disabled, only [generic](../overview/event.html#generic-events) non-input device-related events are sent.
If you want to catch all of the generic events of an image object, you should enable its clicking by using this: `lv_obj_set_click(img, true)`

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_img/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_img.h
  :project: lvgl

```


# JPG decoder

Allow the use of JPG images in LVGL. Besides that it also allows the use of a custom format, called Split JPG (SJPG), which can be decoded in more optimal way on embedded systems.

## Overview
  - Supports both normal JPG and the custom SJPG formats.
  - Decoding normal JPG consumes RAM with the size fo the whole uncompressed image (recommended only for devices with more RAM)
  - SJPG is a custom format based on "normal" JPG and specially made for LVGL.
  - SJPG is 'split-jpeg' which is a bundle of small jpeg fragments with an sjpg header.
  - SJPG size will be almost comparable to the jpg file or might be a slightly larger.
  - File read from file and c-array are implemented.
  - SJPEG frame fragment cache enables fast fetching of lines if available in cache.
  - By default the sjpg image cache will be image width * 2 * 16 bytes (can be modified)
  - Only the required partion of the JPG and SJPG images are decoded, therefore they can't be zoomed or rotated.

## Usage

If enabled in `lv_conf.h` by `LV_USE_SJPG` LVGL will register a new image decoder automatically so JPG and SJPG files can be directly used as image sources. For example:
```
lv_img_set_src(my_img, "S:path/to/picture.jpg");
```

Note that, a file system driver needs to registered to open images from files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...`



## Converter

### Converting JPG to C array
  - Use lvgl online tool https://lvgl.io/tools/imageconverter
  - Color format = RAW, output format = C Array

### Converting JPG to SJPG
python3 and the PIL library required. (PIL can be installed with `pip3 install pillow`)

To create SJPG from JPG:
- Copy the image to convert into `lvgl/scripts`
- `cd lvgl/scripts`
- `python3 jpg_to_sjpg.py image_to_convert.jpg`. It creates both a C files and an SJPG image.

The expected result is:
```sh
Conversion started...

Input:
        image_to_convert.jpg
        RES = 640 x 480

Output:
        Time taken = 1.66 sec
        bin size = 77.1 KB
        walpaper.sjpg           (bin file)
        walpaper.c              (c array)

All good!
```


## Example
```eval_rst

.. include:: ../../examples/libs/sjpg/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_sjpg.h
  :project: lvgl

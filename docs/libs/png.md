```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/png.md
```

# PNG decoder

Allow the use of PNG images in LVGL. This implementation uses [lodepng](https://github.com/lvandeve/lodepng) library.

## Get started
- Download or clone this repository
  - [Download from GitHub](https://github.com/littlevgl/lv_lib_lodepng/archive/master.zip)
  - Clone: `git clone https://github.com/littlevgl/lv_lib_png.git`
- Include the library: `#include "lv_lib_png/lv_png.h"`
- Initalize the decocer with `lv_png_init()`
- Test with the following code:
```c
    LV_IMG_DECLARE(png_decoder_test);
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &png_decoder_test);
```

## Use PNG images from file
By deafult `lodepng` uses C file IO API (e.g. `fopen`) and images can be opened like this:
```c
lv_img_set_src(img, "./lv_lib_png/png_decoder_test.png");
```

If you want to make `lodepng` to use LVGL's file system API add `#define LV_PNG_USE_LV_FILESYSTEM  1` to the end of your`lv_conf.h`.
In this case you need to [register a driver](https://docs.lvgl.io/latest/en/html/overview/file-system.html) fo LVGL. The following functions are required:
- `open_cb()`
- `read_cb()`
- `close_cb()`
- `size_cb()`

After that fiels can be opened like this:
```c
lv_img_set_src(img, "P:lv_lib_lodepng/png_decoder_test.png");
```


Note that the path of the file might be different.

## Use PNG images from flash
To store a PNG image in flash it needs to be converted to C array with [Online Image converter](https://lvgl.io/tools/imageconverter). Choose `Raw with alpha` Color format and `C array` Output format. Copy the result C array to your project and use it like this:
```c
    LV_IMG_DECLARE(my_test_img);
    lv_obj_t * img = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img, &my_test_img);
```

## Learn more
To learn more about the PNG decoder itself read [this blog post](https://blog.littlevgl.com/2018-10-05/png_converter)

To learn more about the Image decoder interface of LittlevGL read the realevant part of the [documentation](https://docs.littlevgl.com/en/html/overview/image.html#image-decoder).

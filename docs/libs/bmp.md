```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/bmp.md
```

# BMP decoder 

Allow the use of BMP images in LVGL. 
This implementation uses [bmp-decoder](https://github.com/caj-johnson/bmp-decoder) library.
The pixel are read on demand (not the whole image is loaded) so using BMP imgages requires very little RAM.

Compatible with LVGL's `feat/new-fs-api` branch. 
If you use the [lv_fs_if](https://github.com/lvgl/lv_fs_if) repository it's `new-api` branch should be used.

## Get started
- Download or clone this repository
  - [Download from GitHub](https://github.com/littlevgl/lv_lib_bmp/archive/master.zip)
  - Clone: `git clone https://github.com/lvgl/lv_lib_bmp.git`
- Include the library: `#include "lv_lib_bmp/lv_bmp.h"`
- Initalize the decocer with `lv_bmp_init();`
- Test with the following code:
```c;
    lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "S/path/to/image.bmp");
```

## Limitations
- BMP files can be loaded only from file. If you want to store them in flash it's better to convert them to C array with [LVGL's image converter](https://lvgl.io/tools/imageconverter).
- The BMP files color format needs to match with `LV_COLOR_DEPTH`. Use GIMP to save the image in the required format.
  RGB888 and ARGB888 works with `LV_COLOR_DEPTH 32`
- Palette is not supported.

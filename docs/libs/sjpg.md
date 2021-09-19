```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/sjpg.md
```

# JPG decoder

## Overview
  - lv_lib_split_jpg supports both normal jpg and the custom sjpg formats.
  - Decoding normal jpg consumes RAM with the site fo the whole uncompressed image (recommended only for devices with more RAM)
  - sjpg is a custom format based on "normal" JPG and specially made for lvgl.
  - sjpg is 'split-jpeg' which is a bundle of small jpeg fragments with an sjpg header.
  - sjpg size will be almost comparable to the jpg file or might be a slightly larger.
  - lv_sjpg can open and handle multiple files at same time.
  - File read from disk (fread) and c-array is implemented.
  - SJPEG frame fragment cache enables fast fetching of lines if availble in cache.
  - By default the sjpg image cache will be xres * 2 * 16 bytes (can be modified)
  - Currently only 16 bit image format is supported (to do)

## Getting started
- Clone this repository next to lvgl with `git clone https://github.com/lvgl/lv_lib_split_jpg.git`
- If you need to open (S)JPG files use [lv_fs_if](https://github.com/lvgl/lv_fs_if) or add your own [file system drivers](https://docs.lvgl.io/latest/en/html/overview/file-system.html).
  - With `lv_fs_if` add these line to the end of `lv_conf.h` to enable "PC file system" handling
```c
#define LV_USE_FS_IF	1
#if LV_USE_FS_IF
#  define LV_FS_IF_FATFS    '\0'
#  define LV_FS_IF_PC     'S'
#endif  
```
- For fast simulatneous multi jpg/sjpg rendering in expense of additional RAM, you can modify `LV_IMG_CACHE_DEF_SIZE` to 2 or above if testing in simulator or using it in devices like raspberry pi etc.
- To open large JPG image a lot of dymaic memory is required. Be sure to set `LV_MEM_SIZE` to large enough value. In simulator `(4 * 1024 * 1024)` shiuld be a good start.
 
## Example code
```c
#include <lvgl/lvgl.h>
#include "lv_lib_split_jpg/lv_sjpg.h"

LV_IMG_DECLARE( small_image_sjpg );
LV_IMG_DECLARE( wallpaper_jpg );

void demo_jpg_sjpg( void )
{
  lv_fs_if_init();
  lv_obj_t * img1;
  lv_obj_t * img2;

  lv_split_jpeg_init();
  img1 = lv_img_create(lv_scr_act(), NULL);
  img2 = lv_img_create(lv_scr_act(), NULL);

  //jpg from c array
  lv_img_set_src( img1,  &wallpaper_jpg);

  //sjpg from file (with lv_fs)
  //On Windows
  lv_img_set_src(img2,  "S.\\lv_lib_split_jpg\\example\\images\\small_image.sjpg"); 
  
  //On Linux
  //lv_img_set_src(img2,  "S/lv_lib_split_jpg/example/images/small_image.sjpg"); 
}
```
## Converter

# Converting JPG to C array
  - Use lvgl online tool https://lvgl.io/tools/imageconverter 
  - Color format = RAW, output format = C Array
  
# Converting JPG to SJPG  
 python3 and PIL library required

To create SJPG from JPG:
1. Drag and drop a jpeg image on top of the jpg_to_sjpg.py 
2. Run the python script on shell with jpeg filename as argument. It should generate filename.c and filename.sjpg files.
```sh
python3 jpg_to_sjpg.py wallpaper.jpg
```
Expected result:
```sh
Conversion started...

Input:
        walpaper.jpg
        RES = 640 x 480

Output:
        Time taken = 1.66 sec
        bin size = 77.1 KB
        walpaper.sjpg           (bin file)
        walpaper.c              (c array)

All good!
```


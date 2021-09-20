```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/bmp.md
```

# FreeType integration
Interface to FreeType to generate font bitmaps run time

## Install FreeType
- Download Freetype from [here](https://sourceforge.net/projects/freetype/files/)
- `make`
- `sudo make install`

## Add FreeType to your project
- Add include path: `/usr/include/freetype2` (for GCC: `-I/usr/include/freetype2 -L/usr/local/lib`)
- Add library: `freetype` (for GCC: `-L/usr/local/lib -lfreetype`)

## Usage in LVGL

To enable cache, set`LV_USE_FT_CACHE_MANAGER  1`in lv_freetype.h.

```c
  /*Init freetype library
   *Cache max 64 faces and 1 size*/
  lv_freetype_init(64, 1, 0);

  /*Create a font*/
  static lv_ft_info_t info;
  info.name = "./lv_lib_freetype/arial.ttf";
  info.weight = 16;
  info.style = FT_FONT_STYLE_NORMAL;
  lv_ft_font_init(&info);

  /*Create style with the new font*/
  static lv_style_t style;
  lv_style_init(&style);
  lv_style_set_text_font(&style, info.font);

  /*Create a label with the new style*/
  lv_obj_t * label = lv_label_create(lv_scr_act());
  lv_obj_add_style(label, &style, 0);
  lv_label_set_text(label, "Hello world");

```

## Learn more
- FreeType [tutorial](https://www.freetype.org/freetype2/docs/tutorial/step1.html) 
- LVGL's [font interface](https://docs.lvgl.io/v7/en/html/overview/font.html#add-a-new-font-engine)

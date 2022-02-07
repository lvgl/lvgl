```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/freetype.md
```

# FreeType support
Interface to [FreeType](https://www.freetype.org/) to generate font bitmaps run time.

## Install FreeType
- Download Freetype from [here](https://sourceforge.net/projects/freetype/files/)
- `make`
- `sudo make install`

## Add FreeType to your project
- Add include path: `/usr/include/freetype2` (for GCC: `-I/usr/include/freetype2 -L/usr/local/lib`)
- Add library: `freetype` (for GCC: `-L/usr/local/lib -lfreetype`)

## Usage
Enable `LV_USE_FREETYPE` in `lv_conf.h`.

To cache the glyphs from the opened fonts, set  `LV_FREETYPE_CACHE_SIZE >= 0` and then use the following macros for detailed configuration:
1. `LV_FREETYPE_CACHE_SIZE`:maximum memory(bytes) used to cache font bitmap, outline, character maps, etc. 0 means use the system default value, less than 0 means disable cache. Note: that this value does not account for managed FT_Face and FT_Size objects.
1. `LV_FREETYPE_CACHE_FT_FACES`:maximum number of opened FT_Face objects managed by this cache instance.0 means use the system default value. Only useful when LV_FREETYPE_CACHE_SIZE >= 0.
1. `LV_FREETYPE_CACHE_FT_SIZES`:maximum number of opened FT_Size objects managed by this cache instance. 0 means use the system default value. Only useful when LV_FREETYPE_CACHE_SIZE >= 0.

When you are sure that all the used font sizes will not be greater than 256, you can enable `LV_FREETYPE_SBIT_CACHE`, which is much more memory efficient for small bitmaps.

You can use `lv_ft_font_init()` to create FreeType fonts. It returns `true` to indicate success, at the same time, the `font` member of `lv_ft_info_t` will be filled with a pointer to an LVGL font, and you can use it like any LVGL font.

Font style supports bold and italic, you can use the following macros to set:
1. `FT_FONT_STYLE_NORMAL`:default style.
1. `FT_FONT_STYLE_ITALIC`:Italic style
1. `FT_FONT_STYLE_BOLD`:bold style

They can be combined.eg:`FT_FONT_STYLE_BOLD | FT_FONT_STYLE_ITALIC`.

Note that, the FreeType extension doesn't use LVGL's file system. 
You can simply pass the path to the font as usual on your operating system or platform.

## Example
```eval_rst
.. include:: ../../examples/libs/freetype/index.rst
```


## Learn more
- FreeType [tutorial](https://www.freetype.org/freetype2/docs/tutorial/step1.html) 
- LVGL's [font interface](https://docs.lvgl.io/v7/en/html/overview/font.html#add-a-new-font-engine)


## API
```eval_rst
.. doxygenfile:: lv_freetype.h
  :project: lvgl
```
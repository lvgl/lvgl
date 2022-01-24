```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/bmp.md
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

See the examples below.

Note that, the FreeType extension doesn't use LVGL's file system. 
You can simply pass the path to the font as usual on your operating system or platform.

## Learn more
- FreeType [tutorial](https://www.freetype.org/freetype2/docs/tutorial/step1.html) 
- LVGL's [font interface](https://docs.lvgl.io/v7/en/html/overview/font.html#add-a-new-font-engine)


## API

```eval_rst

.. doxygenfile:: lv_freetype.h
  :project: lvgl

```
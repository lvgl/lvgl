# Tiny TTF font engine

## Usage

Allow using TrueType fonts LVGL. Based on https://github.com/nothings/stb

When enabled in `lv_conf.h` with `LV_USE_TINY_TTF` `lv_tiny_ttf_create_data(data, data_size, line_height)` can be used to create a TTF font instance at the specified line height. You can then use that font anywhere `lv_font_t` is accepted.

By default, the TTF or OTF file must be embedded as an array, either in a header, or loaded into RAM in order to function.

However, if `LV_TINY_TTF_FILE_SUPPORT` is enabled, `lv_tiny_ttf_create_file(path, line_height)` will also be available, allowing tiny_ttf to stream from a file. The file must remain open the entire time the font is being used.

After a font is created, you can change the size by using `lv_tiny_ttf_set_size(font, line_height)`.

By default, a font will use up to 4KB of cache to speed up rendering glyphs. This maximum can be changed by using `lv_tiny_ttf_create_data_ex(data, data_size, line_height, cache_size)` or `lv_tiny_ttf_create_file_ex(path, line_height, cache_size)` (when available). The cache size is indicated in bytes.

## Example
```eval_rst

.. include:: ../../examples/libs/tiny_ttf/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_tiny_ttf.h
  :project: lvgl

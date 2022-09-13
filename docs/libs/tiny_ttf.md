# Tiny TTF font engine
Allow using TrueType fonts LVGL. Based on https://github.com/nothings/stb

When enabled in `lv_conf.h` with `LV_USE_TINY_TTF` `lv_tiny_ttf_create(path,line_height,fallback)` can be used to create a TTF font instance at the specified size. You can then use that font anywhere `lv_font_t` is accepted.

Note that, a file system driver needs to be registered to open TTF files. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...`

## Memory and performance

This library does not load the entire font file into memory. It fetches as needed. This allows the font engine to work with far less RAM than would otherwise be needed, at the cost of performance. Streaming from a header or PSRAM using the CBFS filesystem is the preferred way to display fonts, and will yield the best performance.


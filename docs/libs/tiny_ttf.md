# Tiny TTF font engine
Allow using TrueType fonts LVGL. Based on https://github.com/nothings/stb

When enabled in `lv_conf.h` with `LV_USE_TINY_TTF` `lv_tiny_ttf_create(path,line_height,fallback)` can be used to create a TTF font instance at the specified size. You can then use that font anywhere `lv_font_t` is accepted.

Note that, a file system driver needs to be registered to open a TTF from a file. Read more about it [here](https://docs.lvgl.io/master/overview/file-system.html) or just enable one in `lv_conf.h` with `LV_USE_FS_...`

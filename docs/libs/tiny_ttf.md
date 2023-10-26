====================
Tiny TTF font engine
====================

Usage
-----

Allow using TrueType fonts LVGL. Based on
https://github.com/nothings/stb

When enabled in ``lv_conf.h`` with :c:macro:`LV_USE_TINY_TTF`
:c:expr:`lv_tiny_ttf_create_data(data, data_size, font_size)` can be used to
create a TTF font instance at the specified line height. You can then
use that font anywhere :c:struct:`lv_font_t` is accepted.

By default, the TTF or OTF file must be embedded as an array, either in
a header, or loaded into RAM in order to function.

However, if :c:macro:`LV_TINY_TTF_FILE_SUPPORT` is enabled,
:c:expr:`lv_tiny_ttf_create_file(path, font_size)` will also be available,
allowing tiny_ttf to stream from a file. The file must remain open the
entire time the font is being used.

After a font is created, you can change the font size in pixels by using
:c:expr:`lv_tiny_ttf_set_size(font, font_size)`.

By default, a font will use up to 4KB of cache to speed up rendering
glyphs. This maximum can be changed by using
:c:expr:`lv_tiny_ttf_create_data_ex(data, data_size, font_size, cache_size)`
or :c:expr:`lv_tiny_ttf_create_file_ex(path, font_size, cache_size)` (when
available). The cache size is indicated in bytes.

Example
-------

.. include:: ../examples/libs/tiny_ttf/index.rst

API
---

:ref:`stb_rect_pack`

:ref:`stb_truetype_htcw`

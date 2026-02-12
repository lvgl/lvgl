.. _tiny_ttf:

====================
Tiny TTF Font Engine
====================

The lv_tiny_ttf extension allows using TrueType fonts in LVGL using the
`stb_truetype 3rd-Party Library <https://github.com/nothings/stb>`__.

For a detailed introduction, see:  https://github.com/nothings/stb.



.. _tiny_ttf_usage:

Usage
*****

When enabled in ``lv_conf.h`` by setting :c:macro:`LV_USE_TINY_TTF` to ``1``,
:cpp:expr:`lv_tiny_ttf_create_data(data, data_size, font_size)` can be used to
create a TTF font instance with the specified line height.  You can then
use that font anywhere :cpp:struct:`lv_font_t` is accepted.

By default, the TTF or OTF file must be embedded as an array, either in
a header, or loaded into RAM in order to function.

However, if :c:macro:`LV_TINY_TTF_FILE_SUPPORT` is enabled (i.e. ``1``),
:cpp:expr:`lv_tiny_ttf_create_file(path, font_size)` will also be available,
allowing tiny_ttf to stream from a file. The file must remain open the
entire time the font is being used.

After a font is created, you can change the font size in pixels by using
:cpp:expr:`lv_tiny_ttf_set_size(font, font_size)`.

By default, a font will cache data for up to 256 glyph elements to speed up rendering.
This maximum can be changed by using
:cpp:expr:`lv_tiny_ttf_create_data_ex(data, data_size, font_size, kerning, cache_size)`
or :cpp:expr:`lv_tiny_ttf_create_file_ex(path, font_size, kerning, cache_size)` (when
available). The cache size is indicated in number of entries.  The ``kerning``
argument will be one of the ``LV_FONT_KERNING_...`` values, indicating whether to
allow kerning, if supported, or disable.



.. _tiny_ttf_example:

Examples
********

.. include:: /examples/libs/tiny_ttf/index.rst



.. _tiny_ttf_api:

API
***

.. API startswith:  lv_tiny_ttf_


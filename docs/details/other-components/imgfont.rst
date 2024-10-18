.. _lv_imgfont:

==========
Image font
==========

Draw image in **label** or **span** obj with :cpp:type:`lv_imgfont`. This is often used to
display Unicode emoji icons in text.

Supported image formats: determined by enabled LVGL :ref:`image decoders <overview_image_decoder>`.

.. _lv_imgfont_usage:

Usage
-----

Enable :c:macro:`LV_USE_IMGFONT` in ``lv_conf.h``.

To create a new *imgfont* use :cpp:expr:`lv_imgfont_create(height, path_cb, user_data)`.

- ``height`` Font size.
- ``path_cb`` A function to get the image path of a character.
  Return ``NULL`` if no image should be shown, but the character itself.
- ``user_data`` Pointer to user data.

To use the *imgfont* in a label, reference it:
:cpp:expr:`lv_obj_set_style_text_font(label, imgfont, LV_PART_MAIN)`

To destroy the *imgfont* that is no longer used, use :cpp:expr:`lv_imgfont_destroy(imgfont)`.

.. _lv_imgfont_example:

Example
-------

.. include:: ../../examples/others/imgfont/index.rst

.. _lv_imgfont_api:

API
---

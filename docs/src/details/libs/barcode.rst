.. _barcode:

=======
Barcode
=======

The LVGL Barcode utility enables you to generate Code-128 bar codes.  It uses the
`code128 <https://github.com/fhunleth/code128>`__ library by
`fhunleth <https://github.com/fhunleth>`__.



.. _barcode_usage:

Usage
*****

Set :c:macro:`LV_USE_BARCODE` to ``1`` in ``lv_conf.h``.

Use :cpp:func:`lv_barcode_create` to create a barcode object, and use
:cpp:func:`lv_barcode_update` to generate a barcode.

Call :cpp:func:`lv_barcode_set_scale` to adjust scaling,
call :cpp:func:`lv_barcode_set_dark_color` and :cpp:func:`lv_barcode_set_light_color`
adjust colors.  Call :cpp:func:`lv_barcode_set_direction` to set the bar code's
orientation.  After any of a bar code's settings have changed, call
:cpp:func:`lv_barcode_update` again to regenerate it.



Notes
*****

-  It is best not to manually set the width of the barcode, because when
   the width of the Widget is lower than the width of the barcode, the
   display will be incomplete due to truncation.
-  The scale adjustment can only be an integer multiple, for example,
   :cpp:expr:`lv_barcode_set_scale(barcode, 2)` means 2x scaling.
-  The direction setting can be :cpp:enumerator:`LV_DIR_HOR` or :cpp:enumerator:`LV_DIR_VER`.



.. _barcode_example:

Example
*******

.. include:: ../../examples/libs/barcode/index.rst



.. _barcode_api:

API
***

.. API startswith:  lv_barcode_


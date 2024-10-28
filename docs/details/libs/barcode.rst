.. _barcode:

=======
Barcode
=======

Barcode generation with LVGL. Uses
`code128 <https://github.com/fhunleth/code128>`__ by
`fhunleth <https://github.com/fhunleth>`__.

.. _barcode_usage:

Usage
-----

Enable :c:macro:`LV_USE_BARCODE` in ``lv_conf.h``.

Use :cpp:func:`lv_barcode_create` to create a barcode object, and use
:cpp:func:`lv_barcode_update` to generate a barcode.

Call :cpp:func:`lv_barcode_set_scale` to adjust scaling,
call :cpp:func:`lv_barcode_set_dark_color` and :cpp:func:`lv_barcode_set_light_color`
adjust color, call :cpp:func:`lv_barcode_set_direction` will set
direction to display, and call :cpp:func:`lv_barcode_update` again to regenerate
the barcode.

Notes
-----

-  It is best not to manually set the width of the barcode, because when
   the width of the Widget is lower than the width of the barcode, the
   display will be incomplete due to truncation.
-  The scale adjustment can only be an integer multiple, for example,
   :cpp:expr:`lv_barcode_set_scale(barcode, 2)` means 2x scaling.
-  The direction adjustment can be :cpp:enumerator:`LV_DIR_HOR` or :cpp:enumerator:`LV_DIR_VER`

.. _barcode_example:

Example
-------

.. include:: ../../examples/libs/barcode/index.rst

.. _barcode_api:

API
---

:ref:`code128_h`


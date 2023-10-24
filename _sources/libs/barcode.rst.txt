=======
Barcode
=======

Barcode generation with LVGL. Uses
`code128 <https://github.com/fhunleth/code128>`__ by
`fhunleth <https://github.com/fhunleth>`__.

Usage
-----

Enable :c:macro:`LV_USE_BARCODE` in ``lv_conf.h``.

Use :c:expr:`lv_barcode_create()` to create a barcode object, and use
:c:expr:`lv_barcode_update()` to generate a barcode.

Call :c:expr:`lv_barcode_set_scale()` or :c:expr:`lv_barcode_set_dark/light_color()`
to adjust scaling and color, call :c:expr:`lv_barcode_set_direction()` will set 
direction to display, and call :c:expr:`lv_barcode_update()` again to regenerate 
the barcode.

Notes
-----

-  It is best not to manually set the width of the barcode, because when
   the width of the object is lower than the width of the barcode, the
   display will be incomplete due to truncation.
-  The scale adjustment can only be an integer multiple, for example,
   :c:expr:`lv_barcode_set_scale(barcode, 2)` means 2x scaling.
-  The direction adjustment can be `LV_DIR_HOR` or `LV_DIR_VER`

Example
-------

.. include:: ../examples/libs/barcode/index.rst

API
---

:ref:`code128`


=======
QR code
=======

QR code generation with LVGL. Uses
`QR-Code-generator <https://github.com/nayuki/QR-Code-generator>`__ by
`nayuki <https://github.com/nayuki>`__.

Usage
-----

Enable :c:macro:`LV_USE_QRCODE` in ``lv_conf.h``.

Use :c:expr:`lv_qrcode_create()` to create a qrcode object, and use
:c:expr:`lv_qrcode_update()` to generate a QR code.

If you need to re-modify the size and color, use
:c:expr:`lv_qrcode_set_size()` and :c:expr:`lv_qrcode_set_dark/light_color()`, and
call :c:expr:`lv_qrcode_update()` again to regenerate the QR code.

Notes
-----

-  QR codes with less data are smaller, but they scaled by an integer
   number to best fit to the given size.

Example
-------

.. include:: ../examples/libs/qrcode/index.rst

API
---

:ref:`qrcodegen`


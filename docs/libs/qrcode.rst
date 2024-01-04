.. _qrcode:

=======
QR code
=======

QR code generation with LVGL. Uses
`QR-Code-generator <https://github.com/nayuki/QR-Code-generator>`__ by
`nayuki <https://github.com/nayuki>`__.

.. _qrcode_usage:

Usage
-----

Enable :c:macro:`LV_USE_QRCODE` in ``lv_conf.h``.

Use :cpp:func:`lv_qrcode_create` to create a qrcode object, and use
:cpp:func:`lv_qrcode_update` to generate a QR code.

If you need to re-modify the size and color, use
:cpp:func:`lv_qrcode_set_size` and :cpp:func:`lv_qrcode_set_dark_color` or 
:cpp:func:`lv_qrcode_set_light_color`, and
call :cpp:func:`lv_qrcode_update` again to regenerate the QR code.

Notes
-----

-  QR codes with less data are smaller, but they scaled by an integer
   number to best fit to the given size.

.. _qrcode_example:

Example
-------

.. include:: ../examples/libs/qrcode/index.rst

.. _qrcode_api:

API
---

:ref:`qrcodegen`


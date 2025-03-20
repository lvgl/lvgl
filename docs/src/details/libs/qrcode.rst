.. _qrcode:

=======
QR Code
=======

The `QR-Code-generator library <https://github.com/nayuki/QR-Code-generator>`__ by
`nayuki <https://github.com/nayuki>`__ is a 3rd-party library that generates QR-Code
bitmaps.

The lv_qrcode LVGL extension is an interface to that library which implements a custom
Widget that generates and displays QR Codes using the library.



.. _qrcode_usage:

Usage
*****

Enable :c:macro:`LV_USE_QRCODE` in ``lv_conf.h`` by setting its value to ``1``.

Use :cpp:func:`lv_qrcode_create` to create the QR-Code Widget.  Then use
:cpp:func:`lv_qrcode_update` to generate the QR Code on it.

If you need to re-modify the size and color, use
:cpp:func:`lv_qrcode_set_size` and :cpp:func:`lv_qrcode_set_dark_color` or
:cpp:func:`lv_qrcode_set_light_color` respectively, and then
call :cpp:func:`lv_qrcode_update` again to update the QR Code.



Notes
*****

-  QR Codes with less data are smaller, but they are scaled by an integer
   value to best fit to the given size.



.. _qrcode_example:

Example
*******

.. include:: ../../examples/libs/qrcode/index.rst



.. _qrcode_api:

API
***

.. API startswith:  lv_qrcode_


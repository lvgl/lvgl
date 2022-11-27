
# QR code

QR code generation with LVGL. Uses [QR-Code-generator](https://github.com/nayuki/QR-Code-generator) by [nayuki](https://github.com/nayuki).

## Usage

Enable `LV_USE_QRCODE` in `lv_conf.h`.

Use `lv_qrcode_create()` to create a qrcode object, and use `lv_qrcode_update()` to generate a QR code.

If you need to re-modify the size and color, use `lv_qrcode_set_size()` and `lv_qrcode_set_dark/light_color()`, and call `lv_qrcode_update()` again to regenerate the QR code.

## Notes
- QR codes with less data are smaller, but they scaled by an integer number to best fit to the given size.


## Example
```eval_rst

.. include:: ../../examples/libs/qrcode/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_qrcode.h
  :project: lvgl

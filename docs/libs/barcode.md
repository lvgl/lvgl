
# Barcode

Barcode generation with LVGL. Uses [code128](https://github.com/fhunleth/code128) by [fhunleth](https://github.com/fhunleth).

## Usage

Enable `LV_USE_BARCODE` in `lv_conf.h`.

Use `lv_barcode_create()` to create a barcode object, and use `lv_barcode_update()` to generate a barcode.

Call `lv_barcode_set_scale()` or `lv_barcode_set_dark/light_color()` to adjust scaling and color, and call `lv_barcode_update()` again to regenerate the barcode.

## Notes
- It is best not to manually set the width of the barcode, because when the width of the object is lower than the width of the barcode, the display will be incomplete due to truncation.
- The scale adjustment can only be an integer multiple, for example, `lv_barcode_set_scale(barcode, 2)` means 2x scaling.

## Example
```eval_rst

.. include:: ../../examples/libs/barcode/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_barcode.h
  :project: lvgl

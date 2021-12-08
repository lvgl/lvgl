```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/others/monkey.md
```
# Monkey

A simple monkey test. Use random input to stress test the application.

## Usage

First set the `type`, check [input devices](/overview/indev) for the supported types, and then set the range of `period_range` and `input_range`. The monkey will output random operations at random times within this range.

Note that `input_range` has different meanings in different `type`:

- `LV_INDEV_TYPE_POINTER` No effect, click randomly within the pixels of the screen resolution.
- `LV_INDEV_TYPE_ENCODER` The minimum and maximum values ​​of `enc_diff`.
- `LV_INDEV_TYPE_BUTTON` The minimum and maximum values ​​of `btn_id`.
- `LV_INDEV_TYPE_KEYPAD` No effect, Send random [Keys](/overview/indev).

## Example

```eval_rst

.. include:: ../../examples/others/monkey/index.rst

```
## API


```eval_rst

.. doxygenfile:: lv_monkey.h
  :project: lvgl

```

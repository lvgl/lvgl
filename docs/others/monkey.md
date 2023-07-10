# Monkey

A simple monkey test. Use random input to stress test the application.

## Usage

Enable `LV_USE_MONKEY` in `lv_conf.h`.

First configure monkey, use `lv_monkey_config_t` to define the configuration structure, set the `type` (check [input devices](/overview/indev) for the supported types), and then set the range of `period_range` and `input_range`, the monkey will output random operations at random times within this range. Call `lv_monkey_create` to create monkey. Finally call `lv_monkey_set_enable(monkey, true)` to enable monkey.

If you want to pause the monkey, call `lv_monkey_set_enable(monkey, false)`. To delete the monkey, call `lv_monkey_del(monkey)`.

Note that `input_range` has different meanings in different `type`:

- `LV_INDEV_TYPE_POINTER` No effect, click randomly within the pixels of the screen resolution.
- `LV_INDEV_TYPE_ENCODER` The minimum and maximum values ​​of `enc_diff`.
- `LV_INDEV_TYPE_BUTTON` The minimum and maximum values ​​of `btn_id`. Use `lv_monkey_get_indev()` to get the input device, and use `lv_indev_set_button_points()` to map the key ID to the coordinates.
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

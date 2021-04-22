```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/led.md
```
# LED (lv_led)

## Overview

The LEDs are rectangle-like (or circle) object. It's brightness can be adjusted. With lower brightness the the colors of the LED become darker.

## Parts and Styles
The LEDs have only one main part, called `LV_LED_PART_MAIN` and it uses all the typical background style properties.


## Usage

### Brightness
You can set their brightness with `lv_led_set_bright(led, bright)`. The brightness should be between 0 (darkest) and 255 (lightest).

### Toggle
Use `lv_led_on(led)` and `lv_led_off(led)` to set the brightness to a predefined ON or OFF value. The `lv_led_toggle(led)` toggles between the ON and OFF state.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_led/index.rst

```

## API 

```eval_rst

.. doxygenfile:: lv_led.h
  :project: lvgl
        
```

```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/bar.md
```
# Bar (lv_bar)

## Overview

The bar object has a background and an indicator on it. The width of the indicator is set according to the current value of the bar. 

Vertical bars can be created if the width of the object is smaller than its height.

Not only end, but the start value of the bar can be set which changes the start position of the indicator.


## Parts and Styles
The Bar's main part is called `LV_BAR_PART_BG` and it uses the typical background style properties.

`LV_BAR_PART_INDIC` is a virtual part which also uses all the typical background properties. 
By default the indicator maximal size is the same as the background's size but setting positive padding values in `LV_BAR_PART_BG` will make the indicator smaller. (negative values will make it larger)
If the *value* style property is used on the indicator the alignment will be calculated based on the current size of the indicator. 
For example a center aligned value is always shown in the middle of the indicator regardless it's current size.

## Usage

### Value and range
A new value can be set by `lv_bar_set_value(bar, new_value, LV_ANIM_ON/OFF)`.
The value is interpreted in a range (minimum and maximum values) which can be modified with `lv_bar_set_range(bar, min, max)`.
The default range is 1..100.

The new value in `lv_bar_set_value` can be set with or without an animation depending on the last parameter (`LV_ANIM_ON/OFF`).
The time of the animation can be adjusted by `lv_bar_set_anim_time(bar, 100)`. The time is in milliseconds unit.

It's also possible to set the start value of the bar using `lv_bar_set_start_value(bar, new_value, LV_ANIM_ON/OFF)`

### Modes
The bar can be drawn symmetrical to zero (drawn from zero, left to right), if it's enabled with `lv_bar_set_type(bar, LV_BAR_TYPE_SYMMETRICAL)`.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_bar/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_bar.h
  :project: lvgl

```

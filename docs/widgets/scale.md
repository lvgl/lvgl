# Scale (lv_scale)

## Overview

Scale allows you to have a linear scale with ranges and sections with custom styling.

## Parts and Styles
- `LV_PART_MAIN` Main line and major ticks.
- `LV_PART_ITEMS` Minor ticks.
- `LV_PART_INDICATOR` Labels for major ticks (if enabled).

## Usage

### Set options
Options are passed to the Roller as a string with `lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL/INFINITE)`. The options should be separated by `\n`. For example: `"First\nSecond\nThird"`.

`LV_ROLLER_MODE_INFINITE` makes the roller circular.

You can select an option manually with `lv_roller_set_selected(roller, id, LV_ANIM_ON/OFF)`, where *id* is the index of an option.

### Get selected option
To get the *index* of the currently selected option use `lv_roller_get_selected(roller)`.

`lv_roller_get_selected_str(roller, buf, buf_size)` will copy the name of the selected option to `buf`.

### Visible rows
The number of visible rows can be adjusted with `lv_roller_set_visible_row_count(roller, num)`.

This function calculates the height with the current style. If the font, line space, border width, etc. of the roller changes this function needs to be called again.

## Events
No events supported by this widget.

## Keys
No keys supported by this widget.

## Example

```eval_rst

.. include:: ../../../examples/widgets/scale/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_scale.h
  :project: lvgl

```

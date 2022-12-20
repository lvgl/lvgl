# Meter (lv_meter)

## Overview
The Meter widget can visualize data in very flexible ways. In can show arcs, needles, ticks lines and labels.

## Parts and Styles
- `LV_PART_MAIN` The background of the Meter. Uses the typical background properties.
- `LV_PART_TICK` The tick lines a labels using the *line* and *text* style properties.
- `LV_PART_INDICATOR` The needle line or image using the *line* and *img* style properties, as well as the background properties to draw a square (or circle) on the pivot of the needles. Padding makes the square larger.
- `LV_PART_ITEMS` The arcs using the *arc* properties.

## Usage

### Scale

The Scale has minor and major ticks, and labels on the major ticks.

The minor tick lines can be configured with: `lv_meter_set_scale_ticks(meter, tick_count, line_width, tick_length, ctick_olor)`.

To show major tick lines use `lv_meter_set_scale_major_ticks(meter, nth_major, tick_width, tick_length, tick_color, label_gap)`. `nth_major` to specify how many minor ticks to skip to draw a major tick.

Labels are added automatically on major ticks with `label_gap` distance from the ticks with text proportionally to the values of the tick line.

`lv_meter_set_scale_range(meter, min, max, angle_range, rotation)` sets the value and angle range of the scale.

### Add indicators

Indicators can  be added to meter and their value is interpreted in the range of the scale.

All the indicator add functions return an `lv_meter_indicator_t *`.

#### Needle line

`indic = lv_meter_add_needle_line(meter, line_width, line_color, r_mod)` adds a needle line to a Scale. By default, the length of the line is the same as the scale's radius but `r_mod` changes the length.

`lv_meter_set_indicator_value(meter, indic, value)` sets the value of the indicator.

#### Needle image

`indic = lv_meter_add_needle_img(meter, img_src, pivot_x, pivot_y)` sets an image that will be used as a needle. `img_src` should be a needle pointing to the right like this `-O--->`.
`pivot_x` and `pivot_y` sets the pivot point of the rotation relative to the top left corner of the image.

`lv_meter_set_indicator_value(meter, inidicator, value)` sets the value of the indicator.

#### Arc
`indic = lv_meter_add_arc(meter, arc_width, arc_color, r_mod)` adds and arc indicator. . By default, the radius of the arc is the same as the scale's radius but `r_mod` changes the radius.

`lv_meter_set_indicator_start_value(meter, indic, value)` and `lv_meter_set_indicator_end_value(meter, inidicator, value)` sets the value of the indicator.

#### Scale lines (ticks)
`indic = lv_meter_add_scale_lines(meter, color_start, color_end, local, width_mod)` adds an indicator that modifies the ticks lines.
If `local` is `true` the ticks' color will be faded from `color_start` to `color_end` in the indicator's  start and end value range.
If `local` is `false` `color_start` and `color_end` will be mapped to the start and end value of the scale and only a "slice" of that color gradient will be visible in the indicator's start and end value range.
`width_mod` modifies the width of the tick lines.

`lv_meter_set_indicator_start_value(meter, inidicator, value)` and `lv_meter_set_indicator_end_value(meter, inidicator, value)` sets the value of the indicator.

## Events
- `LV_EVENT_DRAW_PART_BEGIN` and `LV_EVENT_DRAW_PART_END` is sent for the following types:
    - `LV_METER_DRAW_PART_ARC` The arc indicator
       - `part`: `LV_PART_ITEMS`
       - `sub_part_ptr`: pointer to the indicator
       - `arc_dsc`
       - `radius`: radius of the arc
       - `p1` center of the arc
    - `LV_METER_DRAW_PART_NEEDLE_LINE` The needle lines
       - `part`: `LV_PART_ITEMS`
       - `p1`, `p2` points of the line
       - `line_dsc`
       - `sub_part_ptr`: pointer to the indicator
    - `LV_METER_DRAW_PART_NEEDLE_IMG`  The needle images
       - `part`: `LV_PART_ITEMS`
       - `p1`, `p2` points of the line
       - `img_dsc`
       - `sub_part_ptr`: pointer to the indicator
    - `LV_METER_DRAW_PART_TICK` The tick lines and labels
       - `part`: `LV_PART_TICKS`
       - `value`: the value of the line
       - `text`: `value` converted to decimal or `NULL` on minor lines
       - `label_dsc`: label draw descriptor or `NULL` on minor lines
       - `line_dsc`:
       - `id`: the index of the line


See the events of the [Base object](/widgets/obj) too.

Learn more about [Events](/overview/event).

## Keys
No keys are handled by the Meter widget.

Learn more about [Keys](/overview/indev).


## Example

```eval_rst

.. include:: ../../examples/widgets/meter/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_meter.h
  :project: lvgl

```

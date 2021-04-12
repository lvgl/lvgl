```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/gauge.md
```
# Gauge (lv_gauge)

## Overview
The gauge is a meter with scale labels and one or more needles.

## Parts and Styles
The Gauge's main part is called `LV_GAUGE_PART_MAIN`. It draws a background using the typical background style properties and "minor" scale lines using the *line* and *scale* style properties.
It also uses the *text* properties to set the style of the scale labels. *pad_inner* is used to set space between the scale lines and the scale labels. 

`LV_GAUGE_PART_MAJOR` is a virtual part which describes the  major scale lines (where labels are added) using the *line* and *scale* style properties.

`LV_GAUGE_PART_NEEDLE` is also virtual part and it describes the needle(s) via the *line* style properties. 
The *size* and the typical background properties are used to describe a rectangle (or circle) in the pivot point of the needle(s).
*pad_inner* is used to to make the needle(s) smaller than the outer radius of the scale lines. 

## Usage

### Set value and needles
The gauge can show more than one needle.
Use the `lv_gauge_set_needle_count(gauge, needle_num, color_array)` function to set the number of needles and an array with colors for each needle. 
The array must be static or global variable because only its pointer is stored.

You can use `lv_gauge_set_value(gauge, needle_id, value)` to set the value of a needle.


### Scale
You can use the `lv_gauge_set_scale(gauge, angle, line_num, label_cnt)` function to adjust the scale angle and the number of the scale lines and labels.
The default settings are 220 degrees, 6 scale labels, and 21 lines.

The scale of the Gauge can have offset. It can be adjusted with `lv_gauge_set_angle_offset(gauge, angle)`.

### Range
The range of the gauge can be specified by `lv_gauge_set_range(gauge, min, max)`. The default range is 0..100.

### Needle image
An images also can be used as needles. The image should point to the right (like `==>`). To set an image use `lv_gauge_set_needle_img(gauge1, &img, pivot_x, pivot_y)`. `pivot_x` and `pivot_y` are offset of the rotation center from the top left corner. Images will be recolored to the needle's color with `image_recolor_opa` intensity coming from the styles in `LV_GAUGE_PART_NEEDLE`. 


### Critical value
To set a critical value, use `lv_gauge_set_critical_value(gauge, value)`. The scale color will be changed to *scale_end_color* after this value. The default critical value is 80.

## Events
Only the [Generic events](../overview/event.html#generic-events) are sent by the object type.

Learn more about [Events](/overview/event).

## Keys
No *Keys* are processed by the object type.

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_gauge/index.rst

```
## API

```eval_rst

.. doxygenfile:: lv_gauge.h
  :project: lvgl

```

.. _lv_scale:

================
Scale (lv_scale)
================

Overview
********

Scale allows you to have a linear scale with ranges and sections with custom styling.

.. _lv_scale_parts_and_styles:

Parts and Styles
****************

The scale widget is divided in the following three parts:

- :cpp:enumerator:`LV_PART_MAIN` Main line. See blue line in the example image.
- :cpp:enumerator:`LV_PART_ITEMS` Minor ticks. See red minor ticks in the example image.
- :cpp:enumerator:`LV_PART_INDICATOR` Major ticks and its labels (if enabled).
  See pink labels and green major ticks in the example image.

.. image:: /misc/scale.png

.. _lv_scale_usage:

Usage
*****

Set ranges
----------

The minor and major range (values of each tick) are configured with :cpp:expr:`lv_scale_set_range(scale, minor_range, major_range)`.

Tick drawing order
------------------
You can set the drawing of the ticks on top of the main line with :cpp:expr:`lv_scale_set_draw_ticks_on_top(scale, true)`. The default
drawing order is below the main line.

This is a scale with the ticks being drawn below of the main line (default):

.. image:: /misc/scale_ticks_below.png

This is an scale with the ticks being drawn at the top of the main line:

.. image:: /misc/scale_ticks_on_top.png

Configure ticks
---------------

Set the number of total ticks with :cpp:expr:`lv_scale_set_total_tick_count(scale, total_tick_count)`
and then configure the major tick being every Nth ticks with :cpp:expr:`lv_scale_set_major_tick_every(scale, nth_tick)`.

Labels on major ticks can be configured with :cpp:expr:`lv_scale_set_label_show(scale, show_label)`,
set `show_label` to true if labels should be drawn, :cpp:expr:`false` to hide them.
If instead of a numerical value in the major ticks a text is required they can be set
with :cpp:expr:`lv_scale_set_text_src(scale, custom_labels)` using ``NULL`` as the last element,
i.e. ``static char * custom_labels[3] = {"One", "Two", NULL}``.

It's possible to have the labels automatically rotated to match the ticks (only for RADIAL scales) using
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS, LV_PART_INDICATOR);`
Or rotated a fixed amount (on any scale type) - here for 20 degrees:
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, 20, LV_PART_INDICATOR);`
Or both at the same time
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS + 200, LV_PART_INDICATOR);`
Some labels of the scale might be drawn upside down (to match the tick) if the scale includes a certain angle range.
If you don't want this, to automatically rotate the labels to keep them upright, an additional flag can be used.
Labels that would be upside down are then rotated 180
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS | LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT, LV_PART_INDICATOR);`
Labels can also be moved a fixed distance in X and Y using e.g.
:cpp:expr:`lv_obj_set_style_translate_x(scale, 10, LV_PART_INDICATOR);`

.. note::

    The major tick value is calculated with the :cpp:expr:`lv_map` API (when not
    setting custom labels), this calculation takes into consideration the total
    number of ticks and the scale range, so the label drawn can present rounding
    errors when the calculated value is a float number.

The length of the ticks can be configured with the length style property on the :cpp:enumerator:`LV_PART_INDICATOR`
for major ticks and :cpp:enumerator:`LV_PART_ITEMS` for minor ticks, for example with local style:
:cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_INDICATOR);` for major ticks
and :cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);` for minor ticks. The ticks can be
padded in either direction (outwards or inwards) for RADIAL scales only with:
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_INDICATOR);` for major ticks and
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_ITEMS);` for minor .
Using length and radial offset together allows total control of the tick position.
i.e. :cpp:expr:`static char * custom_labels[3] = {"One", "Two", NULL}`.
It is also possible to offset the labels from the major ticks (either positive or negative) using 
:cpp:expr:`lv_obj_set_style_pad_radial(scale, 5, LV_PART_INDICATOR);`

Sections
--------

A section is the space between a minor and a major range. They can be created with :cpp:expr:`lv_scale_add_section(scale)`
and it handles back an :cpp:type:`lv_scale_section_t` pointer.

The range of the section is configured with :cpp:expr:`lv_scale_section_set_range(section, minor_range, major_range)`.
The style of each of the three parts of the scale section can be set with
:cpp:expr:`lv_scale_section_set_style(section, PART, style_pointer)`, where `PART` can be
:cpp:enumerator:`LV_PART_MAIN`, :cpp:enumerator:`LV_PART_ITEMS` or :cpp:enumerator:`LV_PART_INDICATOR`,
:cpp:expr:`style_pointer` should point to a global or static :cpp:type:`lv_style_t` variable.

For labels the following properties can be configured:
:cpp:func:`lv_style_set_text_font`, :cpp:func:`lv_style_set_text_color`,
:cpp:func:`lv_style_set_text_letter_space`, :cpp:func:`lv_style_set_text_opa`.

For lines (main line, major and minor ticks) the following properties can be configured:
:cpp:func:`lv_style_set_line_color`, :cpp:func:`lv_style_set_line_width`.



.. _lv_scale_events:

Events
******

No events supported by this widget.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_scale_keys:

Keys
****

No *Keys* are processed by Scale Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_scale_example:

Example
*******

.. include:: ../../examples/widgets/scale/index.rst



.. _lv_scale_api:

API
***

:ref:`lv_scale`

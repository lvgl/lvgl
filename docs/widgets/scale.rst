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

Configure ticks
---------------

Set the number of total ticks with :cpp:expr:`lv_scale_set_total_tick_count(scale, total_tick_count)` 
and then configure the major tick being every Nth ticks with :cpp:expr:`lv_scale_set_major_tick_every(scale, nth_tick)`.

Labels on major ticks can be configured with :cpp:expr:`lv_scale_set_label_show(scale, show_label)`, 
set `show_label` to true if labels should be drawn, :cpp:expr:`false` to hide them. 
If instead of a numerical value in the major ticks a text is required they can be set 
with :cpp:expr:`lv_scale_set_text_src(scale, custom_labels)` using ``NULL`` as the last element, 
i.e. :cpp:expr:`static char * custom_labels[3] = {"One", "Two", NULL};`.

The length of the ticks can be configured with the length style property on the :cpp:enumerator:`LV_PART_INDICATOR` 
for major ticks and :cpp:enumerator:`LV_PART_ITEMS` for minor ticks, for example with local style: 
:cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_INDICATOR);` for major ticks 
and :cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);` for minor ticks.

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

.. _lv_scale_keys:

Keys
****

No keys supported by this widget.

.. _lv_scale_example:

Example
*******

.. include:: ../examples/widgets/scale/index.rst

.. _lv_scale_api:

API
***

:ref:`lv_scale`

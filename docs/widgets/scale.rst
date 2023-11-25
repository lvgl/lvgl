Scale (lv_scale)
===============

Overview
********

Scale allows you to have a linear scale with ranges and sections with custom styling.

Parts and Styles
****************

The scale widget is divided in the following three parts:

- :cpp:enumerator:`LV_PART_MAIN` Main line. See blue line in the example image.
- :cpp:enumerator:`LV_PART_ITEMS` Minor ticks. See red minor ticks in the example image.
- :cpp:enumerator:`LV_PART_INDICATOR` Major ticks and its labels (if enabled). See pink labels and green major ticks in the example image.

.. image:: /misc/scale.png

Usage
*****

Set ranges
----------

The minor and major range (values of each tick) are configured with :cpp:expr:`lv_scale_set_range(scale, minor_range, major_range)`.

Configure ticks
---------------

Set the number of total ticks with :cpp:expr:`lv_scale_set_total_tick_count(scale, total_tick_count)` and then configure the major tick being every Nth ticks with :cpp:expr:`lv_scale_set_major_tick_every(scale, nth_tick)`.

Labels on major ticks can be configured with :cpp:expr:`lv_scale_set_label_show(scale, show_label)`, set `show_label` to true if labels should be drawn, :cpp:expr:`false` to hide them. If instead of a numerical value in the major ticks a text is required they can be set with :cpp:expr:`lv_scale_set_text_src(scale, custom_labels)` using NULL as the last element, i.e. :cpp:expr:`static char * custom_labels[3] = {"One", "Two", NULL};`

Sections
--------

A section is the space between a minor and a major range. They can be created with :cpp:expr:`lv_scale_add_section(scale)` and it handles back an :cpp:expr:`lv_scale_section_t` pointer.

The range of the section is configured with :cpp:expr:`lv_scale_section_set_range(section, minor_range, major_range)`. The style of each of the three parts of the scale section can be set with :cpp:expr:`lv_scale_section_set_style(section, PART, style_pointer)`, where `PART` can be :cpp:enumerator: `LV_PART_MAIN`, :cpp:enumerator: `LV_PART_ITEMS` or :cpp:enumerator: `LV_PART_INDICATOR`, :cpp:expr:`style_pointer` should point to a global or static :cpp:expr:`lv_style_t` variable.

For labels the following properties can be configured:
:cpp:expr:`lv_style_set_text_font`, :cpp:expr:`lv_style_set_text_color`, :cpp:expr:`lv_style_set_text_letter_space`, :cpp:expr:`lv_style_set_text_opa`.

For lines (main line, major and minor ticks) the following properties can be configured:
:cpp:expr:`lv_style_set_line_color`, :cpp:expr:`lv_style_set_line_width`.

Events
******

No events supported by this widget.

Keys
****

No keys supported by this widget.


Example
*******

.. include:: ../examples/widgets/scale/index.rst

API
***

:ref:`lv_scale`

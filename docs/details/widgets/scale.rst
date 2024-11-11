.. _lv_scale:

================
Scale (lv_scale)
================



Overview
********

Scale Widgets show linear or circular scales with ranges and sections with custom styling.

.. _lv_scale_parts_and_styles:



Parts and Styles
****************

The Scale Widget has the following three parts:

- :cpp:enumerator:`LV_PART_MAIN` Main line --- the blue line in example image.
- :cpp:enumerator:`LV_PART_ITEMS` Minor ticks --- the red minor ticks in example image.
- :cpp:enumerator:`LV_PART_INDICATOR` Major ticks and their labels (if enabled) ---
  the green major ticks and pink labels in example image.

.. image:: /misc/scale.png

.. _lv_scale_usage:



Usage
*****

Mode
----

Once your Scale Widget is created, set its shape, orientation and what side of the
line the major ticks are drawn on by setting its mode with
:cpp:expr:`lv_scale_set_mode(scale, mode)`, where ``mode`` can be one of these values:

- :cpp:enumerator:`LV_SCALE_MODE_HORIZONTAL_TOP`
- :cpp:enumerator:`LV_SCALE_MODE_HORIZONTAL_BOTTOM`
- :cpp:enumerator:`LV_SCALE_MODE_VERTICAL_LEFT`
- :cpp:enumerator:`LV_SCALE_MODE_VERTICAL_RIGHT`
- :cpp:enumerator:`LV_SCALE_MODE_ROUND_INNER`
- :cpp:enumerator:`LV_SCALE_MODE_ROUND_OUTER`

Set range
---------

A Scale's starts its life with a default range of [0..100].  You can change this
range with :cpp:expr:`lv_scale_set_range(scale, min, max)`.

Tick drawing order
------------------

Normally ticks and their labels are drawn first and the main line is drawn next,
giving the ticks and their labels the appearance of being underneath the main line
when there is overlap.  You can reverse this sequence if you wish, making the ticks
and labels appear on top the main line, using
:cpp:expr:`lv_scale_set_draw_ticks_on_top(scale, true)`.

Example with with ticks and labels drawn *under* the main line (default):

.. image:: /misc/scale_ticks_below.png

Example with ticks and labels drawn *on top of* the main line:

.. image:: /misc/scale_ticks_on_top.png

Configure ticks
---------------

You configure the major and minor ticks of a Scale by calling 2 functions:

- :cpp:expr:`lv_scale_set_total_tick_count(scale, total_tick_count)`, and
- :cpp:expr:`lv_scale_set_major_tick_every(scale, nth_tick)`.

If you want labels to be drawn with the major ticks, call
:cpp:expr:`lv_scale_set_label_show(scale, true)`.  (Pass ``false`` to hide them again.)

By default, the labels shown are the numeric values of the scale at the major tick
points.  Can you specify different label content by calling
:cpp:expr:`lv_scale_set_text_src(scale, custom_labels)` where ``custom_labels`` is an
array of string pointers.  Example:

.. code-block:: c

    static char * custom_labels[3] = {"One", "Two", NULL};

Note that a ``NULL`` pointer is used to terminate the list.

The content of the buffers pointed to need to remain valid for the life of the Scale.

For a Scale in one of the ``..._ROUND_...`` modes, you can optionally get it to
rotate the major-tick labels to match the rotation of the major ticks using
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS, LV_PART_INDICATOR)`.

Alternately, labels can be rotated by a fixed amount (for any Scale mode).  This
example rotates labels by 20 degrees:
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, 20, LV_PART_INDICATOR)`.

Or both of the above can be done at the same time:
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS + 200, LV_PART_INDICATOR)`.

Some labels of the Scale might be drawn upside down (to match the tick) if the Scale includes a certain angle range.
If you don't want this, to automatically rotate the labels to keep them upright, an additional flag can be used.
Labels that would be upside down are then rotated 180
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, LV_SCALE_LABEL_ROTATE_MATCH_TICKS | LV_SCALE_LABEL_ROTATE_KEEP_UPRIGHT, LV_PART_INDICATOR)`.
Labels can also be moved a fixed distance in X and Y pixels using
:cpp:expr:`lv_obj_set_style_translate_x(scale, 10, LV_PART_INDICATOR)`.

.. note::

    The major tick value is calculated with the :cpp:expr:`lv_map` API (when not
    setting custom labels), this calculation takes into consideration the total
    number of ticks and the Scale range, so the label drawn can present rounding
    errors when the calculated value is a floating-point value.

The length of the ticks can be configured with the length style property on the
:cpp:enumerator:`LV_PART_INDICATOR` for major ticks and :cpp:enumerator:`LV_PART_ITEMS`
for minor ticks.  Example with local style:
:cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_INDICATOR)` for major ticks
and :cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_ITEMS)` for minor ticks. The ticks can be
padded in either direction (outwards or inwards) for ``..._ROUND_...`` Scales only with:
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_INDICATOR)` for major ticks and
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_ITEMS)` for minor.
Using length and radial offset together allows total control of the tick position.

It is also possible to offset the labels from the major ticks (either positive or negative) using
:cpp:expr:`lv_obj_set_style_pad_radial(scale, 5, LV_PART_INDICATOR)`

Sections
--------

A Section is a portion of the Scale between the minimum and maximum values of its range.
A Section can be created with :cpp:expr:`lv_scale_add_section(scale)`, which returns
a pointer to a :cpp:type:`lv_scale_section_t` object.

The range of the Section is configured with
:cpp:expr:`lv_scale_section_set_range(section, min, max)` where ``min`` and ``max``
are the Section's boundary values that should be within the Scale's value range.
The style of each of the three parts of the Scale Section can be set with
:cpp:expr:`lv_scale_section_set_style(section, PART, style_pointer)`, where ``PART`` can be
:cpp:enumerator:`LV_PART_MAIN`, :cpp:enumerator:`LV_PART_ITEMS` or :cpp:enumerator:`LV_PART_INDICATOR`,
and :cpp:expr:`style_pointer` should point to a global or static :cpp:type:`lv_style_t` variable.

For labels the following properties can be configured:
:cpp:func:`lv_style_set_text_font`, :cpp:func:`lv_style_set_text_color`,
:cpp:func:`lv_style_set_text_letter_space`, :cpp:func:`lv_style_set_text_opa`.

For lines (main line, major and minor ticks) the following properties can be configured:
:cpp:func:`lv_style_set_line_color`, :cpp:func:`lv_style_set_line_width`.



.. _lv_scale_events:

Events
******

No special events are sent by Scale Widgets.

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

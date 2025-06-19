.. _lv_scale:

================
Scale (lv_scale)
================


Overview
********

Scale Widgets show linear or circular scales with configurable ranges, tick counts,
placement, labeling, and subsections (:ref:`scale_sections`) with custom styling.



.. _lv_scale_parts_and_styles:

Parts and Styles
****************

The Scale Widget has the following three parts:

- :cpp:enumerator:`LV_PART_MAIN` Main line --- the blue line in example image.
- :cpp:enumerator:`LV_PART_ITEMS` Minor ticks --- the red minor ticks in example image.
- :cpp:enumerator:`LV_PART_INDICATOR` Major ticks and their labels (if enabled) ---
  the green major ticks and pink labels in example image.

.. image:: /_static/images/scale.png

.. _lv_scale_usage:



Usage
*****

Mode
----

When a Scale Widget is created, it starts out in MODE
:cpp:enumerator:`LV_SCALE_MODE_HORIZONTAL_BOTTOM`.  This makes the scale horizontal
with tick marks below the line.  If you need it to have a different shape, orientation
or tick position, use :cpp:expr:`lv_scale_set_mode(scale, mode)`, where ``mode`` can
be any of these values:

- :cpp:enumerator:`LV_SCALE_MODE_HORIZONTAL_TOP`
- :cpp:enumerator:`LV_SCALE_MODE_HORIZONTAL_BOTTOM`
- :cpp:enumerator:`LV_SCALE_MODE_VERTICAL_LEFT`
- :cpp:enumerator:`LV_SCALE_MODE_VERTICAL_RIGHT`
- :cpp:enumerator:`LV_SCALE_MODE_ROUND_INNER`
- :cpp:enumerator:`LV_SCALE_MODE_ROUND_OUTER`


Setting range
-------------

A Scale starts its life with a default numeric range of [0..100] and a default
angular range of 270.  You can change these ranges with:

- :cpp:expr:`lv_scale_set_range(scale, min, max)`, and
- :cpp:expr:`lv_scale_set_angle_range(scale, angle_range)`

where ``min`` and ``max`` will become the numeric low and high values for the Scale,
and ``angle_range`` is the angle between the low and high ends of the Scale.


Tick drawing order
------------------

Normally ticks and their labels are drawn first and the main line is drawn next,
giving the ticks and their labels the appearance of being underneath the main line
when there is overlap.  You can reverse this sequence if you wish, making the ticks
and labels appear on top the main line, using
:cpp:expr:`lv_scale_set_draw_ticks_on_top(scale, true)`.  (This effect can be
reversed by passing ``false`` instead.)

Example with with ticks and labels drawn *under* the main line (default):

.. image:: /_static/images/scale_ticks_below.png

Example with ticks and labels drawn *on top of* the main line:

.. image:: /_static/images/scale_ticks_on_top.png


Configuring ticks
-----------------

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
:cpp:expr:`lv_obj_set_style_transform_rotation(scale, 200, LV_PART_INDICATOR)`.

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

The length of the ticks can be configured with the length Style property on the
:cpp:enumerator:`LV_PART_INDICATOR` for major ticks and :cpp:enumerator:`LV_PART_ITEMS`
for minor ticks.  Example with local Style:
:cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_INDICATOR)` for major ticks
and :cpp:expr:`lv_obj_set_style_length(scale, 5, LV_PART_ITEMS)` for minor ticks. The ticks can be
padded in either direction (outwards or inwards) for ``..._ROUND_...`` Scales only with:
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_INDICATOR)` for major ticks and
:cpp:expr:`lv_obj_set_style_radial_offset(scale, 5, LV_PART_ITEMS)` for minor.
Using length and radial offset together allows total control of the tick position.

It is also possible to offset the labels from the major ticks (either positive or negative) using
:cpp:expr:`lv_obj_set_style_pad_radial(scale, 5, LV_PART_INDICATOR)`


.. _scale_sections:

Sections
--------

Sections make it possible for portions of a Scale to *convey meaning* by using
different Style properties to draw them (colors, line thicknesses, font, etc.).

A Section represents a sub-range of the Scale, whose Styles (like Cascading Style
Sheets) take precedence while drawing the PARTS (lines, arcs, ticks and labels) of
the Scale that are within the range of that Section.

If a PART of a Scale is within the range of 2 or more Sections (i.e. those Sections
overlap), the Style's properties belonging to the most recently added Section takes
precedence over the same style properties of other Section(s) that "involve" that
PART.


.. _scale_creating_sections:

Creating Sections
~~~~~~~~~~~~~~~~~

A Section is created using :cpp:expr:`lv_scale_add_section(scale)`, which returns a
pointer to a :cpp:type:`lv_scale_section_t` object.  This creates a Section with
range [0..0] and no Styles added to it, which ensures that Section will not be drawn
yet:  it needs both a range inside the Scale's range and at least one :ref:`Style
<styles>` added to it before it will be used in drawing the Scale.

Next, set the range using :cpp:expr:`lv_scale_section_set_range(section, min, max)`
where ``min`` and ``max`` are the Section's boundary values that should normally be
within the Scale's value range.  (If they are only partially within the Scale's
range, the Scale will only use that portion of the Section that overlaps the Scale's
range.  If a Section's range is not within the Scale's range at all, it will not be
used in drawing.  That can be useful to temporarily "disable" a Section, e.g.
:cpp:expr:`lv_scale_section_set_range(section, 0, -1)`.)


.. _scale_styling_sections:

Styling Sections
~~~~~~~~~~~~~~~~

You set a Section's Style properties by creating a :cpp:type:`lv_style_t` object
for each "section" you want to appear different than the parent Scale.  Add style
properties as is documented in :ref:`style_initialize`.

You attach each :cpp:type:`lv_style_t` object to each Section it will apply to using
:cpp:expr:`lv_scale_section_set_style(section, PART, style_pointer)`, where:

- ``style_pointer`` should point to the contents of a global or static variable (can
  be dynamically-allocated), since it needs to remain valid through the life of the
  Scale; and

- ``PART`` indicates which single :ref:`PART <lv_scale_parts_and_styles>` of the
  parent Scale it will apply to, namely :cpp:enumerator:`LV_PART_MAIN`,
  :cpp:enumerator:`LV_PART_ITEMS` or :cpp:enumerator:`LV_PART_INDICATOR`.

Unlike adding normal styles to Widgets, you cannot combine PARTs by bit-wise OR-ing
the PART values together to get the style to apply to more than one part.  However,
you can do something like this to accomplish the same thing:

.. code-block:: c

    static lv_style_t  tick_style;
    lv_style_init(&tick_style);
    lv_style_set_line_color(&tick_style, lv_palette_darken(LV_PALETTE_RED, 3));
    lv_scale_section_set_style(section, LV_PART_ITEMS, &tick_style);
    lv_scale_section_set_style(section, LV_PART_INDICATOR, &tick_style);

to get that one Style object to apply to both major and minor ticks.

:cpp:type:`lv_style_t` objects can be shared among Sections and among PARTs, but
unlike normal Styles added to a Widget, a Section can only have 1 style per PART.
Thus, doing this:

.. code-block:: c

    lv_scale_section_set_style(section, LV_PART_INDICATOR, &tick_style_1);
    lv_scale_section_set_style(section, LV_PART_INDICATOR, &tick_style_2);

replaces ``tick_style_1`` with ``tick_style_2`` for part
:cpp:enumerator:`LV_PART_INDICATOR` rather than adding to it.


Useful Style Properties for Sections
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Style properties that are used during Scale drawing (and are thus useful) are:

- For main line *when it is a straight line* (:cpp:enumerator:`LV_PART_MAIN`):

  :LV_STYLE_LINE_WIDTH:         :cpp:func:`lv_style_set_line_width`
  :LV_STYLE_LINE_COLOR:         :cpp:func:`lv_style_set_line_color`
  :LV_STYLE_LINE_OPA:           :cpp:func:`lv_style_set_line_opa`

- For main line *when it is an arc* (:cpp:enumerator:`LV_PART_MAIN`):

  :LV_STYLE_ARC_WIDTH:          :cpp:func:`lv_style_set_arc_width`
  :LV_STYLE_ARC_COLOR:          :cpp:func:`lv_style_set_arc_color`
  :LV_STYLE_ARC_OPA:            :cpp:func:`lv_style_set_arc_opa`
  :LV_STYLE_ARC_ROUNDED:        :cpp:func:`lv_style_set_arc_rounded`
  :LV_STYLE_ARC_IMAGE_SRC:      :cpp:func:`lv_style_set_arc_image_src`

- For tick lines (:cpp:enumerator:`LV_PART_ITEMS` and :cpp:enumerator:`LV_PART_INDICATOR`):

  :LV_STYLE_LINE_WIDTH:         :cpp:func:`lv_style_set_line_width`
  :LV_STYLE_LINE_COLOR:         :cpp:func:`lv_style_set_line_color`
  :LV_STYLE_LINE_OPA:           :cpp:func:`lv_style_set_line_opa`

- For labels on major ticks (:cpp:enumerator:`LV_PART_INDICATOR`)

  :LV_STYLE_TEXT_COLOR:         :cpp:func:`lv_style_set_text_color`
  :LV_STYLE_TEXT_OPA:           :cpp:func:`lv_style_set_text_opa`
  :LV_STYLE_TEXT_LETTER_SPACE:  :cpp:func:`lv_style_set_text_letter_space`
  :LV_STYLE_TEXT_FONT:          :cpp:func:`lv_style_set_text_font`



.. _lv_scale_events:

Events
******

No special events are sent by Scale Widgets.

In :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED` events, a major or minor line
draw descriptor's members ``id1`` and ``id2`` will be the tick index and
tick value, respectively. If the part is :cpp:enumerator:`LV_PART_INDICATOR`,
it is a major tick. If the part is :cpp:enumerator:`LV_PART_ITEMS` it is a
minor tick.

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

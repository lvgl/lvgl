.. _lv_arclabel:

========================
Arc Label (lv_arclabel)
========================


Overview
********

The Arc Label is a specialized widget designed to display text along an arc. It allows for flexible text placement and styling, making it suitable for applications where text needs to follow a curved path, such as in gauges, dials, or custom interfaces. The widget supports various configurations, including text alignment, direction, radius adjustment, and color customization.

.. _lv_arclabel_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Represents the main part of the Arc Label, including the arc path and the text rendered along it. The appearance of the text and the arc can be customized using typical text and background style properties.

.. _lv_arclabel_usage:

Usage
*****

Text Management
---------------

- Text can be set using :cpp:expr:`lv_arclabel_set_text(arclabel, "Your text")`.
- For formatted text, use :cpp:expr:`lv_arclabel_set_text_fmt(arclabel, "Formatted %s", "text")`.
- Static text can be set with :cpp:expr:`lv_arclabel_set_text_static(arclabel, static_text)`, which avoids dynamic memory allocation.

Angle Configuration
-------------------

- Set the starting angle of the arc with :cpp:expr:`lv_arclabel_set_angle_start(arclabel, angle)`.
- Define the arc size (angular span) using :cpp:expr:`lv_arclabel_set_angle_size(arclabel, size)`.
- Angles are measured in degrees, starting from the positive x-axis (3 o'clock position) and increasing clockwise.

Direction
---------

Specify the text direction along the arc using :cpp:expr:`lv_arclabel_set_dir(arclabel, LV_ARCLABEL_DIR_CLOCKWISE)` or :cpp:expr:`lv_arclabel_set_dir(arclabel, LV_ARCLABEL_DIR_COUNTERCLOCKWISE)`.

- :cpp:enumerator:`LV_ARCLABEL_DIR_CLOCKWISE` Text flows in a clockwise direction along the arc.
- :cpp:enumerator:`LV_ARCLABEL_DIR_COUNTERCLOCKWISE` Text flows in a counter-clockwise direction along the arc.

Alignment
---------

Adjust vertical text alignment with :cpp:expr:`lv_arclabel_set_text_vertical_align(arclabel, LV_ARCLABEL_TEXT_ALIGN_CENTER)`.
Set horizontal alignment using :cpp:expr:`lv_arclabel_set_text_horizontal_align(arclabel, LV_ARCLABEL_TEXT_ALIGN_CENTER)`.

Both vertical and horizontal use the same logic.

- :cpp:enumerator:`LV_ARCLABEL_TEXT_ALIGN_DEFAULT` Uses the default alignment.
- :cpp:enumerator:`LV_ARCLABEL_TEXT_ALIGN_LEADING` Aligns text to the leading edge of the arc.
- :cpp:enumerator:`LV_ARCLABEL_TEXT_ALIGN_CENTER` Centers text along the arc.
- :cpp:enumerator:`LV_ARCLABEL_TEXT_ALIGN_TRAILING` Aligns text to the trailing edge of the arc.

Radius and Center Offset
------------------------

- Set the radius of the arc with :cpp:expr:`lv_arclabel_set_radius(arclabel, radius)`.
- Adjust the center offset of the arc using :cpp:expr:`lv_arclabel_set_center_offset_x(arclabel, x)` and :cpp:expr:`lv_arclabel_set_center_offset_y(arclabel, y)`.

Color and Recoloring
--------------------

Enable text recoloring with :cpp:expr:`lv_arclabel_set_recolor(arclabel, true)`. This allows parts of the text to be colored differently using color commands embedded in the text string.

Interactive Behavior
--------------------

By default, Arc Label is not clickable. To make it interactive, you would need to add custom event handling, as it does not inherit clickability by default.

.. _lv_arclabel_events:

Events
******

The Arc Label primarily inherits events from the base object class. It does not define specific events beyond those common to all widgets. You can attach custom event handlers to respond to interactions if needed.

.. _lv_arclabel_keys:

Keys
****

The Arc Label does not define specific key bindings beyond those inherited from the base object class. Keyboard navigation and interaction would require additional implementation.

.. _lv_arclabel_example:

Example
*******

.. include:: ../../examples/widgets/arclabel/index.rst

.. _lv_arclabel_api:

API
***

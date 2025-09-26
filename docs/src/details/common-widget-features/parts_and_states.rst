
.. _widget_parts_and_states:

================
Parts and States
================



.. _widget_parts:

Parts
*****

Widgets are built from multiple parts. For example, a
:ref:`Base Widget <base_widget>` has "main" and "scrollbar" parts, while a
:ref:`Slider <lv_slider>` has "main", "indicator", and "knob" parts. Parts are
similar to *pseudo-elements* in CSS.

The following predefined parts exist in LVGL:

- :cpp:enumerator:`LV_PART_MAIN`: A background-like rectangle.
- :cpp:enumerator:`LV_PART_SCROLLBAR`: The scrollbar(s).
- :cpp:enumerator:`LV_PART_INDICATOR`: The indicator, e.g., for sliders, bars,
  switches, or the tick box of a checkbox.
- :cpp:enumerator:`LV_PART_KNOB`: A handle used to adjust a value.
- :cpp:enumerator:`LV_PART_SELECTED`: Indicates the currently selected option or
  section.
- :cpp:enumerator:`LV_PART_ITEMS`: Used if the widget has multiple similar elements
  (e.g., table cells).
- :cpp:enumerator:`LV_PART_CURSOR`: Marks a specific position, e.g., the cursor in a
  text area or chart.
- :cpp:enumerator:`LV_PART_CUSTOM_FIRST`: Custom parts can be added starting from
  here.

The main purpose of parts is to allow styling individual "components" of a widget.
They are described in more detail in the :ref:`Style overview <styles>` section.



.. _widget_states:

States
******

A widget can be in a combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: Normal, released state.
- :cpp:enumerator:`LV_STATE_CHECKED`: Toggled or checked state.
- :cpp:enumerator:`LV_STATE_FOCUSED`: Focused via keypad, encoder, or clicked via
  touchpad/mouse.
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: Focused via keypad or encoder but not via
  touchpad/mouse.
- :cpp:enumerator:`LV_STATE_EDITED`: Being edited by an encoder.
- :cpp:enumerator:`LV_STATE_HOVERED`: Hovered by a mouse (currently not supported).
- :cpp:enumerator:`LV_STATE_PRESSED`: Being pressed.
- :cpp:enumerator:`LV_STATE_SCROLLED`: Being scrolled.
- :cpp:enumerator:`LV_STATE_DISABLED`: Disabled state.
- :cpp:enumerator:`LV_STATE_USER_1`: Custom state.
- :cpp:enumerator:`LV_STATE_USER_2`: Custom state.
- :cpp:enumerator:`LV_STATE_USER_3`: Custom state.
- :cpp:enumerator:`LV_STATE_USER_4`: Custom state.

States are usually changed automatically by the library as the user interacts with a
widget (e.g., pressing, releasing, focusing). However, states can also be modified
manually. To set or clear a given state (while leaving other states untouched), use:

- :cpp:expr:`lv_obj_add_state(widget, LV_STATE_...)`
- :cpp:expr:`lv_obj_remove_state(widget, LV_STATE_...)`

In both cases, you can bit-wise OR multiple state values. For example:
:cpp:expr:`lv_obj_add_state(widget, LV_STATE_PRESSED | LV_STATE_CHECKED)`

To learn more about states, see the related section in :ref:`styles_overview`.

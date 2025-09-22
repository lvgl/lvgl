.. _widget_parts_and_states:

================
Parts and States
================

.. _widget_parts:

Parts
*****

The widgets are built from multiple parts. For example a
:ref:`Base Widget <base_widget>` uses the main and scrollbar parts but a
:ref:`Slider <lv_slider>` uses the main, indicator and knob parts.
Parts are similar to *pseudo-elements* in CSS.

The following predefined parts exist in LVGL:

- :cpp:enumerator:`LV_PART_MAIN`: A background like rectangle
- :cpp:enumerator:`LV_PART_SCROLLBAR`: The scrollbar(s)
- :cpp:enumerator:`LV_PART_INDICATOR`: Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox
- :cpp:enumerator:`LV_PART_KNOB`: Like a handle to grab to adjust the value
- :cpp:enumerator:`LV_PART_SELECTED`: Indicate the currently selected option or section
- :cpp:enumerator:`LV_PART_ITEMS`: Used if the widget has multiple similar elements (e.g. table cells)
- :cpp:enumerator:`LV_PART_CURSOR`: Mark a specific place e.g. text area's or chart's cursor
- :cpp:enumerator:`LV_PART_CUSTOM_FIRST`: Custom parts can be added from here.

The main purpose of parts is to allow styling the "components" of the
widgets. They are described in more detail in the
:ref:`Style overview <styles>` section.

.. _widget_states:

States
******

The Widget can be in a combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: Focused via keypad or encoder or clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: Focused via keypad or encoder but not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: Edit by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: Hovered by mouse (not supported now)
- :cpp:enumerator:`LV_STATE_PRESSED`: Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: Disabled state
- :cpp:enumerator:`LV_STATE_USER_1`: Custom state
- :cpp:enumerator:`LV_STATE_USER_2`: Custom state
- :cpp:enumerator:`LV_STATE_USER_3`: Custom state
- :cpp:enumerator:`LV_STATE_USER_4`: Custom state

The states are usually automatically changed by the library as the user
interacts with a Widget (presses, releases, focuses, etc.). However,
the states can be changed manually as well. To set or clear given state (but
leave the other states untouched) use
:cpp:expr:`lv_obj_add_state(widget, LV_STATE_...)` and
:cpp:expr:`lv_obj_remove_state(widget, LV_STATE_...)`.  In both cases OR-ed state
values can be used as well. E.g.
:cpp:expr:`lv_obj_add_state(widget, LV_STATE_PRESSED | LV_PRESSED_CHECKED)`.

To learn more about the states, read the related section of
:ref:`styles_overview`.


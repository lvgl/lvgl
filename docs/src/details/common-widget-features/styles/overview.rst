.. _styles_overview:

===============
Styles Overview
===============

Styles are used to set the appearance of Widgets. Styles in LVGL are heavily inspired
by CSS. The concept in a nutshell is that a style is an :cpp:type:`lv_style_t`
variable which can hold properties like border width, font, text color and so on.
It's similar to a ``class`` in CSS.

- Styles can be assigned to Widgets to change their appearance. Upon assignment, the
  target part (pseudo-element_ in CSS) and target state (pseudo-class_ in CSS) can be
  specified. For example one can add ``style_blue`` to the knob of a slider when it's
  in pressed state.
- The same style can be used by any number of Widgets.
- Styles can be cascaded which means multiple styles may be assigned to a Widget and
  each style can have different properties. Therefore, not all properties have to be
  specified in a style. LVGL will search for a property until a style defines it or
  use a default value if it's not specified by any of the styles. For example
  ``style_btn`` can result in a default gray button and ``style_btn_red`` can add only
  a ``background-color=red`` to overwrite the background color.
- The most recently added style has higher precedence. This means if a property is
  specified in two styles the newest style in the Widget will be used.
- Some properties (e.g. text color) can be inherited from a parent(s) if it's not
  specified in a Widget.
- Widgets can also have :ref:`local styles <style_local>` with higher precedence than
  "normal" styles.
- Unlike CSS (where pseudo-classes_ describe different states, e.g. ``:focus``), in
  LVGL a property is assigned to a given state.
- Transitions can be applied when the Widget changes state.

For the full list of style properties click :ref:`here <style_properties>`.



.. _style_states:

States
******

The Widgets can be in the combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: (0x0000) Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: (0x0001) Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: (0x0002) Focused via keypad or encoder or
  clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: (0x0004) Focused via keypad or encoder but
  not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: (0x0008) Edit by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: (0x0010) Hovered by mouse
- :cpp:enumerator:`LV_STATE_PRESSED`: (0x0020) Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: (0x0040) Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: (0x0080) Disabled state
- :cpp:enumerator:`LV_STATE_USER_1`: (0x1000) Custom state
- :cpp:enumerator:`LV_STATE_USER_2`: (0x2000) Custom state
- :cpp:enumerator:`LV_STATE_USER_3`: (0x4000) Custom state
- :cpp:enumerator:`LV_STATE_USER_4`: (0x8000) Custom state

A Widget can be in a combination of states such as being focused and pressed at the
same time. This is represented as :cpp:expr:`LV_STATE_FOCUSED | LV_STATE_PRESSED`.

A style can be added to any state or state combination. For example, setting a
different background color for the default and pressed states. If a property is not
defined in a state the best matching state's property will be used. Typically this
means the property with :cpp:enumerator:`LV_STATE_DEFAULT` is used. If the property is
not set even for the default state the default value will be used. (See later)

Since :cpp:enumerator:`LV_PART_MAIN` and :cpp:enumerator:`LV_STATE_DEFAULT` both have
zero values, you can simply pass ``0`` as the ``selector`` argument instead of
``LV_PART_MAIN | LV_STATE_DEFAULT`` as a shortcut when adding styles to an object.


What does the "best matching state's property" mean?
----------------------------------------------------

States have a precedence which is shown by their value (see in the above list). A
higher value means higher precedence. To determine which state's property to use let's
take an example. Imagine the background color is defined like this:

- :cpp:enumerator:`LV_STATE_DEFAULT`: white
- :cpp:enumerator:`LV_STATE_PRESSED`: gray
- :cpp:enumerator:`LV_STATE_FOCUSED`: red

1. Initially the Widget is in the default state, so it's a simple case: the property is
   perfectly defined in the Widget's current state as white.
2. When the Widget is pressed there are 2 related properties: default with white
   (default is related to every state) and pressed with gray. The pressed state has
   0x0020 precedence which is higher than the default state's 0x0000 precedence, so
   gray color will be used.
3. When the Widget has focus the same thing happens as in pressed state and red color
   will be used. (Focused state has higher precedence than default state).
4. When the Widget has focus and pressed both gray and red would work, but the pressed
   state has higher precedence than focused so gray color will be used.
5. It's possible to set e.g. rose color for
   :cpp:expr:`LV_STATE_PRESSED | LV_STATE_FOCUSED`. In this case, this combined state
   has 0x0020 + 0x0002 = 0x0022 precedence, which is higher than the pressed state's
   precedence so rose color would be used.
6. When the Widget is in the checked state there is no property to set the background
   color for this state. So for lack of a better option, the Widget remains white from
   the default state's property.

Some practical notes:

- The precedence (value) of states is quite intuitive, and it's something the user
  would expect naturally. Example: if a Widget has focus the user will still want to
  see if it's pressed, therefore the pressed state has a higher precedence. If the
  focused state had a higher precedence it would overwrite the pressed color.
- If you want to set a property for all states (e.g. red background color) just set it
  for the default state. If the Widget can't find a property for its current state it
  will fall back to the default state's property.
- Use ORed states to describe the properties for complex cases (e.g. pressed + checked
  + focused).
- It might be a good idea to use different style elements for different states. For
  example, finding background colors for released, pressed, checked + pressed,
  focused, focused + pressed, focused + pressed + checked, etc. states is quite
  difficult. Instead, for example, use the background color for pressed and checked
  states and indicate the focused state with a different border color.



.. _style_cascading:

Cascading Styles
****************

It's not required to set all the properties in one style. It's possible to add more
styles to a Widget and have the latter added style modify or extend appearance. For
example, create a general gray button style and create a new one for red buttons where
only the new background color is set.

This is much like in CSS when used classes are listed like ``<div class=".btn
.btn-red">``.

Styles added later have precedence over ones set earlier. So in the gray/red button
example above, the normal button style should be added first and the red style second.
However, the precedence of the states are still taken into account. So let's examine
the following case:

- the basic button style defines dark-gray color for the default state and light-gray
  color for the pressed state
- the red button style defines the background color as red only in the default state

In this case, when the button is released (it's in default state) it will be red
because a perfect match is found in the most recently added style (red). When the
button is pressed the light-gray color is a better match because it describes the
current state perfectly, so the button will be light-gray.



.. _style_inheritance:

Inheritance
***********

Some properties (typically those related to text) can be inherited from the parent
Widget's styles. Inheritance is applied only if the given property is not set in the
Widget's styles (even in default state). In this case, if the property is inheritable,
the property's value will be searched up the parent hierarchy until a Widget specifies
a value for the property. The parents will use their own state to determine the value.
So if a button is pressed, and the text color comes from a parent, the pressed text
color will be used.



.. _style_parts:

Parts
*****

Widgets can be composed of *parts* which may each have their own styles.

The following predefined parts exist in LVGL:

- :cpp:enumerator:`LV_PART_MAIN`: (0x000000) A background like rectangle
- :cpp:enumerator:`LV_PART_SCROLLBAR`: (0x010000) The scrollbar(s)
- :cpp:enumerator:`LV_PART_INDICATOR`: (0x020000) Indicator, e.g. for slider, bar,
  switch, or the tick box of the checkbox
- :cpp:enumerator:`LV_PART_KNOB`: (0x030000) Like a handle to grab to adjust a value
- :cpp:enumerator:`LV_PART_SELECTED`: (0x040000) Indicate the currently selected
  option or section
- :cpp:enumerator:`LV_PART_ITEMS`: (0x050000) Used if the widget has multiple similar
  elements (e.g. table cells)
- :cpp:enumerator:`LV_PART_CURSOR`: (0x060000) Mark a specific place e.g. Text Area's
  or chart's cursor
- :cpp:enumerator:`LV_PART_CUSTOM_FIRST`: (0x080000) Custom part identifiers can be
  added starting from here.
- :cpp:enumerator:`LV_PART_ANY`: (0x0F0000) Special value can be used in some
  functions to target all parts.

For example a :ref:`Slider <lv_slider>` has three parts:

- Main (background)
- Indicator
- Knob

This means all three parts of the slider can have their own styles. See later how to
add styles to Widgets and parts.

Since :cpp:enumerator:`LV_PART_MAIN` and :cpp:enumerator:`LV_STATE_DEFAULT` both have
zero values, you can simply pass ``0`` as the ``selector`` argument instead of
``LV_PART_MAIN | LV_STATE_DEFAULT`` as a shortcut when adding styles to an object.



.. _style_opacity_blend_modes_transformations:

Properties Requiring New Layers
*******************************

If the ``opa``, ``blend_mode``, ``transform_angle``, or ``transform_zoom`` properties
are set to a non-default value LVGL creates a snapshot of the widget and its children
in order to blend the whole widget with the set opacity, blend mode and transformation
properties.

These properties have this effect only on the ``MAIN`` part of the widget.

The created snapshot is called "intermediate layer" or simply "layer". If only ``opa``
and/or ``blend_mode`` is set to a non-default value LVGL can build the layer from
smaller chunks. The size of these chunks can be configured by the following properties
in ``lv_conf.h``:

- :cpp:enumerator:`LV_LAYER_SIMPLE_BUF_SIZE`: [bytes] the optimal target buffer size.
  LVGL will try to allocate this size of memory.
- :cpp:enumerator:`LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE`: [bytes] used if
  :cpp:enumerator:`LV_LAYER_SIMPLE_BUF_SIZE` couldn't be allocated.

If transformation properties were also used the layer cannot be rendered in chunks



.. _typical bg props:

Typical Background Properties
*****************************

In documentation of widgets you will see sentences like "XY Widget
uses the typical background style properties".  These "typical
background properties" are the properties being referred to:

- Background
- Border
- Outline
- Shadow
- Padding
- Width and height transformation
- X and Y translation


.. _style_examples:

Examples
********

.. include:: ../../../examples/styles/index.rst



..  Hyperlinks

.. _pseudo-elements:
.. _pseudo-element:   https://developer.mozilla.org/en-US/docs/Learn/CSS/Building_blocks/Selectors#pseudo-classes_and_pseudo-elements
.. _pseudo-classes:
.. _pseudo-class:     https://developer.mozilla.org/en-US/docs/Glossary/Pseudo-class



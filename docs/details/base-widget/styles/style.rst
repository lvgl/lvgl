.. _styles_overview:

===============
Styles Overview
===============

Styles are used to set the appearance of Widgets.  Styles in LVGL are
heavily inspired by CSS. The concept in a nutshell is that a
style is an :cpp:type:`lv_style_t` variable which can hold properties like
border width, font, text color and so on. It's similar to a ``class`` in CSS.

- Styles can be assigned to Widgets to change their appearance. Upon
  assignment, the target part (pseudo-element_ in CSS) and target state
  (pseudo-class_ in CSS) can be specified. For example one can add
  ``style_blue`` to the knob of a slider when it's in pressed state.
- The same style can be used by any number of Widgets.
- Styles can be cascaded which means multiple styles may be assigned to a Widget and
  each style can have different properties. Therefore, not all properties
  have to be specified in a style. LVGL will search for a property until a
  style defines it or use a default value if it's not specified by any of the
  styles. For example ``style_btn`` can result in a default gray button
  and ``style_btn_red`` can add only a ``background-color=red`` to
  overwrite the background color.
- The most recently added style has higher precedence. This means if a property
  is specified in two styles the newest style in the Widget will be used.
- Some properties (e.g. text color) can be inherited from a parent(s) if it's not specified in a Widget.
- Widgets can also have :ref:`local styles <style_local>` with higher precedence than "normal" styles.
- Unlike CSS (where pseudo-classes_ describe different states, e.g. ``:focus``),
  in LVGL a property is assigned to a given state.
- Transitions can be applied when the Widget changes state.



.. _style_states:

States
******

The Widgets can be in the combination of the following states:

- :cpp:enumerator:`LV_STATE_DEFAULT`: (0x0000) Normal, released state
- :cpp:enumerator:`LV_STATE_CHECKED`: (0x0001) Toggled or checked state
- :cpp:enumerator:`LV_STATE_FOCUSED`: (0x0002) Focused via keypad or encoder or clicked via touchpad/mouse
- :cpp:enumerator:`LV_STATE_FOCUS_KEY`: (0x0004) Focused via keypad or encoder but not via touchpad/mouse
- :cpp:enumerator:`LV_STATE_EDITED`: (0x0008) Edit by an encoder
- :cpp:enumerator:`LV_STATE_HOVERED`: (0x0010) Hovered by mouse
- :cpp:enumerator:`LV_STATE_PRESSED`: (0x0020) Being pressed
- :cpp:enumerator:`LV_STATE_SCROLLED`: (0x0040) Being scrolled
- :cpp:enumerator:`LV_STATE_DISABLED`: (0x0080) Disabled state
- :cpp:enumerator:`LV_STATE_USER_1`: (0x1000) Custom state
- :cpp:enumerator:`LV_STATE_USER_2`: (0x2000) Custom state
- :cpp:enumerator:`LV_STATE_USER_3`: (0x4000) Custom state
- :cpp:enumerator:`LV_STATE_USER_4`: (0x8000) Custom state

A Widget can be in a combination of states such as being focused and
pressed at the same time. This is represented as :cpp:expr:`LV_STATE_FOCUSED | LV_STATE_PRESSED`.

A style can be added to any state or state combination. For example,
setting a different background color for the default and pressed states.
If a property is not defined in a state the best matching state's
property will be used. Typically this means the property with
:cpp:enumerator:`LV_STATE_DEFAULT` is used.˛ If the property is not set even for the
default state the default value will be used. (See later)

What does the "best matching state's property" mean?
----------------------------------------------------
States have a precedence which is shown by their value (see in the above list).
A higher value means higher precedence. To determine which state's
property to use let's take an example. Imagine the background color is
defined like this:

- :cpp:enumerator:`LV_STATE_DEFAULT`: white
- :cpp:enumerator:`LV_STATE_PRESSED`: gray
- :cpp:enumerator:`LV_STATE_FOCUSED`: red

1. Initially the Widget is in the default state, so it's a simple case:
   the property is perfectly defined in the Widget's current state as
   white.
2. When the Widget is pressed there are 2 related properties: default
   with white (default is related to every state) and pressed with gray.
   The pressed state has 0x0020 precedence which is higher than the
   default state's 0x0000 precedence, so gray color will be used.
3. When the Widget has focus the same thing happens as in pressed state
   and red color will be used. (Focused state has higher precedence than
   default state).
4. When the Widget has focus and pressed both gray and red would work,
   but the pressed state has higher precedence than focused so gray
   color will be used.
5. It's possible to set e.g. rose color for :cpp:expr:`LV_STATE_PRESSED | LV_STATE_FOCUSED`.
   In this case, this combined state has 0x0020 + 0x0002 = 0x0022 precedence, which is higher than
   the pressed state's precedence so rose color would be used.
6. When the Widget is in the checked state there is no property to set
   the background color for this state. So for lack of a better option,
   the Widget remains white from the default state's property.

Some practical notes:

- The precedence (value) of states is quite intuitive, and it's something the
  user would expect naturally. Example:  if a Widget has focus the user will still
  want to see if it's pressed, therefore the pressed state has a higher
  precedence. If the focused state had a higher precedence it would overwrite
  the pressed color.
- If you want to set a property for all states (e.g. red background color)
  just set it for the default state. If the Widget can't find a property
  for its current state it will fall back to the default state's property.
- Use ORed states to describe the properties for complex cases (e.g.
  pressed + checked + focused).
- It might be a good idea to use different
  style elements for different states. For example, finding background
  colors for released, pressed, checked + pressed, focused, focused +
  pressed, focused + pressed + checked, etc. states is quite difficult.
  Instead, for example, use the background color for pressed and checked
  states and indicate the focused state with a different border color.



.. _style_cascading:

Cascading Styles
****************

It's not required to set all the properties in one style. It's possible
to add more styles to a Widget and have the latter added style modify
or extend appearance. For example, create a general gray button style
and create a new one for red buttons where only the new background color
is set.

This is much like in CSS when used classes are listed like
``<div class=".btn .btn-red">``.

Styles added later have precedence over ones set earlier. So in the
gray/red button example above, the normal button style should be added
first and the red style second. However, the precedence of the states
are still taken into account. So let's examine the following case:

- the basic button style defines dark-gray color for the default state and
  light-gray color for the pressed state
- the red button style defines the background color as red only in the default state

In this case, when the button is released (it's in default state) it
will be red because a perfect match is found in the most recently added
style (red). When the button is pressed the light-gray color is a better
match because it describes the current state perfectly, so the button
will be light-gray.



.. _style_inheritance:

Inheritance
***********

Some properties (typically those related to text) can be inherited from
the parent Widget's styles. Inheritance is applied only if the given
property is not set in the Widget's styles (even in default state). In
this case, if the property is inheritable, the property's value will be
searched up the parent hierarchy until a Widget specifies a value for the
property. The parents will use their own state to determine the value.
So if a button is pressed, and the text color comes from a parent, the
pressed text color will be used.



.. _style_parts:

Parts
*****

Widgets can be composed of *parts* which may each have their own styles.

The following predefined parts exist in LVGL:

- :cpp:enumerator:`LV_PART_MAIN`: A background like rectangle
- :cpp:enumerator:`LV_PART_SCROLLBAR`: The scrollbar(s)
- :cpp:enumerator:`LV_PART_INDICATOR`: Indicator, e.g. for slider, bar, switch, or the tick box of the checkbox
- :cpp:enumerator:`LV_PART_KNOB`: Like a handle to grab to adjust a value
- :cpp:enumerator:`LV_PART_SELECTED`: Indicate the currently selected option or section
- :cpp:enumerator:`LV_PART_ITEMS`: Used if the widget has multiple similar elements (e.g. table cells)
- :cpp:enumerator:`LV_PART_CURSOR`: Mark a specific place e.g. Text Area's or chart's cursor
- :cpp:enumerator:`LV_PART_CUSTOM_FIRST`: Custom part identifiers can be added starting from here.

For example a :ref:`Slider <lv_slider>` has three parts:

- Main (background)
- Indicator
- Knob

This means all three parts of the slider can have their own styles. See
later how to add styles to Widgets and parts.



.. _style_initialize:

Initialize Styles and Set/Get Properties
****************************************

Styles are stored in :cpp:type:`lv_style_t` variables. Style variables should be
``static``, global or dynamically allocated. In other words they cannot
be local variables in functions which are destroyed when the function
exits. Before using a style it should be initialized with
:cpp:expr:`lv_style_init(&my_style)`. After initializing a style, properties can
be added or changed.

Property set functions looks like this:
``lv_style_set_<property_name>(&style, <value>);`` For example:

.. code-block:: c

   static lv_style_t style_btn;
   lv_style_init(&style_btn);
   lv_style_set_bg_color(&style_btn, lv_color_hex(0x115588));
   lv_style_set_bg_opa(&style_btn, LV_OPA_50);
   lv_style_set_border_width(&style_btn, 2);
   lv_style_set_border_color(&style_btn, lv_color_black());

   static lv_style_t style_btn_red;
   lv_style_init(&style_btn_red);
   lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
   lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);

To remove a property use:

.. code-block:: c

   lv_style_remove_prop(&style, LV_STYLE_BG_COLOR);

To get a property's value from a style:

.. code-block:: c

   lv_style_value_t v;
   lv_result_t res = lv_style_get_prop(&style, LV_STYLE_BG_COLOR, &v);
   if(res == LV_RESULT_OK) {  /* Found */
       do_something(v.color);
   }

:cpp:union:`lv_style_value_t` has 3 fields, only one of which will apply, depending
on the type of property it is applied to:

- :cpp:member:`num`: for integer, boolean and opacity properties
- :cpp:member:`color`: for color properties
- :cpp:member:`ptr`: for pointer properties

To reset a style (free all its data) use:

.. code-block:: c

   lv_style_reset(&style);

Styles can be built as ``const`` as well to save RAM:

.. code-block:: c

   const lv_style_const_prop_t style1_props[] = {
      LV_STYLE_CONST_WIDTH(50),
      LV_STYLE_CONST_HEIGHT(50),
      LV_STYLE_CONST_PROPS_END
   };

   LV_STYLE_CONST_INIT(style1, style1_props);

Later ``const`` style can be used like any other style but (obviously)
new properties cannot be added.



.. _style_add_remove:

Add and remove styles to a widget
*********************************

A style on its own has no effect until it is added (assigned) to a Widget.


Add styles
----------

To add a style to a Widget use
``lv_obj_add_style(widget, &style, <selector>)``. ``<selector>`` is an
OR-ed value of parts and state to which the style should be added. Some
examples:

- :cpp:expr:`LV_PART_MAIN | LV_STATE_DEFAULT`
- :cpp:enumerator:`LV_STATE_PRESSED`: The main part in pressed state. :cpp:enumerator:`LV_PART_MAIN` can be omitted
- :cpp:enumerator:`LV_PART_SCROLLBAR`: The scrollbar part in the default state. :cpp:enumerator:`LV_STATE_DEFAULT` can be omitted.
- :cpp:expr:`LV_PART_SCROLLBAR | LV_STATE_SCROLLED`: The scrollbar part when the Widget is being scrolled
- :cpp:expr:`LV_PART_INDICATOR | LV_STATE_PRESSED | LV_STATE_CHECKED` The indicator part when the Widget is pressed and checked at the same time.

Using :cpp:func:`lv_obj_add_style`:

.. code-block:: c

   lv_obj_add_style(btn, &style_btn, 0);                     /* Default button style */
   lv_obj_add_style(btn, &btn_red, LV_STATE_PRESSED);        /* Overwrite only some colors to red when pressed */

Replace styles
--------------

To replace a specific style of a Widget use
:cpp:expr:`lv_obj_replace_style(widget, old_style, new_style, selector)`. This
function will only replace ``old_style`` with ``new_style`` if the
``selector`` matches the ``selector`` used in ``lv_obj_add_style``. Both
``old_style`` and ``new_style`` must not be ``NULL``.  Separate functions exist for
adding and removing styles.  If the combination of
``old_style`` and ``selector`` exists multiple times in ``obj``\ 's
styles, all occurrences will be replaced. The return value of the
function indicates whether at least one successful replacement took
place.

Using :cpp:func:`lv_obj_replace_style`:

.. code-block:: c

   lv_obj_add_style(btn, &style_btn, 0);                      /* Add a button style */
   lv_obj_replace_style(btn, &style_btn, &new_style_btn, 0);  /* Replace the button style with a different one */

Remove styles
-------------

To remove all styles from a Widget use :cpp:expr:`lv_obj_remove_style_all(widget)`.

To remove specific styles use
:cpp:expr:`lv_obj_remove_style(widget, style, selector)`. This function will remove
``style`` only if the ``selector`` matches with the ``selector`` used in
:cpp:func:`lv_obj_add_style`. ``style`` can be ``NULL`` to check only the
``selector`` and remove all matching styles. The ``selector`` can use
the :cpp:enumerator:`LV_STATE_ANY` and :cpp:enumerator:`LV_PART_ANY` values to remove the style from
any state or part.

Reporting style changes
-----------------------

If a style which is already assigned to a Widget changes (i.e. a
property is added or changed), the Widgets using that style should be
notified. There are 3 options to do this:

1. If you know that the changed properties can be applied by a simple redraw
   (e.g. color or opacity changes) just call :cpp:expr:`lv_obj_invalidate(widget)`
   or :cpp:expr:`lv_obj_invalidate(lv_screen_active())`.
2. If more complex style properties were changed or added, and you know which
   Widget(s) are affected by that style call :cpp:expr:`lv_obj_refresh_style(widget, part, property)`.
   To refresh all parts and properties use :cpp:expr:`lv_obj_refresh_style(widget, LV_PART_ANY, LV_STYLE_PROP_ANY)`.
3. To make LVGL check all Widgets to see if they use a style and refresh them
   when needed, call :cpp:expr:`lv_obj_report_style_change(&style)`. If ``style``
   is ``NULL`` all Widgets will be notified about a style change.

Get a style property's value on a Widget
----------------------------------------

To get the final value of a style's property considering

- cascading,
- inheritance,
- local styles and transitions (see below)

property "get" functions like this can be used: ``lv_obj_get_style_<property_name>(widget, <part>)``.
These functions use the Widget's current state and if no better candidate exists they return the default value.
For example:

.. code-block:: c

    lv_color_t color = lv_obj_get_style_bg_color(btn, LV_PART_MAIN);



.. _style_local:

Local Styles
************

In addition to "normal" styles, Widgets can also store local styles.
This concept is similar to inline styles in CSS
(e.g. ``<div style="color:red">``) with some modification.

Local styles are like normal styles, but they can't be shared among
other Widgets. If used, local styles are allocated automatically, and
freed when the Widget is deleted. They are useful to add local
customization to a Widget.

Unlike in CSS, LVGL local styles can be assigned to states
(pseudo-classes_) and parts (pseudo-elements_).

To set a local property use functions like
``lv_obj_set_style_<property_name>(widget, <value>, <selector>);``   For example:

.. code-block:: c

    lv_obj_set_style_bg_color(slider, lv_color_red(), LV_PART_INDICATOR | LV_STATE_FOCUSED);



.. _style_properties_overview:

Style Properties Overview
*************************

For the full list of style properties click :ref:`here <style_properties>`.


.. _typical bg props:

Typical background properties
-----------------------------

In documentation of widgets you will see sentences like "The
_____ Widget uses the typical background style properties".  These "typical
background properties" are the properties being referred to:

- Background
- Border
- Outline
- Shadow
- Padding
- Width and height transformation
- X and Y translation

See :ref:`boxing_model` for the meanings of these terms.



.. _style_transitions:

Transitions
***********

By default, when a Widget changes state (e.g. it's pressed) the new
properties from the new state are set immediately. However, with
transitions it's possible to play an animation on state change. For
example, on pressing a button its background color can be animated to
the pressed color over 300 ms.

The parameters of the transitions are stored in the styles. It's
possible to set

- the time of the transition
- the delay before starting the transition
- the animation path (also known as the timing or easing function)
- the properties to animate

The transition properties can be defined for each state. For example,
setting a 500 ms transition time in the default state means that when
the Widget goes to the default state a 500 ms transition time is
applied. Setting a 100 ms transition time in the pressed state causes a
100 ms transition when going to the pressed state. This example
configuration results in going to the pressed state quickly and then
going back to default slowly.

To describe a transition an :cpp:struct:`lv_transition_dsc_t` variable needs to be
initialized and added to a style:

.. code-block:: c

   /* Only its pointer is saved so must static, global or dynamically allocated */
   static const lv_style_prop_t trans_props[] = {
                                               LV_STYLE_BG_OPA, LV_STYLE_BG_COLOR,
                                               0, /* End marker */
   };

   static lv_style_transition_dsc_t trans1;
   lv_style_transition_dsc_init(&trans1, trans_props, lv_anim_path_ease_out, duration_ms, delay_ms);

   lv_style_set_transition(&style1, &trans1);



.. _style_opacity_blend_modes_transformations:

Opacity, Blend Modes and Transformations
****************************************

If the ``opa``, ``blend_mode``, ``transform_angle``, or
``transform_zoom`` properties are set to a non-default value LVGL
creates a snapshot of the widget and its children in order to
blend the whole widget with the set opacity, blend mode and
transformation properties.

These properties have this effect only on the ``MAIN`` part of the
widget.

The created snapshot is called "intermediate layer" or simply "layer".
If only ``opa`` and/or ``blend_mode`` is set to a non-default value LVGL
can build the layer from smaller chunks. The size of these chunks can be
configured by the following properties in ``lv_conf.h``:

- :cpp:enumerator:`LV_LAYER_SIMPLE_BUF_SIZE`: [bytes] the optimal target buffer size. LVGL will try to allocate this size of memory.
- :cpp:enumerator:`LV_LAYER_SIMPLE_FALLBACK_BUF_SIZE`: [bytes] used if :cpp:enumerator:`LV_LAYER_SIMPLE_BUF_SIZE` couldn't be allocated.

If transformation properties were also used the layer cannot be
rendered in chunks, but one larger memory block needs to be allocated. The
required memory depends on the angle, zoom and pivot parameters, and the
size of the area to redraw, but it's never larger than the size of the
widget (including the extra draw size used for shadow, outline, etc).

If the widget can fully cover the area to redraw, LVGL creates an RGB
layer (which is faster to render and uses less memory). If the opposite
case ARGB rendering needs to be used, a widget might not cover its area
if it has radius, ``bg_opa < 255``, has shadow, outline, etc.

The click area of the widget is also transformed accordingly.



.. _style_color_filter:

Color Filter
************

TODO



.. _style_themes:

Themes
******

Themes are a collection of styles. If there is an active theme LVGL
applies it to every newly-created widget. This will give a default appearance
to the UI which can then be modified by adding further styles.

Every display can have a different theme. For example, you could have a
colorful theme on a TFT and monochrome theme on a secondary monochrome
display.

To set a theme for a display, two steps are required:

1. Initialize a theme
2. Assign the initialized theme to a display.

Theme initialization functions can have different prototypes. This
example shows how to set the "default" theme:

.. code-block:: c

   lv_theme_t * th = lv_theme_default_init(display,                 /* Use DPI, size, etc. from this display */
                                           LV_COLOR_PALETTE_BLUE,   /* Primary and secondary palette */
                                           LV_COLOR_PALETTE_CYAN,
                                           false,                   /* Dark theme?  False = light theme. */
                                           &lv_font_montserrat_10,  /* Small, normal, large fonts */
                                           &lv_font_montserrat_14,
                                           &lv_font_montserrat_18);

   lv_display_set_theme(display, th); /* Assign theme to display */

The included themes are enabled in ``lv_conf.h``. If the default theme
is enabled by :c:macro:`LV_USE_THEME_DEFAULT` LVGL automatically initializes
and sets it when a display is created.

Extending themes
----------------

Built-in themes can be extended. If a custom theme is created, a parent
theme can be selected. The parent theme's styles will be added before
the custom theme's styles. Any number of themes can be chained this way.
E.g. default theme -> custom theme -> dark theme.

:cpp:expr:`lv_theme_set_parent(new_theme, base_theme)` extends the
``base_theme`` with the ``new_theme``.

There is an example of this below.

.. _styles_example:

Examples
********

.. include:: ../../../examples/styles/index.rst



..  Hyperlinks

.. _pseudo-elements:
.. _pseudo-element:   https://developer.mozilla.org/en-US/docs/Learn/CSS/Building_blocks/Selectors#pseudo-classes_and_pseudo-elements
.. _pseudo-classes:
.. _pseudo-class:     https://developer.mozilla.org/en-US/docs/Glossary/Pseudo-class




.. _styles_api:

API
***

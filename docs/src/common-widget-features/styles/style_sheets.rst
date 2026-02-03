.. _style_initialize:

============
Style Sheets
============

Overview
********

Initializing Styles
*******************

Styles are stored in :cpp:type:`lv_style_t` variables. Style variables should be
``static``, global or dynamically allocated. In other words they cannot
be local variables in functions which are no longer valid after the function
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

To reset a style (freeing all its data) use:

.. code-block:: c

   lv_style_reset(&style);

If a style will not need to be changed at runtime, styles can be created as ``const`` variables as well to save RAM:

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

Adding Styles to Widgets
************************

A style on its own has no effect until it is added (assigned) to a Widget.

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


Removing Styles
****************

To remove all styles from a Widget use :cpp:expr:`lv_obj_remove_style_all(widget)`.

To remove specific styles use
``lv_obj_remove_style(widget, &style, selector)``. This function will remove
``style`` only if the ``selector`` matches with the ``selector`` used in
:cpp:func:`lv_obj_add_style`. The ``style`` argument can be ``NULL`` to match only the
``selector`` and remove all matching styles. The ``selector`` can also use
the :cpp:enumerator:`LV_STATE_ANY` and :cpp:enumerator:`LV_PART_ANY` values to remove the style from
any state or part.


Replacing Styles
****************

To replace a specific style of a Widget use
:cpp:expr:`lv_obj_replace_style(widget, old_style, new_style, selector)`. This
function will only replace ``old_style`` with ``new_style`` if the
``selector`` matches the ``selector`` used in :cpp:func:`lv_obj_add_style`. Both
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


Reporting Style Changes
***********************

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


Binding Styles
**************

By using :cpp:expr:`lv_obj_bind_style`, it's possible to add a style to a Widget
but enable it only if a :ref:`Subject <observer_overview>`'s value is equal to
a reference value.

It's a great way to implement a light/dark theme switch by normally adding the styles
for the light theme, and binding only a few styles for the dark theme to change
only a few colors if, e.g., a ``dark_theme`` subject is ``1``.

For example:

.. code-block:: c

   /*Style for the light theme*/
   lv_obj_add_style(btn, &style_btn, LV_PART_MAIN);
   lv_obj_add_style(btn, &style_btn_pressed, LV_STATE_PRESSED);

   /*Style for the dark theme changing only a few colors*/
   lv_obj_bind_style(btn, &style_btn_dark, LV_PART_MAIN, &dark_theme_subject, 1);
   lv_obj_bind_style(btn, &style_btn_pressed_dark, LV_STATE_PRESSED, &dark_theme_subject, 1);

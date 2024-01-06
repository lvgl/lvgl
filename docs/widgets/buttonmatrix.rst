.. _lv_buttonmatrix:

===============================
Button matrix (lv_buttonmatrix)
===============================

Overview
********

The Button Matrix object is a lightweight way to display multiple
buttons in rows and columns. Lightweight because the buttons are not
actually created but just virtually drawn on the fly. This way, one
button use only eight extra bytes of memory instead of the ~100-150
bytes a normal `Button </widgets/button>`__ object plus the 100 or so bytes
for the `Label </widgets/label>`__ object.

The Button matrix is added to the default group (if one is set). Besides
the Button matrix is an editable object to allow selecting and clicking
the buttons with encoder navigation too.

.. _lv_buttonmatrix_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the button matrix, uses the
   typical background style properties. ``pad_row`` and ``pad_column``
   sets the space between the buttons.
-  :cpp:enumerator:`LV_PART_ITEMS` The buttons all use the text and typical background
   style properties except translations and transformations.

.. _lv_buttonmatrix_usage:

Usage
*****

Button's text
-------------

There is a text on each button. To specify them a descriptor string
array, called *map*, needs to be used. The map can be set with
:cpp:expr:`lv_buttonmatrix_set_map(buttonm, my_map)`. The declaration of a map should
look like :cpp:expr:`const char * map[] = {"button1", "button2", "button3", NULL}`. Note
that the last element has to be either ``NULL`` or an empty string
(``""``)!

Use ``"\n"`` in the map to insert a **line break**. E.g.
``{"button1", "button2", "\n", "button3", ""}``. Each line's buttons have their
width calculated automatically. So in the example the first row will
have 2 buttons each with 50% width and a second row with 1 button having
100% width.

Control buttons
---------------

The buttons' width can be set relative to the other button in the same
row with :cpp:expr:`lv_buttonmatrix_set_button_width(buttonm, button_id, width)` E.g. in a
line with two buttons: *buttonA, width = 1* and *buttonB, width = 2*, *buttonA*
will have 33 % width and *buttonB* will have 66 % width. It's similar to
how the
`"flex-grow" <https://developer.mozilla.org/en-US/docs/Web/CSS/flex-grow>`__
property works in CSS. The width must be in the [1..15] range and the
default width is 1.

In addition to the width, each button can be customized with the
following parameters:

- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_HIDDEN`: Makes a button hidden (hidden buttons still take up space in the layout, they are just not visible or clickable)
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_NO_REPEAT`: Disable repeating when the button is long pressed
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_DISABLED`: Makes a button disabled Like :cpp:enumerator:`LV_STATE_DISABLED` on normal objects
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CHECKABLE`: Enable toggling of a button. I.e. :cpp:enumerator:`LV_STATE_CHECKED` will be added/removed as the button is clicked
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CHECKED`: Make the button checked. It will use the :cpp:enumerator:`LV_STATE_CHECHKED` styles.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CLICK_TRIG`: Enabled: send LV_EVENT_VALUE_CHANGE on CLICK, Disabled: send :cpp:enumerator:`LV_EVENT_VALUE_CHANGE` on PRESS
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_POPOVER`: Show the button label in a popover when pressing this key
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CUSTOM_1`: Custom free to use flag
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CUSTOM_2`: Custom free to use flag

By default, all flags are disabled.

To set or clear a button's control attribute, use
:cpp:expr:`lv_buttonmatrix_set_button_ctrl(buttonm, button_id, LV_BUTTONMATRIX_CTRL_...)` and
:cpp:expr:`lv_buttonmatrix_clear_button_ctrl(buttonm, button_id, LV_BUTTONMATRIX_CTRL_...)`
respectively. More ``LV_BUTTONMATRIX_CTRL_...`` values can be OR-ed

To set/clear the same control attribute for all buttons of a button
matrix, use :cpp:expr:`lv_buttonmatrix_set_button_ctrl_all(buttonm, LV_BUTTONMATRIX_CTRL_...)`
and :cpp:expr:`lv_buttonmatrix_clear_button_ctrl_all(buttonm, LV_BUTTONMATRIX_CTRL_...)`.

The set a control map for a button matrix (similarly to the map for the
text), use :cpp:expr:`lv_buttonmatrix_set_ctrl_map(buttonm, ctrl_map)`. An element of
``ctrl_map`` should look like
:cpp:expr:`ctrl_map[0] = width | LV_BUTTONMATRIX_CTRL_NO_REPEAT |  LV_BUTTONMATRIX_CTRL_CHECHKABLE`.
The number of elements should be equal to the number of buttons
(excluding newlines characters).

One check
---------

The "One check" feature can be enabled with
:cpp:expr:`lv_buttonmatrix_set_one_checked(buttonm, true)` to allow only one button to
be checked at a time.

.. _lv_buttonmatrix_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED`: Sent when a button is pressed/released or
   repeated after long press. The event parameter is set to the ID of
   the pressed/released button.

See the events of the :ref:`Base object <lv_obj>` too.

:cpp:expr:`lv_buttonmatrix_get_selected_button(buttonm)` returns the index of the most
recently released or focused button or :cpp:enumerator:`LV_BUTTONMATRIX_BUTTON_NONE` if no
such button.

:cpp:expr:`lv_buttonmatrix_get_button_text(buttonm, button_id)` returns a pointer to the
text of ``button_id``\ th button.

Learn more about :ref:`events`.

.. _lv_buttonmatrix_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons to
   select one
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected button

Note that long pressing the button matrix with an encoder can mean to
enter/leave edit mode and simply long pressing a button to make it
repeat as well. To avoid this contradiction it's suggested to add
:cpp:expr:`lv_buttonmatrix_set_button_ctrl_all(buttonm, LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_NO_REPEAT)`
to the button matrix if used with encoder. This way, the pressed button
repeat feature is disabled and on leaving edit mode the selected button
won't be activated.

Learn more about :ref:`indev_keys`.

.. _lv_buttonmatrix_example:

Example
*******

.. include:: ../examples/widgets/buttonmatrix/index.rst

.. _lv_buttonmatrix_api:

API
***

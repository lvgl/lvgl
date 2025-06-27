.. _lv_buttonmatrix:

===============================
Button Matrix (lv_buttonmatrix)
===============================


Overview
********

The Button Matrix Widget is a lightweight way to display multiple
Buttons in rows and columns --- lightweight because the buttons are not
actually created but just virtually drawn on the fly.  With Button Matrix,
each button uses only eight extra bytes of memory instead of the ~100-150
bytes a normal :ref:`Button <lv_button>` Widget plus the 100 or so bytes
for the :ref:`Label <lv_label>` Widget.

New Button Matrix Widgets are added to the default group (if one is set).
Additionally, Button Matrix is an editable Widget:  it allows selecting and clicking
the buttons with encoder and keyboard navigation as well.


.. _lv_buttonmatrix_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Button Matrix, uses the
   typical background style properties. ``pad_row`` and ``pad_column``
   sets the space between the buttons.
-  :cpp:enumerator:`LV_PART_ITEMS` The buttons all use the text and typical background
   style properties except translations and transformations.


.. _lv_buttonmatrix_usage:

Usage
*****

.. _button map:

Button map
-----------

The number of buttons, their positions and text are controlled by a descriptor string
array, called a *map*, passed to
:cpp:expr:`lv_buttonmatrix_set_map(btn_matrix, my_map)`. The declaration of a map should
look like ``const char * map[] = {"button1", "button2", "button3", NULL}``. Note
that the last element must be either ``NULL`` or an empty string
(``""``)!

Use ``"\n"`` in the map to insert a **line break**. E.g.
``{"button1", "button2", "\n", "button3", ""}``. Each line's buttons have their
width calculated automatically. So in the example the first row will
have 2 buttons each with 50% width and a second row with 1 button having
100% width.

.. note::
    The number of buttons neither includes the newline elements nor the terminating
    element of the array.

Button widths
-------------

The buttons' width can be set in proportion to the width of other buttons in the same
row with :cpp:expr:`lv_buttonmatrix_set_button_width(btn_matrix, button_id, width)` E.g. in a
line with two buttons: *buttonA, width = 1* and *buttonB, width = 2*, *buttonA*
will have 33 % width and *buttonB* will have 66 % width.  This is similar to how the
`"flex-grow" <https://developer.mozilla.org/en-US/docs/Web/CSS/flex-grow>`__
property works in CSS. The width must be in the range [1..15] with the default being 1.

Button behavior
---------------

Each button's behavior can be customized with the following control flags:

- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_HIDDEN`: Hides button; it continues to hold its space in layout.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_NO_REPEAT`: Do not emit :cpp:enumerator:`LV_EVENT_LONG_PRESSED_REPEAT` events while button is long-pressed.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_DISABLED`: Disables button like :cpp:enumerator:`LV_STATE_DISABLED` on normal Widgets.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CHECKABLE`: Enable toggling of :cpp:enumerator:`LV_STATE_CHECKED` when clicked.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CHECKED`: Make the button checked. It will use the :cpp:enumerator:`LV_STATE_CHECKED` styles.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CLICK_TRIG`: 1: Enables sending :cpp:enumerator:`LV_EVENT_VALUE_CHANGE` on ``CLICK``, 0: sends :cpp:enumerator:`LV_EVENT_VALUE_CHANGE` on ``PRESS``.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_POPOVER`: Show button text in a pop-over while being pressed.
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_RECOLOR`: Enable text recoloring with ``#color``
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CUSTOM_1`: Custom free-to-use flag
- :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_CUSTOM_2`: Custom free-to-use flag

By default, these flags are disabled.

To set and clear a button's control flags, use

- :cpp:expr:`lv_buttonmatrix_set_button_ctrl(btn_matrix, button_id, LV_BUTTONMATRIX_CTRL_...)` and
- :cpp:expr:`lv_buttonmatrix_clear_button_ctrl(btn_matrix, button_id, LV_BUTTONMATRIX_CTRL_...)`

respectively.  ``button_id`` is a zero-based button index (0 = first button).
``LV_BUTTONMATRIX_CTRL_...`` values can be bit-wise OR-ed together when passed to
these functions.

To set and clear the same control attribute for all buttons in a Button Matrix, use

- :cpp:expr:`lv_buttonmatrix_set_button_ctrl_all(btn_matrix, LV_BUTTONMATRIX_CTRL_...)` and
- :cpp:expr:`lv_buttonmatrix_clear_button_ctrl_all(btn_matrix, LV_BUTTONMATRIX_CTRL_...)`

respectively.

To set a control map for a Button Matrix (similar to `Button map`_), use

- :cpp:expr:`lv_buttonmatrix_set_ctrl_map(btn_matrix, ctrl_map)`.

An element of ``ctrl_map`` should look like
:cpp:expr:`ctrl_map[0] = width | LV_BUTTONMATRIX_CTRL_NO_REPEAT |  LV_BUTTONMATRIX_CTRL_CHECKABLE`.
The number of elements must be equal to the number of buttons.

One checked
-----------

The "One-checked" feature can be enabled with
:cpp:expr:`lv_buttonmatrix_set_one_checked(btn_matrix, true)` to allow only one button to
be checked at a time.



.. _lv_buttonmatrix_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED`: Sent when a button is pressed/released or
   repeated after long press. The event parameter is set to the ID of
   the pressed/released button.

:cpp:expr:`lv_buttonmatrix_get_selected_button(btn_matrix)` returns the index of the button
most recently released (the button with focus) or :cpp:enumerator:`LV_BUTTONMATRIX_BUTTON_NONE`
if no such button was found.

:cpp:expr:`lv_buttonmatrix_get_button_text(btn_matrix, button_id)` returns a pointer
to the text of the button specified by zero-based index ``button_id``.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_buttonmatrix_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons to
   select one
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected button

Note that long pressing the Button Matrix with an encoder can mean to
enter/leave edit mode and simply long pressing a button to make it
repeat as well. To avoid this contradiction, add
:cpp:expr:`lv_buttonmatrix_set_button_ctrl_all(btn_matrix, LV_BUTTONMATRIX_CTRL_CLICK_TRIG | LV_BUTTONMATRIX_CTRL_NO_REPEAT)`
to the Button Matrix if used with an encoder.  This disables the repeat feature so
the button will not be activated upon leaving edit mode.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_buttonmatrix_example:

Example
*******

.. include:: ../../examples/widgets/buttonmatrix/index.rst



.. _lv_buttonmatrix_api:

API
***

.. _lv_keyboard:

======================
Keyboard (lv_keyboard)
======================


Overview
********

The Keyboard Widget is a special :ref:`lv_buttonmatrix`
with predefined keymaps and other features to provide an on-screen virtual keyboard
to write text into a :ref:`lv_textarea`.


.. _lv_keyboard_parts_and_styles:

Parts and Styles
****************

Similar to Button Matrix, the Keyboard Widget consist of 2 part:

- :cpp:enumerator:`LV_PART_MAIN` The main part. Uses the :ref:`typical background
  style properties <typical bg props>`
- :cpp:enumerator:`LV_PART_ITEMS` The buttons. Also uses the :ref:`typical background
  style properties <typical bg props>` as well as *text* properties.


.. _lv_keyboard_usage:

Usage
*****

Modes
-----

Keyboards have the following modes:

- :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_LOWER` Display lower case letters
- :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_UPPER` Display upper case letters
- :cpp:enumerator:`LV_KEYBOARD_MODE_SPECIAL` Display special characters
- :cpp:enumerator:`LV_KEYBOARD_MODE_NUMBER` Display numbers, +/- sign, and decimal dot
- :cpp:enumerator:`LV_KEYBOARD_MODE_USER_1` through :cpp:enumerator:`LV_KEYBOARD_MODE_USER_4` User-defined modes.

The layouts of the ``TEXT`` modes contain "keys" to change mode.

To set the mode programmatically, use :cpp:expr:`lv_keyboard_set_mode(kb, mode)`. The
default mode is :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_UPPER`.

Assign Text Area
----------------

You can assign a :ref:`Text area <lv_textarea>` to the Keyboard to
automatically put the clicked characters there. To assign the Text Area,
use :cpp:expr:`lv_keyboard_set_textarea(kb, text_area)`.

Key Pop-Overs
-------------

To enable key pop-overs on press, like on common Android and iOS
keyboards, use :cpp:expr:`lv_keyboard_set_popovers(kb, true)`. Default
control maps are preconfigured to only show the pop-overs on keys that
produce a symbol (i.e. not on space). If you use a custom keymap (see below), set
the :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_POPOVER` flag for each key for which
a pop-over should be shown.

Note that pop-overs for keys in the top row will draw outside the Widget
boundaries. To account for this, reserve extra free space on top of the
Keyboard or ensure that the Keyboard is added *after* any Widgets
adjacent to its top boundary (placing it "above" those Widgets) so that pop-overs
will be drawn over them.

Pop-overs currently are merely a visual effect and don't allow
selecting additional characters such as accented characters yet.

New Keymap
----------

You can specify a new map (layout) for the Keyboard with
:cpp:expr:`lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_..., kb_map, kb_ctrl)`. See
Button Matrix's :ref:`button map` section for more information about
creating new maps.

Keep in mind that using following keywords in the map will have the same effect as
with the original map:

- :c:macro:`LV_SYMBOL_OK` Send :cpp:enumerator:`LV_EVENT_READY` to the assigned Text Area.
- :c:macro:`LV_SYMBOL_CLOSE` or :c:macro:`LV_SYMBOL_KEYBOARD` Send :cpp:enumerator:`LV_EVENT_CANCEL` to the assigned Text Area.
- :c:macro:`LV_SYMBOL_BACKSPACE` Delete character to the left.
- :c:macro:`LV_SYMBOL_LEFT` Move cursor left.
- :c:macro:`LV_SYMBOL_RIGHT` Move cursor right.
- :c:macro:`LV_SYMBOL_NEW_LINE` New line.
- ``"ABC"`` Load upper-case map.
- ``"abc"`` Load lower-case map.
- ``"1#"`` Load number map.

Key customization
-----------------

Each key can be customized by using draw events such as :cpp:enumerator:`LV_EVENT_DRAW_TASK_ADDED`.

Check out the examples to see how it can be used in the practice.


.. _lv_keyboard_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the button is pressed/released
   or repeated after long press. The event data contains the ID of the
   pressed/released button.
-  :cpp:enumerator:`LV_EVENT_READY`: The *Ok* button was clicked.
-  :cpp:enumerator:`LV_EVENT_CANCEL`: The *Close* button was clicked.

The Keyboard has a **default event handler** callback called
:cpp:func:`lv_keyboard_def_event_cb`, which handles the button pressing, map
changing, sending events to the assigned text area, etc. You can remove it and replace it
with a custom event handler if you wish, or add an additional call-back of your own.

.. note::

    In LVGL v8.0 and newer, adding an event handler to the Keyboard does not remove the
    default event handler. This behavior differs from v7, where adding an event
    handler would replace the previous one.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_keyboard_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons,
   selecting the one navigated to.
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected button.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_keyboard_example:

Example
*******

.. include:: ../../examples/widgets/keyboard/index.rst



.. _lv_keyboard_api:

API
***

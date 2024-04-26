.. _lv_keyboard:

======================
Keyboard (lv_keyboard)
======================

Overview
********

The Keyboard object is a special :ref:`Button matrix <lv_buttonmatrix>`
with predefined keymaps and other features to realize a virtual keyboard
to write texts into a :ref:`Text area <lv_textarea>`.

.. _lv_keyboard_parts_and_styles:

Parts and Styles
****************

Similarly to Button matrices Keyboards consist of 2 part:

- :cpp:enumerator:`LV_PART_MAIN` The main part. Uses all the typical background properties
- :cpp:enumerator:`LV_PART_ITEMS` The buttons. Also uses all typical background properties as well as the *text* properties.

.. _lv_keyboard_usage:

Usage
*****

Modes
-----

The Keyboards have the following modes:

- :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_LOWER` Display lower case letters
- :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_UPPER` Display upper case letters
- :cpp:enumerator:`LV_KEYBOARD_MODE_SPECIAL` Display special characters
- :cpp:enumerator:`LV_KEYBOARD_MODE_NUMBER` Display numbers, +/- sign, and decimal dot
- :cpp:enumerator:`LV_KEYBOARD_MODE_USER_1` through :cpp:enumerator:`LV_KEYBOARD_MODE_USER_4` User-defined modes.

The ``TEXT`` modes' layout contains buttons to change mode.

To set the mode manually, use :cpp:expr:`lv_keyboard_set_mode(kb, mode)`. The
default mode is :cpp:enumerator:`LV_KEYBOARD_MODE_TEXT_UPPER`.

Assign Text area
----------------

You can assign a :ref:`Text area <lv_textarea>` to the Keyboard to
automatically put the clicked characters there. To assign the text area,
use :cpp:expr:`lv_keyboard_set_textarea(kb, ta)`.

Key Popovers
------------

To enable key popovers on press, like on common Android and iOS
keyboards, use :cpp:expr:`lv_keyboard_set_popovers(kb, true)`. The default
control maps are preconfigured to only show the popovers on keys that
produce a symbol and not on e.g. space. If you use a custom keymap, set
the :cpp:enumerator:`LV_BUTTONMATRIX_CTRL_POPOVER` flag for all keys that you want to
show a popover.

Note that popovers for keys in the top row will draw outside the widget
boundaries. To account for this, reserve extra free space on top of the
keyboard or ensure that the keyboard is added *after* any widgets
adjacent to its top boundary so that the popovers can draw over those.

The popovers currently are merely a visual effect and don't allow
selecting additional characters such as accents yet.

New Keymap
----------

You can specify a new map (layout) for the keyboard with
:cpp:expr:`lv_keyboard_set_map(kb, LV_KEYBOARD_MODE_..., kb_map, kb_ctrl)`. See
the :ref:`Button matrix <lv_buttonmatrix>` for more information about
creating new maps and ctrls.

Keep in mind that using following keywords will have the same effect as
with the original map:

- :c:macro:`LV_SYMBOL_OK` Send :cpp:enumerator:`LV_EVENT_READY` to the assigned Text area.
- :c:macro:`LV_SYMBOL_CLOSE` or :c:macro:`LV_SYMBOL_KEYBOARD` Send :cpp:enumerator:`LV_EVENT_CANCEL` to the assigned Text area.
- :c:macro:`LV_SYMBOL_BACKSPACE` Delete on the left.
- :c:macro:`LV_SYMBOL_LEFT` Move the cursor left.
- :c:macro:`LV_SYMBOL_RIGHT` Move the cursor right.
- :c:macro:`LV_SYMBOL_NEW_LINE` New line.
- ``"ABC"`` Load the uppercase map.
- ``"abc"`` Load the lower case map.
- ``"1#"`` Load the lower case map.

.. _lv_keyboard_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the button is pressed/released
   or repeated after long press. The event data is set to the ID of the
   pressed/released button.
-  :cpp:enumerator:`LV_EVENT_READY`: The *Ok* button is clicked.
-  :cpp:enumerator:`LV_EVENT_CANCEL`: The *Close* button is clicked.

The keyboard has a **default event handler** callback called
:cpp:func:`lv_keyboard_def_event_cb`, which handles the button pressing, map
changing, the assigned text area, etc. You can remove it and replace it
with a custom event handler if you wish.


:note: In 8.0 and newer, adding an event handler to the keyboard does not remove the default event handler.
       This behavior differs from v7, where adding an event handler would always replace the previous one.

Learn more about :ref:`events`.

.. _lv_keyboard_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP/LEFT/RIGHT`` To navigate among the buttons and
   select one.
-  :cpp:enumerator:`LV_KEY_ENTER` To press/release the selected button.

Learn more about :ref:`indev_keys`.

.. _lv_keyboard_example:

Example
*******

.. include:: ../examples/widgets/keyboard/index.rst

.. _lv_keyboard_api:

API
***

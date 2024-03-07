.. _lv_msgbox:

=======================
Message box (lv_msgbox)
=======================

Overview
********

The Message boxes act as pop-ups. They are built from a background
container, a title, an optional close button, a text and optional
buttons.

The text will be broken into multiple lines automatically and the height
will be set automatically to include the text and the buttons.

The message box can be modal (blocking clicks on the rest of the screen)
or not modal.

.. _lv_msgbox_parts_and_styles:

Parts and Styles
****************

The message box is built from other widgets, so you can check these
widgets' documentation for details.

- Background: :ref:`lv_obj`
- Close button: :ref:`lv_button`
- Title and text: :ref:`lv_label`
- Buttons: :ref:`lv_buttonmatrix`

.. _lv_msgbox_usage:

Usage
*****

Create a message box
--------------------

:cpp:expr:`lv_msgbox_create(parent, title, txt, btn_txts[], add_close_btn)`
creates a message box.

If ``parent`` is ``NULL`` the message box will be modal. ``title`` and
``txt`` are strings for the title and the text. ``btn_txts[]`` is an
array with the buttons' text.

E.g. :cpp:expr:`const char * btn_txts[] = {"Ok", "Cancel", NULL}`. 
``add_close_btn`` can be ``true`` or ``false`` to add/don't add a close button.

Get the parts
-------------

The building blocks of the message box can be obtained using the
following functions:

.. code:: c

   lv_obj_t * lv_msgbox_get_title(lv_obj_t * mbox);
   lv_obj_t * lv_msgbox_get_close_btn(lv_obj_t * mbox);
   lv_obj_t * lv_msgbox_get_text(lv_obj_t * mbox);
   lv_obj_t * lv_msgbox_get_btns(lv_obj_t * mbox);

Close the message box
---------------------

:cpp:expr:`lv_msgbox_close(msgbox)` closes (deletes) the message box.

.. _lv_msgbox_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` is sent by the buttons if one of them is
   clicked. :cpp:enumerator:`LV_OBJ_FLAG_EVENT_BUBBLE` is enabled on the buttons so
   you can add events to the message box itself. In the event handler,
   :cpp:expr:`lv_event_get_target(e)` will return the button matrix and
   :cpp:expr:`lv_event_get_current_target(e)` will return the message box.
   :cpp:expr:`lv_msgbox_get_active_button(msgbox)` and
   :cpp:expr:`lv_msgbox_get_active_button_text(msgbox)` can be used to get the
   index and text of the clicked button.

Learn more about :ref:`events`.

.. _lv_msgbox_keys:

Keys
****

Keys have effect on the close button and button matrix. You can add them
manually to a group if required.

Learn more about :ref:`indev_keys`.

.. _lv_msgbox_example:

Example
*******

.. include:: ../examples/widgets/msgbox/index.rst

.. _lv_msgbox_api:

API
***

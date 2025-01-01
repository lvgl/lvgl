.. _lv_msgbox:

=======================
Message Box (lv_msgbox)
=======================


Overview
********

Message boxes act as pop-ups. They are built from a content area
with a helper to add text, an optional header (which can have
a title, a close button, and other buttons), and an optional footer
with buttons.

The text will be broken into multiple lines and the height
will be set automatically. If the height
is set programmatically, the content will become scrollable.

The message box can be modal (blocking clicks on the rest of the screen)
or not modal.



.. _lv_msgbox_parts_and_styles:

Parts and Styles
****************

The message box is built from other Widgets, so you can check these
Widgets' documentation for details.

- Content, header, and footer: :ref:`base_widget`
- Buttons: :ref:`lv_button`
- Title and content text: :ref:`lv_label`



.. _lv_msgbox_usage:

Usage
*****


Create a message box
--------------------

:cpp:expr:`lv_msgbox_create(parent)` creates a message box.
If ``parent`` is ``NULL`` the message box will be modal, and will use the
:ref:`default_display`'s Top :ref:`Layer <screen_layers>` as a parent.


Adding buttons
--------------
If you want to add an [OK] or [Cancel] or other buttons for the
user to have a choice of responses, add each button using the
:cpp:expr:`lv_msgbox_add_footer_button(msgbox, btn_text)` function.  Calling this
function adds a footer (container) if one was not already present, and it returns a
pointer to the button created, which can be used to add ``LV_EVENT_CLICKED`` (or
other) events to detect and act on the user's response.

Footer buttons so added are evenly spaced and centered.

Buttons can also be added to the header if desired with
:cpp:expr:`lv_msgbox_add_header_button(msgbox, symbol)`.
Buttons so added are added to the right end of the header.


Getting the parts
-----------------

The building blocks of the message box can be obtained using the
following functions:

.. code-block:: c

   lv_obj_t * lv_msgbox_get_content(lv_obj_t * msgbox);
   lv_obj_t * lv_msgbox_get_title(lv_obj_t * msgbox);
   lv_obj_t * lv_msgbox_get_header(lv_obj_t * msgbox);
   lv_obj_t * lv_msgbox_get_footer(lv_obj_t * msgbox);

Functions that add something to the message box return a pointer to the newly added Widget:

.. code:: c

   lv_obj_t * lv_msgbox_add_text(lv_obj_t * msgbox, const char * text);
   lv_obj_t * lv_msgbox_add_title(lv_obj_t * msgbox, const char * title);
   lv_obj_t * lv_msgbox_add_close_button(lv_obj_t * msgbox);
   lv_obj_t * lv_msgbox_add_header_button(lv_obj_t * msgbox, const void * symbol);
   lv_obj_t * lv_msgbox_add_footer_button(lv_obj_t * msgbox, const char * text);


Close the message box
---------------------

:cpp:expr:`lv_msgbox_close(msgbox)` closes (deletes) the message box.

:cpp:expr:`lv_msgbox_close_async(msgbox)` closes (deletes) the message box
asynchronously. This is useful if you want the message box to close the on
the next call to ``lv_timer_handler`` instead of immediately.



.. _lv_msgbox_events:

Events
******

No special events are sent by Message Box Widgets.
See these Widgets' documentation for details:  :ref:`lv_button` and :ref:`lv_label`.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_msgbox_keys:

Keys
****

No *Keys* are processed by Message Box Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_msgbox_example:

Example
*******

.. include:: ../../examples/widgets/msgbox/index.rst



.. _lv_msgbox_api:

API
***

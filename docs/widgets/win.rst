.. _lv_win:

===============
Window (lv_win)
===============

Overview
********

The Window is container-like object built from a header with title and
buttons and a content area.

.. _lv_win_parts_and_styles:

Parts and Styles
****************

The Window is built from other widgets so you can check their
documentation for details:

- Background: :ref:`lv_obj`
- Header on the background: :ref:`lv_obj`
- Title on the header: :ref:`lv_label`
- Buttons on the header: :ref:`lv_button`
- Content area on the background: :ref:`lv_obj`

.. _lv_win_usage:

Usage
*****

Create a Window
---------------

:cpp:expr:`lv_win_create(parent, header_height)` creates a Window with an empty
header.

Title and buttons
-----------------

Any number of texts (but typically only one) can be added to the header
with :cpp:expr:`lv_win_add_title(win, "The title")`.

Control buttons can be added to the window's header with
:cpp:expr:`lv_win_add_button(win, icon, button_width)`. ``icon`` can be any image
source, and ``button_width`` is the width of the button.

The title and the buttons will be added in the order the functions are
called. So adding a button, a text and two other buttons will result in
a button on the left, a title, and 2 buttons on the right. The width of
the title is set to take all the remaining space on the header. In other
words: it pushes to the right all the buttons that are added after the
title.

.. _lv_win_get_parts:

Get the parts
*************

:cpp:expr:`lv_win_get_header(win)` returns a pointer to the header,
:cpp:expr:`lv_win_get_content(win)` returns a pointer to the content container
to which the content of the window can be added.

.. _lv_win_events:

Events
******

No special events are sent by the windows, however events can be added
manually to the return value of :cpp:func:`lv_win_add_button`.

Learn more about :ref:`events`.

.. _lv_win_keys:

Keys
****

No *Keys* are handled by the window.

Learn more about :ref:`indev_keys`.

.. _lv_win_example:

Example
*******

.. include:: ../examples/widgets/win/index.rst

.. _lv_win_api:

API
***

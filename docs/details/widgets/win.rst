.. _lv_win:

===============
Window (lv_win)
===============


Overview
********

The Window Widget is built from a header (like a title bar) with title and
buttons and a content area.



.. _lv_win_parts_and_styles:

Parts and Styles
****************

The Window is built from other Widgets so you can check their
documentation for details:

- Background: :ref:`base_widget`
- Header on the background: :ref:`base_widget`
- Title on the header: :ref:`lv_label`
- Buttons on the header: :ref:`lv_button`
- Content Area on the background: :ref:`base_widget`



.. _lv_win_usage:

Usage
*****


Create a Window
---------------

:cpp:expr:`lv_win_create(parent)` creates a Window that is initially
composed of the following Widget structure:

- Background (a :ref:`base_widget`, the main window container), is set up to be a
  Flex-Flow container that flows its contained Widgets vertically
  (:cpp:enumerator:`LV_FLEX_FLOW_COLUMN`).

- Header (like a title bar) is initially empty, and is a Flex-Flow container set up
  to flow its contained Widgets horizontally (:cpp:enumerator:`LV_FLEX_FLOW_ROW`),
  left to right.  The Header occupies the full width of the Background (its parent)
  and the top approximately 1/2 inch (according to :c:macro:`LV_DPI_DEF`).

- Content Area (a :ref:`base_widget`) occupies the full width of the Background (its
  parent) the remainder of the available Background area below the Header.  It is
  *not itself* a Flex-Flow container, but you can make it so if you wish.  See
  :ref:`flex` for details.


Title and buttons
-----------------

You can add Button and Label Widgets to the Header using these two functions.  They
will be placed in the Header in left-to-right order as they are added.  These
functions can be called in any order, any number of times.

- :cpp:expr:`lv_win_add_title(win, "The title")` adds a Label to the header, with
  long mode :c:enumerator:`LV_LABEL_LONG_DOT` so that if its text contents are wider
  than the area it has, the text will be truncated with an ellipsis ("...") placed
  at the end of the text.  It is set to be FLEX GROW 1, so if it is the only Label
  in the header, it will occupy all available space after any Buttons are added.
  If more than one label is added, each label will share that space equally unless
  its FLEX GROW value is altered.  See :ref:`flex` for details about how this works.
  Because of this, any buttons added after the last Label added will be "stacked"
  on the far right of the Header.

- :cpp:expr:`lv_win_add_button(win, icon, button_width)` adds a Button with the
  specified width that occupies the full height of the Header (its parent).  If
  ``icon`` is not NULL, an image is created, centered on the button, using ``icon``
  as its image source.  All valid image sources are supported, but a common source
  to use is one of the ``LV_SYMBOL_...`` macros, such as :c:macro:`LV_SYMBOL_CLOSE`
  to provide an "x" (close) symbol.  You get back a pointer to the Button created so
  you can add an event callback with it and/or whatever else might be needed.



.. _lv_win_get_parts:

Getting the parts
*****************

:cpp:expr:`lv_win_get_header(win)` returns a pointer to the header,
:cpp:expr:`lv_win_get_content(win)` returns a pointer to the content container
to which the content of the window can be added.



.. _lv_win_events:

Events
******

No special events are sent by Window Widgets, however events can be added
to any Buttons added.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_win_keys:

Keys
****

No *Keys* are processed by Window Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_win_example:

Example
*******

.. include:: ../../examples/widgets/win/index.rst



.. _lv_win_api:

API
***

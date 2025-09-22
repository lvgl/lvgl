.. _widget_basics:

========
Overview
========


What is a widget?
*****************

A Widget is the **basic building block** of the LVGL user interface.

Examples of Widgets:  :ref:`Base Widget (and Screen) <base_widget>`,
:ref:`Button <lv_button>`, :ref:`Label <lv_label>`,
:ref:`Image <lv_image>`, :ref:`List <lv_list>`,
:ref:`Chart <lv_chart>` and :ref:`Text Area <lv_textarea>`.

See :ref:`widgets` to see all Widget types.

.. _base_widget:
.. _lv_obj:

Base Widget
***********

The most fundamental of all Widgets is the Base Widget, on which all other widgets
are based.  From an Object-Oriented perspective, think of the Base Widget as the
Widget class from which all other Widgets inherit.

By this mechanism, all Widgets carry the features of the Base Widget.
Therefore, the functions and functionalities of the Base
Widget can be used with other widgets as well. For example
:cpp:expr:`lv_obj_set_width(slider, 100)`.

The Base Widget although it's just a simple rectangle it's the most
powerful widget as it supports all the featuers other widgets can do.

The base widget can have

- Children
- Position and Size
- Layouts
- Styles
- Events
- Animations
- Data bindings
- Flags
- Parts
- States
- And many more


A Slider widget just uses these features to realize a Slider-like behaviour, however it's all based on the
same style, event, layout, etc concepts that are supported by the base widget too.
All these versatile features are introduced in detail in the upciming sections.


In HTML terms, think of it as a ``<div>``, that with some CSS and JS is a very verstile
building block in HTML too.


API
***

.. API startswith:  lv_obj_

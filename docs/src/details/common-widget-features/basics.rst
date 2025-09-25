.. _widget_basics:

========
Overview
========

What is a Widget?
*****************

A Widget is the **basic building block** of the LVGL user interface.

Examples of Widgets: :ref:`Base Widget (and Screen) <base_widget>`,
:ref:`Button <lv_button>`, :ref:`Label <lv_label>`,
:ref:`Image <lv_image>`, :ref:`List <lv_list>`,
:ref:`Chart <lv_chart>`, and :ref:`Text Area <lv_textarea>`.

See :ref:`widgets` to view all widget types.

.. _base_widget:
.. _lv_obj:

Base Widget
***********

The most fundamental of all widgets is the Base Widget, on which all other widgets
are based. From an object-oriented perspective, think of the Base Widget as the
widget class from which all other widgets inherit.

By this mechanism, all widgets carry the features of the Base Widget.
Therefore, the functions and functionalities of the Base
Widget can be used with other widgets as well. For example:
:cpp:expr:`lv_obj_set_width(slider, 100)`.

Although it's just a simple rectangle, the Base Widget is the most
powerful widget, as it supports all the features that other widgets can use.

The Base Widget can have:

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

A Slider widget, for example, uses these features to realize slider-like behavior, but it’s all based on the
same style, event, layout, and other concepts supported by the Base Widget.

All these versatile features are introduced in detail in the upcoming sections.

In HTML terms, think of it as a ``<div>``—a very versatile
building block that, with some CSS and JS, can become almost anything.



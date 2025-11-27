.. _base_widget:
.. _lv_obj:



====================
Base Widget (lv_obj)
====================

The most fundamental of all widgets is the Base Widget, on which all other widgets
are based. From an object-oriented perspective, think of the Base Widget as the
widget class from which all other widgets inherit.

By this mechanism, all widgets carry the features of the Base Widget. Therefore, the
functions and functionalities of the Base Widget can be used with other widgets as
well. For example: :cpp:expr:`lv_obj_set_width(slider, 100)`.

Although it's just a simple rectangle, the Base Widget is the most powerful widget,
as it supports all the features that other widgets can use.

The Base Widget can have:

- :ref:`Children <obj_tree>`
- :ref:`Position and Size <coord>`
- :ref:`Layouts <layouts>`
- :ref:`Styles <styles>`
- :ref:`Events <events>`
- :ref:`Data bindings <observer>`
- :ref:`Flags <lv_obj_flags>`
- :ref:`Parts and States <widget_parts_and_states>`
- And many more

A Slider widget, for example, uses these features to realize slider-like behavior,
but it’s all based on the same style, event, layout, and other concepts supported by
the Base Widget.

All these common and versatile features are introduced in the
:ref:`common_widget_features` chapter.

In HTML terms, think of it as a ``<div>``, a very versatile building block that,
with some CSS and JS, can become almost anything.

.. _base_widget:

===========
Base Widget
===========

The most fundamental of all Widgets is the Base Widget, on which all other widgets
are based.  From an Object-Oriented perspective, think of the Base Widget as the
Widget class from which all other Widgets inherit.

And by this mechanisms, all Widgets carry the features of the :ref:`Base Widget
<base_widget_overview>`.  Therefore, the functions and functionalities of the Base
Widget can be used with other widgets as well.  For example
:cpp:expr:`lv_obj_set_width(slider, 100)`.

The Base Widget can be used directly as a simple widget.  While it is a simple
rectangle, it has a large number of features shared with all Widgets, detailed
below and in subsequent pages.  In HTML terms, think of it as a ``<div>``.




.. _base_widget:
.. _lv_obj:

====================
Base Widget (lv_obj)
====================

The most fundamental of all Widgets is the Base Widget, on which all other widgets
are based.  From an Object-Oriented perspective, think of the Base Widget as the
Widget class from which all other Widgets inherit.

By this mechanism, all Widgets carry :ref:`the features of the Base Widget
<common_widget_features>`. Therefore, the functions and functionalities of the Base
Widget can be used with other widgets as well. For example
:cpp:expr:`lv_obj_set_width(slider, 100)`.

The Base Widget can be used directly as a simple widget.  While it is a simple
rectangle, it has a large number of features shared with all Widgets, detailed
in :ref:`common_widget_features`.  In HTML terms, think of it as a ``<div>``.




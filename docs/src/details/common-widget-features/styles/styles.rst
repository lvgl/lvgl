.. _styles_overview:


Get a style property's value on a Widget
----------------------------------------

To get the final value of a style's property considering

- cascading,
- inheritance,
- local styles and transitions (see below)

property "get" functions like this can be used: ``lv_obj_get_style_<property_name>(widget, <part>)``.
These functions use the Widget's current state and if no better candidate exists they return the default value.
For example:

.. code-block:: c

    lv_color_t color = lv_obj_get_style_bg_color(btn, LV_PART_MAIN);




.. _style_properties_overview:

Style Properties Overview
*************************

For the full list of style properties click :ref:`here <style_properties>`.


.. _typical bg props:

Typical background properties
-----------------------------

In documentation of widgets you will see sentences like "The
_____ Widget uses the typical background style properties".  These "typical
background properties" are the properties being referred to:

- Background
- Border
- Outline
- Shadow
- Padding
- Width and height transformation
- X and Y translation

See :ref:`boxing_model` for the meanings of these terms.






.. _style_color_filter:

Color Filter
************

TODO




.. _styles_example:

Examples
********

.. include:: ../../../examples/styles/index.rst



.. _styles_api:

API
***

.. API equals:  lv_style_init, lv_style_t, lv_obj_add_style, LV_PART_MAIN, LV_STATE_DEFAULT

.. API startswith:  lv_style_set_, lv_obj_set_

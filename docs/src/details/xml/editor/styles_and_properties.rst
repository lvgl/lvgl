.. include:: custom_tools.txt

.. _styles and properties:

=====================
Styles and Properties
=====================

Styles
******

In the context of |lvglpro|, Styles refer to `LVGL Styles`_.

Styles are created by creating ``<style>`` XML elements within a ``<styles>`` list in
the scope where they will be applied:

:global scope:  is achieved by placing them in the ``globals.xml`` file.

:local scope:   local to the Component, Screen, or Widget, is achieved by placing them
                in the XML definition of the respective Component, Screen or Widget.



Properties
**********

In the context of |lvglpro|, Properties refer to a value "owned by" a Component
or Widget that stores a value that is part of that Component's or Widget's state.

Properties are created in by including ``<prop>`` elements within an ``<api>`` element
within the XML definition of that Component or Widget.  In many cases you will want to
make these properties an Observer_ of a Subject_.

You can do so by including a ``bind_pppppp="property_name"`` attribute in the
definition of the XML definition of the UI element involved, where ``pppppp`` is the
name of the property.  For example, since ``lv_slider`` Widget already has a property
named "value", this could look like this:

.. todo::  confirm or adjust the above

.. code-block:: xml

    <lv_slider bind_value="battery_value" min_value="0" max_value="100" align="left_mid" width="56" height="24">
        <!-- content here -->
    </lv_slider>

Substitute custom property names accordingly.

See :ref:`binding to data` for details.


.. _xml_components:

==========
Components
==========

Overview
********

``<component>`` can have ``<consts>``, ``<api>``, ``<styles>``, and ``<view>`` tags inside.

Unlike widgets (which are always compiled into the application), components can either:

1. Be loaded at runtime from XML.
2. Be exported to C code.



Usage from Exported Code
************************

From each component XML file, a C and H file is exported with a single function inside:

.. code-block:: c

    lv_obj_t * component_name_create(lv_obj_t * parent, ...api properties...);

When a component is used in another component’s XML code and the code is exported, this ``create`` function will be called.
This means that components do not have a detailed set/get API but can be created with a fixed set of parameters.

If the user needs to access or modify values dynamically, it is recommended to use a :ref:`subject  <observer>`.

The user can also call these ``_create_`` functions at any time from the application code.



Usage from XML
**************

Registration
------------

Once a component is created (e.g., ``my_button``), it can be registered by calling either:

- ``lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml");``
- ``lv_xml_component_register_from_data("my_button", xml_data_of_my_button);``

These registration functions process the XML data and store relevant information internally.
This is required to make LVGL recognize the component by name.

When loaded from a file, the file name is used as the component name.

Instantiation
-------------

After registration, a new instance of any registered component can be created with:

.. code-block:: c

    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "my_button", NULL);

The created widget is a normal LVGL widget that can be used like any other manually created widget.

The last parameter can be ``NULL`` or an attribute list, like this:

.. code-block:: c

    /* Can be local */
    char * my_button_attrs[] = {
        "x", "10",
        "y", "-10",
        "align", "bottom_left",
        "btn_text", "New button",
        NULL, NULL,
    };

    lv_obj_t * btn1 = lv_xml_create(lv_screen_active(), "my_button", my_button_attrs);



Parameters
**********

The properties of child elements can be adjusted, such as:

.. code-block:: xml

    <lv_label x="10" text="Hello"/>

These parameters can be set to a fixed value. However, with the help of ``<prop>`` elements inside the ``<api>`` tag,
they can also be passed when an instance is created. Only the whole property can be passed, but not individual ``<param>`` elements.

Additionally, when a component is created, it can use the extended widget's attributes
(see ``<view extends="...">``).

This means that components inherit the API of the extended widget as well.

The following example demonstrates parameter passing and the use of the
``text`` label property on a component:

.. code-block:: xml

    <!-- h3.xml -->
    <component>
        <view extends="lv_label"/>
    </component>

.. code-block:: xml

    <!-- red_button.xml -->
    <component>
        <api>
            <prop type="string" name="btn_text" default="None"/>
        </api>
        <view extends="lv_button" style_radius="0" style_bg_color="0xff0000">
            <h3 text="Some text"/>
            <h3 text="$btn_text" y="40"/>
        </view>
    </component>

.. code-block:: c

    lv_xml_component_register_from_file("A:path/to/h3.xml");
    lv_xml_component_register_from_file("A:path/to/red_button.xml");

    /* Creates a button with "None" text */
    lv_xml_create(lv_screen_active(), "red_button", NULL);

    /* Use attributes to set the button text */
    const char * attrs[] = {
        "btn_text", "Click here",
        NULL, NULL,
    };
    lv_xml_create(lv_screen_active(), "red_button", attrs);



Example
*******

.. include:: ../../../examples/others/xml/index.rst



API
***

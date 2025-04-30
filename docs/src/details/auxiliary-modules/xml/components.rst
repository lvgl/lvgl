.. _xml_components:

==========
Components
==========

Overview
********

``<component>`` the following child elements:

- ``<consts>``,
- ``<api>``,
- ``<styles>``, and
- ``<view>``.

Unlike Widgets (which are always compiled into the application), Components can either:

1. be loaded at runtime from XML, or
2. be exported to C code and compiled with the application.



Usage from Exported Code
************************

From each Component XML file, a C and H file is exported with a single function inside:

.. code-block:: c

    lv_obj_t * component_name_create(lv_obj_t * parent, ...api properties...);

where 'component_name' (in the function above) is replaced by the Component's XML
file name.

When a Component is used in another Component's XML code and the code is exported,
this ``create`` function will be called.  This means that Components do not have a
detailed set/get API but can be created with a fixed set of parameters.

If the user needs to access or modify values dynamically, it is recommended to use a
:ref:`Subject  <observer_subject>`.

The user can also call these ``..._create()`` functions at any time from application code.



Usage from XML
**************


Registration
------------

Once a Component is created (e.g., ``my_button``), it can be registered by calling either:

- ``lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml");``
- ``lv_xml_component_register_from_data("my_button", xml_data_of_my_button);``

These registration functions process the XML data and store relevant information internally.
This is required to make LVGL recognize the Component by name.

Note that the "A:" in the above path is a file system "driver identifier letter" from
:ref:`file_system` and used accordingly.  See that documentation for details.

When loaded from a file, the file name is used as the Component name.


Instantiation
-------------

After registration, a new instance of any registered Component can be created with:

.. code-block:: c

    lv_obj_t * obj = lv_xml_create(lv_screen_active(), "my_button", NULL);

The created Widget is a normal LVGL Widget that can be used like any other manually-created Widget.

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

These parameters can be set to a fixed value.  However, with the help of ``<prop>``
elements inside the ``<api>`` tag, they can also be passed when an instance is
created.  Only the whole property can be passed, but not individual ``<param>``
elements.

Additionally, when a Component is created, it can use the extended Widget's attributes
(see ``<view extends="...">`` in the code examples below).

This means that Components inherit the API of the extended Widget as well.

The following example demonstrates parameter passing and the use of the
``text`` label property on a Component:

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

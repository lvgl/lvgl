.. _xml_components:

==========
Components
==========

Overview
********

Components are one of the main building blocks for creating new UI elements.

``<component>``\ s support the following child XML tags:

- :ref:`<consts> <xml_consts>`
- :ref:`<api> <xml_api>`
- :ref:`<styles> <xml_styles>`, and
- :ref:`<view> <xml_view>`
- :ref:`<previews> <xml_preview>`

Although they can't contain C code, they are very powerful:

- They can extend another Component or Widget (the base can be defined)
- Components can be built from Widgets and other Components
- A custom API can be defined
- Local styles can be defined, and the global styles can be used
- Local constants can be defined, and the global constants can be used
- Function calls, subject changes, or screen load/create events can be added. See :ref:`XML Events <xml_events>`
- Previews can be defined to preview the components in various settings

Unlike Widgets (which are always compiled into the application), Components can either:

1. be loaded at runtime from XML, or
2. be exported to C code and compiled with the application.

Usage from XML
**************

In XML Files
------------

Using Components in XMLs is very intuitive. The name of the components can be used as XML tag
in the ``<view>`` of other Components, Screens, and Widgets.

The Component properties are just XML attributes.

To load Components from file, it's assumed that the XML files are saved to the device
either as data (byte array) or as file. Once the data is saved, each component
can be registered, and instances can be created after that.

.. code-block:: xml

    <!-- my_button.xml -->
    <component>
        <view extends="lv_button" flex_flow="row">
            <lv_image src="logo"/>
            <my_h3 text="Title"/>
        </view>
    </component>

:ref:`Styles <xml_styles>`, :ref:`Constants <xml_consts>`, and :ref:`custom API <component_custom_api>`
can also be described in the XML files.

Registration
------------

Once a Component is created (e.g., ``my_button``), it can be registered by calling either:

- :cpp:expr:`lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml")`
- :cpp:expr:`lv_xml_component_register_from_data("my_button", xml_data_of_my_button)`

These registration functions process the XML data and store relevant information internally.
This is required to make LVGL recognize the Component by name.

Note that the "A:" in the above path is a file system "driver identifier letter" from
:ref:`file_system` and used accordingly. See that documentation for details.

When loaded from a file, the file name is used as the Component name.

During registration, the ``<view>`` of the Component is saved in RAM.

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
        NULL, NULL,
    };

    lv_obj_t * btn1 = lv_xml_create(lv_screen_active(), "my_button", my_button_attrs);

Usage from Exported Code
************************

From each Component XML file, a C and H file is exported with a single function inside:

.. code-block:: c

    lv_obj_t * component_name_create(lv_obj_t * parent, ...api properties...);

where 'component_name' (in the function above) is replaced by the Component's XML
file name.

When a Component is used in another Component's XML code and the code is exported,
this ``create`` function will be called. This means that Components do not have a
detailed set/get API but can be created with a fixed set of parameters.

If the user needs to access or modify values dynamically, it is recommended to use a
:ref:`Data bindings via Subject  <xml_subjects>`.

The user can also call these ``..._create()`` functions at any time from application code
to create new components on demand.

Extending
*********

Additionally, when a Component is created, it can use the extended Widget's attributes
(see ``<view extends="...">`` in the code examples below).

This means that Components inherit the API of the extended Widget as well.

.. _component_custom_api:

Custom Properties
*****************

The properties of child elements can be adjusted, such as:

.. code-block:: xml

    <my_button x="10" width="200"/>

However, it's also possible to define custom properties in the ``<api>`` tag.
The properties then can be passed to any properties of the children by
referencing them by ``$``. For example:

.. code-block:: xml

    <!-- my_button.xml -->
    <component>
        <api>
            <prop name="btn_text" type="string"/>
        </api>

        <view extends="lv_button">
            <lv_label text="$btn_text"/>
        </view>
    </component>

And it can be used like

.. code-block:: xml

    <!-- my_list.xml -->
    <component>
        <view>
            <my_button btn_text="First"/>
            <my_button btn_text="Second"/>
            <my_button btn_text="Third"/>
        </view>
    </component>

In this setup, the ``btn_text`` property is mandatory, however it can be made optional
by setting a default value:

.. code-block:: xml

    <prop name="btn_text" type="string" default="Title"/>

See :ref:`<api> <xml_api>` for more details and :ref:`XML Syntax <xml_syntax>` for all the supported types.

Examples
********

The following example demonstrates parameter passing and the use of the
``text`` label property on a Component. Styles and Constants are also shown.

.. code-block:: xml

    <!-- h3.xml -->
    <component>
        <view extends="lv_label" style_text_color="0xffff00"/>
    </component>

.. code-block:: xml

    <!-- red_button.xml -->
    <component>
        <api>
            <prop type="string" name="btn_text" default="None"/>
        </api>

        <consts>
            <int name="thin" value="2"/>
        </consts>

        <styles>
            <style name="pressed_style" border_width="#thin" border_color="0xff0000"/>
        </styles>

        <view extends="lv_button" style_radius="0" style_bg_color="0xff0000">
            <style name="pressed_style" selector="pressed"/>

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

Live Example
*************

.. include:: ../../../../../../examples/others/xml/index.rst

API
***

.. _xml:

====================
XML - Declarative UI
====================

Introduction
************

LVGL is capable of loading UI elements written in XML.
Although still under development, the basics are already functional, serving as a preview.

This declarative language serves as the backend for LVGL's UI editor (currently under development),
which enables faster and more maintainable UI implementation.

Note that, the UI editor is not required to utilize LVGL's XML loading capabilities.

Describing the UI in XML in a declarative manner offers several advantages:

- XML files can be loaded at runtime (e.g., from an SD card) to change the application build.
- XML is simpler to write than C, enabling people with different skill sets to create LVGL UIs.
- XML is textual data, making it easy to parse and manipulate with scripts.
- XML can be used to generate LVGL code in any language.
- XML helps to separate the view from the logic.

Currently supported features:

- Load XML components at runtime from file or data
- Nest components and widgets any deep
- Dynamically create instances of XML components in C
- Register images and font that can be accessed by name later in the XMLs
- Constants are working for widget and style properties
- Parameters can be defined and passed and used for components
- Basic built in widgets (label, slider, bar, button, etc)
- Style sheets and :ref:`local styles <style_local>` that can be assigned to parts
  and states support the basic style properties

Limitations:

- Only basic widgets are supported with limited functionality.
- Only a few style properties are supported.
- Events are not supported yet.
- Animations are not supported yet.
- Subjects are not supported yet.
- The documentation is not complete yet.

Main Concept
------------

It's important to distinguish between widgets and components:

Widgets are the core building blocks of the UI and are not meant to be loaded at runtime
but rather compiled into the application. The main characteristics of widgets are:

- Similar to LVGL's built-in widgets.
- Built from classes.
- Have a large API with set/get/add/etc. functions.
- Support "internal widgets" (e.g., tabview's tabs, dropdown's list).
- Have custom and complex logic inside.
- Cannot be loaded from XML at runtime because custom code cannot be loaded.

Components are built from other components and widgets and can be loaded at runtime.
The main characteristics of components are:

- Built from widgets or other components.
- Can be used for styling widgets.
- Can contain widgets or other components.
- Cannot have custom C code.
- Can be loaded from XML at runtime as they describe only the visuals.



Components
**********

Overview
--------

In light of the above, only components can be loaded from XML.
An example of a ``my_button`` component looks like this:

.. code-block:: xml

    <component>
        <consts>
            <px name="size" value="100"/>
            <color name="orange" value="0xffa020"/>
        </consts>

        <api>
            <prop name="btn_text" default="Apply" type="string"/>
        </api>

        <styles>
            <style name="blue" bg_color="0x0000ff" radius="2"/>
            <style name="red" bg_color="0xff0000"/>
        </styles>

        <view extends="lv_button" width="#size" styles="blue red:pressed">
            <my_h3 text="$btn_text" align="center" color="#orange" style_text_color:checked="0x00ff00"/>
        </view>
    </component>

- ``<component>``: The root element.
- ``<consts>``: Constants with ``int``, ``px``, ``string``, ``color``, or ``style`` types.
  Constants can later be referenced as ``#name``.
- ``<params>``: Parameters with ``int``, ``px``, ``string``, ``color``, or ``style`` types.
  Parameters can later be referenced as ``$name``.
- ``<styles>``: ``<style>`` properties can be defined with names and properties.
- ``<view>``: Describes how the component looks. Can reference constants, parameters, and styles.

Naming conventions:

- A standard XML syntax is used.
- Lowercase letters with ``_`` separation are used for attribute names.
- The usual variable name rules apply for attribute and tag names: only letters, numbers, `'_'` and can't start with a number.
- LVGL API is followed as much as possible, e.g., ``align="center"``, ``bg_color="0xff0000"``.
- ``params`` can be referenced with ``$``
- ``consts`` can be referenced with ``#``
- ``styles`` can be attached to states and/or parts like ``styles="red blue:pressed green:focused:scrollbar"``
- Local styles can be used like  ``<lv_label style_text_color="0xff0000" style_text_color:checked="0x00ff00"``

Usage
-----

Once a component is created (e.g., ``my_button``), it can be registered by calling either:

- ``lv_xml_component_register_from_file("A:lvgl/examples/others/xml/my_button.xml");``
- ``lv_xml_component_register_from_data("my_button", xml_data_of_my_button);``

These registration functions process the XML data and save some relevant data internally.
This is required to make LVGL recognize the components by name.
When loaded from a file, the file name is used as the component name.

After this, a new instance of any of the registered components can be created with:
``lv_obj_t * obj = lv_xml_create(lv_screen_active(), "my_button", NULL);``

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
----------

It is possible to pass parameters to child components and widgets.
These parameters can be set on a parent widget or provided by the user.

Additionally, it's possible to use the extended widget's attributes
(see ``<view extends="...">``) when a widget or component is created.
This means that components and widgets inherit the API of the extended widget
as well.

The following example demonstrates parameter chaining and the use of the
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
            <h3 text="$btn_text"/>
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



Widgets
*******

Overview
--------

Widgets are written in C and compiled into the application.
They can be referenced from components, and their API can be used via the exposed attributes
(e.g., label text or slider value).

Usage
-----

To make the widgets accessible from XML, an XML parser needs to be registered for each widget.
The XML parser for the slider looks like this:

.. code-block:: c

    void * lv_xml_label_create(lv_xml_parser_state_t * state, const char ** attrs)
    {
        /* Create the label */
        void * item = lv_label_create(lv_xml_state_get_parent(state));
        return item;
    }

    void lv_xml_label_apply(lv_xml_parser_state_t * state, const char ** attrs)
    {
        void * item = lv_xml_state_get_item(state);

        /*Apply the common properties, e.g. width, height, styles flags etc*/
        lv_xml_obj_apply(state, attrs);

        /* Apply the common properties, e.g., width, height, styles, flags, etc. */
        lv_obj_xml_apply_attrs(state, item, attrs);

        /* Process the label-specific attributes */
        for(int i = 0; attrs[i]; i += 2) {
            const char * name = attrs[i];
            const char * value = attrs[i + 1];

            if(lv_streq("text", name)) lv_label_set_text(item, value);
            if(lv_streq("long_mode", name)) lv_label_set_long_mode(item, long_mode_text_to_enum_value(value));
        }
    }

    /* Helper to convert the string to enum values */
    static lv_label_long_mode_t long_mode_text_to_enum_value(const char * txt)
    {
        if(lv_streq("wrap", txt)) return LV_LABEL_LONG_WRAP;
        if(lv_streq("scroll", txt)) return LV_LABEL_LONG_SCROLL;

        LV_LOG_WARN("%s is an unknown value for label's long_mode", txt);
        return 0; /* Return 0 in the absence of a better option. */
    }

A widget XML process can be registered like
:cpp:expr:`lv_xml_widget_register("lv_label", lv_xml_label_create, lv_xml_label_apply)`

After this, a widget can be created like this:

.. code-block:: c

    const char * attrs[] = {
        "text", "Click here",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(lv_screen_active(), "lv_label", attrs);

LVGL automatically registers its built-in widgets,
so only custom widgets need to be registered manually.



Images and Fonts
****************

In an XML file, images and fonts can be referenced via a name like this:
``<lv_image src="image1" style_text_font="font1"/>``

The font and image names must be mapped to the actual resources in the following way:

.. code-block:: c

    lv_xml_register_image("image1", "path/to/logo.png");
    lv_xml_register_image("image2", &some_image_dsc);

    lv_xml_register_font("font1", &arial_14);

The built-in fonts are automatically registered with names like
 ``"lv_montserrat_16"``.

The registration functions should be called after
:cpp:expr:`lv_init()` but before :cpp:expr:`lv_xml_create(...)`.



Example
*******

.. include:: ../../examples/others/xml/index.rst



.. _xml_api:

API
***

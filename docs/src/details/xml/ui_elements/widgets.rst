.. include:: /include/substitutions.txt
.. _xml_widgets:

=======
Widgets
=======

Overview
********

Besides Components and Screens, Widgets are the other main building blocks of UIs.

The XML file of a Widget is wrapped in a ``<widget>`` XML root element.

``<widget>``\s support the following child XML tags:

- :ref:`<consts> <xml_consts>`
- :ref:`<api> <xml_api>`
- :ref:`<styles> <xml_styles>`
- :ref:`<view> <xml_view>`
- :ref:`<previews> <xml_preview>`

Just like Components, Widgets can also be children of other Widgets and Components.

Note that Widgets cannot be loaded from XML directly, but it's possible to write and register
simple XML parsers for Widgets. When a Widget is referenced in a Component's or Screen's XML,
the XML parser will be called to map the properties to C function calls.

LVGL already provides all the helper functions and required libraries. There are also
`many XML parser examples here <https://github.com/lvgl/lvgl/tree/master/src/others/xml/parsers>`__
for the built-in LVGL Widgets.

Built-in Widgets
****************

The built-in LVGL widgets (e.g., :ref:`lv_slider`, :ref:`lv_label`, :ref:`lv_chart`, etc.) already
have XML parsers and are therefore available in XML.

For example:

.. code-block:: xml

    <component>
        <view>
            <lv_label x="10" text="Hello"/>
        </view>
    </component>

The built-in widgets consist of:

- Pure C code
  (e.g., `lv_slider.c <https://github.com/lvgl/lvgl/tree/master/src/widgets/slider/lv_slider.c>`__)
- An XML file to define only the API
  (e.g., `lv_slider.xml <https://github.com/lvgl/lvgl/blob/master/xmls/lv_slider.xml>`__),
  used only in the UI |nbsp| Editor to validate and autocomplete properties
- An XML parser C file to map the XML attributes to C functions
  (e.g., `lv_xml_slider_parser.c <https://github.com/lvgl/lvgl/blob/master/src/others/xml/parsers/lv_xml_slider_parser.c>`__)

XML Parser
**********

Write a parser
--------------

To make Widgets accessible from XML, an XML parser needs to be created and
registered for each Widget. The XML parser for the label Widget looks like this:

.. code-block:: c

    void * lv_xml_label_create(lv_xml_parser_state_t * state, const char ** attrs)
    {
        /* Create the label */
        void * obj = lv_label_create(lv_xml_state_get_parent(state));
        return obj;
    }

    void lv_xml_label_apply(lv_xml_parser_state_t * state, const char ** attrs)
    {
        void * obj = lv_xml_state_get_item(state);

        /* Apply the common properties, e.g., width, height, styles, flags, etc. */
        lv_xml_obj_apply(state, attrs);

        /* Process the label-specific attributes */
        for(int i = 0; attrs[i]; i += 2) {
            const char * name = attrs[i];
            const char * value = attrs[i + 1];

            if(lv_streq("text", name)) lv_label_set_text(obj, value);
            if(lv_streq("long_mode", name)) lv_label_set_long_mode(obj, long_mode_text_to_enum(value));
            /* Process more props here ... */
        }
    }

    /* Helper to convert strings to enum values */
    static lv_label_long_mode_t long_mode_text_to_enum(const char * txt)
    {
        if(lv_streq("wrap", txt)) return LV_LABEL_LONG_MODE_WRAP;
        if(lv_streq("scroll", txt)) return LV_LABEL_LONG_MODE_SCROLL;

        LV_LOG_WARN("%s is an unknown value for label's long_mode", txt);
        return 0; /* Return 0 in the absence of a better option. */
    }

By using lines like ``if(lv_streq("text", name)) lv_label_set_text(obj, value);``,
any ``set`` function can be mapped to XML properties.

Register a widget
-----------------

A Widget XML processor can be registered as follows:

.. code-block:: c

    lv_xml_register_widget("lv_label", lv_xml_label_create, lv_xml_label_apply);

After registration, a Widget can be created like this from C code:

.. code-block:: c

    const char * attrs[] = {
        "text", "Click here",
        "align", "center",
        NULL, NULL,
    };

    lv_obj_t * label = lv_xml_create(lv_screen_active(), "lv_label", attrs);

And in XML, it can be used like this:

.. code-block:: xml

    <view>
        <lv_label width="100" text="I'm a label!" wrap="scroll"/>
    </view>

Usage in LVGL's UI Editor
*************************

New widget
----------

It's possible to create new widgets by writing C code manually (the same way as built-in LVGL widgets are created),
however, using the UI |nbsp| Editor is much faster and simpler.

When an XML file is created and the ``<widget>`` root element is used, the following .C/.H files are generated automatically:

:<widget_name>_gen.h:           Contains the generated API implementation of the Widget
                                (overwritten on each code export)
:<widget_name>_private_gen.h:   Contains private API and data for the Widget
                                (overwritten on each code export)
:<widget_name>_gen.c:           Contains the internals of the Widget, e.g., constructor with children,
                                destructor, event handler, etc. (overwritten on each code export)
:<widget_name>.h:               Includes ``<widget_name>_gen.h`` and allows the user to
                                define custom APIs. Only a skeleton is exported once
:<widget_name>.c:               Contains hooks from ``<widget_name>_gen.c`` and allows
                                the user to write custom code. Only a skeleton is
                                exported once
:<widget_name>_xml_parser.c:    Processes the XML strings and calls the required
                                functions according to the set attributes. Only a
                                skeleton is exported once

Adding Custom Code
------------------

``<widget_name>.c`` contains three hooks:

- **Constructor hook**: Called when the Widget and all its children are created.
  Any modifications can be done on the children here.
- **Destructor hook**: Called when the Widget is deleted.
  All manually allocated memory needs to be freed here.
- **Event hook**: Called at the beginning of the Widget's event callback to perform
  any custom action.

In this C file, the ``set`` functions for each API ``<prop>`` also need to be
implemented. The declaration of these functions is automatically exported in
``<widget_name>_gen.h``.

Besides these, any custom code and functions can be freely implemented in this file.

Elements
--------

Elements are internal parts of the Widget that can be accessed and/or created dynamically.
For example: tabs of a tabview, list of a dropdown, series of a chart, etc.

Just like any other Widget API properties, Elements can also be defined in the ``<api>``
tag of the Widget's XML.

Learn more about Elements in the documentation page for :ref:`<api> <xml_widget_element>`.


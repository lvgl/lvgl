.. _xml_widgets:

=======
Widgets
=======

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

Besides components and Screens, Widgets are the other main building blocks of UIs.
XML files defining Widgets have ``<widget>`` as their XML root element.

``<widget>`` elements may contain the following child elements:

- :ref:`<consts> <xml_consts>`
- :ref:`<api> <xml_api>`
- :ref:`<styles> <xml_styles>`, and
- :ref:`<view> <xml_view>`
- :ref:`<previews> <xml_preview>`

Just like Components, Widgets can also be the children of other Widgets and Components.

The main difference is that Widgets are written in C and compiled into the
application.  This means, unlike Components, Widgets can contain custom C code.  For
example, when a property is set, any complex C code can run to set that value.

To connect the C code to XML, XML parser functions need to be implemented and registered. It is
pretty simple since LVGL already provides all the helper functions and the required libraries. Also there
are `many XML parser examples here <https://github.com/lvgl/lvgl/tree/master/src/others/xml/parsers>`__.



Built-in Widgets
****************

LVGL already has XML parsers for LVGL's built-in widgets (e.g. :ref:`lv_slider`,
:ref:`lv_label`, :ref:`lv_chart`, etc.), which makes them available in XML.

Example:

.. code-block:: xml

    <view>
        <lv_label x="10" text="Hello"/>
    </view>

The built-in widgets are:

- pure C code
  (e.g. `lv_slider.c <https://github.com/lvgl/lvgl/tree/master/src/widgets/slider/lv_slider.c>`__)
- an XML file to define only the API
  (e.g. `lv_slider.xml <https://github.com/lvgl/lvgl/blob/master/xmls/lv_slider.xml>`__)
  (used only in the UI |nbsp| Editor to validate and autocomplete properties)
- an XML parser C file to map the XML attributes to C functions.
  (e.g. `lv_xml_slider_parser.c <https://github.com/lvgl/lvgl/blob/master/src/others/xml/parsers/lv_xml_slider_parser.c>`__)



Creating New Widgets
********************

It is possible to create new Widgets that can be treated in the same way as built-in
LVGL Widgets.

However, using the UI |nbsp| Editor it's much faster and simpler. When an XML file is created and
the ``<widget>`` root element is used, the following .C/.H files are generated automatically:

:<widget_name>_gen.h:           Contains the generated API implementation of the widget
                                (overwritten on each code export).
:<widget_name>_private_gen.h:   Contains private API and the data for the widget
                                (overwritten on each code export).
:<widget_name>_gen.c:           Contains the internals of the Widget, e.g. constructor with the children,
                                destructors, event handler, etc. (overwritten on each code export).
:<widget_name>.h:               Includes ``<widget_name>_gen.h`` and allows the user to
                                define custom APIs. Only a skeleton is exported once.
:<widget_name>.c:               Contains hooks from ``<widget_name>_gen.c`` and allows
                                the user to write custom code. Only a skeleton is
                                exported once.
:<widget_name>_xml_parser.c:    Processes the XML strings and calls the required
                                functions according to the set attributes. Only a
                                skeleton is exported once.



Usage
*****

XML Parser
----------

To make the Widgets accessible from XML, an XML parser needs to be created and
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
            /* Process more props here */
        }
    }

    /* Helper to convert strings to enum values */
    static lv_label_long_mode_t long_mode_text_to_enum(const char * txt)
    {
        if(lv_streq("wrap", txt)) return LV_LABEL_LONG_WRAP;
        if(lv_streq("scroll", txt)) return LV_LABEL_LONG_SCROLL;

        LV_LOG_WARN("%s is an unknown value for label's long_mode", txt);
        return 0; /* Return 0 in the absence of a better option. */
    }

A Widget XML processor can be registered like this:

.. code-block:: c

    lv_xml_widget_register("lv_label", lv_xml_label_create, lv_xml_label_apply);

After registration, a Widget can be created like this from C code:

.. code-block:: c

    const char * attrs[] = {
        "text", "Click here",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(lv_screen_active(), "lv_label", attrs);

And in XML it can be used like this:

.. code-block:: xml

    <view>
        <lv_label width="100" text="I'm a label!" wrap="scroll"/>
    </view>


Adding Custom Code
------------------

``<widget_name>.c`` contains three hooks:

- **Constructor hook**: Called when the Widget and all its children are created. Any
  modifications to the children can be done here.
- **Destructor hook**: Called when the Widget is deleted. All manually allocated
  memory needs to be freed here.
- **Event hook**: Called at the beginning of the Widget's event callback to perform
  any custom actions.

In this C file, the ``set`` functions for each API ``<prop>`` also need to be
implemented. The declaration of these functions is automatically exported in
``<widget_name>_gen.h``.

Besides these, any custom code and functions can be freely implemented in this file.


Elements
--------

Elements are internal parts of the widget that can be accessed and/or created dynamically.
For example, tabs of a tabview, list of a dropdown, series of a chart, etc..

Just like any other Widget API properties, Elements also can be defined within the
``<api>`` element of the Widget's XML.

Learn more about Elements under the :ref:`<api> element documentation <xml_widget_element>`.



API
***

.. API startswith:  lv_xml_widget_

.. API equals:  lv_xml_create
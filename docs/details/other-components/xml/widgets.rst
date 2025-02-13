.. _xml_widgets:

=======
Widgets
=======

Overview
********

Widgets are written in C and compiled into the application.
They can be referenced from components, and their API can be used via the exposed attributes
(e.g., label text or slider value).

Using the UI Editor, all the following C/H files can be exported from the XML of the widgets:

- ``<widget_name>_gen.h``: Contains the generated API implementation of the widget. Overwritten on each code export.
- ``<widget_name>_private_gen.h``: Contains private API and the data for the widget. Overwritten on each code export.
- ``<widget_name>_gen.c``: Contains the internals of the widget. Overwritten on each code export.
- ``<widget_name>.h``: Includes ``<widget_name>_gen.h`` and allows the user to define custom APIs. Only a skeleton is exported once.
- ``<widget_name>.c``: Contains hooks from ``<widget_name>_gen.c`` and allows the user to write custom code. Only a skeleton is exported once.
- ``<widget_name>_xml_parser.c``: Processes the XML strings and calls the required functions according to the set attributes. Only a skeleton is exported once.

Usage
*****

XML Parser
----------

To make the widgets accessible from XML, an XML parser needs to be created and registered for each widget.
The XML parser for the label widget looks like this:

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
        }
    }

    /* Helper to convert the string to enum values */
    static lv_label_long_mode_t long_mode_text_to_enum(const char * txt)
    {
        if(lv_streq("wrap", txt)) return LV_LABEL_LONG_WRAP;
        if(lv_streq("scroll", txt)) return LV_LABEL_LONG_SCROLL;

        LV_LOG_WARN("%s is an unknown value for label's long_mode", txt);
        return 0; /* Return 0 in the absence of a better option. */
    }

A widget XML processor can be registered like this:

.. code-block:: c

    lv_xml_widget_register("lv_label", lv_xml_label_create, lv_xml_label_apply);

After registration, a widget can be created like this:

.. code-block:: c

    const char * attrs[] = {
        "text", "Click here",
        "align", "center",
        NULL, NULL,
    };

    lv_xml_create(lv_screen_active(), "lv_label", attrs);

LVGL automatically registers its built-in widgets,
so only custom widgets need to be registered manually.

Adding Custom Code
------------------

``<widget_name>.c`` contains three hooks:

- **Constructor hook**: Called when the widget and all its children are created. Any modifications can be done on the children here.
- **Destructor hook**: Called when the widget is deleted. All manually allocated memory needs to be freed here.
- **Event hook**: Called at the beginning of the widget's event callback to perform any custom action.

In this C file, the ``set`` functions for each API ``<prop>`` also need to be implemented. The declaration of these functions is
automatically exported in ``<widget_name>_gen.h``.

Besides these, any custom code and functions can be freely implemented in this file.

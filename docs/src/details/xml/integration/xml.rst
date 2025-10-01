.. _editor_integration_xml:

============================
Loading XML Files at Runtime
============================

Overview
********

LVGL is capable of loading XML files at runtime without recompiling the application.

The XMLs can be sent to the device via Bluetooth, WiFi, Serial port, loaded from an SD
card, downloaded from a server, or anything else.

The XML files need to be registered so that LVGL can parse their content. It's like
saving the blueprint of the components.

After that, instances of the registered Components and Screens can be created.

Note that :ref:`Widgets in XML <xml_widgets>` cannot be loaded at runtime, as widgets
are built using C code.



Registering XML Files
*********************


Registering Manually
--------------------

Use :cpp:expr:`lv_xml_register_component_from_data(name, xml_string)` and
:cpp:expr:`lv_xml_register_component_from_file("A:path/to/my.xml")` to register the
XML files defining Components, Screens, and ``globals.xml`` either as strings or file paths.

As a result, LVGL will know all the ``<view>``\ s, ``<style>``\ s, ``<const>``\ s, etc., i.e.,
the full content of these XML files needed to create instances later.

Similarly, translations can be registered by
:cpp:expr:`lv_xml_register_translation_from_data()` and
:cpp:expr:`lv_xml_register_translation_from_file()`.

Fonts and Images are registered automatically when ``globals.xml`` is registered.


Batch Registration from a Folder
--------------------------------

Instead of calling the register functions one by one, it's also possible to load many
XML files and assets at once.

:cpp:expr:`lv_xml_load_all_from_path("A:path/to/dir")` will traverse a directory and
register all XML Components, Screens, globals, and translations.


Registering from a Blob
-----------------------

There are some additional XML loading functions available when using :ref:`frogfs`.

With the help of FrogFS, a "blob" (binary file) can be created from a folder containing
all XMLs, images, and fonts. Having a single blob is easier to manage than many smaller
files.

If the blob is saved in memory mapped to addressable memory (flash, RAM, etc.), use
:cpp:expr:`lv_xml_load_all_from_data`:

.. code-block:: c

    extern const unsigned char my_blob[];
    extern unsigned int my_blob_len;
    lv_xml_load_t * handle = lv_xml_load_all_from_data(my_blob, my_blob_len);
    if(handle == NULL) {
        LV_LOG_USER("An error occurred while loading XML content from data");
    }
    /* `handle` can optionally be passed to `lv_xml_unload` later */

If the blob is saved as a file (e.g., on an SD card), use
:cpp:expr:`lv_xml_load_all_from_file`:

.. code-block:: c

    lv_xml_load_t * handle = lv_xml_load_all_from_file("A:path/to/frogfs.bin");
    if(handle == NULL) {
        LV_LOG_USER("An error occurred while loading XML content from a file");
    }
    /* `handle` can optionally be passed to `lv_xml_unload` later */


Registering External Data
-------------------------

The only thing that cannot be learned from the XML files is the data stored in the
application's flash. This includes images and fonts stored in memory, subjects and
constants provided by the application, and most importantly, event callbacks.

To connect this data to the XML world, LVGL provides many ``lv_xml_register_...()``
functions to register:

- events
- constants
- timeline animations
- subjects
- images
- fonts

For example, use
:cpp:expr:`lv_xml_register_event_cb(scope, "event_cb_name", the_callback)` to connect
a callback to a name. After that, the registered data will be available by name in the
application.

``scope`` is usually ``NULL`` to register assets globally.
:cpp:expr:`lv_xml_component_get_scope(component_name)` returns a pointer to a "scope
descriptor" that can be used to register data available only for a given component.

Besides events, it's common to register images and fonts stored in the application code
by :cpp:expr:`lv_xml_register_image(scope, "image_name", path_or_pointer)` and
:cpp:expr:`lv_xml_register_font(scope, "font_name", path_or_pointer)`.


Registering Widgets
-------------------

Although widgets are compiled into the application, they need to be registered so the
XML parser knows how to create an element like ``<lv_slider>`` when it appears in XML.

Use :cpp:expr:`lv_xml_register_widget("widget_name", create_cb, apply_cb)` for that.

To learn more about Widgets in XML and the callbacks refer to :ref:`xml_widgets`.

Note that the built-in widgets of LVGL are registered automatically.



Creating Instances
******************


Creating Screens
----------------

By default, no Widget or Screen is created. The user needs to create the
:ref:`Permanent Screens <xml_screen_permanent>` and any other required Screens to
get started.

Use :cpp:expr:`lv_xml_create_screen("name")`, where ``"name"`` is the name of the XML
file or the name used when the XML data was registered.

It returns an ``lv_obj_t *`` that can be loaded as any regular :ref:`Screen <screens>` using
:cpp:expr:`lv_screen_load()`.


Creating Components
-------------------

Use :cpp:expr:`lv_xml_create(parent, "name", attributes)` to create any Widget,
Component, or Screen at runtime from the registered XMLs.

``attributes`` is an array of property name-value pairs, terminated by ``NULL, NULL``.

For example, to create a widget:

.. code-block:: c

    const char * attrs[] = {
        "width", "100",
        "value", "35",
        NULL, NULL
    };

    lv_obj_t * slider_1 = lv_xml_create(lv_screen_active(), "lv_slider", attrs);

Or a custom component:

.. code-block:: c

    const char * attrs[] = {
        "width", "100",
        "button_label", "Hello!",
        "color", "0xff0000",
        NULL, NULL
    };

    lv_obj_t * my_button_1 = lv_xml_create(lv_screen_active(), "my_button", attrs);

Or to create non-``lv_obj_t`` children:

.. code-block:: c

    const char * attrs[] = {
        "color", "0xff0000",
        "axis", "primary_y",
        NULL, NULL
    };

    lv_chart_series_t * ser_1 = lv_xml_create(chart1, "lv_chart-series", attrs);

Or to add styles:

.. code-block:: c

    const char * attrs[] = {
        "name", "style1",
        "selector", "knob|pressed",
        NULL, NULL
    };

    lv_xml_create(button1, "style", attrs);



The Whole Flow
***************

To load everything at runtime correctly, the steps need to be executed in this order:

1. **Register custom widgets**. These are independent of XML, but XML components rely
   on them. Built-in widgets are registered automatically.
2. Register events, fonts, images, etc., that are compiled into the code so they are
   available for the Components and Screens registered later.
3. Register ``globals.xml``\ s and those defining Components and Screens.
4. Create the :ref:`Permanent Screens <xml_screen_permanent>` and other required screens.
5. Load the start screen.

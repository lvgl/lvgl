.. _xml_overview:

========
Overview
========


.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

The LVGL XML Module implements LVGL's Declarative UI by making it possible to
describe UIs in XML.

Describing the UI in XML in a declarative manner offers several advantages:

- XML files can be loaded at runtime (e.g., from an SD card), allowing you to change
  the application's UI without changing the firmware.
- XML files can also be loaded dynamically, enabling use cases such as creating
  UIs from plugins or apps downloaded by the firmware.
- XML is simpler to write than C, enabling people with different skill sets to create LVGL UIs.
- XML is textual data, making it easy to parse and manipulate with a large number of
  programming and scripting languages.
- XML can be used to generate LVGL code in any language.
- XML helps separate the view from the internal logic.
- The XML syntax uses the same properties as the C API, so it's easy to learn. E.g.,
  style properties like ``bg_color``, ``line_width``, or widget properties like ``width``,
  ``height``, label ``text``, etc.
- A powerful data binding engine allows you to easily bind widgets to global data.

The XML file can be written by hand, but it's highly recommended to use `LVGL's
UI editor  <https://lvgl.io/editor>`__ to write the XML files. This UI editor
makes UI development much faster by providing features like:

- Instant preview of the XML files (components, screens)
- Inspector mode to visualize widget sizes, paddings, etc.
- Generate C code from XML files
- Autocomplete and syntax highlighting
- Online share/preview for collaboration and testing
- `Figma <https://www.figma.com/>`__ integration to easily reimplement Figma designs

.. warning::

    The UI editor and the XML loader are still under development and not
    production-ready. Consider them as open beta or experimental features.

Using the XML files
*******************

If writing XMLs by hand, the concept is very simple:

1. When ``LV_USE_XML`` is enabled, LVGL's built-in widgets and other XML parsers for styles, events,
   data bindings, etc., are registered automatically.
2. The XML files created by the user (such as Screens, Components, Images, Fonts, etc.) can be
   registered (loaded) at runtime, and screen or component instances can also be created based on the
   XML "blueprints".
3. The widgets created from XML look like any normal widgets, so functions of the C API can be applied
   to them. E.g., start an animation, add a special style, etc.

UI Elements
***********

It is important to distinguish between :dfn:`Widgets`, :dfn:`Components`, and :dfn:`Screens`.

Widgets
-------

:dfn:`Widgets` are the core building blocks of the UI and are **not meant to be loaded at runtime**
but rather compiled into the application as C code. The main characteristics of Widgets are:

- In XML, they start with a ``<widget>`` root element.
- They are similar to LVGL's built-in widgets.
- They can contain custom and complex logic.
- They cannot be loaded from XML at runtime because the custom code cannot be loaded.
- They can have a large and custom API with ``set/get/add`` functions.
- A custom XML parser is needed to map XML properties to API function calls.
- They can be compound, containing other Widgets (e.g., ``Tabview``'s tabs, ``Dropdown``'s lists).
- In the UI editor, they can also be described in XML to create visuals more quickly and export C code.

Components
----------

:dfn:`Components` are simpler UI elements and **can be loaded at runtime**.
The main characteristics of Components are:

- In XML, they start with a ``<component>`` root element.
- They are built from Widgets and/or other Components.
- They are defined only in XML and cannot have custom C code (but they can call C functions as event callbacks).
- They can be loaded from XML since they contain only XML.
- They can be used for styling Widgets and other Components, using data bindings via subjects, defining animations, etc.
- As children, they can contain Widgets and/or other Components.
- They can have a simple API to pass properties to their children (e.g., ``btn_text`` to a Label's text).

Whether the XML was written manually or by the UI |nbsp| Editor, the files
defining Components can be registered in LVGL, and after that, instances can be created.

In other words, LVGL can read the XML files, "learn" the Components from them, and
thereafter create children as part of Screens and other Components.

Screens
-------

:dfn:`Screens` are similar to Components:

- In XML, they start with a ``<screen>`` root element.
- They are built from Widgets and/or other Components to describe the :ref:`Screen <screens>`.
- They can be loaded from XML at runtime since they describe only visual aspects of the UI.
- They do not have an API.
- They can be referenced in screen load events.

Global data
***********

``globals.xml`` is a special XML file in which globally available

- styles
- constants
- images
- fonts
- subjects for data bindings

can be defined.

Multiple ``globals.xml`` files can be loaded if needed, but each will be saved in the same global scope,
meaning duplicated items will be added only once.

Usage Teaser
************

Each Component or Screen XML file describes a single UI element.

The syntax and supported XML tags are very similar in all three.

Note that for Widgets, XML can be used to export C code in LVGL's UI Editor.

This is a high-level overview of the most important XML elements that
can be children of these root elements:

:<api>:     Describes the properties that can be ``set`` in a Component.
            Properties can be referenced by ``$``.
:<consts>:  Specifies constants (local to the Widget or Component) for colors, sizes,
            and other values. Constant values can be referenced using ``#``.
:<styles>:  Describes style (``lv_style_t``) objects that can be referenced
            by Widgets and Components later.
:<view>:    Specifies the appearance of the Widget, Component, or Screen by describing the
            children and their properties.

An XML component
----------------

This is a simple example illustrating what an LVGL XML Component looks like.
Note that only the basic features are shown here.

.. code-block:: xml

    <!-- my_button.xml -->
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

        <view extends="lv_button" width="#size">
            <style name="blue"/>
            <style name="red" selector="pressed"/>
            <my_h3 text="$btn_text"
                   color="#orange"
                   align="center"/>
        </view>
    </component>

Load the UI from XML
--------------------

The Component XML can be loaded, and any number of instances can be created at runtime.

In the simplest case, a Component can be registered with
:cpp:expr:`lv_xml_component_register_from_file("A:path/to/my_button.xml")` and an instance can be created with
:cpp:expr:`lv_obj_t * obj = lv_xml_create(parent, "my_button", NULL)`.
:cpp:expr:`lv_xml_load_all_from_path("A:path/to/dir")`
will traverse a directory and register all the XML components,
screens, globals, and translations.

Note that loading the UI from XML has practically no impact on performance.
Once the XML files are registered and the UI is created, it behaves the same way
as if it were created from C code.

Registering XML files and creating instances is not memory-hungry nor slow. The biggest
memory overhead is that the ``<view>`` of the Components is saved in RAM (typically
1–2 kB per component).

Load many XML files and assets
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As mentioned, :cpp:expr:`lv_xml_load_all_from_path("A:path/to/dir")`
will traverse a directory and register all the XML components,
screens, globals, and translations.

There are some additional XML loading functions available when using :ref:`frogfs`.
If you have a frogfs blob you just want to load all the XMLs and assets from,
you can directly load from the blob using :cpp:func:`lv_xml_load_all_from_data`
without registering it with :cpp:func:`lv_fs_frogfs_register_blob` first.

.. code-block:: c

    extern const unsigned char frogfs_bin[];
    extern unsigned int frogfs_bin_len;
    lv_xml_load_t * handle = lv_xml_load_all_from_data(frogfs_bin, frogfs_bin_len);
    /* `handle` can optionally be passed to `lv_xml_unload` later */

There is one more function provided for the special use case when a frogfs blob
is in another filesystem like an SD card.

.. code-block:: c

    lv_xml_load_t * handle = lv_xml_load_all_from_file("A:path/to/frogfs.bin");
    /* `handle` can optionally be passed to `lv_xml_unload` later */

Export C and H Files
--------------------

By using LVGL's UI Editor, the Widgets, Components, Screens, images, fonts, etc., can be
converted to .C/.H files containing plain LVGL code.

The exported code works the same way as if it were written by the user.

In this case, the XML files are not required anymore to run the C code.

The XML files are used only during the editing/implementation of the Widgets, Components, and Screens to save
recompilation time and optionally leverage other useful UI |nbsp| Editor features.

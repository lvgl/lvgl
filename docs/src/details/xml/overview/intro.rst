.. _xml_intro:

============
Introduction
============

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

The LVGL XML Module implements LVGL's Declarative UI by making it possible to
describe UIs in XML

Describing the UI in XML in a declarative manner offers several advantages:

- XML files can be loaded at runtime (e.g. from an SD card) allowing you to change
  the application's UI without changing the firmware.
- XML files can be loaded dynamically as well, which allows for things like creating
  UIs from plugins or apps that are downloaded by the firmware.
- XML is simpler to write than C, enabling people with different skill sets to create LVGL UIs.
- XML is textual data, making it easy to parse and manipulate with a large number of
  programming and scripting languages.
- XML can be used to generate LVGL code in any language.
- XML helps to separate the view from the internal logic.
- The XML syntax uses the same properties as the C API, so it's easy to learn. E.g.
  style properties like ``bg_color``, ``line_width``, or widget properties like ``width``,
  ``height``, label ``text``, etc.
- A powerful data binding engine allows you to easily bind the widgets to global data.


The XML file can be written by hand, but it's highly recommended to use `LVGL's
UI editor  <https://lvgl.io/editor>`__ to write the XML files.  This UI editor
make UI development way faster by provides features like:

- Instant preview of the XML files (components, screens)
- Inspector mode to visalise widgets sizes, paddings, etc
- Generate C code from XML files
- Autocomplete and Syntax highlighting
- Online share/preview for collaboration and testing
- `Figma <https://www.figma.com/>`__ integration to easily reimplement Figma designs

.. warning::

    The UI editor and the XML loader are still under development and not
    production-ready.  Consider them as an open beta or experimental features.


Using the XML files
*******************

If writing XML's by hand the concept is very simple:

1. When ``LV_USE_XML`` is enabled LVGL's built-in widgets and other XML parsers for styles, events,
data bindings etc., will be registered automatically.
2. The XML files created by the user (such as Screens, Components, Images, Fonts, etc) can be
registered (loaded) at  runtime, and screen or component instances can be also created based on the
XML "blueprints".
3. The Widgets created from XML look like any normal widgets, therefore functions of the C API can be applied
on them. E.g. start an animation, add a special style, etc.


UI Elements
***********

It is important to distinguish between :dfn:`Widgets`, :dfn:`Components`, and :dfn:`Screens`.


Widgets
-------

:dfn:`Widgets` are the core building blocks of the UI and are **not meant to be loaded at runtime**
but rather compiled into the application as C code.  The main characteristics of Widgets are:

- In XML, they start with a ``<widget>`` root element.
- They are similar to LVGL's built-in Widgets.
- They can have custom and complex logic inside.
- They cannot be loaded from XML at runtime because the custom code cannot be loaded.
- They can have a large and custom API with ``set/get/add`` functions.
- A custom XML parser is needed to map XML properties to the API function calls of the widget
- They can be compaund containing other Widgets (e.g. ``Tabview``'s tabs, ``Dropdown``'s lists).
- In the UI editor they can be also described in XML to the create visuals quicker and export C code.

Components
----------

:dfn:`Components` are simpler UI elements and **can be loaded at runtime**.
The main characteristics of Components are:

- In XML, they start with a ``<component>`` root element.
- They are built from Widgets and/or other Components.
- They are built in XML only and cannot have custom C code (but they can call C functions as event callbacks).
- They can be loaded from XML as they don't contain custom C code, only XML.
- They can be used for styling Widgets and other Components, use databindings via subjects, define animations, etc.
- As children they can contain Widgets and/or other Components.
- They can have a simple API to pass properties to their children (e.g. ``btn_text`` to a Label's text).

Regardless of whether the XML was written manually or by the UI |nbsp| editor, the XML files
defining Components can be registered in LVGL, and after that, instances can be created.

In other words, LVGL can just read the XML files, "learn" the Components from them, and
thereafter create children as part of Screens and other Components.


Screens
-------

:dfn:`Screens` are similar to Components:

- In XML, they start with a ``<screen>`` root element.
- They are built from Widgets and/or other Components to describe the :ref:`Screen <screens>`.
- They can be loaded from XML at runtime as they describe only visual aspects of the UI.
- They do not have an API.
- They can be referenced in Screen load events.


Global data
***********

``globals.xml`` is a special XML file in which globally available

- styles
- constants
- images
- fonts
- subjects for data bindings

can be defined.

Multiple ``globlas.xml`` can be loaded  if needed but each will be saved in the same global scope,
meaning duplicated items will be added only once.

Usage Teaser
************

Each Component, or Screen XML file describes a single UI element.

The syntax and supported XML tags are very similar in all three.

Note that, for Widgets XML can be used to export C code in LVGL's UI Editor.

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

The Components'XML can be loaded and any number of instances can be created at runtime.

In the simplest case, a Component can be registered with
:cpp:expr:`lv_xml_component_register_from_file(A:path/to/my_button.xml)` and an instance can be created with
:cpp:expr:`lv_obj_t * obj = lv_xml_create(parent, "my_button", NULL)`.

Note that loading the UI from XML practically has no impact on performance.
Once the XML files are registered and the UI is created, it behaves the same way
as if it were created from C code.

Registering XMLs and creating instances is not memory hungry nor slow. The biggest
memory overhead is that the ``<view>`` of the Components is saved in RAM (typically
1–2 kB/component).

Export C and H Files
--------------------

By using LVGL's UI Editor the Widgets, Components, Screens, images, fonts, etc., can be
converted to .C/.H files having plain LVGL code.

The exported code works the same way as if it was written by the user.

In this case, the XML files are not required anymore to run the C code.

The XML files were used only during editing/implementing the Widgets, Components, and Screens to save
recompilation time and optionally leverage other useful UI |nbsp| Editor features.


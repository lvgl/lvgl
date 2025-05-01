.. _xml_intro:

============
Introduction
============

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

LVGL is capable of loading UI elements written in XML. The XML file can be written by hand, but
it's highly recommended to use LVGL's UI |nbsp| Editor to write the XML files. The UI |nbsp| Editor provides
features like:

- Instant preview the XML files
- Autocomplete and Syntax highlighting
- Online preview for collaboration and testing
- `Figma <https://www.figma.com/>`__ integration to easily reimplement the designs

.. warning::

    The UI |nbsp| Editor and the XML loader are still under development and not production ready.
    Consider them as an open beta, or experimental features.

Describing the UI in XML in a declarative manner offers several advantages:

- XML files can be loaded at runtime (e.g., from an SD card) to change the application build.
- XML is simpler to write than C, enabling people with different skill sets to create LVGL UIs.
- XML is textual data, making it easy to parse and manipulate with a large number of programming and scripting languages.
- XML can be used to generate LVGL code in any language.
- XML helps to separate the view from the internal logic.

Currently supported features:

- Load XML Components at runtime from file or any type of input
- Nest Components and Widgets to any depth
- Dynamically create instances of XML Components in C
- Register images and font that can be accessed by name later in the XML data (only from
  file, no C file is generated for images and fonts)
- Constants are working for Widget- and style-properties
- Parameters can be defined, passed and used for Components
- Most of the built-in Widgets are supported, even the complex ones (``label``, ``slider``,
  ``bar``, ``button``, ``chart``, ``scale``, ``button matrix``, ``table``, etc.)
- Style sheets and local styles can be assigned to parts and states supporting
  almost all style properties

Limitations:

- Screens are not supported yet (only Components)
- Events are not supported yet.
- Animations are not supported yet.
- Observer pattern Subjects are not supported yet.
- The documentation is not complete yet.



Concept
*******

The XML files are Component-oriented. To be more specific, they are ``Component-Library`` oriented.
That is, they are structured in a way to make it easy to create reusable Component Libraries.

For example, a company can have a Component Library for the basic Widgets for all its products
(smart home, smart watch, smart oven, etc.), and create other industry-specific Libraries
(smart-home specific, smart-watch specific, etc.) containing only a few extra Widgets.

These Component Libraries are independent, can be reused across many products, and
can be freely versioned and managed.

Imagine a Component Library as a collection of XML files, images, fonts, and other
assets stored in a git repository, which can be a submodule in many projects.

If someone finds a bug in the Component Library, they can just fix it in just one
place and push it back to the git repository so that other projects can be updated
from it.

The built-in Widgets of LVGL are also considered a ``Component Library`` which is
always available.

A UI |nbsp| Editor project can have any number of Component Libraries and will always have
at least 2:

- LVGL's built-in Widgets, and
- XML-based definitions of Screen contents, along with other project-specific Components.



Widgets, Components, and Screens
********************************

It is important to distinguish between :dfn:`Widgets` and :dfn:`Components`.


Widgets
-------

:dfn:`Widgets` are the core building blocks of the UI and are not meant to be loaded at runtime
but rather compiled into the application. The main characteristics of Widgets are:

- In XML, they start with a ``<widget>`` root element.
- They are similar to LVGL's built-in Widgets.
- They are built using ``lv_obj_class`` objects.
- They have custom and complex logic inside.
- They cannot be loaded from XML at runtime because the custom code cannot be loaded.
- They have a large API with ``set/get/add`` functions.
- They can themselves contain Widgets as children (e.g., ``Tabview``'s tabs, ``Dropdown``'s lists).

Any handwritten Widget can be accessed from XML by:

1. Defining its API in an XML file.
2. Writing and registering an XML parser for it.
   `See some examples here. <https://github.com/lvgl/lvgl/tree/master/src/others/xml/parsers>`__


Components
----------

:dfn:`Components` are built from other Components and Widgets, and can be loaded at runtime.
The main characteristics of Components are:

- In XML, they start with a ``<component>`` root element.
- They are built in XML only and cannot have custom C code.
- They can be loaded from XML at runtime as they describe only visual aspects of the UI.
- They are built from Widgets or other Components.
- They can be used for styling Widgets.
- They can contain (as children) Widgets or other Components.
- They can have a simple API to pass properties to their children (e.g., ``btn_text`` to a Label's text).

Regardless of whether the XML was written manually or by the UI |nbsp| Editor, the XML files
of Components can be registered in LVGL, and after that, instances can be created.
In other words, LVGL can just read the XML files, "learn" the Components from them, and
thereafter create Components as part of a :ref:`Screen's <screens>` :ref:`Widget Tree
<basic_data_flow>` according to their structure.

:dfn:`Screens` are similar to Components:

- In XML, they start with a ``<screen>`` root element.
- They are built from Widgets or other Components to describe the :ref:`Screen <screens>`.
- They can be loaded from XML at runtime as they describe only visual aspects of the UI.
- They do not have an API.
- They can be referenced in Screen load events.



Syntax Teaser
*************

Each Widget or Component XML file describes a single Widget or Component.  The root
element for Widgets, Components, and Screens are ``<widget>``, ``<component>`` and
``<screen>`` respectively.  Other than that, the contained XML elements are almost
identical.  This is a high-level overview of the most important XML elements that
will be children of these root elements:

:<api>:     Describes the properties that can be ``set`` for a Widget or Component.
            Properties can be referenced ysubg ``$``.  For Widgets, custom enums can
            also be defined with the ``<enumdef>`` tag.
:<consts>:  Specifies constants (local to the Widget or Component) for colors, sizes,
            and other values.  Constant values can be referenced using ``#``.
:<styles>:  Describes style (``lv_style_t``) objects that can be referenced (and
            shared) by Widgets and Components later.
:<view>:    Specifies the appearance of the Widget or Component by describing the
            children and their properties.

This is a simple example illustrating what an LVGL XML Component looks like.
Note that only the basic features are shown here.

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



Usage Teaser
************

LVGL's UI |nbsp| Editor can be used in two different ways.


Export C and H Files
--------------------

The Widgets, Components, images, fonts, etc., can be converted to .C/.H files with
plain LVGL code.  The exported code works the same way as if it was written by the
user.  In this case, the XML files are not required anymore unless modifications may
be made later.  The XML files were used only during editing/implementing the Widgets
and Components to save recompilation time and optionally leverage other useful
UI |nbsp| Editor features.


Load the UI from XML
--------------------

Although the Widgets' code always needs to be exported in C and compiled into the
application (just like the built-in LVGL Widgets are also part of the application), the Components'
XML can be loaded and any number of instances can be created at runtime. In the simplest case,
a Component can be registered with :cpp:expr:`lv_xml_component_register_from_file(path)` and
an instance can be created with :cpp:expr:`lv_obj_t * obj = lv_xml_create(parent, "my_button", NULL)`.

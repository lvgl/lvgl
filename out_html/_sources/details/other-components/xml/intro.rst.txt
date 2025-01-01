.. _xml_intro:

============
Introduction
============

Overview
********

LVGL is capable of loading UI elements written in XML. The XML file can be written by hand, but
it's highly recommended to use LVGL's UI Editor to write the XML files. The UI Editor provides
features like:

- Instant preview the XML files
- Autocomplete and Syntax highlight
- Online preview for collaboration and testing
- Figma integration to easily reimplement the designs

.. warning::

	The UI Editor and the XML loader are still under development and not production ready.
	Consider them as an open beta, or experimental features.

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
- Register images and font that can be accessed by name later in the XMLs (only from file, no C file is generated for image and fonts)
- Constants are working for widget and style properties
- Parameters can be defined and passed and used for components
- Most of the built-in widgets, even the complex ones (``label``, ``slider``, ``bar``, ``button``, ``chart``, ``scale``, ``button matrix``, ``table``, etc.)
- Style sheets and local styles that can be assigned to parts and states supporting almost all style properties

Limitations:

- Screens are not supported yet (only components)
- Events are not supported yet.
- Animations are not supported yet.
- Subjects are not supported yet.
- The documentation is not complete yet.

Concept
*******

The XML files are component-oriented. To be more specific, they are ``component library`` oriented.
That is, they are structured in a way to make it easy to create reusable component libraries.

For example, a company can have a component library for the basic widgets for all its products
(smart home, smart watch, smart oven, etc.), and create other industry-specific libraries
(smart home-specific, smart watch-specific, etc.) containing only a few extra widgets.

These component libraries are independent, can be reused across many products, and can be freely versioned and managed.

You can imagine a component library as a collection of XML files, images, fonts, and other assets
stored in a git repository, which can be a submodule in many projects.

If someone finds a bug in the component library, they can just fix it and push it back to the git
repository so that other projects can pull it.

The built-in widgets of LVGL are also considered a ``component library`` which is always available.


Widgets, Components, and Screens
********************************

It's important to distinguish between ``widgets`` and ``components``:

**Widgets** are the core building blocks of the UI and are not meant to be loaded at runtime
but rather compiled into the application. The main characteristics of widgets are:

- In XML, they start with a ``<widget>`` root element.
- Similar to LVGL's built-in widgets.
- Built from ``lv_obj_class``-es.
- Have custom and complex logic inside.
- Cannot be loaded from XML at runtime because the custom code cannot be loaded.
- Have a large API with ``set/get/add`` functions.
- Support "internal widgets" (e.g., ``tabview``'s tabs, ``dropdown``'s list).

Any handwritten widget can be accessed from XML by:

1. Defining its API in an XML file.
2. Writing and registering an XML parser for it. See some examples here.

**Components** are built from other components and widgets and can be loaded at runtime.
The main characteristics of components are:

- In XML, they start with a ``<component>`` root element.
- Built in XML only and cannot have custom C code.
- Can be loaded from XML at runtime as they describe only the visuals.
- Built from widgets or other components.
- Can be used for styling widgets.
- Can contain widgets or other components.
- Can have a simple API to pass properties to the children (e.g., ``btn_text`` to label's text).

Regardless of whether the XML was written manually or by the UI Editor, the XMLs of the components can be registered in LVGL, and after that, instances can be created.
In other words, LVGL can just read the XML files, "learn" the components from them, so that it can create components accordingly.

**Screens** are similar to components:

- In XML, they start with a ``<screen>`` root element.
- Built from widgets or other components to describe the screen.
- Can be loaded from XML at runtime as they describe only the visuals.
- Do not have an API.
- Can be referenced in screen load events.

Syntax teaser
*************

Each widget or component XML file describes a single widget or component.
The root element for widgets, components, and screens are ``<widget>``, ``<component>``, and ``<screen>`` respectively.
Other than that, the other XML elements inside are almost identical.
This is the high-level overview of the most important XML tags inside these root elements:

- ``<api>``: Describes the properties that can be ``set`` for a widget or component.
  Properties can be referenced by ``$``.
  For widgets, custom enums can also be defined with the ``<enumdef>`` tag.
- ``<consts>``: Specifies constants (local to the widget or component) for colors, sizes, and other values.
  Constant values can be referenced by ``#``.
- ``<styles>``: Describes styles (``lv_style_t``) that can be referenced by widgets and components later.
- ``<view>``: Specifies the appearance of the widget or component by describing the
  children and their properties.

This is a simple example illustrating how an LVGL XML component looks like.
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

Usage teaser
************

LVGL's UI Editor can be used in two different ways.

Export C and H files
--------------------

The widgets, components, images, fonts, etc., can be converted to C/H files
with plain LVGL code. The exported code works the same way as if it was written by the user.
In this case, the XML files are not required anymore. The XML files were used only during
editing/implementing the widgets and components to save recompilation time and
optionally leverage other useful Editor features.

Load the UI from XML
--------------------

Although the widgets' code always needs to be exported in C and compiled into the
application (just like the built-in LVGL widgets are also part of the application), the components'
XML can be loaded and any number of instances can be created at runtime. In the simplest case,
a component can be registered with ``lv_xml_component_register_from_file(path)`` and
an instance can be created with ``lv_obj_t * obj = lv_xml_create(parent, "my_button", NULL);``.

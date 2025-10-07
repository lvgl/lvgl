.. _xml_overview:

========
Overview
========


.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

Introduction
************

LVGL's XML Module implements LVGL's Declarative UI by making it possible to describe
UIs in XML.

Using XML instead of pure drag-and-drop editing has several advantages:

- **Familiar syntax** works like HTML, easy to learn and read
- **Version control friendly** plain text with human-readable diffs, no binary files
- **Easy to share** copy, paste, and send as text
- **Reusable patterns** copy and reuse snippets across projects
- **Automation ready** scripts and CI/CD can process XML effortlessly
- **AI compatible** AI tools can read, generate, and refactor XML
- **Modular by design** create reusable components for structured UIs
- **Fast to edit** quicker to type than to drag and drop with a mouse
- **Runtime loading** parse XML at runtime without recompiling
- **Cross-platform** the same XML works across all LVGL targets

The XML file can be written by hand, but it's highly recommended to use `LVGL Pro's
Editor <editor>` to write the XML files. The Editor makes UI development much faster by
providing features like:

- Instant preview of the XML files (components, screens)
- Inspector mode to visualize widget sizes, paddings, etc.
- Generate C code from XML files
- Autocomplete and syntax highlighting
- Online share/preview for collaboration and testing
- `Figma <https://www.figma.com/>`__ integration to easily reimplement Figma designs

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


Using the XML Files
*******************

The XML files can be used in two ways:

1. Load them directly at runtime
2. Convert them to C code by using the Editor or the CLI

Both use cases are covered in detail in the :ref:`Integration <editor_integration>`
section.



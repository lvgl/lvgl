.. _xml_component_library:

=================
Component Library
=================

Overview
********

The collection of Components, Widgets, Screens, Images, Fonts, etc., is called a component library.

A component library can be fully self-sufficient, but it can also reference data from other component libraries.

LVGL itself is a component library that supplies the built-in widgets, types, etc., so typically, component libraries use
at least the core LVGL data. You can find the XML files that describe the LVGL widgets `here <https://github.com/lvgl/lvgl/tree/master/xmls>`_.

A project is also a component library, where the screens, components, and widgets of the project are implemented.

In light of that, a project can contain multiple component libraries (its own project component library, LVGL, and possibly others).

Structure
*********

A typical structure for a component library looks like this:

.. code-block:: none

  name_of_the_component_library
  ├── globals.xml
  ├── components
  │   ├── component1.xml
  │   ├── component2.xml
  │   └── other_folder
  │       ├── component3.xml
  │       └── component4.xml
  ├── widgets
  │   ├── widget1
  │   │   ├── widget1.xml
  │   │   ├── widget1.c
  │   │   ├── widget1.h
  │   │   ├── widget1_gen.c
  │   │   ├── widget1_gen.h
  │   │   ├── widget1_private_gen.h
  │   │   └── widget1_xml_parser.c
  │   └── widget2
  │       └── ...same as widget1...
  ├── screens
  │   ├── screen1.xml
  │   └── screen2.xml
  ├── fonts
  │   ├── font1.ttf
  │   └── font2.ttf
  └── images
      ├── image1.png
      └── image2.png

Visibility
**********

The content of all ``globals.xml`` files is part of a common global scope, and
any components, widgets, or screens can use data from there.

Styles, constants, and other data defined in the XML file of components, widgets, or screens
are local to the given file.

In this sense, there are two namespaces:

1. **Local namespace** in the given XML file of components, widgets, and screens.
2. **Global namespace** created from the data in all ``globals.xml`` files.

The referenced names are always checked first in the local namespace.
If not found there, the global namespace is also checked.

The defined components, widgets, and screens are part of the global namespace, meaning
there cannot be two ``mybutton`` components.

All data belonging to the LVGL core component library is prefixed by ``lv_``
(e.g., ``lv_label``, ``lv_font_default``).

A custom component can be prefixed with ``watch_``, ``small_``, ``light_``, or
anything else that the developer finds appropriate.

LVGL's UI editor will show an error if there is a naming conflict.


``globals.xml``
***************

A ``globals.xml`` file should be created in each component library.
The definitions in it do not belong to any specific widget but are available throughout the entire UI, widgets, and all XML files.
The valid tags in it are:

- ``<config>``: Can specify name and help.
- ``<api>``: Only for ``<enumdefs>``.
- ``<subjects>``: List of subjects. Can be considered the API of a component library.
- ``<consts>``: Globally available constants.
- ``<styles>``: Globally available styles.
- ``<fonts>``: Globally available fonts.
- ``<images>``: Globally available images.
- ``<const_variants>``: See below.
- ``<style_variants>``: See below.

``globals.xml`` files cannot be nested, meaning that there cannot be another ``globals.xml`` file in a subfolder.

From each ``globals.xml`` file, an ``<config.name>.h`` file is generated,
which is included in all generated header files — not only in the subfolders where ``globals.xml`` is created, but in all exported C and H files.
This ensures that constants, fonts, and other global data are available for all widgets and new widgets.

Variants
--------

``<const_variant>`` can be used by constants to create variants that can be selected at compile time.
This is useful for selecting a different display size, color scheme, etc.

``<style_variant>`` can be used by styles only, to modify styles at runtime.
To select the current style variant, an integer subject ``<style_variant.name>_variant`` is created.
Styles can subscribe to this, and the style properties can be changed according to the
selected variant.

In ``globals.xml``, the possible variants should be described.

Example
-------

A ``globals.xml`` file of a component library can look like this:

.. code-block:: xml

  <globals>
    <config name="mylib" help="This is my great component library"/>
    <const_variants>
      <const_variant name="size" help="Select the size">
        <case name="small" help="Assets for 320x240 screen"/>
        <case name="large" help="Assets for 1280x768 screen"/>
      </const_variant>
    </const_variants>

    <style_variants>
      <style_variant name="color" help="Select the color of the UI">
        <case name="red" help="Select a red theme"/>
        <case name="blue" help="Select a blue theme"/>
      </style_variant>
    </style_variants>

    <api>
      <enumdef name="mode">
        <enum name="slow"/>
        <enum name="fast"/>
      </enumdef>
    </api>

    <consts>
      <px name="small_unit" value="8"/>
      <px name="large_unit" value="16"/>
    </consts>

    <styles>
      <style name="card" bg_color="0xeee" radius="#small_unit" padding="12px"/>
    </styles>

    <images>
      <file name="arrow_left" src="A:/images/arrow_left.png"/>
    </images>

    <fonts>
      <tiny_ttf name="big" src="A:/fonts/arial.ttf" size="28"/>
    </fonts>
  </globals>

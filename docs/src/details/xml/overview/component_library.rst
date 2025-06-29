.. _xml_component_library:

=================
Component Library
=================

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

The collection of Components, Widgets, Screens, Images, Fonts, etc., is called a
Component Library.

A Component Library can be fully self-sufficient, but it can also reference data from
other Component Libraries.

LVGL itself is a Component Library that supplies the built-in Widgets, data types,
etc. You can
find the XML files that describe the LVGL Widgets
`here <https://github.com/lvgl/lvgl/tree/master/xmls>`__.

A project always has at least 2 Component Libraries:  that of LVGL as mentioned
above, and its own where the Screens, Components, and Widgets of the project are
defined.  A project may include additional Component Libraries.


Structure
*********

A typical structure for a Component Library looks like this:

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
any Components, Widgets or Screens defined therein can be used in all .XML files.

Styles, constants, and other data defined in the XML file of Components, Widgets, or Screens
are local to that XML file.

Thus, there are two namespaces:

1. **local namespace** within the given XML file of Components, Widgets and Screens.
2. **global namespace** created from the data in the ``globals.xml`` file from each
   Component Library included.

To find names referenced in XML files, the local namespace is checked first, and if
a name is not found there, then the global namespace is checked.

The names of defined Components, Widgets and Screens become part of the global
namespace and must be unique therein.  This ensures that each Component has a unique
name.

All data belonging to the LVGL core Component Library is prefixed by ``lv_``
(e.g., ``lv_label``, ``lv_font_default``).

A custom Component can be prefixed with ``watch_``, ``small_``, ``light_``, or
anything else the developer deems appropriate.

LVGL's UI |nbsp| Editor will show an error if there is a name conflict.



``globals.xml``
***************

A single ``globals.xml`` file should be created in the root directory of each
Component Library.  The definitions in it do not belong to any specific Widget but
are available throughout the entire UI, Widgets, and all XML files.  The valid tags
in it are:

:<config>:          Can specify name and help.
:<api>:             Used with ``<enumdefs>`` to show possible values for Widget or Component attributes.
:<subjects>:        List of :ref:`Subjects <observer_subject>`. Can be considered the API of a Component Library.
:<consts>:          Globally available constants.
:<styles>:          Globally available styles.
:<fonts>:           Globally available fonts.
:<images>:          Globally available images.
:<const_variants>:  See below.
:<style_variants>:  See below.

The ``globals.xml`` must be the only ``globals.xml`` file within the Component
Library's folder tree.

From each ``globals.xml`` file, a ``<config.name>.h`` file is generated, which is
included by all generated header files --- not only in the sub-folders where
``globals.xml`` is created, but in all exported .C and .H files.  This ensures that
constants, fonts, and other global data are available for all Widgets and new Widgets.


Example
-------

A ``globals.xml`` file of a Component Library might look like this:

.. code-block:: xml

    <globals>
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
            <style name="style_red" bg_color="0xff0000">
        </styles>

        <images>
            <file name="arrow_left" src="A:/images/arrow_left.png"/>
        </images>

        <fonts>
            <tiny_ttf name="big" src="A:/fonts/arial.ttf" size="28"/>
        </fonts>
    </globals>

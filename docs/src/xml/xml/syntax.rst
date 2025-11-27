.. include:: /include/substitutions.txt
.. _xml_syntax:

======
Syntax
======

Overview
********

The XML engine uses standard XML syntax; however, there are some
special conventions for property names and values.

Naming Conventions
******************

- A standard XML syntax is used.
- Lowercase letters with ``_`` separation are used for attribute names.
- Tag names follow the usual variable-name rules: they must start with a letter or
  ``'_'`` and the rest of the name may be comprised of letters, ``'_'`` and digits.
- ``-`` is used to create compound names, e.g. ``lv_chart-series``
- The LVGL API is followed as much as possible, e.g., ``align="center"``, ``bg_color="0xff0000"``.
- For colors, all these syntaxes are supported (similar to CSS colors): ``0x112233``,
  ``#112233``, ``112233``, ``0x123``, ``#123``, ``123``.  Note that like CSS,
  ``0x123``, ``#123`` and ``123`` all mean ``#112233``.
- ``params`` can be referenced with ``$``.
- ``consts`` can be referenced with ``#``.


Property Names
**************

In most of the cases XML property names don't have special syntax.
for example:

.. code-block:: xml

    <some_tag prop="value" other_prop="other value"/>

Local styles also support adding "selectors" to property names:

.. code-block:: xml

    <lv_checkbox style_bg_color-indicator-checked-disabled="0xff0000"/>

Types
*****

All of the types can be used as API property types, but only a subset of them can be
used as constant and :ref:`Subject <observer_subject>` types.


Simple types
------------

The following simple built-in types are supported:

:bool:      a ``true`` or ``false``.

:int:       Integer number in the range of roughly -2B to 2B by default.
            (Same as ``int32_t`` in C.)

:px:        Simple pixel units.  The unit ``px`` can be omitted.

:%:         Percentage.  ``%`` must be appended to the value as the unit.
            (Means the same as :cpp:expr:`lv_pct()`.)

:content:   Means ``LV_SIZE_CONTENT``.

:string:    Simple ``\0``-terminated string.  When multiple strings are used in a
            property or string array, ``'`` should be used.  E.g. ``foo="'a' 'b'"``.

:color:     A color stored as 24-bit RGB (:cpp:expr:`lv_color_t`).

:opa:       Opacity value in the range of 0 to 255 or 0 to 100%.  Like CSS,
            percentage values must be be followed by '%'.

:lv_obj:    Pointer to a Widget (:cpp:expr:`lv_obj_t *`).

:screen:    Pointer to a screen (also :cpp:expr:`lv_obj_t *`).

:time_ms:   Means some time in milliseconds unit

:deg_0.1:   Degrees with 0.1 resolution

:scale_1/256:   Scale/Zoom, where 256 means 100%, 128 means 50%, 512 means 200% etc.

:style_prop:    A style property, for example ``"bg_opa"`` meaning :cpp:enumerator:`LV_STYLE_BG_OPA`.

Name-based types
----------------

In XML files, fonts, images, styles, etc., are not used by pointer but by string
names.  For example, a style is defined like ``<style name="red" bg_color="0xff0000"/>``.
Later, they can be referenced by their names.

This means that the actual values need to be bound to the names when the UI is loaded
from XML, otherwise, LVGL will not know what a name means.

Most of these connections are done automatically (e.g., for styles, fonts, images,
animations, gradients, etc.) when the components and ``globals.xml``\ s
are registered.

However, others need to be connected manually. For example the event callbacks where
the callback itself is available only in the code, or fonts or images if they are
referring to data stored in memory.

To learn more about how to register the name-data pairs visit :ref:`editor_integration_xml`.

Arrays
------

An array of any type can be defined in three ways:

:int[count]:        An integer array. The number of elements will be passed as a separate parameter.
:string[NULL]:      An array terminated with a ``NULL`` element.
:string[]:          No ``NULL`` termination and no count parameter. Used when the
                    number of elements is not known or delivered via another
                    mechanism, such as via another setting.

Enums
-----

``<enumdef>`` can be used in the ``<api>`` tags to create custom enums for
**Widgets**.  This is not supported for Components.

For example:

.. code-block:: xml

    <api>
        <enumdef name="my_widget_mode" help="Possible modes" help-zh="Chinese help">
            <enum name="normal" help="Normal mode" help-zh="Normal mode in Chinese" value="0x10"/>
            <enum name="inverted" help="Inverted mode"/>
        </enumdef>

        <prop name="mode" help="help"type="enum:my_widget_mode" help="help"/>
    </api>

When used as a type, a ``+`` suffix means multiple values can be selected and ORed.
For example: ``type="axis+"``.  In this case, the options should be separated by
``|``, for example: ``axis=primary_x|secondary_y``.


Compound types
--------------

Types can be compound, meaning multiple options/types are possible.  For example, for
width: ``type="px|%|content"``.

Limiting Accepted Values
************************

**Not supported yet; the examples below illustrate the planned syntax.**

It is also possible to limit the possible options the user can select from an enum.
For example:

- Enums: ``type="dir(top bottom)"``
- Colors: ``type="color(0xff0000 0x00ff00 0x0000ff)"``
- Strings: ``type="string('Ok' 'Cancel')``


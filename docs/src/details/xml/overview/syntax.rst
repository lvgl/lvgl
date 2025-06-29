.. _xml_syntax:

======
Syntax
======

Naming conventions
******************

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

- A standard XML syntax is used.
- Lowercase letters with ``_`` separation are used for attribute names.
- Tag names follow the usual variable-name rules: they must start with a letter or
  ``'_'`` and the rest of the name may be comprised of letters, ``'_'`` and digits.
- The LVGL API is followed as much as possible, e.g., ``align="center"``, ``bg_color="0xff0000"``.
- For colors, all these syntaxes are supported (similar to CSS colors): ``0x112233``,
  ``#112233``, ``112233``, ``0x123``, ``#123``, ``123``.  Note that like CSS,
  ``0x123``, ``#123`` and ``123`` all mean ``#112233``.
- ``params`` can be referenced with ``$``.
- ``consts`` can be referenced with ``#``.
- ``styles`` can be attached to states and/or parts like this:
  ``styles="style1 style2:pressed style3:focused:scrollbar"``.
- Local styles (i.e. styles that are stored within the Component and thus not shared
  by any other Components) can be used like this:
  ``<lv_label style_text_color="0xff0000" style_text_color:checked="0x00ff00"/>``.



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

:string:    Simple NUL-terminated string.  When multiple strings are used in a
            property or string array, ``'`` should be used.  E.g. ``foo="'a' 'b'"``.

:color:     A color stored as 24-bit RGB (:cpp:expr:`lv_color_t`).

:opa:       Opacity value in the range of 0 to 255 or 0 to 100%.  Like CSS,
            percentage values must be be followed by '%'.

:lv_obj:    Pointer to a Widget (:cpp:expr:`lv_obj_t *`).

:screen:    Pointer to a screen (also :cpp:expr:`lv_obj_t *`).

:time_ms:   Means some time in milliseconds unit

:deg_0.1:   Degrees with 0.1 resolution

:scale_1/256:   Scale/Zoom, where 256 means 100%, 128 means 50%, 512 means 200% etc.

Name-based types
----------------

In XML files, fonts, images, styles, etc., are not used by pointer but by string
names.  For example, a style is defined like ``<style name="red" bg_color="0xff0000"/>``.
Later, they can be referenced by their names.

This means that the actual values need to be bound to the names when the UI is loaded
from XML, otherwise, LVGL wouldn't know what a name means.

Most of these connections are done automatically (e.g., for styles, fonts, images,
animations, gradients, etc.), but others need to be connected manually (e.g., event
callbacks where the callback itself is available only in the code).

For fonts and images, the connections are created automatically if the source is a file.
If the font or image is compiled into the application (as a C array), the user needs
to specify which variable a given name refers to.

To create these connections, functions like

- ``lv_xml_register_image(scope, name, pointer)``
- ``lv_xml_register_font(scope, name, pointer)``
- ``lv_xml_register_event_cb(scope, name, callback)``
- etc.

can be used.  Later, a pointer to the object can be retrieved by

- ``lv_xml_get_image(scope, name)``
- ``lv_xml_get_font(scope, name)``
- ``lv_xml_get_event_cb(scope, name)``
- etc.

``scope`` can be ``NULL`` to use the global scope or :cpp:expr:`lv_xml_component_get_scope("my_component")`
returns the a component's local scope.

:style:     Name of a style. :cpp:expr:`lv_xml_get_style_by_name(&ctx, name)` returns an :cpp:expr:`lv_style_t *`.
:font:      Name of a font. :cpp:expr:`lv_xml_get_font(&ctx, name)` returns an :cpp:expr:`lv_font_t *`.
:image:     Name of an image. :cpp:expr:`lv_xml_get_image(&ctx, name)` returns an :cpp:expr:`const void *`,
            which can be :cpp:expr:`lv_image_dsc_t *` or a NUL-terminated string path to a file.
:animation: Name of an animation descriptor. :cpp:expr:`lv_xml_get_anim(&ctx, name)` returns an :cpp:expr:`lv_anim_t *`.
:subject:   Name of a :ref:`Subject <observer_subject>`. :cpp:expr:`lv_xml_get_subject(&ctx, name)` returns an :cpp:expr:`lv_subject_t *`.
:grad:      Name of a gradient. :cpp:expr:`lv_xml_get_grad(&ctx, name)` returns an :cpp:expr:`lv_grad_dsc_t *`.
:event_cb:  Name of an event callback. :cpp:expr:`lv_xml_get_event_cb(&ctx, name)` returns an :cpp:expr:`lv_event_cb_t`.
:screen_create_cb:  In XML it's the name of a screen XML file. In exported code it's a function like ``lv_obj_t * my_screen_create(void)``

Arrays
------

An array of any type can be defined in four ways:

:int[N]:            An integer array with ``N`` elements.
                    In the exported code ``N`` is passed a parameter after the array.
:string[...NULL]:   An array terminated with a ``NULL`` element. ``NULL`` can be
                    replaced by any value.
:string[5]:         An array that must have exactly 5 elements. In the exported code only the array will be passed
                    as the Widget assumes the given number of elements.
:string[]:          No ``NULL`` termination and no count parameter, used when the
                    number of elements is not known or delivered via another
                    mechanism, such as via a function parameter.

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


Limiting accepted values
------------------------

It is also possible to limit the possible options the user can select from an enum.
For example:

- Enums: ``type="dir(top bottom)"``
- Colors: ``type="color(0xff0000 0x00ff00 0x0000ff)"``
- Strings: ``type="string('Ok' 'Cancel')``

Limiting accepted values is not supported yet, however in the UI |nbsp| Editor if
an invalid option is selected, it will be highlighted as an error.

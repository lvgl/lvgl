.. _xml:

======
Syntax
======

Naming conventions
******************

- A standard XML syntax is used.
- Lowercase letters with ``_`` separation are used for attribute names.
- The usual variable name rules apply for attribute and tag names: only letters, numbers, ``'_'`` and cannot start with a number.
- LVGL API is followed as much as possible, e.g., ``align="center"``, ``bg_color="0xff0000"``.
- For colors, all these options are supported: ``0x112233``, ``#112233``, ``112233``, ``0x123``, ``#123``, ``123``.
- ``params`` can be referenced with ``$``.
- ``consts`` can be referenced with ``#``.
- ``styles`` can be attached to states and/or parts like ``styles="style1 style2:pressed style3:focused:scrollbar"``.
- Local styles can be used like ``<lv_label style_text_color="0xff0000" style_text_color:checked="0x00ff00"/>``.

Types
*****

All of the types can be used as API property types, but only a subset of them can be used as constant and subject types.

Simple types
------------

The following simple built-in types are supported:

- ``bool``: ``true`` or ``false``.
- ``int``: Integer number in the range of -2M to 2M by default. (``int32_t`` in C)
- ``px``: Simple pixel unit. The unit ``px`` can be omitted.
- ``%``: Percentage. ``%`` must be appended as a unit. (Means :cpp:expr:`lv_pct()`)
- ``content``: Means ``LV_SIZE_CONTENT``.
- ``string``: Simple ``\0`` terminated string. When multiple strings are used in a property or string array, ``'`` should be used. E.g. ``foo="'a' 'b'"``.
- ``color``: A color stored as 24-bit RGB. (:cpp:expr:`lv_color_t`)
- ``opa``: Opacity value in the range of 0 to 255 or 0 to 100%.
- ``lv_obj``: Pointer to a widget (:cpp:expr:`lv_obj_t *`).
- ``point``: A point with ``x`` and ``y`` values. (:cpp:expr:`lv_point_t`)
- ``arglist``: Just list all the parameters as arguments. Supports only ``int`` and ``string``. E.g. ``foo="1 23 'hello' 'a'"``.

Name-based types
----------------

In XML files, fonts, images, styles, etc., are not used by pointer but by string names. For example, a style is defined like
``<style name="red" bg_color="0xff0000"/>``. Later, these can be referenced by their names.

This means that the actual values need to be bound to the names when the UI is loaded from XML,
otherwise, LVGL wouldn't know what a name means.

Most of these connections are done automatically (e.g., for styles, fonts, images, animations, gradients, etc.),
but others need to be connected manually (e.g., event callbacks where the callback itself is available only in the code).

For fonts and images, the connections are created automatically if the source is a file.
If the font or image is compiled into the application (as a C array), the user needs to specify which
variable a given name refers to.

To create these connections, functions like ``lv_xml_register_image/font/event_cb/etc(name, pointer)`` can be used.
Later, the set pointer can be retrieved by ``lv_xml_get_image/font/event_cb(name)``.

- ``style``: The name of a style. :cpp:expr:`lv_xml_get_style(name)` returns an :cpp:expr:`lv_style_t *`.
- ``font``: The name of a font. :cpp:expr:`lv_xml_get_font(name)` returns an :cpp:expr:`lv_font_t *`.
- ``image``: The name of an image. :cpp:expr:`lv_xml_get_image(name)` returns an :cpp:expr:`const void *`, which can be :cpp:expr:`lv_image_dsc_t *` or a path to a file.
- ``animation``: The name of an animation descriptor. :cpp:expr:`lv_xml_get_anim(name)` returns an :cpp:expr:`lv_anim_t *`.
- ``subject``: The name of a subject. :cpp:expr:`lv_xml_get_subject(name)` returns an :cpp:expr:`lv_subject_t *`.
- ``grad``: The name of a gradient. :cpp:expr:`lv_xml_get_grad(name)` returns an :cpp:expr:`lv_grad_dsc_t *`.
- ``event_cb``: The name of an event callback. :cpp:expr:`lv_xml_get_event_cb(name)` returns an :cpp:expr:`lv_event_cb_t`.

Arrays
------

Any type can be an array in four ways:
- ``int[N]``: An integer array with ``N`` elements, and the count is passed as the next parameter.
- ``string[...NULL]``: An array terminated with ``NULL``. ``NULL`` can be replaced by any value, e.g., ``grid_template_last``.
- ``string[5]``: An array that must have exactly 5 elements.
- ``string[]``: No ``NULL`` termination and no count parameter.

Enums
-----

``<enumdef>`` can be used in the ``<api>`` tags to create custom enums for **widgets**. It is not supported for components.

For example:

.. code-block:: xml

    <api>
        <enumdef name="my_widget_mode" help="Possible modes" help-zh="Chinese help">
            <enum name="normal" help="Normal mode" help-zh="Normal mode in Chinese" value="0x10"/>
            <enum name="inverted" help="Inverted mode"/>
        </enumdef>

        <prop name="mode" help="help">
            <param name="mode" type="enum:my_widget_mode" help="help"/>
        </prop>
    </api>

When used as a type, a ``+`` suffix means multiple values can be selected and ORed. For example: ``type="axis+"``.
In this case, the options should be separated by ``|``, for example: ``axis=primary_x|secondary_y``.

Compound types
--------------

Types can be compound, meaning multiple options/types are possible. For example, for width: ``type="px|%|content"``.

Limit the possible values
-------------------------

It is also possible to limit the possible options the user can select from an enum. For example:

- Enums: ``type="dir(top bottom)"``
- Colors: ``type="color(0xff0000 0x00ff00 0x0000ff)"``
- Strings: ``type="string('Ok' 'Cancel')``

These are checked in the UI Editor, and if an invalid option is selected, it will be highlighted as an error.

.. _obj_property:

=================
Widget Properties
=================

Use of Widget Properties feature of LVGL is entirely optional.  Using it consumes
more program space and more CPU overhead while setting and getting Widget properties,
but may be helpful if you are developing a wrapper layer for other modules like
Micropython, Lua, or for an external animation engine.

Use of the property-name-lookup feature also makes possible creating a UI from an
externally-stored text file without modifying firmware.



What is a Widget Property?
**************************

A Widget's properties are a superset of its styles plus additional properties that
are unique to each type of Widget.  In all cases, properties determine what the
Widget looks like and how it behaves.  Examples:  size, position, color, font, etc.
are properties of a Widget.  A Widget's local styles are also valid properties in
this context.

The properties available for any given Widget that can be used by this feature of
LVGL are:

- the styles of that Widget, as defined in ``lv_style_properties.c`` and
  ``lv_style_properties.h``, as well as

- additional properties unique to that Widget, such as TEXT, LONG_MODE,
  SELECTION_START and SELECTION_END for Label Widgets.

Not every property is set-able and not every property is get-able.  The non-style
Widget properties available for a given Widget are implemented at the top of that
Widget's primary ``.c`` file as a ``const`` id-to-function-pointer lookup
array, like this example for the Label Widget:

.. code:: c

    #if LV_USE_OBJ_PROPERTY
    static const lv_property_ops_t properties[] = {
        {
            .id = LV_PROPERTY_LABEL_TEXT,
            .setter = lv_label_set_text,
            .getter = lv_label_get_text,
        },
        {
            .id = LV_PROPERTY_LABEL_LONG_MODE,
            .setter = lv_label_set_long_mode,
            .getter = lv_label_get_long_mode,
        },
        {
            .id = LV_PROPERTY_LABEL_TEXT_SELECTION_START,
            .setter = lv_label_set_text_selection_start,
            .getter = lv_label_get_text_selection_start,
        },
        {
            .id = LV_PROPERTY_LABEL_TEXT_SELECTION_END,
            .setter = lv_label_set_text_selection_end,
            .getter = lv_label_get_text_selection_end,
        },
    };
    #endif

Looking at this array, you can see which non-style properties are available using
this fature, and whether "setting" and/or "getting" is available for each.  (Function
pointers are provided in the array when that type of operation is available.)



.. _obj_property_usage:

Usage
*****

By default, this feature of LVGL is turned off.  It can be turned on by configuring
:c:macro:`LV_USE_OBJ_PROPERTY` to ``1`` in ``lv_conf.h``.

The 2 functions that then become available (looking up the relevant function pointer
and then calling it) are:

.. code:: c

    v_result_t      lv_obj_set_property(lv_obj_t * widget, const lv_property_t * value);
    lv_property_t   lv_obj_get_property(lv_obj_t * widget, lv_prop_id_t id);


A ``lv_property_t`` is a paired ID and value, and a ``lv_prop_id_t`` is just an ID.

.. code:: c

    lv_result_t     lv_obj_set_properties(lv_obj_t * obj, const lv_property_t * value, uint32_t count);

can be used to set multiple properties where ``value`` will point to an array of
``lv_property_t`` objects defining what is to be set.  The following is an example
of such an array:

.. code-block:: c

    lv_property_t props[] = {
        { .id = LV_PROPERTY_IMAGE_SRC, .ptr = &img_demo_widgets_avatar, },
        { .id = LV_PROPERTY_IMAGE_PIVOT, .ptr = &pivot_50, },
        { .id = LV_PROPERTY_IMAGE_SCALE, .num = 128, },
        { .id = LV_PROPERTY_OBJ_FLAG_CLICKABLE, .num = 1, },
        { .id = LV_STYLE_IMAGE_OPA, .num = 128, },
        { .id = LV_STYLE_BG_COLOR, .color = (lv_color_t){.red = 0x11, .green = 0x22, .blue = 0x33}, },
    }


.. _obj_property_id:

Property ID
-----------

:cpp:type:`lv_prop_id_t` identifies which property to get/set.  It is an enum value
defined in the primary ``.h`` file for the Widget in question.  Because the actual
names are "assembled" by a preprocessor string-concatenation macro and are thus
hard to visualize, you can also find the names in the Widget's primary ``.c`` file in
the ``properties[]`` array initializing the ``.id`` fields in the array.  For example,
``LV_PROPERTY_LABEL_TEXT`` is one found in ``lv_label.c``.

That array is attached to the Widget's class, enabling "getter" and "setter" functions
to be looked up for each type of Widget where Widget properties has been implemented.

If the property you need to set or get using this API is not implemented yet, you can
add your own Widget property ID following same rules and using helper macro
:c:macro:`LV_PROPERTY_ID` in the ``enum`` in the Widget's primary ``.h`` file.
Just make sure the ID is unique across all Widgets.

The "assembled" identifer is a 32-bit value.  The highest 4 bits contain the
property's value type.  The lower 28 bits is the property ID.

Note that :cpp:type:`lv_style_prop_t` (enumerator values beginning with ``LV_STYLE_...``)
are also valid property IDs, and can be used to set or get a Widget's style values.


.. _obj_property_value:

Property Value
--------------

:cpp:type:`lv_property_t` is a struct that begins with an ``id`` field whose meaning
is the same as property ID described above, paired with a value, which is a union of
all possible property types including integer, pointer and color.  The value field is
also capable of carrying the different values for styles.


Name Lookup
-----------

Setting configuration macro :c:macro:`LV_USE_OBJ_PROPERTY_NAME` to ``1`` enables the
following functions to look up property IDs by passing property name (a string):

.. code:: c

    lv_prop_id_t     lv_obj_property_get_id(const lv_obj_t * obj, const char * name);
    lv_prop_id_t     lv_obj_class_property_get_id(const lv_obj_class_t * clz, const char * name);
    lv_prop_id_t     lv_style_property_get_id(const char * name);

.. note::

    The 1st function uses the 2nd function, and if the name is not found in THAT
    list, then it uses the 3rd function to attempt to find it.  The 3rd function uses
    the name-to-id lookup array in ``lv_style_properties.c::lv_style_property_names[]``.
    These functions use binary searches in an alphabetically-ordered name list, so
    they are somewhat faster than a mere sequential search.



Additional Notes
****************

For pointer type of property value, which typically points to a specific struct, it still needs
additional code to convert values from dict, table, etc. to a C struct before setting to Widget.



API
***

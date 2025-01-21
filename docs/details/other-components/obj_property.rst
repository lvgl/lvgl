.. _widget_property:

=================
Widget Properties
=================

Widget Properties provides a way to greatly reduce the size of the interface between
LVGL and whatever logic layer is just above it, to get and set the most important
properties of Widgets.  It's intended use is to:

- simplify (decreasing development time) writing bindings for LVGL in another
  language, such as:

    - Micropython,
    - Lua,
    - Python,
    - Perl,
    - .NET

- make it possible to control the UI (or parts of it, e.g. animation) via external
  input, without modifying firmware, such as:

    - an external text file (YAML, JSON, XML, custom)
    - any external input source (e.g. serial)

While using it consumes more program space and more CPU overhead while setting and
getting Widget properties, it is designed so minimize that additional CPU overhead.



What is a Widget Property?
**************************

A Widget's properties are the combined set of :ref:`styles` plus additional properties
that are unique to each type of Widget, that determine what the Widget looks like and
how it behaves.  Examples:  size, position, color, are properties of all Widgets
whereas text, long-mode, selection-start, and selection-end, are properties unique to
Label Widgets.  A Widget's :ref:`local styles <style_local>` are also valid
properties in this context.

The non-style Widget properties available for a given Widget are implemented at the
top of that Widget's primary ``.c`` file as a ``const`` id-to-function-pointer lookup
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

This array is attached to the ``properties`` field of the Widget's class, so all
Widgets of the same type share the same id-to-function-pointer lookup array.

Some properties are read-only.  When this is the case, only the ``getter`` field in
the corresponding array element will be initialized with a function pointer.
Example:  an object's child-Widget count or scroll position must be controlled via
other types of input, but their values are readable through this API.



.. _widget_property_usage:

Usage
*****

By default, this feature of LVGL is turned off.  It can be turned on by configuring
:c:macro:`LV_USE_OBJ_PROPERTY` to ``1`` in ``lv_conf.h``.

The 3 functions that then become available are:

.. code:: c

    lv_result_t    lv_obj_set_property(lv_obj_t * widget, const lv_property_t * value);
    lv_property_t  lv_obj_get_property(lv_obj_t * widget, lv_prop_id_t id);

A ``lv_property_t`` is a paired ID and value, and a ``lv_prop_id_t`` is just an ID.

.. code:: c

    lv_result_t    lv_obj_set_properties(lv_obj_t * widget, const lv_property_t * value, uint32_t count);

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

Alternately, :cpp:expr:`lv_obj_set_property(widget, value)` could be called inside
a loop.


.. _widget_property_id:

Property ID
-----------

:cpp:type:`lv_prop_id_t` identifies which property to get/set.  It is an enum value
defined in the primary ``.h`` file for the Widget in question.  Because the actual
names are "assembled" by a preprocessor string-concatenation macro and are thus
hard to visualize, you can also find the names in the Widget's primary ``.c`` file in
the ``properties[]`` array initializing the ``.id`` fields in the array.  For example,
``LV_PROPERTY_LABEL_TEXT`` is one found in ``lv_label.c``, and the properties
available to all Widgets are found near the top of the ``lv_obj.c`` file.

That array is attached to the Widget's class, enabling "getter" and "setter" functions
to be looked up for each type of Widget where Widget properties has been implemented.
(Note:  this is done internally so you don't have to.)

If the property you need to set or get using this API is not implemented yet, you can
add your own Widget property ID following same rules and using one of two helper
macros in the ``enum`` in the Widget's primary ``.h`` file:

- :c:macro:`LV_PROPERTY_ID` (for single values -- see :ref:`Single Values` below)`
- :c:macro:`LV_PROPERTY_ID2` (for paired values -- see :ref:`Paired Values` below)`

Just make sure the ID is unique across all Widgets.

The "assembled" identifer is a 32-bit value.  The highest 4 bits contain the
property's value type.  The lower 28 bits is the property ID.

Note that :cpp:type:`lv_style_prop_t` (enumerator values beginning with ``LV_PROPERTY_STYLE_...``)
are also valid property IDs, and can be used to set or get a Widget's style values.


.. _widget_property_value:

Property Value
--------------

:cpp:type:`lv_property_t` is a struct that begins with an ``id`` field whose meaning
is the same as property ID described above, paired with a value, which is a union of
all possible property types including integer, pointer and color.  The ``value``
field is also capable of carrying the different value types for styles.  It does this
by being a union of all the different types that might be needed.  The list of
"union-ed" fields at this writing are:

.. _single values:

Single Values
~~~~~~~~~~~~~

.. code-block:: c

    int32_t             num;      /**< Signed integer number (enums or "normal" numbers) */
    uint32_t            num_u;    /**< Unsigned integer number (opacity, Booleans) */
    bool                enable;   /**< Booleans */
    const void        * ptr;      /**< Constant pointers  (font, cone text, etc.) */
    lv_color_t          color;    /**< Colors */
    lv_value_precise_t  precise;  /**< float or int for precise value */
    lv_point_t          point;    /**< Point, contains two int32_t */

.. _paired values:

Paired Values
~~~~~~~~~~~~~

.. code-block:: c

    struct {
        /**
         * Note that place struct member `style` at first place is intended.
         * `style` shares same memory with `num`, `ptr`, `color`.
         * So we set the style value directly without using `prop.style.num`.
         *
         * E.g.
         *
         * static const lv_property_t obj_pos_x = {
         *      .id = LV_PROPERTY_STYLE_X,
         *      .num = 123,
         *      .selector = LV_STATE_PRESSED,
         * }
         *
         * instead of:
         * static const lv_property_t obj_pos_x = {
         *      .id = LV_PROPERTY_STYLE_X,
         *      .style.num = 123, // note this line.
         *      .selector = LV_STATE_PRESSED,
         * }
         */
        lv_style_value_t  style;     /**< Make sure it's the first element in struct. */
        uint32_t          selector;  /**< Style selector, lv_part_t | lv_state_t */
    };

    /**
     * For some properties like slider range, it contains two simple (4-byte) values
     * so we can use `arg1.num` and `arg2.num` to set the argument.
     */
    struct {
        union {
            int32_t             num;
            uint32_t            num_u;
            bool                enable;
            const void        * ptr;
            lv_color_t          color;
            lv_value_precise_t  precise;
        } arg1, arg2;
    };

You can find the current :cpp:type:`lv_property_t` struct the
`lv_obj_property.h <https://github.com/lvgl/lvgl/blob/master/src/core/lv_obj_property.h>`__ file.

.. code-block:: c




Name Lookup
-----------

Setting configuration macro :c:macro:`LV_USE_OBJ_PROPERTY_NAME` to ``1`` enables the
following functions to look up property IDs by passing property name (a string):

.. code:: c

    lv_prop_id_t  lv_obj_property_get_id(const lv_obj_t * widget, const char * name);
    lv_prop_id_t  lv_obj_class_property_get_id(const lv_obj_class_t * clz, const char * name);
    lv_prop_id_t  lv_style_property_get_id(const char * name);

.. note::

    The 1st function uses the 2nd function, and if the name is not found in THAT
    list, then it uses the 3rd function to attempt to find it.  The 3rd function uses
    the name-to-id lookup array in ``lv_style_properties.c::lv_style_property_names[]``.
    These functions use binary searches in an alphabetically-ordered name list, so
    they are somewhat faster than a mere sequential search.

You can tell which names are available by looking in the ``.c`` files in the
``./src/widgets/property/`` directory.  Note that to support binary name searches,
these arrays are generated so that they are guaranteed to be in alphabetical order.
Do not edit these files.



Additional Notes
****************

For the ``lv_property_t * value`` argument of the :cpp:func:`lv_obj_set_property`
function, the language used to call that function (e.g. in a static or
dynamically-loaded library) may need additional code to convert values from their
local data type (e.g. dict, table, etc.) to a C struct before passing it to the
:cpp:func:`lv_obj_set_property` function.



API
***

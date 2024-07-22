.. _obj_property:

===============
Widget Property
===============

Widgets have many properties that can decide what they look like and how they behave.
For example, the size, position, color, font, etc. are properties of a widget.
Specially, widget local style is also a property of a widget.

.. _obj_property_usage:

Usage
-----

Two APIs are provided to get/set widget properties. It can be enabled by setting
:c:macro:`LV_USE_OBJ_PROPERTY` to `1` in ``lv_conf.h``.

Set :c:macro:`LV_USE_OBJ_PROPERTY_NAME` to `1` in order to use property name instead of ID.

.. code:: c

    typedef struct {
        lv_prop_id_t id;
        union {
            int32_t num;                /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
            const void * ptr;           /**< Constant pointers  (font, cone text, etc)*/
            lv_color_t color;           /**< Colors*/
            lv_value_precise_t precise; /**< float or int for precise value*/
            struct {
                lv_style_value_t style; /**< Make sure it's the first element in struct. */
                uint32_t selector;      /**< Style selector, lv_part_t | lv_state_t */
            };
        };
    } lv_property_t;

    lv_result_t lv_obj_set_property(lv_obj_t * obj, const lv_property_t * value);
    lv_property_t lv_obj_get_property(lv_obj_t * obj, lv_prop_id_t id);

    lv_prop_id_t lv_obj_property_get_id(const lv_obj_class_t * clz, const char * name);

.. _obj_property_id:

Property ID
~~~~~~~~~~~

:cpp:type:`lv_prop_id_t` identifies which property to get/set. :cpp:type:`lv_property_t` is an enum value
defined in ``lv_obj_property.h`` that are grouped by widget class. You can add your own
widget property ID following same rule and using helper macro :c:macro:`LV_PROPERTY_ID`.
Do make sure the ID is unique across all widgets.


Property ID is a 32-bit value. The higher 4bits indicates the property value type.
The lower 28bits is the property ID.

Note that :cpp:type:`lv_style_prop_t` is also valid property ID.

.. _obj_property_value:

Property Value
~~~~~~~~~~~~~~

Property value is a union of all possible property types including integer, pointer and color.
``_style`` is kept their just to indicate it's compatible with ``style`` value type.


A Step Further
--------------
The unified widget property set/get API is useful when developing wrapper layer for other
modules like micropython, lua, or for an external animation engine.

For pointer type of property value, which typically points to a specific struct, it still needs
additional code to convert values from dict, table etc to a C struct before setting to widget.

Another possible use case is to ease of creating UI from lots of code. For example, you can gather
all properties to an array now and set properties with a for loop.

.. code:: c

    lv_property_t props[] = {
        { .id = LV_PROPERTY_IMAGE_SRC, .ptr = &img_demo_widgets_avatar, },
        { .id = LV_PROPERTY_IMAGE_PIVOT, .ptr = &pivot_50, },
        { .id = LV_PROPERTY_IMAGE_SCALE, .num = 128, },
        { .id = LV_PROPERTY_OBJ_FLAG_CLICKABLE, .num = 1, },
        { .id = LV_STYLE_IMAGE_OPA, .num = 128, },
        { .id = LV_STYLE_BG_COLOR, .color = (lv_color_t){.red = 0x11, .green = 0x22, .blue = 0x33}, },
    }

    LV_OBJ_SET_PROPERTY_ARRAY(obj, props);

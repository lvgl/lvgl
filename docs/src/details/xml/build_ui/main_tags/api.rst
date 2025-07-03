.. _xml_api:

===
API
===

The ``<api>`` tag can be a child of ``<widget>`` and ``<component>`` tags.
(``<screen>``\ s don't support custom APIs.)

The only common point is that both Widgets and Components support having
``<prop>`` (properties) in the ``<api>`` tag to describe their interface.

However, as Widgets and Components work very differently (Widgets have C code,
but Components are pure XML), even properties are interpreted differently.

Components
**********

Overview
--------

While Widgets can have complex ``set``/``get`` APIs, Components are very simple.

When their XML is converted to a C file, only a ``create`` function is generated,
where all the ``<prop>``\ s are arguments. For example:

.. code-block:: xml

    <api>
        <prop name="prop1" type="int"/>
        <prop name="prop2" type="string"/>
    </api>

This generates the following C function:

.. code-block:: c

    lv_obj_t * my_component_create(lv_obj_t * parent, int32_t prop1, const char * prop2);

These properties are set once (at creation time), and there are no specific
``set`` functions to modify the property later. LVGL's general API can still be
used to modify any widget in the component, but no dedicated API functions are generated.

Referencing properties
----------------------

``<prop>``\ s are simply forwarded to widget or component APIs.
For example, if a component has ``<prop name="button_label" type="string"/>``,
it can be used in a label as ``<lv_label text="$button_label"/>``.

In the generated code, these are passed as arguments in create/set functions.

Default values
--------------

Since each property is passed as an argument to the create function, each must have a value.
This can be ensured by:

- Simply setting them in the XML instance
- Providing a default value in the ``<api>``, e.g., ``<prop name="foo" type="string" default="bar"/>``

Limitations
-----------

Note that none of the Widget API features such as ``<param>``, ``<enumdef>``, or ``<element>``
can be used for Components. Only simple properties that are forwarded are supported.

Example
-------

.. code-block:: xml

    <!-- my_button.xml -->
    <component>
        <api>
            <prop name="button_icon" type="image" default="NULL"/>
            <prop name="button_label" type="string" default="Label"/>
        </api>

        <view extends="lv_button" flex_flow="row" width="100%">
            <lv_image src="$button_icon" inner_align="stretch" width="16" height="16"/>
            <lv_label text="$button_label"/>
        </view>
    </component>

    <!-- my_list.xml -->
    <component>
        <view flex_flow="column">
            <my_button button_label="First"/>
            <my_button button_label="Wifi" button_icon="img_wifi"/>
            <my_button button_label="Third"/>
        </view>
    </component>

Widgets
*******

Properties
----------

Properties are the core part of describing a Widget's API.

.. code-block:: xml

    <api>
        <prop name="text" type="string" help="Text of the label."/>
    </api>

Parameters
----------

Some properties take multiple parameters. For example:
:cpp:expr:`lv_label_set_bind_text(label, subject, "%d °C")`

It's described as:

.. code-block:: xml

    <api>
        <prop name="bind_text" help="Bind a subject's value to a label.">
            <param name="bind_text" type="subject" help="Integer or string subject"/>
            <param name="fmt" type="string" help="Format string, e.g. %d °C "/>
        </prop>
    </api>

And used as:

.. code-block:: xml

    <lv_label bind_text="subject" bind_text-fmt="%d °C"/>

Parameters with the same name as the property can be referenced directly.
Other parameters use ``property-param`` notation.

Unset parameters fall back to:

- Their default value (if defined)
- Type-specific defaults (e.g., 0, false, NULL)

Mapping
-------

Each ``<prop>`` is mapped to a ``set`` function. This mapping is implemented
in the Widget's XML parser.
See `the LVGL XML parsers <https://github.com/lvgl/lvgl/tree/master/src/others/xml/parsers>`_.

If ``<param>``s are used, they are passed to the same ``set`` function.
If a property is not set on a Widget instance, it is skipped and the Widget's
built-in default is used.

<enumdef>
---------

Only used with Widgets, this tag defines enums for parameter values.

.. code-block:: xml

    <api>
        <enumdef name="my_widget_mode" help="Possible modes">
            <enum name="normal" help="Normal mode" value="0x10"/>
            <enum name="inverted" help="Inverted mode"/>
        </enumdef>
        <prop name="mode" help="Set Widget mode">
            <param name="mode" type="enum:my_widget_mode"/>
        </prop>
    </api>

Enum values are ignored in export; the names are used and resolved by the compiler.
XML parsers must handle mapping enum names to C enums.

.. _xml_widget_element:

<element>
---------

Also exclusive to Widgets, elements define sub-widgets or internal structures
(e.g., chart series, dropdown list, tab views).

They support ``<arg>`` and ``<prop>``:

- ``<arg>``\ s are required and used when creating/getting the element.
- ``<prop>``\ s are optional and mapped to setters.

Elements are referenced as ``<widget-element>`` in views.

Name parts are separated by `-` ( as `-` is not allowed inside names).

Element `access` types:

- ``add``: Create multiple elements dynamically.
- ``get``: Access implicitly created elements.
- ``set``: Access indexed parts (e.g., table cells).
- ``custom``: Map custom C function to XML.

As ``add`` and ``get`` elements return an object they also have a type.
This type can be any custom type, for example, `type="my_data"`. In the exported code the
return value will be saved in a ``my_data_t *`` variable.

If the type is ``type="lv_obj"`` it allows the element to have children widgets or components.

Note that, only the API can be defined in XML for elements; implementations must be in C.

access="add"
~~~~~~~~~~~~

Elements are created via an ``add`` function:

.. code-block:: xml

    <api>
        <element name="indicator" type="obj" help="The indicator of my_widget" access="add">
            <arg name="color" type="color"/>
            <arg name="max_value" type="int"/>
            <prop name="value">
                <param name="value" type="int"/>
            </prop>
        </element>
    </api>

Used in a view:

.. code-block:: xml

    <my_widget width="100px">
        <my_widget-indicator name="indic1" color="0xff0000" max_value="120" value="30"/>
    </my_widget>

Generates:

.. code-block:: c

    lv_obj_t * my_widget_add_indicator(lv_obj_t * parent, lv_color_t color, int32_t max_value);
    void my_widget_set_indicator_value(lv_obj_t * obj, int32_t value);

access="get"
~~~~~~~~~~~~

Used for internal/implicit elements:

.. code-block:: xml

    <api>
        <element name="control_button" type="obj" help="A control button of my_widget" access="get">
            <arg name="index" type="int"/>
            <prop name="title" type="string"/>
        </element>
    </api>

Used in a view:

.. code-block:: xml

    <my_widget width="100px">
        <my_widget-control_button name="btn1" index="3" title="Hello"/>
    </my_widget>

Generates:

.. code-block:: c

    lv_obj_t * my_widget_get_control_button(lv_obj_t * parent, int32_t index);
    void my_widget_set_control_button_title(lv_obj_t * obj, const char * text);

access="set"
~~~~~~~~~~~~

Used for indexed access, like setting values in a table:

.. code-block:: xml

    <api>
        <element name="item" type="obj" access="set">
            <arg name="index" type="int"/>
            <prop name="icon" type="img_src"/>
            <prop name="color" type="color"/>
        </element>
    </api>

Used in a view:

.. code-block:: xml

    <my_widget width="100px">
        <my_widget-item index="3" icon_src="image1" color="0xff0000"/>
    </my_widget>

Generates:

.. code-block:: c

    void my_widget_set_item_icon(lv_obj_t * parent, int32_t index, const void * icon_src);
    void my_widget_set_item_color(lv_obj_t * parent, int32_t index, lv_color_t color);

access="custom"
~~~~~~~~~~~~~~~

Used to describe any custom API functions with a custom name.
"custom" elements can have only arguments and no `type` so they are pure setters.

.. code-block:: xml

    <element name="bind_color" access="custom">
        <arg name="subject" type="subject"/>
        <arg name="new_color" type="color"/>
        <arg name="ref_value" type="int"/>
    </element>

Used in a view:

.. code-block:: xml

    <my_widget width="100px">
        <my_widget-bind_color subject="subject_1" color="0xff0000" ref_value="15"/>
    </my_widget>

Generates:

.. code-block:: c

    void my_widget_bind_color(lv_obj_t * parent, lv_subject_t * subject, lv_color_t color, int32_t ref_value);

.. _xml_api:

===
API
===

The ``<api>`` tag can be a child of ``<widget>`` and ``<components>`` tags, although
each supports slightly different features.



Properties
**********

Inside ``<prop>`` elements, ``<param>`` elements can be defined to describe the arguments.

For **Widgets**, all properties are optional.
If a property is not set on an instance of a Widget, it simply won't be applied,
and the created Widget's default value for that property will be used (e.g., ``text``
for a label's text).

For **Components**, all properties are mandatory; however, default values can be defined
to be used when a property is not set.

If a property has only one parameter (which is usually the case), a shorthand syntax
can be applied as shown below.

For example:

.. code-block:: xml

    <api>
        <prop name="range" default="0 100" help="Set the range.">
            <param name="range_min" type="int" help="Sets the minimum value."/>
            <param name="range_max" type="int" help="Sets the maximum value."/>
        </prop>
        <prop name="title" type="string" help="The title of the slider"/>
    </api>

When a property is used, all parameters are set as a single attribute value.  For example:

.. code-block:: xml

    <my_slider range="-100 100" title="Room 1"/>

For **Widgets**, each property corresponds to a setter function.
The ``name`` in ``<prop>`` is used to build the name of the setter function like this:

.. code-block:: c

    <widget_name>_set_<prop_name>(lv_obj_t * obj, <param1_type> <param1_name>, <param2_type> <param2_name>, ...);

For **Components**, the exported code contains only a single ``create`` function
to which all the properties are passed:

.. code-block:: c

    <component_name>_create(lv_obj_t * parent, <param1_type> <param1_name>, <param2_type> <param2_name>, ...);

``<prop>`` elements have an optional ``<postponed>`` boolean attribute.
By default, it is ``false``, but if set to ``true``, the given property will be
applied after all children are created.  A practical example is setting the current
tab of a tab view, which cannot be set before the tabs are created.  This feature is
not supported yet.



``<enumdef>``
*************

This tag is used only with Widgets.  It is used to define new enum types for a given
Widget.  It should contain ``<enum>`` elements to define possible options.

Example:

.. code-block:: xml

    <!-- my_widget.xml -->
    <api>
        <enumdef name="my_widget_mode" help="Possible modes">
            <enum name="normal" help="Normal mode" value="0x10"/>
            <enum name="inverted" help="Inverted mode"/>
        </enumdef>

        <prop name="mode" help="Set Widget mode">
            <param name="mode" type="enum:my_widget_mode"/>
        </prop>
    </api>

Note that the enum values are not important because:

1. When the code is exported, the enum names will be used, and the compiler generates
   its own value for each enumerator symbol.
2. When loaded from XML, the Widget's XML parser should convert the enum names to C
   enum fields.



``<element>``
*************

``<element>`` tags also apply only to Widgets.  Elements are used to describe
sub-Widgets or internal parts of Widgets.  Examples include the list of a dropdown,
the tabs of a tab view, or the data series of a chart.

Elements can have ``<arg>`` and ``<prop>`` definitions.  ``<arg>`` elements are
mandatory (default values are supported) as they are used to create the element,
whereas ``<prop>`` elements are optional as they are mapped to setter functions.

An element in a ``<view>`` can be referenced like this:  ``<widget_name-element_name>``.
Note that the ``-`` separates two names inside that tag name:  the Widget name and the
element name.  ``-`` is not allowed in Widget and element names.  Only ``_`` can be
used to separate words in tag names.

Example:

.. code-block:: xml

    <my_chart-super_series color="0xff0000"/>

An important attribute of elements is ``access``.  The possible values are:

- ``add``: Create any number of elements dynamically (e.g., chart series).
- ``get``: Get a pointer to an implicitly created Widget or any data (e.g., list of a Drop-Down List).
- ``set``: Select specific parts of the Widget with indexes (e.g., table cells).

Elements with ``access="add"`` or ``access="get"`` can have a custom data type
defined using ``type="my_data"``.  In these cases, no children can be added.  If the
``type`` is ``lv_obj``, the element can have children.

It is not yet possible to describe the ``<view>`` of elements in XML; only the API can be defined.
The actual implementation needs to be done in C.


``access="add"``
----------------

The element is explicitly created with an ``add`` function, e.g., ``lv_tabview_add_tab(obj, "Title");``.

``<arg>`` elements defined as direct children of the ``<element>`` are passed to the
``add`` function as arguments.

Example:

.. code-block:: xml

    <!-- my_widget.xml -->
    <api>
        <element name="indicator" type="lv_obj" help="The indicator of my_widget" access="add">
            <arg name="color" type="color" help="Help for color"/>
            <arg name="max_value" type="int" help="Help for max_value"/>
            <prop name="value" help="Set a new value for the indicator">
                <param name="value" type="int" help="Help for value"/>
            </prop>
        </element>
    </api>

    <view extends="obj">
        <button name="btn1"/>
    </view>

In a view it can be used like this:

.. code-block:: xml

    <!-- complex_widget.xml -->
    <view>
        <lv_label text="Title"/>
        <my_widget width="100px" y="40px">
            <my_widget-indicator name="indic1" color="0xff0000" max_value="120" value="30"/>
        </my_widget>
    </view>

From the API definition the following functions are generated:

.. code-block:: c

    lv_obj_t * my_widget_add_indicator(lv_obj_t * parent, lv_color_t color, int32_t max_value);

    void my_widget_set_indicator_value(lv_obj_t * obj, int32_t value);

And this is the related C file where the indicator is created:

.. code-block:: c

    lv_obj_t * indic1 = my_widget_add_indicator(parent, color, max_value);
    lv_my_widget_set_indicator_value(indic1, value);


``access="get"``
----------------

If the element is created internally and implicitly, it can be retrieved with a
function like ``lv_dropdown_get_list(obj);``.

``<arg>`` elements are passed to the ``get`` function as arguments.

Example:

.. code-block:: xml

    <!-- my_widget.xml -->
    <api>
        <element name="control_button" type="lv_obj" help="A control button of my_widget" access="get">
            <arg name="index" type="int" help="Zero-based index of the control button"/>
            <prop name="title">
                <param name="text" type="string"/>
            </prop>
        </element>
    </api>

In a view:

.. code-block:: xml

    <!-- complex_widget.xml -->
    <view>
        <my_widget width="100px">
            <my_widget-control_button name="btn1" index="3" title="Hello"/>
        </my_widget>
    </view>

Generated API:

.. code-block:: c

    lv_obj_t * my_widget_get_control_button(lv_obj_t * parent, int32_t index);
    void my_widget_set_control_button_title(lv_obj_t * obj, const char * text);

And this is a C file where the control button is retrieved:

.. code-block:: c

    lv_obj_t * btn1 = lvmy_widget_get_control_button(parent, index);
    my_widget_set_control_button_title(btn1, text);


``access="set"``
----------------

The "set" value is used when elements are created automatically but need to be selected in API calls,
e.g., ``lv_table_set_cell_value(table, row, col, "text");``.

Example:

.. code-block:: xml

    <!-- my_widget.xml -->
    <api>
        <element name="item" type="lv_obj" help="An item on my_widget" access="set">
            <arg name="index" type="int" help="The zero-based index of the item"/>
            <prop name="icon" help="Set the icon of an item">
                <param name="icon_src" type="img_src" help="The image to set as an icon."/>
            </prop>
            <prop name="color" help="Set the color">
                <param name="color" type="color" help="The color to set for the item."/>
            </prop>
        </element>
    </api>

In a view:

.. code-block:: xml

    <!-- complex_widget.xml -->
    <view>
        <my_widget width="100px">
            <my_widget-item index="3" icon_src="image1" color="0xff0000"/>
        </my_widget>
    </view>

This is the generated header file:

.. code-block:: c

    void my_widget_set_item_icon(lv_obj_t * parent, int32_t index, const void * icon_src);

    void my_widget_set_item_color(lv_obj_t * parent, int32_t index, lv_color_t color);

And this is the related C file where the item properties are set:

.. code-block:: c

    my_widget_set_item_icon(parent, index, image1);
    my_widget_set_item_color(parent, index, color);

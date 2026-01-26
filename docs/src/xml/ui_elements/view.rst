.. _xml_view:

=========
View
=========

Overview
********

The ``<view>`` tag can be used in:

- ``<component>``\ s
- ``<widget>``\ s
- ``<screen>``\ s
- ``<test>``\ s

to describe how these items look. Inside ``<view>``, children can be added in a nested way
using already defined ``widget``\s and ``component``\s. For example:

.. code-block:: xml

    <view>
        <lv_button width="200">
            <my_icon src="image1"/>
            <lv_label text="Click me"/>
        </lv_button>
    </view>

Extends
*******

The ``<view>`` itself is also a widget or component, which will become the parent of the children.
To define the type of the ``<view>``, use the ``extends`` attribute. For example, ``extends="lv_slider"``.
In this case, an ``lv_slider`` will be created first, and the children will be added to it.

By adding properties to the ``<view>``, the extended type can be customized. For example:

.. code-block:: xml

    <view extends="lv_slider" width="100%" style_bg_color="0xff8800" flex_flow="row">
        <style name="my_style" selector="pressed|knob"/>
        <lv_label text="Current value: "/>
        <lv_label bind_text="subject_1"/>
    </view>

Rules for allowed values of ``extends``:

- ``<component>``: can extend both ``<widget>``\ s and ``<component>``\ s
- ``<widget>``: can extend only ``<widget>``\ s
- ``<screen>``: cannot extend anything
- ``<test>``: can extend ``<widget>``, ``<component>``, or ``<screen>``


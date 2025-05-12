.. _xml_styles:

======
Styles
======

Overview
********

In XML files, both style sheets (:cpp:expr:`lv_style_t`) and local styles can be used.

Style variants are also supported to change style properties at runtime.



Style Sheets
************

In the ``<styles>`` section, styles and their properties can be defined like this:

.. code-block:: xml

    <style name="red"
        help="What is this style about?"
        border_width="2px"
        border_color="0xff0000"/>

Styles can be referenced like this in the ``<view>``:

.. code-block:: xml

    <view>
        <slider styles="main red:indicator red:knob:focused"/>
    </view>

As shown in the example, parts and states are appended after a ``:`` to the style's name.



Local Styles
************

Local styles can be used directly in a Widget, for example:

.. code-block:: xml

    <lv_label style_bg_opa="200" style_bg_opa:disabled="100"/>



Gradients
*********

Before the ``<styles>`` tag, the ``<gradients>`` tag can be used to describe various
gradients, which can later be referenced in styles.

When a gradient is created, it can be referenced by its name, like:

.. code-block:: xml

    <style bg_grad="grad1"/>

or

.. code-block:: xml

    <lv_button style_bg_grad="grad1"/>


Horizontal or Vertical Gradient
-------------------------------

To define a simple ``<horizontal>`` or ``<vertical>`` gradients:

.. code-block:: xml

    <gradients>
        <horizontal name="grad1">
            <stop color="#ff0000" offset="20%" opa="40%"/>
            <stop color="#00ff00" offset="128" opa="100%"/>
        </horizontal>
    </gradients>


Linear Gradient
---------------

To define a skewed gradient from two points:

.. code-block:: xml

    <gradients>
        <linear name="grad1" start="50 50" end="100 80">
            <stop color="#ff0000" offset="20%" opa="100%"/>
            <stop color="#00ff00" offset="240" opa="100%"/>
        </linear>
    </gradients>


Radial Gradient
---------------

To define a radial gradient:

.. code-block:: xml

    <gradients>
        <radial name="grad1" center="100 50%" edge="200 50" focal_center="50 80%" focal_edge="55 80%">
            <stop color="#ff0000" opa="100%"/>
            <stop color="#00ff00" opa="100%"/>
        </radial>
    </gradients>


Conical Gradient
----------------

To define a conical gradient:

.. code-block:: xml

    <gradients>
        <conical name="grad1" center="80 50%" angle="45 270">
            <stop color="#ff0000" opa="100%"/>
            <stop color="#00ff00" opa="100%"/>
        </conical>
    </gradients>

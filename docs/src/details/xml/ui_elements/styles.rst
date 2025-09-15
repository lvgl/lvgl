.. _xml_styles:

======
Styles
======

Overview
********

In XML files, both style sheets (:cpp:expr:`lv_style_t`) and local styles can be used.

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
        <lv_slider value="20">
             <style name="main"/>
             <style name="red" selector="knob"/>
             <style name="blue" selector="knob|focused"/>
        </lv_slider>
    </view>

As shown in the example, parts and states can be set using ``selector``.

Style binding
*************

Instead of directly adding styles to the UI elements it's also possible to add them conditionally
when a :ref:`Subject <observer_how_to_use>`\ 's value equals to a reference value.

It works at runtime and it's a great way to check the appearace or event the layout based on a condition.

A typical use case is the light/dark theme switching. It requires

- a subject such as ``dark_theme_on``
- some default style that are added normally with the ``<style>`` tag
- some dark styles to check the required colors to dark

Here is an example:

.. code-block:: xml

    <component>
       <styles>
           <style name="style_base" bg_color="0xeee" text_color="0x111" radius="20" />
           <style name="style_dark" bg_color="0x333" text_color="0xeee" radius="20" />
       </styles>

       <view extends="lv_button">
            <style name="style_base" />
            <bind_style name="style_dark" subject="dark_theme_on" ref_value="1"/>
            <lv_label text="Apply"/>
       </view>
    </component>

Local Styles
************

Local styles can be used directly, for example:

.. code-block:: xml

    <lv_label style_bg_opa="200" style_bg_color="0x123456"/>

Selectors are not supported for local style properties yet.

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

Note that gradients are not supported in LVGL's UI Editor yet.

Horizontal or Vertical Gradient
-------------------------------

To define a simple ``<horizontal>`` or ``<vertical>`` gradient:

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


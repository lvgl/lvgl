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

Local styles can be used directly in a widget, for example:

.. code-block:: xml

	<lv_label style_bg_opa="200" style_bg_opa:disabled="100"/>

Style Variants
**************

The ``<style>`` tags can have ``<variant>`` child tags:

.. code-block:: xml

	<styles>
		<style name="big_button" bg_color="0xf00" border_width="1px" pad_left="10px">
			<variant name="color" case="red" bg_color="0xf00"/>
			<variant name="color" case="green" bg_color="0x0f0"/>
			<variant name="color" case="blue" bg_color="0x00f"/>
		</style>
	</styles>

``<variant>`` elements allow altering styles at runtime.
The ``variant_<name>`` subjects of the component library are used for each setting, and an observer callback is generated with all the style properties valid for that variant. The observer callback first resets the style and then sets all the properties.

This feature is not supported yet.

Gradients
*********

Before the ``<styles>`` tag, the ``<gradients>`` tag can be used to describe various gradients, which can later be referenced in styles.

When a gradient is created, it can be referenced by its name, like:

.. code-block:: xml

	<style bg_grad="grad1"/>

or

.. code-block:: xml

	<lv_button style_bg_grad="grad1"/>

Horizontal or Vertical Gradient
-------------------------------

Define simple ``<horizontal>`` or ``<vertical>`` gradients:

.. code-block:: xml

	<gradients>
		<horizontal name="grad1">
			<stop color="#ff0000" offset="20%" opa="40%"/>
			<stop color="#00ff00" offset="128" opa="100%"/>
		</horizontal>
	</gradients>

Linear Gradient
---------------

Define a skewed gradient from two points:

.. code-block:: xml

	<gradients>
		<linear name="grad1" start="50 50" end="100 80">
			<stop color="#ff0000" offset="20%" opa="100%"/>
			<stop color="#00ff00" offset="240" opa="100%"/>
		</linear>
	</gradients>

Radial Gradient
---------------

Define a radial gradient:

.. code-block:: xml

	<gradients>
		<radial name="grad1" center="100 50%" edge="200 50" focal_center="50 80%" focal_edge="55 80%">
			<stop color="#ff0000" opa="100%"/>
			<stop color="#00ff00" opa="100%"/>
		</radial>
	</gradients>

Conical Gradient
----------------

Define a conical gradient:

.. code-block:: xml

	<gradients>
		<conical name="grad1" center="80 50%" angle="45 270">
			<stop color="#ff0000" opa="100%"/>
			<stop color="#00ff00" opa="100%"/>
		</conical>
	</gradients>

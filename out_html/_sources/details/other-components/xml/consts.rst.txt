.. _xml_consts:

=========
Constants
=========

Overview
********

Constants can be defined to replace any value with a selected type or to be used as special values.

The supported types are:
- ``color``
- ``px``
- ``percentage``
- ``string``
- ``opa``
- ``bool``

Usage
*****

.. code-block:: xml

	<consts>
		<color name="color1" value="0xff0000" help="Primary color"/>
		<px name="pad_xs" value="8" help="Small padding"/>
	</consts>

Constants can be used in:

- Style properties
- Widget and component properties

And they can be used like this:

.. code-block:: xml

	<styles>
		<style name="style1" bg_color="#color1"/>
	</styles>

Variants
********

Constants support a ``<variant>`` attribute to change the constants at compile time. For example:

.. code-block:: xml

	<consts>
		<px name="pad" value="8" help="General padding">
			<variant name="size" case="small" value="4"/>
			<variant name="size" case="large" value="12"/>
		</px>
	</consts>

From which the following C code can be exported:

.. code-block:: c

	#if SIZE == SMALL
		#define PAD 4
	#elif SIZE == LARGE
		#define PAD 12
	#else
		#define PAD 8
	#endif

Where ``SMALL`` and ``LARGE`` are just preprocessor defines with incremental values.

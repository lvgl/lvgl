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
- Widget and Component properties

Constant values can be referenced by ``#constant_name``. For example:

.. code-block:: xml

    <styles>
        <style name="style1" bg_color="#color1"/>
    </styles>


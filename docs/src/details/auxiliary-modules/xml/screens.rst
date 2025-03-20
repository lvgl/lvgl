.. _xml_screens:

=======
Screens
=======

Overview
********

Screens work very similarly to components. Both can be:

- Loaded from XML
- Contain widgets and components as children
- Have ``<styles>``
- Have ``<consts>``
- Have a ``<view>``

However, screens **cannot** have an ``<api>``.

Usage
*****

Each XML file describes a screen. The name of the XML file will also be the name of the screen.

In the ``project.xml`` file, multiple ``<display>`` elements can be defined. In the UI Editor, when a screen is being developed,
the user can select from all the defined displays in the Preview, and the screen will be shown with the given resolution and color depth.

This is useful for verifying responsive designs.

Example
*******

.. code-block:: xml

	<screen>
		<consts>
			<string name="title" value="Main menu"/>
		</consts>

		<styles>
			<style name="dark" bg_color="0x333"/>
		</styles>

		<view>
			<header label="#title"/>
			<selector_container styles="dark">
				<button text="Weather" icon="cloudy"/>
				<button text="Messages" icon="envelope"/>
				<button text="Settings" icon="cogwheel"/>
				<button text="About" icon="questionmark"/>
			</selector_container>
		</view>
	</screen>

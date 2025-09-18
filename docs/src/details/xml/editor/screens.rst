.. include:: custom_tools.txt

.. _creating screens:

================
Creating Screens
================

From the :ref:`project files pane`, place focus on the "screens" folder,
right click and select "New File".  Name your file after the name you wish your
new Screen to have, and end it with ``.xml``.

Once in the editor, the auto-completion assistance will help you to create the
Screen's contents.

First create ``<screen>`` tag itself.  Create any "local" enumerations, constants
or styles that will be specific to this Screen, then create the ``<view>`` tag
to hold the Screen itself.

The first attribute of the ``<view>`` tag is typically ``style_bg_color="..."``
and ``scrollable="false"`` (or true) and ``style_layout="..."`` for any desired
automated layout capabilities (e.g. flex).  Use the Editor's auto-completion mechanism
to discover the available attributes and values.

After that, add each Component and/or Widget that will be a part of this screen.
Use the Component's or Widget's name as the XML element name (e.g.
``<top_bar>...</top_bar>``, ``<percent_slider>...</percent_slider>``, etc.).
Names of core LVGL Widgets can be used by using their ``lv_xxxxx`` names (e.g.
lv_button, lv_slider, lv_checkbox, etc.).

As might be expected, global and/or Screen-scope styles can be applied to any
Component or Widget by simply including a ``<style>`` element inside the XML element
for that Component or Widget.  Example:

.. code-block:: xml

    <style name="slider_knob_style" selector="knob"/>

You can test your Screen at each phase using your mouse in the :ref:`preview pane`.



Permanent vs Temporary Screens
******************************

In the opening ``<screen ...>`` in a screen's definition, you can have an attribute
``permanent="true"``.  If this attribute is present, then once the screen has been
created, it remains in ram to be later referenced by name in a
``<screen_load_event screen="my_screen_name" ... />`` element.

Other screens are considered temporary and are automatically deleted from RAM when
another screen is loaded.



Screen Create and Load Events
*****************************

``<screen_create_event ... />`` and ``<screen_load_event ... />`` elements are placed
inside of a Component in order to create (temporary) or load (permanent) screens by
name as a response to that Component's default event (e.g. CLICK for an ``lv_button``).

To create and load temporary screens, use a ``<screen_create_event ... />`` element.
To load a permanent screen, use a ``<screen_load_event ... />`` to re-load that
screen.

As an example, let's say the main (permanent) screen of an application is called
"settings" (by way of it being defined in a file ``settings.xml``.  In that screen
there is an "About" button:

.. code-block:: xml

    <lv_button>
        <lv_label text="About" />
        <!-- other stuff here -->
        <screen_create_event screen="about" anim_type="fade_on" duration="500"/>
    </lv_button>

In the :ref:`Preview Pane`, when the "About" button is clicked, the Preview Pane will
create and load the screen defined in the ``about.xml`` file, which is a temporary
screen.

If the About screen then has a back button defined like this

.. code-block:: xml

    <lv_button>
        <!-- other stuff here -->
        <lv_label text="Back"/>
        <screen_load_event screen="settings"/>
    </lv_button>

then clicking it will re-load the "Settings" screen again.
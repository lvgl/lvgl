.. _xml_screens:

=======
Screens
=======

Overview
********

.. |nbsp|   unicode:: U+000A0 .. NO-BREAK SPACE
    :trim:

Screens work very similarly to Components. Both can be:

- Loaded from XML
- Contain Widgets and Components as children
- Have ``<styles>``
- Have ``<consts>``
- Have a ``<view>``

However, Screens **cannot** have an ``<api>``.

It's also possible to define ``<screen permanent="true">`` which
will make the screen created automatically.


Usage
*****

Each XML file describes a :ref:`Screen <screens>`.  The name of the XML file will
also be the name of the Screen.

In the ``project.xml`` file, multiple ``<display>`` elements can be defined. In the
UI |nbsp| Editor, when a Screen is being developed, the user can select from all the
defined displays in the Preview, and the Screen will be shown with the given
resolution and color depth.

This is useful for verifying responsive designs.

This example illustrates a screen in XML:

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
Screen Load and Create events
*****************************

By using the ``<screen_load_event>`` and ``<screen_create_event>`` tags as a child
of a widget or component, screens can be loaded or created on a trigger (e.g. click).

The difference between load and create is that:

- **load**: Just loads an already existing screen. When the screen is left, it remains in memory,
  so all states are preserved.
- **create**: The screen is created dynamically, and when it's left, it is deleted, so all changes are lost
  (unless they are saved in ``subjects``).

Both tags support the following optional attributes:

- ``trigger``: Event code that triggers the action (e.g. ``"clicked"``, ``"long_pressed"``, etc). Default: ``"clicked"``.
- ``anim_type``: Describes how the screen is loaded (e.g. ``"move_right"``, ``"fade_in"``). Default: ``"none"``.
- ``duration``: Length of the animation in milliseconds. Default: ``0``. Only used if ``anim_type`` is not ``"none"``.
- ``delay``: Wait time before loading the screen in milliseconds.

This is a simple example of both load and create:

.. code-block:: xml

    <!-- screen1.xml -->
    <screen>
       <view style_bg_color="0xff7788" name="first">
           <lv_button>
               <lv_label text="Create"/>
               <!-- Create an instance of "screen2" and load it.
                    Note that here the name of the XML file is used. -->
               <screen_create_event screen="screen2" anim_type="over_right" duration="500" delay="1000"/>
           </lv_button>
       </view>
    </screen>

    <!-- screen2.xml -->
    <screen>
       <view style_bg_color="0x77ff88" name="second">
           <lv_button>
               <lv_label text="Load"/>
               <!-- Load an already created screen that has the name "first".
                    Note that here the name of the instance is used,
                    and not the name of the XML file. -->
               <screen_load_event screen="first"/>
           </lv_button>
       </view>
    </screen>

.. code-block:: c

    lv_xml_component_register_from_data("screen1", screen1_xml);
    lv_xml_component_register_from_data("screen2", screen2_xml);

    /*Create an instance of screen_1 so that it can loaded from screen2.*/
    lv_obj_t * screen1 = lv_xml_create(NULL, "screen1", NULL);
    lv_screen_load(screen1);
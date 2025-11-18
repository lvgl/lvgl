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

Screens are wrapped in the ``<screen>`` XML root element and used to organize
the content of the UI.

Screens can have only the following child XML tags:

- :ref:`<consts> <xml_consts>`
- :ref:`<styles> <xml_styles>`
- :ref:`<view> <xml_view>`

That is, Screens **cannot** have:

- :ref:`<api> <xml_api>`: Screens are always created as they are, with no parameters.
- :ref:`<preview> <xml_preview>`: In LVGL's UI Editor, ``<preview>`` is used to set the style, size, and other parameters of the previewing "screen". For Screens, the
  preview options are defined in ``project.xml``. Learn more in :ref:`Screen preview <xml_screen_preview>`

Usage
*****

Each XML file describes a :ref:`Screen <screens>`. The name of the XML file will
also be the name of the Screen.

This example illustrates a screen in XML. In the example, a ``<my_header>``
and a ``<my_main_cont>`` component are used to keep the screen simple.

.. code-block:: xml

    <screen>
        <consts>
            <string name="title" value="Main menu"/>
        </consts>

        <styles>
            <style name="dark" bg_color="0x333"/>
        </styles>

        <view>
            <my_header label="#title"/>
            <my_main_cont>
                <style name="dark"/>

                <button text="Weather" icon="cloudy"/>
                <button text="Messages" icon="envelope"/>
                <button text="Settings" icon="cogwheel"/>
                <button text="About" icon="questionmark"/>
            </my_main_cont>
        </view>
    </screen>

Code export
***********

LVGL's UI Editor can export C code for Screens as well. It generates ``screen_name_gen.c/h`` files,
containing only a single ``lv_obj_t * screen_name_create(void)`` create function.

By using this function, any number of screen instances can be created and loaded as needed.

.. _xml_screen_permanent:

Permanent Screens
*****************

The only possible property of the ``<screen>`` tag is ``permanent``, which can be
``true`` or ``false`` (default).

This property affects the :ref:`screen load and create events <xml_events_screen>`.
If a screen is permanent, then when a new screen is loaded, the permanent screen is
not deleted so that it keeps its state. Non-permanent screens will be deleted and
created automatically when they are opened and closed.

It's also assumed that permanent screens are created when the UI is initialized. When
code is exported from the UI Editor, the permanent screens are actually created and
stored in global pointers.

In light of that, permanent screens only need to be **loaded** by
``<screen_load_event screen="my_permanent_screen"/>`` and
non-permanent screens need to be **created** by
``<screen_create_event screen="my_non_permanent_screen"/>``.


.. _xml_screen_preview:

Preview
*******

Screens don't support the :ref:`<preview> <xml_preview>` tag because it doesn't make
sense to preview each screen in different resolutions.

As Screens are related to the target hardware in the ``project.xml`` file, multiple
``<display>`` elements can be defined. In the UI |nbsp| Editor, when a Screen is being developed,
the user can select from all the defined displays in the Preview, and the Screen will be shown with
the given resolution and color depth.

This is useful for verifying responsive designs.

Events
******

It's very common to load or create Screens on a button click or other events.

Both are supported by adding special XML tags as children of Components or Widgets:

.. code-block:: xml

   <view>
       <lv_button>
           <lv_label text="Click or Long press me"/>

           <!-- Create an instance of "screen2" and load it. -->
           <screen_load_event screen="first" trigger="clicked" anim_type="fade"/>

            <!-- Load an already created instance of screen1.-->
           <screen_create_event screen="about" trigger="long_pressed"/>
       </lv_button>
   </view>

Learn more on the :ref:`XML Events <xml_events_screen>` page.


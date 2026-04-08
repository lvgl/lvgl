.. include:: /include/substitutions.txt
.. _xml_preview:

=======
Preview
=======

Overview
********

In ``<component>`` and ``<widget>``, it is possible to define ``<preview>`` tags.
These are **not** exported to code and are **not** loaded from XML.

They are used only by the UI |nbsp| Editor to describe the context of the Component.
For example, you might want to:

- change the background of the Editor's preview to dark,
- center the Component,
- set margins,
- change the size of the preview.

``style_radius`` can be used to make the preview rounded.

Usage
*****

You can think of a ``<preview>`` tag as an ``lv_obj`` where the following properties can be used:

- ``width``, ``height``
- Any local style properties, for example, ``style_bg_color="0x333"``
- ``flex`` and ``flex_flow``

It is also possible to define multiple previews, and in the UI |nbsp| Editor, you can
select one of them.

For Screens
-----------

Screens don't support the :ref:`<preview> <xml_preview>` tag because it doesn't make
sense to preview each screen in different resolutions.

As Screens are related to the target hardware in the ``project.xml`` file, multiple
``<display>`` elements can be defined. In the UI |nbsp| Editor, when a Screen is being developed,
the user can select from all the defined displays in the Preview, and the Screen will be shown with
the given resolution and color depth.

This is useful for verifying responsive designs.

Example
*******

.. code-block:: xml

    <component>
        <previews>

            <preview name="small_dark_round"
                     width="240" height="240"
                     style_bg_color="0x333" style_pad_all="32"
                     style_radius="32"/>

            <preview name="large_light"
                     width="1980" height="1080"
                     style_bg_color="0xeeeeee"/>

        </previews>

        <view>
            ...
        </view>

    </component>


.. include:: /include/substitutions.txt
.. _editor_figma:

=================
Figma integration
=================

Overview
********

|lvglpro|\ 's `Figma <https://www.figma.com/community/plugin/1362005814860504095/
figma-to-lvgl>`__ plugin displays the style properties of selected UI elements in
Figma.

These properties can be easily copy-pasted as style attributes in the Editor,
enabling pixel-perfect design implementation.

The plugin also shows the "Figma node ID" of the selected element, which allows
style synchronization. This means that, with a single click in the Editor or with
a CLI command, style properties from Figma can be downloaded and updated in the
XML files.

Since this is not a magical auto-importer, the developer retains full control
over how files, styles, components, etc., are organized. At the same time, the
designer is free to work in Figma without constraints. The developer should
take responsibility for restructuring the design with a developer mindset to
ensure a maintainable, clean, and optimal structure.

The Figma to LVGL Plugin
************************

The Figma to LVGL Plugin is available publicly in Figma's plugin store and can
be added to both public and private projects, even without a Figma subscription.

To install the Figma to LVGL Plugin, go to the Figma menu -> Plugins -> Manage
plugins and find it in the list.

.. figure:: /_static/images/figma_to_lvgl_menu.png
    :align: center
    :alt: Figma to LVGL Installation
    :scale: 75

    Click "Manage Plugins ..." to find and install Figma to LVGL

If you used it earlier, it will appear among the "Recents".

Once it's installed, a small window will appear showing the style properties of
the selected element.

.. figure:: /_static/images/figma_to_lvgl_plugin.png
    :align: center
    :alt: Figma to LVGL listing style properties of the selected UI element
    :scale: 75

    Figma to LVGL listing style properties of the selected UI element

Usage in the Editor
*******************

The style properties listed by the plugin can be copy-pasted into any XML style
section.
For example:

.. code-block:: xml

   <component>
     <styles>
        <style name="style_from_figma"
               figma_node_id="1:2" bg_color="0xfb0000"
               border_color="0x4e3535"
               border_opa="255"
               border_width="14"
               width="200"
               height="177"
               radius="13"
        />
     </styles>

     <view>
        <style name="style_from_figma" />
     </view>

   </component>

Synchronizing Style Properties
******************************

If you copied the ``figma_node_id="..."`` part from the plugin, the style in the
XML is connected to a specific element in Figma, and the Editor can download the
style properties.

To enable synchronization, add a ``project_local.xml`` file with the following
content:

.. code-block:: xml

    <project_local>
        <figma document="XXXXXX" token="YYYYY"/>
    </project_local>

where:

:XXXXXX:   is the string after "design" in the Figma URL. For example, if the Figma
           URL is ``https://www.figma.com/design/P8NM0qOqgWfYpbENhAKqYb``, then
           XXXXXX would be "P8NM0qOqgWfYpbENhAKqYb".

:YYYYY:    is the Personal Access Token that you can generate as described in the
           `Manage Personal Access Tokens <https://help.figma.com/hc/en-us/articles/
           8085703771159-Manage-personal-access-tokens>`__ article in Figma
           documentation.

Because ``project_local.xml`` contains a personal token, it should be added to
``.gitignore`` and stored only locally.

After that, clicking the FigmaSync button (top right-hand corner) will update each
style with a ``figma_node_id`` from the latest Figma design.

Using with the CLI
------------------

The :ref:`CLI tool <editor_cli>` can also synchronize styles from Figma in CI/CD:

.. code-block:: bash

   lved-cli.js figma-sync path/to/project --start-service

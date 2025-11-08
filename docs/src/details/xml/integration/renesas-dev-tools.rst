.. _editor_integration_renesas:

============================
Integrations with Renesas Dev Tools
============================

Eclipse / E2Studio
******************

Setup
-----

First, download and install the LVGL Editor from `pro.lvgl.io <https://pro.lvgl.io/download>`_.

LVGL provides an Eclipse / E2Studio plugin to facilitate working with LVGL projects. You can download it from `here <https://github.com/lvgl/lvgl_editor/releases/tag/v1.0.0>`_.

The plugin adds an LVGL Editor button to the toolbar, which opens the LVGL Editor directly within the IDE:

.. figure:: /_static/images/renesas/e2studio_toolbar_with_editor_plugin.png
    :align: center
    :alt: |lvglpro| Eclipse / E2Studio Toolbar with LVGL Editor Plugin

    ..

    |lvglpro| Eclipse / E2Studio Toolbar with LVGL Editor Plugin

This will automatically open the Editor (if installed to the `/Applications` folder) <u>on MacOS</u>. On Linux and Windows, it will ask the user to browse for the install location of the Editor. Pick the Editor executable when prompted.

NOTE: the path to the Editor executable is stored in the workspace folder in a file named `"lvgl_pro_editor_path.txt"`. If there is an issue with the path configured or you need to change it, simply delete this file and click the LVGL Editor button again to reconfigure it.

Usage
-----

The LVGL Editor plugin will attempt to determine the currently active project in the IDE.  It pick the first project that:

1. Contains a Renesas `configuration.xml` file in its root folder. Or
2. Pick the project to which the currently active file (in the code editor) belongs.

Upon first launching the LVGL Editor for a project, it will create a `ui` sub-folder for the LVGL project. Some of the LVGL configuration (such as target screen resolution) will be automatically derived from the Renesas `configuration.xml`. Subsequently launching the LVGL Editor, after the `ui` project has been creating, will trigger it to re-scan the Renesas `configuration.xml`. If any of the LVGL-related config has changed, the LVGL Editor will ask if the user wishes to update the `ui` project configuration accordingly.

VS Code
*******


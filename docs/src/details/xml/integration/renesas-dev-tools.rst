.. _editor_integration_renesas:

=======
Renesas
=======

e2 Studio
*********

Setup
-----

First, download and install the LVGL Editor from `pro.lvgl.io <https://pro.lvgl.io/download>`_.

LVGL provides an e2 Studio plugin to facilitate working with LVGL projects. You can download it from `here <https://github.com/lvgl/lvgl_editor/releases>`_.

The plugin adds an LVGL Editor button to the toolbar, which launches the LVGL Editor as a separate application, outside of e2 Studio:

.. figure:: /_static/images/renesas/e2studio_toolbar_with_editor_plugin.png
    :align: center
    :alt: e2 Studio Toolbar with LVGL Editor Plugin

    ..

    e2 Studio Toolbar with LVGL Editor Plugin

This will automatically open the Editor (if installed to the `/Applications` folder) <u>on MacOS</u>. On Linux and Windows, it will ask the user to browse for the install location of the Editor. Pick the Editor executable when prompted.

NOTE: the path to the Editor executable is stored in the workspace folder in a file named `"lvgl_pro_editor_path.txt"`. If there is an issue with the path configured or you need to change it, simply delete this file and click the LVGL Editor button again to reconfigure it.

Usage
-----

The LVGL Editor plugin will attempt to determine the currently active project in the IDE.  It picks the first project that:

1. Contains a Renesas `configuration.xml` file in its root folder. Or
2. Pick the project to which the currently active file (in the code editor) belongs.

Upon first launching the LVGL Editor for a project, it will create a `ui` sub-folder for the LVGL project. Some of the LVGL configuration (such as target screen resolution) will be automatically derived from the Renesas `configuration.xml`. Subsequently launching the LVGL Editor, after the `ui` project has been created, will trigger it to re-scan the Renesas `configuration.xml`. If any of the LVGL-related config has changed, the LVGL Editor will ask if the user wishes to update the `ui` project configuration accordingly.

Once the LVGL Editor is open, you can create and edit LVGL XML files as usual. Use the Editor's export code functionality to generate C code, which E2Studio will automatically detect and include in its build.

VS Code
*******

Setup
-----

1. Install the LVGL Editor VS Code extension from within VS Code.
2. Install the Renesas VS Code extensions that you need.

Usage
-----

The VS Code Renesas integration behaves very similarly to the e2 Studio plugin described above. To use it, add a Renesas project folder (with a `configuration.xml` file) to your workspace. Then, use the command palette to run "LVGL: Create from Renesas Project". The LVGL Editor VS Code extension will create a `ui` sub-folder for the LVGL project and derive configuration from the Renesas `configuration.xml` file.

You may then open the LVGL project by using the command palette to run "LVGL: Open Editor".

As with the e2 Studio plugin, opening the LVGL Editor extension will trigger it to re-scan the Renesas `configuration.xml` file and prompt to update the LVGL project configuration if any relevant settings have changed.

Lastly, use the LVGL Editor's export code functionality to generate C code, which the Renesas extension will automatically detect and include in its build.

Loading the UI
**************

Both the e2 studio and VSCode plugins help create a new UI project integrated with the Renesas project; however, it's the user's responsibility to initialize the UI and create the screens.

In practice you need to call these functions:

.. code-block:: c

    /* Standard LVGL initialization */
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    /* Initialize the UI generated from the LVGL Editor */
    ui_init();

    /* (replace "my_screen" with the actual name of your main screen) */
    lv_obj_t * my_screen = my_screen_create();
    lv_screen_load(my_screen);


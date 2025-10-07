.. include:: substitutions.txt
.. _editor_install:

============
Installation
============

Here are the guides to download and install |lvglpro| Editor appropriate for your platform:

Windows
*******

Navigate to https://pro.lvgl.io/#download and press the [Download] button and save the
resulting image to your workstation.  Then execute the downloaded installer to install
|lvglpro|.


Linux
*****

Installing |lvglpro| Editor on Linux is as simple as downloading the AppImage, making it
executable (``chmod +x LVGL_Pro_Editor.AppImage``), and running it.

Troubleshooting
---------------

:Problem:   Some users have experienced trouble launching Editor on some Linux
            distributions.  If you double click the ``.AppImage`` file and nothing
            happens, try running it from the terminal.  If you see this error:

            .. code-block:: bash

                (process:3943): Gtk-ERROR **: 11:45:01.301: GTK 2/3 symbols detected.
                Using GTK 2/3 and GTK 4 in the same process is not supported
                Trace/breakpoint trap (core dumped)

:Solution:  Specify the gtk version of the application with ``--gtk-version=3``.

            .. code-block:: bash

                ./path/to/editor.AppImage --gtk-version=3



MacOS
*****

Just download either the ARM for Apple Silicon (AMD) version) and drag the application
to your Applications folder in Finder.
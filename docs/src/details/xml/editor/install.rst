.. include:: substitutions.txt
.. _editor_install:

============
Installation
============

Here are the guides to download and install |lvglpro| appropriate for your platform:



Windows
*******

Navigate to https://github.com/lvgl/lvgl_editor/, scan down the right panel and
find "Releases".

If the version you want to download is showing, click it to go to the downloads page.
If not, click on "+ N releases", then find and click on the release you want to
download.

Note:  this method will change as |lvglpro| reaches commercial-release stage.

Once on the desired download page, right click on the file appropriate for your
platform and select "Save Link As...", and save it on your workstation.

Extract the file(s) from the compressed archive file and execute the installer to
install |lvglpro|.  On Windows, that means launch the ``LVGL Pro Setup
x.x.x.exe`` installer.


Troubleshooting
---------------



Linux
*****

TODO Gabor


Troubleshooting
---------------

Launching on Linux
~~~~~~~~~~~~~~~~~~

:Problem:   Some users have experienced trouble launching |lvglpro| on some Linux

            distributions.  If you double click the ``.AppImage`` file and nothing
            happens, try running it from the terminal.  If you see this error:

            .. code-block:: bash

                (process:3943): Gtk-ERROR \*\*: 11:45:01.301: GTK 2/3 symbols detected.
                Using GTK 2/3 and GTK 4 in the same process is not supported
                Trace/breakpoint trap (core dumped)

:Solution:  Specify the gtk version of the application with ``--gtk-version=3``.

            .. code-block:: bash

                ./path/to/editor.AppImage --gtk-version=3



MacOS
*****

TODO Gabor


Troubleshooting
---------------

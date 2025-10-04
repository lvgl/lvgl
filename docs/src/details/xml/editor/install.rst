.. include:: substitutions.txt
.. _editor_install:

============
Installation
============

Here are the guides to download and install |lvglpro| Editor appropriate for your
platform:

Windows
*******

Install WSL
-----------

Before using the Editor ``WSL`` (Windows Subsystem for Linux) needs to be installed.

Open a Terminal and type ``wsl.exe --list --verbose`` to see if it's already installed.

If it's not, just type ``wsl --install`` and hit Enter.

Install the Editor
------------------

Navigate to https://pro.lvgl.io/#download and press the [Download] button and save
the resulting image to your workstation. Then execute the downloaded installer to
install |lvglpro| Editor.

Linux
*****

Install Podman
--------------

The Editor is capable of recompiling the preview to use and run your C code as well.
We handle the compilation in a container using `Podman <https://podman.io/>`__
(similar to Docker).

The simple way of installing Podman is via Terminal:

- **Debian, Ubuntu, Linux Mint**: ``sudo apt-get install podman``
- **Fedora**: ``sudo dnf -y install podman``
- **Arch, Manjaro Linux**: ``sudo pacman -S podman``

For other distos check out https://podman.io/docs/installation#installing-on-linux

Install the Editor
------------------

Installing |lvglpro| Editor on Linux is as simple as downloading the AppImage, making
it executable (``chmod +x LVGL_Pro_Editor.AppImage``), and running it.

Troubleshooting
---------------

:Problem:   If you double click the ``.AppImage`` file and nothing happens, try
            running it from the terminal. If you see this error:

            .. code-block:: bash

                (process:3943): Gtk-ERROR **: 11:45:01.301: GTK 2/3 symbols detected.
                Using GTK 2/3 and GTK 4 in the same process is not supported
                Trace/breakpoint trap (core dumped)

:Solution:  Specify the GTK version for the application with ``--gtk-version=3``.

            .. code-block:: bash

                ./path/to/editor.AppImage --gtk-version=3

MacOS
*****

Install Podman
--------------

The Editor is capable of recompiling the preview to use and run your C code as well.
We handle the compilation in a container using `Podman <https://podman.io/>`__
(similar to Docker). The simplest way to install Podman is via ``brew`` using
``brew install podman``.

For more information visit https://podman.io/docs/installation#macos

Install the Editor
------------------

Just download either the ARM (for Apple Silicon) or AMD version and drag the
application to your Applications folder in Finder.

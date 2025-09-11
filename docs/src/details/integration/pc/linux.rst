=====
Linux
=====

Overview
********

LVGL comes with a lot of built-in Linux-related drivers and support:

- Wayland, DRM, SDL, and fbdev display drivers
- ``pthread`` integration
- :ref:`build_cmake` support

Multiple repositories are available to help you get started easily.
Just follow the README of the repositories listed below.

Ready-to-Use Projects
*********************

VSCode
------

LVGL is available for `VSCode <https://code.visualstudio.com/>`__, a well-known cross-platform code editor.

It uses `SDL <https://www.libsdl.org/>`__ to open a window, show the rendered content, and manage mouse and keyboard.

A ready-to-use LVGL project for VSCode is available at
`https://github.com/lvgl/lv_port_pc_vscode <https://github.com/lvgl/lv_port_pc_vscode>`__.

The project uses :ref:`build_cmake` to compile LVGL, so it also serves as a CMake example.

This repository also has built-in FreeRTOS integration, making it easy to test the UI
with an embedded OS.

Generic Linux Port
------------------

The `lv_port_linux <https://github.com/lvgl/lv_port_linux>`__ project is typically meant to
be used for embedded hardware, but it runs perfectly on PC as well.

It supports all the built-in LVGL drivers in a preconfigured :ref:`build_cmake` build system,
making it easy to integrate into any platform or environment.

Eclipse
-------

The project for `Eclipse CDT <https://projects.eclipse.org/projects/tools.cdt>`__ also uses `SDL <https://www.libsdl.org/>`__.
It can be found at
`https://github.com/lvgl/lv_port_pc_eclipse <https://github.com/lvgl/lv_port_pc_eclipse>`__.

The project uses CMake to compile LVGL, so it also serves as a :ref:`build_cmake` example.

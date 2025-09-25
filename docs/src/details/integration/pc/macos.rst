=====
macOS
=====

Overview
********

Ready-to-use projects are available to run LVGL on macOS as well.
These projects use the :ref:`sdl_driver`.

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
with an embedded OS as well.

Eclipse
-------

The project for `Eclipse CDT <https://projects.eclipse.org/projects/tools.cdt>`__ also uses `SDL <https://www.libsdl.org/>`__.
It can be found at
`https://github.com/lvgl/lv_port_pc_eclipse <https://github.com/lvgl/lv_port_pc_eclipse>`__.

The project uses CMake to compile LVGL, so it also serves as a :ref:`build_cmake` example.

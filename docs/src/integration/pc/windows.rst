=======
Windows
=======

Overview
********

LVGL comes with built-in drivers for Windows, covering:

- Display, mouse, and keyboard support
- Operating system drivers

Using these or other drivers, multiple repositories are available to help you get started easily.
Just follow the README of the repositories listed below.

Ready-to-Use Projects
*********************

VSCode
------

LVGL is available for `VSCode <https://code.visualstudio.com/>`__, a well-known cross-platform code editor.

It uses `SDL <https://www.libsdl.org/>`__ to open a window, show the rendered content, and manage mouse and keyboard.

A ready-to-use LVGL project for VSCode is available at
https://github.com/lvgl/lv_port_pc_vscode\ .

The project uses :ref:`build_cmake` to compile LVGL, so it also serves as a CMake example.

This repository also includes built-in FreeRTOS integration, making it easy to test the UI
with an embedded OS.

Visual Studio
-------------

`Visual Studio <https://visualstudio.microsoft.com/>`__ is a comprehensive IDE from Microsoft
containing all the tools required for development out of the box.

To use LVGL in Visual Studio, visit the
`lvgl/lv_port_pc_visual_studio <https://github.com/lvgl/lv_port_pc_visual_studio>`__ repository.

It uses LVGL's built-in Windows drivers to open a window where you can see the LVGL-rendered UI,
and use your mouse as a touch input replacement.

Eclipse
-------

The project for `Eclipse CDT <https://projects.eclipse.org/projects/tools.cdt>`__ also uses `SDL <https://www.libsdl.org/>`__.
It can be found at
`https://github.com/lvgl/lv_port_pc_eclipse <https://github.com/lvgl/lv_port_pc_eclipse>`__.

The project uses :ref:`build_cmake` to compile LVGL and serves as a CMake example.

Windows Driver
**************

It's also easy to port LVGL to any Windows IDEs or toolchains using either the built-in SDL driver or
the Windows driver.

To learn more about the SDL driver, visit :ref:`sdl_driver`.

Details of the Windows driver are provided below.

The **Windows** display/input `driver <https://github.com/lvgl/lvgl/tree/master/src/drivers/windows>`__
offers support for simulating the LVGL display and keyboard/mouse inputs in a Windows Win32 window.

Modes
-----

The main purpose of this driver is for testing/debugging LVGL applications in
a **Windows** simulation window via **simulator mode**, or developing standard **Windows** desktop
applications with LVGL via **application mode**.

Both simulator mode and application mode:

- Support LVGL pointer, keypad, and encoder device integration
- Support Windows touch input
- Support Windows input method integration
- Support per-monitor DPI awareness (both V1 and V2)
- Provide HWND-based interoperability for other Windows UI infrastructures

However, **Simulator Mode**:

- is designed for simulating LVGL rendering on hardware-like displays,
- keeps LVGL display resolution constant to simulate production UI layouts, and
- uses Windows DPI scaling to stretch content,

whereas **Application Mode**:

- is designed for native Windows desktop application development,
- supports dynamic window resizing with corresponding changes to LVGL display resolution,
- updates LVGL display DPI when Windows DPI changes,
- uses window size (not client area) when setting resolution via :cpp:func:`lv_windows_create_display`, and
- requires apps to handle display resolution changes properly.

The mode can be selected by calling :cpp:func:`lv_windows_create_display`.

Prerequisites
-------------

The minimum Windows OS requirement for this driver is Windows Vista RTM.

If you use Windows API shim libraries like `YY-Thunks <https://github.com/Chuyu-Team/YY-Thunks>`__,
the tested minimum OS version is Windows XP RTM.

Due to the use of Windows GDI APIs, the theoretical minimum may be Windows 2000 RTM.

Usage
-----

Enable Windows driver support in ``lv_conf.h``, via CMake compiler define, or KConfig:

.. code-block:: c

    #define LV_USE_WINDOWS  1

Once enabled, the Windows driver can be used like this:

.. code-block:: c

    #include <Windows.h>
    #include "lvgl/lvgl.h"
    #include "lvgl/examples/lv_examples.h"
    #include "lvgl/demos/lv_demos.h"

    int main()
    {
        lv_init();

        int32_t zoom_level = 100;
        bool allow_dpi_override = false;
        bool simulator_mode = false;
        lv_display_t* display = lv_windows_create_display(
            L"LVGL Display Window",
            800, 480,
            zoom_level, allow_dpi_override, simulator_mode);

        if (!display) return -1;

        lv_lock();

        lv_indev_t* pointer_device = lv_windows_acquire_pointer_indev(display);
        if (!pointer_device) return -1;

        lv_indev_t* keypad_device = lv_windows_acquire_keypad_indev(display);
        if (!keypad_device) return -1;

        lv_indev_t* encoder_device = lv_windows_acquire_encoder_indev(display);
        if (!encoder_device)  return -1;

        lv_demo_widgets();

        lv_unlock();

        while (1)
        {
            uint32_t time_till_next = lv_timer_handler();
            // handle LV_NO_TIMER_READY. Another option is to always sleep a few milliseconds
            if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD;
            lv_sleep_ms(time_till_next);
        }

        return 0;
    }

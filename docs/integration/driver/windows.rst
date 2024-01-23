=============================
Windows Display/Inputs driver
=============================

Overview
-------------

The **Windows** display/input `driver <https://github.com/lvgl/lvgl/src/drivers/windows>`__ offers support for simulating the LVGL display and keyboard/mouse inputs in a Windows Win32 window.

The main purpose for this driver is for testing/debugging the LVGL application in a **Windows** simulation window via **simulator mode**, or developing a standard **Windows** desktop application with LVGL via **application mode**.

Here are the **similarity** for simulator mode and application mode.

- Support LVGL pointer, keypad and encoder devices integration.
- Support Windows touch input.
- Support Windows input method integration input.
- Support Per-monitor DPI Aware (both V1 and V2).

Here are the **differences** for simulator mode and application mode.

Simulator Mode
^^^^^^^^^^^^^^

- Designed for LVGL simulation scenario.
- Keep the LVGL display resolution all time for trying best to simulate UI layout which will see in their production devices.
- When Windows DPI scaling setting is changed, Windows backend will stretch the display content.

Application Mode
^^^^^^^^^^^^^^^^

- Designed for Windows desktop application development scenario.
- Have the Window resizing support and LVGL display resolution will be changed.
- When Windows DPI scaling setting is changed, the LVGL display DPI value will also be changed.

Prerequisites
-------------

The tested minimum Windows OS requirement for this driver is Windows XP RTM.

According to the Windows GDI API this driver used. Maybe the minimum Windows OS requirement for this driver is Windows 2000 RTM.

Configure Windows driver
--------------------

Enable the Windows driver support in lv_conf.h, by cmake compiler define or by KConfig

.. code:: c

    #define LV_USE_WINDOWS  1

Usage
-----

.. code:: c

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
            800,
            480,
            zoom_level,
            allow_dpi_override,
            simulator_mode);
        if (!display)
        {
            return -1;
        }

        lv_indev_t* pointer_device = lv_windows_acquire_pointer_indev(display);
        if (!pointer_device)
        {
            return -1;
        }

        lv_indev_t* keypad_device = lv_windows_acquire_keypad_indev(display);
        if (!keypad_device)
        {
            return -1;
        }

        lv_indev_t* encoder_device = lv_windows_acquire_encoder_indev(display);
        if (!encoder_device)
        {
            return -1;
        }

        lv_demo_widgets();

        while (1)
        {
            uint32_t time_till_next = lv_timer_handler();
            Sleep(time_till_next);
        }

        return 0;
    }

.. _sdl_driver:

==========
SDL Driver
==========

Overview
********

`SDL <https://www.libsdl.org/>`__ (Simple DirectMedia Layer) provides a cross-platform way to handle graphics,
input, and multimedia, making it an excellent choice for running LVGL applications on
a PC.

The `Eclipse <https://github.com/lvgl/lv_port_pc_eclipse>`__,
`VSCode <https://github.com/lvgl/lv_port_pc_vscode>`__, and `Generic Linux <https://github.com/lvgl/lv_port_linux>`__
projects of LVGL use SDL to open a window and manage input devices.

As SDL is fully cross-platform, it is easy to integrate into any environment.

Prerequisites
*************

Install SDL according to your platform:

- Linux: ``sudo apt install libsdl2-dev``
- macOS: ``brew install sdl2``
- Windows: Recommended: ``vcpkg install sdl2``  
  Alternatively, you can download prebuilt SDL2 binaries from the `SDL website <https://www.libsdl.org/download-2.0.php>`__ and follow their installation instructions.

Configuration
*************

1. Required linked libraries: ``-lSDL2``
2. Enable SDL driver support in ``lv_conf.h``, via CMake compiler definitions, or by using KConfig.

.. code-block:: c

    #define LV_USE_SDL  1

Feel free to adjust the other options as needed, but the default values are usually sufficient.

Usage
*****

.. code-block:: c

    #define SDL_MAIN_HANDLED        /* To fix SDL's "undefined reference to WinMain" issue */
    #include "lvgl/lvgl.h"

    static lv_display_t *display;
    static lv_indev_t *mouse;
    static lv_indev_t *mouse_wheel;
    static lv_indev_t *keyboard;

    int main()
    {
        /* Initialize LVGL */
        lv_init();

        display = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);
        mouse = lv_sdl_mouse_create();
        mouse_wheel = lv_sdl_mousewheel_create();
        keyboard = lv_sdl_keyboard_create();

        /* Create widgets on the screen */
        lv_demo_widgets();

        while (1) {
            lv_timer_handler();
            lv_delay_ms(5);
        }

        return 0;
    }

When building for 32-bit architecture, add the following workaround at the beginning of ``main``:

.. code-block:: c

    // Workaround for SDL2 `-m32` crash
    // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
    #ifndef WIN32
        setenv("DBUS_FATAL_WARNINGS", "0", 1);
    #endif

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



******************
Rendering Backends
******************

The SDL driver supports multiple rendering backends that determine how LVGL renders content to the screen.
Each backend offers different performance characteristics and use cases.


Standard Software Rendering (Default)
======================================

By default, the SDL driver uses pure software rendering with no additional configuration required.

**Configuration:**

No additional configuration needed - this is the default when only ``LV_USE_SDL 1`` is enabled.

SDL Draw Unit
=============

The SDL Draw Unit uses software rendering to create SDL textures which are then cached and blended together by the GPU,
providing a hybrid approach that combines software rendering flexibility with hardware-accelerated texture composition.

**Configuration:**

.. code-block:: c

    #define LV_USE_SDL 1
    #define LV_USE_DRAW_SDL 1

For complete details, see :ref:`sdl_draw_unit`.


OpenGL-Based Rendering
=======================

The SDL driver can leverage OpenGL for hardware-accelerated rendering. LVGL provides two OpenGL-based rendering options:

- :ref:`NanoVG Draw Unit <nanovg_draw_unit>`
- :ref:`OpenGL Draw Unit <opengles_draw_unit>`


.. code-block:: c

    #define LV_USE_SDL 1
    #define LV_USE_OPENGLES 1

    /* For NanoVG Draw Unit*/
    #define LV_USE_DRAW_NANOVG 1 
    #define LV_USE_NANOVG 1

    /* For OpenGL Draw Unit */
    #define LV_USE_DRAW_OPENGLES 1 

See the :ref:`Complete OpenGL overview <opengl_overview>` for more information. 



********
See Also
********

- :ref:`sdl_draw_unit` - SDL texture-based rendering
- :ref:`opengl_overview` - Complete OpenGL integration overview
- :ref:`nanovg_draw_unit` - NanoVG vector graphics rendering
- :ref:`opengles_draw_unit` - OpenGL ES for embedded systems


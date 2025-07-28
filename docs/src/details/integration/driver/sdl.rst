.. _sdl_driver:

===============================
SDL Driver
===============================

Overview
--------

| SDL (Simple DirectMedia Layer) provides a cross-platform way to handle graphics, input, and multimedia, making it an excellent choice for running LVGL applications on a PC.


Prerequisites
-------------

Install SDL according to your platform.

- Linux ``sudo apt install libsdl2-dev``
- MacOS ``brew install sdl2``
- Windows https://github.com/libsdl-org/SDL/releases


Configure SDL Driver
-----------------------

1. Required linked libraries: -lSDL2
2. Enable SDL driver support in lv_conf.h, CMake compiler definitions or KConfig.

    .. code-block:: c

        #define LV_USE_SDL  1
        #define LV_SDL_INCLUDE_PATH <SDL2/SDL.h>
        #define SDL_HOR_RES 400
        #define SDL_VER_RES 400

Basic Usage
-----------

.. code-block:: c

    #include <unistd.h>
    #define SDL_MAIN_HANDLED        /*To fix SDL's "undefined reference to WinMain" issue*/
    #include <SDL2/SDL.h>
    #include "drivers/sdl/lv_sdl_mouse.h"
    #include "drivers/sdl/lv_sdl_mousewheel.h"
    #include "drivers/sdl/lv_sdl_keyboard.h"

    static lv_display_t *lvDisplay;
    static lv_indev_t *lvMouse;
    static lv_indev_t *lvMouseWheel;
    static lv_indev_t *lvKeyboard;

    #if LV_USE_LOG != 0
    static void lv_log_print_g_cb(lv_log_level_t level, const char * buf)
    {
        LV_UNUSED(level);
        LV_UNUSED(buf);
    }
    #endif

    int main()
    {
        /* initialize lvgl */
        lv_init();

        // Workaround for sdl2 `-m32` crash
        // https://bugs.launchpad.net/ubuntu/+source/libsdl2/+bug/1775067/comments/7
        #ifndef WIN32
            setenv("DBUS_FATAL_WARNINGS", "0", 1);
        #endif

        /* Register the log print callback */
        #if LV_USE_LOG != 0
        lv_log_register_print_cb(lv_log_print_g_cb);
        #endif

        /* Add a display
        * Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/

        lvDisplay = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);
        lvMouse = lv_sdl_mouse_create();
        lvMouseWheel = lv_sdl_mousewheel_create();
        lvKeyboard = lv_sdl_keyboard_create();

        /* create Widgets on the screen */
        lv_demo_widgets();


        Uint32 lastTick = SDL_GetTicks();
        while(1) {
            SDL_Delay(5);
            Uint32 current = SDL_GetTicks();
            lv_tick_inc(current - lastTick); // Update the tick timer. Tick is new for LVGL 9
            lastTick = current;
            lv_timer_handler(); // Update the UI-
        }

        return 0;
    }

Using an IDE
------------

LVGL with SDL has been ported to various IDEs.

- `Eclipse with SDL driver <https://github.com/lvgl/lv_sim_eclipse_sdl>`__: Recommended on Linux and Mac, supports CMake as well
- `VSCode with SDL driver <https://github.com/lvgl/lv_port_pc_vscode>`__: Recommended on Linux (SDL) and Mac (SDL)
- `Generic Linux <https://github.com/lvgl/lv_port_linux>`__: CMake based project where you can easily switch between fbdev, DRM, and SDL.

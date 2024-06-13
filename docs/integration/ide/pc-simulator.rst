.. _simulator:

===============
Simulator on PC
===============

You can try out LVGL **using only your PC** (i.e. without any
development boards). LVGL will run on a simulator environment on the PC
where anyone can write and experiment with real LVGL applications.

Using the simulator on a PC has the following advantages:

- Hardware independent: Write code, run it on the PC and see the result on a monitor.
- Cross-platform: Any Windows, Linux or macOS system can run the PC simulator.
- Portability: The written code is portable, which means you can simply copy it when migrating to embedded hardware.
- Easy Validation: The simulator is also very useful to report bugs because it
  provides a common platform for every user.
- Better developer experience: On PC Debuggers are usually faster and better, you can log to files,
  add a lot of ``printf``-s, do profiling, and so on.


Select an IDE
-------------

The simulator is ported to various IDEs (Integrated Development Environments).
Choose your favorite IDE, read its README on GitHub, download the project, and load it to the IDE.

- `Eclipse with SDL driver <https://github.com/lvgl/lv_sim_eclipse_sdl>`__: Recommended on Linux and Mac, supports CMake too
- `VisualStudio <https://github.com/lvgl/lv_port_pc_visual_studio>`__: Recommended on Windows
- `VSCode with SDL driver <https://github.com/lvgl/lv_port_pc_vscode>`__: Recommended on Linux (SDL) and Mac (SDL)
- `CodeBlocks <https://github.com/lvgl/lv_sim_codeblocks_win>`__: Recommended on Windows
- `PlatformIO with SDL driver <https://github.com/lvgl/lv_platformio>`__: Recommended on Linux and Mac but has an STM32 environment too
- `Generic Linux <https://github.com/lvgl/lv_port_linux>`__: CMake based project where you can easily switch between fbdev, DRM, and SDL.
- `MDK with FastModel <https://github.com/lvgl/lv_port_an547_cm55_sim>`__: For Windows

External project not maintained by the LVGL organization:

- `QT Creator <https://github.com/Varanda-Labs/lvgl-qt-sim>`__: Cross platform

Built-in drivers
----------------

LVGL comes with several `built-in drivers <https://docs.lvgl.io/master/integration/driver/index.html>`__.

Even if a simulator project comes with e.g. SDL, you can easily replace it by enabling
another driver in ``lv_conf.h`` and calling its ``create`` function.

For example to use the Linux frame buffer device instead of SDL just enable ``LV_USE_LINUX_FBDEV``
and call

.. code:: c

   lv_display_t *display = lv_linux_fbdev_create();
   lv_linux_fbdev_set_file(display, "/dev/fb0")

.. _integration_index:

===========
Integration
===========


LVGL is a bunch of C and header files that you can easily add and compile
in your project.

Writing a custom display and input device from scratch is also easy,
but you can also pick from ready-to-use drivers for LCD peripheries
(:ref:`ST's LTDC <stm32_ltdc_driver>`, :ref:`NXP's eLCDIF <nxp_elcdif>`, etc.),
external display controllers (e.g. :ref:`ILI9341 <ili9341>`),
:ref:`SDL <sdl_driver>`, :ref:`OpenGL <opengl_driver>`,
:ref:`Wayland <wayland_driver>`, and other :ref:`Embedded Linux <embedded_linux>`
frameworks.

All the mainstream RTOS and desktop operating systems are also supported,
including :ref:`FreeRTOS <freertos>`, :ref:`Zephyr <zephyr>`,
:ref:`NuttX <nuttx>`, :ref:`Windows <windows_pc>`, :ref:`Linux <linux_pc>`,
:ref:`macOS <macos_pc>`, etc.

If you are looking for an :ref:`Arduino <arduino>`,
:ref:`ESP-IDF <espressif>`, or :ref:`PlatformIO <platformio>`
guide, you are also covered.

You can also pick a `ready-to-use project <https://lvgl.io/boards>`__ for many
development boards.

.. toctree::
    :maxdepth: 2

    overview
    pc/index
    embedded_linux/index
    rtos/index
    frameworks/index
    boards/index
    chip_vendors/index
    external_display_controllers/index
    building/index
    bindings/index


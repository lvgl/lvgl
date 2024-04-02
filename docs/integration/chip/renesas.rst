=======
Renesas
=======

`Renesas <https://renesas.com/>`__ is an official partner of LVGL.
Therefore, LVGL contains built-in support for `Dave2D <https://lpccs-docs.renesas.com/DA1470x/UM-B-157_DA1470x-GPU-API-Manual/files/doc/overview-txt.html>`__ (the GPU of Renesas)
and LVGL also hosts ready-to-use Renesas projects.

Dave2D
------

Dave2D is capable of accelerating most of the drawing operations of LVGL:

- Rectangle drawing, even with gradients
- Image drawing, scaling, and rotation
- Letter drawing
- Triangle drawing
- Line drawing

As Dave2D works in the background, the CPU is free for other tasks. In practice, during rendering, Dave2D can reduce the CPU usage by half or to one-third, depending on the application.

Certified boards
----------------

LVGL has `certified <https://lvgl.io/certificate>`__ one Renesas board so far (more will come soon).

.. raw:: html

  <iframe width="560" height="315" src="https://www.youtube.com/embed/LHPIqBV_MGA?si=mtW3g-av56bCdR4k" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

Get started with the Renesas ecosystem
--------------------------------------

The official IDE of Renesas is called `e² studio <https://www.renesas.com/us/en/software-tool/e-studio?gad_source=1&gclid=CjwKCAjw5ImwBhBtEiwAFHDZx2V3lumaenbyJnc5Ctrclr_lEQM3G22iZgB-4F92OVLCI7xmzp1YQRoCcRgQAvD_BwE>`__. As it's Eclipse-based, it runs on Windows, Linux, and Mac as well.

To get started, just download and install e² studio.

Getting started with LVGL
-------------------------

LVGL provides a ready-to-use project for the `EK-RA8D1 <https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group>`__ development board. Its main features from the HMI's point of view are:

- 480MHz, Arm Cortex®-M85 core
- 2MB Code Flash, 1MB SRAM
- MIPI DSI & Parallel Graphics Expansion Ports
- 4.5 Inch backlit TFT display, 16.7M display colors
- 480x854 pixels resolution

To get a ready-to-use project, clone the [lv_renesas](https://github.com/lvgl/lv_renesas.git) repository:

.. code:: shell
  git clone https://github.com/lvgl/lv_renesas.git --recurse-submodules

After that, *Import* ``lv_ek_ra8d1`` into e² studio, build the project, and flash it.

Note that on the ``SW1`` DIP switch (middle of the board) 7 should be ON, all others are OFF.

Modify the project
------------------

Open a demo
~~~~~~~~~~~

In `LVGL_thread_entry <https://github.com/lvgl/lv_renesas/blob/master/lv_ek_ra8d1/src/LVGL_thread_entry.c>`__, the demos are automatically enabled based on the settings in `lv_conf.h <https://github.com/lvgl/lv_renesas/blob/master/lv_ek_ra8d1/src/lv_conf.h>`__.

You can disable all demos (or just comment them out) and call some ``lv_example_...()`` functions, or add your custom code.

Configuration
~~~~~~~~~~~~~

``lv_conf.h`` contains the most important settings for LVGL. Namely:

- ``LV_COLOR_DEPTH`` to set LVGL's default color depth
- ``LV_MEM_SIZE to`` set the maximum RAM available for LVGL
- ``LV_USE_DAVE2D`` to enable the GPU


``configuration.xml`` contains the settings for the board and the MCU. By opening this file, all the hardware and software components can be customized in a visual way.


Support
-------

In case of an problems or questions open an issue in the `lv_renesas <https://github.com/lvgl/lv_renesas/issues>`__ repository.

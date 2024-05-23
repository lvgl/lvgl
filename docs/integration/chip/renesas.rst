.. _renesas:

=======
Renesas
=======

`Renesas <https://renesas.com/>`__ is an official partner of LVGL.
Therefore, LVGL contains built-in support for `Dave2D <https://www.renesas.com/document/mas/tes-dave2d-driver-documentation>`__ (the GPU of Renesas)
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

GLCDC
-----

GLCDC is a multi-stage graphics output peripheral available in several Renesas MCUs.
It is able to drive LCD panles via a higly configurable RGB interface.

More info can be found at the :ref:`dirver's page<renesas_glcdc>`.

Certified boards
----------------

LVGL has `certified <https://lvgl.io/certificate>`__ one Renesas board so far (more will come soon).

.. raw:: html

  <iframe width="560" height="315" src="https://www.youtube.com/embed/LHPIqBV_MGA?si=mtW3g-av56bCdR4k" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

Get started with the Renesas ecosystem
--------------------------------------

The official IDE of Renesas is called `e² studio <https://www.renesas.com/us/en/software-tool/e-studio?gad_source=1&gclid=CjwKCAjw5ImwBhBtEiwAFHDZx2V3lumaenbyJnc5Ctrclr_lEQM3G22iZgB-4F92OVLCI7xmzp1YQRoCcRgQAvD_BwE>`__. As it's Eclipse-based, it runs on Windows, Linux, and Mac as well.

To get started, just download and install e² studio.

JLink is used for debugging, it can be downloaded [here](https://www.segger.com/downloads/jlink/)

Getting started with LVGL
-------------------------

LVGL provides a ready-to-use project for the `EK-RA8D1 <https://www.renesas.com/us/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra8d1-evaluation-kit-ra8d1-mcu-group>`__ development board. Its main features from the HMI's point of view are:

- 480MHz, Arm Cortex®-M85 core
- 2MB Code Flash, 1MB SRAM
- MIPI DSI & Parallel Graphics Expansion Ports
- 4.5 Inch backlit TFT display, 16.7M display colors
- 480x854 pixels resolution

**Setting up the project**

- First, clone the ready-to-use `lv_port_renesas_ek-ra8d1 <https://github.com/lvgl/lv_port_renesas_ek-ra8d1.git>`__ repository:

   .. code-block:: shell

      git clone https://github.com/lvgl/lv_port_renesas_ek-ra8d1.git --recurse-submodules

- Open e² studio, go to ``File`` -> ``Import project`` and select ``General`` / ``Exsisting projects into workspace``

   .. image:: /misc/renesas/import.png
      :alt: Importing the project
  
  
- Browse the cloned folder and press ``Finish``

- Double click on ``configuration.xml``. This will activate the configuration window.

  Renesas' Flexible Software Package (FSP) incudes BSP and HAL layer support extended with multiple RTOS variants and other middleware stacks.
  The components will be available via code generation, incuding the entry point of *"main.c"*.

  Press ``Generate Project Content`` in the top right corner.

   .. image:: /misc/renesas/generate.png
      :alt: Code generation with FSP

- Build the project by pressing ``Ctrl`` + ``Alt`` + ``B``

- Click the Debug button. When prompted select the `J-Link ARM` Debugger and the `R7FA8D1BH` MCU.

Note that on the ``SW1`` DIP switch (middle of the board) 7 should be ON, all others are OFF.

Modify the project
------------------

Open a demo
~~~~~~~~~~~

In `LVGL_thread_entry <https://github.com/lvgl/lv_port_renesas_ek-ra8d1/blob/master/src/LVGL_thread_entry.c>`__, the demos are automatically enabled based on the settings in `lv_conf.h <https://github.com/lvgl/lv_port_renesas_ek-ra8d1/blob/master/src/lv_conf.h>`__.

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

In case of an problems or questions open an issue in the `lv_port_renesas_ek-ra8d1 <https://github.com/lvgl/lv_port_renesas_ek-ra8d1/issues>`__ repository.

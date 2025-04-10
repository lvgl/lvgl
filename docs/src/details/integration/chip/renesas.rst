.. _renesas:

=======
Renesas
=======

.. |sup2|   unicode:: U+000B2 .. SUPERSCRIPT TWO

`Renesas <https://renesas.com/>`__ is an official partner of LVGL. Therefore, LVGL contains built-in support for
`Dave2D <https://www.renesas.com/document/mas/tes-dave2d-driver-documentation>`__ (the GPU of Renesas) and we also maintain
ready-to-use Renesas projects.


Dave2D
------

Dave2D is capable of accelerating most of the drawing operations of LVGL:

- Rectangle drawing, even with gradients
- Image drawing, scaling, and rotation
- Letter drawing
- Triangle drawing
- Line drawing


As Dave2D works in the background, the CPU is free for other tasks. In practice, during rendering, Dave2D can reduce the CPU usage by
half or to one-third, depending on the application.


GLCDC
-----

GLCDC is a multi-stage graphics output peripheral available in several Renesas MCUs. It is able to drive LCD panels via a highly
configurable RGB interface.

More info can be found at the :ref:`driver's page<renesas_glcdc>`.


Supported boards
----------------

.. list-table::
   :widths: 10 30 30 30

   * -
     - **EK-RA8D1**
     - **EK-RA6M3G**
     - **RX72N Envision Kit**
   * - CPU
     - 480MHz, Arm Cortex-M85 core
     - 120MHz, Arm Cortex-M4 core
     - 240MHz, Renesas RXv3 core
   * - Memory
     -
         | 1MB internal, 64MB external SDRAM
         | 2MB internal, 64MB External Octo-SPI Flash
     -
         | 640kB internal SRAM
         | 2MB internal, 32MB external QSPI Flash
     -
         | 1MB internal SRAM
         | 4MB internal, 32MB external QSPI Flash
   * - Display
     -
         | 4.5”
         | 480x854
         | 2-lane MIPI
     -
         | 4.3”
         | 480x272
         | Parallel RGB565
     -
         | 4.3”
         | 480x272
         | Parallel RGB565
   * - `Board <https://lvgl.io/boards>`__ video
     - .. raw:: html

           <iframe width="320" height="180" src="https://www.youtube.com/embed/WkJPB8wto_U" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

     - .. raw:: html

           <iframe width="320" height="180" src="https://www.youtube.com/embed/0kar4Ee3Qic" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

     - .. raw:: html

           <iframe width="320" height="180" src="https://www.youtube.com/embed/__56v8DsfH0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

   * - Links
     - `Demo repository for EK-RA8D1 <https://github.com/lvgl/lv_port_renesas_ek-ra8d1>`__
     - `Demo repository for EK-RA6M3G <https://github.com/lvgl/lv_port_renesas_ek-ra6m3g>`__
     - `Demo repository for RX72N Envision Kit <https://github.com/lvgl/lv_port_renesas_rx72n-envision-kit>`__


Get started with the Renesas ecosystem
--------------------------------------

.. |img_debug_btn| image:: /_static/images/renesas/debug_btn.png
   :alt: Debug button

.. dropdown:: RA Family

    - The official IDE of Renesas is called e\ |sup2| studio. As it's Eclipse-based, it runs on Windows, Linux, and Mac as well.
      The RA family requires the latest version with FSP 5.3. It can be downloaded `here <https://www.renesas.com/us/en/software-tool/flexible-software-package-fsp>`__.
    - JLink is used for debugging, it can be downloaded `here <https://www.segger.com/downloads/jlink/>`__.
    - Clone the ready-to-use repository for your selected board:

        .. code-block:: shell

            git clone https://github.com/lvgl/lv_port_renesas_ek-ra8d1.git --recurse-submodules


        Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.
    - Open e\ |sup2| studio, go to ``File`` -> ``Import project`` and select ``General`` / ``Existing projects into workspace``
    - Browse the cloned folder and press ``Finish``.
    - Double click on ``configuration.xml``. This will activate the `Configuration Window`.

      Renesas' Flexible Software Package (FSP) includes BSP and HAL layer support extended with multiple RTOS variants and other middleware stacks.
      The components will be available via code generation, including the entry point of *"main.c"*.

      Press ``Generate Project Content`` in the top right corner.

      .. image:: /_static/images/renesas/generate.png
         :alt: Code generation with FSP

    - Build the project by pressing ``Ctrl`` + ``Alt`` + ``B``
    - Click the Debug button (|img_debug_btn|). If prompted with `Debug Configurations`, on the `Debugger` tab select the ``J-Link ARM`` as `Debug hardware` and the proper IC as `Target Device`:

      - ``R7FA8D1BH`` for EK-RA8D1

        .. image:: /_static/images/renesas/debug_ra8.png
           :alt: Debugger parameters for RA8

      - ``R7FA6M3AH`` for EK-RA6M3G

        .. image:: /_static/images/renesas/debug_ra6.png
            :alt: Debugger parameters for RA6

   .. note::
      On EK-RA8D1 boards, the ``SW1`` DIP switch (middle of the board) 7 should be ON, all others are OFF.

.. dropdown:: RX Family

    - The official IDE of Renesas is called e\ |sup2| studio. As it's Eclipse-based, it runs on Windows, Linux, and Mac as well.
      It can be downloaded `here <https://www.renesas.com/us/en/software-tool/e-studio>`__.
    - Download and install the required driver for the debugger

       - for Windows: `64 bit here <https://www.renesas.com/us/en/document/uid/usb-driver-renesas-mcu-tools-v27700-64-bit-version-windows-os?r=488806>`__
         and `32 bit here <https://www.renesas.com/us/en/document/uid/usb-driver-renesas-mcu-toolse2e2-liteie850ie850apg-fp5-v27700for-32-bit-version-windows-os?r=488806>`__
       - for Linux: `here <https://www.renesas.com/us/en/document/swo/e2-emulator-e2-emulator-lite-linux-driver?r=488806>`__

    - RX72 requires an external compiler for the RXv3 core. A free and open-source version is available
      `here <https://llvm-gcc-renesas.com/rx-download-toolchains/>`__ after a registration.

      The compiler must be activated in e\ |sup2| studio:

      - Go to go to ``Help`` -> ``Add Renesas Toolchains``
      - Press the ``Add...`` button
      - Browse the installation folder of the toolchain

      <br/>

      .. image:: /_static/images/renesas/toolchains.png
         :alt: Toolchains

    - Clone the ready-to-use `lv_port_renesas_rx72n-envision-kit <https://github.com/lvgl/lv_port_renesas_rx72n-envision-kit.git>`__ repository:

      .. code-block:: shell

          git clone https://github.com/lvgl/lv_port_renesas_rx72n-envision-kit.git --recurse-submodules


      Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.

    - Open e\ |sup2| studio, go to ``File`` -> ``Import project`` and select ``General`` / ``Existing projects into workspace``
    - Select the cloned folder and press ``Finish``.
    - Double click on ``RX72N_EnVision_LVGL.scfg``. This will activate the `Configuration Window`.

      Renesas' Smart Configurator (SMC) includes BSP and HAL layer support extended with multiple RTOS variants and other middleware stacks.
      The components will be available via code generation, including the entry point of the application.

      Press ``Generate Code`` in the top right corner.

      .. image:: /_static/images/renesas/generate_smc.png
         :alt: Code generation with SMC

    - Build the project by pressing ``Ctrl`` + ``Alt`` + ``B``
    - Click the Debug button (|img_debug_btn|). If prompted with `Debug Configurations`, on the `Debugger` tab select the ``E2 Lite``
      as `Debug hardware` and ``R5F572NN`` as `Target Device`:

      .. image:: /_static/images/renesas/debug_rx72.png
         :alt: Debugger parameters for RX72

    .. note::
       Make sure that both channels of ``SW1`` DIP switch (next to ``ECN1``) are OFF.


Modify the project
------------------


Open a demo
~~~~~~~~~~~

The entry point of the main task is contained in ``src/LVGL_thread_entry.c`` in all 3 projects.

You can disable the LVGL demos (or just comment them out) and call some ``lv_example_...()`` functions, or add your custom code.


Configuration
~~~~~~~~~~~~~

``src/lv_conf.h`` contains the most important settings for LVGL. Namely:

- :c:macro:`LV_COLOR_DEPTH` to set LVGL's default color depth
- :c:macro:`LV_MEM_SIZE` to set the maximum RAM available for LVGL
- :c:macro:`LV_USE_DAVE2D` to enable the GPU


Hardware and software components can be modified in a visual way using the `Configuration Window`.


Support
-------

In case of any problems or questions open an issue in the corresponding repository.

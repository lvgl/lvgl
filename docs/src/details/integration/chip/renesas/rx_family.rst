=========
RX Family
=========

.. |sup2|   unicode:: U+000B2 .. SUPERSCRIPT TWO

.. |img_debug_btn| image:: /_static/images/renesas/debug_btn.png
   :alt: Debug button

Supported boards in the RX Family:

- **RX72N Envision Kit**



Run the Project
***************

- The official IDE of Renesas is called e\ |sup2| studio. Because it's Eclipse-based,
  it runs on Windows, Linux, and Mac as well.  It can be downloaded
  `here <https://www.renesas.com/us/en/software-tool/e-studio>`__.
- Download and install the required driver for the debugger

   - for Windows: `64 bit here <https://www.renesas.com/us/en/document/uid/usb-driver-renesas-mcu-tools-v27700-64-bit-version-windows-os?r=488806>`__
     and `32 bit here <https://www.renesas.com/us/en/document/uid/usb-driver-renesas-mcu-toolse2e2-liteie850ie850apg-fp5-v27700for-32-bit-version-windows-os?r=488806>`__
   - for Linux: `here <https://www.renesas.com/us/en/document/swo/e2-emulator-e2-emulator-lite-linux-driver?r=488806>`__

- RX72 requires an external compiler for the RXv3 core. A free and open-source version is available
  `here <https://llvm-gcc-renesas.com/rx-download-toolchains/>`__ after registration.

  The compiler must be activated in e\ |sup2| studio:

  - Go to go to ``Help`` -> ``Add Renesas Toolchains``
  - Press the ``Add...`` button
  - Select the installation folder of the toolchain

  .. image:: /_static/images/renesas/toolchains.png
     :alt: Toolchains

- Clone the ready-to-use `lv_port_renesas_rx72n-envision-kit <https://github.com/lvgl/lv_port_renesas_rx72n-envision-kit.git>`__ repository:

  .. code-block:: shell

      git clone https://github.com/lvgl/lv_port_renesas_rx72n-envision-kit.git --recurse-submodules


  Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.

- Open e\ |sup2| studio, go to ``File`` -> ``Import project`` and select ``General`` / ``Existing projects into workspace``
- Select the cloned folder and press ``Finish``.
- Double-click on ``RX72N_EnVision_LVGL.scfg`` to activate the ``Configuration Window``.

  Renesas' Smart Configurator (SMC) includes BSP and HAL layer support extended with
  multiple RTOS variants and other middleware stacks.  The components will be
  available via code generation, including the entry point of the application.

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
******************


Open a demo
-----------

The entry point of the main task is contained in ``src/LVGL_thread_entry.c``.

You can disable the LVGL demos (or just comment them out) and call some
``lv_example_...()`` functions, or add your own custom code.


Configuration
-------------

``src/lv_conf.h`` contains the most important settings for LVGL. Namely:

- :c:macro:`LV_COLOR_DEPTH` to set LVGL's default color depth
- :c:macro:`LV_MEM_SIZE` to set the maximum RAM available to LVGL
- :c:macro:`LV_USE_DAVE2D` to enable the GPU


Hardware and software components can be modified in a visual way using the
``Configuration Window``.



Support
*******

In case of any problems or questions open an issue in the corresponding repository.

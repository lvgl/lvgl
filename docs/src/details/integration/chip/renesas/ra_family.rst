=========
RA Family
=========

.. |sup2|   unicode:: U+000B2 .. SUPERSCRIPT TWO

.. |img_debug_btn| image:: /_static/images/renesas/debug_btn.png
   :alt: Debug button

Supported boards in the RA Family:

- **EK-RA8D1**
- **EK-RA6M3G**

Run the Project
***************

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


Modify the project
******************


Open a demo
-----------

The entry point of the main task is contained in ``src/LVGL_thread_entry.c``.

You can disable the LVGL demos (or just comment them out) and call some ``lv_example_...()`` functions, or add your custom code.


Configuration
-------------

``src/lv_conf.h`` contains the most important settings for LVGL. Namely:

- :c:macro:`LV_COLOR_DEPTH` to set LVGL's default color depth
- :c:macro:`LV_MEM_SIZE` to set the maximum RAM available for LVGL
- :c:macro:`LV_USE_DAVE2D` to enable the GPU


Hardware and software components can be modified in a visual way using the `Configuration Window`.


Support
*******

In case of any problems or questions open an issue in the corresponding repository.

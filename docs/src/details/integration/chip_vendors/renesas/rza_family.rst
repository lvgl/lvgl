.. include:: /include/substitutions.txt
===========
RZ/A Family
===========

Supported boards in the RZ/A Family:

- **RZ/A3M**



Run the Project
***************

- The RZ/A boards are MPUs with a focus on bare metal and RTOS applications. Projects are built for them using e\ |sup2| Studio IDE, available for Windows, Mac, and Linux.
- Clone the ready-to-use repository for your board:

    .. code-block:: shell

        git clone https://github.com/lvgl/lv_port_renesas-ek-rz_a3m --recurse-submodules


  Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.
- Follow the instructions in the project README.md to
  build and flash the project to the board.



Modify the project
******************


Open a demo
-----------

The entry point is contained in ``src/LVGL_thread_entry.c``.

You can disable the LVGL demos (``lv_demo_benchmark()``) (or just comment them out)
and call some ``lv_example_...()`` functions, or add your own custom code.


Configuration
-------------

Edit ``lv_conf.h`` to configure LVGL.

It will automatically run any demo that is enabled in ``lv_conf.h``. You can see
``lv_conf.defaults`` for a summary of the configs which have been changed from the defaults.


Support
*******

In case of any problems or questions open an issue in the corresponding repository.

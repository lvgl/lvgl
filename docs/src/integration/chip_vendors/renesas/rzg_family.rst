===========
RZ/G Family
===========

Supported boards in the RZ/G Family:

- **RZ/G2L-EVKIT**
- **RZ/G2UL-EVKIT**
- **RZ/G3E-EVKIT**


G2L and G2UL
************

Run the Project
---------------

- The RZ/G2L and RZ/G2UL are MPUs with support for running Linux. Projects are built for them
  using a cross-compiler CLI toolchain SDK in a Linux PC environment (WSL, Docker,
  etc. can be used on Windows).
- The G2L has a Wayland desktop and the project appears as a Wayland window. The G2UL
  does not have a desktop so the project is fullscreen and uses the ``fbdev`` driver.
- The SDK currently uses LVGL v8.3 so this project uses this version to mirror the SDK
  version, even though LVGL is statically linked. You may try using newer versions of LVGL.
  See the `v8-to-v9 Migration Guide <https://docs.lvgl.io/9.0/CHANGELOG.html#migration-guide>`__ for things you will need to address.
- Clone the ready-to-use repository for your selected board:

  .. code-block:: shell

      git clone https://github.com/lvgl/lv_port_renesas_rz-g2l-evkit --recurse-submodules


  Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.
- Follow the instructions in the project README.md to obtain the SD Card image and toolchain installer,
  build, and upload the project to the board.
- Stop any automatically started demos (on G2UL run ``systemctl stop demo-launcher`` in the terminal).
- Run the project:

  .. code-block:: shell

      ./lvgl_demo_benchmark



Modify the project
------------------


Open a demo
~~~~~~~~~~~

The entry point is contained in ``src/main.c``.

You can disable the LVGL demos (``lv_demo_benchmark()``) (or just comment them out)
and call some ``lv_example_...()`` functions, or add your own custom code.


Configuration
~~~~~~~~~~~~~

Edit ``lv_conf.h`` and ``lv_drv_conf.h`` to configure LVGL. The board image
contains LVGL and lv_drivers as dynamically linkable libraries. This project builds
LVGL statically for customizability and to port the LVGL v9 benchmark from LVGL v8.3.


G3E
***

Run the Project
---------------

- The RZ/G3E is a high-performance quad-core MPU. Projects are built for it using an
  SDK that was generated with Yocto. You can customize a Yocto image+SDK or use the
  pre-built ones. The SDK is intended for a Linux PC environment (WSL, Docker,
  etc. can be used on Windows).
- The G3E has a Wayland desktop but it should be stopped with ``systemctl stop weston``
  before running the default project since it runs in fullscreen using the OpenGL EGL+DRM driver.
- LVGL is not included in the SDK so you should build whichever version you need. Later
  versions of LVGL include the optimal OpenGL driver.
- Clone the ready-to-use repository:

  .. code-block:: shell

      git clone https://github.com/lvgl/lv_port_renesas_rz-g3e-evkit --recurse-submodules


  Downloading the `.zip` from GitHub doesn't work as it doesn't download the submodules.
- Follow the instructions in the project README.md to flash the board bootloader and obtain the
  Yocto SD Card image and SDK (pre-built or custom-built), build, and upload the project to the board.
- Stop the Wayland desktop if using the OpenGL, DRM, or fbdev drivers. Run ``systemctl stop weston`` in the terminal.
- Run the project:

  .. code-block:: shell

      ./lvglsim



Modify the project
------------------


Open a demo
~~~~~~~~~~~

The entry point is contained in ``lv_port_linux/src/main.c``.

You can disable the LVGL demos (``lv_demo_benchmark()``) (or just comment them out)
and call some ``lv_example_...()`` functions, or add your own custom code.


Configuration
~~~~~~~~~~~~~

Edit ``lv_conf.h`` to configure LVGL. You can edit ``lv_conf.defaults`` --- a sparse
version of ``lv_conf.h`` which you can use to regenerate ``lv_conf.h`` by running

.. code-block:: shell

    python3 lv_port_linux/lvgl/scripts/generate_lv_conf.py


Support
*******

In case of any problems or questions open an issue in the corresponding repository.

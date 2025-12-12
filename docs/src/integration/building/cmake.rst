.. _build_cmake:

=====
CMake
=====


Overview
********
CMake is a cross-platform build system generator. It is used to easily integrate a project/library into another project.
It also offers the possibility to configure the build with different options, to enable or disable components, or to
integrate custom scripts executions during the configuration/build phase.

LVGL includes CMake natively, which means that one can use it to configure and build LVGL directly or integrate it into a higher level cmake build.

This project uses CMakePresets to ensure an easy build.
Find out more on Cmake Presets here: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html


Prerequisites
-------------

You need to install

- CMake with GNU make or Ninja (for Linux builds). Be sure to add ninja/make to your PATH!
- The prerequisites listed in ``scripts/install-prerequisites.sh/bat``
- A python3 interpreter if you wish to use KConfig.


How to build this project using cmake
-------------------------------------


Build with Command line
~~~~~~~~~~~~~~~~~~~~~~~

The simplest way to build LVGL using cmake is to use the command line calls:

.. code-block:: bash

    # Method 1
    cd <lvgl_repo>
    mkdir build
    cd build
    cmake ..            # Configure phase
    cmake --build .     # Build phase

    # Method 2
    cd <lvgl_repo>
    cmake -B build      # Configure phase
    cmake --build build # build phase


Build with cmake presets
~~~~~~~~~~~~~~~~~~~~~~~~

Another way to build this project is to use the provided CMakePresets.json or pass options using the command line.
The CMakePresets.json file describes some cmake configurations and build phase. It is a way to quickly use a set of
predefined cmake options.

For now, these configuration presets are available:

- ``windows-base``: A Windows configuration, using VS MSVC. Uses ``lv_conf.h`` as the configuration system.
- ``windows-kconfig``: A Windows configuration, using VS MSVC. Uses Kconfig as the configuration system.
- ``linux-base``: A Linux configuration, using Ninja and GCC. Uses ``lv_conf.h`` as the configuration system.
- ``linux-kconfig``: A Linux configuration, using Ninja and GCC. Uses Kconfig as the configuration system.


And these build presets:

- ``windows-base_dbg``: Windows Debug build.
- ``windows-base_rel``: Windows Release build.
- ``linux-base_dbg``: Linux Debug build.
- ``linux-base_rel``: Linux Release build.


Here is how to build using the presets:

.. code-block:: bash

    cmake --preset windows-base
    cmake --build --preset windows-base_dbg
    ctest --preset windows-base_dbg


Build with IDE
~~~~~~~~~~~~~~

The recommended way for consuming CMakePresets is a CMakePresets aware IDE such as

- VS 2022
- VS Code
- CLion


Simply load this project into your IDE and select your desired preset and you are good to go.


Build with CMake GUI
~~~~~~~~~~~~~~~~~~~~

Open this project with CMake GUI and select your desired preset. When hitting the generate button,
CMake will create solution files (for VS) or Ninja Files (for Linux Ninja Build)

The following targets are available.

- lvgl (the actual library, required)
- lvgl_thorvg (a vector graphics extension, optional)
- lvgl_examples (example usages, optional)
- lvgl_demos (some demos, optional)


All optional targets can be disabled by setting the proper cache variables.
If you use cmake to install lvgl, 3 folders will be created.

- include/lvgl (contains all public headers)
- bin (contains all binaries (\*.dll))
- lib (contains all precompiled source files (\*.lib))


.. _integrating_lvgl_cmake:

Integrate LVGL to your project using cmake
------------------------------------------

The LVGL cmake system is made to be integrated into higher level projects. To do so, simply add this to your
project's ``CMakeLists.txt``.

This snippet adds LVGL and needs an ``lv_conf.h`` file present next to the lvgl folder:

.. code-block:: cmake

    add_subdirectory(lvgl)

This snippet sets up LVGL and tells it which ``lv_conf.h`` file to use:

.. code-block:: cmake

    set(LV_BUILD_CONF_PATH path/to/my_lv_conf.h)
    add_subdirectory(lvgl)

This snippet sets up LVGL and points to the folder where ``lv_conf.h`` is located:

.. code-block:: cmake

    set(LV_BUILD_CONF_DIR path/to/directory)
    add_subdirectory(lvgl)


This snippet adds LVGL and specifies to use Kconfig as the configuration system:

.. code-block:: cmake

    set(LV_BUILD_USE_KCONFIG ON)
    add_subdirectory(lvgl)

This snippet adds LVGL and specifies to use Kconfig as the configuration system and to use a specific defconfig:

.. code-block:: cmake

    set(LV_BUILD_USE_KCONFIG ON)
    set(LV_BUILD_DEFCONFIG_PATH path/to/my_defconfig)
    add_subdirectory(lvgl)


To enable the demos and examples set these options:

.. code-block:: cmake

    set(CONFIG_LV_BUILD_EXAMPLES ON)
    set(CONFIG_LV_BUILD_DEMOS ON)
    add_subdirectory(lvgl)

Below is a list of the available options/variables

.. list-table::
   :header-rows: 1
   :widths: 20 10 50

   * - Variable/Option
     - Type
     - Description
   * - LV_BUILD_CONF_PATH
     - PATH
     - Allows to set a custom path for ``lv_conf.h``
   * - LV_BUILD_CONF_DIR
     - PATH
     - Allows to set a directory containing ``lv_conf.h``
   * - LV_BUILD_USE_KCONFIG
     - BOOLEAN
     - When set KConfig is used as the configuration source. This option is disabled by default.
   * - LV_BUILD_DEFCONFIG_PATH
     - PATH
     - Specify to use a .defconfig file instead of the current .config in a Kconfig setup.
   * - LV_BUILD_LVGL_H_SYSTEM_INCLUDE
     - BOOLEAN
     - Enable if LVGL will be installed to the system or your build system uses a sysroot.
       Turning this option on implies that the resources generated by the image generation script
       will include ``lvgl.h`` as a system include. i.e: ``#include <lvgl.h>``.
       This option is disabled by default.
   * - LV_BUILD_LVGL_H_SIMPLE_INCLUDE
     - BOOLEAN
     - When enabled the resources will include ``lvgl.h`` as a simple include, this option
       is enabled by default.
   * - LV_BUILD_SET_CONFIG_OPTS
     - BOOLEAN
     - When enabled, this option runs a script that processes the ``lv_conf.h``/Kconfig
       configuration using ``pcpp`` to generate corresponding ``CONFIG_LV_*`` and
       ``CONFIG_LV_BUILD_*`` CMake variables based on the contents of ``lv_conf_internal.h``.
       This requires python3 with ``venv`` and ``pip`` or access to a working ``pcpp``.
       If KConfig is used, this is enabled automatically.
   * - CONFIG_LV_BUILD_DEMOS
     - BOOLEAN
     - When enabled builds the demos
   * - CONFIG_LV_BUILD_EXAMPLES
     - BOOLEAN
     - When enabled builds the examples
   * - CONFIG_LV_USE_THORVG_INTERNAL
     - BOOLEAN
     - When enabled the in-tree LVGL version of ThorVG is compiled
   * - CONFIG_LV_USE_PRIVATE_API
     - BOOLEAN
     - When enabled the private headers ``*_private.h`` are installed on the system

.. note::

   When ``LV_BUILD_SET_CONFIG_OPTS`` or ``LV_BUILD_USE_KCONFIG`` are enabled,
   the options/variables beginning with the prefix ``CONFIG_*`` are automatically
   set to the values found in ``lv_conf.h``


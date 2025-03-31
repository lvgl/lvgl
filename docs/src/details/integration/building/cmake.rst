.. _build_cmake:

=====
CMake
=====


Overview
********
CMake is a cross-platform build system generator. It is used to easily integrate a project/library into another project.
It also offer the possibility to configure the build with different options, to enable or disable components, or to
integrate custom scripts executions during the configuration/build phase.

LVGL includes CMake natively, which means that one can use it to configure and build LVGL directly or integrate it into an higher
level cmake build.

This project uses CMakePresets to ensure an easy build.
Find out more on Cmake Presets here: https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html


Prerequisites
-------------

You need to install

- CMake
- Ninja (for Linux builds). Be sure to Add ninja to your PATH!
- The prerequisites listed in ``scripts/install-prerequisites.sh/bat``


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

Another way to build this project is to use the provided CMakePresets.json or passing option using the command line.
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

The recommend way for consuming CMakePresets is a CMakePresets aware IDE such as

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
- lvgl_thorvg (an vector graphics extension, optional)
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

    set(LV_CONF_INCLUDE_SIMPLE OFF)
    add_subdirectory(lvgl)


This snippet adds LVGL and needs an ``lv_conf.h`` file present in lvgl/src folder:

.. code-block:: cmake

    add_subdirectory(lvgl)


This snippet adds LVGL and specify a ``lv_conf.h`` to use:

.. code-block:: cmake

    set(LV_CONF_PATH path/to/my_lv_conf.h)
    add_subdirectory(lvgl)


This snippet adds LVGL and specify to use Kconfig as the configuration system:

.. code-block:: cmake

    set(LV_USE_KCONFIG ON)
    add_subdirectory(lvgl)

This snippet adds LVGL and specify to use Kconfig as the configuration system and to use a specific defconfig:

.. code-block:: cmake

    set(LV_USE_KCONFIG ON)
    set(LV_DEFCONFIG_PATH path/to/my_defconfig)
    add_subdirectory(lvgl)


To disable the demo/example set these options:

.. code-block:: cmake

    set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
    set(LV_CONF_BUILD_DISABLE_DEMOS ON)
    add_subdirectory(lvgl)


These cmake options are available to configure LVGL:

- ``LV_CONF_PATH`` (STRING): Specify a custom path for ``lv_conf.h``.
- ``LV_CONF_INCLUDE_SIMPLE`` (BOOLEAN): Use ``#include "lv_conf.h"`` instead of ``#include "../../lv_conf.h"``
- ``LV_USE_KCONFIG`` (BOOLEAN): Use Kconfig as the configuration source.
- ``LV_DEFCONFIG_PATH`` (STRING): Specify to use a defconfig file instead of the current .config in a Kconfig setup.
- ``LV_CONF_BUILD_DISABLE_EXAMPLES`` (BOOLEAN): Disable building the examples if set.
- ``LV_CONF_BUILD_DISABLE_DEMOS`` (BOOLEAN): Disable building the demos if set.
- ``LV_CONF_BUILD_DISABLE_THORVG_INTERNAL``: Disable the internal compilation of ThorVG.
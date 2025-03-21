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

The recommended way to build this project is to use the provided CMakePresets.json. This file contains 2 configurations

- a windows (MSVC) build using Visual Studio
- a linux (gcc) build using Ninja


More configurations will be added once available.


Build with IDE
--------------

The recommend way for consuming CMakePresets is a CMakePresets aware IDE such as

- VS 2022
- VS Code
- CLion


Simply load this project into your IDE and select your desired preset and you are good to go.


Build with CMake GUI
--------------------

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


Build with Command line
-----------------------
You can also build your project using the command line. Run the following commands to use the presets:

.. code-block:: bash

    cmake --preset windows-base
    cmake --build --preset windows-base_dbg
    ctest --preset windows-base_dbg


To not use preset, use these commands:

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

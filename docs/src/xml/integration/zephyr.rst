.. _editor_integration_zephyr:

========================================
Zephyr RTOS
========================================

Overview
********

The goal of this guide is to ensure a simple integration between a Zephyr project
and the LVGL Editor.

Build System
------------

When creating a new interface in the LVGL Editor, a ``CMakeLists.txt`` file is generated, which
allows for conditional fields, differentiating between what belongs to the editor and the target project.
This enables the inclusion of symbols and directories with lower complexity.

.. code-block:: cmake

    if (LV_EDITOR_PREVIEW)
        # things for the Preview
    else ()
        # things for your Project
    endif ()

The Zephyr ecosystem is based on CMake, making the integration between projects relatively straightforward
through the use of ``CMakeLists.txt`` files. For libraries like LVGL, the Kconfig system is used
to configure parameters, thus avoiding direct modification of ``lv_conf.h``.

For more details on Kconfig configuration methods, refer to the
`Zephyr Kconfig documentation <https://docs.zephyrproject.org/latest/build/kconfig/setting.html>`.

In this guide, we will use ``prj.conf`` for simplicity, but you may adapt the configuration
approach to your project's needs.

Configuration
-------------

Zephyr has its own configuration wrapper for LVGL that translates Kconfig options into LVGL settings.
Therefore, some symbols need to be included in configuration files:

.. code-block:: kconfig

    CONFIG_LVGL=y
    CONFIG_LV_Z_MEM_POOL_SIZE=58368
    CONFIG_LV_Z_SHELL=y

.. note:: The CONFIG_LV_Z_MEM_POOL_SIZE parameter defines the LVGL heap size in bytes.
    The value must be adjusted based on the number and complexity of UI components.
    Insufficient memory allocation will result in Out Of Memory (OOM) errors.

As a reference, some LVGL demos require significantly more than the default allocation.
Start with at least 58 KB (58368 bytes) and increase if needed based on your application.

Additionally, it is necessary to enable display and input drivers:

.. code-block:: kconfig

    CONFIG_DISPLAY=y
    CONFIG_INPUT=y

In the devicetree file ``<your_board>.dts`` and/or overlay ``<your_overlay>.overlay``, you must 
configure the specific nodes for your hardware's display and touchscreen. Example:

.. code-block:: dts

    / {
        chosen {
            zephyr,display = &display_controller;
        };

        lvgl_pointer {
            compatible = "zephyr,lvgl-pointer-input";
            input = <&touch_controller>;
        };
    };

    &display_controller {
    /* Display-specific parameters (resolution, timing, etc.) */
        status = "okay";
    };

    &touch_controller {
    /* Touch controller-specific parameters (sensitivity, etc.) */
        status = "okay";
    }

If some resources are not supported on your board, refer to the official Zephyr documentation. There is a guide detailing
how to `port a board <https://docs.zephyrproject.org/latest/hardware/porting/board_porting.html>`_ and the valid properties
in the devicetree for `input <https://docs.zephyrproject.org/latest/build/dts/api/bindings/input/zephyr%2Clvgl-pointer-input.html>`_.

Integration
-----------

Considering an example structure using Zephyr with LVGL Editor:

.. code-block:: text

    app/
    ├── src/
    │   └── main.c
    ├── ui_project/
    │   ├── ui_project.h
    │   ├── ui_project.c
    │   ├── screens/
    │   ├── widgets/
    │   ├── components/
    │   └── CMakeLists.txt
    └── CMakeLists.txt

Add a subdirectory in the project's root ``CMakeLists.txt`` to include the UI in the compilation stage:

.. code-block:: cmake

    add_subdirectory(ui_project)
    target_link_libraries(app PUBLIC lib-ui)

In the ``CMakeLists.txt`` file of the ``ui_project/`` directory, created by the LVGL Editor, add:

.. code-block:: cmake

    # Create the UI sources as a library
    add_library(lib-ui ${PROJECT_SOURCES})

    # You may use this check to add configuration when compiling for the Editor preview,
    # or for your target.
    if (LV_EDITOR_PREVIEW)
        # things for the Preview
    else ()
        # things for your target

        # Link with Zephyr interface to inherit compiler flags and settings
        target_link_libraries(lib-ui PUBLIC zephyr_interface)

        # Include LVGL headers from Zephyr's module path
        target_include_directories(lib-ui PUBLIC
            ${CMAKE_BINARY_DIR}/zephyr/include/generated/lvgl
            ${ZEPHYR_LVGL_MODULE_DIR}
        )
    endif ()

**Important note:** Zephyr automatically defines the ``LV_CONF_INCLUDE_SIMPLE`` symbol during
the LVGL module build process, so it is not necessary to define it manually in your CMakeLists.txt.
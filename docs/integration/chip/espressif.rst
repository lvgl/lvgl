=============================
Espressif (ESP32 Chip Series)
=============================

LVGL can be used and configured as standard `ESP-IDF <https://github.com/espressif/esp-idf>`__ component.

If you are new to ESP-IDF, follow the instructions in the `ESP-IDF Programming guide <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html>`__ to install and set up ESP-IDF on your machine.


LVGL Demo Projects for ESP32
---------------------------

For a quick start with LVGL and ESP32, the following pre-configured demo projects are available for specific development boards:

-  `ESP-BOX-3 <https://github.com/lvgl/lv_port_espressif_esp-box-3>`__
-  `ESP32-S3-LCD-EV-BOARD <https://github.com/lvgl/lv_port_espressif_esp32-s3-lcd-ev-board>`__
-  `M5Stack-CoreS3 <https://github.com/lvgl/lv_port_espressif_M5Stack_CoreS3>`__

Refer to the README.md files in these repositories for build and flash instructions.

These demo projects use Espressif's Board Support Packages (BSPs). Additional BSPs and examples are available in the `esp-bsp <https://github.com/espressif/esp-bsp>`__ repository.


Using LVGL in Your ESP-IDF Project
----------------------------------

The simplest way to integrate LVGL into your ESP-IDF project is via the `esp_lvgl_port <https://components.espressif.com/components/espressif/esp_lvgl_port>`__ component. This component, used in the demo projects mentioned above, provides helper functions for easy installation of LVGL and display drivers. Moreover, it can add support for touch, rotary encoders, button or USB HID inputs. It simplifies power savings, screen rotation and other platform specific nuances.

The esp_lvgl_port supports LVGL versions 8 and 9 and is compatible with ESP-IDF v4.4 and above. To add it to your project, use the following command:

.. code:: sh

   idf.py add-dependency "espressif/esp_lvgl_port^2.3.0"

By default, esp_lvgl_port depends on the latest stable version of LVGL, so no additional steps are needed for new projects. If a specific LVGL version is required, specify this in your project to avoid automatic updates. LVGL can also be used without esp_lvgl_port, as described below.

Obtaining LVGL
~~~~~~~~~~~~~~

LVGL is distributed through `ESP Registry <https://components.espressif.com/>`__, where all LVGL releases are uploaded.
In case you do not want to use esp_lvgl_port, you can add `LVGL component <https://components.espressif.com/component/lvgl/lvgl>`__ into your project with following command:

.. code:: sh

   idf.py add-dependency lvgl/lvgl^9.*

Adjust the ``^9.*`` part to match your LVGL version requirement. More information on version specifications can be found in the `IDF Component Manager documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html#range-specifications>`__. During the next build, the LVGL component will be fetched from the component registry and added to the project.

**Advanced usage: Use LVGL as local component**

For LVGL development and testing, it may be useful to use LVGL as a local component instead of from the ESP Registry, which offers only released versions and does not allow local modifications. To do this, clone LVGL to your project with the following command:

.. code:: sh

   git submodule add https://github.com/lvgl/lvgl.git components/lvgl

.. note::

   All components from ``${project_dir}/components`` are automatically added to build.

Configuration
~~~~~~~~~~~~~

To configure LVGL, launch the configuration menu with ``idf.py menuconfig`` in your project root directory. Navigate to ``Component config`` and then ``LVGL configuration``.


Support for Display and Touch Drivers
-------------------------------------

For successful LVGL project you will need a display driver and optionally a touch driver. Espressif provides these drivers that are built on its `esp_lcd <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/lcd/index.html>`__ component.

-  esp_lcd natively supports for some `basic displays <https://github.com/espressif/esp-idf/tree/master/components/esp_lcd/src>`__
-  Other displays are maintained in `esp-bsp repository <https://github.com/espressif/esp-bsp/tree/master/components/lcd>`__ and are uploaded to ESP Registry
-  Touch drivers are maintained in `esp-bsp repository <https://github.com/espressif/esp-bsp/tree/master/components/lcd_touch>`__ and are uploaded to ESP Registry

These components share a common public API, making it easy to migrate your projects across different display and touch drivers.

To add a display or touch driver to your project, use a command like:

.. code:: sh

   idf.py add-dependency "espressif/esp_lcd_gc9a01^2.0.0"

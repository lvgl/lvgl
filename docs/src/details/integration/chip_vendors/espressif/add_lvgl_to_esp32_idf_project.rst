.. _lvgl_esp_idf:

================================
Add LVGL to an ESP32 IDF project
================================


LVGL can be used and configured as a standard `ESP-IDF <https://github.com/espressif/esp-idf>`__ component.

If you are new to ESP-IDF, follow the instructions in the `ESP-IDF Programming guide <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html>`__ to install and set up ESP-IDF on your machine.


Using LVGL in Your ESP-IDF Project
**********************************

The simplest way to integrate LVGL into your ESP-IDF project is via the `esp_lvgl_port <https://components.espressif.com/components/espressif/esp_lvgl_port>`__ component. This component, used in the demo projects mentioned above, provides helper functions for easy installation of LVGL and display drivers. Moreover, it can add support for touch, rotary encoders, button or USB HID inputs. It simplifies power savings, screen rotation and other platform specific nuances.

The esp_lvgl_port supports LVGL versions 8 and 9 and is compatible with ESP-IDF v4.4 and above. To add it to your project, use the following command:

.. code:: sh

   idf.py add-dependency "espressif/esp_lvgl_port^2.3.0"

By default, esp_lvgl_port depends on the latest stable version of LVGL, so no additional steps are needed for new projects. If a specific LVGL version is required, specify this in your project to avoid automatic updates. LVGL can also be used without esp_lvgl_port, as described below.

Obtaining LVGL
--------------

LVGL is distributed through `ESP Registry <https://components.espressif.com/>`__, where all LVGL releases are uploaded.
In case you do not want to use esp_lvgl_port, you can add `LVGL component <https://components.espressif.com/component/lvgl/lvgl>`__ into your project with following command:

.. code-block:: sh

   idf.py add-dependency "lvgl/lvgl^9.*"

Adjust the ``^9.*`` part to match your LVGL version requirement. More information on version specifications can be found in the `IDF Component Manager documentation <https://docs.espressif.com/projects/idf-component-manager/en/latest/reference/versioning.html#range-specifications>`__. During the next build, the LVGL component will be fetched from the component registry and added to the project.

**Advanced usage: Use LVGL as local component**

For LVGL development and testing, it may be useful to use LVGL as a local component instead of from the ESP Registry, which offers only released versions and does not allow local modifications. To do this, clone LVGL to your project with the following command:

.. code-block:: sh

   git submodule add https://github.com/lvgl/lvgl.git components/lvgl

.. note::

   All components from ``${project_dir}/components`` are automatically added to the build.

Display Integration
-------------------

For a successful LVGL project, you will need a display driver and optionally a touch driver. Espressif provides these drivers that are built on its `esp_lcd <https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/lcd/index.html>`__ component.

-  esp_lcd natively supports some `basic displays <https://github.com/espressif/esp-idf/tree/master/components/esp_lcd/src>`__
-  Other displays are maintained in `esp-bsp repository <https://github.com/espressif/esp-bsp/tree/master/components/lcd>`__ and are uploaded to ESP Registry
-  Touch drivers are maintained in `esp-bsp repository <https://github.com/espressif/esp-bsp/tree/master/components/lcd_touch>`__ and are uploaded to ESP Registry

These components share a common public API, making it easy to migrate your projects across different display and touch drivers.

To add a display or touch driver to your project, use a command like:

.. code-block:: sh

   idf.py add-dependency "espressif/esp_lcd_gc9a01^2.0.0"

Configuration
-------------

To configure LVGL, launch the configuration menu with ``idf.py menuconfig`` in your project root directory. Navigate to ``Component config`` and then ``LVGL configuration``.

Addtionally the user can make the current LVGL settings permanent, or default, for the current
project, all that is needed is to create a file on the project root called 
`sdkconfig.defaults` and move the `CONFIG_LV_` symbols to that file.

It is possible to create a per-chip default confiuration files by creating A
configuration default files which starts to the chip variant, for example
`sdkconfig.esp32p4` will only apply the default configuration for an ESP32-P4
IDF project.

Starting the LVGL component
---------------------------

Once the IDF project and the LVGL component have been configured, all 
the early initialization process inside of the code will be ready to use, however
the user should manually start the LVGL subsystem for IDF by calling `bsp_display_start()`, 
or `lvgl_port_init()` if LVGL was manually configured, for example without using
the `esp_bsp` component. 

After calling this function, LVGL will be running in the background; that is, 
unlike the usual approach, there is no need to periodically call :cpp:expr:`lv_timer_handler()`,
this function is called by a background task managed by the IDF.

.. code-block:: c

        void app_main(void)
        {
            bsp_display_start();
            bsp_display_backlight_on();

            bsp_display_lock(0);
            lv_demo_benchmark();
            bsp_display_unlock();
        }

For cases when the `esp_bsp` is not being used, it is possible to invoke
the ESP-LVGL port directly:

.. code-block:: c

        void app_main(void)
        {
            const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
            esp_err_t err = lvgl_port_init(&lvgl_cfg);
  
            lv_demo_benchmark();
        }

Building and Flashing
---------------------

Building an IDF project that features the LVGL usage, is similar to any other
project, by using IDF through the command line the user can combine various
commands into a single prompt:

.. code-block:: sh

    idf.py build flash monitor

After the flashing the monitor console will be launched automatically.

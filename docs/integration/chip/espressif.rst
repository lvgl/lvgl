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

Using the File System under ESP-IDF
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP-IDF uses the standard C functions (``fopen``, ``fread``) in all storage related APIs.
This allows seamless interoperability with LVGL when enabling the :c:macro:`LV_USE_FS_STDIO` configuration.
The process is described in details below, using ``SPIFFS`` as demonstration.

- **Decide what storage system you want to use**

   ESP-IDF has many, ready-to-use examples like
   `SPIFFS <https://github.com/espressif/esp-idf/tree/master/examples/storage/spiffsgen>`__
   , 
   `SD Card <https://github.com/espressif/esp-idf/tree/master/examples/storage/sd_card/sdspi>`__ 
   and 
   `LittleFS <https://github.com/espressif/esp-idf/tree/master/examples/storage/littlefs>`__
   .

- **Re-configure your own project**

   The example project should be examined for details, but in general the changes involve:

   - Enabling LVGL's STDIO file system in the configuration

      You can use ``menuconfig``:

         - ``Component config → LVGL configuration → 3rd Party Libraries``: enable ``File system on top of stdio API``
         - Then select ``Set an upper cased letter on which the drive will accessible`` and set it to ``65`` (ASCII **A**)
         - You can also set ``Default driver letter`` to 65 to skip the prefix in file paths.

   - Modifying the partition table

      The exact configuration depends on your flash size and existing partitions,
      but the new final result should look something like this:

      .. code:: csv

         nvs,      data, nvs,     0x9000,  0x6000,
         phy_init, data, phy,     0xf000,  0x1000,
         factory,  app,  factory, 0x10000, 1400k,
         storage,  data, spiffs,         ,  400k,


      .. note::

         If you are not using a custom ``parition.csv`` yet, it can be added
         via ``menuconfig`` (``Partition Table → Partition Table → Custom partition table CSV``).

   - Apply changes to the build system

      Some ESP file systems provide automatic generation from a host folder using CMake. The proper line(s) must be copied to ``main/CMakeLists.txt``

      .. note::

         ``LittleFS`` has extra dependencies that should be added to ``main/idf_component.yml``

- **Prepare the image files**

   LVGL's ``LVGLImage.py`` Python tool can be used to convert images to binary pixel map files.
   It supports various formats and compression.

   Meanwhile 3rd party libraries
   (like :ref:`LodePNG<lodepng>` and :ref:`Tiny JPEG<tjpgd>`)
   allow using image files without conversion.

   After preparing the files, they should be moved to the target device:

   - If properly activated a **SPIFFS** file system based on the ``spiffs_image`` folder should be automatically generated and later flashed to the target
   - Similar mechanism for **LittleFS** uses the ``flash_data`` folder, but it's only available for Linux hosts
   - For the **SD Card**, a traditional file browser can be used

- **Invoke proper API calls in the application code**

   The core functionality requires only a few lines. The following example draws the image as well.

   .. code:: c

      #include "esp_spiffs.h"

      void lv_example_image_from_esp_fs(void) {

         esp_vfs_spiffs_conf_t conf = {
            .base_path = "/spiffs",
            .partition_label = NULL,
            .max_files = 5,
            .format_if_mount_failed = false
         };

         esp_err_t ret = esp_vfs_spiffs_register(&conf);

         if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register SPIFF filesystem");
            return;
         }

         lv_obj_t * obj = lv_image_create(lv_screen_active());
         lv_image_set_src(obj, "A:/spiffs/logo.bin");
         lv_obj_center(obj);
      }

- **Build and flash**

   After calling ``idf.py build flash`` the picture should be displayed on the screen.


.. note::

   Changes made by ``menuconfig`` are not being tracked in the repository if the ``sdkconfig`` file is added to ``.gitignore``, which is the default for many ESP-IDF projects.
   To make your configuration permanent, add the following lines to ``sdkconfig.defaults``:

   .. code:: c

      CONFIG_PARTITION_TABLE_CUSTOM=y
      CONFIG_LV_USE_FS_STDIO=y
      CONFIG_LV_FS_STDIO_LETTER=65
      CONFIG_LV_LV_FS_DEFAULT_DRIVE_LETTER=65

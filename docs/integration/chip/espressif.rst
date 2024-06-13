=============================
Espressif (ESP32 chip series)
=============================

LVGL can be used and configured as a standard `ESP-IDF <https://github.com/espressif/esp-idf>`__ component.

More information about ESP-IDF build system can be found `here <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html>`__.


LVGL demo project for ESP32
---------------------------

We've created `lv_port_esp32 <https://github.com/lvgl/lv_port_esp32>`__,
a project using ESP-IDF and LVGL to show one of the demos from
`demos <https://github.com/lvgl/lvgl/demos>`__. You can configure the
project to use one of the many supported display controllers and targets
(chips).

See `lvgl_esp32_drivers <https://github.com/lvgl/lvgl_esp32_drivers>`__
repository for a complete list of supported display and indev (touch)
controllers and targets.


Using LVGL in your ESP-IDF project
----------------------------------

Prerequisites
~~~~~~~~~~~~~

-  ESP-IDF v4.1 and above
-  ESP evaluation board with a display


Obtaining LVGL
~~~~~~~~~~~~~~

**Option 1:** git submodule

Simply clone LVGL into your ``project_root/components`` directory and it
will be automatically integrated into the project. If the project is a
git repository you can include LVGL as a git submodule:

.. code:: sh

   git submodule add https://github.com/lvgl/lvgl.git components/lvgl

The above command will clone LVGL's main repository into the
``components/lvgl`` directory. LVGL includes a ``CMakeLists.txt`` file
that sets some configuration options so you can use LVGL right away.

**Option 2:** IDF Component Manager

LVGL is also distributed through `IDF Component Manager <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html>`__.
It allows users to seamlessly integrate `LVGL component <https://components.espressif.com/component/lvgl/lvgl>`__ into
their project with following command:

.. code:: sh

   idf.py add-dependency lvgl/lvgl>=8.*

During next project build, LVGL component will be fetched from the
component registry and added to project build.


Configuration
~~~~~~~~~~~~~

When you are ready to configure LVGL, launch the configuration menu with
``idf.py menuconfig`` in your project root directory, go to
``Component config`` and then ``LVGL configuration``.


Using lvgl_esp32_drivers in ESP-IDF project
-------------------------------------------

You can also add ``lvgl_esp32_drivers`` as a "component". This component
should be located inside a directory named "components" in your project
root directory.

When your project is a git repository you can include
``lvgl_esp32_drivers`` as a git submodule:

.. code:: sh

   git submodule add https://github.com/lvgl/lvgl_esp32_drivers.git components/lvgl_esp32_drivers


Using the File System under ESP-IDF
-----------------------------------

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

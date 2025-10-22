===============
Tips and tricks
===============


Improving LVGL speed of execution
*********************************

The IDF project in general are configured to optimize the final application image
in respect of its size. For some LVGL applications this may not be desired or will
result on poor speed of execution.

In this case, it is interesting to set some of the IDF project wide options on the 
`sdkconfig.defaults` such as: 

.. code-block:: c

        CONFIG_COMPILER_OPTIMIZATION_PERF=y

This one will compile the application with performance as priority, using SIMD 
instructions where is possible. It is possible to perceive an increase up to 30%
of overall speed execution increment.

it is also possible to speed-up the execution of the critical code of the LVGL, by
telling the compiler to put these sections on the IRAM area of ESP32 chips setting
the following option:

.. code-block:: c

        CONFIG_LV_ATTRIBUTE_FAST_MEM_USE_IRAM=y

It is also possible to set the CPU to always run on its maximum speed by
setting the `CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_` option, the value of the 
frequency varies from chip to chip, for example P4 families support 360MHz:

.. code-block:: c

        CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_360

And ESP32/ESP32-S3 support 240MHz:

.. code-block:: c

        CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ_240

Please notice, some of these options needs to be enabled by setting the IDF 
experimental options to true:

.. code-block:: c

        CONFIG_IDF_EXPERIMENTAL_FEATURES=y


Configuring the PSRAM on ESP32 supported devices
************************************************

Some of the high-end chips of ESP32 features an external memory on its module, it is 
Pseudo-Static Random Access Memory, the PSRAM. In general values from 4 to 16MB are
present on the chip and LVGL can take a portion of this memory to:

- Copy read-only objects from Flash to PSRAM to increase speed.
- Use direct-mode plus dual buffer even on ESP32 that does not have built-in display controller.

In both scenarios the result will be reflected on less time to flush data to the 
display, resulting in higher frame-rates. To enable the PSRAM usage the user should:

.. code-block:: c

        CONFIG_SPIRAM=y
        CONFIG_SPIRAM_MODE_HEX=y
        CONFIG_SPIRAM_USE=y
        CONFIG_SPIRAM_ALLOW_BSS_SEG_EXTERNAL_MEMORY=y
        CONFIG_SPIRAM_RODATA=y

These options can reside on the IDF project `sdkconfig.defaults`, the last option


Application crashes when enabling PPA
*************************************

Is it possible to happen of an application to start crashing because the user
enabled `CONFIG_LV_USE_PPA` option. The typical symptom is the appearance of 
a message on the monitor console that indicates error when esp32 calls the 
`esp_msync` function.

This happens because PPA only accepts chunks of data that are aligned to the 
cache L1 line size, that is it 64-bytes, even though the PPA draw unit handles
the alignment of the source buffer, the target draw buffer area should be also
aligned otherwise the transfer from PPA to it may fail. To prevent this 
behavior is interesting to make `CONFIG_LV_DRAW_BUF_ALIGN` to be a multiple of the
cache L1 line size, that is it, set its value to `64` instead of the default of `4`.

.. code-block:: c

        CONFIG_LV_DRAW_BUF_ALIGN=64


EPS32-P4 monitor log reports buffer underrun and frame-rate decreases
*********************************************************************

In cases when the PSRAM is enabled and the PPA is used, it is common to see
frame-rate degradation followed by a message on the log that reports the display
buffer will underrun. This behavior happens because depending the IDF version the
PSRAM was not enabled with maximum supported speed.

To fix that behavior just add to the `sdkconfig.defaults` the following option:

.. code-block:: c

        CONFIG_SPIRAM_SPEED_200M=y 


Enabling LVGL logs on IDF project
*********************************

The LVGL logs are not enabled by default, for enable it, add the following
options on the `sdkconfig.defaults`:

.. code-block:: c

        CONFIG_LV_USE_LOG=y
        CONFIG_LV_LOG_LEVEL_INFO=y
        CONFIG_LV_LOG_PRINTF=y

The logging subsystem of LVGL relies on the ESP-IDF presence of the 
printf.

Using the File System under ESP-IDF
***********************************

ESP-IDF uses the standard C file operation functions (``fopen``, ``fread``) in all its storage related APIs.
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

    .. csv-table:: Partition Table

       nvs,      data, nvs,     0x9000,  0x6000
       phy_init, data, phy,     0xf000,  0x1000
       factory,  app,  factory, 0x10000, 1400k
       storage,  data, spiffs,         ,  400k


    .. note::

       If you are not using a custom ``partition.csv`` yet, it can be added
       via ``menuconfig`` (``Partition Table → Partition Table → Custom partition table CSV``).

  - Apply changes to the build system

    Some ESP file systems provide automatic generation from a host folder using CMake. The proper line(s) must be copied to ``main/CMakeLists.txt``

    .. note::

       ``LittleFS`` has extra dependencies that should be added to ``main/idf_component.yml``

- **Prepare the image files**

  LVGL's ``LVGLImage.py`` Python tool can be used to convert images to binary pixel map files.
  It supports various formats and compression.

  Meanwhile 3rd party libraries
  (like :ref:`LodePNG<lodepng_rst>` and :ref:`Tiny JPEG<tjpgd>`)
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
        lv_image_set_src(widget, "A:/spiffs/logo.bin");
        lv_obj_center(widget);
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
     CONFIG_LV_FS_DEFAULT_DRIVER_LETTER=65

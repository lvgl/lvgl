.. _platformio:

==========
PlatformIO
==========

`PlatformIO <https://platformio.org/>`__ is an open-source ecosystem for 
embedded development that integrates with VS Code and supports many boards and frameworks.

The `LVGL library <https://github.com/lvgl/lvgl>`__ is directly available 
in the PlatformIO library registry and can be added to your project with just a few clicks.

Note that you need to choose a board powerful enough to run LVGL and
your GUI. See the :ref:`requirements of LVGL <requirements>`.

For example, the ESP32 family is a good candidate to create UIs with LVGL.
Recommended boards include the ESP32 (original dual-core) and the ESP32-S3.

Get the LVGL library
********************

The recommended way to add LVGL and display drivers is through the PlatformIO Home GUI in VS Code:

1. Open PIO Home by clicking the PlatformIO icon in the left toolbar.
2. Go to ``Libraries``.
3. In the search bar, type the library name (i.e. “lvgl”).
4. Click on the library you want from the results.
5. Press ``Add to Project``.
6. Select your target project from the dropdown list.

This will automatically add the library to the lib_deps section of your project’s ``platformio.ini``

If you prefer, you can also edit the ``platformio.ini`` file directly and add the libraries under lib_deps:

.. code-block:: ini

    lib_deps =
        lvgl/lvgl

You can also specify the library version to use. 
See `platformio.ini configuration <https://docs.platformio.org/en/latest/projectconf/index.html>`__

If you prefer manual installation, you can also clone the LVGL repository
into the ``lib/`` directory of your PlatformIO project.

Set up drivers
**************

To get started, the recommended option is to use the ``LovyanGFX library`` as the TFT driver, 
since it makes testing much easier.
In PlatformIO, you’ll add both ``LVGL`` and ``LovyanGFX`` as dependencies in your ``platformio.ini``:

.. code-block:: ini

    lib_deps =
        lvgl/lvgl
        lovyan03/LovyanGFX

Another popular option is the ``TFT_eSPI library``, which supports a wide range of displays.

In addition, LVGL also provides its own inbuilt display drivers that can be used directly in your project.
See the :ref:`supported drivers section <display controllers>` for a full list.

Display Configuration
*********************

You’ll need to create a display configuration file, such as ``my_display.hpp`` similar to the example in
`LovyanGFX user_setting.ino <https://github.com/lovyan03/LovyanGFX/blob/master/examples/HowToUse/2_user_setting/2_user_setting.ino>`__.

In PlatformIO, you can place this file in the ``include/`` folder of your project (e.g., ``include/my_display.hpp``).

If you prefer not to use LovyanGFX, you can integrate other graphics libraries as well by 
implementing a wrapper class in the same way LovyanGFX is handled;
`lv_lgfx_user.hpp <https://github.com/lvgl/lvgl/blob/master/src/drivers/display/lovyan_gfx/lv_lgfx_user.hpp>`__ 
provides a good example of this.

Once your configuration file is ready, update ``lv_conf.h`` to include it, for example:

.. code-block:: c

    /* Interface for Lovyan_GFX */
    #define LV_USE_LOVYAN_GFX 1

    #if LV_USE_LOVYAN_GFX
        #define LV_LGFX_USER_INCLUDE "my_display.hpp"
    #endif /*LV_USE_LOVYAN_GFX*/

When using the LovyanGFX interface, you need to add the following to your ``platformio.ini``:

.. code-block:: ini

    build_flags =
        -I include ; the folder containing my_display.hpp
        -I .pio/libdeps/${PIOENV}/LovyanGFX/src  ; so that PIO includes this when compiling LVGL 

Alternatively, you can use the `TFT_eSPI <https://github.com/Bodmer/TFT_eSPI>`__ library.
To make it work, set up TFT_eSPI according to your TFT display type via editing either:

- ``User_Setup.h``
- or by selecting a configuration in the ``User_Setup_Select.h``

Both files are located in ``TFT_eSPI`` library's folder inside PlatformIO’s ``.pio/libdeps/<env>/TFT_eSPI/`` directory.


With PlatformIO, you can also override the configuration using build_flags in 
``platformio.ini``, which avoids editing library files directly.
For example:

.. code-block:: ini

    build_flags =
        -D USER_SETUP_LOADED=1
        -D ST7789_DRIVER=1
        -D TFT_WIDTH=240
        -D TFT_HEIGHT=240
        -D TFT_MOSI=23
        -D TFT_SCLK=18
        -D TFT_CS=5
        -D TFT_DC=16
        -D TFT_RST=17

This approach keeps your configuration under version control.

Configure LVGL
**************

- Using ``lv_conf.h``

After installing LVGL, copy ``lv_conf_template.h`` from the LVGL repo into your project 
and rename it to ``lv_conf.h``
Place it in your ``include/`` or ``src/`` folder, and enable/disable LVGL features by 
editing the macros. For example:

.. code-block:: c

    #define LV_COLOR_DEPTH 16
    #define LV_USE_LOG 1

- Using ``platformio.ini``

Alternatively, you can define LVGL configuration macros directly in ``platformio.ini`` under build_flags:

.. code-block:: ini

    build_flags =
        -D LV_CONF_SKIP
        -D LV_COLOR_DEPTH=16
        -D LV_USE_LOG=1
        -D LV_FONT_MONTSERRAT_24=1


This avoids maintaining a separate ``lv_conf.h`` file.

Initialize and run LVGL
***********************

In your ``main.cpp``, initialize LVGL and the display driver:

.. code-block:: cpp

    #include <Arduino.h>
    #include <lvgl.h>

    #define BUF_SIZE 320 * 50
    uint8_t lv_buffer[BUF_SIZE];

    /* Tick source, tell LVGL how much time (milliseconds) has passed */
    static uint32_t my_tick(void)
    {
        return millis();
    }

    void setup() {
    
        /* Initialize LVGL */
        lv_init();
        /* Set the tick callback */
        lv_tick_set_cb(my_tick);
        /* Initialize the display driver */
        lv_lovyan_gfx_create(320, 480, lv_buffer, BUF_SIZE, true);

        lv_obj_t *label = lv_label_create(lv_screen_active());
        lv_label_set_text(label, "Hello PlatformIO, I'm LVGL!");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0 );

    }

    void loop() {
        lv_timer_handler(); // Update the UI-
        delay(5);
    }


Use the examples and demos
**************************

LVGL comes with a wide range of examples and demo applications that you can try out in PlatformIO.
These are included with the library under the ``examples`` and ``demos`` folders.

To force PlatformIO to compile and run them, add the following to your ``platformio.ini``:

.. code-block:: ini

    build_src_filter =
        +<*>
        ; Force compile LVGL demos and examples, remove when working on your own project
        +<../.pio/libdeps/${PIOENV}/lvgl/demos>
        +<../.pio/libdeps/${PIOENV}/lvgl/examples>

This will include all LVGL ``demos`` and ``examples`` in the build.
Once you are ready to work on your own project, you should remove those lines so only your 
project sources are compiled.

For reference, the `lv_platformio <https://github.com/lvgl/lv_platformio>`__ repository demonstrates 
how to set up and run LVGL on different targets, including ESP32, STM32, and a PC simulator (SDL2). 
This can be very helpful when exploring how LVGL behaves across platforms.

Debugging and logging
*********************

LVGL can display debug information in case of trouble.
In PlatformIO you can enable logging by editing the ``lv_conf.h`` file (or by adding defines in ``platformio.ini``).

In ``lv_conf.h``, go to the Log settings section and enable the log module:

.. code-block:: c

    /* Log settings */
    #define LV_USE_LOG 1 /* Enable/disable the log module */
    #if LV_USE_LOG
        /* How important log should be added:
        * LV_LOG_LEVEL_TRACE A lot of logs to give detailed information
        * LV_LOG_LEVEL_INFO Log important events
        * LV_LOG_LEVEL_WARN Log if something unwanted happened but didn't cause a problem
        * LV_LOG_LEVEL_ERROR Only critical issues, when the system may fail
        * LV_LOG_LEVEL_NONE Do not log anything
        */
        #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
    #endif

After enabling the log module and setting :c:macro:`LV_LOG_LEVEL` accordingly, LVGL will send log messages to the ``Serial`` port (default 115200 bps).

If you prefer not to edit ``lv_conf.h``, the same can be achieved by adding build flags in ``platformio.ini``:

.. code-block:: ini

    build_flags =
        -D LV_USE_LOG=1
        -D LV_LOG_LEVEL=LV_LOG_LEVEL_INFO

You can then print the logs to serial with a callback function, for example:

.. code-block:: cpp

    static void lv_log_print_g_cb(lv_log_level_t level, const char *buf)
    {
        LV_UNUSED(level);
        Serial.write(buf);
    }

    void setup() {
        Serial.begin(115200);
        lv_log_register_print_cb(lv_log_print_g_cb); // Register print function for LVGL logs
    }

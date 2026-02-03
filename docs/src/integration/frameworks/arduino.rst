.. _arduino:

=======
Arduino
=======

The `LVGL library <https://github.com/lvgl/lvgl>`__ is directly available as Arduino libraries.

Note that you need to choose a board powerful enough to run LVGL and
your GUI. See the :ref:`requirements of LVGL <requirements>`.

For example ESP32 is a good candidate to create UI's with LVGL.

If you are using the Arduino GIGA Display Shield (docs `here <https://docs.arduino.cc/hardware/giga-display-shield>`__), use the `Arduino_H7_Video <https://github.com/arduino/ArduinoCore-mbed/tree/main/libraries/Arduino_H7_Video>`__ library, included in the Arduino Mbed OS board package (`here <https://github.com/arduino/ArduinoCore-mbed>`__), which provides built-in LVGL compatibility.

Get the LVGL Arduino library
----------------------------

LVGL can be installed via the Arduino IDE Library Manager or as a .ZIP library.

You can `Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of LVGL from GitHub and simply copy it to Arduino's
library folder.

Set up drivers
--------------

To get started, the recommended option is to use the LovyanGFX
library as the TFT driver, since it makes testing much easier. 
You’ll need to create a display configuration file, such as ``my_display.hpp``
similar to the example in `LovyanGFX user_setting.ino <https://github.com/lovyan03/LovyanGFX/blob/master/examples/HowToUse/2_user_setting/2_user_setting.ino>`__
In the Arduino IDE, you can create a new tab and name it ``my_display.hpp`` and paste the content of your configuration file there.
If you prefer not to use LovyanGFX, you can integrate other graphics libraries as well by implementing a wrapper class in the same way LovyanGFX is handled; 
`lv_lgfx_user.hpp <https://github.com/lvgl/lvgl/blob/master/src/drivers/display/lovyan_gfx/lv_lgfx_user.hpp>`__
provides a good example of this. Once your configuration file 
is ready, update ``lv_conf.h`` to include it, for example:

.. code-block:: c

   /* Interface for Lovyan_GFX */
   #define LV_USE_LOVYAN_GFX         1

   #if LV_USE_LOVYAN_GFX
       #define LV_LGFX_USER_INCLUDE "my_display.hpp"
   #endif /*LV_USE_LOVYAN_GFX*/


Alternatively, you can use `TFT_eSPI <https://github.com/Bodmer/TFT_eSPI>`__ library.
To make it work, set up ``TFT_eSPI`` according to your 
TFT display type via editing either:

- ``User_Setup.h``
- or by selecting a configuration in the ``User_Setup_Select.h``

Both files are located in ``TFT_eSPI`` library's folder.


Configure LVGL
--------------

LVGL has its own configuration file called ``lv_conf.h``. When LVGL is
installed, follow these configuration steps:

1. Go to the directory of the installed Arduino libraries
2. Go to ``lvgl`` and copy ``lv_conf_template.h`` as ``lv_conf.h`` into the Arduino Libraries directory next to the ``lvgl`` library folder.
3. Open ``lv_conf.h`` and change the first ``#if 0`` to ``#if 1`` to enable the content of the file
4. Set the color depth of you display in :c:macro:`LV_COLOR_DEPTH`

Finally the layout with ``lv_conf.h`` should look like this:

::

   arduino
    |-libraries
      |-lvgl
      |-other_lib_1
      |-other_lib_2
      |-lv_conf.h


Initialize and run LVGL
-----------------------

Take a look at `LVGL_Arduino.ino <https://github.com/lvgl/lvgl/blob/master/examples/arduino/LVGL_Arduino/LVGL_Arduino.ino>`__
to see how to initialize LVGL. ``TFT_eSPI`` is used as the display driver.

In the INO file you can see how to register a display and a touchpad for
LVGL and call an example.


Use the examples and demos
--------------------------

Note that, there is no dedicated INO file for every example. Instead,
you can load an example by calling an ``lv_example_...`` function. For
example :cpp:func:`lv_example_btn_1`.

:important: Due to some the limitations of Arduino's build system you
            need to copy ``lvgl/examples`` to ``lvgl/src/examples``. Similarly for
            the demos ``lvgl/demos`` to ``lvgl/src/demos``.


Debugging and logging
---------------------

LVGL can display debug information in case of trouble. In the
``LVGL_Arduino.ino`` example there is a ``my_print`` method, which sends
this debug information to the serial interface. To enable this feature
you have to edit the ``lv_conf.h`` file and enable logging in the
section ``log settings``:

.. code-block:: c

   /* Log settings */
   #define USE_LV_LOG      1   /* Enable/disable the log module */
   #if LV_USE_LOG
   /* How important log should be added:
    * LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
    * LV_LOG_LEVEL_INFO        Log important events
    * LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
    * LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
    * LV_LOG_LEVEL_NONE        Do not log anything
    */
   #  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN

After enabling the log module and setting :c:macro:`LV_LOG_LEVEL` accordingly, the
output log is sent to the ``Serial`` port @ 115200 bps.

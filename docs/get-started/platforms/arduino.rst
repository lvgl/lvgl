=======
Arduino
=======

The `LVGL library <https://github.com/lvgl/lvgl>`__ is directly available as Arduino libraries.

Note that you need to choose a board powerful enough to run LVGL and
your GUI. See the `requirements of LVGL <https://docs.lvgl.io/master/intro/index.html#requirements>`__.

For example ESP32 is a good candidate to create UI's with LVGL.

Get the LVGL Arduino library
----------------------------

LVGL can be installed via the Arduino IDE Library Manager or as a .ZIP library.

You can `Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of LVGL from GitHub and simply copy it to Arduino's
library folder.

Set up drivers
--------------

To get started it's recommended to use `TFT_eSPI <https://github.com/Bodmer/TFT_eSPI>`__ library as a TFT
driver to simplify testing. To make it work, setup ``TFT_eSPI``
according to your TFT display type via editing either:

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
5. Set :c:macro:`LV_TICK_CUSTOM`

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

.. code:: c

   /*Log settings*/
   #define USE_LV_LOG      1   /*Enable/disable the log module*/
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

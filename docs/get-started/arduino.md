```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/get-started/arduino.md
```

# Arduino

The [core LVGL library](https://github.com/lvgl/lvgl) and the [examples](https://github.com/lvgl/lv_examples) are directly available as Arduino libraries.

Note that you need to choose a powerful enough board to run LVGL and your GUI.  See the [requirements of LVGL](https://docs.lvgl.io/latest/en/html/intro/index.html#requirements). 

For example ESP32 is a good candidate to create your UI with LVGL.


## Get the LVGL Ardunio library

LVGL can be installed via Arduino IDE Library Manager or as an .ZIP library.
It will also install [lv_exmaples](https://github.com/lvgl/lv_examples) which contains a lot of examples and demos to try LVGL.

## Set up drivers

To get started it's recommended to use [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library as a TFT driver to simplify testing. 
To make it work setup `TFT_eSPI` according to your TFT display type via editing either
- `User_Setup.h` 
- or by selecting a configuration in the `User_Setup_Select.h`

Both files are located in `TFT_eSPI` library's folder.

## Configure LVGL

LVGL has its own configuration file called `lv_conf.h`. When LVGL is installed the followings needs to be done to configure it:
1. Go to directory of the installed Arduino libraries
2. Go to `lvgl` and copy `lv_conf_template.h` as `lv_conf.h` into the Arduino Libraries directory next to the `lvgl` library folder.
3. Open `lv_conf.h` and change the first `#if 0` to `#if 1`
4. Set the resolution of your display in `LV_HOR_RES_MAX` and `LV_VER_RES_MAX`
5. Set the color depth of you display in `LV_COLOR_DEPTH`
6. Set `LV_TICK_CUSTOM 1`

## Configure the examples
`lv_examples` can be configures similarly to LVGL but it's configuration file is called `lv_ex_conf.h`. 
1. Go to directory of the installed Arduino libraries
2. Go to `lv_examples` and copy `lv_ex_template.h` as `lv_ex_conf.h` next to the `lv_examples` folder.
3. Open `lv_ex_conf.h` and change the first `#if 0` to `#if 1`
4. Enable the demos you want to use. (The small examples starting with `lv_ex_...()` are always enabled.)

## Initialize LVGL and run an example

Take a look at [LVGL_Arduino.ino](https://github.com/lvgl/lvgl/blob/master/examples/LVGL_Arduino.ino) to see how to initialize LVGL.
It also uses TFT_eSPI as driver.

In the INO file you can see how to register a display and a touch pad for LVGL and call an example.

Note that, there is no dedicated INO file for every example but you can call functions like `lv_ex_btn1()` or `lv_ex_slider1()` to run an example.
For the full list of examples see the [README of lv_examples](https://github.com/lvgl/lv_examples/blob/master/README.md).

## Debugging and logging

In case of trouble there are debug information inside LVGL. 
In the `LVGL_Arduino.ino` example there is `my_print` method, which allow to send this debug information to the serial interface. 
To enable this feature you have to edit `lv_conf.h` file and enable logging in section `log settings`:

```c
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
```

After enabling log module and setting LV_LOG_LEVEL accordingly the output log is sent to the `Serial` port @ 115200 Baud rate.


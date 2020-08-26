# LVGL Arduino examples

LVGL can be installed via Arduino IDE Library Manager or as an .ZIP library.
It will install [lv_exmaples](https://github.com/lvgl/lv_examples) which contains a lot of examples and demos to try LVGL.

## Example

There are simple examples which use the [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library as a TFT driver to simplify testing. 
To get all this to work you have to setup TFT_eSPI to work with your TFT display type via editing the `User_Setup.h` file in TFT_eSPI library folder, or by selecting your own configurtion in the `User_Setup_Select.h` file in TFT_eSPI library folder.

LVGL library has its own configuration file called `lv_conf.h`. When LVGL is installed to followings needs to be done to configure it:
1. Go to directory of the installed Arduno libraries
2. Go to `lvgl` and copy `lv_conf_template.h` as `lv_conf.h` next to the `src` folder.
3. Open `lv_conf.h` and change the first `#if 0` to `#if 1`
4. Set the resolution of your display in `LV_HOR_RES_MAX` and `LV_VER_RES_MAX`
5. Set the color depth of you display in `LV_COLOR_DEPTH`
6. Set `LV_TICK_CUSTOM 1`

## Debugging

In case of trouble there are debug informations inside LVGL. In the `ESP32_TFT_eSPI` example there is `my_print` method, which allow to send this debug informations to the serial interface. To enable this feature you have to edit `lv_conf.h` file and enable logging in section `log settings`:

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

After enabling log module and setting LV_LOG_LEVEL accordingly the output log is sent to the `Serial` port @ 115200 Bd.

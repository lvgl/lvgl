.. _arduino_sd:

==========
Arduino SD
==========

Enables reading and writing on SD cards.
Once an SD memory card is connected to the SPI interface of the Arduino or Genuino board you can create files
and read/write on them. You can also move through directories on the SD card..

Detailed introduction:

- https://www.arduino.cc/reference/en/libraries/sd/


Usage
-----

Enable :c:macro:`LV_USE_FS_ARDUINO_SD` and define a :c:macro:`LV_FS_ARDUINO_SD_LETTER` in ``lv_conf.h``.
You will need to initialize the SD card before LVGL can use it (i.e. :cpp:expr:`SD.begin(0, SPI, 40000000)`).


API
---

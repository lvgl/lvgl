.. _arduino_sd:

==========
Arduino SD
==========

Enables reading from and writing to SD cards.
Once an SD memory card is connected to the SPI interface of the Arduino or Genuino board, you can create files
and read from, and write to them.  You can also move through directories on the SD card.

For a detailed introduction, see:

- https://www.arduino.cc/reference/en/libraries/sd/


Usage
*****

Enable :c:macro:`LV_USE_FS_ARDUINO_SD` and define a :c:macro:`LV_FS_ARDUINO_SD_LETTER` in ``lv_conf.h``.
You will need to initialize the SD card before LVGL can use it (i.e. :cpp:expr:`SD.begin(0, SPI, 40000000)`).


API
***

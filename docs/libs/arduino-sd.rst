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

Enable :c:macro:`LV_USE_FS_ARDUINO_SD` and define a :c:macro`LV_FS_ARDUINO_SD_LETTER` in ``lv_conf.h``.
You probably need to configure the :c:macro:`LV_FS_ARDUINO_SD_CS_PIN` and :c:macro:`LV_FS_ARDUINO_SD_FREQUENCY` that
corresponds to the pin connected and the frequency used by the chip of the SD CARD.


API
---

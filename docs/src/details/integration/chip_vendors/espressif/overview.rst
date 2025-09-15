========
Overview
========

About
*****

Espressif Systems is a fabless chip manufacturer that produces the ESP32 series of 
system on chips, these chips can be based on Xtensa or Risc-V architectures, and they
feature the common set of analog and digital peripherals of a general purpose microcontroller
combined to a radio subsystem capable to run a Bluetooth and/or a WiFi stack.

Espressif System also provides its comprehensive software development kit (SDK) for
users to focus only in their applications, which is the IDF, all other 3rd party
support such as Arduino, Zephyr, NuttX and Micropython are built on top of the IDF or
some of its components.


Application Development
***********************

The LVGL is supported by Espressif SDK, that is it, the IDF as mentioned before, 
therefore ESP32 series of chips are supported on a different sets of frameworks
called by Espressif as 3rd party projects.

This guide will cover the usage of the LVGL using the Espressif-IDF software development
kit. Although ESP32 are supported on other frameworks also supported by LVGL, it 
is recommended the user to check the following pages: 

- :ref:`Arduino Framework  <arduino>`
- :ref:`Platformio  <platformio>`
- :ref:`MicroPython  <micropython>`
- :ref:`NuttX RTOS  <nuttx>`
- :ref:`Zephyr RTOS  <zephyr>`

On the pages above users would be able to check the respective getting started on
these platforms where ESP32, and other chips are abstracted by the framework.

Ready to use projects
*********************

For a quick start with LVGL and ESP32, the LVGL maintains an demo project compatible to
several ESP32 boards under its `LV Port for ESP32 <https://github.com/lvgl/lv_esp_idf>`.

Refer to the README.md files in this repository for quick build and 
flash instructions.

These demo projects use Espressif's Board Support Packages (BSPs). 
Additional BSPs and examples are available in the `esp-bsp <https://github.com/espressif/esp-bsp>`__ repository.


LVGL Support for ESP32 Graphical Peripherals
********************************************

Some of the ESP32 chips like the ESP32P4 family have built-in support
for driving display through standard interfaces like RGB and MIPI, 
the Espressif IDF (esp-idf), provides the necessary drivers, leaving 
to the user the responsibility to integrate them into the LVGL display
subsystem.

Espressif, via its component manager system, provides a ready to use
LVGL porting component, which is the recommended and preferred way of 
integrating input and output devices from the ESP32 chip to the LVGL
Display subsystem, this component is covered in
:ref:`Add LVGL to an ESP32 IDF project <lvgl_esp_idf>`

Besides the display controller, some ESP32 chips offer graphical hardware
acceleration peripherals like:

- :ref:`Support 2D Direct Memory Access (DMA2D) <esp_dma2d>`
- :ref:`Support for Pixel Processing Accelerator <esp_ppa>`

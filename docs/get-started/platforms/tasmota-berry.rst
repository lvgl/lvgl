=================
Tasmota and berry
=================

What is Tasmota?
----------------

`Tasmota <https://github.com/arendst/Tasmota>`__ is a widely used
open-source firmware for ESP8266 and EPS32 based devices. It supports a
wide variety of devices, sensors and integrations to Home Automation and
Cloud services. Tasmota firmware is downloaded more than 200,000 times
each month, and has an active and growing community.

Tasmota provides access to hundreds of supported devices, full support
of MQTT, HTTP(S), integration with major Home Automation systems, myriad
of sensors, IR, RF, Zigbee, Bluetooth, AWS IoT, Azure IoT, Alexa and
many more.

What is Berry?
--------------

`Berry <https://github.com/berry-lang/berry>`__ is a ultra-lightweight
dynamically typed embedded scripting language. It is designed for
lower-performance embedded devices. The interpreter of Berry include a
one-pass compiler and register-based VM, all the code is written in ANSI
C99. Berry offers a syntax very similar to Python, and is inspired from
LUA VM. It is fully integrated in Tasmota

Highlights of Berry
~~~~~~~~~~~~~~~~~~~

Berry has the following advantages:

- Lightweight: A well-optimized interpreter with very little resources. Ideal for use in microprocessors.
- Fast: optimized one-pass bytecode compiler and register-based virtual machine.
- Powerful: supports imperative programming, object-oriented programming, functional programming.
- Flexible: Berry is a dynamic type script, and it's intended for embedding in applications.
  It can provide good dynamic scalability for the host system.
- Simple: simple and natural syntax, support garbage collection, and easy to use FFI (foreign function interface).
- RAM saving: With compile-time object construction, most of the constant objects are stored
  in read-only code data segments, so the RAM usage of the interpreter is very low when it starts.

All features are detailed in the `Berry Reference Manual <https://github.com/berry-lang/berry/wiki/Reference>`__

--------------

Why LVGL + Tasmota + Berry?
---------------------------

In 2021, Tasmota added full support of LVGL for ESP32 based devices. It
also introduced the Berry scripting language, a small-footprint language
similar to Python and fully integrated in Tasmota.

A comprehensive mapping of LVGL in Berry language is now available,
similar to the mapping of Micropython. It allows to use +98% of all LVGL
features. It is also possible to write custom widgets in Berry.

Versions supported: LVGL v8.0.2, LodePNG v20201017, Freetype 2.10.4

Tasmota + Berry + LVGL could be used for:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- Fast prototyping GUI.
- Shortening the cycle of changing and fine-tuning the GUI.
- Modelling the GUI in a more abstract way by defining reusable composite objects, taking
  advantage of Berry's language features such as Inheritance, Closures, Exception Handling…
- Make LVGL accessible to a larger audience. No need to know C to create a nice GUI on an embedded system.

A higher level interface compatible with
`OpenHASP <https://github.com/HASwitchPlate/openHASP>`__
is also under development.

--------------

So what does it look like?
--------------------------

TL;DR: Similar to MicroPython, it's very much like the C API, but Object-Oriented for LVGL components.

Let's dive right into an example!

A simple example
~~~~~~~~~~~~~~~~

.. code:: python

   lv.start()                 # start LVGL
   scr = lv.screen_active()         # get default screen
   btn = lv.btn(scr)          # create button
   btn.center()
   label = lv.label(btn)      # create a label in the button
   label.set_text("Button")   # set a label to the button

How can I use it?
-----------------

You can start in less than 10 minutes on a M5Stack or equivalent device
in less than 10 minutes in this `short tutorial <https://tasmota.github.io/docs/LVGL_in_10_minutes/>`__

Where can I find more information?
----------------------------------

- `Tasmota Documentation <https://tasmota.github.io/docs/>`__
- `Berry Documentation <https://github.com/berry-lang/berry/wiki/Reference>`__
- `Tasmota LVGL Berry documentation <https://tasmota.github.io/docs/LVGL/>`__

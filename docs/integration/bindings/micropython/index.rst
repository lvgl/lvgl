===========
Micropython
===========

What is Micropython?
====================

`Micropython <http://micropython.org/>`__ is Python for
microcontrollers. Using Micropython, you can write Python3 code and run
it even on a bare metal architecture with limited resources.


Highlights of Micropython
=========================

- **Compact**: Fits and runs within just 256k of code space and 16k of RAM. No OS is needed, although you
  can also run it with an OS, if you want.
- **Compatible**: Strives to be as compatible as possible with normal Python (known as CPython).
- **Versatile**: Supports many architectures (x86, x86-64, ARM, ARM Thumb, Xtensa).
- **Interactive**: No need for the compile-flash-boot cycle. With the REPL (interactive prompt) you can type
  commands and execute them immediately, run scripts, etc.
- **Popular**: Many platforms are supported. The user base is growing bigger. Notable forks:

  - `MicroPython <https://github.com/micropython/micropython>`__
  - `CircuitPython <https://github.com/adafruit/circuitpython>`__
  - `MicroPython_ESP32_psRAM_LoBo <https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo>`__

- **Embedded Oriented**: Comes with modules specifically for embedded systems, such as the
  `machine module <https://docs.micropython.org/en/latest/library/machine.html#classes>`__
  for accessing low-level hardware (I/O pins, ADC, UART, SPI, I2C, RTC, Timers etc.)


Why Micropython + LVGL?
=======================

Micropython `does not have a good native high-level GUI library <https://forum.micropython.org/viewtopic.php?f=18&t=5543>`__.
LVGL is an `Object-Oriented Component Based <https://blog.lvgl.io/2018-12-13/extend-lvgl-objects>`__
high-level GUI library, which seems to be a natural candidate to map into a higher level language, such as Python.
LVGL is implemented in C and its APIs are in C.


Here are some advantages of using LVGL in Micropython
-----------------------------------------------------

- Develop GUI in Python, a very popular high level language. Use paradigms such as Object-Oriented Programming.
- Usually, GUI development requires multiple iterations to get things right. With C, each iteration consists of
  **``Change code`` > ``Build`` > ``Flash`` > ``Run``**. In Micropython it's just
  **``Change code`` > ``Run``** ! You can even run commands interactively using the
  `REPL <https://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop>`__ (the interactive prompt)


Micropython + LVGL could be used for
------------------------------------

- Fast prototyping GUI.
- Shortening the cycle of changing and fine-tuning the GUI.
- Modelling the GUI in a more abstract way by defining reusable composite objects, taking advantage of Python's language features
  such as Inheritance, Closures, List Comprehension, Generators, Exception Handling, Arbitrary Precision Integers and others.
- Make LVGL accessible to a larger audience. No need to know C to create a nice GUI on an embedded system. This goes well with
  `CircuitPython vision <https://learn.adafruit.com/welcome-to-circuitpython/what-is-circuitpython>`__.
  CircuitPython was designed with education in mind, to make it easier for new or inexperienced users to get started with
  embedded development.
- Creating tools to work with LVGL at a higher level (e.g. drag-and-drop designer).



So what does it look like?
==========================

It's very much like the C API, but Object-Oriented for LVGL components.

Let's dive right into an example!


A simple example
----------------

.. code:: python

   import lvgl as lv
   lv.init()
   scr = lv.obj()
   btn = lv.btn(scr)
   btn.align(lv.ALIGN.CENTER, 0, 0)
   label = lv.label(btn)
   label.set_text('Hello World!')
   lv.screen_load(scr)


How can I use it?
=================

Online Simulator
----------------

If you want to experiment with LVGL + Micropython without downloading
anything - you can use our online simulator! It's a fully functional
LVGL + Micropython that runs entirely in the browser and allows you to
edit a python script and run it.

`Click here to experiment on the online simulator <https://sim.lvgl.io/>`__

Many `LVGL examples <https://docs.lvgl.io/master/examples.html>`__ are available also for Micropython. Just click the link!


PC Simulator
------------

Micropython is ported to many platforms. One notable port is "unix", which allows you to build and run Micropython
(+LVGL) on a Linux machine. (On a Windows machine you might need Virtual Box or WSL or MinGW or Cygwin etc.)

`Click here to know more information about building and running the unix port <https://github.com/lvgl/lv_micropython>`__


Embedded Platforms
------------------

In the end, the goal is to run it all on an embedded platform. Both Micropython and LVGL can be used on many embedded
architectures. `lv_micropython <https://github.com/lvgl/lv_micropython>`__ is a fork of Micropython+LVGL and currently
supports Linux, ESP32, STM32 and RP2. It can be ported to any other platform supported by Micropython.

- You would also need display and input drivers. You can either use one of the existing drivers provided with lv_micropython,
  or you can create your own input/display drivers for your specific hardware.
- Drivers can be implemented either in C as a Micropython module, or in pure Python!

lv_micropython already contains these drivers:

- Display drivers:

  - SDL on Linux
  - X11 on Linux
  - ESP32 specific:

    - ILI9341
    - ILI9488
    - GC9A01
    - ST7789
    - ST7735

  - Generic (pure Python):

    - ILI9341
    - ST7789
    - ST7735

- Input drivers:

  - SDL
  - X11
  - XPT2046
  - FT6X36
  - ESP32 ADC with resistive touch


Where can I find more information?
==================================

- ``lv_micropython`` `README <https://github.com/lvgl/lv_micropython>`__
- ``lv_binding_micropython`` `README <https://github.com/lvgl/lv_binding_micropython>`__
- The `LVGL micropython forum <https://forum.lvgl.io/c/micropython>`__ (Feel free to ask anything!)
- At Micropython: `docs <http://docs.micropython.org/en/latest/>`__ and `forum <https://forum.micropython.org/>`__
- `Blog Post <https://blog.lvgl.io/2019-02-20/micropython-bindings>`__, a little outdated.


The Micropython Binding is auto generated!
==========================================

- LVGL is a git submodule inside `lv_micropython <https://github.com/lvgl/lv_micropython>`__
  (LVGL is a git submodule of `lv_binding_micropython <https://github.com/lvgl/lv_binding_micropython>`__
  which is itself a submodule of `lv_micropython <https://github.com/lvgl/lv_micropython>`__).
- When building lv_micropython, the public LVGL C API is scanned and Micropython API is auto-generated. That means that
  lv_micropython provides LVGL API for **any** LVGL version, and generally does not require code changes as LVGL evolves.


LVGL C API Coding Conventions
-----------------------------

For a summary of coding conventions to follow see the `CODING STYLE <CODING_STYLE>`__.



.. toctree::
   :maxdepth: 2

   memory_management
   callbacks


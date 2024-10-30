.. _introduction:

============
Introduction
============

LVGL (Light and Versatile Graphics Library) is a free and open-source graphics
library providing everything you need to create an embedded GUI with easy-to-use
graphical elements, beautiful visual effects, and a low memory footprint.


Key features
------------

- Powerful building blocks such as :ref:`buttons, charts, lists, sliders, images <widgets>`, etc.
- Advanced graphics with animations, anti-aliasing, opacity, smooth scrolling
- Various input devices such as touchpad, mouse, keyboard, encoder, etc.
- Multi-language support with UTF-8 encoding
- Multi-display support, even with mixed color formats
- Fully customizable graphic elements with CSS-like styles
- Hardware independent:  use with any microcontroller or display
- Scalable: able to operate with little memory (64 kB Flash, 16 kB RAM)
- :ref:`OS <threading>`, external memory and :ref:`GPU <draw>` are supported but not required
- Single frame buffer operation even with advanced graphic effects
- Written in C for maximal compatibility (C++ compatible)
- :ref:`Simulator <simulator>` to start embedded GUI design on a PC without embedded hardware
- User code developed under simulator can be shared with firmware to make UI development more efficient.
- Binding to :ref:`MicroPython`
- Tutorials, examples, themes for rapid GUI design
- Documentation is available online
- Free and open-source under MIT license


.. _requirements:

Requirements
------------

Basically, every modern controller which is able to drive a display is suitable to run LVGL. The minimal requirements are:

* 16, 32 or 64 bit microcontroller or processor
* > 16 MHz clock speed is recommended
* Flash/ROM: > 64 kB for the very essential components (> 180 kB is recommended)
* RAM:
    * Static RAM usage: ~2 kB depending on the used features and Widget types
    * stack: > 2kB (> 8 kB recommended)
    * Dynamic data (heap): > 2 KB (> 48 kB is recommended if using many GUI Widgets).
        Set by :c:macro:`LV_MEM_SIZE` in ``lv_conf.h``.
    * Display buffer:  > *"Horizontal resolution"* pixels (> 10 X *"Horizontal resolution"* is recommended)
    * One frame buffer in the MCU or in an external display controller
* C99 or newer compiler
* Basic C (or C++) knowledge:
    * `pointers <https://www.tutorialspoint.com/cprogramming/c_pointers.htm>`__.
    * `structs <https://www.tutorialspoint.com/cprogramming/c_structures.htm>`__.
    * `callbacks <https://www.geeksforgeeks.org/callbacks-in-c/>`__.

.. note::
    *Memory usage may vary depending on architecture, compiler and build options.*


License
-------

The LVGL project (including all repositories) is licensed under the `MIT license <https://github.com/lvgl/lvgl/blob/master/LICENCE.txt>`__.
This means you can use it even in commercial projects.

It is not mandatory, but we highly appreciate it if you write a few words about your project in the `My projects <https://forum.lvgl.io/c/my-projects/10>`__ category of the forum or a private message to `lvgl.io <https://lvgl.io/#contact>`__.

Although you can get LVGL for free there is a massive amount of work behind it. It's created by a group of volunteers who made it available for you in their free time.

To make the LVGL project sustainable, please consider :ref:`contributing` to the project.
You can choose from many different ways of contributing See :ref:`contributing` such as simply writing a tweet about you using LVGL, fixing bugs, translating the documentation, or even becoming a maintainer.


Repository layout
-----------------

All repositories of the LVGL project are hosted on `GitHub <https://github.com/lvgl>`_.

You will find these repositories there:

* `lvgl <https://github.com/lvgl/lvgl>`__: The library itself with many `examples <https://github.com/lvgl/lvgl/blob/master/examples/>`_ and `demos <https://github.com/lvgl/lvgl/blob/master/demos/>`__.
* `lv_drivers <https://github.com/lvgl/lv_drivers>`__: Display and input device drivers
* `blog <https://github.com/lvgl/blog>`__: Source of the `blog's site <https://blog.lvgl.io>`__
* `sim <https://github.com/lvgl/sim>`__: Source of the `online simulator's site <https://sim.lvgl.io>`__
* `lv_port_* <https://github.com/lvgl?q=lv_port&type=&language=>`__: LVGL ports to development boards or environments
* `lv_binding_* <https://github.com/lvgl?q=lv_binding&type=&language=l>`__: Bindings to other languages


Release policy
--------------

The core repositories follow the rules of `Semantic Versioning <https://semver.org/>`__:

* Major version: incompatible API changes. E.g. v5.0.0, v6.0.0
* Minor version: new but backward-compatible functionalities. E.g. v6.1.0, v6.2.0
* Patch version: backward-compatible bug fixes. E.g. v6.1.1, v6.1.2

Tags like `vX.Y.Z` are created for every release.


Release cycle
^^^^^^^^^^^^^

* Bug fixes: released on demand even weekly
* Minor releases: every 3-4 months
* Major releases: approximately yearly


Branches
^^^^^^^^

The core repositories have at least the following branches:

* `master`: latest version, patches are merged directly here
* `release/vX.Y`: stable versions of the minor releases
* `fix/some-description`: temporary branches for bug fixes
* `feat/some-description`: temporary branches for features


Change log
^^^^^^^^^^

The changes are recorded in :ref:`changelog`.


Version support
^^^^^^^^^^^^^^^

Before v8 the last minor release of each major series was supported for 1 year.
Starting from v8, every minor release is supported for 1 year.


+---------+--------------+--------------+--------+
| Version | Release date | Support end  | Active |
+=========+==============+==============+========+
|v5.3     | 1 Feb, 2019  | 1 Feb, 2020  | No     |
+---------+--------------+--------------+--------+
|v6.1     | 26 Nov, 2019 | 26 Nov, 2020 | No     |
+---------+--------------+--------------+--------+
|v7.11    | 16 Mar, 2021 | 16 Mar, 2022 | No     |
+---------+--------------+--------------+--------+
|v8.0     | 1 Jun, 2021  | 1 Jun, 2022  | No     |
+---------+--------------+--------------+--------+
|v8.1     | 10 Nov, 2021 | 10 Nov, 2022 | No     |
+---------+--------------+--------------+--------+
|v8.2     | 31 Jan, 2022 | 31 Jan, 2023 | No     |
+---------+--------------+--------------+--------+
|v8.3     | 6 July, 2022 | 1 Jan, 2025  | Yes    |
+---------+--------------+--------------+--------+
|v8.4     | Mar 19, 2024 | Mar 20, 2025 | Yes    |
+---------+--------------+--------------+--------+
|v9.0     | Jan 22, 2024 | Jan 22, 2025 | Yes    |
+---------+--------------+--------------+--------+
|v9.1     | Mar 20, 2024 | Mar 20, 2025 | Yes    |
+---------+--------------+--------------+--------+
|v9.2     | Aug 26, 2024 | Aug 26, 2025 | Yes    |
+---------+--------------+--------------+--------+


FAQ
---

Where can I ask questions?
^^^^^^^^^^^^^^^^^^^^^^^^^^

You can ask questions in the forum:  https://forum.lvgl.io/.

We use `GitHub issues <https://github.com/lvgl/lvgl/issues>`_ for development related discussion.
You should use them only if your question or issue is tightly related to the development of the library.

Before posting a question, please read this FAQ section since you might find the answer to your issue here as well.


Is my MCU/hardware supported?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Every MCU which is capable of driving a display via parallel port, SPI, RGB interface or anything else and fulfills the :ref:`requirements` is supported by LVGL.

This includes:

* "Common" MCUs like STM32F, STM32H, NXP Kinetis, LPC, iMX, dsPIC33, PIC32, SWM341 etc.
* Bluetooth, GSM, Wi-Fi modules like Nordic NRF, Espressif ESP32 and Raspberry Pi Pico W
* Linux with frame buffer device such as /dev/fb0. This includes Single-board computers like the Raspberry Pi
* Anything else with a strong enough MCU and a peripheral to drive a display


Is my display supported?
^^^^^^^^^^^^^^^^^^^^^^^^

LVGL needs just one simple driver function to copy an array of pixels into a given area of the display.
If you can do this with your display then you can use it with LVGL.

Some examples of the supported display types:

* TFTs with 16 or 24 bit color depth
* Monitors with an HDMI port
* Small monochrome displays
* Gray-scale displays
* even LED matrices
* or any other display where you can control the color/state of the pixels

See the :ref:`display` section to learn more.


LVGL doesn't start, randomly crashes or nothing is drawn on the display. What can be the problem?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Try increasing :c:macro:`LV_MEM_SIZE`.
* Be sure your display works without LVGL. E.g. paint it to red on start up.
* Enable :ref:`logging`.
* Enable assertions in ``lv_conf.h`` (``LV_USE_ASSERT_...``).
* If you use an RTOS:
   * Increase the stack size of the task that calls :cpp:func:`lv_timer_handler`.
   * Be sure you are using one of the methods for thread management as described in :ref:`threading`.


My display driver is not called. What have I missed?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Be sure you are calling :cpp:expr:`lv_tick_inc(x)` as prescribed in
:ref:`tick_interface` and are calling :cpp:func:`lv_timer_handler` as prescribed in
:ref:`timer_handler`.

Learn more in the :ref:`tick_interface` and :ref:`timer_handler` sections.


Why is the display driver called only once? Only the upper part of the display is refreshed.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Be sure you are calling :cpp:expr:`lv_display_flush_ready(drv)` at the end of your
"*display flush callback*" as per :ref:`flush_callback` section.


Why do I see only garbage on the screen?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is probably a bug in your display driver. Try the following code without using LVGL. You should see a square with red-blue gradient.

.. code-block:: c

    #define BUF_W 20
    #define BUF_H 10

    lv_color_t buf[BUF_W * BUF_H];
    lv_color_t * buf_p = buf;
    uint16_t x, y;
    for(y = 0; y < BUF_H; y++) {
        lv_color_t c = lv_color_mix(LV_COLOR_BLUE, LV_COLOR_RED, (y * 255) / BUF_H);
        for(x = 0; x < BUF_W; x++){
            (*buf_p) =  c;
            buf_p++;
        }
    }

    lv_area_t a;
    a.x1 = 10;
    a.y1 = 40;
    a.x2 = a.x1 + BUF_W - 1;
    a.y2 = a.y1 + BUF_H - 1;
    my_flush_cb(NULL, &a, buf);


Why do I see nonsense colors on the screen?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The configured LVGL color format is probably not compatible with your display's color format. Check :c:macro:`LV_COLOR_DEPTH` in *lv_conf.h*.


How do I speed up my UI?
^^^^^^^^^^^^^^^^^^^^^^^^

- Turn on compiler optimization and enable instruction- and data-caching if your MCU has them.
- Increase the size of the display buffer.
- Use two display buffers and flush the buffer with DMA (or similar peripheral) in the background.
- Increase the clock speed of the SPI or parallel port if you use them to drive the display.
- If your display has an SPI port consider changing to a model with a parallel interface because it has much higher throughput.
- Keep the display buffer in internal RAM (not in external SRAM) because LVGL uses it a lot and it should have fast access time.


How do I reduce flash/ROM usage?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can disable unused features (such as animations, file system, GPU etc.) and widget types in *lv_conf.h*.

If you are using GCC/CLANG you can add `-fdata-sections -ffunction-sections` compiler flags and `--gc-sections` linker flag to remove unused functions and variables from the final binary. If possible, add the `-flto` compiler flag to enable link-time-optimisation together with `-Os` for GCC or `-Oz` for CLANG.


How do I reduce RAM usage?
^^^^^^^^^^^^^^^^^^^^^^^^^^

* Lower the size of the *Display buffer*.
* Reduce :c:macro:`LV_MEM_SIZE` in *lv_conf.h*. This memory is used when you create Widgets like buttons, labels, etc.
* To work with lower :c:macro:`LV_MEM_SIZE` you can create Widgets only when required and delete them when they are not needed anymore.


How do I use LVGL with an operating system?
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To work with an operating system where tasks can interrupt each other (preemptively),
you must ensure that no LVGL function call be called while another LVGL call is in
progress.  There are several ways to do this.  See the :ref:`threading` section to
learn more.


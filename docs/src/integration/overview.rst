.. _adding_lvgl_to_your_project:

========
Overview
========

Getting LVGL
************

Clone or Download
------------------

LVGL is available on GitHub: https://github.com/lvgl/lvgl.

You can clone it or
`Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of the library from GitHub.

In this case, you can copy the ``lvgl`` into your project, use a
built-in display driver or write your own, and compile LVGL with the
rest of your source code.

Frameworks and Package Registries
---------------------------------

LVGL is also available as:

- `Arduino library <https://docs.lvgl.io/master/details/integration/framework/arduino.html>`__
- `PlatformIO package <https://registry.platformio.org/libraries/lvgl/lvgl>`__
- `Zephyr library <https://docs.lvgl.io/master/details/integration/os/zephyr.html>`__
- `ESP-IDF (ESP32) component <https://components.espressif.com/components/lvgl/lvgl>`__
- `NXP MCUXpresso component <https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY>`__
- `NuttX library <https://docs.lvgl.io/master/details/integration/os/nuttx.html>`__
- `RT-Thread RTOS <https://docs.lvgl.io/master/details/integration/os/rt-thread.html>`__
- CMSIS-Pack
- `RIOT OS package <https://doc.riot-os.org/group__pkg__lvgl.html#details>`__

Usually, LVGL is ready to use in these frameworks. You can also find a description
at :ref:`integration_index`.

Folder Structure
----------------

The graphics library itself is the ``lvgl`` directory. It contains several
directories, but to use LVGL, you only need the ``.c`` and ``.h`` files under
the ``src`` directory, plus ``lvgl/lvgl.h``, and ``lvgl/lv_version.h``.

The ``lvgl`` directory also contains an ``examples`` and a ``demos``
directory. If your project needs examples and/or demos, add these
directories to your project and enable them in ``lv_conf.h`` or ``Kconfig``
(``LV_BUILD_DEMOS`` and ``LV_BUILD_EXAMPLES``).

Configuration
*************

LVGL has several compile-time settings to

- set default values
- enable Widgets
- enable GPU support
- enable 3rd party library support
- enable operating system support
- and many more

These compile-time settings can be configured in 3 ways:

1. ``lv_conf.h``: a header file where ``#define``\ s can be adjusted
2. Kconfig
3. Defines passed to the compiler (usually ``-D...``)

The above options can also be mixed.

.. _lv_conf:

lv_conf.h
---------

When setting up your project for the first time, copy ``lvgl/lv_conf_template.h`` to
``lv_conf.h`` next to the ``lvgl`` folder. Change the first ``#if 0`` to ``1`` to
enable the file's content and set the :c:macro:`LV_COLOR_DEPTH` define to align with
the color depth used by your display panel.

Regarding the other config options, see comments in ``lv_conf.h``.

The layout of the files should look like this::

    lvgl/
    lv_conf.h
    other files and folders in your project

If more control is needed, any of the following can be used as well:

1. Set the :c:macro:`LV_CONF_INCLUDE_SIMPLE` define to your compiler
options (e.g. ``-DLV_CONF_INCLUDE_SIMPLE`` for GCC compiler) and set the
include path manually (e.g. ``-I../include/gui``). In this case, LVGL
will attempt to include ``lv_conf.h`` simply with ``#include "lv_conf.h"``
instead of ``lvgl/lvgl.h``.

2. Set a custom path via the :c:macro:`LV_CONF_PATH` define.
For example: ``-DLV_CONF_PATH="/home/joe/my_project/my_custom_conf.h"``.

3. Delete some settings from ``lv_conf.h`` and set them via compile options.
For example: ``-DLV_COLOR_DEPTH=32 -DLV_USE_BUTTON=1``.

4. To fully skip ``lv_conf.h`` and use only compiler options (see point 3),
define :c:macro:`LV_CONF_SKIP` as a compiler option. Unset options will get a
default value which is the same as the content of ``lv_conf_template.h``.

Kconfig
-------

When Kconfig is used, LVGL is usually integrated into a larger project and
this parent project's Kconfig tooling is used, where LVGL's
`Kconfig file <https://github.com/lvgl/lvgl/blob/master/Kconfig>`__ is
included. For example, when LVGL is used in ESP-IDF, it can be configured in
``idf.py menuconfig``.

When LVGL is compiled to a static library, it might make sense to configure LVGL
on its own via Kconfig as well. For now, this is only available using CMake.

Under the hood, it uses ``kconfiglib``, Kconfig's Python port to be able to use it
across different platforms. ``kconfiglib`` offers the Python API and some CLI
commands. Here is a list of some useful commands:

- ``menuconfig``: Opens a console menu interface to modify the configuration values.
- ``guiconfig`` (needs ``tkinter``): Opens a graphical interface to modify the
  configuration values.
- ``savedefconfig``: Saves the current .config as a defconfig, listing only non-default
  values.
- ``alldefconfig``: Creates a .config with all default values.
- ``genconfig``: Generates a C header from the config, following ``autoconf.h`` format.

Usage
~~~~~

1. Install the prerequisites using ``scripts/install_prerequisites.sh/bat``.
2. Create the configuration (.config)

.. code-block:: shell

    cd <lvgl_repo>
    menuconfig

Make changes to the config and exit using ``Esc`` or ``Q``, and save your configuration.
The ``.config`` file is now created and lists the configuration values.

3. Run CMake with the ``-DLV_USE_KCONFIG=ON`` flag:

.. code-block:: shell

    cd <lvgl_repo>
    cmake -B build -DLV_USE_KCONFIG=ON
    cmake --build build

To use a ``defconfig`` file, one can use the
``-DLV_DEFCONFIG_PATH=<path_to_defconfig>`` flag.

Some defconfigs are available in the ``configs/defconfigs`` folder. However, new
``defconfig`` files can also be created easily:

.. code-block:: shell

    cd <lvgl_repo>
    menuconfig # make your changes to the default config
    savedefconfig
    cp defconfig configs/defconfigs/my_custom_defconfig # save it where you want
    # Then use it to build LVGL
    cmake -B build -DLV_USE_KCONFIG=ON -DLV_DEFCONFIG_PATH=configs/defconfigs/my_custom_defconfig
    cmake --build build

.. _connecting_lvgl:

Connecting to Hardware
**********************

Several frameworks integrate LVGL deeply, and all the drivers are already in place.
This is the case in :ref:`Zephyr <zephyr>`, :ref:`ESP-IDF <espressif>`,
:ref:`NuttX <nuttx>`, :ref:`RT-Thread <rt_thread>`, :ref:`NXP <nxp>`,
:ref:`Renesas FSP <renesas>`, etc.

LVGL also comes with many built-in display and input device drivers for on-chip
:ref:`LCD peripheries <chip_vendors>`, :ref:`Embedded Linux <embedded_linux>`,
:ref:`external display controllers <display controllers>`, and many more.
These drivers do the heavy lifting for the drivers and also serve as references
for custom drivers.

If the existing support in frameworks or the drivers is not enough, setting up
everything from scratch is also simple. The process can be read in the following.

Initialization
--------------

1. Include ``lvgl/lvgl.h``
2. Initialize your hardware (clock, peripherals, etc.)
3. Call ``lv_init()`` to initialize LVGL

.. _tick_interface:

Tick Interface
--------------

Set the tick for LVGL by calling ``lv_tick_inc(x)`` in a timer interrupt every
``x`` milliseconds, or set a callback that returns the milliseconds elapsed
since startup with ``lv_tick_set_cb(my_cb)``. Many platforms have built-in
functions that can be used as they are. For example:

- SDL:  ``lv_tick_set_cb(SDL_GetTicks);``
- Arduino:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is:
  ``static uint32_t my_tick_get_cb(void) { return millis(); }``
- FreeRTOS:  ``lv_tick_set_cb(xTaskGetTickCount);``
- STM32:  ``lv_tick_set_cb(HAL_GetTick);``
- ESP32:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is a
  wrapper for ``esp_timer_get_time() / 1000;``

Displays and Input Devices
--------------------------

Create a **Display** (``lv_display``), set the buffers, and the flush callback.
In practice, this means implementing a single function that can show the rendered
image on the screen. It is called a ``flush`` callback. To learn more about
buffering options, see implementation examples and learn more about all the
features in :ref:`display`.

Add **Input devices** if needed (touchpad, external buttons, keyboard, etc.) by
creating ``lv_indev``\ s. To do so, a single ``read`` callback needs to be implemented
which returns the state of the given input device. Read more about the input device
types, their features, and check the examples at :ref:`indev`.

.. _timer_handler:

Timer Handler
-------------

All the main tasks of LVGL are implemented as software timers handled by LVGL.
There are timers for:

- rendering
- input device reading
- animation updates
- timers occasionally used by widgets
- user-created timers
- etc.

See the :ref:`timer` section to learn more about timers.

To process the timers of LVGL, you need to call :cpp:func:`lv_timer_handler`
periodically in one of the following ways:

- in the ``while(1)`` loop of the ``main()`` function, or
- in an OS task periodically. (See :ref:`threading`.)

In the simplest case, it can be done like this:

.. code-block:: c

   while(1) {
     lv_timer_handler(); /*Might return immediately or execute some timers*/
     lv_sleep_ms(5);
   }

If ``LV_USE_OS`` is set, :cpp:expr:`lv_sleep_ms()` will be the sleep function
provided by the operating system, otherwise it will fall back to a blocking delay.
Of course, you can use any custom delay, wait, or sleep functions instead.

Sleep Management
~~~~~~~~~~~~~~~~

To better control the delay/sleep time, :cpp:func:`lv_timer_handler`
returns the remaining time until the next timer:

.. code-block:: c

   while(1) {
     uint32_t time_till_next = lv_timer_handler();

     /*If there is nothing to do now, check again a little bit later.*/
     if(time_till_next == LV_NO_TIMER_READY) {
        time_till_next = LV_DEF_REFR_PERIOD; /*33 ms by default in lv_conf.h*/
     }
     lv_sleep_ms(time_till_next); /*Sleep the thread*/
   }

:cpp:func:`lv_timer_handler` will return :c:macro:`LV_NO_TIMER_READY` (``UINT32_MAX``)
if there are no running timers. This can happen if there is nothing to redraw, there are
no indevs or they are disabled with :cpp:expr:`lv_indev_enable()`, there are no running
animations, and no user-created timers.

When :c:macro:`LV_NO_TIMER_READY` is returned, special handling is needed to make
LVGL run again later:

- don’t sleep forever, just for a shorter time to check again a little bit later, or
- wait for an event that you will trigger when LVGL needs to run again, or
- sleep the CPU (not just the thread)

Also check the :ref:`lvgl_and_threads` section of the documentation to learn more
about the considerations when using LVGL in an operating system.


Sleep Management
****************

The MCU can go to sleep when no user input has been received for a certain period.
In this case, the main ``while(1)`` could look like this:

.. code-block:: c

    while(1) {
        /* Normal operation (no sleep) if < 5 sec inactivity */
        if(lv_display_get_inactive_time(NULL) < 5000) {
            lv_timer_handler();
        }
        /* Sleep after 5 sec inactivity */
        else {
           my_device_sleep(); /* Sleep the device, execution stops here */
        }
        lv_sleep_ms(5);
    }

In addition to :cpp:func:`lv_display_get_inactive_time`, you can check
:cpp:func:`lv_anim_count_running` to see if all animations have finished.

.. _threading:

Operating Systems and Threads
*****************************

LVGL is **not thread-safe**.

That is, while LVGL is executing a function, you cannot call another
LVGL function from another thread. This includes calls
to :cpp:func:`lv_timer_handler`.

Typically, this means for example, if you want to set a label's text using
:cpp:func:`lv_label_set_text` in one thread while in another thread
:cpp:func:`lv_timer_handler` is running, the same widget
can be accessed concurrently, causing issues. The same applies to creating
and deleting widgets in different threads.

Solution
--------

The solution is simple: before calling LVGL functions, take a mutex and
release the mutex after the functions.

LVGL has some helper functions to make it even simpler. If :c:macro:`LV_USE_OS`
is set to something other than ``LV_OS_NONE`` in ``lv_conf.h``, you can use
:cpp:func:`lv_lock()` and :cpp:func:`lv_unlock()`. :cpp:func:`lv_timer_handler`
calls these internally. Here is an example:

.. code-block:: c

    void main_ui_thread(void)
    {
        while(1) {
            lv_timer_handler(); /* lv_lock/lv_unlock is called internally */
            lv_sleep_ms(5);
        }
    }

    void other_thread(void)
    {
        lv_lock();
        lv_obj_t * label = lv_label_create(lv_screen_active());
        lv_unlock();

        int cnt = 0;
        while(1) {
            lv_lock();
            lv_label_set_text_fmt(label, "%d", cnt++);
            /*Call more functions if needed*/
            lv_unlock();
            lv_sleep_ms(2000);
        }
    }

Exceptions
----------

There are some exceptions when no locking/protecting is needed.

:ref:`event callbacks <events>`, :ref:`timer callbacks <timer>`,
:ref:`animation callbacks <animation>`, and callbacks passed to LVGL functions
in general are called sequentially from :cpp:func:`lv_timer_handler`, therefore
no special consideration is required as they are already protected in
:cpp:func:`lv_timer_handler`.

Also, :cpp:func:`lv_tick_inc` and :cpp:func:`lv_display_flush_ready` are implemented
in a special way; therefore, they can be called from any thread without issues.

Compiling LVGL
**************

In general, it's trivial to build LVGL: just compile it as you would compile
the other source files of your project.

LVGL has built-in support for
- :ref:`make <build_make>`
- :ref:`CMake <build_cmake>`
- and Managed builds: when the IDE just globs all the files from the ``lvgl`` folder


.. _lvgl_and_threads:

Operating System Support
************************

LVGL is **not thread-safe**.

That means it is the programmer's responsibility to see that no LVGL function is
called while another LVGL call is in progress in another thread.  This includes calls
to :cpp:func:`lv_timer_handler`.

Assuming the above is the case, it is safe to call LVGL functions in

- :ref:`event callbacks <events>`, and in
- :ref:`timer callbacks <timer>`

because the thread that drives both of these is the thread that calls
:cpp:func:`lv_timer_handler`.

**Reason:**

LVGL manages many complex data structures, and those structures are "system
resources" that must be protected from being "seen" by other threads in an
inconsistent state.  A high percentage LVGL functions (functions that start with
``lv_``) either read from or change those data structures.  Those that change them
place the data in an inconsistent state during call execution (because such changes are
multi-step sequences), but return them to a consistent state before those functions
return.  For this reason, execution of each LVGL function must be allowed to complete
before any other LVGL function is started.

**Exceptions to the Above:**

These two LVGL functions may be called from any thread:

- :cpp:func:`lv_tick_inc` (if writing to a ``uint32_t`` is atomic on your
  platform; see :ref:`tick_interface` for more information) and
- :cpp:func:`lv_display_flush_ready` (:ref:`flush_callback` for more information)




Use of MUTEXes requires:

1.  acquiring the MUTEX (locking it) before each LVGL call (or group of calls), and

2.  releasing the MUTEX (unlocking it) afterwards.

If your OS is integrated with LVGL (the macro :c:macro:`LV_USE_OS` has a value
other than ``LV_OS_NONE`` in ``lv_conf.h``) you can use :cpp:func:`lv_lock()` and
:cpp:func:`lv_unlock()` to perform #1 and #2.

When this is the case, :cpp:func:`lv_timer_handler` calls :cpp:func:`lv_lock()`
and :cpp:func:`lv_unlock()` internally, so you do not have to bracket your
calls to :cpp:func:`lv_timer_handler` with them.

If your OS is NOT integrated with LVGL, then these calls either return
immediately with no effect, or are optimized away by the linker.

This pseudocode illustrates the concept of using a MUTEX:

.. code-block:: c

    void lvgl_thread(void)
    {
        while(1) {
            uint32_t time_till_next;
            time_till_next = lv_timer_handler(); /* lv_lock/lv_unlock is called internally */
            if(time_till_next == LV_NO_TIMER_READY) time_till_next = LV_DEF_REFR_PERIOD; /*try again soon because the other thread can make the timer ready*/
            thread_sleep(time_till_next); /* sleep for a while */
        }
    }

    void other_thread(void)
    {
        /* You must always hold (lock) the MUTEX while calling LVGL functions. */
        lv_lock();
        lv_obj_t *img = lv_image_create(lv_screen_active());
        lv_unlock();

        while(1) {
            lv_lock();
            /* Change to next image. */
            lv_image_set_src(img, next_image);
            lv_unlock();
            thread_sleep(2000);
        }
    }


Multiple Instances
******************

It is possible to run multiple, independent instances of LVGL in the same firmware.
To enable its multi-instance feature, set :c:macro:`LV_GLOBAL_CUSTOM` in ``lv_conf.h``
and provide a custom function to :cpp:func:`lv_global_default` using ``__thread`` or
``pthread_key_t``. This allows running multiple LVGL instances by storing LVGL's
global variables in TLS (Thread-Local Storage).

For example:

.. code-block:: c

    lv_global_t * lv_global_default(void)
    {
        static __thread lv_global_t lv_global;
        return &lv_global;
    }



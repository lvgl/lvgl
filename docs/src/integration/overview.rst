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

In this case you can copy the ``lvgl`` into your project, use a
built-in display driver or write yours, and compile LVGL with the
rest of your source code.

Frameworks and Package Registries
--------------------------------

LVGL is also available as:
- [Arduino library](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.lvgl.io/master/details/integration/os/zephyr.html)
- [ESP-IDF(ESP32) component](https://components.espressif.com/components/lvgl/lvgl)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- CMSIS-Pack
- [RIOT OS package](https://doc.riot-os.org/group__pkg__lvgl.html#details)

Usually LVGL is ready to use in these frameworks. You can also find description :ref:`integration_index`

Folder Structure
----------------

The graphics library itself is the ``lvgl`` directory.  It contains several
directories but to use LVGL you only need the ``.c`` and ``.h`` files under
the ``src`` directory, plus ``lvgl/lvgl.h``, and ``lvgl/lv_version.h``.

The ``lvgl`` directory also contains an ``examples`` and a ``demos``
directory. If your project needs examples and/or demos, add these
directories to your project and enable them in ``lv_conf.h`` or ``Kconfig``
(``LV_BUILD_DEMOS`` and ``LV_BUILD_EXAMPLES``)


Configuration
*************

LVGL's compile time settings can be configured in 3 ways:

1. ``lv_conf.h``: a header file where ``#define``\ s can be adujsted
2. Kconfig
3. Defines passed to the compier (Usually ``-D...``)

These can be mixed too.

.. _lv_conf:

lv_conf.h
---------

When setting up your project for the first time, copy ``lvgl/lv_conf_template.h`` to
``lv_conf.h`` next to the ``lvgl`` folder.  Change the first ``#if 0`` to ``1`` to
enable the file's content and set the :c:macro:`LV_COLOR_DEPTH` define to align with
the color depth used by your display panel.

Regarding the other config options see comments in ``lv_conf.h``.

The layout of the files should look like this::

    lvgl/
    lv_conf.h
    other files and folders in your project

If more control is needed any of the followings can be used as well:

1. Set the :c:macro:`LV_CONF_INCLUDE_SIMPLE` define to your compiler
options (e.g. ``-DLV_CONF_INCLUDE_SIMPLE`` for GCC compiler) and set the
include path manually (e.g. ``-I../include/gui``). In this case LVGL
will attempt to include ``lv_conf.h`` simply with ``#include "lv_conf.h"``
instead of ``lvgl/lvgl.h``

2. Set a custom path via the :c:macro:`LV_CONF_PATH` define. For example
``-DLV_CONF_PATH="/home/joe/my_project/my_custom_conf.h"``.

3. Delete some settings from ``lv_conf.h`` and set them via compile options.
For example ``"-DLV_COLOR_DEPTH=32 -DLV_USE_BUTTON=1"``.

4. To fully skip ``lv_conf.h`` and use only compiler options (see point 3)
define :c:macro:`LV_CONF_SKIP` as a compiler option. Unset options will get a
default value which is the same as the content of ``lv_conf_template.h``.


Kconfig
-------

When Kconfig is used LVGL is usually integrated into a larger project and
this parent project's Kconfig tooling is used, where LVGL's
`Kconfig file <https://github.com/lvgl/lvgl/blob/master/Kconfig>`__ is
included. For example when LVGL is used in ESP-IDF it can configures in
``idf.py menuconfig``.

When LVGL is complied to a static library it might make sense to configure LVGL
on its own via Kconfig as well. For now, this is only available using CMake.

Under the hood, it uses ``kconfiglib``, Kconfig's Python port to be able to use it
across different platforms. The ``kconfiglib`` offers the python API and some CLI
commands. Here is a list of some useful commands:

- ``menuconfig``: Opens a console menu interface to modify the configuration values.
- ``guiconfig`` (needs ``tkinter``): Opens a graphical interface to modify the configuration values.
- ``savedefconfig``: Saves the current .config as a defconfig, listing only non-default values.
- ``alldefconfig``: Creates a .config with all default values.
- ``genconfig``: Generates a C header from the config, following ``autoconf.h`` format.

Usage
~~~~~

1. Install the prerequisites using ``scripts/install_prerequisites.sh/bat``.
2. Create the configuration (.config)

.. code-block:: shell

    cd <lvgl_repo>
    menuconfig

Make changes to the config and exit using `Esc` or `Q`, and save your c
onfiguration. The ``.config`` file is now created and lists the configuration values.

3. Run cmake with the ``-DLV_USE_KCONFIG=ON`` flag:

.. code-block:: shell

    cd <lvgl_repo>
    cmake -B build -DLV_USE_KCONFIG=ON
    cmake --build build

To use a ``defconfig`` file, one can use the
``-DLV_DEFCONFIG_PATH=<path_to_defconfig>`` flag.

Some defconfigs are available in ``configs/defconfigs`` folder, however new
``defconfig`` can also be created easily as well:

.. code-block:: shell

    cd <lvgl_repo>
    menuconfig # make your changes to the default config
    savedefconfig
    cp defconfig configs/defconfigs/my_custom_defconfig # save it where you want
    # Then use it to build LVGL
    cmake -B build -DLV_USE_KCONFIG=ON -DLV_DEFCONFIG_PATH=configs/defconfigs/my_custom_defconfig
    cmake --build build


.. _connecting_lvgl:

Connecting LVGL to Your Hardware
********************************

Several frameworks integrated LVGL deeply and all the drivers are already in place.
This the case in Zephyr, ESP-IDF, NuttX, RT-Thread, NXP, Renesas FSP, etc.

LVGL also comes with many built-in display and input device drivers for chip peritheries,
embedded Linux, external display controllers, and many more. These drivers do the heavy
lifiting for the drivers and also serve as reference for custom drivers.

If the existing support in frameworks or the driver are not enough, settinf up everything from scratch
is also simple:

Basic Setup
-----------

1. Include ``lvgl/lvgl.h``
2. Initialize your hardware (clock, perhepheiries, etc)
3. Call ``lv_init()`` to initialize LVGL

Tick Interface
--------------
Set the tick for LVGL by calling ``lv_tick_inc(x)`` in timer interrupt every
 ``x`` millisconds or set a callback that return the millisecinds elapsed
since startup with ``lv_tick_set_cb(my_cb)``. Many platforms have built-in
functions that can be used as they are.  For example:

    - SDL:  ``lv_tick_set_cb(SDL_GetTicks);``
    - Arduino:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is:
      ``static uint32_t my_tick_get_cb(void) { return millis(); }``
    - FreeRTOS:  ``lv_tick_set_cb(xTaskGetTickCount);``
    - STM32:  ``lv_tick_set_cb(HAL_GetTick);``
    - ESP32:  ``lv_tick_set_cb(my_tick_get_cb);``, where ``my_tick_get_cb`` is a
      wrapper for ``esp_timer_get_time() / 1000;``


Displays and Input Devices
--------------------------

5. Create an ``lv_display``, set the buffers and the flush callback. Read more
   at  :ref:`display`.
6. Add input devices if needed (touchpad, external buttns, keyboard, etc) by
   creating ``lv_indev``\ s. Read more at :ref:`indev`.



.. _timer_handler:


Timer Handler
-------------

All the main tasks of LVGL are implemented as timers:

- rendering
- input device polling
- animations
- etc


See :ref:`timer` section to learn more about timers.

To process the timers of LVGL you need to call :cpp:func:`lv_timer_handler`
periodically in one of the following ways:

- *while(1)* of *main()* function, or
- an OS task periodically.  (See :ref:`lvgl_and_threads`.)

In the most simple case it can done like this:

.. code-block:: c

   while(1) {
     lv_timer_handler(); /*Might return immediately or execute some timers*/
     lv_sleep_ms(5);
   }


If ``LV_USE_OS`` is set, :cpp:expr:`lv_sleep_ms()` will the sleep function
provided by the operating system (else it will fall back to a blocking delay).
Of course you can use and custom delay, wait, or sleep functions instead.

To better control the delay/sleep time :cpp:func:`lv_timer_handler`
returns the remaining time until the next timer:

.. code-block:: c

   while(1) {
     uint32_t time_till_next = lv_timer_handler();

     /*If there is nothing to do now, check again a little bit later.*/
     if(time_till_next == LV_NO_TIMER_READY) {
        time_till_next = LV_DEF_REFR_PERIOD; /*33 ms by default in lv_conf.h*/
     }
     lv_sleep_ms(time_till_next);
   }



.. _timer_handler_no_timer_ready:

:cpp:func:`lv_timer_handler` will return :c:macro:`LV_NO_TIMER_READY` (``UINT32_MAX``)
if there are no running timers. This can happen if there is nothig the redraw, there are
no indevs or they are disabled with :cpp:expr:`lv_indev_enable()`, there are no running
animations, and user-created timers.

That is when :c:macro:`LV_NO_TIMER_READY` is returned a special handling is needed to make
LVGL running again:

- don't sleep forever, just for a shorter time to check again a little bit later
- or wait for an operating system event that you will trigger when LVGL needs to run again

Also check the :ref:`lvgl_and_threads` section of the documentation to learn more
about the considerations when using LVGL in an operating system.


Sleep Management
****************

The MCU can go to sleep when no user input has been received for a certain period.
In this case, the main ``while(1)`` could look like this:

.. code-block:: c

    while(1) {
        /* Normal operation (no sleep) in < 1 sec inactivity */
        if(lv_display_get_inactive_time(NULL) < 1000) {
            lv_timer_handler();
        }
        /* Sleep after 1 sec inactivity */
        else {
            timer_stop();   /* Stop the timer where lv_tick_inc() is called */
            sleep();        /* Sleep the MCU */
        }
        my_delay_ms(5);
    }

You should also add the following lines to your input device read
function to signal a wake-up (press, touch, click, etc.) has happened:

.. code-block:: c

    lv_tick_inc(LV_DEF_REFR_PERIOD);  /* Force task execution on wake-up */
    timer_start();                    /* Restart timer where lv_tick_inc() is called */
    lv_timer_handler();               /* Call `lv_timer_handler()` manually to process the wake-up event */

In addition to :cpp:func:`lv_display_get_inactive_time` you can check
:cpp:func:`lv_anim_count_running` to see if all animations have finished.




Compiling LVGL
**************

In general it's trivial to build LVGL: just compile it as you would compile
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




Multiple Instances of LVGL
~~~~~~~~~~~~~~~~~~~~~~~~~~

It is possible to run multiple, independent instances of LVGL.  To enable its
multi-instance feature, set :c:macro:`LV_GLOBAL_CUSTOM` in ``lv_conf.h``
and provide a custom function to :cpp:func:`lv_global_default` using ``__thread`` or
``pthread_key_t``.  It will allow running multiple LVGL instances by storing LVGL's
global variables in TLS (Thread-Local Storage).

For example:

.. code-block:: c

    lv_global_t * lv_global_default(void)
    {
        static __thread lv_global_t lv_global;
        return &lv_global;
    }



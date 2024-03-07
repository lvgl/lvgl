================
Set up a project
================

Get the library
---------------

LVGL is available on GitHub: https://github.com/lvgl/lvgl.

You can clone it or
`Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of the library from GitHub.

Add lvgl to your project
------------------------

The graphics library itself is the ``lvgl`` directory. It contains a
couple of folders but to use ``lvgl`` you only need the ``.c`` and ``.h``
files in the ``src`` folder.

Automatically add files
~~~~~~~~~~~~~~~~~~~~~~~

If your IDE automatically adds the files from the folders copied to the
project folder (as Eclipse or VSCode does), you can simply copy the
``lvgl`` folder as it is into your project.

Make and CMake
~~~~~~~~~~~~~~

LVGL also supports ``make`` and ``CMake`` build systems out of the box.
To add LVGL to your Makefile based build system add these lines to your
main Makefile:

.. code:: make

   LVGL_DIR_NAME ?= lvgl     #The name of the lvgl folder (change this if you have renamed it)
   LVGL_DIR ?= ${shell pwd}  #The path where the lvgl folder is
   include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

For integration with CMake take a look this section of the
`Documentation </integration/build/cmake>`__.

Other platforms and tools
~~~~~~~~~~~~~~~~~~~~~~~~~

The `Get started </integartion/index>`__ section contains many platform
specific descriptions e.g. for ESP32, Arduino, NXP, RT-Thread, NuttX,
etc.

Demos and Examples
~~~~~~~~~~~~~~~~~~

The ``lvgl`` folder also contains an ``examples`` and a ``demos``
folder. If you needed to add the source files manually to your project,
you can do the same with the source files of these two folders too.
``make`` and ``CMake`` handles the examples and demos, so no extra
action required in these cases.

Configuration file
------------------

There is a configuration header file for LVGL called **lv_conf.h**. You
can modify this header to set the library's basic behavior, disable unused
modules and features, adjust the size of buffers in compile-time,
etc.

To get ``lv_conf.h`` **copy lvgl/lv_conf_template.h** next to the
``lvgl`` directory and rename it to *lv_conf.h*. Open the file and
change the ``#if 0`` at the beginning to ``#if 1`` to enable its
content. So the layout of the files should look like this:

::

   |-lvgl
   |-lv_conf.h
   |-other files and folders

Comments in the config file explain the meaning of the options. Be sure
to set at least :c:macro:`LV_COLOR_DEPTH` according to your display's color
depth. Note that, the examples and demos explicitly need to be enabled
in ``lv_conf.h``.

Alternatively, ``lv_conf.h`` can be copied to another place but then you
should add the :c:macro:`LV_CONF_INCLUDE_SIMPLE` define to your compiler
options (e.g. ``-DLV_CONF_INCLUDE_SIMPLE`` for GCC compiler) and set the
include path manually (e.g. ``-I../include/gui``). In this case LVGL
will attempt to include ``lv_conf.h`` simply with
``#include "lv_conf.h"``.

You can even use a different name for ``lv_conf.h``. The custom path can
be set via the :c:macro:`LV_CONF_PATH` define. For example
``-DLV_CONF_PATH="/home/joe/my_project/my_custom_conf.h"``

If :c:macro:`LV_CONF_SKIP` is defined, LVGL will not try to include
``lv_conf.h``. Instead you can pass the config defines using build
options. For example ``"-DLV_COLOR_DEPTH=32 -DLV_USE_BUTTON=1"``. The unset
options will get a default value which is the same as the content of
``lv_conf_template.h``.

LVGL also can be used via ``Kconfig`` and ``menuconfig``. You can use
``lv_conf.h`` together with Kconfig too, but keep in mind that the value
from ``lv_conf.h`` or build settings (``-D...``) overwrite the values
set in Kconfig. To ignore the configs from ``lv_conf.h`` simply remove
its content, or define :c:macro:`LV_CONF_SKIP`.

To enable multi-instance feature, set :c:macro:`LV_GLOBAL_CUSTOM` in
``lv_conf.h`` and provide a custom function to
:cpp:func:`lv_global_default` using ``__thread`` or ``pthread_key_t``.
It will allow running multiple LVGL instances by storing the global variables
in TLS (Thread Local Storage).

For example:

.. code:: c

   lv_global_t * lv_global_default(void)
   {
     static __thread lv_global_t lv_global;
     return &lv_global;
   }


Initialization
--------------

To use the graphics library you have to initialize it and setup required
components. The order of the initialization is:

1. Call :cpp:func:`lv_init`.
2. Initialize your drivers.
3. Register the display and input devices drivers in LVGL. Learn more
   about `Display </porting/display>`__ and `Input
   device </porting/indev>`__ registration.
4. Call :cpp:expr:`lv_tick_inc(x)` every ``x`` milliseconds in an interrupt to
   report the elapsed time to LVGL. `Learn more </porting/tick>`__.
5. Call :cpp:func:`lv_timer_handler` every few milliseconds to handle LVGL
   related tasks. `Learn more </porting/timer-handler>`__.

.. _setting-up-your-project:

=======================
Setting Up Your Project
=======================


Get LVGL
--------

LVGL is available on GitHub: https://github.com/lvgl/lvgl.

You can clone it or
`Download <https://github.com/lvgl/lvgl/archive/refs/heads/master.zip>`__
the latest version of the library from GitHub.


.. add_lvgl_to_your_project

Add LVGL to your project
------------------------

1.  The graphics library itself is the ``lvgl`` directory. It contains several
    directories but to use LVGL you only need the ``.c`` and ``.h`` files under
    the ``src`` directory, plus ``lvgl/lvgl.h``, and ``lvgl/lv_version.h``.  You can
    add ``lvgl/examples/`` and ``lvgl/demos/`` if you need examples and/or demos.

2.  Copy ``lvgl/lv_conf_template.h`` to ``lv_conf.h`` next to the ``lvgl`` folder.
    Change the first ``#if 0`` to ``1`` to enable the file's content and set the
    :c:macro:`LV_COLOR_DEPTH` define to align with the color depth used by your
    display panel.  See comments in ``lv_conf.h`` for details.

3.  Initialize LVGL once early during system execution by doing the following in
    this order:

    - Call :cpp:func:`lv_init`.
    - Initialize your drivers.
    - Create display(s), register buffer-flush callback, and set draw buffers.
      See :ref:`display` for how to do this.
    - Create and configure any input devices.
      See :ref:`indev` for how to do this.
    - Optionally set a theme with :cpp:func:`lv_display_set_theme`.

4.  Supply LVGL with time awareness.  See :ref:`tick` for two ways to do this.

5.  Ensure :cpp:func:`lv_timer_handler` gets called every few milliseconds to manage
    LVGL timers.  See :ref:`timer_handler` for different ways to do this.

6.  #include "lvgl/lvgl.h" in source files wherever you need to use LVGL functions.


Make and CMake
~~~~~~~~~~~~~~

LVGL also supports ``make`` and ``CMake`` build systems out of the box.
To add LVGL to your Makefile based build system add these lines to your
main Makefile:

.. code-block:: make

    LVGL_DIR_NAME ?= lvgl     #The name of the lvgl folder (change this if you have renamed it)
    LVGL_DIR ?= ${shell pwd}  #The path where the lvgl folder is
    include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

For integration with CMake take a look this section of the
:ref:`Documentation <build_cmake>`.


Other platforms and tools
~~~~~~~~~~~~~~~~~~~~~~~~~

The :ref:`Integration <integration_index>` section contains many platform-specific
descriptions e.g. for ESP32, Arduino, NXP, RT-Thread, NuttX, etc.


Demos and Examples
~~~~~~~~~~~~~~~~~~

The ``lvgl`` directory also contains an ``examples`` and a ``demos``
directory.  If your project needs examples and/or demos, add the these
directories to your project.  If ``make`` or ``CMake`` handle the
examples and demos directories, no extra action is required.


Configuration file
------------------

There is a configuration header file for LVGL called **lv_conf.h**. You
can modify this header to set the library's behavior, disable unused
modules and features, adjust the size of buffers, etc.

To get ``lv_conf.h``, follow the steps in #2 above.
The layout of the files should look like this:

::

    lvgl/
    lv_conf.h
    other files and folders in your project

The comments in ``lv_conf.h`` explain the meaning of each setting.  Be sure
to at least set :c:macro:`LV_COLOR_DEPTH` according to your display's color
depth.  Note that the examples and demos explicitly need to be enabled
in ``lv_conf.h`` if you need them.

Alternatively, ``lv_conf.h`` can be copied to another place but then you
should add the :c:macro:`LV_CONF_INCLUDE_SIMPLE` define to your compiler
options (e.g. ``-DLV_CONF_INCLUDE_SIMPLE`` for GCC compiler) and set the
include path manually (e.g. ``-I../include/gui``). In this case LVGL
will attempt to include ``lv_conf.h`` simply with ``#include "lv_conf.h"``.

You can even use a different name for ``lv_conf.h``. The custom path can
be set via the :c:macro:`LV_CONF_PATH` define. For example
``-DLV_CONF_PATH="/home/joe/my_project/my_custom_conf.h"``. If this define
is set :c:macro:`LV_CONF_SKIP` is assumed to be ``0``.

If :c:macro:`LV_CONF_SKIP` is defined, LVGL will not try to include
``lv_conf.h``. Instead you can pass the config defines using build
options. For example ``"-DLV_COLOR_DEPTH=32 -DLV_USE_BUTTON=1"``.  Unset
options will get a default value which is the same as the content of
``lv_conf_template.h``.

LVGL also can be used via ``Kconfig`` and ``menuconfig``. You can use
``lv_conf.h`` together with Kconfig as well, but keep in mind that the values
from ``lv_conf.h`` or build settings (``-D...``) override the values
set in Kconfig. To ignore the configs from ``lv_conf.h`` simply remove
its content, or define :c:macro:`LV_CONF_SKIP`.

To enable multi-instance feature, set :c:macro:`LV_GLOBAL_CUSTOM` in
``lv_conf.h`` and provide a custom function to
:cpp:func:`lv_global_default` using ``__thread`` or ``pthread_key_t``.
It will allow running multiple LVGL instances by storing LVGL's global variables
in TLS (Thread-Local Storage).

For example:

.. code-block:: c

    lv_global_t * lv_global_default(void)
    {
        static __thread lv_global_t lv_global;
        return &lv_global;
    }


